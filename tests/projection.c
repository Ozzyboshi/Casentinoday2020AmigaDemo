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
#include <../src/matrix.h>

int main()
{
   mf16 a = {
      2,  // Rows
      3,  // Columns
      0,  // Errors

      // Matrix actual data
      {
         {fix16_from_int(1), fix16_from_int(0), fix16_from_int(0)}, // Row 1
         {fix16_from_int(0), fix16_from_int(1), fix16_from_int(0)}, // Row 2
      }
   };

   mf16 b = {
      3,  // Rows
      1,  // Columns
      0,  // Errors

      // Matrix actual data
      {
         {fix16_from_int(100)}, // Row 1
         {fix16_from_int(75)}, // Row 2
         {fix16_from_int(50)} // Row3
      }
   };

   v3d bv = {fix16_from_int(100), fix16_from_int(75), fix16_from_int(50)};

   printf("******** Projection matrix \n");
   MPRINT_MATRIX(a);

   printf("******** Point matrix \n");
   MPRINT_MATRIX(b);

   // Dot multiplication matrix with matrix , a x b = r
   mf16 r;
   mf16_mul(&r, &a, &b);

   printf("******** Resulting matrix (matrix X matrix dot mult) \n");
   MPRINT_MATRIX(r);

   // Dot multiplication matrix with v3d , a x vb = r2
   mf16 r2;
   mf16_mul_v3d(&r2, &a, &bv);

   printf("******** Resulting matrix (matrix X v3d dot mult) \n");
   MPRINT_MATRIX(r2);

   // Resulted matrix converted to v2d (awesome we can draw on a 2d context!!!!!)
   v2d rv2d;
   mf16_to_v2d(&rv2d,&r2);

   printf("******** Resulting v2d vector \n");
   char buf[100];
   fix16_to_str(rv2d.x,buf,2);
   printf("X --> %s\n",buf);
   fix16_to_str(rv2d.y,buf,2);
   printf("Y --> %s\n",buf);
}
