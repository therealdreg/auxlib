#include "main.h"

DWORD WINAPI ThreadToDeadlock( char * lpThreadParameter )
{
	printf( " Thread B --> Thread B created, Loading %s ...\n\n", lpThreadParameter );

	LoadLibraryA( lpThreadParameter );

	printf( " Thread B --> DllMain returns... NOW PRESS ANY KEY!\n\n" );

	return 0;
}

int main( void )
{
	DWORD thread_to_deadlock_tid;

	printf
	( 
		"  - \n"
		" + This POC avoid deadlock situation with LoaderLock held\n" 
		"       (NtCurrentTeb()->Peb->LoaderLock)\n"
		" + The situation:\n"
		"       - Main Thread (Thread A), Creates Thread B\n"
		"       - Thread A pass the control to Thread B with getchar().\n"
		"       - Thread B load the poc DLL.\n"
		"       - Thread B executes DllMain.\n"
		"              (When loader call DllMain the LoaderLock is held)\n" 
		"       - Thread B DLL_PROCESS_ATTACH detects the LoaderLock held and returns.\n"
		"       - User press any key after execution of DllMain.\n"
		"              (When DllMain returns LoadLock is not held.)\n"
		"		- Thread B Shows something like PRESS ANY KEY and... \n"
		"       - Thread A calls GetModuleHandleA without DeadLocks\n"
		"       - Note: GetModudleHandleA held the LoaderLock.\n"
		" + David Reguera Garcia aka Dreg - Dreg@fr33project.org\n"
		" + http://www.fr33project.org/ \n"
		"  -\n\n"
	);

	printf( " Thread A --> Main thread (this thread).\n\n" );

	printf( " Thread A --> Creating Thread B...\n\n" );
	CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE) ThreadToDeadlock, "poc_without_deadlock_dll.dll", 0, & thread_to_deadlock_tid );
	
	printf
	( 
		" Thread A --> Pass control to Thread B, press any key ONLY when console shows:\n"
		"     \"NOW PRESS ANY KEY!\"\n\n" 
	);
	getchar();

	printf( " Thread A --> Calling GetModuleHandleA without DeadLocks...\n\n" );

	GetModuleHandleA( "ntdll.dll" );

	printf( " Thread A --> This text only is showed with aux library ...\n\n" );

	printf( " Thread A --> Press Any key to ExitProcess" ); 

	getchar();

	ExitProcess( 0 );
	
	return 0;
}

