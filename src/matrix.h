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

#ifndef _MATRIX_H_
#define _MATRIX_H_

#include <inttypes.h>
#include <stdio.h>

// @ts-ignore
#include <fixmath/fix16.h>

#include "vectors.h"

// Maximum size of matrices.
#ifndef FIXMATRIX_MAX_SIZE
#define FIXMATRIX_MAX_SIZE 8
#endif

typedef struct {
    uint8_t rows;
    uint8_t columns;
    
    /* Error flags are used to detect exceptions in the computations.
     * The flags are automatically propagated to the result if either
     * of the operands is invalid.
     * Currently the following flags are defined:
     * - FIXMATRIX_OVERFLOW: A value has exceeded 32767 and wrapped around
     * - FIXMATRIX_DIMERR: Operands have incompatible dimensions
     * - FIXMATRIX_USEERR: Function was called in unsupported way
     */
    uint8_t errors;
    
    /* Data is stored in memory in row-major format, e.g.
     * entry at (row, column) is data[row][column]
     */
    fix16_t data[FIXMATRIX_MAX_SIZE][FIXMATRIX_MAX_SIZE];
} mf16;

#define FIXMATRIX_OVERFLOW 0x01
#define FIXMATRIX_DIMERR   0x02
#define FIXMATRIX_USEERR   0x04
#define FIXMATRIX_SINGULAR 0x08
#define FIXMATRIX_NEGATIVE 0x10

// Operations between two matrices
void mf16_mul(mf16 *dest, const mf16 *a, const mf16 *b);

// Prints a matrix row to stdout
static inline void MPRINT_ROW(mf16 m, uint8_t row)
{
    char buf[100];
    uint8_t cont;
    for (cont = 0;cont<m.columns;cont++)
    {
        fix16_to_str(m.data[row][cont],buf,2);
        printf("%s ",buf);
    }
    printf("\n");
}

// Prints a whole matrix to stdout
static inline void MPRINT_MATRIX(mf16 m)
{
    uint8_t cont;
    for (cont = 0;cont<m.rows;cont++)
    {
        MPRINT_ROW(m,cont);
    }
}

// Conversion helpers
uint8_t mf16_to_v2d(v2d* dest,const mf16* a);
uint8_t mf16_to_v3d(v3d* dest,const mf16* a);

void v3d_to_mf16(mf16 *dest,const v3d *bv);
void mf16_mul_v3d(mf16 *dest, const mf16 *a, const v3d *bv);
/*uint8_t mf16_mul_v3d_to_v3d(v3d *dest, const mf16 *a, const v3d *bv);
uint8_t mf16_mul_v3d_to_v2d(v2d *dest, const mf16 *a, const v3d *bv);*/

inline uint8_t mf16_mul_v3d_to_v3d(v3d *dest, const mf16 *a, const v3d *bv)
{
    mf16 app;
    mf16_mul_v3d(&app,a,bv);
    if (app.errors) return -1;
    return mf16_to_v3d(dest,&app);
}

// Dot mult matrix with 3dvector and return 2dvector
inline uint8_t mf16_mul_v3d_to_v2d(v2d *dest, const mf16 *a, const v3d *bv)
{
    mf16 app;
    mf16_mul_v3d(&app,a,bv);
    if (app.errors) return -1;
    return mf16_to_v2d(dest,&app);
}

// Calculates the dotproduct of two vectors of size n.
// If overflow happens, returns fix16_overflow.
fix16_t fa16_dot(const fix16_t *a, uint_fast8_t a_stride,
                 const fix16_t *b, uint_fast8_t b_stride,
                 uint_fast8_t n);
#endif
