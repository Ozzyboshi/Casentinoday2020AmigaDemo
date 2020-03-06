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

#ifndef _CASENTINO_H_
#define _CASENTINO_H_

#include <fixmath/fix16.h>
#include "vectors.h"
#include "queues.h"
/*
typedef struct _tMover
{
	uint8_t ubSpriteIndex;
	v2d tLocation;
	v2d tVelocity;
	v2d tAccelleration;

	fix16_t tMass;

	uint8_t ubLocked;

  struct Queue* tQueue;
}tMover;*/

//tMover g_Sprite0Vector = { {0,0},{0,0},{0,0} };


#define WIND_MAX_STEP 1
#define  BOUNCE_TOP_LIMIT 10
//#define  BOUNCE_BOTTOM_LIMIT 239

void gameGsCreate(void);
void gameGsLoop(void);
void gameGsDestroy(void);

#endif // _CASENTINO_H_

