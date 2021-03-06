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

#define MAXSTAGES 4 // Max num of stages inside the demo (progressive order)

typedef struct _tStageManager {
  void (*g_pPreStageFunction) ();
  void (*g_pStageFunction) ();
  void (*g_pStageInputFunction) ();
  void (*g_pPostStageFunction) ();
  void (*g_pInitStageFunction) ();
  void (*g_pAfterWaitStageFunction) ();
} tStageManager;

tStageManager s_pStagesFunctions[]={

	// Stage 1 - gravity + wind
  {
  	.g_pPreStageFunction = stage1pre, 
  	.g_pStageFunction=stage1, 
  	.g_pStageInputFunction=stage1input,
    .g_pPostStageFunction = NULL,
    .g_pInitStageFunction = NULL,
    .g_pAfterWaitStageFunction = NULL
  },

  // Stage 2 - 3dcube
  {
  	.g_pPreStageFunction = stage3dCubePre, 
  	.g_pStageFunction=stage3dCube, 
  	.g_pStageInputFunction=stage3dCubeInput,
    .g_pPostStageFunction = stage3dCubePost,
    .g_pInitStageFunction = stage3dCubeInit,
    .g_pAfterWaitStageFunction = stage3dCube2
  },

  // Stage 3 - friction
  {
  	.g_pPreStageFunction = stage2pre,
  	.g_pStageFunction=stage2,
  	.g_pStageInputFunction=stage2input,
    .g_pPostStageFunction = NULL,
    .g_pInitStageFunction = NULL,
    .g_pAfterWaitStageFunction = NULL
  },

  // Stage 4 - attraction
  {
    .g_pPreStageFunction = stageAttractionPre,
    .g_pStageFunction=stageAttraction,
    .g_pStageInputFunction=stageAttractionInput,
    .g_pPostStageFunction = NULL,
    .g_pInitStageFunction = NULL,
    .g_pAfterWaitStageFunction = NULL
  }
};
