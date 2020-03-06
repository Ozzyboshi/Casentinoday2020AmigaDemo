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

#ifndef _PHYSICS_H_
#define _PHYSICS_H_

#include <stdint.h>
#include "vectors.h"
#include "acecustom.h"
#include "global.h"

typedef uint8_t  UBYTE;
typedef uint16_t UWORD;

UBYTE g_ubVBounceEnabled;
UBYTE g_ubHBounceEnabled;

//tAceSprite s_pAceSprites[8];

typedef struct _tMover
{
	uint8_t ubSpriteIndex;
	v2d tLocation;
	v2d tVelocity;
	v2d tAccelleration;

	fix16_t tMass;

	uint8_t ubLocked;

  struct Queue* tQueue;
}tMover;

inline static void moverAddVelocityToLocation(tMover* pMover)
{
  v2d_add(&pMover->tLocation,&pMover->tLocation,&pMover->tVelocity);
}

inline static void moverAddAccellerationToVelocity(tMover* pMover)
{
  v2d_add(&pMover->tVelocity,&pMover->tVelocity,&pMover->tAccelleration);
}

inline static void spriteVectorResetAccelleration(tMover* pMover)
{
  pMover->tAccelleration.x=0;
  pMover->tAccelleration.y=0;
}

inline static void spriteVectorApplyForce(tMover* pMover,v2d* pForce)
{
  // Force must be divided by mass because of Newton second law (A = F/M)
  // So the accelleration is Force/Mass og the object I am moving
  v2d tNewForce;
  v2d_div_s(&tNewForce,pForce,pMover->tMass);
  v2d_add(&pMover->tAccelleration,&pMover->tAccelleration,&tNewForce);
}

inline static void spriteVectorApplyForceToVelocity(tMover* pMover,v2d* pForce)
{
  // Force must be divided by mass because of Newton second law (A = F/M)
  // So the accelleration is Force/Mass og the object I am moving
  v2d tNewForce;
  v2d_div_s(&tNewForce,pForce,pMover->tMass);
  v2d_add(&pMover->tVelocity,&pMover->tVelocity,&tNewForce);
}

inline static void spriteVectorGetFrictionDirection(v2d pForce, v2d* dest)
{
  fix16_t mag = v2d_get_mag(&pForce);
  if (mag){

    //v2d_div_s(dest,pForce,pMover->tMass);
  
    dest->x = fix16_div(pForce.x, mag);
    dest->y = fix16_div(pForce.y, mag);
  }
  //v2d_normalize(dest, &pForce);
  //v2d_mul_s(dest,dest, fix16_to_int(-1));
}

inline static void spriteVectorGetFrictionMagnitude(v2d pForce, v2d* dest,fix16_t c)
{
  v2d_mul_s(dest,&pForce, c);
}

void moverBounce(tMover*);

UBYTE moverMove(tMover);

void spriteVectorInit(tMover* ,const UBYTE,const int, const int, const int, const int, const unsigned int);

#endif