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

#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

#include "../include/util.h"


int min(int x, int y)
{
    return (x < y) ? x : y;
}


int minmin(int x, int y, int z)
{
    return min(min(x, y), z);
}


int file_copy_to(FILE* infile, FILE* outfile, u_int32_t pos_to)
{
    char c;

    while (ftell(infile) < pos_to)
    {
        /* read & copy char by char */
        if (fread(&c, 1, 1, infile) != 0)
        {
            fputc(c, outfile);
        }
    }

    return 0;
}


void file_copy(FILE* infile, FILE* outfile)
{
    char c = 0;
    while (fread(&c, 1, 1, infile))
    {
        fputc(c, outfile);
    }
}


int file_load(const char* filename, char** buffer)
{
    /* open read */
    FILE *f = fopen(filename, "r");
    if (f == NULL)
    {
        *buffer = NULL;
        return -1;
    }

    /* get file size */
    struct stat st;
    stat(filename, &st);
    int size = st.st_size;

    /* reset seek */
    fseek(f, 0, SEEK_SET);

    /* allocate buffer */
    *buffer = (char*) calloc(size + 1, sizeof(char));

    /* copy file contents into buffer */
    if (!*buffer || fread(*buffer, sizeof(char), size, f) != size)
    {
        free(*buffer);
        return -2;
    }

    /* close file */
    fclose(f);

    /* null-terminate buffer */
    (*buffer)[size] = 0;

    return size;
}


u_int32_t bytes_to_uint32(const char* buf)
{
    return buf[0] + (buf[1] << 8) + (buf[2] << 16) + (buf[3] << 24);
}
