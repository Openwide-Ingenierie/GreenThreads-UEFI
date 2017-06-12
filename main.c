#include <Uefi.h>
#include <Library/UefiApplicationEntryPoint.h>
#include <Library/UefiLib.h>
#include <Library/BaseMemoryLib.h>
#include <Uefi/UefiSpec.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UefiRuntimeServicesTableLib.h>
#include <Library/BaseLib.h>
#include <Protocol/GraphicsOutput.h>
#include "Green.h"
#include "Mutex.h"
#define CELLSIZE 10

static EFI_GRAPHICS_OUTPUT_PROTOCOL* Gop = NULL;
static EFI_GRAPHICS_OUTPUT_BLT_PIXEL White = {0xFF, 0xFF, 0xFF, 0};
static EFI_GRAPHICS_OUTPUT_BLT_PIXEL Black = {0, 0, 0, 0};

VOID
DrawCell(UINT32 X, UINT32 Y, BOOLEAN Reset)
{
  EFI_GRAPHICS_OUTPUT_BLT_PIXEL Color = (Reset)?Black:White;
  Gop->Blt(Gop, &Color, EfiBltVideoFill,
           0, 0,
           X*CELLSIZE, Y*CELLSIZE,
           CELLSIZE, CELLSIZE,
           0);
}

VOID*
Loading(VOID* p)
{
  UINTN i;
  struct { MUTEX* M; UINT32 Y; UINT32 W; }* Args = p;
  
  MUTEX* lock = Args->M;
  UINT32 Y = Args->Y;
  UINT32 W = Args->W;
  BOOLEAN Reset = FALSE;
  
  while(TRUE){
    for(i = 0; i < W; i++){
      MutexLock(lock);
      DrawCell(i, Y, Reset);
      MutexUnlock(lock);
      gBS->Stall(10000);
    }
    Reset = !Reset;
  }
  return NULL;
}

EFI_STATUS EFIAPI
UefiMain(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable)
{
  EFI_STATUS St = gBS->LocateProtocol(&gEfiGraphicsOutputProtocolGuid,
                                      NULL,
                                      (VOID**) &Gop);
  if (EFI_ERROR(St)) {
    Print(L"Unable to locate Graphics Output Protocol\n");
    Print(L"Press any key to exit...\n");
    UINTN EventIndex;
    gBS->WaitForEvent(1, &gST->ConIn->WaitForKey, &EventIndex);
    Exit(1);
  }
  CONST UINT32 ScreenWidth  = Gop->Mode->Info->HorizontalResolution;
  CONST UINT32 GameWidth  = ScreenWidth / CELLSIZE;
  
  GTHREAD thread1;
  MUTEX lock;
  MutexInit(&lock);
  ThreadInitialize();
  struct { MUTEX* M; UINT32 Y; UINT32 W; } Args = {&lock, 20, GameWidth};
  ThreadCreate(&thread1, Loading, &Args);

  ThreadYield();
  struct { MUTEX* M; UINT32 Y; UINT32 W; } Args2 = {&lock, 25, GameWidth};
  Loading(&Args2);

  // Will never come here but generally, threads have to be joined to clean the threads table
  ThreadJoin(&thread1, NULL);
  
  Print(L"Press any key to exit...\n");
  EFI_INPUT_KEY Key;
  UINTN EventIndex;
  gBS->WaitForEvent(1, &gST->ConIn->WaitForKey, &EventIndex);
  gST->ConIn->ReadKeyStroke (gST->ConIn, &Key);
  
  return EFI_SUCCESS;
}
