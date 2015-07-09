#include <windows.h>
#include <stdio.h>

BOOL WINAPI DllMain( HINSTANCE hinst, DWORD dwReason, LPVOID reserved )
{
    reserved = reserved;
    hinst = hinst;

    switch ( dwReason )
    {
        case DLL_PROCESS_ATTACH:
			printf( " Thread B --> Suspending Thread B... NOW PRESS ANY KEY!\n\n" );
			SuspendThread( GetCurrentThread() );
        break; 

        case DLL_PROCESS_DETACH:
        break;

        default:
        break;
    }

    return TRUE;
}