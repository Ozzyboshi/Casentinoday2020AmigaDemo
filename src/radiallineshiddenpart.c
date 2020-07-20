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

#include "radiallineshiddenpart.h"
#include <ace/managers/key.h>                   // Keyboard processing
#include <ace/managers/game.h>                  // For using gameClose
#include <ace/managers/system.h>                // For systemUnuse and systemUse
#include <ace/managers/viewport/simplebuffer.h> // Simple buffer
#include <ace/managers/blit.h>
#include <ace/utils/font.h> // needed for tFont and font stuff
#include <ace/managers/joy.h>

#include "vectors.h"
#include "physics.h"
#include "../_res/radiallinespositions.h"
#include "../_res/discocrazy.h"

// Fonts
#include "../_res/fonts.h"

// ASSETS END

#define COPWIDTH 8
#define BITPLANES 2
#define TEXTYPOS 220

long mt_init(const unsigned char *);
void mt_music();
void mt_end();
int chan3played();

void DrawlineOr(UBYTE *, int, int, int, int);
void InitLine();

void blitClear(tSimpleBufferManager *, UBYTE);
static UWORD colorHSV(UBYTE, UBYTE, UBYTE);
void printCharToRight(char);
void scorri();

static tView *s_pView;    // View containing all the viewports
static tVPort *s_pVpMain; // Viewport for playfield
static tSimpleBufferManager *s_pMainBufferRadialLines;
static tFont *s_pFontUI;
static tTextBitMap *s_pGlyph;
static UBYTE s_ubBarHue = 0;
static UWORD s_uwBarY = 44;
static UWORD s_uwCopRawOffs=0;
static tMover g_tVector;
static v2d g_Gravity;
static const char g_cPhrase[]={" THANKS TO OFFENCE FOR THIS AWESOME EFFECT AND ABOVE ALL CONGRATULATIONS FOR THE RECENT WIN IN THE ONLINE REVISION 2020 AMIGA DEMO COMPETITION WITH \"CHILLOBITS\", YOU GUYS ARE THE BEST! HOWEVER IT WOULD BE VERY APPRECIATED A NEW AMIGA600 FRIENDLY RELEASE       THX TO FBY FOR THIS AWESOME MUSIC, I DIDNT GET YOUR APPROVATION TO INCLUDE YOUR MOD IN THIS DEMO, BLAME YOUR FRIEND Z3K WHO GAVE ME PERMISSION TO USE IT          YOU HAVE REACHED THE END OF THIS INVITATION DEMO, CLICK LEFT MOUSE BUTTON TO EXIT                \n"};

const unsigned char *uni54_data_shared;

unsigned char* s_pMusic;


void radialLinesGsCreate(void)
{
    ULONG ulRawSize = (simpleBufferGetRawCopperlistInstructionCount(BITPLANES) +
                 32 * 3 + // 32 bars - each consists of WAIT + 2 MOVE instruction
                 1 +      // Final WAIT
                 1        // Just to be sure
    );

    // Create a view - first arg is always zero, then it's option-value
    s_pView = viewCreate(0,
                         TAG_VIEW_GLOBAL_CLUT, 1,               // Same Color LookUp Table for all viewports
                         TAG_VIEW_COPLIST_MODE, VIEW_COPLIST_MODE_RAW,         // <-- This is important in RAW mode
                         TAG_VIEW_COPLIST_RAW_COUNT, ulRawSize, // <-- This is important in RAW mode
                         TAG_END);                              // Must always end with TAG_END or synonym: TAG_DONE

    // Now let's do the same for main playfield
    s_pVpMain = vPortCreate(0,
                            TAG_VPORT_VIEW, s_pView,
                            TAG_VPORT_BPP, BITPLANES, // color depth
                                              // We won't specify height here - viewport will take remaining space.
                            TAG_END);

    s_pMainBufferRadialLines = simpleBufferCreate(0,
                                       TAG_SIMPLEBUFFER_VPORT, s_pVpMain, // Required: parent viewport
                                       TAG_SIMPLEBUFFER_BITMAP_FLAGS, BMF_CLEAR,
                                       TAG_SIMPLEBUFFER_IS_DBLBUF, 1,
                                       TAG_SIMPLEBUFFER_COPLIST_OFFSET, 0, // <-- Important in RAW mode
                                       TAG_SIMPLEBUFFER_BOUND_WIDTH,320+0,
                                       TAG_END);

    s_pVpMain->pPalette[0] = 0x0000; // First color is also border color
    s_pVpMain->pPalette[1] = 0x0FF0; // Yellow
    s_pVpMain->pPalette[2] = 0x0BBB; 
    s_pVpMain->pPalette[3] = 0x0FFF; // WHITE

    s_uwCopRawOffs = simpleBufferGetRawCopperlistInstructionCount(BITPLANES);
    tCopBfr *pCopBfr = s_pView->pCopList->pBackBfr;
    tCopCmd *pBarCmds = &pCopBfr->pList[s_uwCopRawOffs];

    UWORD pColors[32];
    for (UBYTE i = 0; i < 16; ++i)
    {
        pColors[i] = colorHSV(s_ubBarHue, 255, i * 17);
    }
    for (UBYTE i = 16; i < 32; ++i)
    {
        pColors[i] = colorHSV(s_ubBarHue, 255, (31 - i) * 17);
    }

    for(UBYTE i = 0; i < 32; ++i) {
			copSetWait(&pBarCmds[i * 2 + 0].sWait, 0, s_uwBarY + i*COPWIDTH);
			copSetMove(&pBarCmds[i * 2 + 1].sMove, &g_pCustom->color[0], pColors[i]);
		}

    CopyMemQuick(
			s_pView->pCopList->pBackBfr->pList,
			s_pView->pCopList->pFrontBfr->pList,
			s_pView->pCopList->pBackBfr->uwAllocSize
		);

    // We don't need anything from OS anymore
    systemUnuse();

   /* s_pFontUI = fontCreateFromMem((UBYTE *)uni54_data_shared);
    if (s_pFontUI == NULL)
        return;
    s_pGlyph = fontCreateTextBitMap(320, s_pFontUI->uwHeight);
    fontFillTextBitMap(s_pFontUI, s_pGlyph, "THX to Chillobits team for this awesome effect");
    fontDrawTextBitMap(s_pMainBufferRadialLines->pFront, s_pGlyph, 150, 230, 3, FONT_CENTER | FONT_LAZY);
    fontDrawTextBitMap(s_pMainBufferRadialLines->pBack, s_pGlyph, 150, 230, 3, FONT_CENTER | FONT_LAZY);*/

    // Load the view
    viewLoad(s_pView);

    //mt_init(discocrazy_data);
    systemUse();
    s_pMusic = (unsigned char*)AllocMem(g_tDiscocrazy_data_size,MEMF_CHIP|MEMF_CLEAR);
    if (s_pMusic==NULL)
    {
        gameExit();
        return ;
    }
    for (size_t i=0;i<g_tDiscocrazy_data_size;i++)
        s_pMusic[i]=g_tDiscocrazy_data_data_fast[i];
     mt_init(s_pMusic);

    viewProcessManagers(s_pView);
    copProcessBlocks();

    systemSetDma(DMAB_SPRITE, 0);

    // Sprite reset
    UWORD* p_Sprites=(UWORD*)0xdff140;
    while (p_Sprites<=(UWORD*)0xdff17E)
    {
        *p_Sprites=0;
        p_Sprites+=2;
    }

    // Init mover 
    g_Gravity.x=fix16_div(fix16_from_int(1),fix16_from_int(50));
    g_Gravity.y=fix16_div(fix16_from_int(1),fix16_from_int(2));
    spriteVectorInit(&g_tVector,0,0,0,0,0,LITTLE_BALLS_MASS);
    g_tVector.tLocation.x = 0;
    g_tVector.tLocation.y = 0;
}

void radialLinesGsLoop(void)
{
    mt_music();

    int iChan3Played = chan3played();

    // This will loop forever until you "pop" or change gamestate
    // or close the game
    UBYTE isAnyPressed = (keyUse(KEY_RETURN) | keyUse(KEY_ESCAPE)/* |
                          joyUse(JOY1 + JOY_FIRE) | joyUse(JOY2 + JOY_FIRE)*/);
    if (isAnyPressed)
    {
         gameExit();
        return;
    }

    static BYTE bCounter = 0;
    static UBYTE *ptr = (UBYTE *)radiallinespositions_data;
    static UWORD uwFrame = 0;
    static UWORD uwCenterOffset = 0;
    static UWORD uwPattern = 0xFFFF;

    spriteVectorApplyForce(&g_tVector,&g_Gravity);
    moverAddAccellerationToVelocity(&g_tVector);
    moverAddVelocityToLocation(&g_tVector);
    
    UWORD x=(UWORD)fix16_to_int(g_tVector.tLocation.x);
    UWORD y=(UWORD)fix16_to_int(g_tVector.tLocation.y);
    if (x>170)
    {
        g_tVector.tVelocity.x=fix16_mul(g_tVector.tVelocity.x,fix16_from_int(-1));
        g_tVector.tVelocity.x=fix16_sub(g_tVector.tVelocity.x,g_tVector.tAccelleration.x);
    }
    if (y>50)
    {
        g_tVector.tVelocity.y=fix16_mul(g_tVector.tVelocity.y,fix16_from_int(-1));
        g_tVector.tVelocity.y=fix16_sub(g_tVector.tVelocity.y,g_tVector.tAccelleration.y);
    }
    spriteVectorResetAccelleration(&g_tVector);
    
    blitClear(s_pMainBufferRadialLines, 0);
    scorri();

    static UBYTE i;
    for (i = 0; i < 40; i++)
    {
        if (i==10)
        { 
            if (bCounter==0 )
            {
                //static const char* pTextPointer = &g_cPhrase[0];
                static int iCharCounter = 0;
                printCharToRight(g_cPhrase[iCharCounter++]);
                if (g_cPhrase[iCharCounter]=='\n') iCharCounter = 0;
                /*pTextPointer++;
                if (*pTextPointer=='\n') pTextPointer=g_cPhrase;*/
            }
        }
        UWORD uwX1 = (UWORD)(*ptr);
        UWORD uwY1 = (UWORD)(*(ptr + 1));

        if (uwX1 > 0 && uwY1 > 0)
        {
            InitLine();
            g_pCustom->bltbdat = uwPattern;

            UWORD uwHalfX1 = (80+uwX1)>>1;
            UWORD uwHalfY1 = (80+uwY1)>>1;

            UWORD uwThreeqX1 = (uwHalfX1+uwX1)>>1;
            UWORD uwThreeqY1 = (uwHalfY1+uwY1)>>1;

            if (iChan3Played) DrawlineOr((UBYTE *)((ULONG)s_pMainBufferRadialLines->pBack->Planes[0]), 
                80 + x + uwCenterOffset, 
                80 + y + uwCenterOffset, 
                x + uwX1, 
                y + uwY1
            );

            else DrawlineOr((UBYTE *)((ULONG)s_pMainBufferRadialLines->pBack->Planes[0]), 
                80 + x + uwCenterOffset, 
                80 + y + uwCenterOffset, 
                x + uwThreeqX1, 
                y + uwThreeqY1
            );
        }

        ptr = ptr + 2;
    }

    vPortWaitForEnd(s_pVpMain);
    mt_music();

    for (i = 40; i < 80; i++)
    {

        UWORD uwX1 = (UWORD)(*ptr);
        UWORD uwY1 = (UWORD)(*(ptr + 1));

        if (uwX1 > 0 && uwY1 > 0)
        {
            //InitLine();
            g_pCustom->bltbdat = uwPattern;

            UWORD uwHalfX1 = (80+uwX1)>>1;
            UWORD uwHalfY1 = (80+uwY1)>>1;

            UWORD uwThreeqX1 = (uwHalfX1+uwX1)>>1;
            UWORD uwThreeqY1 = (uwHalfY1+uwY1)>>1;

            if (iChan3Played) DrawlineOr((UBYTE *)((ULONG)s_pMainBufferRadialLines->pBack->Planes[0]), 
                80 + x + uwCenterOffset, 
                80 + y + uwCenterOffset, 
                x + uwX1, 
                y + uwY1
            );

            else DrawlineOr((UBYTE *)((ULONG)s_pMainBufferRadialLines->pBack->Planes[0]), 
                80 + x + uwCenterOffset, 
                80 + y + uwCenterOffset, 
                x + uwThreeqX1, 
                y + uwThreeqY1
            );

        }
        ptr = ptr + 2;

        static UWORD pColors[32];
        if (i==40) ++s_ubBarHue;
        
        else if (i==50)
        {   
            for(UBYTE i = 0; i < 16; ++i) {
                pColors[i] = colorHSV(s_ubBarHue, 255, i * 17);
            }
        }
        else if (i==60)
        {
            for(UBYTE i = 16; i < 32; ++i) {
                pColors[i] = colorHSV(s_ubBarHue, 255, (31-i) * 17);
            }
        }
        else if (i==70)
        {
        
            tCopBfr *pCopBfr = s_pView->pCopList->pBackBfr;
            tCopCmd *pBarCmds = &pCopBfr->pList[s_uwCopRawOffs];
            for(UBYTE i = 0; i < 32; ++i) {
                // Replace WAIT cmd's Y value.
                pBarCmds[i * 2 + 0].sWait.bfWaitY = s_uwBarY + i*COPWIDTH;
                // Replace color value
                pBarCmds[i * 2 + 1].sMove.bfValue = pColors[i];
                if (s_uwBarY + i*COPWIDTH==252) copSetWait(&pBarCmds[i * 2 + 1].sWait, 0xdf, 0xff);
            }
        }
    }


    if (ptr > radiallinespositions_data + radiallinespositions_size - 2)
    {
        ptr = (UBYTE *)radiallinespositions_data;
    }

    

    bCounter ++;
    if (bCounter > 15) bCounter = 0;

    vPortWaitForEnd(s_pVpMain);
    viewProcessManagers(s_pView);
    
    copSwapBuffers();

    if (chan3played())
    {
        if (uwFrame==0)
        {
            g_pCustom->color[1] = 0x0FF0;
            uwCenterOffset = 0;
            uwPattern = 0xFFFF;
        }
        else if (uwFrame==1)
        {
            g_pCustom->color[1] = 0x0555;
            uwCenterOffset = 0;
            uwPattern = 0xFFFF;
        }
        else if (uwFrame==2)
        {
            g_pCustom->color[1] = 0x00AA;
            uwCenterOffset = 15;
            uwPattern = 0x0F0F;
        }
        else if (uwFrame==3)
        {
            g_pCustom->color[1] = 0x0FFF;
            uwCenterOffset = 5;
            uwPattern = 0xF0F0;
        }
        else if (uwFrame==4)
        {
            g_pCustom->color[1] = 0x0F00;
            uwCenterOffset = 25;
            uwPattern = 0xFFFF;
        }
        uwFrame++;
        if (uwFrame>4) uwFrame=0;
    }
}

void radialLinesGsDestroy(void)
{
    mt_end();
    
    // Cleanup when leaving this gamestate
    systemUse();
    FreeMem(s_pMusic,g_tDiscocrazy_data_size);

    fontDestroyTextBitMap(s_pGlyph);
    fontDestroy(s_pFontUI);

    // This will also destroy all associated viewports and viewport managers
    viewDestroy(s_pView);
}

void blitClear(tSimpleBufferManager *buffer, UBYTE nBitplane)
{
    blitWait();
    //waitblit();
    g_pCustom->bltcon0 = 0x0100;
    g_pCustom->bltcon1 = 0x0000;
    g_pCustom->bltafwm = 0xFFFF;
    g_pCustom->bltalwm = 0xFFFF;
    g_pCustom->bltamod = 0x0000;
    g_pCustom->bltbmod = 0x0000;
    g_pCustom->bltcmod = 0x0000;
    g_pCustom->bltdmod = 0x0000;
    g_pCustom->bltdpt = (UBYTE *)((ULONG)buffer->pBack->Planes[nBitplane]);
    g_pCustom->bltsize = 0x3794;

    return;
}

/**
 * Converts 24-bit HSV to 12-bit RGB
 * This fn is messy copypasta from stackoverflow to make it run on 12-bit.
 */
static UWORD colorHSV(UBYTE ubH, UBYTE ubS, UBYTE ubV)
{
    UBYTE ubRegion, ubRem, p, q, t;

    if (ubS == 0)
    {
        ubV >>= 4; // 12-bit fit
        return (ubV << 8) | (ubV << 4) | ubV;
    }

    ubRegion = ubH / 43;
    ubRem = (ubH - (ubRegion * 43)) * 6;

    p = (ubV * (255 - ubS)) >> 8;
    q = (ubV * (255 - ((ubS * ubRem) >> 8))) >> 8;
    t = (ubV * (255 - ((ubS * (255 - ubRem)) >> 8))) >> 8;

    ubV >>= 4;
    p >>= 4;
    q >>= 4;
    t >>= 4; // 12-bit fit
    switch (ubRegion)
    {
    case 0:
        return (ubV << 8) | (t << 4) | p;
    case 1:
        return (q << 8) | (ubV << 4) | p;
    case 2:
        return (p << 8) | (ubV << 4) | t;
    case 3:
        return (p << 8) | (q << 4) | ubV;
    case 4:
        return (t << 8) | (p << 4) | ubV;
    default:
        return (ubV << 8) | (p << 4) | q;
    }
}

void printCharToRight(char carToPrint)
{
  int carToPrintOffset = ((int)carToPrint-0x20)*40;     // Prima tolgo 0x20 perché il primo carattere nel file di ramjam è uno spazio, poi moltiplico per 40 bytes perché i caratteri sono 2 bytes*20 righe
  UBYTE* firstBitPlane = (UBYTE *)((ULONG)s_pMainBufferRadialLines->pBack->Planes[1]);
  
  // vogliamo stampare all'estrema destra quindi aggiungiamo 38
  firstBitPlane += 38;
  firstBitPlane += 40*TEXTYPOS;
  
  for (int i=0;i<20;i++)
  {
      *firstBitPlane = (UBYTE)fonts_data[carToPrintOffset];
      firstBitPlane++;
      *firstBitPlane = (UBYTE)fonts_data[carToPrintOffset+1];
      firstBitPlane++;
      carToPrintOffset+=2;
      firstBitPlane+=38;
  }
}

void scorri()
{
   /* MoveText
	move.l #PIC+((20*(0+20))-1)*2,d0
	btst #6,$dff002
waitblitmovetext
	btst #6,$dff002
	bne.s waitblitmovetext
	
	move.l #$19f00002,$dff040 ; BLTCON0 and BLTCON1
				  ; copy from chan A to chan D
				  ; with 1 pixel left shift
	
	move.l #$ffff7fff,$dff044 ; delete leftmost pixel
	
	move.l d0,$dff050	  ; load source
	move.l d0,$dff054	  ; load destination
	
	move.l #$00000000,$dff064 ; BTLAMOD and BTLDMOD will be zeroed
				  ; because the blitter operation will take
				  ; the whole screen width
				  
	move.w #(20*64)+20,$dff058 ; the rectangle we are blitting it is 20px
				  ; high (like the font heght) and
				  ; 20 bytes wide (the whole screen)
	
	rts*/
	
	blitWait();
	//waitblit();
	g_pCustom->bltcon0 = 0x19f0;
	g_pCustom->bltcon1 = 0x0002;
	
	g_pCustom->bltafwm = 0xffff;
	g_pCustom->bltalwm = 0x7fff;
	
	g_pCustom->bltamod = 0x0000;
    g_pCustom->bltbmod = 0x0000;
    g_pCustom->bltcmod = 0x0000;
    g_pCustom->bltdmod = 0x0000;
    
    UBYTE* firstBitPlane = (UBYTE *)((ULONG)s_pMainBufferRadialLines->pFront->Planes[1])+((20*20-1)*2);
    UBYTE* firstBitPlane2 = (UBYTE *)((ULONG)s_pMainBufferRadialLines->pBack->Planes[1])+((20*20-1)*2);

    firstBitPlane += 40*TEXTYPOS;
    firstBitPlane2 += 40*TEXTYPOS;
    
    g_pCustom->bltdpt = firstBitPlane2;
    g_pCustom->bltapt = firstBitPlane;
    
    g_pCustom->bltsize = 0x0514;
}


