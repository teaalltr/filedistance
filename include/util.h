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

#ifndef FILEDISTANCE_UTIL_H
#define FILEDISTANCE_UTIL_H

#include <stdio.h>
#include "script.h"

int load_file(char* filename, char** buffer);

int min(int x, int y);

int max(int x, int y);

int minmin(int x, int y, int z);

void swap_array_edit(edit* a, edit* b, size_t n);

void swap_array_int(int* a, int* b, size_t n);

int filecpy(FILE * sourceFile, FILE * destFile);


#endif // FILEDISTANCE_UTIL_H
