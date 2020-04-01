#include "acecustom.h"

UBYTE moveCameraByFraction(tCameraManager *pManager, WORD wDx, WORD wDy, const UBYTE ubSkip,const UBYTE ubCameraCounterIndex)
{
  static UBYTE ubCameraCountersArray[8]={0,0,0,0,0,0,0};

  if (ubCameraCounterIndex>=8) return 0;
  /*static UBYTE ubCounter1 = 0;
  static UBYTE ubCounter2 = 0;*/
  UBYTE* p_Counter=&ubCameraCountersArray[ubCameraCounterIndex];
  /*if (ubCameraIndex==1)
    p_Counter=&ubCounter1;
  else
    p_Counter=&ubCounter2;*/


  (*p_Counter)++;
  if(*p_Counter >= ubSkip) 
  {
    cameraMoveBy(pManager, wDx,wDy);
    /*viewProcessManagers(s_pView);
    copProcessBlocks();*/
    *p_Counter = 0;
    return 1;
  }
  return 0;
}

UBYTE printCursorPixel(tSimpleBufferManager* pMainBuffer,UWORD uwXCoordinate,UWORD uwYCoordinate,const UWORD uwOffset)
{
  //if (uwYCoordinate>224) gameClose();
  UBYTE ris=0;
  UBYTE* primo = (UBYTE*)((ULONG)pMainBuffer->pBack->Planes[0]);
  UBYTE* secondo = (UBYTE*)((ULONG)pMainBuffer->pBack->Planes[1]);
  UBYTE* terzo = (UBYTE*)((ULONG)pMainBuffer->pBack->Planes[2]);
  UBYTE* quarto = (UBYTE*)((ULONG)pMainBuffer->pBack->Planes[3]);
  
  primo+=uwOffset*uwYCoordinate; // Y Offset
  secondo+=uwOffset*uwYCoordinate;
  terzo+=uwOffset*uwYCoordinate; // Y Offset
  quarto+=uwOffset*uwYCoordinate;

  UBYTE resto=(UBYTE)uwXCoordinate&7;
  //UBYTE temp=uwXCoordinate>>3;
  UWORD temp=uwXCoordinate>>3;

  primo+=temp;
  secondo+=temp;
  terzo+=temp;
  quarto+=temp;

  temp=~resto;
  resto=temp&7;

  // Set bit to 1 only if it is zero
  if (!(((*primo) >> resto) & 1))
  {
    *primo|=1UL<<resto;
    ris|=1;
  }

  if (!(((*secondo) >> resto) & 1))
  {
    *secondo|=1UL<<resto;
    ris|=2;
  }

  if (!(((*terzo) >> resto) & 1))
  {
    *terzo|=1UL<<resto;
    ris|=4;
  }

  if (!(((*quarto) >> resto) & 1))
  {
    *quarto|=1UL<<resto;
    ris|=8;
  }

  return ris;
}

UBYTE restorePixels(tSimpleBufferManager* pMainBuffer,struct MemPoint elem,const UWORD uwOffset)
{
  UWORD uwXCoordinate=elem.x;
  UWORD uwYCoordinate=elem.y;
  UBYTE ris=0;
  UBYTE* primo = (UBYTE*)((ULONG)pMainBuffer->pBack->Planes[0]);
  UBYTE* secondo = (UBYTE*)((ULONG)pMainBuffer->pBack->Planes[1]);
  UBYTE* terzo = (UBYTE*)((ULONG)pMainBuffer->pBack->Planes[2]);
  UBYTE* quarto = (UBYTE*)((ULONG)pMainBuffer->pBack->Planes[3]);

  //if (uwYCoordinate>224) gameClose();


  //UWORD offset = 40*NUM_IMAGES;
  
  primo+=uwOffset*uwYCoordinate; // Y Offset
  secondo+=uwOffset*uwYCoordinate;
  terzo+=uwOffset*uwYCoordinate; // Y Offset
  quarto+=uwOffset*uwYCoordinate;

  UBYTE resto=(UBYTE)uwXCoordinate&7;
  UWORD temp=uwXCoordinate>>3;

  primo+=temp;
  secondo+=temp;
  terzo+=temp;
  quarto+=temp;

  temp=~resto;
  resto=temp&7;

  // Set bit to 1 only if it is zero
  //if (!(((*primo) >> resto) & 1))
  if (elem.changed&1)
  {
    *primo &= ~(1UL << resto);

    ris|=1;
  }

  //if (!(((*secondo) >> resto) & 1))
  if (elem.changed&2)
  {
    *secondo &= ~(1UL << resto);
    ris|=2;
  }

  //if (!(((*terzo) >> resto) & 1))
  if (elem.changed&4)
  {
    *terzo &= ~(1UL << resto);
    ris|=4;
  }

  //if (!(((*quarto) >> resto) & 1))
  if (elem.changed&8)
  {
    *quarto &= ~(1UL << resto);
    ris|=8;
  }

  return ris;
}

void copChangeMove(tCopList *pCopList, tCopBlock *pBlock,UWORD uwIndex, volatile void *pAddr, UWORD uwValue) 
{
  if (uwIndex>pBlock->uwCurrCount) return ;
  copSetMove((tCopMoveCmd*)&pBlock->pCmds[uwIndex], pAddr, uwValue);

  pBlock->ubUpdated = 2;
  pBlock->ubResized = 2;
  pCopList->ubStatus |= STATUS_UPDATE;
}

// Converts a regular bitmap (not interleaved) to sprite format to be pushed into amiga hardware sprites
// THE CONTENTS OF THE MEMORY WILL BE OVERWRITTEN
void memBitmapToSprite(UBYTE* pData, const size_t iDataLength)
{
  // Sprite data must always be even
  if (iDataLength%2) return ;

  size_t iBitplaneLength = iDataLength/2;
  unsigned char* pBuf1 = malloc (iBitplaneLength);
  memcpy(pBuf1,pData,iBitplaneLength);
  unsigned char* pBuf2 = malloc (iBitplaneLength);
  memcpy(pBuf2,pData+iBitplaneLength,iBitplaneLength);


  UWORD ubImgOffset=0;
  //UWORD ubHalfOffset=(UWORD)((ulSpriteSize-8)/2);

  // For each line of the sprite
  for (UWORD ubImgCount=0;ubImgCount< (UWORD)(iDataLength/4);ubImgCount++)
  {
    // First two bytes from first bitplane
    if (ubImgOffset<=iDataLength-2) memcpy(pData+ubImgOffset,pBuf1+ubImgCount*2,2);
    ubImgOffset+=2;

    // Other two bytes from second bitplane
    if (ubImgOffset<=iDataLength-2) memcpy(pData+ubImgOffset,pBuf2+ubImgCount*2,2);
    //memcpy(s_pAceSprites[fubSpriteIndex].pSpriteData+4+ubImgOffset,pSpriteData+ubHalfOffset+ubImgCount*2,2);
    ubImgOffset+=2;
  }
  free(pBuf1);
  free(pBuf2);
  return ;
}

void spriteMove3(FUBYTE fubSpriteIndex,UWORD x,UWORD y)
{
  if (s_pAceSprites[fubSpriteIndex].bTrailingSpriteIndex>=0)
  {
    spriteMove3(s_pAceSprites[fubSpriteIndex].bTrailingSpriteIndex, x+16, y);
  }

  x+=128;
  int y1;
  y+=0x2C;
  //y1=y+32;
  y1=y+s_pAceSprites[fubSpriteIndex].uwSpriteHeight;

  /*unsigned char* vStart = (unsigned char*)Cursor_data+0;
  unsigned char* hStart = (unsigned char*)Cursor_data+1;  
  unsigned char* vStop = (unsigned char*)Cursor_data+2;   
  unsigned char* ctrlByte = (unsigned char*)Cursor_data+3;*/

  UBYTE * vStart=s_pAceSprites[fubSpriteIndex].pSpriteData+0;
  UBYTE * hStart=s_pAceSprites[fubSpriteIndex].pSpriteData+1;
  UBYTE * vStop=s_pAceSprites[fubSpriteIndex].pSpriteData+2; 
  UBYTE * ctrlByte =s_pAceSprites[fubSpriteIndex].pSpriteData+3;

  UBYTE oldCtrlByteVal=*ctrlByte;
  UWORD spritePosTmp = x >> 1;   
  *hStart=(UBYTE)spritePosTmp & 0xFF;

  //if (x%2==1)
  if(x & 1)
    oldCtrlByteVal|= 0x01;
  else
    oldCtrlByteVal &=0xFE ;

  *vStart=(UBYTE) y & 0xFF;
  *vStop=(UBYTE) y1 & 0xFF;

  if (y>255)
    oldCtrlByteVal|= 0x04;
  else
    oldCtrlByteVal &=0xFB ;

  if (y1>255)
    oldCtrlByteVal|= 0x02;
  else
    oldCtrlByteVal &=0xFD ;
  
  *ctrlByte=oldCtrlByteVal;

  return ;
}

void SetTrailingSprite(const FUBYTE bTrailingSpriteIndex,const FUBYTE fubSpriteIndex)
{
  s_pAceSprites[fubSpriteIndex].bTrailingSpriteIndex = (BYTE)bTrailingSpriteIndex;
  s_pAceSprites[fubSpriteIndex].iBounceRightLimit-=16;
  s_pAceSprites[fubSpriteIndex].uwSpriteCenter+=8;
}


