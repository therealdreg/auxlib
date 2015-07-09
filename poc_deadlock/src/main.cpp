#include "main.h"

DWORD WINAPI ThreadToDeadlock( char * lpThreadParameter )
{
	printf( " Thread B --> Thread B created, Loading %s ...\n\n", lpThreadParameter );

	LoadLibraryA( lpThreadParameter );

	return 0;
}

int main( void )
{
	DWORD thread_to_deadlock_tid;

	printf
	( 
		"  - \n"
		" + This POC create a deadlock situation with LoaderLock held\n" 
		"       (NtCurrentTeb()->Peb->LoaderLock)\n"
		" + The situation:\n"
		"       - Main Thread (Thread A), Creates Thread B\n"
		"       - Thread A pass the control to Thread B with getchar().\n"
		"       - Thread B load the poc DLL.\n"
		"       - Thread B executes DllMain, send to user text for press any key.\n"
		"              (When loader call DllMain the LoaderLock is held)\n" 
		"       - Thread B DLL_PROCESS_ATTACH Supend Thread B.\n"
		"       - User press any key after or before of the suspend.\n"
		"              (But always when Thread B is in DllMain.)\n"
		"       - Thread A Enabled and Call GetModuleHandleA\n"
		"       - GetModuleHandleA waits for LoaderLock forever.\n"
		"       - Note: GetModudleHandleA uses the LoaderLock.\n"
		" + David Reguera Garcia aka Dreg - Dreg@fr33project.org\n"
		" + http://www.fr33project.org/ \n"
		"  -\n\n"
	);

	printf( " Thread A --> Main thread (this thread).\n\n" );

	printf( " Thread A --> Creating Thread B...\n\n" );
	CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE) ThreadToDeadlock, "poc_deadlock_dll.dll", 0, & thread_to_deadlock_tid );
	
	printf
	( 
		" Thread A --> Pass control to Thread B, press any key ONLY when console shows:\n"
		"     \"NOW PRESS ANY KEY!\"\n\n" 
	);
	getchar();

	printf( " Thread A --> Now, deadlocking.. executing GetModuleHandleA... the process never close... :-)\n\n" );
	GetModuleHandleA( "ntdll.dll" );

	printf( " Thread A --> This text never showed...\n\n" );
	
	return 0;
}

