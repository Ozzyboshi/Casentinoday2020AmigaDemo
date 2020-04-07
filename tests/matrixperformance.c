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

#include <stdio.h>
#include <clib/timer_protos.h>
#include <clib/exec_protos.h>
#include "../src/customtrigonometry.h"

int MULTIPLICATIONS = 0;

fix16_t fix16_mul_2(fix16_t inArg0, fix16_t inArg1)
{
	uint32_t _a = (inArg0 >= 0) ? inArg0 : (-inArg0);
	uint32_t _b = (inArg1 >= 0) ? inArg1 : (-inArg1);
	
	uint8_t va[4] = {_a, (_a >> 8), (_a >> 16), (_a >> 24)};
	uint8_t vb[4] = {_b, (_b >> 8), (_b >> 16), (_b >> 24)};
	
	uint32_t low = 0;
	uint32_t mid = 0;
	
	// Result column i depends on va[0..i] and vb[i..0]

	#ifndef FIXMATH_NO_OVERFLOW
	// i = 6
	if (va[3] && vb[3]) return fix16_overflow;
	#endif
	
	// i = 5
	if (va[2] && vb[3]) mid += (uint16_t)va[2] * vb[3];
	if (va[3] && vb[2]) mid += (uint16_t)va[3] * vb[2];
	mid <<= 8;
	
	// i = 4
	if (va[1] && vb[3]) mid += (uint16_t)va[1] * vb[3];
	if (va[2] && vb[2]) mid += (uint16_t)va[2] * vb[2];
	if (va[3] && vb[1]) mid += (uint16_t)va[3] * vb[1];
	
	#ifndef FIXMATH_NO_OVERFLOW
	if (mid & 0xFF000000) return fix16_overflow;
	#endif
	mid <<= 8;
	
	// i = 3
	if (va[0] && vb[3]) mid += (uint16_t)va[0] * vb[3];
	if (va[1] && vb[2]) mid += (uint16_t)va[1] * vb[2];
	if (va[2] && vb[1]) mid += (uint16_t)va[2] * vb[1];
	if (va[3] && vb[0]) mid += (uint16_t)va[3] * vb[0];
	
	#ifndef FIXMATH_NO_OVERFLOW
	if (mid & 0xFF000000) return fix16_overflow;
	#endif
	mid <<= 8;
	
	// i = 2
	if (va[0] && vb[2]) mid += (uint16_t)va[0] * vb[2];
	if (va[1] && vb[1]) mid += (uint16_t)va[1] * vb[1];
	if (va[2] && vb[0]) mid += (uint16_t)va[2] * vb[0];		
	
	// i = 1
	if (va[0] && vb[1]) low += (uint16_t)va[0] * vb[1];
	if (va[1] && vb[0]) low += (uint16_t)va[1] * vb[0];
	low <<= 8;
	
	// i = 0
	if (va[0] && vb[0]) low += (uint16_t)va[0] * vb[0];
	
	#ifndef FIXMATH_NO_ROUNDING
	low += 0x8000;
	#endif
	mid += (low >> 16);
	
	#ifndef FIXMATH_NO_OVERFLOW
	if (mid & 0x80000000)
		return fix16_overflow;
	#endif
	
	fix16_t result = mid;
	
	/* Figure out the sign of result */
	if ((inArg0 >= 0) != (inArg1 >= 0))
	{
		result = -result;
	}
	
	return result;
}

/* 32-bit implementation of fix16_mul. Potentially fast on 16-bit processors,
 * and this is a relatively good compromise for compilers that do not support
 * uint64_t. Uses 16*16->32bit multiplications.
 */
fix16_t fix16_mul_1(fix16_t inArg0, fix16_t inArg1)
{
	// Each argument is divided to 16-bit parts.
	//					AB
	//			*	 CD
	// -----------
	//					BD	16 * 16 -> 32 bit products
	//				 CB
	//				 AD
	//				AC
	//			 |----| 64 bit product
	int32_t A = (inArg0 >> 16), C = (inArg1 >> 16);
	uint32_t B = (inArg0 & 0xFFFF), D = (inArg1 & 0xFFFF);
	
	int32_t AC = A*C;
	int32_t AD_CB = A*D + C*B;
	uint32_t BD = B*D;
	
	int32_t product_hi = AC + (AD_CB >> 16);
	
	// Handle carry from lower 32 bits to upper part of result.
	uint32_t ad_cb_temp = AD_CB << 16;
	uint32_t product_lo = BD + ad_cb_temp;
	if (product_lo < BD)
		product_hi++;
	

	
	return (product_hi << 16) | (product_lo >> 16);

}

mf16 rotationZ_test = 
        {
            3,  // Rows
            3,  // Columns
            0,  // Errors

            // Matrix actual data
            {
                { 1, 1,1 }, // Row 1
                { 1,1, 1 }, // Row 2
                { 1,1,1 }  // Row 3
            }
        };

#define POINT_ARRAY_LENGTH 8

struct Device* TimerBase;
static struct IORequest timereq;


static ULONG timer()
{
  static struct timeval t;
  struct timeval a, b;

  GetSysTime(&a);
  b = a;
  SubTime(&b, &t);
  t = a;

  return b.tv_secs*1000 + b.tv_micro/1000;
}

static ULONG timer2()
{
  static struct timeval t;
  struct timeval a, b;

  GetSysTime(&a);
  b = a;
  SubTime(&b, &t);
  t = a;

  return b.tv_secs*1000 + b.tv_micro/1000;
}

// Calculates the dotproduct of two vectors of size n.
// If overflow happens, sets flag in errors
fix16_t fa16_dot_test(const fix16_t *a, uint_fast8_t a_stride,
                 const fix16_t *b, uint_fast8_t b_stride,
                 uint_fast8_t n)
{
   /* static fix16_t alol;
    static fix16_t blol;
    static fix16_t product = 0;*/
    fix16_t sum = 0;
    
    while (n--)
    {
        /* alol = *a;
         blol = *b;*/
        // Compute result
        if (*a && *b)
        {
            MULTIPLICATIONS++;
            fix16_t product = fix16_mul(*a, *b);
            //fix16_t product = fix16_from_int(19);
            sum = fix16_add(sum, product);
            
            /*if (sum == fix16_overflow || product == fix16_overflow)
                return fix16_overflow;*/
        }
        
        // Go to next item
        a += a_stride;
        b += b_stride;
    }
    
    return sum;
}

static inline void mf16_mul_test(mf16 *dest, const mf16 *a, const mf16 *b)
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
            dest->data[row][column] = fa16_dot_test(
                &a->data[row][0], 1,
                &b->data[0][column], FIXMATRIX_MAX_SIZE,
                a->columns);
            
            if (dest->data[row][column] == fix16_overflow)
                dest->errors |= FIXMATRIX_OVERFLOW;
        }
    }
}

// Convert a 1 col X 3 rows matrix to v3d
static inline uint8_t mf16_to_v3d_test(v3d* dest,const mf16* a)
{
    if (a->columns!=1 || a->rows!=3) return -1;

    dest->x = a->data[0][0];
    dest->y = a->data[1][0];
    dest->z = a->data[2][0];
    return 0;
}

// Convert a v3d to a 1 col X 3 rows matrix
static inline void v3d_to_mf16_test(mf16 *dest,const v3d *bv)
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

// Dot mult matrix with 3dvector
static inline void mf16_mul_v3d_test(mf16 *dest, const mf16 *a, const v3d *bv)
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
    v3d_to_mf16_test(&b,bv);
    mf16_mul_test(dest,a, &b);
}

// Dot mult matrix with 3dvector and return 3dvector
static inline uint8_t mf16_mul_v3d_to_v3d_test(v3d *dest, const mf16 *a, const v3d *bv)
{
    mf16 app;
    mf16_mul_v3d_test(&app,a,bv);
    if (app.errors) return -1;
    return mf16_to_v3d_test(dest,&app);
}





// Dot mult matrix with 3dvector and return 2dvector
static inline uint8_t mf16_mul_v3d_to_v2d_test(v2d *dest, const mf16 *a, const v3d *bv)
{
    mf16 app;
    mf16_mul_v3d_test(&app,a,bv);
    if (app.errors) return -1;
    return 0;
    //return mf16_to_v2d(dest,&app);
}

/*********************************
 * Operations between 2 matrices *
 *********************************/



int main()
{
    // Points of the cube
    v3d points[POINT_ARRAY_LENGTH] = {
      {fix16_from_int(-50), fix16_from_int(-50), fix16_from_int(-50)},
      {fix16_from_int(50), fix16_from_int(-50), fix16_from_int(-50)},
      {fix16_from_int(50), fix16_from_int(50), fix16_from_int(-50)},
      {fix16_from_int(-50), fix16_from_int(50), fix16_from_int(-50)},

      {fix16_from_int(-50), fix16_from_int(-50), fix16_from_int(50)},
      {fix16_from_int(50), fix16_from_int(-50), fix16_from_int(50)},
      {fix16_from_int(50), fix16_from_int(50), fix16_from_int(50)},
      {fix16_from_int(-50), fix16_from_int(50), fix16_from_int(50)},
    };

    // projection matrix
    mf16 projection = 
    {
        2,  // Rows
        3,  // Columns
        0,  // Errors

        // Matrix actual data
        {
            { fix16_from_int(1), fix16_from_int(0), fix16_from_int(0) }, // Row 1
            { fix16_from_int(0), fix16_from_int(1), fix16_from_int(0) } // Row 2
        }
    };

    OpenDevice((CONST_STRPTR)"timer.device", 0, &timereq, 0);
    TimerBase = timereq.io_Device;

    timer();

    fix16_t rotationAngle = 0;
    int iRotationAngle = 45;

//    for (fix16_t rotationAngle =0; fix16_to_int(rotationAngle)<=360; rotationAngle = fix16_add(rotationAngle,fix16_from_int(10)))
    {
        // Radians conversion
        fix16_t rotationAngleRad = fix16_deg2rad(rotationAngle);

        // Z rotation matrix - rotation along the Z axis - X and Y changes, not Z
       /* mf16 rotationZ = 
        {
            3,  // Rows
            3,  // Columns
            0,  // Errors

            // Matrix actual data
            {
                { fix16_cos(rotationAngleRad), fix16_mul(fix16_sin(rotationAngleRad),fix16_from_int(-1)),   fix16_from_int(0) }, // Row 1
                { fix16_sin(rotationAngleRad), fix16_cos(rotationAngleRad),                                 fix16_from_int(0) }, // Row 2
                { fix16_from_int(0),           fix16_from_int(0),                                           fix16_from_int(1) }  // Row 3
            }
        };*/

        // Z rotation matrix - rotation along the Z axis - X and Y changes, not Z
        mf16 rotationZ = 
        {
            3,  // Rows
            3,  // Columns
            0,  // Errors

            // Matrix actual data
            {
                { fix16_coslist[iRotationAngle],fix16_sinlist_inv[iRotationAngle],   0}, // Row 1
                {  fix16_sinlist[iRotationAngle],  fix16_coslist[iRotationAngle],                                 0 }, // Row 2
                { 0,           0,                                           fix16_from_int(1) }  // Row 3
            }
        };

        /*ULONG t = timer();
        printf("\nTime to build transformation matrix : %u ms\n", (unsigned int)t);*/

        for (int i=0;i<POINT_ARRAY_LENGTH;i++)
        {
             // Init my point
             v3d point = points[i];

            // Rotate along Y axis
            v3d rotated;
            fix16_t oldZ = point.z;
            point.z=0;
            if (mf16_mul_v3d_to_v3d(&rotated, &rotationZ, &point))
            {
                printf("Error on v3d mult (1)\n");
                return 1;
            }
            rotated.z=oldZ;

            // Project the rotated vector
            v2d projected2dvector;
            if (mf16_mul_v3d_to_v2d(&projected2dvector, &projection, &rotated))
            {
                printf("Error on v3d mult (2)\n");
                return 1;
            }
        }
    }

    ULONG t = timer();
    printf("\n%u ms\n", (unsigned int)t);
    printf ("MULTIPLICATIONS %d\n",MULTIPLICATIONS);

    // text mult
    fix16_t a,b,c;
    a = fix16_div(fix16_from_int(4),fix16_from_int(100));
    b = fix16_div(fix16_from_int(33),fix16_from_int(77));
    timer2();
    for (int i=0;i<100;i++)
    {
        c = fix16_mul_1(a,fix16_from_int(i));
        
    }

    ULONG t2 = timer2();
    printf("\n%u ms\n", (unsigned int)t2);

printf("%d",c);
    CloseDevice(&timereq);
    return 0;
}

/*v3d rotateAroundZ(v3d vec3, int deg) {
    v3d vec3prime;
  vec3prime.x = vec3.x * fix16_sintlist[deg] + vec3.y * fix16_costlist[deg];
  vec3prime.y = vec3.x * fix16_costlist[deg] - vec3.y * sin(deg);
  vec3prime.z = vec3.z;
  return vec3prime;
}*/


