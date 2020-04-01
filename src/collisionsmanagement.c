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

#include "collisionsmanagement.h"
#include "global.h"
#include "queues.h"
#include "acecustom.h"

static UWORD uwTextCollOnCoordsArray [] = {
                                      // C
                                      152,51,
                                      151,51,

                                      150,50,
                                      149,49,
                                      149,48,
                                      149,47,
                                      149,46,
                                      150,45,
                                      151,44,
                                      152,44,

                                      // O
                                      155,51,
                                      156,51,
                                      157,51,
                                      154,50,
                                      158,50,
                                      154,49,
                                      158,49,

                                      154,48,
                                      158,48,
                                      154,48,
                                      158,48,

                                      154,47,
                                      158,47,
                                      154,47,
                                      158,47,
                                      154,47,
                                      158,47,

                                      154,46,
                                      158,46,
                                      154,45,
                                      158,45,
                                      155,44,
                                      157,44,
                                      156,44,

                                      //L
                                      160,44,
                                      160,45,
                                      160,46,
                                      160,47,
                                      160,48,
                                      160,49,
                                      160,50,
                                      160,51,
                                      161,51,
                                      162,51,
                                      163,51,

                                      //L
                                      165,44,
                                      165,45,
                                      165,46,
                                      165,47,
                                      165,48,
                                      165,49,
                                      165,50,
                                      165,51,
                                      166,51,
                                      167,51,
                                      168,51,

                                      // O
                                      // O
                                      175,51,
                                      176,51,
                                      177,51,
                                      174,50,
                                      178,50,
                                      174,49,
                                      178,49,

                                      174,48,
                                      178,48,
                                      174,48,
                                      178,48,

                                      174,47,
                                      178,47,
                                      174,47,
                                      178,47,
                                      174,47,
                                      178,47,

                                      174,46,
                                      178,46,
                                      174,45,
                                      178,45,
                                      175,44,
                                      177,44,
                                      176,44,

                                      // N
                                      180,44,
                                      180,45,
                                      180,46,
                                      180,47,
                                      180,48,
                                      180,49,
                                      180,50,
                                      180,51,

                                      184,44,
                                      184,45,
                                      184,46,
                                      184,47,
                                      184,48,
                                      184,49,
                                      184,50,
                                      184,51,

                                      181,45,
                                      182,46,
                                      183,47,
                                      184,48,

                                      0,0};

static UWORD uwTextCollOffCoordsArray [] = {
                                      // C
                                      152,51,
                                      151,51,

                                      150,50,
                                      149,49,
                                      149,48,
                                      149,47,
                                      149,46,
                                      150,45,
                                      151,44,
                                      152,44,

                                      // O
                                      155,51,
                                      156,51,
                                      157,51,
                                      154,50,
                                      158,50,
                                      154,49,
                                      158,49,

                                      154,48,
                                      158,48,
                                      154,48,
                                      158,48,

                                      154,47,
                                      158,47,
                                      154,47,
                                      158,47,
                                      154,47,
                                      158,47,

                                      154,46,
                                      158,46,
                                      154,45,
                                      158,45,
                                      155,44,
                                      157,44,
                                      156,44,

                                      //L
                                      160,44,
                                      160,45,
                                      160,46,
                                      160,47,
                                      160,48,
                                      160,49,
                                      160,50,
                                      160,51,
                                      161,51,
                                      162,51,
                                      163,51,

                                      //L
                                      165,44,
                                      165,45,
                                      165,46,
                                      165,47,
                                      165,48,
                                      165,49,
                                      165,50,
                                      165,51,
                                      166,51,
                                      167,51,
                                      168,51,

                                      // O
                                      // O
                                      175,51,
                                      176,51,
                                      177,51,
                                      174,50,
                                      178,50,
                                      174,49,
                                      178,49,

                                      174,48,
                                      178,48,
                                      174,48,
                                      178,48,

                                      174,47,
                                      178,47,
                                      174,47,
                                      178,47,
                                      174,47,
                                      178,47,

                                      174,46,
                                      178,46,
                                      174,45,
                                      178,45,
                                      175,44,
                                      177,44,
                                      176,44,

                                      // F
                                      180,44,
                                      180,45,
                                      180,46,
                                      180,47,
                                      180,48,
                                      180,49,
                                      180,50,
                                      180,51,

                                      181,44,
                                      182,44,
                                      183,44,

                                      181,47,
                                      182,47,
                                      183,47,


                                      // F
                                      185,44,
                                      185,45,
                                      185,46,
                                      185,47,
                                      185,48,
                                      185,49,
                                      185,50,
                                      185,51,

                                      186,44,
                                      187,44,
                                      188,44,

                                      186,47,
                                      187,47,
                                      188,47,
                                      

                                      0,0};

struct Queue* tCollisionSignQueue;

void CollisionInit()
{
	tCollisionSignQueue=createQueue(150);
}

void PrintTextCollisionOn()
{
	struct MemPoint elem;

    if (isEmpty(tCollisionSignQueue))
    {
      UWORD* uwTextCoordsPtr = &uwTextCollOnCoordsArray[0];
      
      while (* uwTextCoordsPtr)
      {
        elem.x=s_pCameraMain->uPos.uwX+(UWORD)*uwTextCoordsPtr++;
        elem.y=((UWORD)*uwTextCoordsPtr++) - s_pVpScore->uwHeight;
        elem.changed = printCursorPixel(s_pMainBuffer,elem.x,elem.y,40*NUM_IMAGES);
        enqueue(tCollisionSignQueue, elem);
      }
    }
    
}

void PrintTextCollisionOff()
{
	struct MemPoint elem;

    if (isEmpty(tCollisionSignQueue))
    {
      UWORD* uwTextCoordsPtr = &uwTextCollOffCoordsArray[0];
      
      while (* uwTextCoordsPtr)
      {
        elem.x=s_pCameraMain->uPos.uwX+(UWORD)*uwTextCoordsPtr++;
        elem.y=((UWORD)*uwTextCoordsPtr++) - s_pVpScore->uwHeight;
        elem.changed = printCursorPixel(s_pMainBuffer,elem.x,elem.y,40*NUM_IMAGES);
        enqueue(tCollisionSignQueue, elem);
      }
    }
    
}

void DeleteTextCollision()
{
	struct MemPoint elem;
	while (!isEmpty(tCollisionSignQueue))
      {
        elem = dequeue(tCollisionSignQueue);
        restorePixels(s_pMainBuffer,elem,40*NUM_IMAGES);
      }
}