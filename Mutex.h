#ifndef MUTEX_H
#define MUTEX_H

#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiLib.h>

typedef struct{
  UINT8 Taken;
} MUTEX;

VOID MutexInit(MUTEX* Mut);

VOID MutexLock(MUTEX* Mut);

VOID MutexUnlock(MUTEX* Mut);

VOID MutexDestroy(MUTEX* Mut);

#endif
