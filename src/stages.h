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

#ifndef _STAGES_H_
#define _STAGES_H_

#include "casentino.h"
#include "physics.h"
UBYTE g_ubIsCollisionEnabled;

tMover g_Sprite1Vector;
tMover g_Sprite2Vector;
tMover g_Sprite3Vector;
tMover g_Sprite4Vector;

v2d g_Gravity , g_Wind;

void stage1input();
void stage1pre();
void stage1();
void stage2input();
void stage2pre();
void stage2();

// Attraction stage prototypes
void stageAttractionInput();
void stageAttraction();
void stageAttractionPre();

// 3d Cube stage prototypes
void stage3dCubeInit();
void stage3dCubeInput();
void stage3dCubePre();
void stage3dCube();
void stage3dCubePost();
void stage3dCube2();
#endif