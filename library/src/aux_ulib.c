/*++
The MIT License

Copyright (c) 2008 Dreg aka David Reguera Garcia
	http://www.fr33project.org
	Dreg@fr33project.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

Project: OpenSource Project, 
         Reverse engineering of Auxiliary Windows API Library (x86 y x86_64), Release 1.0

Module Name:

    aux_ulib.c

Abstract:

    User mode shim to access system functionality that is not properly exposed
    to applications in currently shipping operating systems.

--*/

#include "aux_ulib.h"
#include "aux_ulib_priv.h"

BOOL WINAPI AuxUlibSetSystemFileCacheSize 
(
    SIZE_T MinimumFileCacheSize,
    SIZE_T MaximumFileCacheSize,
    DWORD  Flags
)
{
	BOOL    result; 
	char    system_information[36];

	if ( AuxpInitState == 1 )
	{
		if ( AuxpUlibData.RtSetSystemFileCacheSize )
			result = AuxpUlibData.RtSetSystemFileCacheSize( MinimumFileCacheSize, MaximumFileCacheSize, Flags );
		else
		{
			if ( AuxpUlibData.dwMajorVersion >= 5 )
			{
				if ( Flags )
				{
					SetLastError( ERROR_INVALID_PARAMETER );
					result = FALSE;
				}
				else
				{
					if ( AuxpUlibData.RtNtSetSystemInformation( SystemFileCacheInformation, & system_information, sizeof( system_information ) ) >= 0 )
						result = TRUE;
					else
					{
						SetLastError( ERROR_INTERNAL_ERROR );
						result = FALSE;
					}
				}
			}
			else
			{
				SetLastError( ERROR_NOT_SUPPORTED );
				result = FALSE;
			}
		}
	}
	else
	{
		SetLastError( ERROR_INVALID_FUNCTION );
		result = FALSE;
	}

	return result;
}

BOOL WINAPI AuxUlibInitialize( VOID ) 
{
	DWORD          last_error = ERROR_SUCCESS;
	OSVERSIONINFOW os_version_info;
	HMODULE        hmodule_ntdll;
	HMODULE        hmodule_kernel32;

	if ( AuxpInitState == 0 )
	{ 
		memset( & os_version_info, 0, sizeof( os_version_info ) );
		os_version_info.dwOSVersionInfoSize = sizeof( os_version_info );
		if ( GetVersionExW( & os_version_info ) == FALSE )
		{
			last_error = GetLastError();
			goto END;
		}

		if 
		( 
			( os_version_info.dwPlatformId != VER_PLATFORM_WIN32_NT ) 
			&& 
			( os_version_info.dwPlatformId != VER_PLATFORM_WIN32_WINDOWS )
		)
		{
			last_error = ERROR_NOT_SUPPORTED;
			goto END;
		}

		hmodule_kernel32 = GetModuleHandleW( L"kernel32.dll" );
		if ( hmodule_kernel32 == NULL )
		{
			last_error = ERROR_DLL_NOT_FOUND;
			goto END;
		}

		if ( os_version_info.dwPlatformId != VER_PLATFORM_WIN32_NT )
			hmodule_ntdll = NULL;
		else
		{
			hmodule_ntdll = GetModuleHandleW(L"ntdll.dll");
			if ( hmodule_ntdll == NULL )
			{
				last_error = ERROR_DLL_NOT_FOUND;
				goto END;
			}
		}

		AuxpUlibData.RtSetSystemFileCacheSize = (SetSystemFileCacheSize_t) GetProcAddress( hmodule_kernel32, "SetSystemFileCacheSize" );
		last_error = GetLastError();
		if ( AuxpUlibData.RtSetSystemFileCacheSize || last_error == ERROR_PROC_NOT_FOUND )
		{
			if ( hmodule_ntdll )
			{
				AuxpUlibData.RtNtSetSystemInformation = (NtSetSystemInformation_t) GetProcAddress( hmodule_ntdll, "NtSetSystemInformation" );
				if ( AuxpUlibData.RtNtSetSystemInformation == NULL )
				{
					last_error = ERROR_NOT_SUPPORTED;
					goto END;
				}

				AuxpUlibData.RtPrivIsDllSynchronizationHeld = (PrivIsDllSynchronizationHeld_t) GetProcAddress( hmodule_kernel32, "PrivIsDllSynchronizationHeld" );
				if ( AuxpUlibData.RtPrivIsDllSynchronizationHeld == NULL )
				{
					last_error = GetLastError();
					if ( last_error )
					{
						if ( last_error != ERROR_PROC_NOT_FOUND )
							goto END;
					}
				}
			}
			else
				AuxpUlibData.RtPrivIsDllSynchronizationHeld = Win9xCheckDllSynchronization;

			AuxpUlibData.dwPlatformId   = os_version_info.dwPlatformId; 
			AuxpUlibData.dwMajorVersion = os_version_info.dwMajorVersion;
			AuxpUlibData.dwMinorVersion = os_version_info.dwMinorVersion;
			
			last_error = 0;

			InterlockedExchange( & AuxpInitState, 1 );
		}
	}

	END:
	if ( last_error )
	{
		SetLastError( last_error );

		return FALSE;
	}

	return TRUE;
}

BOOL WINAPI AuxUlibIsDLLSynchronizationHeld( PBOOL SynchronizationHeld )
{
	if ( DLLSynchAPISafe != 1 )
	{
		if ( AuxpInitState != 1 )
		{
			SetLastError( ERROR_INVALID_FUNCTION ); 

			return FALSE;
		}

		if ( AuxpUlibData.RtPrivIsDllSynchronizationHeld )
			return AuxpUlibData.RtPrivIsDllSynchronizationHeld( SynchronizationHeld );

		if ( DLLSynchAPISafe )
		{
			if ( DLLSynchAPISafe != 1 )
				goto NOT_SUPPORTED;
		}
		else
		{
			if ( AuxpUlibData.dwMajorVersion < 5 )
			{
				DLLSynchAPISafe = 2;
				goto NOT_SUPPORTED;
			}

			DllSynchronizationOwnerPtr = & NtCurrentTeb()->Peb->LoaderLock->OwningThread;
			
			InterlockedExchange( & DLLSynchAPISafe, 1 );
		}
	}

	if ( * DllSynchronizationOwnerPtr )
		* SynchronizationHeld = ( * DllSynchronizationOwnerPtr == GetCurrentThreadId() );
	else
		* SynchronizationHeld = 0;

	return TRUE;

	NOT_SUPPORTED:
		SetLastError( ERROR_NOT_SUPPORTED );

		return FALSE;
}

BOOL Win9xCheckDllSynchronization( PBOOL in )
{
  * in = FALSE;

  return TRUE;
}
