#include <Mutex.h>

VOID
MutexInit(MUTEX* Mut)
{
  __atomic_clear(&Mut->Taken, 0);
}

VOID
__attribute__ ((noinline)) MutexLock(MUTEX* Mut)
{
  while(__atomic_test_and_set(&Mut->Taken, 0));
}

VOID
MutexUnlock(MUTEX* Mut)
{
  __atomic_clear(&Mut->Taken, 0);
}

VOID
MutexDestroy(MUTEX* Mut)
{
}

