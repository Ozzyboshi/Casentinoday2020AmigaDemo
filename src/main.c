#include <ace/generic/main.h>
#include <ace/managers/key.h>
#include <ace/managers/joy.h>

#include "casentino.h"

void genericCreate(void) {
  // Here goes your startup code
  logWrite("Hello, Amiga!\n");
  keyCreate();
  gamePushState(gameGsCreate,gameGsLoop,gameGsDestroy);
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
