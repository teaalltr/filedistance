/* This file is part of filedistance
*  Copyright (C) 2020  Marco Savelli
*
*  This program is free software: you can redistribute it and/or modify
*  it under the terms of the GNU General Public License as published by
*  the Free Software Foundation, either version 3 of the License, or
*  (at your option) any later version.
*
*  This program is distributed in the hope that it will be useful,
*  but WITHOUT ANY WARRANTY; without even the implied warranty of
*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU General Public License
*  along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <stdlib.h> // malloc, free
#include <string.h>
#include <stdbool.h>

#include "../include/script.h"
#include "../include/util.h"
#include "../include/endianness.h"

#define BUFSIZE 256


void print_edit(const edit* e, FILE* outfile)
{
    switch (e->operation)
    {
        case ADD:
        {
            const char op[] = "ADD";
            unsigned int n = htonl(e->pos);
            char b = e->c;
            fwrite(op, sizeof(char), 3, outfile);
            fwrite(&n, sizeof(unsigned int), 1, outfile);
            fwrite(&b, 1, 1, outfile);
            break;
        }
        case DEL:
        {
            const char op[] = "DEL";
            unsigned int n = htonl(e->pos);
            fwrite(op, sizeof(char), 3, outfile);
            fwrite(&n, sizeof(unsigned int), 1, outfile);
            break;
        }
        case SET:
        {
            const char op[] = "SET";
            unsigned int n = htonl(e->pos);
            char b = e->c;
            fwrite(op, sizeof(char), 3, outfile);
            fwrite(&n, sizeof(unsigned int), 1, outfile);
            fwrite(&b, 1, 1, outfile);
            break;
        }
        default:
            return;
    }
}


unsigned int levenshtein_fill_matrix(edit** mat, const char* str1, size_t len1, const char *str2, size_t len2)
{
    for (int j = 1; j <= len2; j++)
    {
        for (int i = 1; i <= len1; i++)
        {
            unsigned int substitution_cost;
            unsigned int del = 0, ins = 0, subst = 0;
            unsigned int best;

            if (str1[i - 1] == str2[j - 1])
            {
                substitution_cost = 0;
            }
            else
            {
                substitution_cost = 1;
            }

            del = mat[i - 1][j].score + 1;                       /* deletion */
            ins = mat[i][j - 1].score + 1;                       /* insertion */
            subst = mat[i - 1][j - 1].score + substitution_cost; /* substitution */

            best = minmin(del, ins, subst);

            mat[i][j].score = best;
            mat[i][j].c = str2[j - 1];
            mat[i][j].pos = i - 1;

            if (best == del)
            {
                mat[i][j].operation = DEL;
                mat[i][j].prev = &mat[i - 1][j];
            }
            else if (best == ins)
            {
                mat[i][j].operation = ADD;
                mat[i][j].prev = &mat[i][j - 1];
            }
            else
            {
                if (substitution_cost > 0)
                {
                    mat[i][j].operation = SET;
                }
                else
                {
                    mat[i][j].operation = NONE;
                }
                mat[i][j].prev = &mat[i - 1][j - 1];
            }
        }
    }

    return mat[len1][len2].score;
}

edit** levenshtein_matrix_create(size_t len1, size_t len2)
{
    edit** mat = malloc((len1 + 1) * sizeof(edit*));
    if (mat == NULL)
    {
        return NULL;
    }
    for (int i = 0; i <= len1; i++)
    {
        mat[i] = malloc((len2 + 1) * sizeof(edit));
        if (mat[i] == NULL)
        {
            for (int j = 0; j < i; j++)
            {
                free(mat[j]);
            }

            free(mat);
            return NULL;
        }
    }

    for (int i = 0; i <= len1; i++)
    {
        mat[i][0].score = i;
        mat[i][0].prev = NULL;
        mat[i][0].c = 0;
    }

    for (int j = 0; j <= len2; j++)
    {
        mat[0][j].score = j;
        mat[0][j].prev = NULL;
        mat[0][j].c = 0;
    }

    return mat;
}

int levenshtein_distance_script(const char* str1, size_t l1, const char* str2, size_t l2, edit** script)
{
    unsigned int dist;

    edit** mat = NULL;


    /* degenerate cases */
    if (l1 == 0)
    {
        return l2;
    }
    if (l2 == 0)
    {
        return l1;
    }

    /* create matrix */
    mat = levenshtein_matrix_create(l1, l2);

    if (mat == NULL)
    {
        *script = NULL;
        return 0;
    }

    /* Main algorithm */
    dist = levenshtein_fill_matrix(mat, str1, l1, str2, l2);

    /* Read back the edit script */
    *script = malloc(dist * sizeof(edit));

    if (!(*script))
    {
        dist = 0;
    }
    else
    {
        unsigned int i = dist - 1;
        edit* head = NULL;
        for (head = &mat[l1][l2]; head->prev != NULL; head = head->prev)
        {
            if (head->operation == NONE)
                continue;

            memcpy(*script + i, head, sizeof(edit));
            i--;
        }
    }


    /* Clean up */
    for (int i = 0; i <= l1; i++)
    {
        free(mat[i]);
    }

    free(mat);

    return dist;
}


void append_script_file(FILE* file, edit* script, size_t len)
{
    for (int i = 0; i < len; i++)
    {
        print_edit(&script[i], file);
    }
}

int levenshtein_file_distance_script(const char* file1, const char* file2, const char* outfile)
{
    edit* script = NULL;
    char buffer1[BUFSIZE + 1];
    char buffer2[BUFSIZE + 1];

    buffer1[BUFSIZE] = '\0';
    buffer2[BUFSIZE] = '\0';

    FILE* f1 = fopen(file1, "r" );
    FILE* f2 = fopen(file2, "r" );
    FILE* out = fopen(outfile, "w");

    int tot = 0;

    while (true)
    {
        size_t read1 = fread(buffer1, 1, BUFSIZE, f1);
        size_t read2 = fread(buffer2, 1, BUFSIZE, f2);

        buffer1[read1] = '\0';
        buffer2[read2] = '\0';

        int distance = levenshtein_distance_script(buffer1, strlen(buffer1), buffer2, strlen(buffer2), &script);

        append_script_file(out, script, distance);
        tot += distance;

        if (feof(f1) && feof(f2))
            break;
    }

    printf("Distance: %d\n", tot);

    printf("Edit script saved successfully: %s\n", outfile);

    fclose(f1);
    fclose(f2);
    fclose(out);

    return 0;

}

