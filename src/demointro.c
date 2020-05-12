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

#include "demointro.h"
#include "casentino.h"
//#include "customtrigonometry.h"

#include <ace/managers/key.h> // Keyboard processing
#include <ace/managers/game.h> // For using gameClose
#include <ace/managers/system.h> // For systemUnuse and systemUse
#include <ace/managers/viewport/simplebuffer.h> // Simple buffer
#include <ace/utils/palette.h>
#include "../_res/bootimg.h"
#include "../_res/bootimgpalette.h"
#include <ace/managers/blit.h>

static tView *s_pView; // View containing all the viewports

static tVPort *s_pVpMain; // Viewport for playfield
static tSimpleBufferManager *s_pMainBuffer;
static UBYTE s_ubTrigInit = 0;
void copyToMainBplIntro(const unsigned char*,const UBYTE, const UBYTE);

void demointroGsCreate(void) {
  // Create a view - first arg is always zero, then it's option-value
  s_pView = viewCreate(0,
    TAG_VIEW_GLOBAL_CLUT, 1, // Same Color LookUp Table for all viewports
  TAG_END); // Must always end with TAG_END or synonym: TAG_DONE

  // Now let's do the same for main playfield
  s_pVpMain = vPortCreate(0,
    TAG_VPORT_VIEW, s_pView,
    TAG_VPORT_BPP, 3, // 2 bits per pixel, 4 colors
    // We won't specify height here - viewport will take remaining space.
  TAG_END);
  s_pMainBuffer = simpleBufferCreate(0,
    TAG_SIMPLEBUFFER_VPORT, s_pVpMain, // Required: parent viewport
    TAG_SIMPLEBUFFER_BITMAP_FLAGS, BMF_CLEAR,
  TAG_END);

  // Since we've set up global CLUT, palette will be loaded from first viewport
  // Colors are 0x0RGB, each channel accepts values from 0 to 15 (0 to F).
  s_pVpMain->pPalette[0] = 0x0111; // First color is also border color
  s_pVpMain->pPalette[1] = 0x09ad; // Gray
  s_pVpMain->pPalette[2] = 0x055b; // Red - not max, a bit dark
  s_pVpMain->pPalette[3] = 0x0127; // Blue - same brightness as red
  s_pVpMain->pPalette[4] = 0x0cce;
  s_pVpMain->pPalette[5] = 0x077c;
  s_pVpMain->pPalette[6] = 0x0dff;
  s_pVpMain->pPalette[7] = 0x033a;

  /*s_pVpMain->pPalette[8] = 0x0955;
  s_pVpMain->pPalette[9] = 0x0069;
  s_pVpMain->pPalette[10] = 0xe0a;
  s_pVpMain->pPalette[11] = 0x0f99;
  s_pVpMain->pPalette[12] = 0x0f88;
  s_pVpMain->pPalette[13] = 0x0eaa;
  s_pVpMain->pPalette[14] = 0x0004;
  s_pVpMain->pPalette[15] = 0x0346;*/

  UWORD s_pPaletteRef[32];
  paletteLoadFromMem(bootimgpalette_data, s_pPaletteRef, 1 << s_pVpMain->ubBPP);
  /*for (UBYTE ubContColor=0;ubContColor< 8;ubContColor++)
    s_pVpMain->pPalette[ubContColor]=s_pPaletteRef[ubContColor];*/

  copyToMainBplIntro(bootimg_data,0,3);

  

  // We don't need anything from OS anymore
  systemUnuse();

  // Load the view
  viewLoad(s_pView);
}

void demointroGsLoop(void) {
  // This will loop forever until you "pop" or change gamestate
  // or close the game
  //if(keyCheck(KEY_ESCAPE)) {
    static int frameno = 0;
  if (frameno++>1000) {
    //gameClose();return ;
    gameChangeState(gameGsCreate,gameGsLoop,gameGsDestroy);
    return ;
  }
  else {
    // Process loop normally
    // We'll come back here later
  }
  viewUpdateCLUT(s_pView);
  copProcessBlocks();
  vPortWaitForEnd(s_pVpMain);

  if (!s_ubTrigInit)
  {
    /*fix16_sinlist_init();
    fix16_coslist_init();
    s_ubTrigInit=1;*/
  }
}

void demointroGsDestroy(void) {
  // Cleanup when leaving this gamestate
  systemUse();

  //viewDestroy(s_pView);
}

// Function to copy data to a main bitplane
// Pass ubMaxBitplanes = 0 to use all available bitplanes in the bitmap
void copyToMainBplIntro(const unsigned char* pData,const UBYTE ubSlot,const UBYTE ubMaxBitplanes)
{
  UBYTE ubBitplaneCounter;
  for (ubBitplaneCounter=0;ubBitplaneCounter<s_pMainBuffer->pBack->Depth;ubBitplaneCounter++)
  {
    blitWait();
    g_pCustom->bltcon0 = 0x09F0;
    g_pCustom->bltcon1 = 0x0000;
    g_pCustom->bltafwm = 0xFFFF;
    g_pCustom->bltalwm = 0xFFFF;
    g_pCustom->bltamod = 0x0000;
    g_pCustom->bltbmod = 0x0000;
    g_pCustom->bltcmod = 0x0000;
    g_pCustom->bltdmod = 0x0000;
    g_pCustom->bltapt = (UBYTE*)((ULONG)&pData[40*256*ubBitplaneCounter]);
    g_pCustom->bltdpt = (UBYTE*)((ULONG)s_pMainBuffer->pBack->Planes[ubBitplaneCounter]+(40*ubSlot));
    g_pCustom->bltsize = 0x4014;
    if (ubMaxBitplanes>0 && ubBitplaneCounter+1>=ubMaxBitplanes) return ;
  }
  return ;
}
