#include <time.h>
#include "acecustom.h"

tCopBlock *pBlockSprites=NULL;

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
  if (uwYCoordinate>=224) return 0;
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

  if (uwYCoordinate>=224) return 0;


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
  y1=y+s_pAceSprites[fubSpriteIndex].uwSpriteHeight;

  UBYTE * vStart=s_pAceSprites[fubSpriteIndex].pSpriteData+0;
  UBYTE * hStart=s_pAceSprites[fubSpriteIndex].pSpriteData+1;
  UBYTE * vStop=s_pAceSprites[fubSpriteIndex].pSpriteData+2; 
  UBYTE * ctrlByte =s_pAceSprites[fubSpriteIndex].pSpriteData+3;

  UBYTE oldCtrlByteVal=*ctrlByte;
  UWORD spritePosTmp = x >> 1;   
  *hStart=(UBYTE)spritePosTmp & 0xFF;

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

tCopBlock *copBlockEnableSpriteFull(tCopList *pList, FUBYTE fubSpriteIndex, UBYTE* pSpriteData,ULONG ulSpriteSize) {
  //static tCopBlock *pBlockSprites=NULL;
  tCopMoveCmd * pMoveCmd=NULL;
  
  if (pBlockSprites == NULL) {
    pBlockSprites = copBlockDisableSprites(pList, 0xFF);
    systemSetDma(DMAB_SPRITE, 1);

    // Reset tAceSprite array
    for (UBYTE ubIterator=0;ubIterator<ACE_MAXSPRITES;ubIterator++)
    {
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
   
  s_pAceSprites[fubSpriteIndex].pSpriteData = (UBYTE*)AllocMem(ulSpriteSize,MEMF_CHIP);
  memset (s_pAceSprites[fubSpriteIndex].pSpriteData,0,ulSpriteSize);
  
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
  
  for (UWORD ubImgCount=0;0 && ubImgCount< (UWORD)((ulSpriteSize-8)/4);ubImgCount++)
  {
    // First two bytes from first bitplane
    memcpy(s_pAceSprites[fubSpriteIndex].pSpriteData+4+ubImgOffset,pSpriteData+ubImgCount*2,2);
    ubImgOffset+=2;

    // Other two bytes from second bitplane
    memcpy(s_pAceSprites[fubSpriteIndex].pSpriteData+4+ubImgOffset,pSpriteData+ubHalfOffset+ubImgCount*2,2);
    ubImgOffset+=2;
  }


  // Terminator
  s_pAceSprites[fubSpriteIndex].pSpriteData[ulSpriteSize-1] = 0x00;
  s_pAceSprites[fubSpriteIndex].pSpriteData[ulSpriteSize-2] = 0x00;
  s_pAceSprites[fubSpriteIndex].pSpriteData[ulSpriteSize-3] = 0x00;
  s_pAceSprites[fubSpriteIndex].pSpriteData[ulSpriteSize-4] = 0x00;

  //ULONG ulAddr = (ULONG)pSpriteData;
  ULONG ulAddr = (ULONG)s_pAceSprites[fubSpriteIndex].pSpriteData;

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
    pMoveCmd->bfValue=ulAddr >> 16;

    pMoveCmd = (tCopMoveCmd*)&pBlockSprites->pCmds[3];
    pMoveCmd->bfValue=ulAddr & 0xFFFF;
  // end of sprite 1
  }
   
  if (fubSpriteIndex==2)
  {
    // Start of sprite 2

    pMoveCmd = (tCopMoveCmd*)&pBlockSprites->pCmds[4];
    pMoveCmd->bfValue=ulAddr >> 16;

    pMoveCmd = (tCopMoveCmd*)&pBlockSprites->pCmds[5];
    pMoveCmd->bfValue=ulAddr & 0xFFFF;
  }

  if (fubSpriteIndex==3)
  {
    // Start of sprite 3

    pMoveCmd = (tCopMoveCmd*)&pBlockSprites->pCmds[6];
    pMoveCmd->bfValue=ulAddr >> 16;

    pMoveCmd = (tCopMoveCmd*)&pBlockSprites->pCmds[7];
    pMoveCmd->bfValue=ulAddr & 0xFFFF;
  }

  if (fubSpriteIndex==4)
  {
    // Start of sprite 4

    pMoveCmd = (tCopMoveCmd*)&pBlockSprites->pCmds[8];
    pMoveCmd->bfValue=ulAddr >> 16;

    pMoveCmd = (tCopMoveCmd*)&pBlockSprites->pCmds[9];
    pMoveCmd->bfValue=ulAddr & 0xFFFF;
  }

  if (fubSpriteIndex==5)
  {
    // Start of sprite 5

    pMoveCmd = (tCopMoveCmd*)&pBlockSprites->pCmds[10];
    pMoveCmd->bfValue=ulAddr >> 16;

    pMoveCmd = (tCopMoveCmd*)&pBlockSprites->pCmds[11];
    pMoveCmd->bfValue=ulAddr & 0xFFFF;
  }

  // Start of sprite 6
  if (fubSpriteIndex==6)
  {
    pMoveCmd = (tCopMoveCmd*)&pBlockSprites->pCmds[12];
    pMoveCmd->bfValue=ulAddr >> 16;
    pMoveCmd = (tCopMoveCmd*)&pBlockSprites->pCmds[13];
    pMoveCmd->bfValue=ulAddr & 0xFFFF;
  }

  // Start of sprite 7
  if (fubSpriteIndex==7)
  {
    pMoveCmd = (tCopMoveCmd*)&pBlockSprites->pCmds[14];
    pMoveCmd->bfValue=ulAddr >> 16;
    pMoveCmd = (tCopMoveCmd*)&pBlockSprites->pCmds[15];
    pMoveCmd->bfValue=ulAddr & 0xFFFF;
  }

  //SETSPRITEIMG(fubSpriteIndex,pSpriteData,ulSpriteSize)

  return NULL;
}

UBYTE printCursorPixel5(tSimpleBufferManager* pMainBuffer,UWORD uwXCoordinate,UWORD uwYCoordinate,const UWORD uwOffset)
{
  if (uwYCoordinate>224) return 0;
  UBYTE ris=0;
  
  UBYTE* quinto = (UBYTE*)((ULONG)pMainBuffer->pBack->Planes[4]);
  quinto+=uwOffset*uwYCoordinate;

  UBYTE resto=(UBYTE)uwXCoordinate&7;
  UWORD temp=uwXCoordinate>>3;

  quinto+=temp;

  temp=~resto;
  resto=temp&7;

  // Set bit to 1 only if it is zero
  if (!(((*quinto) >> resto) & 1))
  {
    *quinto|=1UL<<resto;
    ris|=8;
  }

  return ris;
}

void custLine(tSimpleBufferManager* pMainBuffer,UWORD uwX1 ,UWORD uwY1,UWORD uwX2,UWORD uwY2)
{
  static UWORD uwOffset=40*8;
  UWORD uwStart,uwEnd,uwYLength;

  

  if (uwX1<uwX2)
  {
    uwStart = uwX1+1;
    uwEnd = uwX2-1;
  }
  else
  {
    uwStart=uwX2+1;
    uwEnd=uwX1-1;
  }

  if (uwY2>uwY1) uwYLength = uwY2-uwY1;
  else uwYLength = uwY1-uwY2;

  if (uwYLength==0) uwYLength=uwY1;

  UWORD uwXLength = uwEnd-uwStart;

  for (UWORD iCounter = uwStart; iCounter <= uwEnd; iCounter++)
  {
    UBYTE* quinto = quinto=(UBYTE*)((ULONG)pMainBuffer->pBack->Planes[4]);
    UWORD uwYCoordinate=(UWORD)uwXLength*iCounter/uwYLength;
    
    quinto+=uwOffset*uwYCoordinate;

    UBYTE resto=(UBYTE)iCounter&7;
    UWORD temp=iCounter>>3;

    quinto+=temp;

    temp=~resto;
    resto=temp&7;

    *quinto|=1UL<<resto;
  }
}

void copBlockSpritesFree()
{
  // Reset tAceSprite array
  for (UBYTE ubIterator = 0; ubIterator < ACE_MAXSPRITES; ubIterator++)
  {
    if (s_pAceSprites[ubIterator].pSpriteData)
      FreeMem(s_pAceSprites[ubIterator].pSpriteData, s_pAceSprites[ubIterator].ulSpriteSize);
  }
}

tCopBlock *copBlockEnableSpriteRecycled(tCopList *pList, FUBYTE fubSpriteIndex, UBYTE *pSpriteData, ULONG ulSpriteSize)
{
  //static tCopBlock *pBlockSprites = NULL;
  tCopMoveCmd *pMoveCmd = NULL;

  if (pBlockSprites == NULL)
  {
    pBlockSprites = copBlockDisableSprites(pList, 0xFF);
    systemSetDma(DMAB_SPRITE, 1);

    // Reset tAceSprite array
    for (UBYTE ubIterator = 0; ubIterator < ACE_MAXSPRITES; ubIterator++)
    {
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
    FreeMem(s_pAceSprites[fubSpriteIndex].pSpriteData, s_pAceSprites[fubSpriteIndex].ulSpriteSize);
    s_pAceSprites[fubSpriteIndex].pSpriteData = NULL;
    s_pAceSprites[fubSpriteIndex].ulSpriteSize = 0;
    s_pAceSprites[fubSpriteIndex].bTrailingSpriteIndex = -1;
    s_pAceSprites[fubSpriteIndex].uwSpriteHeight = 0;
    s_pAceSprites[fubSpriteIndex].uwSpriteCenter = 0;
    s_pAceSprites[fubSpriteIndex].iBounceBottomLimit = 0;
    s_pAceSprites[fubSpriteIndex].iBounceRightLimit = 0;
  }

  // Bounce limits
  s_pAceSprites[fubSpriteIndex].iBounceBottomLimit = 255 - ulSpriteSize / 4;
  s_pAceSprites[fubSpriteIndex].iBounceRightLimit = 319 - 16;

  s_pAceSprites[fubSpriteIndex].uwSpriteHeight = (ulSpriteSize - 4) / 8;
  s_pAceSprites[fubSpriteIndex].uwSpriteCenter = 8;

  s_pAceSprites[fubSpriteIndex].ulSpriteSize = ulSpriteSize;
  s_pAceSprites[fubSpriteIndex].pSpriteData = (UBYTE *)AllocMem(ulSpriteSize, MEMF_CHIP);
  memcpy(s_pAceSprites[fubSpriteIndex].pSpriteData, (void *)pSpriteData, ulSpriteSize);

  ULONG ulAddr = (ULONG)s_pAceSprites[fubSpriteIndex].pSpriteData;
  if (fubSpriteIndex < ACE_MAXSPRITES)
  {
    pMoveCmd = (tCopMoveCmd *)&pBlockSprites->pCmds[fubSpriteIndex * 2];
    pMoveCmd->bfValue = ulAddr >> 16;

    pMoveCmd = (tCopMoveCmd *)&pBlockSprites->pCmds[1 + fubSpriteIndex * 2];
    pMoveCmd->bfValue = ulAddr & 0xFFFF;
  }
  return pBlockSprites;
}

// Function to move all stars to the right
void moveStars(UBYTE ubSpriteIndex)
{
  // Move each star by 2 pixels to the right and reset at 0xFF
  UBYTE *pStarPointer = (UBYTE *)s_pAceSprites[ubSpriteIndex].pSpriteData + 1;

  UBYTE ubStarType = 0;
  for (UBYTE iStarCounter = 0; iStarCounter < s_pAceSprites[ubSpriteIndex].uwSpriteHeight; iStarCounter++, pStarPointer += 8, ubStarType++)
  {
    UBYTE *pCtrlBit = pStarPointer + 2;
    if (ubStarType > 2)
      ubStarType = 0;

    // move if the star horizontal position is 0xF0, reset the position to the beginning of the line
    if ((*pStarPointer) >= ACE_SPRITE_MAX_HPOSITION)
    {
      (*pStarPointer) = ACE_SPRITE_MIN_HPOSITION;
      (*pCtrlBit) &= ~1UL;
      continue;
    }

    // If star index is more than zero move index per second (and always even positions)
    if (ubStarType)
    {
      (*pStarPointer) += ubStarType;
      continue;
    }

    // check if bit zero of fouth byte is 1, if yes reset it and add 1 to hpos
    if (*(pCtrlBit)&1U)
    {
      (*(pCtrlBit)) &= ~1UL;
      (*pStarPointer)++;
    }
    // else just set it to move sprite only 1px to the right
    else
      (*pCtrlBit) |= 1UL;
  }
}

// Init stars sprite data with random values on the horizontal position starting from ubStarFirstVerticalPosition
void initRandStars(UBYTE *pStarData2, const UBYTE ubStarFirstVerticalPosition, const UBYTE ubStarsCount)
{
  const UBYTE ubSingleSpriteHeight = 1;
  const UBYTE ubSingleSpriteGap = 1;
  // Generate random X position from 64 to 216 (where the sprite is ON SCREEN)
  time_t t;
  srand((unsigned)time(&t));

  UWORD uwStarFirstVerticalPositionCounter = (UWORD)ubStarFirstVerticalPosition + ACE_SPRITE_MIN_VPOSITION;

  for (UBYTE ubStarCounter = 0; ubStarCounter < ubStarsCount; ubStarCounter++, pStarData2 += 8)
  {
    *pStarData2 = (UBYTE)(uwStarFirstVerticalPositionCounter & 0x00FF); // VSTART

    // Calculate HSTART
    UWORD uwRandomXPosition = (rand() % 153) + 64;
    *(pStarData2 + 1) = (UBYTE)uwRandomXPosition; // HSTART

    // Calculate VStop
    UWORD uwVStop = uwStarFirstVerticalPositionCounter + ubSingleSpriteHeight;
    *(pStarData2 + 2) = (UBYTE)(uwVStop & 0x00FF); // VStop

    UBYTE *pControl = pStarData2 + 3;

    // Set HStop least significant bit
    *(pStarData2 + 3) = (UBYTE)uwRandomXPosition & 1UL;

    // Set VStart msb
    if (uwStarFirstVerticalPositionCounter > 255)
      *pControl |= 0x04;
    else
      *pControl &= 0xFB;

    // Set VStop msb
    if (uwVStop > 255)
      *pControl |= 0x02;
    else
      *pControl &= 0xFD;

    *(pStarData2 + 4) = 0x10;
    *(pStarData2 + 5) = 0x00;
    *(pStarData2 + 6) = 0x10;
    *(pStarData2 + 7) = 0x00;

    uwStarFirstVerticalPositionCounter += (ubSingleSpriteHeight + ubSingleSpriteGap);
  }

  *(pStarData2 + 0) = 0x00;
  *(pStarData2 + 1) = 0x00;
  *(pStarData2 + 2) = 0x00;
  *(pStarData2 + 3) = 0x00;
  return;
}
