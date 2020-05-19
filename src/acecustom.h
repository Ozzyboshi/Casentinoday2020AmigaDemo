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

#ifndef _ACECUSTOM_H_
#define _ACECUSTOM_H_
#include <ace/managers/system.h>
#include <ace/managers/viewport/simplebuffer.h>
#include "queues.h"
#include "physics.h"
#include "global.h"

#define FAST __attribute__((fast))

#define ACE_MAXSPRITES 8

#define ACE_SPRITE7_COLLISION_FLAG 0x8000
#define ACE_SPRITE5_COLLISION_FLAG 0x4000
#define ACE_SPRITE3_COLLISION_FLAG 0x2000
#define ACE_SPRITE1_COLLISION_FLAG 0x1000

#define ACE_IS_SPRITE_COLLIDING_1_3 g_ACE_COLLISIONS_FLAGS&0x0400
#define ACE_IS_SPRITE_COLLIDING_3_1 g_ACE_COLLISIONS_FLAGS&0x0400

#define ACE_IS_SPRITE_COLLIDING_3_2 g_ACE_COLLISIONS_FLAGS&0x1000
#define ACE_IS_SPRITE_COLLIDING_2_3 g_ACE_COLLISIONS_FLAGS&0x1000

#define ACE_IS_SPRITE_COLLIDING_3_4 g_ACE_COLLISIONS_FLAGS&0x4000
#define ACE_IS_SPRITE_COLLIDING_4_3 g_ACE_COLLISIONS_FLAGS&0x4000

#define SPRITE_REQ_BYTES(var) 8 * var + 4
#define ACE_SPRITE_MIN_VPOSITION 0x30
#define ACE_SPRITE_MIN_HPOSITION 0x40
#define ACE_SPRITE_MAX_HPOSITION 0xf0

#define ACE_SET_SPRITE_COUPLE_1_COLORS(var, var1, var2, var3) \
  var->pPalette[17] = var1;                                   \
  var->pPalette[18] = var2;                                   \
  var->pPalette[19] = var3;
#define ACE_SET_SPRITE_COUPLE_2_COLORS(var, var1, var2, var3) \
  var->pPalette[21] = var1;                                   \
  var->pPalette[22] = var2;                                   \
  var->pPalette[23] = var3;
#define ACE_SET_SPRITE_COUPLE_3_COLORS(var, var1, var2, var3) \
  var->pPalette[25] = var1;                                   \
  var->pPalette[26] = var2;                                   \
  var->pPalette[27] = var3;
#define ACE_SET_SPRITE_COUPLE_4_COLORS(var, var1, var2, var3) \
  var->pPalette[29] = var1;                                   \
  var->pPalette[30] = var2;                                   \
  var->pPalette[31] = var3;

#define ACE_SET_PLAYFIELD_PRIORITY_0 g_pCustom->bplcon2=0x0000;
#define ACE_SET_PLAYFIELD_PRIORITY_1 g_pCustom->bplcon2=0x0009;
#define ACE_SET_PLAYFIELD_PRIORITY_2 g_pCustom->bplcon2=0x0012;
#define ACE_SET_PLAYFIELD_PRIORITY_3 g_pCustom->bplcon2=0x001b;
#define ACE_SET_PLAYFIELD_PRIORITY_4 g_pCustom->bplcon2=0x0024;

typedef struct _tAceSprite {
  UBYTE* pSpriteData;
  ULONG ulSpriteSize;
  BYTE bTrailingSpriteIndex;
  UWORD uwSpriteHeight;       // Height of the sprite in pixel (ulSpriteSize/4)
  UWORD uwSpriteCenter;       // value 8 for single sprite (since a sprite is 16 bits wide) or 16 for side by side sprite

  int iBounceBottomLimit;
  int iBounceRightLimit;

} tAceSprite;
tAceSprite s_pAceSprites[ACE_MAXSPRITES];

UWORD g_ACE_COLLISIONS_FLAGS;

tCameraManager *s_pCameraMain;
tVPort *s_pVpScore;
tSimpleBufferManager *s_pMainBuffer;

tCopBlock *copBlockEnableSpriteFull(tCopList *, FUBYTE , UBYTE* , ULONG );
UBYTE moveCameraByFraction(tCameraManager *, WORD, WORD, UBYTE, const UBYTE);
UBYTE printCursorPixel(tSimpleBufferManager*,UWORD,UWORD,const UWORD);
UBYTE printCursorPixel5(tSimpleBufferManager*,UWORD,UWORD,const UWORD);
UBYTE restorePixels(tSimpleBufferManager*,struct MemPoint,const UWORD);
void copChangeMove(tCopList *, tCopBlock *, UWORD, volatile void *, UWORD);
void memBitmapToSprite(UBYTE*, const size_t);
void spriteMove3(FUBYTE,UWORD,UWORD);
void SetTrailingSprite(const FUBYTE, const FUBYTE);
void custLine(tSimpleBufferManager* ,UWORD  ,UWORD ,UWORD ,UWORD );

inline void SpriteCollisionEnable(const UWORD flags)
{
  g_pCustom->clxcon=flags;
}

inline void SpriteCollisionDisable(const UWORD flags)
{
  const UWORD flags2=~flags;
  g_pCustom->clxcon&=flags2;
}

inline void SpriteGetCollisions()
{
	g_ACE_COLLISIONS_FLAGS=g_pCustom->clxdat;
}

tCopBlock *copBlockEnableSpriteRecycled(tCopList *, FUBYTE, UBYTE *, ULONG);
void copBlockSpritesFree();
void initRandStars(UBYTE *, const UBYTE, const UBYTE);
void moveStars(UBYTE);

#endif