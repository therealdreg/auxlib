/* 
	Extracted from: 
		- http://undocumented.ntinternals.net 
		- http://www.reactos.org

		(except PrivIsDllSynchronizationHeld)
*/

#ifndef _INTERNALS_H__
#define _INTERNALS_H__

#include <windows.h>

typedef struct _CLIENT_ID
{
	HANDLE UniqueProcess;
	HANDLE UniqueThread;

} CLIENT_ID, *PCLIENT_ID;


typedef LONG NTSTATUS;

typedef struct _LSA_UNICODE_STRING 
{
	USHORT Length;
	USHORT MaximumLength;
	PWSTR Buffer;

} LSA_UNICODE_STRING, *PLSA_UNICODE_STRING, UNICODE_STRING, *PUNICODE_STRING;

typedef struct _PEB_LDR_DATA
{
	ULONG               Length;
	BOOLEAN             Initialized;
	PVOID               SsHandle;
	LIST_ENTRY          InLoadOrderModuleList;
	LIST_ENTRY          InMemoryOrderModuleList;
	LIST_ENTRY          InInitializationOrderModuleList;

} PEB_LDR_DATA, *PPEB_LDR_DATA;

typedef struct _CURDIR
{
	UNICODE_STRING DosPath;
	PVOID Handle;

} CURDIR, *PCURDIR;

typedef struct RTL_DRIVE_LETTER_CURDIR
{
	USHORT              Flags;
	USHORT              Length;
	ULONG               TimeStamp;
	UNICODE_STRING      DosPath;

} RTL_DRIVE_LETTER_CURDIR, *PRTL_DRIVE_LETTER_CURDIR;


typedef struct _RTL_USER_PROCESS_PARAMETERS
{
	ULONG               AllocationSize;
	ULONG               Size;
	ULONG               Flags;
	ULONG               DebugFlags;
	HANDLE              ConsoleHandle;
	ULONG               ConsoleFlags;
	HANDLE              hStdInput;
	HANDLE              hStdOutput;
	HANDLE              hStdError;
	CURDIR              CurrentDirectory;
	UNICODE_STRING      DllPath;
	UNICODE_STRING      ImagePathName;
	UNICODE_STRING      CommandLine;
	PWSTR               Environment;
	ULONG               dwX;
	ULONG               dwY;
	ULONG               dwXSize;
	ULONG               dwYSize;
	ULONG               dwXCountChars;
	ULONG               dwYCountChars;
	ULONG               dwFillAttribute;
	ULONG               dwFlags;
	ULONG               wShowWindow;
	UNICODE_STRING      WindowTitle;
	UNICODE_STRING      Desktop;
	UNICODE_STRING      ShellInfo;
	UNICODE_STRING      RuntimeInfo;
	RTL_DRIVE_LETTER_CURDIR DLCurrentDirectory[0x20];

} RTL_USER_PROCESS_PARAMETERS, *PRTL_USER_PROCESS_PARAMETERS;

typedef struct tagRTL_BITMAP 
{
	ULONG  SizeOfBitMap; /* Number of bits in the bitmap */
	PULONG Buffer; /* Bitmap data, assumed sized to a DWORD boundary */

} RTL_BITMAP, *PRTL_BITMAP;


typedef struct _PEB
{
	BOOLEAN                      InheritedAddressSpace;             /*  00 */
	BOOLEAN                      ReadImageFileExecOptions;          /*  01 */
	BOOLEAN                      BeingDebugged;                     /*  02 */
	BOOLEAN                      SpareBool;                         /*  03 */
	HANDLE                       Mutant;                            /*  04 */
	HMODULE                      ImageBaseAddress;                  /*  08 */
	PPEB_LDR_DATA                LdrData;                           /*  0c */
	RTL_USER_PROCESS_PARAMETERS *ProcessParameters;                 /*  10 */
	PVOID                        SubSystemData;                     /*  14 */
	HANDLE                       ProcessHeap;                       /*  18 */
	PRTL_CRITICAL_SECTION        FastPebLock;                       /*  1c */
	PVOID /*PPEBLOCKROUTINE*/    FastPebLockRoutine;                /*  20 */
	PVOID /*PPEBLOCKROUTINE*/    FastPebUnlockRoutine;              /*  24 */
	ULONG                        EnvironmentUpdateCount;            /*  28 */
	PVOID                        KernelCallbackTable;               /*  2c */
	PVOID                        EventLogSection;                   /*  30 */
	PVOID                        EventLog;                          /*  34 */
	PVOID /*PPEB_FREE_BLOCK*/    FreeList;                          /*  38 */
	ULONG                        TlsExpansionCounter;               /*  3c */
	PRTL_BITMAP                  TlsBitmap;                         /*  40 */
	ULONG                        TlsBitmapBits[2];                  /*  44 */
	PVOID                        ReadOnlySharedMemoryBase;          /*  4c */
	PVOID                        ReadOnlySharedMemoryHeap;          /*  50 */
	PVOID                       *ReadOnlyStaticServerData;          /*  54 */
	PVOID                        AnsiCodePageData;                  /*  58 */
	PVOID                        OemCodePageData;                   /*  5c */
	PVOID                        UnicodeCaseTableData;              /*  60 */
	ULONG                        NumberOfProcessors;                /*  64 */
	ULONG                        NtGlobalFlag;                      /*  68 */
	BYTE                         Spare2[4];                         /*  6c */
	LARGE_INTEGER                CriticalSectionTimeout;            /*  70 */
	ULONG                        HeapSegmentReserve;                /*  78 */
	ULONG                        HeapSegmentCommit;                 /*  7c */
	ULONG                        HeapDeCommitTotalFreeThreshold;    /*  80 */
	ULONG                        HeapDeCommitFreeBlockThreshold;    /*  84 */
	ULONG                        NumberOfHeaps;                     /*  88 */
	ULONG                        MaximumNumberOfHeaps;              /*  8c */
	PVOID                       *ProcessHeaps;                      /*  90 */
	PVOID                        GdiSharedHandleTable;              /*  94 */
	PVOID                        ProcessStarterHelper;              /*  98 */
	PVOID                        GdiDCAttributeList;                /*  9c */
	PRTL_CRITICAL_SECTION        LoaderLock;                        /*  a0 */
	ULONG                        OSMajorVersion;                    /*  a4 */
	ULONG                        OSMinorVersion;                    /*  a8 */
	ULONG                        OSBuildNumber;                     /*  ac */
	ULONG                        OSPlatformId;                      /*  b0 */
	ULONG                        ImageSubSystem;                    /*  b4 */
	ULONG                        ImageSubSystemMajorVersion;        /*  b8 */
	ULONG                        ImageSubSystemMinorVersion;        /*  bc */
	ULONG                        ImageProcessAffinityMask;          /*  c0 */
	ULONG                        GdiHandleBuffer[34];               /*  c4 */
	ULONG                        PostProcessInitRoutine;            /* 14c */
	PRTL_BITMAP                  TlsExpansionBitmap;                /* 150 */
	ULONG                        TlsExpansionBitmapBits[32];        /* 154 */
	ULONG                        SessionId;                         /* 1d4 */

} PEB, *PPEB;

typedef struct _GDI_TEB_BATCH
{
	ULONG  Offset;
	HANDLE HDC;
	ULONG  Buffer[0x136];

} GDI_TEB_BATCH;


typedef struct _TEB
{
	NT_TIB          Tib;                        /* 000 */
	PVOID           EnvironmentPointer;         /* 01c */
	CLIENT_ID       ClientId;                   /* 020 */
	PVOID           ActiveRpcHandle;            /* 028 */
	PVOID           ThreadLocalStoragePointer;  /* 02c */
	PPEB            Peb;                        /* 030 */
	ULONG           LastErrorValue;             /* 034 */
	ULONG           CountOfOwnedCriticalSections;/* 038 */
	PVOID           CsrClientThread;            /* 03c */
	PVOID           Win32ThreadInfo;            /* 040 */
	ULONG           Win32ClientInfo[31];        /* 044 used for user32 private data in Wine */
	PVOID           WOW32Reserved;              /* 0c0 */
	ULONG           CurrentLocale;              /* 0c4 */
	ULONG           FpSoftwareStatusRegister;   /* 0c8 */
	PVOID           SystemReserved1[54];        /* 0cc used for kernel32 private data in Wine */
	PVOID           Spare1;                     /* 1a4 */
	LONG            ExceptionCode;              /* 1a8 */
	BYTE            SpareBytes1[40];            /* 1ac */
	PVOID           SystemReserved2[10];        /* 1d4 used for ntdll private data in Wine */
	GDI_TEB_BATCH   GdiTebBatch;                /* 1fc */
	ULONG           gdiRgn;                     /* 6dc */
	ULONG           gdiPen;                     /* 6e0 */
	ULONG           gdiBrush;                   /* 6e4 */
	CLIENT_ID       RealClientId;               /* 6e8 */
	HANDLE          GdiCachedProcessHandle;     /* 6f0 */
	ULONG           GdiClientPID;               /* 6f4 */
	ULONG           GdiClientTID;               /* 6f8 */
	PVOID           GdiThreadLocaleInfo;        /* 6fc */
	PVOID           UserReserved[5];            /* 700 */
	PVOID           glDispachTable[280];        /* 714 */
	ULONG           glReserved1[26];            /* b74 */
	PVOID           glReserved2;                /* bdc */
	PVOID           glSectionInfo;              /* be0 */
	PVOID           glSection;                  /* be4 */
	PVOID           glTable;                    /* be8 */
	PVOID           glCurrentRC;                /* bec */
	PVOID           glContext;                  /* bf0 */
	ULONG           LastStatusValue;            /* bf4 */
	UNICODE_STRING  StaticUnicodeString;        /* bf8 used by advapi32 */
	WCHAR           StaticUnicodeBuffer[261];   /* c00 used by advapi32 */
	PVOID           DeallocationStack;          /* e0c */
	PVOID           TlsSlots[64];               /* e10 */
	LIST_ENTRY      TlsLinks;                   /* f10 */
	PVOID           Vdm;                        /* f18 */
	PVOID           ReservedForNtRpc;           /* f1c */
	PVOID           DbgSsReserved[2];           /* f20 */
	ULONG           HardErrorDisabled;          /* f28 */
	PVOID           Instrumentation[16];        /* f2c */
	PVOID           WinSockData;                /* f6c */
	ULONG           GdiBatchCount;              /* f70 */
	ULONG           Spare2;                     /* f74 */
	ULONG           Spare3;                     /* f78 */
	ULONG           Spare4;                     /* f7c */
	PVOID           ReservedForOle;             /* f80 */
	ULONG           WaitingOnLoaderLock;        /* f84 */
	PVOID           Reserved5[3];               /* f88 */
	PVOID          *TlsExpansionSlots;          /* f94 */

} TEB, *PTEB;


typedef enum _SYSTEM_INFORMATION_CLASS 
{
	_SystemBasicInformation, 
	_SystemProcessorInformation, 
	SystemPerformanceInformation, 
	SystemTimeOfDayInformation, 
	SystemPathInformation, 
	SystemProcessInformation, 
	SystemCallCountInformation, 
	SystemDeviceInformation, 
	SystemProcessorPerformanceInformation, 
	SystemFlagsInformation, 
	SystemCallTimeInformation, 
	SystemModuleInformation, 
	SystemLocksInformation, 
	SystemStackTraceInformation, 
	SystemPagedPoolInformation, 
	SystemNonPagedPoolInformation, 
	SystemHandleInformation, 
	SystemObjectInformation, 
	SystemPageFileInformation, 
	SystemVdmInstemulInformation, 
	SystemVdmBopInformation, 
	SystemFileCacheInformation, 
	SystemPoolTagInformation, 
	SystemInterruptInformation, 
	SystemDpcBehaviorInformation, 
	SystemFullMemoryInformation, 
	SystemLoadGdiDriverInformation, 
	SystemUnloadGdiDriverInformation, 
	SystemTimeAdjustmentInformation, 
	SystemSummaryMemoryInformation, 
	SystemNextEventIdInformation, 
	SystemEventIdsInformation, 
	SystemCrashDumpInformation, 
	SystemExceptionInformation, 
	SystemCrashDumpStateInformation, 
	SystemKernelDebuggerInformation, 
	SystemContextSwitchInformation, 
	SystemRegistryQuotaInformation, 
	SystemExtendServiceTableInformation, 
	SystemPrioritySeperation, 
	SystemPlugPlayBusInformation, 
	SystemDockInformation, 
	_SystemPowerInformation, 
	SystemProcessorSpeedInformation, 
	SystemCurrentTimeZoneInformation, 
	SystemLookasideInformation

} SYSTEM_INFORMATION_CLASS, *PSYSTEM_INFORMATION_CLASS;


typedef BOOL (WINAPI * SetSystemFileCacheSize_t)
(
	__in  SIZE_T MinimumFileCacheSize,
	__in  SIZE_T MaximumFileCacheSize,
	__in  DWORD Flags
);

typedef NTSTATUS (NTAPI * NtSetSystemInformation_t)
(
	__in SYSTEM_INFORMATION_CLASS SystemInformationClass, 
	__in PVOID SystemInformation, 
	__in ULONG Length
);

typedef BOOL (* PrivIsDllSynchronizationHeld_t)( PBOOL );



#endif /* _INTERNALS_H__ */