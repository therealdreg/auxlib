#include <windows.h>
#include <stdio.h>
#include "aux_ulib.h"

BOOL WINAPI DllMain( HINSTANCE hinst, DWORD dwReason, LPVOID reserved )
{
    reserved = reserved;
    hinst = hinst;
	BOOL held;

    switch ( dwReason )
    {
        case DLL_PROCESS_ATTACH:
			AuxUlibInitialize();

			if ( AuxUlibIsDLLSynchronizationHeld( & held ) == TRUE )
			{
				printf
				( 
					" Thread B --> Detected LoaderLock held by Thread B...\n" 
					"      Avoid suspend Thread B in DllMain.\n\n" 
				);
			}
			else
			{
				printf( " Thread B --> Suspending Thread B... NOW PRESS ANY KEY!\n\n" );
				SuspendThread( GetCurrentThread() );
			}
        break; 

        case DLL_PROCESS_DETACH:
        break;

        default:
        break;
    }

    return TRUE;
}