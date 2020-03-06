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

#define ACE_MAXSPRITES 8

typedef struct _tAceSprite {
  UBYTE* pSpriteData;
  ULONG ulSpriteSize;
  BYTE bTrailingSpriteIndex;
  UWORD uwSpriteHeight;
  UWORD uwSpriteCenter;

  int iBounceBottomLimit;
  int iBounceRightLimit;

} tAceSprite;
tAceSprite s_pAceSprites[ACE_MAXSPRITES];

tCameraManager *s_pCameraMain;
tVPort *s_pVpScore;
tSimpleBufferManager *s_pMainBuffer;

UBYTE moveCameraByFraction(tCameraManager *, WORD, WORD, UBYTE, const UBYTE);
UBYTE printCursorPixel(tSimpleBufferManager*,UWORD,UWORD,const UWORD);
UBYTE restorePixels(tSimpleBufferManager*,struct MemPoint,const UWORD);
void copChangeMove(tCopList *, tCopBlock *, UWORD, volatile void *, UWORD);
void memBitmapToSprite(UBYTE*, const size_t);
void spriteMove3(FUBYTE,UWORD,UWORD);
void SetTrailingSprite(const FUBYTE, const FUBYTE);
#endif