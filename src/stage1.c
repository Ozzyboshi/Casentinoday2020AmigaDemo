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

#include "stages.h"

#include <ace/managers/key.h>

fix16_t g_WindStep;
BYTE bSpriteDirection ;

void stage1input()
{
  if (keyUse(KEY_D))
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
  return ;
}

void stage1pre()
{
    spriteVectorInit(&g_Sprite1Vector,1,125,115,0,0,LITTLE_BALLS_MASS);
    spriteVectorInit(&g_Sprite2Vector,2,105,95,0,0,LITTLE_BALLS_MASS);
    spriteVectorInit(&g_Sprite3Vector,3,75,75,0,0,LITTLE_BALLS_MASS);
    spriteVectorInit(&g_Sprite4Vector,4,BIG_BALL_START_POSITION_X,85,0,0,BIG_BALL_MASS);
    g_ubVBounceEnabled=1;
    g_Wind.x=0;
}

void stage1()
{
	if (g_Sprite1Vector.ubLocked==0)
    {
    spriteVectorApplyForce(&g_Sprite1Vector,&g_Gravity);
    spriteVectorApplyForce(&g_Sprite1Vector,&g_Wind);
    moverAddAccellerationToVelocity(&g_Sprite1Vector);
    moverAddVelocityToLocation(&g_Sprite1Vector);
    }

    if (g_Sprite2Vector.ubLocked==0)
    {
    spriteVectorApplyForce(&g_Sprite2Vector,&g_Gravity);
    spriteVectorApplyForce(&g_Sprite2Vector,&g_Wind);
    moverAddAccellerationToVelocity(&g_Sprite2Vector);
    moverAddVelocityToLocation(&g_Sprite2Vector);
    }

    if (g_Sprite3Vector.ubLocked==0)
    {
    spriteVectorApplyForce(&g_Sprite3Vector,&g_Gravity);
    spriteVectorApplyForce(&g_Sprite3Vector,&g_Wind);
    moverAddAccellerationToVelocity(&g_Sprite3Vector);
    moverAddVelocityToLocation(&g_Sprite3Vector);
    }

    if (g_Sprite4Vector.ubLocked==0)
    {
    spriteVectorApplyForce(&g_Sprite4Vector,&g_Gravity);
    spriteVectorApplyForce(&g_Sprite4Vector,&g_Wind);
    moverAddAccellerationToVelocity(&g_Sprite4Vector);
    moverAddVelocityToLocation(&g_Sprite4Vector);
    }
    #ifdef COLORDEBUG
  g_pCustom->color[0] = 0x0F0F;
#endif

    if (moverMove(g_Sprite1Vector)) g_Sprite1Vector.ubLocked = 1;
    if (moverMove(g_Sprite2Vector)) g_Sprite2Vector.ubLocked = 1;
    if (moverMove(g_Sprite3Vector)) g_Sprite3Vector.ubLocked = 1;
    if (moverMove(g_Sprite4Vector)) g_Sprite4Vector.ubLocked = 1;

    if (g_Sprite1Vector.ubLocked==0) moverBounce(&g_Sprite1Vector);
    if (g_Sprite2Vector.ubLocked==0) moverBounce(&g_Sprite2Vector);
    if (g_Sprite3Vector.ubLocked==0) moverBounce(&g_Sprite3Vector);
    if (g_Sprite4Vector.ubLocked==0) moverBounce(&g_Sprite4Vector);
    //if (g_Sprite5Vector.ubLocked==0) moverBounce(&g_Sprite5Vector);

    if (g_Sprite1Vector.ubLocked==0) spriteVectorResetAccelleration(&g_Sprite1Vector);
    if (g_Sprite2Vector.ubLocked==0) spriteVectorResetAccelleration(&g_Sprite2Vector);
    if (g_Sprite3Vector.ubLocked==0) spriteVectorResetAccelleration(&g_Sprite3Vector);
    if (g_Sprite4Vector.ubLocked==0) spriteVectorResetAccelleration(&g_Sprite4Vector);
    //if (g_Sprite5Vector.ubLocked==0) spriteVectorResetAccelleration(&g_Sprite5Vector);
}