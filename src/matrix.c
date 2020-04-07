/*
Copyright (C) 2020-2021 Alessio Garzi <gun101@email.it>
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the
License, or (at your option) any later version.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.
You should have received a copy of the GNU General Public
License along with this program; if not, write to the
Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.
*/

#include "matrix.h"

/*********************************
 * Operations between 2 matrices *
 *********************************/

void mf16_mul(mf16 *dest, const mf16 *a, const mf16 *b)
{
    int row, column;
    
    // If dest and input matrices alias, we have to use a temp matrix.
    //mf16 tmp;
    //fa16_unalias(dest, (void**)&a, (void**)&b, &tmp, sizeof(tmp));
    
    dest->errors = a->errors | b->errors;
    
    if (a->columns != b->rows)
        dest->errors |= FIXMATRIX_DIMERR;
    
    dest->rows = a->rows;
    dest->columns = b->columns;
    
    for (row = 0; row < dest->rows; row++)
    {
        for (column = 0; column < dest->columns; column++)
        {
            dest->data[row][column] = fa16_dot(
                &a->data[row][0], 1,
                &b->data[0][column], FIXMATRIX_MAX_SIZE,
                a->columns);
            
            if (dest->data[row][column] == fix16_overflow)
                dest->errors |= FIXMATRIX_OVERFLOW;
        }
    }
}

// Convert a 1 col X 2 rows matrix to v2d
uint8_t mf16_to_v2d(v2d* dest,const mf16* a)
{
    if (a->columns!=1 || a->rows!=2) return -1;

    dest->x = a->data[0][0];
    dest->y = a->data[1][0];
    return 0;
}

// Convert a 1 col X 3 rows matrix to v3d
uint8_t mf16_to_v3d(v3d* dest,const mf16* a)
{
    if (a->columns!=1 || a->rows!=3) return -1;

    dest->x = a->data[0][0];
    dest->y = a->data[1][0];
    dest->z = a->data[2][0];
    return 0;
}

// Convert a v3d to a 1 col X 3 rows matrix
void v3d_to_mf16(mf16 *dest,const v3d *bv)
{
    if (dest->columns!=1 || dest->rows!=3)
    {
        dest->errors|=FIXMATRIX_DIMERR;
        return ;
    }

    dest->data[0][0]=bv->x;
    dest->data[1][0]=bv->y;
    dest->data[2][0]=bv->z;
}

// Dot mult matrix with 3dvector and return 3dvector
/*uint8_t mf16_mul_v3d_to_v3d(v3d *dest, const mf16 *a, const v3d *bv)
{
    mf16 app;
    mf16_mul_v3d(&app,a,bv);
    if (app.errors) return -1;
    return mf16_to_v3d(dest,&app);
}

// Dot mult matrix with 3dvector and return 2dvector
uint8_t mf16_mul_v3d_to_v2d(v2d *dest, const mf16 *a, const v3d *bv)
{
    mf16 app;
    mf16_mul_v3d(&app,a,bv);
    if (app.errors) return -1;
    return mf16_to_v2d(dest,&app);
}*/

// Dot mult matrix with 3dvector
void mf16_mul_v3d(mf16 *dest, const mf16 *a, const v3d *bv)
{
    mf16 b = {
        3,  // Rows
        1,  // Columns
        0,  // Errors

        // Matrix actual data
      
        {
            { 0 }, // Row 1
            { 0 }, // Row 2
            { 0 }  // Row3
        }
    };
    v3d_to_mf16(&b,bv);
    mf16_mul(dest,a, &b);
}

// Calculates the dotproduct of two vectors of size n.
// If overflow happens, sets flag in errors
fix16_t fa16_dot(const fix16_t *a, uint_fast8_t a_stride,
                 const fix16_t *b, uint_fast8_t b_stride,
                 uint_fast8_t n)
{
    fix16_t sum = 0;
    
    while (n--)
    {
        // Compute result
        if (*a != 0 && *b != 0)
        {
            fix16_t product = fix16_mul(*a, *b);
            sum = fix16_add(sum, product);
            
            if (sum == fix16_overflow || product == fix16_overflow)
                return fix16_overflow;
        }
        
        // Go to next item
        a += a_stride;
        b += b_stride;
    }
    
    return sum;
}