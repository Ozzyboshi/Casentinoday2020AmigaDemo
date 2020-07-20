#include <ace/generic/main.h>
#include <ace/managers/key.h>
#include <ace/managers/joy.h>

#include <ace/managers/state.h>

#include "demointro.h"
#include "casentino.h"
#include "radiallineshiddenpart.h"
#include "../_res/uni54.h"
#include "shardeddata.h"
#include "main.h"


tStateManager *g_pGameStateManager = 0;
tState *g_pGameStates[GAME_STATE_COUNT] = {0};

void genericCreate(void)
{
  uni54_data_shared = uni54_data;
  keyCreate();
  g_pGameStateManager = stateManagerCreate();
  g_pGameStates[0] = stateCreate(demointroGsCreate, demointroGsLoop, demointroGsDestroy, 0, 0, 0);
  g_pGameStates[1] = stateCreate(gameGsCreate, gameGsLoop, gameGsDestroy, 0, 0, 0);
  g_pGameStates[2] = stateCreate(radialLinesGsCreate, radialLinesGsLoop, radialLinesGsDestroy, 0, 0, 0);
  stateChange(g_pGameStateManager, g_pGameStates[0]);
}

void genericProcess(void)
{
  keyProcess();
  joyProcess();
  stateProcess(g_pGameStateManager);
}

void genericDestroy(void)
{
  // Here goes your cleanup code
  stateManagerDestroy(g_pGameStateManager);
  stateDestroy(g_pGameStates[0]);
  stateDestroy(g_pGameStates[1]);
  stateDestroy(g_pGameStates[2]);
  keyDestroy();
  logWrite("Goodbye, Amiga!\n");
}
