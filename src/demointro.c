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

#include <ace/managers/key.h>                   // Keyboard processing
#include <ace/managers/game.h>                  // For using gameClose
#include <ace/managers/system.h>                // For systemUnuse and systemUse
#include <ace/managers/viewport/simplebuffer.h> // Simple buffer
#include <ace/utils/palette.h>
#include <ace/managers/blit.h>
#include "main.h"
// Assets
#include "../_res/bootimg.h"
#include "../_res/bootimgpalette.h"
//#include "../_res/valkyrie320x244.h"
// #include "../_res/valkyrie320x244palette.h"
// #include "../_res/valchiria320x256.h"

tView *g_pViewIntro=NULL; // View containing all the viewports

static tVPort *s_pVpMain; // Viewport for playfield
static tSimpleBufferManager *s_pMainBuffer;
static UBYTE s_ubTrigInit = 0;
static UWORD s_uwYImgRes = 256;
static UWORD s_uwCopRawOffs;
static tCopCmd *s_pModCmds;
static UBYTE s_ubMoveArray[256];
static int s_iHideCounter = 0;
static UBYTE s_ubDiwStrt = 0x2c;
static UBYTE s_ubDiwStop = 0x2c;

static UWORD s_uwRawToHide[] = {

127, 143,
 111, 159,
  95, 175, 
  79, 191,
  63, 207, 
  47, 223, 
  31, 239, 
 15, 255, 
  
  126, 144, 109, 161, 92, 178, 75, 195, 58, 212, 41, 229, 24, 246, 7, 128, 146, 108, 164, 90, 182, 72, 200, 54, 218, 36, 236, 18, 254, 0, 125, 147, 106, 166, 87, 185, 68, 204, 49, 224, 29, 243, 10, 129, 149, 105, 169, 85, 189, 65, 210, 44, 231, 23, 251, 3, 124, 150, 103, 171, 82, 193, 60, 215, 38, 237, 16, 130, 152, 102, 174, 80, 198, 56, 221, 33, 245, 9, 123, 153, 100, 177, 76, 202, 51, 227, 26, 252, 1, 131, 155, 99, 181, 73, 208, 46, 234, 20, 122, 156, 97, 184, 69, 213, 40, 241, 12, 132, 158, 96, 188, 66, 219, 35, 249, 5, 121, 160, 93, 192, 61, 225, 28, 133, 163, 91, 197, 57, 232, 22, 120, 165, 88, 201, 52, 238, 14, 134, 168, 86, 206, 48, 247, 8, 119, 170, 83, 211, 42, 253, 118, 172, 78, 216, 34, 135, 176, 77, 222, 30, 117, 179, 71, 228, 21, 136, 183, 70, 235, 17, 116, 186, 64, 242, 6, 137, 190, 62, 250, 2, 115, 194, 55, 138, 199, 53, 114, 203, 45, 139, 209, 43, 113, 214, 37, 140, 220, 32, 112, 226, 25, 141, 233, 19, 110, 240, 11, 142, 248, 4, 107, 145, 104, 148, 101, 151, 98, 154, 94, 157, 89, 162, 84, 167, 81,13,244,27,230,39,217,50,205,59,196,67,205,74,217,230
};

// Shrink and expand functions
UBYTE shrinkFromBottom();
UBYTE expandFromBottom();
UBYTE shrinkFromTop();
UBYTE expandFromTop();

inline void myCopSetMove(tCopMoveCmd *pMoveCmd, UWORD uwValue) {
	pMoveCmd->bfUnused = 0;
	pMoveCmd->bfValue = uwValue;
}

void setBplModuleAt3(UWORD, UWORD, UBYTE);
//void copyToMainBplIntro(const unsigned char *, const UBYTE, const UBYTE);
void copyToMainBplFromFastIntro(const unsigned char* ,const UBYTE ,const UBYTE );

UWORD getConsecutiveRowsAfter(UWORD);

void demointroGsCreate(void)
{

  ULONG ulRawSize = (simpleBufferGetRawCopperlistInstructionCount(3) + // Allocate cop space for 3 bitplanes
                     s_uwYImgRes * 4 +                                 // s_uwYImgRes modules changes - each consists of WAIT + 2 MOVE instruction + wait 255
                     1 +                                               // Final WAIT
                     1                                                 // Just to be sure
  );

  g_pViewIntro = viewCreate(0,
                       TAG_VIEW_GLOBAL_CLUT, 1,                      // Same Color LookUp Table for all viewports
                       TAG_VIEW_COPLIST_MODE, VIEW_COPLIST_MODE_RAW, // Let's rock with a raw copperlist
                       TAG_VIEW_COPLIST_RAW_COUNT, ulRawSize,        // rawsize is so much important in a raw copperlist, without this it will crash everything in the world
                       TAG_END);

  s_pVpMain = vPortCreate(0,
                          TAG_VPORT_VIEW, g_pViewIntro,
                          TAG_VPORT_BPP, 3, // 3 bits per pixel, 8 colors
                          // We won't specify height here - viewport will take remaining space.
                          TAG_END);

  s_pMainBuffer = simpleBufferCreate(0,
                                     TAG_SIMPLEBUFFER_VPORT, s_pVpMain, // Required: parent viewport
                                     TAG_SIMPLEBUFFER_BITMAP_FLAGS, BMF_CLEAR,
                                     TAG_SIMPLEBUFFER_COPLIST_OFFSET, 0, // very important for copperlist raw mode
                                     TAG_END);

  s_uwCopRawOffs = simpleBufferGetRawCopperlistInstructionCount(3);

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

  copyToMainBplFromFastIntro(bootimg_data_fast, 0, 3);

  // We don't need anything from OS anymore
  systemUnuse();

  // This is the glue, without this we cant change copperlist
  tCopBfr *pCopBfr = g_pViewIntro->pCopList->pBackBfr;
  s_pModCmds = &pCopBfr->pList[s_uwCopRawOffs];

  // Reset copperlist!!!!!
  for (UWORD uwCounter = 0; uwCounter <= s_uwYImgRes; uwCounter++)
    setBplModuleAt3(uwCounter,(UWORD) 0x0000,1);

  // Copy the same thing to front buffer, so that copperlist has the same
  // structure on both buffers and we can just update parts we need
  CopyMemQuick(
      g_pViewIntro->pCopList->pBackBfr->pList,
      g_pViewIntro->pCopList->pFrontBfr->pList,
      g_pViewIntro->pCopList->pBackBfr->uwAllocSize);

  // Load the view
  viewLoad(g_pViewIntro);
}

void demointroGsLoop(void)
{
  static UBYTE automaticMode = 1;
  static UBYTE ubFrameModule = 0;
  UBYTE ubRefresh = 0;

  if (automaticMode == 1)
  {
    if (ubFrameModule == 0)
    {
      ubRefresh = shrinkFromTop();
      ubFrameModule = 1;
    }
    else
    {
      ubRefresh = shrinkFromBottom();
      ubFrameModule = 0;
    }
    if (automaticMode == 1 && ubRefresh == 0)
    {
      //copyToMainBplIntro(bootimg_data, 0, 3); maybe another image ????
      automaticMode = 2;
      
    }
  }
  else if (automaticMode == 2)
  {
    if (ubFrameModule==0)
    {
      ubRefresh = expandFromTop();
      ubFrameModule=1;
    }
    else
    {
      ubRefresh = expandFromBottom();
      ubFrameModule=0;
    }

    if (automaticMode == 2 && ubRefresh == 0)
    {
      stateChange(g_pGameStateManager, g_pGameStates[1]);
      return ;
    }
  }

  if ( ubRefresh )
  {

    ubRefresh = 0;

    UWORD uwNewBplModValue;
    UWORD uwRayCounter = s_ubDiwStrt - 0x2c;
    UWORD uWModdedRows = 0;
    for (UWORD uwCounter = 0; uwCounter < s_uwYImgRes && uwRayCounter < s_uwYImgRes; uwCounter++)
    {

      UWORD uwNewIndex = uwRayCounter;

      myCopSetMove(&s_pModCmds[uwNewIndex * 4 + 1].sMove, 0x0000);
      myCopSetMove(&s_pModCmds[uwNewIndex * 4 + 2].sMove, 0x0000);

      if (s_ubMoveArray[uwCounter] == 0)
      {
        uwRayCounter++;
      }
      else
      {
        // quante righe consecutive sotto???
        UWORD uwConsecutiveRowsAfter = getConsecutiveRowsAfter(uwCounter);
        uwNewBplModValue = 40 + uwConsecutiveRowsAfter * 40;

        uwNewIndex -= uWModdedRows;
#ifdef DO_LOG_WRITE
        logWrite("Righe sotto consecutive : %u\n", uwConsecutiveRowsAfter);
        logWrite("Imposto singolo mod alla riga : %u\n", uwNewIndex);
#endif

        myCopSetMove(&s_pModCmds[uwNewIndex * 4 + 1].sMove, uwNewBplModValue);
        myCopSetMove(&s_pModCmds[uwNewIndex * 4 + 2].sMove, uwNewBplModValue);

        for (UWORD uwNextCounter = 1; uwNextCounter <= uwConsecutiveRowsAfter; uwNextCounter++)
        {
          UWORD uwNewIndex2 = uwNextCounter + uwNewIndex;
#ifdef DO_LOG_WRITE
          logWrite("Mod aggiuntivo a zero per indice : %u\n", uwNewIndex2);
#endif
          if (uwNewIndex2 < 256)
          {
            myCopSetMove(&s_pModCmds[uwNewIndex2 * 4 + 1].sMove, 0x0000);
            myCopSetMove(&s_pModCmds[uwNewIndex2 * 4 + 2].sMove, 0x0000);
          }
        }
        uWModdedRows++;
        uwRayCounter++;
        uwCounter += uwConsecutiveRowsAfter;
      }
    }

    //copDumpBfr(s_pCopBfr);

    /*CopyMemQuick(
			s_pView->pCopList->pBackBfr->pList,
			s_pView->pCopList->pFrontBfr->pList,
			s_pView->pCopList->pBackBfr->uwAllocSize
		);*/
    //g_pCustom->color[0] = 0x0000;
    // Process loop normally
    // We'll come back here later
  }

  /* viewUpdateCLUT(s_pView);
  copProcessBlocks();*/
  vPortWaitForEnd(s_pVpMain);

  if (!s_ubTrigInit)
  {
    /*fix16_sinlist_init();
    fix16_coslist_init();
    s_ubTrigInit=1;*/
  }
}

void demointroGsDestroy(void)
{
  // Cleanup when leaving this gamestate
  // systemUse();

  //viewDestroy(s_pView);
}

// Function to copy data to a main bitplane
// Pass ubMaxBitplanes = 0 to use all available bitplanes in the bitmap
/*void copyToMainBplIntro(const unsigned char *pData, const UBYTE ubSlot, const UBYTE ubMaxBitplanes)
{
  UBYTE ubBitplaneCounter;
  for (ubBitplaneCounter = 0; ubBitplaneCounter < s_pMainBuffer->pBack->Depth; ubBitplaneCounter++)
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
    g_pCustom->bltapt = (UBYTE *)((ULONG)&pData[40 * 256 * ubBitplaneCounter]);
    g_pCustom->bltdpt = (UBYTE *)((ULONG)s_pMainBuffer->pBack->Planes[ubBitplaneCounter] + (40 * ubSlot));
    g_pCustom->bltsize = 0x4014;
    if (ubMaxBitplanes > 0 && ubBitplaneCounter + 1 >= ubMaxBitplanes)
      return;
  }
  return;
}*/

// Function to copy data to a main bitplane
// Pass ubMaxBitplanes = 0 to use all available bitplanes in the bitmap
void copyToMainBplFromFastIntro(const unsigned char* pData,const UBYTE ubSlot,const UBYTE ubMaxBitplanes)
{
  UBYTE ubBitplaneCounter;
  size_t iSize ;

  if (ubMaxBitplanes==0) iSize = 40*256*s_pMainBuffer->pBack->Depth;
  else iSize = 40*256*ubMaxBitplanes;
  
  UBYTE* pTmp = (UBYTE*)AllocMem(iSize,MEMF_CHIP);
  memcpy((void*)pTmp,(void*)pData,iSize);

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
    g_pCustom->bltapt = (UBYTE*)((ULONG)&pTmp[40*256*ubBitplaneCounter]);
    g_pCustom->bltdpt = (UBYTE*)((ULONG)s_pMainBuffer->pBack->Planes[ubBitplaneCounter]+(40*ubSlot));
    g_pCustom->bltsize = 0x4014;
    if (ubMaxBitplanes>0 && ubBitplaneCounter+1>=ubMaxBitplanes)
    {
      blitWait();
      FreeMem(pTmp,iSize);
      return ;
    }
  }
  blitWait();
  FreeMem(pTmp,iSize);
  return ;
}

void setBplModuleAt3(UWORD uwIndex, UWORD uwModValue, UBYTE ubDoWait)
{
  if (ubDoWait)
  {
    copSetWait(&s_pModCmds[uwIndex * 4 + 0].sWait, 0x00, 43 + uwIndex);
  }
  copSetMove(&s_pModCmds[uwIndex * 4 + 1].sMove, &g_pCustom->bpl1mod, uwModValue);
  copSetMove(&s_pModCmds[uwIndex * 4 + 2].sMove, &g_pCustom->bpl2mod, uwModValue);
  if (43 + uwIndex == 255)
  {
    copSetWait(&s_pModCmds[uwIndex * 4 + 3].sWait, 0xdf, 0xff);
  }
  else
    copSetMove(&s_pModCmds[uwIndex * 4 + 3].sMove, &g_pCustom->color[0], ubDoWait ? 0 : 0x0f00);
}

UBYTE shrinkFromBottom()
{
  // Check if there is some row to hide
  if ((size_t)s_iHideCounter+1>sizeof(s_uwRawToHide)/sizeof(UWORD)) return 0;
   
  // Move diwstop up of one unit
  s_ubDiwStop --;
  g_pCustom->diwstop =(s_ubDiwStop<<8)|0x00C1;

  // set the row I want to hide uquals to 1
  s_ubMoveArray[s_uwRawToHide[s_iHideCounter]]=1;
  
  // Next time we will get the following row
  s_iHideCounter++;
  return 1;
}

UBYTE expandFromBottom()
{
  // Check if there is some row to hide
  if ((size_t)s_iHideCounter<=0) return 0;
  
  // Move diwstop down one unit
  s_ubDiwStop ++;
  g_pCustom->diwstop =(s_ubDiwStop<<8)|0x00C1;

  // Go to the previous row...
  s_iHideCounter--;

  // and set his value to 0 because we want to show it
  s_ubMoveArray[s_uwRawToHide[s_iHideCounter]]=0;
  return 1;
}

UBYTE shrinkFromTop()
{
  // Check if there is some row to hide
  if ((size_t)s_iHideCounter+1>sizeof(s_uwRawToHide)/sizeof(UWORD)) return 0;
   
  // Move Diwstrt down one pixel
  s_ubDiwStrt ++;
  g_pCustom->diwstrt =(s_ubDiwStrt<<8)|0x0081;

  // set the row I want to hide uquals to 1
  s_ubMoveArray[s_uwRawToHide[s_iHideCounter]]=1;

  // Next time we will get the following row
  s_iHideCounter++;

  return 1;
}

UBYTE expandFromTop()
{
  // Check if there is some row to hide
  if ((size_t)s_iHideCounter<=0) return 0;

  // Move diwstrt up one pixel
  s_ubDiwStrt --;
  g_pCustom->diwstrt =(s_ubDiwStrt<<8)|0x0081;

  // Go to the previous row...
  s_iHideCounter--;

  // Clear the row I want to hide
  s_ubMoveArray[s_uwRawToHide[s_iHideCounter]]=0;

  return 1;
}

UWORD getConsecutiveRowsAfter(UWORD uwRowIndex)
{
  WORD wOffsetTmp = uwRowIndex+1;
  UWORD uwConsecutiveRows = 0;
  while (wOffsetTmp<256 && s_ubMoveArray[wOffsetTmp])
  {
    uwConsecutiveRows++;
    wOffsetTmp++;
  }
  return uwConsecutiveRows;
}