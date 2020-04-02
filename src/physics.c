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

#include "physics.h"

tVPort *s_pVpMain;

void moverBounce(tMover* pMover)
{
  int x=fix16_to_int(pMover->tLocation.x);
  int y=fix16_to_int(pMover->tLocation.y);

  if (g_ubHBounceEnabled && (x>s_pAceSprites[pMover->ubSpriteIndex].iBounceRightLimit||x<=0))
  {
    pMover->tVelocity.x=fix16_mul(pMover->tVelocity.x,fix16_from_int(-1));
    pMover->tVelocity.x=fix16_sub(pMover->tVelocity.x,pMover->tAccelleration.x);
    
    // if right bounce check if next iteration we exit to the right bounce zone, if not force location.x to right limit otherwise we could be stuck on the right
    if (x>s_pAceSprites[pMover->ubSpriteIndex].iBounceRightLimit)
    {
    	int exitBouncingStage = fix16_to_int(fix16_add( pMover->tLocation.x,pMover->tVelocity.x));
    	if (exitBouncingStage>s_pAceSprites[pMover->ubSpriteIndex].iBounceRightLimit)
		  pMover->tLocation.x=fix16_from_int(s_pAceSprites[pMover->ubSpriteIndex].iBounceRightLimit);
    }

    // if left bounce check if next iteration we exit to the left bounce zone, if not force location.x to left limit otherwise we could be stuck on the left
    if (x<=0)
    {
    	int exitBouncingStage = fix16_to_int(fix16_add( pMover->tLocation.x,pMover->tVelocity.x));
    	if (exitBouncingStage<=0) 
		  pMover->tLocation.x=fix16_from_int(1);
    }
  }

  if (g_ubVBounceEnabled && (y>s_pAceSprites[pMover->ubSpriteIndex].iBounceBottomLimit||y<10))
  {
    pMover->tVelocity.y=fix16_mul(pMover->tVelocity.y,fix16_from_int(-1));
    
    // to do not stop bounce add
    pMover->tVelocity.y=fix16_sub(pMover->tVelocity.y,pMover->tAccelleration.y);
  }
}


// Moves a mover and his trail , It returns 1 if move was impossible because off screen
UBYTE moverMove(tMover sMover)
{
  UWORD uwLocationX = fix16_to_int(sMover.tLocation.x);
  UWORD uwLocationY=(UWORD) fix16_to_int(sMover.tLocation.y);

  UWORD uwLocationXTrail = uwLocationX + s_pCameraMain->uPos.uwX + s_pAceSprites[sMover.ubSpriteIndex].uwSpriteCenter;
  UWORD uwLocationYTrail = uwLocationY - s_pVpScore->uwHeight + s_pAceSprites[sMover.ubSpriteIndex].uwSpriteCenter;
  
  struct MemPoint elem;
  UBYTE ubBitsChanged =0 ;
  static UWORD uwOffset = 40*NUM_IMAGES;

  if (uwLocationY>255)
  {
    // Just a random point off screen
    spriteMove3((FUBYTE)sMover.ubSpriteIndex,(UWORD) 0,(UWORD)300);
    if (!isEmpty(sMover.tQueue)) restorePixels(s_pMainBuffer,dequeue(sMover.tQueue),uwOffset);
    return 1;
  }
  spriteMove3((FUBYTE)sMover.ubSpriteIndex,uwLocationX,uwLocationY);

#ifdef TRAIL
  //if (uwLocationY>255)
  if (uwLocationYTrail>s_pVpMain->uwHeight)
    ubBitsChanged=0;
  else
  {
//	g_pCustom->color[0] = 0x0AAA;
    ubBitsChanged=printCursorPixel(s_pMainBuffer,uwLocationXTrail, uwLocationYTrail, uwOffset);
//g_pCustom->color[0] = 0x0000;
  }

  if (isFull(sMover.tQueue))
  {
     //g_pCustom->color[0] = 0x0FFF;
    elem = dequeue(sMover.tQueue);
    restorePixels(s_pMainBuffer,elem,uwOffset);
     //g_pCustom->color[0] = 0x0000;
  }

  if (uwLocationYTrail<=s_pVpMain->uwHeight)
  {
    elem.x=uwLocationXTrail;
    elem.y=uwLocationYTrail;
    elem.changed=ubBitsChanged;
    enqueue(sMover.tQueue, elem);
  }
#endif

  return 0;
}

void spriteVectorInit(tMover* pMover,const UBYTE ubSpriteIndex,const int iLx, const int iLy, const int iSx, const int iSy,const unsigned int uiMass)
{
  pMover->ubSpriteIndex=ubSpriteIndex;

  pMover->tLocation.x=fix16_from_int(iLx);
  pMover->tLocation.y=fix16_from_int(iLy);

  pMover->tVelocity.x=fix16_from_int(iSx);
  pMover->tVelocity.y=fix16_from_int(iSy);

  pMover->tAccelleration.x=fix16_from_int(0);
  pMover->tAccelleration.y=fix16_from_int(0);

  pMover->tMass=fix16_from_int((int)uiMass);

#ifdef TRAIL
  //if (pMover->tQueue) FreeMem(pMover->tQueue,TRAIL_LENGTH);
  if (pMover->tQueue==NULL) pMover->tQueue=createQueue(TRAIL_LENGTH);

#endif

  pMover->ubLocked = 0;

  pMover->tPrevLocation.x=fix16_from_int(0);
  pMover->tPrevLocation.y=fix16_from_int(0);

  /*pMover->tAccelleration.x=fix16_from_int(iAx);
  pMover->tAccelleration.y=fix16_from_int(iAy);*/
}
