/* This file is part of FileDistance
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
#include <stdint.h>

#include "../include/script.h"
#include "../include/util.h"


int manhattanDistance(int x1, int y1, int x2, int y2)
{
    return abs(x1 - x2) + abs(y1 - y2);
}

// per la derivazione vedere la documentazione
_Bool isOutCell(i, j, n, m)
{
    return 2 * min(manhattanDistance(i, j, 0, n),manhattanDistance(i, j, m, 0)) <= n;
}

// conta il numero di celle valide per una matrice m x n
int count_valid_cells(int m, int n)
{
    int count = 0;
    for (int i = 0; i <= m; i++)
    {
        for (int j = 0; j <= n; j++)
        {
            if (isOutCell(i, j, n, m))
            {
                count++;
            }
        }
    }
    return count;
}

void clear_edit_array(edit *array, int size)
{
    memset((void*) array, 0, (size)*sizeof(edit));
}

void init_initial(edit *array, int len)
{
    for (int i = 0; i <= len; i++)
    {
        array[i].score = i;
        array[i].prev = NULL;
        array[i].arg1 = 0;
        array[i].arg2 = 0;
    }
}


int left_cell_idx(int i, int j, int n, int m, int idx) // todo
{
    // controllo se la cella vicina è valida o è fuori dalla matrice
    if (j-1 < 0)
    {
        return -1;
    }
    if (isOutCell(i, j-1, n, m))
    {
        return -1;
    }

    return idx - 1;
}

int up_left_cell_idx(int i, int j, int n, int m, int idx) // todo
{
    // controllo se la cella sopra a sinistra è valida o è fuori dalla matrice
    if (j-1 < 0 || i-1 < 0)
    {
        return -1;
    }
    if (isOutCell(i-1, j-1, n, m))
    {
        return -1;
    }

    return idx - 1;
}

int up_cell_idx(int i, int j, int n, int m, int idx) // todo
{
    // controllo se la cella sopra è valida o è fuori dalla matrice
    if (j-1 < 0)
    {
        return -1;
    }
    if (isOutCell(i, j-1, n, m))
    {
        return -1;
    }

    // controllo la cella sopra a destra
    if (j+1 > m)
    {

    }

    if (isOutCell(i-1, j+1, n, m))
    {

    }
    int cnt = 0;
    int currI = i-1;
    int currJ = j;

    do
    {
        cnt++;
        if (isOutCell(currI, currJ, n, m))
        {

        }
        currI++;
        currJ++;
    } while (currI == i && currJ == j);

}

// todo rivedere n, m
int levenshtein_create_script(edit** script, const char* str1, size_t m, const char* str2, size_t n)
{
    if (m < n)
    {
        return levenshtein_create_script(script, str2, n, str1, m);
    }

    //FILE* tmp = tmpfile();
    //if (tmp == NULL)
    //{
    //    perror("Unable to create temp file");
    //    return -1;
    //}


    int idx = 0;

    // calcola il numero di celle valide
    int numValidCells = count_valid_cells(m, n);

    // alloca l'array script_r (lungo al più numValidCells + 1)
    // verrà letto a ritroso fino al primo elemento non nullo
    int numElemsScript_r = numValidCells + 1;
    edit* scriptR = (edit*) malloc((numValidCells + 1)*sizeof(edit));

    // azzera lo script
    clear_edit_array(scriptR, numElemsScript_r);

    /*
      crea i due array riga sopra e colonna a sinistra
      verranno riempiti nel ciclo in base all'elemento corrente
     */

    // all'inizio rowAbove è vuoto (non c'è nessuna riga sopra)
    edit* rowAbove = (edit*) malloc((n+1)*sizeof(edit));
    //clear_edit_array(rowAbove, m);
    init_initial(rowAbove, n);

    // alloco la riga di lavoro; verra swappata con la riga sopra

    edit* current = (edit*) malloc((n+1)*sizeof(edit));
    //clear_edit_array(current, m);

    edit left = {.score = 0,
                 .arg1 = 0,
                 .arg2 = 0,
                 .prev = NULL};

    unsigned int substitution_cost = 0;
    unsigned int del = 0, ins = 0, subst = 0;
    unsigned int best = 0;

    for (int i = 1; i <= m; i++)
    {
        for (int j = 1; j <= n; j++)
        {
            substitution_cost = str1[i - 1] == str2[j - 1] ? 0 : 1;

            del   = rowAbove[j].score + 1;                       /* deletion */
            ins   = left.score + 1;                              /* insertion */
            subst = rowAbove[j - 1].score + substitution_cost;   /* substitution */

            best = minmin(del, ins, subst);

            current[j].score = best;
            current[j].arg1 = str1[i - 1];
            current[j].arg2 = str2[j - 1];
            current[j].pos = i - 1;

            if (!isOutCell(i, j, n, m) && idx+1 <= numValidCells)
            {
                scriptR[idx].score = best;
                scriptR[idx].arg1  = str1[i - 1];
                scriptR[idx].arg2  = str2[j - 1];
                scriptR[idx].pos   = i - 1;
                if (best == del)
                {
                    scriptR[idx].type = DEL;
                    scriptR[idx].prev = &scriptR[0];//&scriptR[left_cell_idx(i, j, n, m, idx)];

                }
                else if (best == ins)
                {
                    scriptR[idx].type = INS;
                    scriptR[idx].prev =  &scriptR[0];//&scriptR[up_cell_idx(i, j, n, m, idx)];
                }
                else
                {
                    if (substitution_cost > 0)
                    {
                        scriptR[idx].type = SET;
                    }
                    else
                    {
                        scriptR[idx].type = NONE;
                    }

                    scriptR[idx].prev = &scriptR[0];//&scriptR[up_left_cell_idx(i, j, n, m, idx)];
                }
                idx++;
            }

            left = current[j];

        }

        swap_array_edit(rowAbove, current, m);
        clear_edit_array(current, n);

    }

    free(rowAbove);
    free(current);

    return best;
}


int levenshtein_distance(char* str1, char* str2, edit** script)
{
    int len1 = strlen(str1), len2 = strlen(str2);
    int i = 0;
    int distance = 0;
    edit** mat;

    edit* head;

    /* If either string is empty, the distance is the other string's length */
    if (len1 == 0)
    {
        return len2;
    }

    if (len2 == 0)
    {
        return len1;
    }

    /* Initialise the matrix */
    //mat = levenshtein_matrix_create(len1, len2);
//
    //if (!mat) {
    //    *script = NULL;
    //    return 0;
    //}
//
    //initFlagsMatrix(len1, len2);
//
    //* Main algorithm */
    edit* ss = NULL;
    distance = levenshtein_create_script(ss, str1, len1, str2, len2);

    /* Read back the edit script */
    *script = malloc(distance * sizeof(edit));
    if (! *script )
    {
        distance = 0;
    }
    else
    {
            i = distance - 1;
            for (head = &mat[len1][len2]; head->prev != NULL; head = head->prev)
                {
                    if (head->type == NONE)
                            continue;

                            memcpy(*script + i, head, sizeof(edit));
                    i--;
                }
        }

            // dealloca gli elementi
            for (i = 0; i <= len1; i++)
        {
            free(mat[i]);
    }// dealloca la matricefree(mat);

    return distance;
}


void print_edit(edit* e, FILE* outfile)
{
    if (outfile == NULL )
    {
        return; // error?
    }

    if (e == NULL)
    {
        return; // error?
    }

    uint32_t n = 0;
    char b = 0;

    switch (e->type)
    {
        case INS:
        {
            // ADDnb - add byte n in position b
            fprintf(outfile, "ADD%u%c", e->pos, e->arg2);
            //printf("Insert %c", e->arg2);
            break;
        }
        case DEL:
        {
            // DELnb - delete byte in position b
            fprintf(outfile, "DEL%u", e->pos);
            //printf("Delete %c", e->arg1);
            break;
        }
        case SET:
        {
            // SETnb - set byte n in position b
            fprintf(outfile, "SET%u%c", e->pos, e->arg1);
            //printf("Substitute %c for %c", e->arg2, e->arg1);
            break;
        }
        default:
            return;
    }
}

void save_file_script(edit** script, size_t len, char* outfile)
{
    FILE* out = fopen(outfile, "w+");
    if (!out)
    {
        perror("Can't open outfile\n");
        return;
    }

    for (int i = 0; i < len; i++)
    {
        print_edit(script[i], out);
    }

    fclose(out);
}

int file_distance_script(char* file1, char* file2, char* outfile)
{
    char* buff1;
    char* buff2;

    int size1 = 0;
    int size2 = 0;

    size1 = load_file(file1, &buff1);
    size2 = load_file(file2, &buff2);

    if (size1 < 0 || size2 < 0 || buff1 == NULL || buff2 == NULL)
    {
        // todo improve
        return -1;
    }

    edit** script = NULL;
    int len = levenshtein_create_script(script, buff1, size1, buff2, size2);

    save_file_script(script, len, outfile);

    free(buff1);
    free(buff2);

    return 0;

}

