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

#include "../_res/radiallinespositions.h"

// Music in mod format
//#include "../_res/paper_cut.h"  // Thx to morph of dual crew

// Fonts
//#include "../_res/uni54.h"
//#include "shardeddata.h"
// ASSETS END

#define COPWIDTH 8
#define BITPLANES 2

long mt_init(const unsigned char *);
void mt_music();
void DrawlineOr(UBYTE *, int, int, int, int);
void InitLine();

#define BLIT_LINE_OR ((ABC | ABNC | NABC | NANBC) | (SRCA | SRCC | DEST))
#define BLIT_LINE_XOR ((ABNC | NABC | NANBC) | (SRCA | SRCC | DEST))
#define BLIT_LINE_ERASE ((NABC | NANBC | ANBC) | (SRCA | SRCC | DEST))

void blitClear(tSimpleBufferManager *, UBYTE);
void printCursorPixel(tSimpleBufferManager *, UWORD, UWORD);
static UWORD colorHSV(UBYTE, UBYTE, UBYTE);

static tView *s_pView;    // View containing all the viewports
static tVPort *s_pVpMain; // Viewport for playfield
static tSimpleBufferManager *s_pMainBuffer;
static tFont *s_pFontUI;
static tTextBitMap *s_pGlyph;
static UBYTE s_ubBarHue = 0;
static UWORD s_uwBarY = 44;
static UWORD s_uwCopRawOffs;

const unsigned char *uni54_data_shared;

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

    s_pMainBuffer = simpleBufferCreate(0,
                                       TAG_SIMPLEBUFFER_VPORT, s_pVpMain, // Required: parent viewport
                                       TAG_SIMPLEBUFFER_BITMAP_FLAGS, BMF_CLEAR,
                                       TAG_SIMPLEBUFFER_IS_DBLBUF, 1,
                                       TAG_SIMPLEBUFFER_COPLIST_OFFSET, 0, // <-- Important in RAW mode
                                       TAG_END);

    s_pVpMain->pPalette[0] = 0x0000; // First color is also border color
    s_pVpMain->pPalette[1] = 0x0FF0; // Yellow
    s_pVpMain->pPalette[2] = 0x0888; // Gray
    s_pVpMain->pPalette[3] = 0x0FFF; // WHITE

    s_uwCopRawOffs += simpleBufferGetRawCopperlistInstructionCount(BITPLANES);
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

    s_pFontUI = fontCreateFromMem((UBYTE *)uni54_data_shared);
    if (s_pFontUI == NULL)
        return;
    s_pGlyph = fontCreateTextBitMap(320, s_pFontUI->uwHeight);
    fontFillTextBitMap(s_pFontUI, s_pGlyph, "THX to Chillobits team for this awesome effect");
    fontDrawTextBitMap(s_pMainBuffer->pFront, s_pGlyph, 150, 230, 3, FONT_CENTER | FONT_LAZY);
    fontDrawTextBitMap(s_pMainBuffer->pBack, s_pGlyph, 150, 230, 3, FONT_CENTER | FONT_LAZY);

    // Load the view
    viewLoad(s_pView);

    //mt_init(g_tPapercutMod_data);
    viewProcessManagers(s_pView);
    copProcessBlocks();

    systemSetDma(DMAB_SPRITE, 0);
}

void radialLinesGsLoop(void)
{
    mt_music();

    // This will loop forever until you "pop" or change gamestate
    // or close the game
    UBYTE isAnyPressed = (keyUse(KEY_RETURN) | keyUse(KEY_ESCAPE) |
                          joyUse(JOY1 + JOY_FIRE) | joyUse(JOY2 + JOY_FIRE));
    if (isAnyPressed)
    {
        gameClose();
        return;
    }

    static BYTE bCounter = 0;
    static BYTE bIncrementer = 1;
    static UBYTE *ptr = (UBYTE *)radiallinespositions_data;
    static int frame = 0;
    static UWORD centerOffset = 0;
    static UWORD uwPattern = 0xFFFF;

    blitClear(s_pMainBuffer, 0);
    static UBYTE i;
    for (i = 0; i < 40; i++)
    {

        UWORD uwX1 = (UWORD)(*ptr);
        UWORD uwY1 = (UWORD)(*(ptr + 1));

        if (uwX1 > 0 && uwY1 > 0)
        {
            InitLine();
            g_pCustom->bltbdat = uwPattern;

            DrawlineOr((UBYTE *)((ULONG)s_pMainBuffer->pBack->Planes[0]), 80 + bCounter + centerOffset, 80 + bCounter + centerOffset, bCounter + uwX1, bCounter + uwY1);
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
            InitLine();
            g_pCustom->bltbdat = uwPattern;

            DrawlineOr((UBYTE *)((ULONG)s_pMainBuffer->pBack->Planes[0]), 80 + bCounter + centerOffset, 80 + bCounter + centerOffset, bCounter + uwX1, bCounter + uwY1);
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

    bCounter += bIncrementer;
    if (bCounter > 70 || bCounter < 1)
        bIncrementer *= -1;

    vPortWaitForEnd(s_pVpMain);
    viewProcessManagers(s_pView);
    //copProcessBlocks();
    copSwapBuffers();

    frame++;

    if (frame == 1)
    {
        //g_pCustom->color[0] = 0x000F;
        centerOffset = 30;
        uwPattern = 0x0FFF;
    }
    else if (frame == 300)
    {
        //g_pCustom->color[0] = 0x0F00;
        centerOffset = 0;
        uwPattern = 0xFFFF;
    }
    else if (frame == 600)
    {
        //g_pCustom->color[0] = 0x00F0;
        centerOffset = 15;
        uwPattern = 0x0F0F;
    }
    else if (frame == 900)
    {
        //g_pCustom->color[0] = 0x0FF0;
        centerOffset = 5;
        uwPattern = 0xF0F0;
    }

    if (frame > 1200)
    {

        frame = 0;
    }
}

void radialLinesGsDestroy(void)
{
    // Cleanup when leaving this gamestate
    systemUse();

    fontDestroyTextBitMap(s_pGlyph);
    fontDestroy(s_pFontUI);

    // This will also destroy all associated viewports and viewport managers
    viewDestroy(s_pView);
}

void blitClear(tSimpleBufferManager *buffer, UBYTE nBitplane)
{
    blitWait();
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
