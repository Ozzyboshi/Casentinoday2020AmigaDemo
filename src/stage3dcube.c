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

#include "stages.h"
#include "matrix.h"
#include <exec/execbase.h>
#include <proto/exec.h>
#include <ace/managers/key.h>

// Points of the cube
#define POINT_ARRAY_LENGTH 8
#define CUBE_SIDE_LENGTH 25

void DrawCubeVertex(int,v2d);

UBYTE g_ubVBallAnimEnabled;

static struct Queue* tPointCubeQueue;
static struct Queue* tPointCubeQueue2;
static struct Queue* tPointCubeQueue3;
static struct Queue* tPointCubeQueue4;
static struct Queue* tPointCubeQueue5;
static struct Queue* tPointCubeQueue6;
static struct Queue* tPointCubeQueue7;
static struct Queue* tPointCubeQueue8;

static UWORD g_ubVBallXOffset;
static UWORD g_ubVBallYOffset;

fix16_t fix16_sinlist[360];
fix16_t fix16_sinlist_inv[360];
fix16_t fix16_coslist[360];

static int g_iRotationAngle;
static fix16_t g_tOne ;

static v3d points[POINT_ARRAY_LENGTH];
static mf16 projection ;

// projection matrix
/*mf16 projection = 
{
    2,  // Rows
    3,  // Columns
    0,  // Errors

    // Matrix actual data
    {
        { fix16_from_int(1), fix16_from_int(0), fix16_from_int(0) }, // Row 1
        { fix16_from_int(0), fix16_from_int(1), fix16_from_int(0) } // Row 2
    }
};*/

void stage3dCubeInput()
{
    if (keyCheck(KEY_D))
    {
        g_ubVBallXOffset++;
    }
    if (keyCheck(KEY_A))
    {
        g_ubVBallXOffset--;
    }
    if (keyCheck(KEY_W))
    {
        g_ubVBallYOffset--;
    }
    if (keyCheck(KEY_S))
    {
        g_ubVBallYOffset++;
    }
    return ;
}

void stage3dCubePre()
{

    points[0].x = fix16_from_int(-CUBE_SIDE_LENGTH);
    points[0].y = fix16_from_int(-CUBE_SIDE_LENGTH);
    points[0].z = fix16_from_int(-CUBE_SIDE_LENGTH);

    points[1].x = fix16_from_int(CUBE_SIDE_LENGTH);
    points[1].y = fix16_from_int(-CUBE_SIDE_LENGTH);
    points[1].z = fix16_from_int(-CUBE_SIDE_LENGTH);

    points[2].x = fix16_from_int(CUBE_SIDE_LENGTH);
    points[2].y = fix16_from_int(CUBE_SIDE_LENGTH);
    points[2].z = fix16_from_int(-CUBE_SIDE_LENGTH);

    points[3].x = fix16_from_int(-CUBE_SIDE_LENGTH);
    points[3].y = fix16_from_int(CUBE_SIDE_LENGTH);
    points[3].z = fix16_from_int(-CUBE_SIDE_LENGTH);

    points[4].x = fix16_from_int(-CUBE_SIDE_LENGTH);
    points[4].y = fix16_from_int(-CUBE_SIDE_LENGTH);
    points[4].z = fix16_from_int(CUBE_SIDE_LENGTH);

    points[5].x = fix16_from_int(CUBE_SIDE_LENGTH);
    points[5].y = fix16_from_int(-CUBE_SIDE_LENGTH);
    points[5].z = fix16_from_int(CUBE_SIDE_LENGTH);

    points[6].x = fix16_from_int(CUBE_SIDE_LENGTH);
    points[6].y = fix16_from_int(CUBE_SIDE_LENGTH);
    points[6].z = fix16_from_int(CUBE_SIDE_LENGTH);

    points[7].x = fix16_from_int(-CUBE_SIDE_LENGTH);
    points[7].y = fix16_from_int(CUBE_SIDE_LENGTH);
    points[7].z = fix16_from_int(CUBE_SIDE_LENGTH);

    projection.rows = 2;
    projection.columns = 3;
    projection.errors = 0;

    g_iRotationAngle = 0 ;

    g_ubVBallAnimEnabled=0;

    g_tOne = fix16_from_int(1);

    tPointCubeQueue=createQueue(2);
    tPointCubeQueue2=createQueue(2);
    tPointCubeQueue3=createQueue(2);
    tPointCubeQueue4=createQueue(2);

    tPointCubeQueue5=createQueue(2);
    tPointCubeQueue6=createQueue(2);
    tPointCubeQueue7=createQueue(2);
    tPointCubeQueue8=createQueue(2);

    g_ubVBallXOffset=60;
    g_ubVBallYOffset=60;
}

void stage3dCube()
{
    static int frameNum = 0 ;
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

    if (g_iRotationAngle>=360) g_iRotationAngle = 0;
    
    // Radians conversion
    //fix16_t rotationAngleRad = fix16_deg2rad(g_tRotationAngle);

    // X rotation matrix - rotation along the X axis - Z and Y changes, not X
    mf16 rotationX = 
    {
        3,  // Rows
        3,  // Columns
        0,  // Errors

        // Matrix actual data
        {
            {g_tOne,       0,                 0,                                          },  // Row 1
            { 0,       fix16_coslist[g_iRotationAngle],       fix16_sinlist_inv[g_iRotationAngle]   },  // Row 2
            { 0,       fix16_sinlist[g_iRotationAngle],       fix16_coslist[g_iRotationAngle]                                 }   // Row 3
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
            { fix16_coslist[g_iRotationAngle], 0, fix16_sinlist_inv[g_iRotationAngle],                }, // Row 1
            { 0,           g_tOne, 0                                                         }, // Row 2
            { fix16_sinlist[g_iRotationAngle], 0, fix16_coslist[g_iRotationAngle]                                               }  // Row 3
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
            { fix16_coslist[g_iRotationAngle], fix16_sinlist_inv[g_iRotationAngle],  0 }, // Row 1
            { fix16_sinlist[g_iRotationAngle], fix16_coslist[g_iRotationAngle],                                0 }, // Row 2
            { 0,           0,                                           g_tOne }  // Row 3
         }
      };


#ifdef COLORDEBUG
    g_pCustom->color[0] = 0x0BBB;
#endif
    int fastCpu=0;
    if (SysBase->AttnFlags & AFF_68010)  fastCpu=1;
    
    int start =0;
    int end = POINT_ARRAY_LENGTH-6;

    if (fastCpu)
    {
        start = 0;
        end = POINT_ARRAY_LENGTH;
    }

    else
    {
        if (frameNum==1)
        {
            start=2;
            end = POINT_ARRAY_LENGTH-4;
        }

        else if (frameNum==2)
        {
            start=4;
            end  =POINT_ARRAY_LENGTH-2;
        }

        else if (frameNum==3)
        {
            start = 6;
            end = POINT_ARRAY_LENGTH;
        }
    }

    for (int i=start;i<end;i++)
    {
        // Init my point
        v3d point = points[i];

        // Rotate along X axis
        v3d rotatedX,rotatedY,rotatedZ;
        /*if (mf16_mul_v3d_to_v3d(&rotatedX, &rotationX, &points[i])) return ;

        if (mf16_mul_v3d_to_v3d(&rotatedY, &rotationY, &rotatedX)) return ;

        if (mf16_mul_v3d_to_v3d(&rotatedZ, &rotationZ, &rotatedY)) return ;*/
        /*fix16_t oldZ = point.z;
        point.z = 0;*/
        if (mf16_mul_v3d_to_v3d(&rotatedX, &rotationX, &point)) return ;
        if (mf16_mul_v3d_to_v3d(&rotatedY, &rotationY, &rotatedX)) return ;
        if (mf16_mul_v3d_to_v3d(&rotatedZ, &rotationZ, &rotatedY)) return ;
        //rotatedZ.z = oldZ;

        // Project the rotated vector
       /* if (0)
        {
        mf16 projected2d;
        mf16_mul_v3d(&projected2d, &projection, &rotatedX);
        if (projected2d.errors) return ;
        }*/
            
        v2d projected2dvector;
        //mf16_to_v2d(&projected2dvector,&projected2d);
        //mf16 projection
        projected2dvector.x = fix16_mul(rotatedZ.x,projection.data[0][0]);
        projected2dvector.y = fix16_mul(rotatedZ.y,projection.data[1][1]);

        DrawCubeVertex(i,projected2dvector);
    }
#ifdef COLORDEBUG
    g_pCustom->color[0] = 0x0F00;
#endif

    //g_tRotationAngle = fix16_add(g_tRotationAngle,fix16_from_int(1));
    
    frameNum++;
    if (frameNum>=4)
    { 
        frameNum=0;
        g_iRotationAngle++;
    }
}

// Clean our mess
void stage3dCubePost()
{
    struct MemPoint elem;
    while (!isEmpty(tPointCubeQueue))
    {
        elem = dequeue(tPointCubeQueue);
        restorePixels(s_pMainBuffer,elem,40*NUM_IMAGES);
    }
    while (!isEmpty(tPointCubeQueue2))
    {
        elem = dequeue(tPointCubeQueue2);
        restorePixels(s_pMainBuffer,elem,40*NUM_IMAGES);
    }
    while (!isEmpty(tPointCubeQueue3))
    {
        elem = dequeue(tPointCubeQueue3);
        restorePixels(s_pMainBuffer,elem,40*NUM_IMAGES);
    }
    while (!isEmpty(tPointCubeQueue4))
    {
        elem = dequeue(tPointCubeQueue4);
        restorePixels(s_pMainBuffer,elem,40*NUM_IMAGES);
    }
    while (!isEmpty(tPointCubeQueue5))
    {
        elem = dequeue(tPointCubeQueue5);
        restorePixels(s_pMainBuffer,elem,40*NUM_IMAGES);
    }
    while (!isEmpty(tPointCubeQueue6))
    {
        elem = dequeue(tPointCubeQueue6);
        restorePixels(s_pMainBuffer,elem,40*NUM_IMAGES);
    }
    while (!isEmpty(tPointCubeQueue7))
    {
        elem = dequeue(tPointCubeQueue7);
        restorePixels(s_pMainBuffer,elem,40*NUM_IMAGES);
    }
    while (!isEmpty(tPointCubeQueue8))
    {
        elem = dequeue(tPointCubeQueue8);
        restorePixels(s_pMainBuffer,elem,40*NUM_IMAGES);
    }

    g_ubVBallAnimEnabled=1;
}

void DrawCubeVertex(int i,v2d projected2dvector)
{
    struct MemPoint elem;
    struct Queue* p_QueuePointer=NULL;

    switch (i)
    {
        case 0: p_QueuePointer=tPointCubeQueue; break;
        case 1: p_QueuePointer=tPointCubeQueue2; break;
        case 2: p_QueuePointer=tPointCubeQueue3; break;
        case 3: p_QueuePointer=tPointCubeQueue4; break;
        case 4: p_QueuePointer=tPointCubeQueue5; break;
        case 5: p_QueuePointer=tPointCubeQueue6; break;
        case 6: p_QueuePointer=tPointCubeQueue7; break;
        case 7: p_QueuePointer=tPointCubeQueue8; break;
    }
    
    if (p_QueuePointer==NULL) return ;

    if (isFull(p_QueuePointer))
    {
        elem = dequeue(p_QueuePointer);
        restorePixels(s_pMainBuffer,elem,40*NUM_IMAGES);
    }

    elem.x = (UWORD) fix16_to_int(projected2dvector.x)+(UWORD)g_ubVBallXOffset+(UWORD)s_pCameraMain->uPos.uwX;
    elem.y = (UWORD) fix16_to_int(projected2dvector.y)+(UWORD)g_ubVBallYOffset;
    elem.changed = printCursorPixel(s_pMainBuffer,elem.x,elem.y,40*NUM_IMAGES);
    enqueue(p_QueuePointer, elem);
}