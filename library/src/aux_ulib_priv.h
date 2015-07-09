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

    aux_ulib_priv.h

Abstract:

    User mode shim to access system functionality that is not properly exposed
    to applications in currently shipping operating systems.

--*/

#ifndef _AUX_ULIB_PRIV_H__
#define _AUX_ULIB_PRIV_H__


#include <windows.h>
#include "internals.h"

typedef struct AuxpUlibData_s
{
	DWORD                          dwPlatformId;
	DWORD                          dwMajorVersion;
	DWORD                          dwMinorVersion;
	SetSystemFileCacheSize_t       RtSetSystemFileCacheSize;
	NtSetSystemInformation_t       RtNtSetSystemInformation;
	PrivIsDllSynchronizationHeld_t RtPrivIsDllSynchronizationHeld;

} AuxpUlibData_t;

AuxpUlibData_t          AuxpUlibData;
LONG                    AuxpInitState;
LONG                    DLLSynchAPISafe;
HANDLE                * DllSynchronizationOwnerPtr;

static BOOL Win9xCheckDllSynchronization( PBOOL );


#endif /* _AUX_ULIB_PRIV_H__*/
