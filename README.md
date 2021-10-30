# auxlib
Full reversing of the Microsoft Auxiliary Windows API Library and ported to C

Plese, consider make a donation: https://github.com/sponsors/therealdreg

The LoaderLock is used by some APIs as GetModuleHandle, and also when DllMain is executed, that is why we have
 to be carefull when working with threads and dlls. Example:

- Thread A Loads a DLL, DllMain executes getchar()...

- Thread B Calls GetModuleHandleA.

In this simple scenario, until the user doesnt press a key Thread B will stand by.

Solution:

- Thread A can check with AuxUlibIsDLLSynchronizationHeld function that is blocking LoaderLock and does not execute calls that block the Thread.

Internal Functions

```c
BOOL Win9xCheckDllSynchronization( PBOOL in )
{
   in = FALSE;

  return TRUE;
}
```

Is used for support of Win9x.

Global variables

```c
AuxpUlibData_t          AuxpUlibData;
```

Structure that initiates AuxUlibInitialize and it will be used by the rest of functions

```c
typedef struct AuxpUlibData_s
{
	DWORD                          dwPlatformId;
	DWORD                          dwMajorVersion;
	DWORD                          dwMinorVersion;
	SetSystemFileCacheSize_t       RtSetSystemFileCacheSize;
	NtSetSystemInformation_t       RtNtSetSystemInformation;
	PrivIsDllSynchronizationHeld_t RtPrivIsDllSynchronizationHeld;

} AuxpUlibData_t;
```

```c
LONG                    AuxpInitState;
```

If the library has been initialized with AuxUlibInitialize value will be 1, otherwise 0.

```c
LONG                    DLLSynchAPISafe;
```

```c
HANDLE                 DllSynchronizationOwnerPtr;
```

Routine Description

    This routine must be successfully called by an application before any
    other routine in the library may be called. It serves to initialize any global
    state that may be required by other routines in the file.

    It is safe to call this routine in a multi-threaded environment.
    
Arguments

    None.

Return Value

    Boolean status. Error code available via GetLastError ().

```c
BOOL WINAPI AuxUlibInitialize( VOID )
```

- If the library has ben initialized ( AuxpInitState == 1 ) will return TRUE straight away.

- Otherwise, the structure AuxpUlibData will initialize in this way

-. dwPlatformId, dwMajorVersion, dwMinorVersion will be equaled to the same fields, with the same values, of the structure OSVERSIONINFOW returned from GeVersionExw().

In case that GetVersionEw returns FALSE the initializing will fail and it will be GetLastError.

-. If the fields dwPlatformId are not valid VER_PLATFORM_WIN32_NT or VER_PLATFORM_WIN32_WINDOWS the initialization will fail and the Last error will be added to ERROR_NOT_SUPPORTED.

-. If dwPlatformId is different of VER_PLATFORM_WIN32_NT it will initialize RtPrivIsDllSynchronizationHeld = Win9xCheckDllSynchronization 

-. It will be obtained with GetModuleHandleW kernel32.dll and only ntdll.dll will be obtained if dwPlatformId is VER_PLATFORM_WIN32_NT. In case that the call fails, the initialization will fail and the Last error will be added to ERROR_DLL_NOT_FOUND.

-. If dwPlatformId is equal to VER_PLATFORM_WIN32_NT

--. They will be obtained with GetProcAddress

---. RtSetSystemFileCacheSize =  GetProcAddress( kernel32.dll, SetSystemFileCacheSize );

If it fails for different reasons of GetLastError() == ERROR_PROC_NOT_FOUND, the initialization will fai and Last error will be added to GetLastError().

---. RtNtSetSystemInformation = GetProcAddress( ntdll.dll, NtSetSystemInformation );

If GetProcAddress fails, initialization will fail and Last error will be placed Last Error to ERROR_NOT_SUPPORTED.

---. RtPrivIsDllSynchronizationHeld = GetProcAddress( kernel32.dll, PrivIsDllSynchronizationHeld );

If GetProcAddress fails, and GetLastError() is different of ERROR_PROC_NOT_FOUND, initialization will fail and will be placed Last error to GetLastError().

- If everything went well AuxpInitState will be set to 1 with
```c
InterlockedExchange( & AuxpInitState, 1 );
```

Routine Description

    This routine is used to set the current file system cache working set size. It 
    requires that the caller has enabled the SE_INCREASE_QUOTA_PRIVILEGE 
    in the currently active token prior to invoking this routine.

    This API is supported on Windows 2000 and later.
    
Arguments

    MinimumFileCacheSize - The minimum file cache size. Use (SIZE_T)-1 if 
        the file cache is being flushed.

    MaximumFileCacheSize - The maximum file cache size. Use (SIZE_T)-1 
        if the file cache is being flushed.

    Flags - Flags relevant to the file cache size adjustment. Currently this must 
        be zero.

Return Value

    Boolean status. Error code available via GetLastError (). If the routine is
        invoked prior to invoking the initialization routine, the returned error code
        will be ERROR_INVALID_FUNCTION.

--
```c
BOOL WINAPI AuxUlibSetSystemFileCacheSize 
(
    IN SIZE_T MinimumFileCacheSize,
    IN SIZE_T MaximumFileCacheSize,
    IN DWORD Flags
);
```

- If the library was not initiated ( AuxpInitState == 0 ) will return FALSE and in the Last error ERROR_INVALID_FUNCTION.

- If the field RtSetSystemFileCacheSize has been initiated it will call SetSystemFileCacheSize with the arguments passed to the function and it will return what SetSystemFileCacheSize returns.

- If dwMajorVersion is lower than 5 (5 = Windows Server 2003, Windows XP, or Windows 2000) it will return FALSE and Last error to ERROR_NOT_SUPPORTED.

-If Flags value is not 0 it will return FALSE and in the Last error ERROR_INVALID_PARAMETER.

- We call RtNtSetSystemInformation in this way
RtNtSetSystemInformation( SystemFileCacheInformation, & system_information, sizeof( system_information ) )

If RtNtSetSystemInformation returns bigger or equal to 0 it will return TRUE.

- Otherwise, it will return FALSE and in Last error ERROR_INTERNAL_ERROR.

Routine Description

    This routine is used to determine whether or not the caller is executing
    code while holding a system synchronization primitive. Such a situation
    can arise when the OS temporarily calls into user-specified code as part 
    of the DLL load procedure.

    A caller can benefit from this information by avoiding operations that 
    could potentially lead to deadlocks, e.g., acquiring a process private lock.

    For example, consider the following case

        Thread A runs the THREAD_ATTACH routine for DLL X. This routine
            is invoked with OS DLL synchronization held. Suppose further that
            as part of this routine Thread A acquires some lock in DLL X (Lx).

        Thread B runs some code in DLL X that, while holding Lx, calls the OS
            library loader to, e.g. GetModuleHandle. As this routine acquires
            OS DLL synchronization, Thread B will deadlock with Thread A.

        This is an inherent limitation in the design of the OS loader as it
        performs such callouts as THREAD_ATTACH while holding loader
        synchronization. It can be partially ameliorated if Thread A detects
        that it is running with DLL synchronization held and only try-acquires
        other locks (such as Lx) that it may wish to take
    
Arguments

    SynchronizationHeld - Boolean value which indicates whether or not
        synchronization is held.

Return Value

    Boolean status. Error code available via GetLastError (). If the routine is
        invoked prior to invoking the initialization routine, the returned error code
        will be ERROR_INVALID_FUNCTION.

--
```c
BOOL WINAPI AuxUlibIsDLLSynchronizationHeld( OUT PBOOL SynchronizationHeld );
```

- If DLLSynchAPISafe is equal to 1, PROC TRUE is executed

PROC TRUE

-. If the value that DllSynchronizationOwnerPtr is pointing is different to 0

--. TRUE is returned and were SynchronizationHeld is pointing

---. TRUE If the value were DllSynchronizationOwnerPtr is pointing (OwningThread de LoaderLock) is equal to GetCurrentThreadId().

---. FALSE Otherwise.

PROC TRUE

- If DLLSynchAPISafe is different than 1

--. If AuxpInitState is different than 1 it returns FALSE and in Last error ERROR_INVALID_FUNCTION.

--. If RtPrivIsDllSynchronizationHeld exists, we call to RtPrivIsDllSynchronizationHeld( SynchronizationHeld )  and returns what the call returns.

--. If DLLSynchAPISafe exists

---. If its different to 1 it returns FALSE and in Last error NOT_SUPPORTED.

---. Otherwise PROC TRUE is executed

--. Otherwise

---. If dwMajorVersion is minor than 5, DLLSynchAPISafe = 2 and returns FALSE and in Last error NOT_SUPPORTED.

---. Otherwise, DllSynchronizationOwnerPtr is initialized to the address were the field is OwningThread of LoaderLock
DllSynchronizationOwnerPtr = & NtCurrentTeb()-Peb-LoaderLock-OwningThread;

And we set DLLSynchAPISafe to 1 this way InterlockedExchange( & DLLSynchAPISafe, 1 );

At the end we call PROC TRUE.
