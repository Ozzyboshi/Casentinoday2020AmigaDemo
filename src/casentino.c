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

#include "casentino.h"

#include <ace/managers/key.h> // Keyboard processing
#include <ace/managers/game.h> // For using gameClose
#include <ace/managers/system.h> // For systemUnuse and systemUse
#include <ace/managers/viewport/simplebuffer.h> // Simple buffer
#include <ace/utils/font.h> // needed for tFont and font stuff
#include <ace/managers/joy.h>
#include <ace/utils/palette.h>

// ASSETS START
// Images and palette
#include "../_res/valkyrie320x244.h"
#include "../_res/valkyrie320x244palette.h"
#include "../_res/ball2bpl16x16_frame1_palette.h"
#include "../_res/VampireItalialogo.h"
#include "../_res/VampireItalialogopalette.h"
#include "../_res/ball2bpl16x16_frame1.h"
#include "../_res/ball2bpl16x16_frame2.h"
#include "../_res/ball2bpl16x16_frame3.h"
#include "../_res/ball2bpl16x16_frame4.h"
#include "../_res/ball2bpl32x32_frame1_1.h"
#include "../_res/ball2bpl32x32_frame1_2.h"
#include "../_res/ball2bpl32x32_frame2_1.h"
#include "../_res/ball2bpl32x32_frame2_2.h"
#include "../_res/ball2bpl32x32_frame3_1.h"
#include "../_res/ball2bpl32x32_frame3_2.h"
#include "../_res/ball2bpl32x32_frame4_1.h"
#include "../_res/ball2bpl32x32_frame4_2.h"
#include "../_res/Aded320x224_1.h"
#include "../_res/Aded320x224_2.h"
#include "../_res/Aded320x224palette.h"

// Music in mod format
#include "../_res/paper_cut.h"  // Thx to morph of dual crew

// Fonts
#include "../_res/uni54.h"
// ASSETS END

#include "vectors.h"
#include "acecustom.h"
#include "physics.h"
#include "stages.h"


//#include <chunky.h>


//static v2d location,velocity;


//#define SOUND
/*#define ACE_MAXSPRITES 8
typedef struct _tAceSprite {
  UBYTE* pSpriteData;
  ULONG ulSpriteSize;
  BYTE bTrailingSpriteIndex;
  UWORD uwSpriteHeight;
  UWORD uwSpriteCenter;

  int iBounceBottomLimit;
  int iBounceRightLimit;

} tAceSprite;
tAceSprite s_pAceSprites[ACE_MAXSPRITES];*/

//#define MAXSTAGES 2
typedef struct _tStageManager {
  void (*g_pPreStageFunction) ();
  void (*g_pStageFunction) ();
} tStageManager;
tStageManager s_pStagesFunctions[MAXSTAGES]={
  { .g_pPreStageFunction = stage1pre, .g_pStageFunction=stage1},
  { .g_pPreStageFunction = stage2pre, .g_pStageFunction=stage2}
};

//#define NUM_IMAGES 8
#define TOTAL_WIDTH 320*NUM_IMAGES
#define RESET_SCROLL 320*(NUM_IMAGES-1)

#define FONT_MAIN_COLOR 10

#define PRINTF(var,var2,var3) fontDrawStr(s_pMainBuffer->pBack, s_pFontUI, var*320+10,var2,var3, FONT_MAIN_COLOR, FONT_LAZY);
#define SETSPRITEIMG(var,var2,var3) memcpy(s_pAceSprites[var].pSpriteData+4,var2,var3);

#ifdef SOUND
void mt_music();
void mt_end();
#endif

//void spriteVectorInit(tMover* ,const UBYTE,const int, const int, const int, const int, const unsigned int);
//void spriteVectorApplyForce(tMover*,v2d*);
//UBYTE moverMove(tMover);
void moverBounce(tMover*);

//void bounce();
//void reposition();
void copyToMainBpl(const unsigned char*,const UBYTE, const UBYTE);
tCopBlock *copBlockEnableSpriteFull(tCopList *, FUBYTE , UBYTE* , ULONG );

//void SetTrailingSprite(const FUBYTE, const FUBYTE);
/*void spriteMove2(FUBYTE fubSpriteIndex,UWORD,UWORD);
void spriteMove3(FUBYTE fubSpriteIndex,UWORD,UWORD);*/

void nextStage();

long mt_init(const unsigned char*);
void mt_music();

static inline void changeCopperColor(tView *s_pView, tCopBlock * myBlock,const unsigned char* p_PaletteFrom,const UWORD uwPaletteColorStart,const UWORD uwCopblockIndex)
{
  UWORD uwColTmp=p_PaletteFrom[uwPaletteColorStart*2];
  uwColTmp=uwColTmp<<8;
  uwColTmp=uwColTmp|p_PaletteFrom[uwPaletteColorStart*2+1];
  copChangeMove(s_pView->pCopList, myBlock, uwCopblockIndex, &g_pCustom->color[uwPaletteColorStart], uwColTmp);
}

tMover g_Sprite1Vector = {0,{0,0},{0,0},{0,0},0,0,0 };
tMover g_Sprite2Vector = {0,{0,0},{0,0},{0,0},0,0,0 };
tMover g_Sprite3Vector = {0,{0,0},{0,0},{0,0},0,0,0 };
tMover g_Sprite4Vector = {0,{0,0},{0,0},{0,0},0,0,0 };
v2d g_Gravity , g_Wind;

fix16_t g_WindStep;

static UWORD g_uwResetScroll = RESET_SCROLL;
static UWORD g_uwBlitModd = RESET_SCROLL/8;

static tFont *s_pFontUI;
static tTextBitMap *s_pGlyph;
static tCameraManager *s_pCamera;
tCameraManager *s_pCameraMain;

// All variables outside fns are global - can be accessed in any fn
// Static means here that given var is only for this file, hence 's_' prefix
// You can have many variables with same name in different files and they'll be
// independent as long as they're static
// * means pointer, hence 'p' prefix
static tView *s_pView; // View containing all the viewports
tVPort *s_pVpScore; // Viewport for score
static tSimpleBufferManager *s_pScoreBuffer;
static tVPort *s_pVpMain; // Viewport for playfield
tSimpleBufferManager *s_pMainBuffer;

static tCopBlock *myBlock;

UBYTE g_ubVBounceEnabled = 1;
UBYTE g_ubHBounceEnabled = 1;

static UBYTE g_ubStageIndex=0;

void gameGsCreate(void) {

  // Create a view - first arg is always zero, then it's option-value
  s_pView = viewCreate(0,
   // TAG_VIEW_COPLIST_MODE, VIEW_COPLIST_MODE_RAW,
    //TAG_VIEW_COPLIST_RAW_COUNT, 50,
    TAG_VIEW_GLOBAL_CLUT, 1, // Same Color LookUp Table for all viewports
  TAG_END); // Must always end with TAG_END or synonym: TAG_DONE

  // Viewport for score bar - on top of screen
  s_pVpScore = vPortCreate(0,
    TAG_VPORT_VIEW, s_pView, // Required: specify parent view
    TAG_VPORT_BPP, 4, // Optional: 2 bits per pixel, 4 colors
    TAG_VPORT_HEIGHT, 32, // Optional: let's make it 32px high
  TAG_END); // same syntax as view creation

  // Create simple buffer manager with bitmap exactly as large as viewport
  s_pScoreBuffer = simpleBufferCreate(0,
    TAG_SIMPLEBUFFER_VPORT, s_pVpScore, // Required: parent viewport
    TAG_SIMPLEBUFFER_BOUND_WIDTH,320*NUM_IMAGES,
    // Optional: buffer bitmap creation flags
    // we'll use them to initially clear the bitmap
    TAG_SIMPLEBUFFER_BITMAP_FLAGS, BMF_CLEAR,
//    TAG_SIMPLEBUFFER_COPLIST_OFFSET, 0,
  TAG_END);

  // Now let's do the same for main playfield
  s_pVpMain = vPortCreate(0,
    TAG_VPORT_VIEW, s_pView,
    TAG_VPORT_BPP, 4, // 2 bits per pixel, 4 colors
    // We won't specify height here - viewport will take remaining space.
  TAG_END);
  s_pMainBuffer = simpleBufferCreate(0,
    TAG_SIMPLEBUFFER_VPORT, s_pVpMain, // Required: parent viewport
    TAG_SIMPLEBUFFER_BITMAP_FLAGS, BMF_CLEAR,
    TAG_SIMPLEBUFFER_BOUND_WIDTH,TOTAL_WIDTH,
//    TAG_SIMPLEBUFFER_COPLIST_OFFSET, 20,
  TAG_END);

 

  // Since we've set up global CLUT, palette will be loaded from first viewport
  // Colors are 0x0RGB, each channel accepts values from 0 to 15 (0 to F).
  s_pVpScore->pPalette[0] = 0x0000; // First color is also border color
  s_pVpScore->pPalette[1] = 0x0888; // Gray
  s_pVpScore->pPalette[2] = 0x0800; // Red - not max, a bit dark
  s_pVpScore->pPalette[3] = 0x0008; // Blue - same brightness as red

  // Sprite 0 colors (cursor)
  s_pVpScore->pPalette[17] = 0x0070; // Gray
  s_pVpScore->pPalette[18] = 0x00f0; // Red - not max, a bit dark
  s_pVpScore->pPalette[19] = 0x0bf0;

  // Sprite 1 colors (cursor)
  s_pVpScore->pPalette[20] = 0x0070; // Gray
  s_pVpScore->pPalette[21] = 0x00f0; // Red - not max, a bit dark
  s_pVpScore->pPalette[22] = 0x0bf0;

  UWORD uwColTmp;
  uwColTmp=ball2bpl16x16_frame1_palette_data[2];
  uwColTmp=uwColTmp<<8;
  uwColTmp=uwColTmp|ball2bpl16x16_frame1_palette_data[3];
  s_pVpScore->pPalette[17] = uwColTmp;
  s_pVpScore->pPalette[21] = uwColTmp;
  s_pVpScore->pPalette[25] = uwColTmp;

  uwColTmp=ball2bpl16x16_frame1_palette_data[4];
  uwColTmp=uwColTmp<<8;
  uwColTmp=uwColTmp|ball2bpl16x16_frame1_palette_data[5];
  s_pVpScore->pPalette[18] = uwColTmp;
  s_pVpScore->pPalette[22] = uwColTmp;
  s_pVpScore->pPalette[26] = uwColTmp;

  uwColTmp=ball2bpl16x16_frame1_palette_data[6];
  uwColTmp=uwColTmp<<8;
  uwColTmp=uwColTmp|ball2bpl16x16_frame1_palette_data[7];
  s_pVpScore->pPalette[19] = uwColTmp;
  s_pVpScore->pPalette[23] = uwColTmp;
  s_pVpScore->pPalette[27] = uwColTmp;

  // Sprite 2 colors (cursor)
 // s_pVpScore->pPalette[23] = 0x0070; // Gray
  /*s_pVpScore->pPalette[24] = 0x00f0; // Red - not max, a bit dark
  s_pVpScore->pPalette[25] = 0x0bf0;*/

  s_pFontUI = fontCreateFromMem((UBYTE*)uni54_data);
  if (s_pFontUI==NULL) return;

  s_pGlyph = fontCreateTextBitMap(250, s_pFontUI->uwHeight);

  copyToMainBpl(valkyrie320x244_data,0,0);                            // Screen 0 valchirie img 320px

  PRINTF(1, 0, "- Showcase Morphos (Ozzyboshi)")                     // Screen 1 empty for text
  PRINTF(1, 10,"- Showcase Vampire (DrProcton)")
  PRINTF(1, 20,"- Showcase A1000 (Z3K)")
  PRINTF(1, 40,"Prenotazioni whatsapp o sms a 333333333")
  PRINTF(1, 60,"Press A to blow wind to left")
  PRINTF(1, 70,"Press S to stop wind")
  PRINTF(1, 80,"Press D to blow wind to right")
                                          
  copyToMainBpl(vampireitalialogo_data,2,3);                          // Screen 2 vampireitalia logo 320px 8 cols

  PRINTF(3, 10,"- Programma dell'evento")                             // Screen 3 empty for text
  PRINTF(3, 30,"- Ore 11 : Ritrovo e presentazione evento")
  PRINTF(3, 40,"- Ore 12 : Showcase ferraglia e scartoffie")
  PRINTF(3, 50,"- Ore 13 : Pranzo a base di specialita toscane alla carta")
  PRINTF(3, 60,"- Ore 14 : Dimostrazione vampire V4 a cura di Dr Procton")
  PRINTF(3, 70,"- Ore 14.30 : Concerto Mistantropixel su c64+cyntcart")
  PRINTF(3, 80,"- Ore 15 : Talk su qualcosa, distribuzione gadgets")
  PRINTF(3, 90,"- Ore 16 : Foto ricordo e tutti a casa")
                                            
  copyToMainBpl(Aded320x224_1_data,4,0);                              // Screen 4 d&d 640px part one
  copyToMainBpl(Aded320x224_2_data,5,0);                              // Screen 5 d&d 640px part two
  PRINTF(6,10,"Code : Ozzyboshi")                                     // Screen 6 empty for text
  PRINTF(6,20,"Background artwork : Dr.Procton")
  PRINTF(6,30,"Ball sprites : Z3k")
  PRINTF(6,40,"Music : Stolen from Morph of dual crew")

  copyToMainBpl(valkyrie320x244_data,7,0);                            // Screen 7 valchirie img 320px

  // Init the gravity force
  g_Gravity.x=fix16_div(fix16_from_int(0),fix16_from_int(10));
  g_Gravity.y=fix16_div(fix16_from_int(1),fix16_from_int(10));

  // Init wind force
  g_Wind.x=fix16_div(fix16_from_int(0),fix16_from_int(10));
  g_Wind.y=fix16_div(fix16_from_int(0),fix16_from_int(10));
  g_WindStep=fix16_div(fix16_from_int(1),fix16_from_int(30));

  fontDrawStr(s_pScoreBuffer->pBack, s_pFontUI, 10,10,"                                                                   Vampireitalia community invites you to CASENTINO DAY 2020 - LOCALITA BADIA PRATAGLIA (AR) - 10/10/2020 - GPS: XXXX XXXX XXXX", 1, FONT_LAZY );
  
  // we need a camera to simulate scrolling
  s_pCamera = s_pScoreBuffer->pCamera;
  s_pCameraMain = s_pMainBuffer->pCamera;
  //cameraMoveBy(s_pCamera, 1, 0);

  // We don't need anything from OS anymore
  systemUnuse();

if (1)
{
  myBlock = copBlockCreate(s_pView->pCopList, 15, 0, 76);
  for (UBYTE ubCounter=1;ubCounter<16;ubCounter++)
  { 
    //copMove(s_pView->pCopList, myBlock,&g_pCustom->color[ubCounter],Valkyrie_data_colors[ubCounter] );
    UWORD uwColTmp;
    uwColTmp=valkyrie320x244palette_data[ubCounter*2];
    uwColTmp=uwColTmp<<8;
    uwColTmp=uwColTmp|valkyrie320x244palette_data[ubCounter*2+1];
    copMove(s_pView->pCopList, myBlock,&g_pCustom->color[ubCounter],uwColTmp );
  }

  tCopBlock *myBlock2 = copBlockCreate(s_pView->pCopList, 3, 0, 300);
  //copMove(s_pView->pCopList, myBlock2,&g_pCustom->color[0],0x0000 );
  copMove(s_pView->pCopList, myBlock2,&g_pCustom->color[1],0x0888 );
  copMove(s_pView->pCopList, myBlock2,&g_pCustom->color[2],0x0800 );
  copMove(s_pView->pCopList, myBlock2,&g_pCustom->color[3],0x0008 );
}

  // Prepare ball sprite frames
  memBitmapToSprite((UBYTE*)ball2bpl16x16_frame1_data, ball2bpl16x16_frame1_size);
  memBitmapToSprite((UBYTE*)ball2bpl16x16_frame2_data, ball2bpl16x16_frame2_size);
  memBitmapToSprite((UBYTE*)ball2bpl16x16_frame3_data, ball2bpl16x16_frame3_size);
  memBitmapToSprite((UBYTE*)ball2bpl16x16_frame4_data, ball2bpl16x16_frame4_size);
  memBitmapToSprite((UBYTE*)ball2bpl32x32_frame1_1_data, ball2bpl32x32_frame1_1_size);
  memBitmapToSprite((UBYTE*)ball2bpl32x32_frame1_2_data, ball2bpl32x32_frame1_2_size);
  memBitmapToSprite((UBYTE*)ball2bpl32x32_frame2_1_data, ball2bpl32x32_frame2_1_size);
  memBitmapToSprite((UBYTE*)ball2bpl32x32_frame2_2_data, ball2bpl32x32_frame2_2_size);
  memBitmapToSprite((UBYTE*)ball2bpl32x32_frame3_1_data, ball2bpl32x32_frame3_1_size);
  memBitmapToSprite((UBYTE*)ball2bpl32x32_frame3_2_data, ball2bpl32x32_frame3_2_size);
  memBitmapToSprite((UBYTE*)ball2bpl32x32_frame4_1_data, ball2bpl32x32_frame4_1_size);
  memBitmapToSprite((UBYTE*)ball2bpl32x32_frame4_2_data, ball2bpl32x32_frame4_2_size);

  // Load the view
  viewLoad(s_pView);

//#ifdef SOUND  
  mt_init(g_tPapercutMod_data);
//#endif

  spriteVectorInit(&g_Sprite1Vector,1,125,115,0,0,LITTLE_BALLS_MASS);
  copBlockEnableSpriteFull(s_pView->pCopList, 1, (UBYTE*)ball2bpl16x16_frame1_data,sizeof(ball2bpl16x16_frame1_data));
  memcpy(s_pAceSprites[1].pSpriteData+4,ball2bpl16x16_frame1_data,ball2bpl16x16_frame1_size);

  spriteVectorInit(&g_Sprite2Vector,2,105,95,0,0,LITTLE_BALLS_MASS);
  copBlockEnableSpriteFull(s_pView->pCopList, 2, (UBYTE*)ball2bpl16x16_frame1_data,sizeof(ball2bpl16x16_frame1_data));
  memcpy(s_pAceSprites[2].pSpriteData+4,ball2bpl16x16_frame1_data,ball2bpl16x16_frame1_size);

  spriteVectorInit(&g_Sprite3Vector,3,75,75,0,0,LITTLE_BALLS_MASS);
  copBlockEnableSpriteFull(s_pView->pCopList, 3, (UBYTE*)ball2bpl16x16_frame1_data,sizeof(ball2bpl16x16_frame1_data));

  //memcpy(s_pAceSprites[3].pSpriteData+4,ball2bpl16x16_frame1_data,ball2bpl16x16_frame1_size);

  spriteVectorInit(&g_Sprite4Vector,4,BIG_BALL_START_POSITION_X,85,0,0,BIG_BALL_MASS);
  copBlockEnableSpriteFull(s_pView->pCopList, 4, (UBYTE*)ball2bpl32x32_frame1_1_data,sizeof(ball2bpl32x32_frame1_1_data));
  //memcpy(s_pAceSprites[4].pSpriteData+4,ball2bpl32x32_frame1_1_data,sizeof(ball2bpl32x32_frame1_1_data));

  //spriteVectorInit(&g_Sprite5Vector,5,BIG_BALL_START_POSITION_X+16,85,0,0,5);
  copBlockEnableSpriteFull(s_pView->pCopList, 5, (UBYTE*)ball2bpl32x32_frame1_2_data,sizeof(ball2bpl32x32_frame1_2_data));
  //memcpy(s_pAceSprites[5].pSpriteData+4,ball2bpl32x32_frame1_2_data,sizeof(ball2bpl32x32_frame1_2_data));
  SetTrailingSprite(5,4);

  memcpy(s_pAceSprites[4].pSpriteData+4,ball2bpl32x32_frame1_1_data,sizeof(ball2bpl32x32_frame1_1_data));
  memcpy(s_pAceSprites[5].pSpriteData+4,ball2bpl32x32_frame1_2_data,sizeof(ball2bpl32x32_frame1_2_data));

  copProcessBlocks();
}

void gameGsLoop(void) {
  static BYTE bSpriteCounter = 0;
  static BYTE bSpriteDirection = 0 ;
#ifdef COLORDEBUG
  g_pCustom->color[0] = 0x0FFF; // white
#endif

//#ifdef SOUND
  mt_music();
//#endif

 // g_pCustom->color[0] = 0x0FF;
  
    //g_pCustom->color[0] = 0xf00;

  // This will loop forever until you "pop" or change gamestate
  // or close the game
  UBYTE isAnyPressed = (
    keyUse(KEY_RETURN) | keyUse(KEY_ESCAPE) |
    joyUse(JOY1 + JOY_FIRE) | joyUse(JOY2 + JOY_FIRE)
  );
  if(isAnyPressed) {
    gameClose();
    return ;
  }
  else if (keyUse(KEY_D))
  {
    if (fix16_abs(g_Wind.x)<fix16_from_int(WIND_MAX_STEP))
      g_Wind.x=fix16_add(g_Wind.x,g_WindStep);
    if (g_Wind.x>0) bSpriteDirection=1;
    else if (g_Wind.x==0) bSpriteDirection=0;
    else if (g_Wind.x<0) bSpriteDirection=-1;
  }
  else if (keyUse(KEY_A))
  {
    if (fix16_abs(g_Wind.x)<fix16_from_int(WIND_MAX_STEP))
      g_Wind.x=fix16_sub(g_Wind.x,g_WindStep);
    if (g_Wind.x>0) bSpriteDirection=1;
    else if (g_Wind.x==0) bSpriteDirection=0;
    else if (g_Wind.x<0) bSpriteDirection=-1;
  }
  else if (keyUse(KEY_S))
  {
    g_Wind.x=0;
    bSpriteDirection=0;
  }
  else 
  {
#ifdef COLORDEBUG
  g_pCustom->color[0] = 0x0F00;
#endif

    // animate sprites
    if (bSpriteCounter==0)         memcpy(s_pAceSprites[3].pSpriteData+4,ball2bpl16x16_frame1_data,ball2bpl16x16_frame1_size);
    else if (bSpriteCounter==1)    memcpy(s_pAceSprites[2].pSpriteData+4,ball2bpl16x16_frame1_data,ball2bpl16x16_frame1_size);
    else if (bSpriteCounter==2)    memcpy(s_pAceSprites[1].pSpriteData+4,ball2bpl16x16_frame1_data,ball2bpl16x16_frame1_size);
    else if (bSpriteCounter==3)
    {
      memcpy(s_pAceSprites[4].pSpriteData+4,ball2bpl32x32_frame1_1_data,sizeof(ball2bpl32x32_frame1_1_data));
      memcpy(s_pAceSprites[5].pSpriteData+4,ball2bpl32x32_frame1_2_data,sizeof(ball2bpl32x32_frame1_2_data));
    }
    
    else if (bSpriteCounter==20) memcpy(s_pAceSprites[3].pSpriteData+4,ball2bpl16x16_frame2_data,ball2bpl16x16_frame2_size);
    else if (bSpriteCounter==21) memcpy(s_pAceSprites[2].pSpriteData+4,ball2bpl16x16_frame2_data,ball2bpl16x16_frame2_size);
    else if (bSpriteCounter==22)    memcpy(s_pAceSprites[1].pSpriteData+4,ball2bpl16x16_frame2_data,ball2bpl16x16_frame2_size);
    else if (bSpriteCounter==23)
    {
      memcpy(s_pAceSprites[4].pSpriteData+4,ball2bpl32x32_frame2_1_data,sizeof(ball2bpl32x32_frame2_1_data));
      memcpy(s_pAceSprites[5].pSpriteData+4,ball2bpl32x32_frame2_2_data,sizeof(ball2bpl32x32_frame2_2_data));
    }
    
    else if (bSpriteCounter==40) memcpy(s_pAceSprites[3].pSpriteData+4,ball2bpl16x16_frame3_data,ball2bpl16x16_frame3_size);
    else if (bSpriteCounter==41)    memcpy(s_pAceSprites[2].pSpriteData+4,ball2bpl16x16_frame3_data,ball2bpl16x16_frame3_size);
    else if (bSpriteCounter==42)    memcpy(s_pAceSprites[1].pSpriteData+4,ball2bpl16x16_frame3_data,ball2bpl16x16_frame3_size);
    else if (bSpriteCounter==43)
    {
      memcpy(s_pAceSprites[4].pSpriteData+4,ball2bpl32x32_frame3_1_data,sizeof(ball2bpl32x32_frame3_1_data));
      memcpy(s_pAceSprites[5].pSpriteData+4,ball2bpl32x32_frame3_2_data,sizeof(ball2bpl32x32_frame3_2_data));
    }

    else if (bSpriteCounter==60) memcpy(s_pAceSprites[3].pSpriteData+4,ball2bpl16x16_frame4_data,ball2bpl16x16_frame4_size);
    else if (bSpriteCounter==61)    memcpy(s_pAceSprites[2].pSpriteData+4,ball2bpl16x16_frame4_data,ball2bpl16x16_frame4_size);
    else if (bSpriteCounter==62)    memcpy(s_pAceSprites[1].pSpriteData+4,ball2bpl16x16_frame4_data,ball2bpl16x16_frame4_size);
    else if (bSpriteCounter==63)
    {
      memcpy(s_pAceSprites[4].pSpriteData+4,ball2bpl32x32_frame4_1_data,sizeof(ball2bpl32x32_frame4_1_data));
      memcpy(s_pAceSprites[5].pSpriteData+4,ball2bpl32x32_frame4_2_data,sizeof(ball2bpl32x32_frame4_2_data));
    }

    //else if (ubSpriteCounter==80) memcpy(s_pAceSprites[3].pSpriteData+4,ball2bpl16x16_frame5_data,ball2bpl16x16_frame5_size);
    bSpriteCounter+=bSpriteDirection;
    if (bSpriteCounter>80) bSpriteCounter=0;
    else if (bSpriteCounter<0) bSpriteCounter=80;

#ifdef COLORDEBUG
  g_pCustom->color[0] = 0x00F0;
#endif

    UBYTE process=0;
    process+=moveCameraByFraction(s_pCamera, 1, 0 ,(const UBYTE)2,0);
    process+=moveCameraByFraction(s_pCameraMain, 1, 0 ,(const UBYTE)3,1);
    if (s_pCamera->uPos.uwX>=1000)
    {
      cameraSetCoord(s_pCamera,0,0);
      process++;
    }

    // New cycle of image starts
    if (s_pCameraMain->uPos.uwX>=g_uwResetScroll)
    {
      /*spriteVectorInit(&g_Sprite1Vector,1,125,115,0,0,5);
      spriteVectorInit(&g_Sprite2Vector,2,105,95,0,0,2);
      spriteVectorInit(&g_Sprite3Vector,3,75,75,0,0,2);
      spriteVectorInit(&g_Sprite4Vector,4,BIG_BALL_START_POSITION_X,85,0,0,5);
      g_ubVBounceEnabled=1;*/
      nextStage();
      cameraSetCoord(s_pCameraMain,0,0);
      process++;
    }

    // After first image change palette for vampire  italia logo
    if (s_pCameraMain->uPos.uwX==320)
    {
      changeCopperColor(s_pView,myBlock,vampireitaliapalette_data,1,0);
      changeCopperColor(s_pView,myBlock,vampireitaliapalette_data,2,1);
      changeCopperColor(s_pView,myBlock,vampireitaliapalette_data,3,2);
      changeCopperColor(s_pView,myBlock,vampireitaliapalette_data,4,3);
      changeCopperColor(s_pView,myBlock,vampireitaliapalette_data,5,4);
      changeCopperColor(s_pView,myBlock,vampireitaliapalette_data,6,5);
      changeCopperColor(s_pView,myBlock,vampireitaliapalette_data,7,6);
      changeCopperColor(s_pView,myBlock,vampireitaliapalette_data,8,7);
    }
    // D&D palette
    else if (s_pCameraMain->uPos.uwX==960)
    {
      changeCopperColor(s_pView,myBlock,Aded320x224palette_data,1,0);
      changeCopperColor(s_pView,myBlock,Aded320x224palette_data,2,1);
      changeCopperColor(s_pView,myBlock,Aded320x224palette_data,3,2);
      changeCopperColor(s_pView,myBlock,Aded320x224palette_data,4,3);
      changeCopperColor(s_pView,myBlock,Aded320x224palette_data,5,4);
      changeCopperColor(s_pView,myBlock,Aded320x224palette_data,6,5);
      changeCopperColor(s_pView,myBlock,Aded320x224palette_data,7,6);
      changeCopperColor(s_pView,myBlock,Aded320x224palette_data,8,7);
      changeCopperColor(s_pView,myBlock,Aded320x224palette_data,9,8);
      changeCopperColor(s_pView,myBlock,Aded320x224palette_data,10,9);
      changeCopperColor(s_pView,myBlock,Aded320x224palette_data,11,10);
      changeCopperColor(s_pView,myBlock,Aded320x224palette_data,12,11);
      changeCopperColor(s_pView,myBlock,Aded320x224palette_data,13,12);
      changeCopperColor(s_pView,myBlock,Aded320x224palette_data,14,13);
      changeCopperColor(s_pView,myBlock,Aded320x224palette_data,15,14);
    }

    else if (s_pCameraMain->uPos.uwX==1920)
    {
      changeCopperColor(s_pView,myBlock,valkyrie320x244palette_data,1,0);
      changeCopperColor(s_pView,myBlock,valkyrie320x244palette_data,2,1);
      changeCopperColor(s_pView,myBlock,valkyrie320x244palette_data,3,2);
      changeCopperColor(s_pView,myBlock,valkyrie320x244palette_data,4,3);
      changeCopperColor(s_pView,myBlock,valkyrie320x244palette_data,5,4);
      changeCopperColor(s_pView,myBlock,valkyrie320x244palette_data,6,5);
      changeCopperColor(s_pView,myBlock,valkyrie320x244palette_data,7,6);
      changeCopperColor(s_pView,myBlock,valkyrie320x244palette_data,8,7);
      changeCopperColor(s_pView,myBlock,valkyrie320x244palette_data,9,8);
      changeCopperColor(s_pView,myBlock,valkyrie320x244palette_data,10,9);
      changeCopperColor(s_pView,myBlock,valkyrie320x244palette_data,11,10);
      changeCopperColor(s_pView,myBlock,valkyrie320x244palette_data,12,11);
      changeCopperColor(s_pView,myBlock,valkyrie320x244palette_data,13,12);
      changeCopperColor(s_pView,myBlock,valkyrie320x244palette_data,14,13);
      changeCopperColor(s_pView,myBlock,valkyrie320x244palette_data,15,14);

      g_ubVBounceEnabled=0;
    }

#ifdef COLORDEBUG
  g_pCustom->color[0] = 0x00F0;
#endif
    s_pStagesFunctions[g_ubStageIndex].g_pStageFunction ();
#ifdef COLORDEBUG
  g_pCustom->color[0] = 0x000F;
#endif

    process++;

    if (process)
    {
      viewProcessManagers(s_pView);
      copProcessBlocks();
    }

#ifdef COLORDEBUG
    g_pCustom->color[0] = 0x0000;
#endif
    
    //viewUpdateCLUT(s_pView);
    vPortWaitForEnd(s_pVpMain);
  }
}

void gameGsDestroy(void) {
  // Cleanup when leaving this gamestate
  systemUse();

  // This will also destroy all associated viewports and viewport managers
  viewDestroy(s_pView);
}

// Function to copy data to a main bitplane
// Pass ubMaxBitplanes = 0 to use all available bitplanes in the bitmap
void copyToMainBpl(const unsigned char* pData,const UBYTE ubSlot,const UBYTE ubMaxBitplanes)
{
  const UWORD dmod = g_uwBlitModd;
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
    g_pCustom->bltdmod = dmod;
    g_pCustom->bltapt = (UBYTE*)((ULONG)&pData[40*224*ubBitplaneCounter]);
    g_pCustom->bltdpt = (UBYTE*)((ULONG)s_pMainBuffer->pBack->Planes[ubBitplaneCounter]+(40*ubSlot));
    g_pCustom->bltsize = 0x3814;
    if (ubMaxBitplanes>0 && ubBitplaneCounter+1>=ubMaxBitplanes) return ;
  }
  return ;
}

/*void bounce()
{
  int x=fix16_to_int(location.x);
  int y=fix16_to_int(location.y);

  if (x>300||x<10)
    velocity.x=fix16_mul(velocity.x,fix16_from_int(-1));

  if (y>220||y<10)
    velocity.y=fix16_mul(velocity.y,fix16_from_int(-1));
}*/

/*void reposition()
{
  int x=fix16_to_int(location2.x);
  int y=fix16_to_int(location2.y);

  if (x>300||x<10)
    location2.x=fix16_from_int(10);

  if (y>220||y<10)
    location2.y=fix16_from_int(10);
}*/

tCopBlock *copBlockEnableSpriteFull(tCopList *pList, FUBYTE fubSpriteIndex, UBYTE* pSpriteData,ULONG ulSpriteSize) {
  static tCopBlock *pBlockSprites=NULL;
  tCopMoveCmd * pMoveCmd=NULL;
  
  if (pBlockSprites == NULL) {
    pBlockSprites = copBlockDisableSprites(pList, 0xFF);
    systemSetDma(DMAB_SPRITE, 1);

    // Reset tAceSprite array
    logWrite("copBlockEnableSprite - resetting all sprites\n");
    for (UBYTE ubIterator=0;ubIterator<ACE_MAXSPRITES;ubIterator++)
    {
      //memset(&s_pAceSprites[ubIterator],0,sizeof(tAceSprite));
      s_pAceSprites[ubIterator].pSpriteData = NULL;
      s_pAceSprites[ubIterator].ulSpriteSize = 0;
      s_pAceSprites[ubIterator].bTrailingSpriteIndex = -1;
      s_pAceSprites[ubIterator].uwSpriteHeight = 0;
      s_pAceSprites[ubIterator].uwSpriteCenter = 0;
      s_pAceSprites[ubIterator].iBounceBottomLimit = 0;
      s_pAceSprites[ubIterator].iBounceRightLimit = 0;
    }
  }

  if (s_pAceSprites[fubSpriteIndex].pSpriteData)
  {
    FreeMem(s_pAceSprites[fubSpriteIndex].pSpriteData,s_pAceSprites[fubSpriteIndex].ulSpriteSize);
    s_pAceSprites[fubSpriteIndex].pSpriteData = NULL;
    s_pAceSprites[fubSpriteIndex].ulSpriteSize = 0;  
    s_pAceSprites[fubSpriteIndex].bTrailingSpriteIndex = -1;
    s_pAceSprites[fubSpriteIndex].uwSpriteHeight = 0;
    s_pAceSprites[fubSpriteIndex].uwSpriteCenter = 0;
    s_pAceSprites[fubSpriteIndex].iBounceBottomLimit = 0;
    s_pAceSprites[fubSpriteIndex].iBounceRightLimit = 0;
  }

  // Bounce limits
  s_pAceSprites[fubSpriteIndex].iBounceBottomLimit = 255-ulSpriteSize/4;
  s_pAceSprites[fubSpriteIndex].iBounceRightLimit = 319-16;

  s_pAceSprites[fubSpriteIndex].uwSpriteHeight = ulSpriteSize/4;
  s_pAceSprites[fubSpriteIndex].uwSpriteCenter = 8;

  //Make some room for sprite extra information
  ulSpriteSize+=8;
   
  logWrite("copBlockEnableSprite - Allocate : %d bytes for sprite %d\n",ulSpriteSize,fubSpriteIndex);
  s_pAceSprites[fubSpriteIndex].pSpriteData = (UBYTE*)AllocMem(ulSpriteSize,MEMF_CHIP);
  memset (s_pAceSprites[fubSpriteIndex].pSpriteData,0,ulSpriteSize);
  logWrite("copBlockEnableSprite - Allocated mem : %x\n",s_pAceSprites[fubSpriteIndex].pSpriteData);
  
  const UBYTE ubVStart=0x30;
  const UBYTE ubHStart=0x90;
  s_pAceSprites[fubSpriteIndex].pSpriteData[0] = ubVStart; // ubVStart
  s_pAceSprites[fubSpriteIndex].pSpriteData[1] = ubHStart; // ubHstart
  s_pAceSprites[fubSpriteIndex].pSpriteData[2] = (UBYTE)((ulSpriteSize-8)/4)+ubVStart;
  s_pAceSprites[fubSpriteIndex].pSpriteData[3] = 0x00;

  s_pAceSprites[fubSpriteIndex].ulSpriteSize = ulSpriteSize-8;


  // For each line of the sprite
  UWORD ubImgOffset=0;
  UWORD ubHalfOffset=(UWORD)((ulSpriteSize-8)/2);
  /*if (fubSpriteIndex>0)
  {*/
  for (UWORD ubImgCount=0;0 && ubImgCount< (UWORD)((ulSpriteSize-8)/4);ubImgCount++)
  {
    // First two bytes from first bitplane
    memcpy(s_pAceSprites[fubSpriteIndex].pSpriteData+4+ubImgOffset,pSpriteData+ubImgCount*2,2);
    ubImgOffset+=2;

    // Other two bytes from second bitplane
    memcpy(s_pAceSprites[fubSpriteIndex].pSpriteData+4+ubImgOffset,pSpriteData+ubHalfOffset+ubImgCount*2,2);
    ubImgOffset+=2;
  }
  /*}*/

  //memcpy(s_pAceSprites[fubSpriteIndex].pSpriteData+4,pSpriteData,ulSpriteSize);

  // Terminator
  s_pAceSprites[fubSpriteIndex].pSpriteData[ulSpriteSize-1] = 0x00;
  s_pAceSprites[fubSpriteIndex].pSpriteData[ulSpriteSize-2] = 0x00;
  s_pAceSprites[fubSpriteIndex].pSpriteData[ulSpriteSize-3] = 0x00;
  s_pAceSprites[fubSpriteIndex].pSpriteData[ulSpriteSize-4] = 0x00;

  //ULONG ulAddr = (ULONG)pSpriteData;
  ULONG ulAddr = (ULONG)s_pAceSprites[fubSpriteIndex].pSpriteData;

  /*copMove(pList, pBlockSprites, &g_pSprFetch[fubSpriteIndex].uwHi, ulAddr >> 16);
  copMove(pList, pBlockSprites, &g_pSprFetch[fubSpriteIndex].uwLo , ulAddr & 0xFFFF);*/
  if (fubSpriteIndex==0)
  {
    pMoveCmd = (tCopMoveCmd*)&pBlockSprites->pCmds[0];
    pMoveCmd->bfValue=ulAddr >> 16;
    logWrite("move command : %hx\n",pMoveCmd->bfDestAddr);

    pMoveCmd = (tCopMoveCmd*)&pBlockSprites->pCmds[1];
    pMoveCmd->bfValue=ulAddr & 0xFFFF;
    logWrite("move command : %hx\n",pMoveCmd->bfDestAddr);
  }

  // Start of sprite 1
  if (fubSpriteIndex==1)
  {
    pMoveCmd = (tCopMoveCmd*)&pBlockSprites->pCmds[2];
    //logWrite("setting Sprite 1 address to %x\n",s_pAceSprites[fubSpriteIndex].pSpriteData);
    //logWrite("value: %x\n",*s_pAceSprites[fubSpriteIndex].pSpriteData+0);
    //logWrite("value: %x\n",*s_pAceSprites[fubSpriteIndex].pSpriteData+1);
    pMoveCmd->bfValue=ulAddr >> 16;
    //logWrite("move command : %hx\n",pMoveCmd->bfDestAddr);

    pMoveCmd = (tCopMoveCmd*)&pBlockSprites->pCmds[3];
    pMoveCmd->bfValue=ulAddr & 0xFFFF;
    //logWrite("move command : %hx\n",pMoveCmd->bfDestAddr);
    //return NULL;
  // end of sprite 1
  }
   
  if (fubSpriteIndex==2)
  {
    // Start of sprite 2

    pMoveCmd = (tCopMoveCmd*)&pBlockSprites->pCmds[4];
    pMoveCmd->bfValue=ulAddr >> 16;
    //logWrite("move command : %hx\n",pMoveCmd->bfDestAddr);

    pMoveCmd = (tCopMoveCmd*)&pBlockSprites->pCmds[5];
    pMoveCmd->bfValue=ulAddr & 0xFFFF;
    //logWrite("move command : %hx\n",pMoveCmd->bfDestAddr);
  }

  if (fubSpriteIndex==3)
  {
    // Start of sprite 3

    pMoveCmd = (tCopMoveCmd*)&pBlockSprites->pCmds[6];
    pMoveCmd->bfValue=ulAddr >> 16;
    //logWrite("move command : %hx\n",pMoveCmd->bfDestAddr);

    pMoveCmd = (tCopMoveCmd*)&pBlockSprites->pCmds[7];
    pMoveCmd->bfValue=ulAddr & 0xFFFF;
    //logWrite("move command : %hx\n",pMoveCmd->bfDestAddr);
  }

  if (fubSpriteIndex==4)
  {
    // Start of sprite 4

    pMoveCmd = (tCopMoveCmd*)&pBlockSprites->pCmds[8];
    pMoveCmd->bfValue=ulAddr >> 16;
    //logWrite("move command : %hx\n",pMoveCmd->bfDestAddr);

    pMoveCmd = (tCopMoveCmd*)&pBlockSprites->pCmds[9];
    pMoveCmd->bfValue=ulAddr & 0xFFFF;
    //logWrite("move command : %hx\n",pMoveCmd->bfDestAddr);
  }

  if (fubSpriteIndex==5)
  {
    // Start of sprite 4

    pMoveCmd = (tCopMoveCmd*)&pBlockSprites->pCmds[10];
    pMoveCmd->bfValue=ulAddr >> 16;
    //logWrite("move command : %hx\n",pMoveCmd->bfDestAddr);

    pMoveCmd = (tCopMoveCmd*)&pBlockSprites->pCmds[11];
    pMoveCmd->bfValue=ulAddr & 0xFFFF;
    //logWrite("move command : %hx\n",pMoveCmd->bfDestAddr);
  }

  //SETSPRITEIMG(fubSpriteIndex,pSpriteData,ulSpriteSize)

  return NULL;
}






#if 0
void moverBounce(tMover* pMover)
{
  //char buf[100];
  int x=fix16_to_int(pMover->tLocation.x);
  int y=fix16_to_int(pMover->tLocation.y);

  //if (g_ubHBounceEnabled && (x>300||x<10))
  if (g_ubHBounceEnabled && (x>s_pAceSprites[pMover->ubSpriteIndex].iBounceRightLimit||x<=0))
  {
    pMover->tVelocity.x=fix16_mul(pMover->tVelocity.x,fix16_from_int(-1));

     // to do not stop bounce add
    //pMover->tVelocity.y=fix16_sub(pMover->tVelocity.y,pMover->tAccelleration.y);
    pMover->tVelocity.x=fix16_sub(pMover->tVelocity.x,pMover->tAccelleration.x);
  }

  if (g_ubVBounceEnabled && (y>s_pAceSprites[pMover->ubSpriteIndex].iBounceBottomLimit||y<BOUNCE_TOP_LIMIT))
  {
    pMover->tVelocity.y=fix16_mul(pMover->tVelocity.y,fix16_from_int(-1));
    
    // to do not stop bounce add
    pMover->tVelocity.y=fix16_sub(pMover->tVelocity.y,pMover->tAccelleration.y);
    //pMover->tVelocity.x=fix16_sub(pMover->tVelocity.x,pMover->tAccelleration.x);
    
    /*fix16_to_str(pMover->tVelocity.y,buf,6);
    logWrite("Low bounce restarts at %s\n",buf);*/
  }
}
#endif

void nextStage()
{
  g_ubStageIndex++;
  if (g_ubStageIndex==MAXSTAGES) g_ubStageIndex=0;
  s_pStagesFunctions[g_ubStageIndex].g_pPreStageFunction ();
}