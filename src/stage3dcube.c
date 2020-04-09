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
#include <ace/managers/blit.h>

// Points of the cube
#define POINT_ARRAY_LENGTH 8
#define CUBE_SIDE_LENGTH 25
#define CUBE_COLOR 0x0F00

#define RESET_SCROLL 320*(NUM_IMAGES-1)
//static UWORD g_uwBlitModd = RESET_SCROLL/8;

/*void blitClear2(
    //const unsigned char* pData,
    const UBYTE ubSlot,const UBYTE ubMaxBitplanes);*/


void DrawCubeVertex(int,v2d,UBYTE);
void saveSpritePalette();
void restoreSpritePalette();
void changeSpritePalette();
//void waitblit();
void blitLine2(
        tBitMap *pDst, WORD x1, WORD y1, WORD x2, WORD y2,
        UBYTE ubColor, UWORD uwPattern
);
void DrawlineOr(UBYTE*,int,int,int,int);
void InitLine();
//void blitClear(tSimpleBufferManager * buffer,UBYTE nBitplane);

tVPort *s_pVpScore;

static int fastCpu=0;
static v2d projected2dvector;

static UWORD uwOldColor[16];

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

void stage3dCubeInput()
{
    if (keyCheck(KEY_D))
    {
        g_ubVBallXOffset++;
        if (g_ubVBallXOffset>280) g_ubVBallXOffset=280;
    }
    if (keyCheck(KEY_A))
    {
        g_ubVBallXOffset--;
        if (g_ubVBallXOffset<50) g_ubVBallXOffset=50;
    }
    
    if (keyCheck(KEY_W))
    {
        g_ubVBallYOffset--;
        if (g_ubVBallYOffset<50) g_ubVBallYOffset=50;
    }
    if (keyCheck(KEY_S))
    {
        g_ubVBallYOffset++;
        if (g_ubVBallYOffset>180) g_ubVBallYOffset=180;
    }
    return ;
}

void stage3dCubePre()
{
    saveSpritePalette();

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
    
    changeSpritePalette();

    if (SysBase->AttnFlags & AFF_68010)  fastCpu=1;
}

void stage3dCube()
{

    static int frameNum = 0 ;
    //printCursorPixel5(s_pMainBuffer,100+(UWORD)s_pCameraMain->uPos.uwX,100,40*NUM_IMAGES);

    /*InitLine();
    UWORD uwPattern3 = 0xffff;
    UWORD uwPattern4 = 0xf0f0;
    g_pCustom->bltbdat = uwPattern3;
    DrawlineOr((UBYTE*)((ULONG)s_pMainBuffer->pBack->Planes[4]),100+(UWORD)s_pCameraMain->uPos.uwX,100,100+(UWORD)s_pCameraMain->uPos.uwX,50);*/
    
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
            
        //v2d projected2dvector;
        //mf16_to_v2d(&projected2dvector,&projected2d);
        //mf16 projection
        projected2dvector.x = fix16_mul(rotatedZ.x,projection.data[0][0]);
        projected2dvector.y = fix16_mul(rotatedZ.y,projection.data[1][1]);

        DrawCubeVertex(i,projected2dvector,1);
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
    for (int i=0;i<8;i++) DrawCubeVertex(i,projected2dvector,0); 
    
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

    restoreSpritePalette();

}
static struct MemPoint point_empty={0,0,0};

inline static struct MemPoint getlastelem(struct Queue* queue) 
{ 
	if (isEmpty(queue)) 
		return point_empty; 
	/*struct MemPoint item = queue->array[queue->front]; 
	
	return item; */
    return  queue->array[queue->front];
} 

void DrawCubeVertex(int i,v2d projected2dvector,UBYTE DrawLineFlag)
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
        default: return;
    }
    
    //if (p_QueuePointer==NULL) return ;

    if (isFull(p_QueuePointer))
    {
        elem = dequeue(p_QueuePointer);
        //restorePixels(s_pMainBuffer,elem,40*NUM_IMAGES);
    }

    elem.x = (UWORD) fix16_to_int(projected2dvector.x)+(UWORD)g_ubVBallXOffset+(UWORD)s_pCameraMain->uPos.uwX;
    elem.y = (UWORD) fix16_to_int(projected2dvector.y)+(UWORD)g_ubVBallYOffset;
    /*if (i==6 || i == 7) elem.changed = printCursorPixel(s_pMainBuffer,elem.x,elem.y,40*NUM_IMAGES);
    else*/ elem.changed = 0;
    static struct MemPoint oldelem;
    static struct MemPoint elemToDel1,elemToDel2;
    static struct MemPoint elemToDel3,elemToDel4;
    static struct MemPoint elemToDel5,elemToDel6;
    static struct MemPoint elemToDel7,elemToDel8;
    static struct MemPoint elemToDel9,elemToDel10;
    static struct MemPoint elemToDel11,elemToDel12;
    static struct MemPoint elemToDel13,elemToDel14;
    static struct MemPoint elemToDel15,elemToDel16;
    static struct MemPoint elemToDel17,elemToDel18;
    static struct MemPoint elemToDel19,elemToDel20;
    static struct MemPoint elemToDel21,elemToDel22;
    static struct MemPoint elemToDel23,elemToDel24;
    if (i==0) 
    {
        oldelem=elem;

        blitLine2(s_pMainBuffer->pBack,elemToDel23.x ,elemToDel23.y,elemToDel24.x,elemToDel24.y,16, 0x0000);
        if (DrawLineFlag) blitLine2(s_pMainBuffer->pBack,getlastelem(tPointCubeQueue7).x ,getlastelem(tPointCubeQueue7).y,getlastelem(tPointCubeQueue8).x,getlastelem(tPointCubeQueue8).y,16, 0xffff);
        elemToDel23=getlastelem(tPointCubeQueue7);
        elemToDel24=getlastelem(tPointCubeQueue8);
    }
    if (i==1)
    {
        blitLine2(s_pMainBuffer->pBack,elemToDel1.x ,elemToDel1.y,elemToDel2.x,elemToDel2.y,16, 0x0000);
        if (DrawLineFlag)  blitLine2(s_pMainBuffer->pBack,elem.x ,elem.y,oldelem.x,oldelem.y,16, 0xffff);

        /*InitLine();
        UWORD uwPattern3 = 0xffff;
        UWORD uwPattern4 = 0xf0f0;
        g_pCustom->bltbdat = uwPattern3;
        DrawlineOr((UBYTE*)((ULONG)s_pMainBuffer->pBack->Planes[4]),elem.x,elem.y,oldelem.x,oldelem.y);*/

        elemToDel1=elem;
        elemToDel2=oldelem;
    }
    if (i==2)
    {
        blitLine2(s_pMainBuffer->pBack,elemToDel3.x ,elemToDel3.y,elemToDel4.x,elemToDel4.y,16, 0x0000);
        if (DrawLineFlag)  blitLine2(s_pMainBuffer->pBack,elem.x ,elem.y,getlastelem(tPointCubeQueue2).x,getlastelem(tPointCubeQueue2).y,16, 0xffff);
        elemToDel3=elem;
        elemToDel4=getlastelem(tPointCubeQueue2);
    }
    if (i==3)
    {
        blitLine2(s_pMainBuffer->pBack,elemToDel5.x ,elemToDel5.y,elemToDel6.x,elemToDel6.y,16, 0x0000);
        if (DrawLineFlag)  blitLine2(s_pMainBuffer->pBack,elem.x ,elem.y,getlastelem(tPointCubeQueue3).x,getlastelem(tPointCubeQueue3).y,16, 0xffff);
        elemToDel5=elem;
        elemToDel6=getlastelem(tPointCubeQueue3);

        blitLine2(s_pMainBuffer->pBack,elemToDel19.x ,elemToDel19.y,elemToDel20.x,elemToDel20.y,16, 0x0000);
        if (DrawLineFlag)  blitLine2(s_pMainBuffer->pBack,elem.x ,elem.y,getlastelem(tPointCubeQueue).x,getlastelem(tPointCubeQueue).y,16, 0xffff);
        elemToDel19=elem;
        elemToDel20=getlastelem(tPointCubeQueue);
    }
    if (i==4)
    {
        blitLine2(s_pMainBuffer->pBack,elemToDel7.x ,elemToDel7.y,elemToDel8.x,elemToDel8.y,16, 0x0000);
        if (DrawLineFlag)  blitLine2(s_pMainBuffer->pBack,elem.x ,elem.y,getlastelem(tPointCubeQueue).x,getlastelem(tPointCubeQueue).y,16, 0xffff);
        elemToDel7=elem;
        elemToDel8=getlastelem(tPointCubeQueue);
    }
    if (i==5)
    {
        blitLine2(s_pMainBuffer->pBack,elemToDel9.x ,elemToDel9.y,elemToDel10.x,elemToDel10.y,16, 0x0000);
        if (DrawLineFlag)  blitLine2(s_pMainBuffer->pBack,elem.x ,elem.y,getlastelem(tPointCubeQueue5).x,getlastelem(tPointCubeQueue5).y,16, 0xffff);
        elemToDel9=elem;
        elemToDel10=getlastelem(tPointCubeQueue5);

        blitLine2(s_pMainBuffer->pBack,elemToDel17.x ,elemToDel17.y,elemToDel18.x,elemToDel18.y,16, 0x0000);
        if (DrawLineFlag)  blitLine2(s_pMainBuffer->pBack,elem.x ,elem.y,getlastelem(tPointCubeQueue2).x,getlastelem(tPointCubeQueue2).y,16, 0xffff);
        elemToDel17=elem;
        elemToDel18=getlastelem(tPointCubeQueue2);
    }
    if (i==6)
    {
        blitLine2(s_pMainBuffer->pBack,elemToDel15.x ,elemToDel15.y,elemToDel16.x,elemToDel16.y,16, 0x0000);
        if (DrawLineFlag)  blitLine2(s_pMainBuffer->pBack,elem.x ,elem.y,getlastelem(tPointCubeQueue3).x,getlastelem(tPointCubeQueue3).y,16, 0xffff);
        elemToDel15=elem;
        elemToDel16=getlastelem(tPointCubeQueue3);

        blitLine2(s_pMainBuffer->pBack,elemToDel21.x ,elemToDel21.y,elemToDel22.x,elemToDel22.y,16, 0x0000);
        if (DrawLineFlag)  blitLine2(s_pMainBuffer->pBack,elem.x ,elem.y,getlastelem(tPointCubeQueue6).x,getlastelem(tPointCubeQueue6).y,16, 0xffff);
        elemToDel21=elem;
        elemToDel22=getlastelem(tPointCubeQueue6);
    }

    if (i==7)
    {
        blitLine2(s_pMainBuffer->pBack,elemToDel11.x ,elemToDel11.y,elemToDel12.x,elemToDel12.y,16, 0x0000);
        if (DrawLineFlag)  blitLine2(s_pMainBuffer->pBack,elem.x ,elem.y,getlastelem(tPointCubeQueue5).x,getlastelem(tPointCubeQueue5).y,16, 0xffff);
        elemToDel11=elem;
        elemToDel12=getlastelem(tPointCubeQueue5);

        blitLine2(s_pMainBuffer->pBack,elemToDel13.x ,elemToDel13.y,elemToDel14.x,elemToDel14.y,16, 0x0000);
        if (DrawLineFlag)  blitLine2(s_pMainBuffer->pBack,elem.x ,elem.y,getlastelem(tPointCubeQueue4).x,getlastelem(tPointCubeQueue4).y,16, 0xffff);
        elemToDel13=elem;
        elemToDel14=getlastelem(tPointCubeQueue4);
    }
    enqueue(p_QueuePointer, elem);
}

void changeSpritePalette()
{
    // Sprite 0 colors (cursor)
    g_pCustom->color[16] = CUBE_COLOR;
    g_pCustom->color[17] = CUBE_COLOR;
    g_pCustom->color[18] = CUBE_COLOR;
    g_pCustom->color[19] = CUBE_COLOR;

    // Sprite 1 colors (cursor)
    g_pCustom->color[20] = CUBE_COLOR;
    g_pCustom->color[21] = CUBE_COLOR;
    g_pCustom->color[22] = CUBE_COLOR;

    // Sprite 2 colors (cursor)
    g_pCustom->color[23] = CUBE_COLOR;
    g_pCustom->color[24] = CUBE_COLOR;
    g_pCustom->color[25] = CUBE_COLOR;

    // Sprite 3 colors (cursor)
    g_pCustom->color[26] = CUBE_COLOR;
    g_pCustom->color[27] = CUBE_COLOR;
    g_pCustom->color[28] = CUBE_COLOR;

        // Sprite 4 colors (cursor)
    g_pCustom->color[29] = CUBE_COLOR;
    g_pCustom->color[30] = CUBE_COLOR;
    g_pCustom->color[31] = CUBE_COLOR;
}

void saveSpritePalette()
{
    for (int i=16;i<32;i++)
        uwOldColor[i-16] = s_pVpScore->pPalette[i];
}

void restoreSpritePalette()
{
    for (int i=16;i<32;i++)
        g_pCustom->color[i] = uwOldColor[i-16] ;
}

#define BLIT_LINE_OR ((ABC | ABNC | NABC | NANBC) | (SRCA | SRCC | DEST))
#define BLIT_LINE_XOR ((ABNC | NABC | NANBC) | (SRCA | SRCC | DEST))
#define BLIT_LINE_ERASE ((NABC | NANBC | ANBC) | (SRCA | SRCC | DEST))

void blitLine2(
        tBitMap *pDst, WORD x1, WORD y1, WORD x2, WORD y2,
        UBYTE ubColor, UWORD uwPattern
) {
        // Based on Cahir's function from:
        // https://github.com/cahirwpz/demoscene/blob/master/a500/base/libsys/blt-line.c

        UWORD uwBltCon1 = LINEMODE;
     
     

        // Always draw the line downwards.
        if (y1 > y2) {
                SWAP(x1, x2);
                SWAP(y1, y2);
        }

        // Word containing the first pixel of the line.
        WORD wDx = x2 - x1;
        WORD wDy = y2 - y1;

        // Setup octant bits
        if (wDx < 0) {
                wDx = -wDx;
                if (wDx >= wDy) {
                        uwBltCon1 |= AUL | SUD;
                }
                else {
                        uwBltCon1 |= SUL;
                        SWAP(wDx, wDy);
                }
        }
        else {
                if (wDx >= wDy) {
                        uwBltCon1 |= SUD;
                }
                else {
                        SWAP(wDx, wDy);
                }
        }

        WORD wDerr = wDy + wDy - wDx;
        if (wDerr < 0) {
                uwBltCon1 |= SIGNFLAG;
        }

        UWORD uwBltSize = (wDx << 6) + 66;
        UWORD uwBltCon0 = ror16(x1&15, 4);
        ULONG ulDataOffs = pDst->BytesPerRow * y1 + ((x1 >> 3) & ~1);
        if (fastCpu) blitWait();
        g_pCustom->bltafwm = -1;
        g_pCustom->bltalwm = -1;
        g_pCustom->bltadat = 0x8000;     
g_pCustom->bltbdat = uwPattern;
        g_pCustom->bltamod = wDerr - wDx;
        g_pCustom->bltbmod = wDy + wDy;  
        g_pCustom->bltcmod = pDst->BytesPerRow;
        g_pCustom->bltdmod = pDst->BytesPerRow;
        g_pCustom->bltcon1 = uwBltCon1;
        g_pCustom->bltapt = (APTR)(LONG)wDerr;
        //for(UBYTE ubPlane = 4; ubPlane != pDst->Depth; ++ubPlane) {
            static UBYTE ubPlane = 4;
                UBYTE *pData = pDst->Planes[ubPlane] + ulDataOffs; 
                UWORD uwOp = ((ubColor & BV(ubPlane)) ? BLIT_LINE_OR : BLIT_LINE_ERASE);

                if (fastCpu) blitWait();
                //waitblit();
                g_pCustom->bltcon0 = uwBltCon0 | uwOp;
                g_pCustom->bltcpt = pData;
                g_pCustom->bltdpt = (APTR)(pData);
                g_pCustom->bltsize = uwBltSize;
        //}

}

/*void blitClear(tSimpleBufferManager * buffer,UBYTE nBitplane)
{
  //memset( (UBYTE*)((ULONG)buffer->pFront->Planes[nBitplane]),0,40*256);
  waitblit();
  g_pCustom->bltcon0 = 0x0100;
  g_pCustom->bltcon1 = 0x0000;
  g_pCustom->bltafwm = 0xFFFF;
  g_pCustom->bltalwm = 0xFFFF;
  g_pCustom->bltamod = 0x0000;
  g_pCustom->bltbmod = 0x0000;
  g_pCustom->bltcmod = 0x0000;
  g_pCustom->bltdmod = 0x0140;
  g_pCustom->bltdpt = (UBYTE*)((ULONG)buffer->pBack->Planes[nBitplane]);
  g_pCustom->bltsize = 0x3814;

  return;
}

void blitClear2(
    //const unsigned char* pData,
    const UBYTE ubSlot,const UBYTE ubMaxBitplanes)
{
    UWORD offset =(UWORD) ((UWORD)s_pCameraMain->uPos.uwX+g_ubVBallXOffset)/8;
  const UWORD dmod = g_uwBlitModd;
  UBYTE ubBitplaneCounter;
  for (ubBitplaneCounter=4;ubBitplaneCounter<s_pMainBuffer->pBack->Depth;ubBitplaneCounter++)
  {
    waitblit();
    g_pCustom->bltcon0 = 0x0100;
    g_pCustom->bltcon1 = 0x0000;
    g_pCustom->bltafwm = 0xFFFF;
    g_pCustom->bltalwm = 0xFFFF;
    g_pCustom->bltamod = 0x0000;
    g_pCustom->bltbmod = 0x0000;
    g_pCustom->bltcmod = 0x0000;
    g_pCustom->bltdmod = dmod;
    //g_pCustom->bltapt = (UBYTE*)((ULONG)&pData[40*224*ubBitplaneCounter]);
    g_pCustom->bltdpt = (UBYTE*)((ULONG)s_pMainBuffer->pBack->Planes[ubBitplaneCounter]+offset);
    //g_pCustom->bltsize = 0x3814;

    g_pCustom->bltsize = 0x3814;
    return ;
    if (ubMaxBitplanes>0 && ubBitplaneCounter+1>=ubMaxBitplanes) return ;
  }
  return ;
}*/