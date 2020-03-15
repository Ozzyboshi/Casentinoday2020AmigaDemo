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

//void moverBounce2(tMover*);


fix16_t g_WindStep;
UBYTE g_endStageFlag;

BYTE bSpriteDirection ;

void stageAttractionInput()
{
  if (g_endStageFlag) return ;
  fix16_t g_WindMaxStep = fix16_div(fix16_from_int(1),fix16_from_int(20));
  if (keyCheck(KEY_W)) if (fix16_to_int(g_Sprite4Vector.tLocation.y)>32) g_Sprite4Vector.tLocation.y=fix16_sub(g_Sprite4Vector.tLocation.y,fix16_from_int(1));
  if (keyCheck(KEY_D))
  {

    if (fix16_to_int(g_Sprite4Vector.tLocation.x)<290) g_Sprite4Vector.tLocation.x=fix16_add(g_Sprite4Vector.tLocation.x,fix16_from_int(1));
    if (fix16_abs(g_Wind.x)<g_WindMaxStep)
      g_Wind.x=fix16_add(g_Wind.x,g_WindStep);
    if (g_Wind.x>0) bSpriteDirection=1;
    else if (g_Wind.x==0) bSpriteDirection=0;
    else if (g_Wind.x<0) bSpriteDirection=-1;
  }
  if (keyCheck(KEY_A))
  {
    if (fix16_to_int(g_Sprite4Vector.tLocation.x)>0) g_Sprite4Vector.tLocation.x=fix16_sub(g_Sprite4Vector.tLocation.x,fix16_from_int(1));
    if (fix16_abs(g_Wind.x)<g_WindMaxStep)
      g_Wind.x=fix16_sub(g_Wind.x,g_WindStep);
    if (g_Wind.x>0) bSpriteDirection=1;
    else if (g_Wind.x==0) bSpriteDirection=0;
    else if (g_Wind.x<0) bSpriteDirection=-1;
  }
  if (keyCheck(KEY_S))
  {
    if (fix16_to_int(g_Sprite4Vector.tLocation.y)<220)
      g_Sprite4Vector.tLocation.y=fix16_add(g_Sprite4Vector.tLocation.y,fix16_from_int(1));
    g_Wind.x=0;
    bSpriteDirection=0;
  }
  return ;
}

void stageAttractionPre()
{
  // Here i set the mass as the big ball because otherwise the screen res is not enough big to contain the orbits
	spriteVectorInit(&g_Sprite1Vector,1,BIG_BALL_START_POSITION_X-50,65,0,0,BIG_BALL_MASS); 

  spriteVectorInit(&g_Sprite2Vector,2,65,300,0,0,LITTLE_BALLS_MASS);
  spriteVectorInit(&g_Sprite3Vector,3,65,300,0,0,LITTLE_BALLS_MASS);
  spriteVectorInit(&g_Sprite4Vector,4,BIG_BALL_START_POSITION_X,120,0,0,BIG_BALL_MASS);

  moverMove(g_Sprite2Vector);
  moverMove(g_Sprite3Vector);

  g_ubVBounceEnabled=1;
  g_ubHBounceEnabled=1;
  g_Wind.x=g_WindStep;
  g_Wind.x=0;
}

void stageAttraction()
{
    /*
     Fgravity = ((G*m1*m2)/d^2 ) * rvector

     G = gravitational constant
     m1 = mass first object (in this case the little ball on sprite 1)
     m2 = mass second object (in this case the big ball on sprite 4)

     in this simulation second object is not moving (not affected to gravitational attraction)
     we only have to calculate Fgravity for first object

     rvector is the direction of the force (r vector normalized)

     --- sprite 1
     | |
     ---
        \
         \
          \ r
           \
           ---
           | | sprite 4
           ---
    
    so the direction is nothing else that the DIFFERENCE between sprite1location and sprite4 (normalized) so

    Last variable is d, this is the distance between 2 sprites

    */

    v2d sDirection,sDirectionNormalized,tForce;

    v2d sprite1Centerlocation,sprite4Centerlocation;
    sprite1Centerlocation.x = fix16_add(g_Sprite1Vector.tLocation.x,fix16_from_int(8));
    sprite1Centerlocation.y = fix16_add(g_Sprite1Vector.tLocation.y,fix16_from_int(8));
    sprite4Centerlocation.x = fix16_add(g_Sprite4Vector.tLocation.x,fix16_from_int(16));
    sprite4Centerlocation.y = fix16_add(g_Sprite4Vector.tLocation.y,fix16_from_int(16));

    //v2d_sub(&sDirection, &g_Sprite4Vector.tLocation, &g_Sprite1Vector.tLocation);
    v2d_sub(&sDirection, &sprite4Centerlocation, &sprite1Centerlocation);

    // Before normalizing we must get the magnitude
    fix16_t sDirectionMagnitude=v2d_get_mag(&sDirection);

    if (fix16_to_int(sDirectionMagnitude)>30) sDirectionMagnitude=fix16_from_int(30);
    else if (fix16_to_int(sDirectionMagnitude)<10) sDirectionMagnitude=fix16_from_int(10);

    //sDirectionMagnitude=fix16_from_int(30);

    // Normalizing means just dividing each component of the vector for its magnitude
    v2d_div_s(&sDirectionNormalized, &sDirection, sDirectionMagnitude);

    // Check!!! we have now rvector represented by sDirection, we used some very intensive coding for NON fpu based Amigas.
    // What do we need now? The magnitude!
    // Let's assume G=1, it's not a scientific simulation. 
    //fix16_t G = fix16_from_int(1);
    fix16_t G = fix16_div(fix16_from_int(1),fix16_from_int(100));

    // What do we need next? Mass, but we already have them embedded into the mover data structure, dont need to make math
    // Last thing, d squared, what is the distance? Of course is the magnitude of the sDirection vector, we only have to get its square...
    fix16_t sDirectionMagnitudeSquared = fix16_mul(sDirectionMagnitude,sDirectionMagnitude);

    // We have all the data to calculate the vector force, let's apply our formula based of sprite1 mover and the attractor
    fix16_t tStrength = fix16_div(fix16_mul(fix16_mul(G, g_Sprite1Vector.tMass), g_Sprite4Vector.tMass),sDirectionMagnitudeSquared);
    //if (fix16_to_int(tStrength)<20) tStrength=fix16_from_int(20);
    v2d_mul_s(&tForce, &sDirection, tStrength);

    /*fix16_t g_WindMaxStep = fix16_div(fix16_from_int(1),fix16_from_int(5));
    if (g_Wind.x<g_WindMaxStep && g_Wind.x>fix16_mul(g_WindMaxStep,fix16_from_int(-1)))
      spriteVectorApplyForce(&g_Sprite1Vector,&g_Wind);*/

    // At the end of the stage let the balls fall
    if (!g_endStageFlag) 
    {
      //Apply the force
      spriteVectorApplyForce(&g_Sprite1Vector,&tForce);
    }
    else
    {
      spriteVectorResetAccelleration(&g_Sprite1Vector);
      spriteVectorResetAccelleration(&g_Sprite4Vector);
      spriteVectorApplyForce(&g_Sprite4Vector,&g_Gravity);
      spriteVectorApplyForce(&g_Sprite1Vector,&g_Gravity);
      moverAddAccellerationToVelocity(&g_Sprite4Vector);
      moverAddVelocityToLocation(&g_Sprite4Vector);
    }

    moverAddAccellerationToVelocity(&g_Sprite1Vector);
    moverAddVelocityToLocation(&g_Sprite1Vector);



	  if (moverMove(g_Sprite1Vector)) g_Sprite1Vector.ubLocked = 1;
    if (moverMove(g_Sprite4Vector)) g_Sprite4Vector.ubLocked = 1;

    if (g_Sprite1Vector.ubLocked==0) moverBounce(&g_Sprite1Vector);
    if (g_Sprite4Vector.ubLocked==0) moverBounce(&g_Sprite4Vector);

    if (g_Sprite1Vector.ubLocked==0) spriteVectorResetAccelleration(&g_Sprite1Vector);
    if (g_Sprite4Vector.ubLocked==0) spriteVectorResetAccelleration(&g_Sprite4Vector);
    
	return ;
}
/*
void moverBounce2(tMover* pMover)
{
  int x=fix16_to_int(pMover->tLocation.x);
  int y=fix16_to_int(pMover->tLocation.y);

  if (g_ubHBounceEnabled && (x>s_pAceSprites[pMover->ubSpriteIndex].iBounceRightLimit||x<=0))
  {
    pMover->tVelocity.x=fix16_mul(pMover->tVelocity.x,fix16_from_int(-1));
    pMover->tVelocity.x=fix16_sub(pMover->tVelocity.x,pMover->tAccelleration.x);
    
   

   
  }

  if (g_ubVBounceEnabled && (y>s_pAceSprites[pMover->ubSpriteIndex].iBounceBottomLimit||y<10))
  {
    pMover->tVelocity.y=fix16_mul(pMover->tVelocity.y,fix16_from_int(-1));
    
    // to do not stop bounce add
    pMover->tVelocity.y=fix16_sub(pMover->tVelocity.y,pMover->tAccelleration.y);
  }
}*/


