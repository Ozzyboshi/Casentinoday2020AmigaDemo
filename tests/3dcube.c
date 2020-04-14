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
#include "../src/customtrigonometry.h"


static fix16_t g_tOne;
static v2d projected2dvectors[4][360];
/*fix16_t fix16_sinlist[360];
fix16_t fix16_sinlist_inv[360];
fix16_t fix16_coslist[360];*/


#define POINT_ARRAY_LENGTH 4

void print_v2d_to_stdout(int,v2d);
void cache_points_build(v3d* ,int );

int main()
{
   char buf[100];

   g_tOne = fix16_from_int(1);

   // Init custom trig functions
   fix16_sinlist_init();
   fix16_coslist_init();

   // Points of the cube
   v3d points[POINT_ARRAY_LENGTH] = {
      {fix16_from_int(-50), fix16_from_int(-50), fix16_from_int(0)},
      {fix16_from_int(50), fix16_from_int(-50), fix16_from_int(0)},
      {fix16_from_int(50), fix16_from_int(50), fix16_from_int(0)},
      {fix16_from_int(-50), fix16_from_int(50), fix16_from_int(0)}
   };
   
   for (int i=0;i<4;i++)
    {
        v3d point = points[i];
        cache_points_build(&point,i);
    }
   printf("Point 0 Cached:\n");
   for (int point=0;point<4;point++)
      for (int angle = 0 ; angle<360 ; angle += 10)
      {
         printf("Angle %d: ",angle);
         print_v2d_to_stdout(point,projected2dvectors[0][angle]);
         getchar();
      }

   // projection matrix
   mf16 projection = {
      2,  // Rows
      3,  // Columns
      0,  // Errors

      // Matrix actual data
      {
         { fix16_from_int(1), fix16_from_int(0), fix16_from_int(0) }, // Row 1
         { fix16_from_int(0), fix16_from_int(1), fix16_from_int(0) } // Row 2
      }
   };

   for (fix16_t rotationAngle =0; fix16_to_int(rotationAngle)<=180; rotationAngle = fix16_add(rotationAngle,fix16_from_int(10)))
   {
      // Radians conversion
      fix16_t rotationAngleRad = fix16_deg2rad(rotationAngle);

      // Z rotation matrix - rotation along the Z axis - X and Y changes, not Z
      /*mf16 rotationZ = 
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

      // X rotation matrix - rotation along the X axis - Z and Y changes, not X
      mf16 rotationX = 
      {
         3,  // Rows
         3,  // Columns
         0,  // Errors

         // Matrix actual data
         {
         { fix16_from_int(1),       fix16_from_int(0),                 fix16_from_int(0),                                          },  // Row 1
         { fix16_from_int(0),       fix16_cos(rotationAngleRad),       fix16_mul(fix16_sin(rotationAngleRad),fix16_from_int(-1))   },  // Row 2
         { fix16_from_int(0),       fix16_sin(rotationAngleRad),       fix16_cos(rotationAngleRad)                                 }   // Row 3
         }
      };

      // Y rotation matrix - rotation along the Y axis - Z and X changes, not Y
      /*mf16 rotationY = 
      {
         3,  // Rows
         3,  // Columns
         0,  // Errors

         // Matrix actual data
         {
         { fix16_cos(rotationAngleRad), fix16_from_int(0), fix16_mul(fix16_sin(rotationAngleRad),fix16_from_int(-1)),                }, // Row 1
         { fix16_from_int(0),           fix16_from_int(1), fix16_from_int(0)                                                         }, // Row 2
         { fix16_sin(rotationAngleRad), fix16_from_int(0), fix16_cos(rotationAngleRad)                                               }  // Row 3
         }
      };*/

      fix16_to_str(rotationAngle,buf,2);
      printf("Calculating at angle %s\n",buf);
      for (int i=0;i<POINT_ARRAY_LENGTH;i++)
      {
         // Init my point
         v3d point = points[i];

         // Rotate along X axis
         v3d rotatedX;
         if (mf16_mul_v3d_to_v3d(&rotatedX, &rotationX, &point))
         {
            printf("Error on v3d mult (1)\n");
            return 1;
         }

         // Project the rotated vector
         mf16 projected2d;
         mf16_mul_v3d(&projected2d, &projection, &rotatedX);
         if (projected2d.errors)
         {
            printf("Error on v3d mult (2)\n");
            return 1;
         }

         v2d projected2dvector;
         mf16_to_v2d(&projected2dvector,&projected2d);

         printf("Point %d: ",i+1);
         fix16_to_str(projected2dvector.x,buf,2);
         printf("%s,",buf);
         fix16_to_str(projected2dvector.y,buf,2);
         printf("%s\n",buf);
      }
      getchar();
   }

   // Rotate along Y axis
   printf("Rotation along Y axis\n");
   for (fix16_t rotationAngle =0; fix16_to_int(rotationAngle)<=180; rotationAngle = fix16_add(rotationAngle,fix16_from_int(10)))
   {
      // Radians conversion
      fix16_t rotationAngleRad = fix16_deg2rad(rotationAngle);

      // Y rotation matrix - rotation along the Y axis - Z and X changes, not Y
      mf16 rotationY = 
      {
         3,  // Rows
         3,  // Columns
         0,  // Errors

         // Matrix actual data
         {
         { fix16_cos(rotationAngleRad), fix16_from_int(0), fix16_mul(fix16_sin(rotationAngleRad),fix16_from_int(-1)),                }, // Row 1
         { fix16_from_int(0),           fix16_from_int(1), fix16_from_int(0)                                                         }, // Row 2
         { fix16_sin(rotationAngleRad), fix16_from_int(0), fix16_cos(rotationAngleRad)                                               }  // Row 3
         }
      };

      fix16_to_str(rotationAngle,buf,2);
      printf("Calculating at angle %s\n",buf);
      for (int i=0;i<POINT_ARRAY_LENGTH;i++)
      {
         // Init my point
         v3d point = points[i];

         // Rotate along Y axis
         v3d rotated;
         if (mf16_mul_v3d_to_v3d(&rotated, &rotationY, &point))
         {
            printf("Error on v3d mult (1)\n");
            return 1;
         }

         // Project the rotated vector
         v2d projected2dvector;
         if (mf16_mul_v3d_to_v2d(&projected2dvector, &projection, &rotated))
         {
            printf("Error on v3d mult (2)\n");
            return 1;
         }
         print_v2d_to_stdout(i+1,projected2dvector);
         
      }
      getchar();
   }

   // Rotate along Z axis
   printf("Rotation along Z axis\n");
   for (fix16_t rotationAngle =0; fix16_to_int(rotationAngle)<=360; rotationAngle = fix16_add(rotationAngle,fix16_from_int(10)))
   {
      // Radians conversion
      fix16_t rotationAngleRad = fix16_deg2rad(rotationAngle);

      // Z rotation matrix - rotation along the Z axis - X and Y changes, not Z
      mf16 rotationZ = 
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
      };

      fix16_to_str(rotationAngle,buf,2);
      printf("Calculating at angle %s\n",buf);
      for (int i=0;i<POINT_ARRAY_LENGTH;i++)
      {
         // Init my point
         v3d point = points[i];

         // Rotate along Y axis
         v3d rotated;
         if (mf16_mul_v3d_to_v3d(&rotated, &rotationZ, &point))
         {
            printf("Error on v3d mult (1)\n");
            return 1;
         }

         // Project the rotated vector
         v2d projected2dvector;
         if (mf16_mul_v3d_to_v2d(&projected2dvector, &projection, &rotated))
         {
            printf("Error on v3d mult (2)\n");
            return 1;
         }
         print_v2d_to_stdout(i+1,projected2dvector);
         
      }
      getchar();
   }
}

void print_v2d_to_stdout(int i,v2d v2dvector)
{
   char buf[100];
   printf("Point %d: ",i);
   fix16_to_str(v2dvector.x,buf,2);
   printf("%s,",buf);
   fix16_to_str(v2dvector.y,buf,2);
   printf("%s\n",buf);
}

void cache_points_build(v3d* point,int iPointIndex)
{
    v3d rotatedX,rotatedY,rotatedZ;
    if (iPointIndex<0||iPointIndex>7) return ;

    mf16 projection = 
    {
        2,  // Rows
        3,  // Columns
        0,  // Errors

        // Matrix actual data
        {
            {g_tOne, 0, 0 }, // Row 1
            { 0, g_tOne, 0 } // Row 2
        }
    };


    for (int iRotationAngle = 0 ; iRotationAngle < 360 ; iRotationAngle++)
    {
        // X rotation matrix - rotation along the X axis - Z and Y changes, not X
        mf16 rotationX = 
        {
            3,  // Rows
            3,  // Columns
            0,  // Errors

            // Matrix actual data
            {
                {g_tOne,       0,                 0,                                          },  // Row 1
                { 0,       fix16_coslist[iRotationAngle],       fix16_sinlist_inv[iRotationAngle]   },  // Row 2
                { 0,       fix16_sinlist[iRotationAngle],       fix16_coslist[iRotationAngle]                                 }   // Row 3
            }
        };

        // Y rotation matrix - rotation along the Y axis - Z and X changes, not Y
        mf16 rotationY = 
        {
            3,  // Rows
            3,  // Columns
            0,  // Errors

            // Matrix actual data
            {
                { fix16_coslist[iRotationAngle], 0, fix16_sinlist_inv[iRotationAngle],                }, // Row 1
                { 0,           g_tOne, 0                                                         }, // Row 2
                { fix16_sinlist[iRotationAngle], 0, fix16_coslist[iRotationAngle]                                               }  // Row 3
            }
        };

        // Z rotation matrix - rotation along the Z axis - X and Y changes, not Z
        mf16 rotationZ = 
        {
            3,  // Rows
            3,  // Columns
            0,  // Errors

            // Matrix actual data
            {
                { fix16_coslist[iRotationAngle], fix16_sinlist_inv[iRotationAngle],  0 }, // Row 1
                { fix16_sinlist[iRotationAngle], fix16_coslist[iRotationAngle],                                0 }, // Row 2
                { 0,           0,                                           g_tOne }  // Row 3
            }
        };

        if (mf16_mul_v3d_to_v3d(&rotatedX, &rotationX, point)) return ;
        if (mf16_mul_v3d_to_v3d(&rotatedY, &rotationY, &rotatedX)) return ;
        if (mf16_mul_v3d_to_v3d(&rotatedZ, &rotationZ, &rotatedY)) return ;

        projected2dvectors[iPointIndex][iRotationAngle].x = fix16_mul(rotatedZ.x,projection.data[0][0]);
        projected2dvectors[iPointIndex][iRotationAngle].y = fix16_mul(rotatedZ.y,projection.data[1][1]);

        /*if (iRotationAngle==0 && iPointIndex==0)
        {
            char buf[100];
            fix16_to_str(projected2dvectors[0][0].x,buf,2);
            logWrite("valore punto zero a gradi zero '%s'\n",buf);
        }*/
    }
}