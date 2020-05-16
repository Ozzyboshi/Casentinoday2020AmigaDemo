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

//#define STAGE1_COLORDEBUG

#define SPRITES_TO_COLLIDE ACE_SPRITE5_COLLISION_FLAG|ACE_SPRITE1_COLLISION_FLAG|ACE_SPRITE3_COLLISION_FLAG
//|ACE_SPRITE7_COLLISION_FLAG

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
  SpriteCollisionEnable(SPRITES_TO_COLLIDE);
}

void stage1()
{
  static UBYTE ubSprite3Colliding = 0;
  static UBYTE ubSprite2Colliding = 0;
  static UBYTE ubSprite1Colliding = 0;
    
  if (g_ubIsCollisionEnabled )
  {
    SpriteCollisionEnable(SPRITES_TO_COLLIDE);
    SpriteGetCollisions();

    //if ( ACE_IS_SPRITE_COLLIDING_4_3 ) gameClose();

    //if (0)
    if ( ACE_IS_SPRITE_COLLIDING_3_2 )
    {
      // Sprite 2 o r sprite 3 colliding (or both)? Get the distance
      v2d tSprite2VectorDistance;
      v2d tSprite3VectorDistance;
      v2d_sub(&tSprite2VectorDistance,&g_Sprite2Vector.tLocation,&g_Sprite4Vector.tLocation);
      v2d_sub(&tSprite3VectorDistance,&g_Sprite3Vector.tLocation,&g_Sprite4Vector.tLocation);

      // Get mag
      fix16_t tSprite2VectorDistanceMag=v2d_get_mag(&tSprite2VectorDistance);
      fix16_t tSprite2VectorDistanceMagAbs = fix16_abs(tSprite2VectorDistanceMag);

      fix16_t tSprite3VectorDistanceMag=v2d_get_mag(&tSprite3VectorDistance);
      fix16_t tSprite3VectorDistanceMagAbs = fix16_abs(tSprite3VectorDistanceMag);

      // sprite2 collision with big ball detected
      if (ubSprite2Colliding==0 && tSprite2VectorDistanceMagAbs<tSprite3VectorDistanceMagAbs)
      {
        v2d tSprite2VectorDistanceNorm;
        v2d_div_s(&tSprite2VectorDistanceNorm, &tSprite2VectorDistance, tSprite2VectorDistanceMag);
        
        fix16_t tSprite2VelocityMag = v2d_get_mag(&g_Sprite2Vector.tVelocity); // save old magnitude
        g_Sprite2Vector.tVelocity=tSprite2VectorDistanceNorm;
        //v2d_mul_s(&g_Sprite2Vector.tVelocity, &g_Sprite2Vector.tVelocity, tSprite2VelocityMag);
        v2d_mul_s(&g_Sprite2Vector.tVelocity, &g_Sprite2Vector.tVelocity, fix16_div(tSprite2VelocityMag,fix16_from_int(2)));

        ubSprite2Colliding=1;
      }

      // sprite3 collision with big ball detected
      if (ubSprite3Colliding==0 && tSprite3VectorDistanceMagAbs<=tSprite2VectorDistanceMagAbs)
      {
        v2d tSprite3VectorDistanceNorm;
        v2d_div_s(&tSprite3VectorDistanceNorm, &tSprite3VectorDistance, tSprite3VectorDistanceMag);

        fix16_t tSprite3VelocityMag = v2d_get_mag(&g_Sprite3Vector.tVelocity); // save old magnitude
        g_Sprite3Vector.tVelocity=tSprite3VectorDistanceNorm;
        //v2d_mul_s(&g_Sprite3Vector.tVelocity, &g_Sprite3Vector.tVelocity, tSprite3VelocityMag);
        v2d_mul_s(&g_Sprite3Vector.tVelocity, &g_Sprite3Vector.tVelocity, fix16_div(tSprite3VelocityMag,fix16_from_int(2)));

        ubSprite3Colliding=1;
      }
    }
    else if (ubSprite2Colliding||ubSprite3Colliding)
    {
      ubSprite2Colliding=0;
      ubSprite3Colliding=0;
#ifdef STAGE1_COLORDEBUG
      g_pCustom->color[0] = 0x0000;
#endif
    }

    if (ACE_IS_SPRITE_COLLIDING_3_1 )
    {
      if (ubSprite1Colliding==0)
      {
        v2d tSprite1VectorDistance;
        
        // Get direction after the rebound subtracting location and normalizing the result
        v2d_sub(&tSprite1VectorDistance,&g_Sprite1Vector.tLocation,&g_Sprite4Vector.tLocation);
        fix16_t tSprite1VectorDistanceMag=v2d_get_mag(&tSprite1VectorDistance);
        v2d_div_s(&tSprite1VectorDistance, &tSprite1VectorDistance, tSprite1VectorDistanceMag);
        
        //apply old magnitude (we dont want the ball to slow down)
        fix16_t tSprite1VelocityMag = v2d_get_mag(&g_Sprite1Vector.tVelocity); // save old magnitude
        g_Sprite1Vector.tVelocity=tSprite1VectorDistance;
        //v2d_mul_s(&g_Sprite1Vector.tVelocity, &g_Sprite1Vector.tVelocity, tSprite1VelocityMag);
        v2d_mul_s(&g_Sprite1Vector.tVelocity, &g_Sprite1Vector.tVelocity, fix16_div(tSprite1VelocityMag,fix16_from_int(2)));

        ubSprite1Colliding=1;
      }
    }
    else if (ubSprite1Colliding)
    {
#ifdef STAGE1_COLORDEBUG
      g_pCustom->color[0] = 0x0000;
#endif
      ubSprite1Colliding=0;
    }
  }

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
#ifdef STAGE1_COLORDEBUG
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
