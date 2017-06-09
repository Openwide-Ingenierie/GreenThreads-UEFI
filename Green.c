#include "Green.h"
#define MAX_THREAD 10

volatile CONTEXT Main;
volatile CONTEXT* Contexts[MAX_THREAD];
volatile UINTN RunningId = 0;
volatile EFI_EVENT* Schedule = NULL;
EFI_TIME Prev;

VOID EFIAPI SwitchContext(volatile CONTEXT*, volatile CONTEXT*);

VOID EFIAPI
CChangeContext(IN EFI_EVENT Event, IN VOID* Args)
{
  EFI_TIME Now;
  gRT->GetTime(&Now, NULL);
  UINT32 Elapsed =
    (Now .Minute * 60 + Now .Second) -
    (Prev.Minute * 60 + Prev.Second);
  Print(L"Elapsed: %d secondes\n", Elapsed);
  Prev = Now;
  UINTN NextRunning = (RunningId+1) % MAX_THREAD;
  while(Contexts[NextRunning] == NULL ||
	Contexts[NextRunning]->Stat != THREAD_RUNNABLE){
    NextRunning = (NextRunning + 1) % MAX_THREAD;
  }
  if(NextRunning == RunningId){
    return;
  }
  volatile CONTEXT* Old = Contexts[RunningId];
  volatile CONTEXT* New = Contexts[NextRunning];
  RunningId = NextRunning;
  SwitchContext(Old, New);
}

VOID
ThreadInitialize(VOID)
{
  if(Schedule){
    return;
  }
  Schedule = AllocatePool(sizeof(EFI_EVENT));
  if(Schedule == NULL){
    Print(L"Cannot allocate pool!\n");
  }
  // Initialize the Contexts
  Contexts[0] = &Main;
  UINT8 i;
  for(i = 1; i < MAX_THREAD; i++){
    Contexts[i] = NULL;
  }

  EFI_STATUS Status = gBS->CreateEvent(EVT_TIMER |
				       EVT_NOTIFY_SIGNAL,
				       TPL_NOTIFY,
				       CChangeContext,
				       NULL,
				       (EFI_EVENT*) Schedule);
  if(EFI_ERROR(Status)){
    Print(L"Error creating the event: %d\n", Status);
  }
  // 40000 (* 100 ns = 4 000 000 ns = 4 ms)
  Status = gBS->SetTimer(*Schedule, TimerPeriodic, 100000000);
  gRT->GetTime(&Prev, NULL);
  if(EFI_ERROR(Status)){
    Print(L"Error setting the timer : %d\n", Status);
  }
}

UINTN
GetFreshId(VOID)
{
  UINT8 i;
  for(i = 1; i < MAX_THREAD; i++){
    if(Contexts[i] == NULL ||
       Contexts[i]->Stat == THREAD_JOINED){
      return i;
    }
  }
  return 0;
}

VOID
ThreadCreate(CONTEXT* Ctx, GREEN_FUNCTION Function, VOID* Param)
{
  gBS->SetMem(Ctx, sizeof(CONTEXT), 0);
  VOID* Stack = AllocatePool(STACKSIZE);
  Ctx->StackBase = Stack;
  // Put the return address at the top of the stack
  Stack += STACKSIZE - 8;
  *((UINTN*) Stack) = (UINTN) ThreadWrapper;

  Ctx->Rsp = (UINTN) Stack;
  Ctx->Rip = (UINTN) ThreadWrapper;
  // Argument of the function
  Ctx->Rdi = (UINTN) Ctx;
  Ctx->Rsi = (UINTN) Function;
  Ctx->Rdx = (UINTN) Param;
  Ctx->Stat = THREAD_RUNNABLE;
  // Context id
  UINTN Id = GetFreshId();
  if(Id == 0){
    Print(L"TODO: return Error\n");
    return;
  }
  Ctx->Id = Id;
  Contexts[Id] = Ctx;
}

static inline
VOID
ThreadFinish(CONTEXT* Ctx, VOID** Return)
{
  if(Return != NULL){
    *Return = Ctx->Return;
  }
  Ctx->Stat = THREAD_JOINED;
  // Free the space for another thread
  Contexts[Ctx->Id] = NULL;
  FreePool(Ctx->StackBase);
}

VOID
ThreadJoin(CONTEXT* Ctx, VOID** Return)
{
  if(Ctx->Stat == THREAD_JOINED){
    return;
  } else if(Ctx->Stat == THREAD_FINISHED) {
    ThreadFinish(Ctx, Return);
  } else {
    // Wait the thread to finish
    while(Ctx->Stat != THREAD_FINISHED);
    ThreadFinish(Ctx, Return); 
  }
}

VOID
ThreadWrapper(CONTEXT* Ctx, GREEN_FUNCTION Function, VOID* Param)
{
  gBS->RestoreTPL(TPL_APPLICATION);
  VOID* Return = Function(Param);
  Ctx->Return = Return;
  Ctx->Stat = THREAD_FINISHED;
  // Wait for a thread to join this one
  for(;;);
}

VOID
ThreadYield(VOID){
  gBS->SignalEvent(*Schedule);
}
