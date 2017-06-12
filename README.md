# Green Threads for UEFI

## Introduction
This project is a an alpha version of "green" threads for UEFI. It's not really like Linux green threads as there is no distinction between user space and kernel space but the different threads are running on the same core

The aim of this project is not the be a very powerful and optimised threads library for UEFI but to have a simple abstraction of threads for UEFI, easily usable to any developper. That's why the signature of the function are similar to linux pthreads.

## IMPORTANT NOTE:
- For the moment, this library is **ONLY** compatible with **x86_64** architecture (it contains assembly code for context switching)

- Most of the functions given in EDK II library are **NOT** thread-safe. Therefore, you should treat those function calls as a critical section and so use a `Mutex` to protect them.
For example, calling EDK II `Print` function in two threads at the same time without using a mutex will result in an unpredictable behavior.

- Lowest-level functions, defined in UEFI official specification, *SEEM* to be thread-safe. As there is no guarantee, it's better to use a mutex too.

## **API**

### GTHREAD:

#### Types:
The type `GTHREAD` defines an abstract thread. It's equivalent to `CONTEXT` type.

The type `GREEN_FUNCTION` defines a function taking a generic pointer as parameter and returning a generic pointer too, so, it's equivalent to:
```
VOID* Function(VOID*);
```

#### Functions:
```
VOID ThreadInitialize(VOID);
```
This function initialize the green thread library, it also launches the round-robin scheduler. It should be called **EXACTLY** once, in the beginning of a program.


```
VOID ThreadCreate(GTHREAD* Ctx, GREEN_FUNCTION Function, VOID* Param);
```
This function creates and launches a thread which will execute the function given as second parameter. The last parameter is the paramater to give to the `GREEN_FUNCTION`.

```
VOID ThreadJoin(GTHREAD* Ctx, VOID** Return)
```
This functions is used to join a thread being executed or already finished. The first parameter is a pointer to the thread to join. The second parameter is **optional** and is used to get the return value of the function executed by the joined thread.

```
VOID ThreadYield(VOID);
```
This function is used to tell the scheduler to switch to another thread than the current one (if available)

### MUTEX:
#### Type:
A mutex is defined by the type `MUTEX`.

#### Functions:
```
VOID MutexInit(MUTEX* Mut);
```
This function is used to initialize a given mutex, this functions should be called once per mutex

```
VOID MutexLock(MUTEX* Mut);
```
This function is used to take the mutex. If it's already taken, the thread will block until te mutex is available again. If it's free, the thread won't be blocked and the mutex is set as "taken".

```
VOID MutexUnlock(MUTEX* Mut);
```
This function releases a taken mutex.
**NOTE:** A mutex taken by a thread _A_ can be released by a thread _B_, so please be careful when using mutex.

```
MutexDestroy(MUTEX* Mut);
```
This function destroys a mutex initialized.

## Compiling this project
To compile this project, you need EDK II, follow the instructions given on their official documentation page which can be find here: https://github.com/tianocore/tianocore.github.io/wiki/Getting-Started-with-EDK-II

## Launching the example

The file `main.c` contains a simple example of a program drawing two lines continuously on the screen. After compiling this program, rename the generated `GrThread.efi` to `BOOTX64.EFI` and put it on a `FAT16` or `FAT32` USB key in:

`/EFI/BOOT/BOOTX64.EFI`