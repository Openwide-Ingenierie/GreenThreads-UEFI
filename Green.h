#ifndef GREEN_H
#define GREEN_H
#include <Library/MemoryAllocationLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/UefiLib.h>

#define STACKSIZE 16384
#define THREAD_RUNNABLE 0 // Preempted
#define THREAD_WAITING  1 // For a lock for example
#define THREAD_FINISHED 2 // Thread waiting to be joined
#define THREAD_JOINED   3

typedef UINTN THREAD_STATUS;

typedef struct{
  UINTN Rax, Rbx, Rcx, Rdx, Rbp, Rsi, Rdi, Rsp;
  UINTN R8, R9, R10, R11, R12, R13, R14, R15;
  UINTN Mmx0, Mmx1, Mmx2, Mmx3, Mmx4, Mmx5, Mmx6, Mmx7;
  UINTN Rflags;
  UINTN Rip;
  VOID* StackBase;
  volatile THREAD_STATUS Stat;
  VOID* Return; // Return value when finished
  UINTN Id;
} CONTEXT;

typedef VOID* (*GREEN_FUNCTION)(VOID*);

VOID ThreadInitialize(VOID);

VOID ThreadCreate(CONTEXT* Ctx, GREEN_FUNCTION Function, VOID* Param);

VOID ThreadJoin(CONTEXT* Ctx, VOID** Return);

VOID ThreadWrapper(CONTEXT* Ctx, GREEN_FUNCTION Function, VOID* Param);

VOID ThreadYield(VOID);

#endif
