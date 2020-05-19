#include <ace/generic/main.h>
#include <ace/managers/key.h>
#include <ace/managers/joy.h>

#include "demointro.h"
#include "casentino.h"
#include "radiallineshiddenpart.h"
#include "../_res/uni54.h"
#include "shardeddata.h"

void genericCreate(void) {
  // Here goes your startup code
  uni54_data_shared = uni54_data;
  //uni54_size_shared = &uni54_size;
  logWrite("Hello, Amiga!\n");
  keyCreate();
  //gamePushState(gameGsCreate,gameGsLoop,gameGsDestroy);
  gamePushState(demointroGsCreate,demointroGsLoop,demointroGsDestroy);
  //gamePushState(radialLinesGsCreate,radialLinesGsLoop,radialLinesGsDestroy);
}

void genericProcess(void) {
  keyProcess();
  joyProcess();
  gameProcess();
}

void genericDestroy(void) {
  // Here goes your cleanup code
  keyDestroy();
  logWrite("Goodbye, Amiga!\n");
}
