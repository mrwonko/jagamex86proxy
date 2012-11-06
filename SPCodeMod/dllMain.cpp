#include <cassert>
#include "../Common/modInterface.h"

static vmMain_t g_realVmMain;
static dllEntry_t g_realDllEntry;
static GetGameAPI_t g_realGetGameAPI;
static game_import_t* g_imports;

extern "C" __declspec( dllexport ) void __cdecl SetOriginalFunctions ( GetGameAPI_t GetGameAPI, dllEntry_t dllEntry, vmMain_t vmMain )
{
	g_realGetGameAPI = GetGameAPI;
	g_realDllEntry = dllEntry;
	g_realVmMain = vmMain;
}

extern "C" __declspec( dllexport ) void __cdecl Shutdown( void )
{
	g_imports->Printf( "Example Mod says Bye Bye!\n" );
}

extern "C" __declspec( dllexport ) game_export_t* __cdecl GetGameAPI( game_import_t* imports )
{
	assert( g_realGetGameAPI );
	g_imports = imports;
	g_imports->Printf( "Example Mod says Hello World!\n" );
	return g_realGetGameAPI( imports );
}

extern "C" __declspec( dllexport ) void __cdecl dllEntry ( syscallptr_t syscallptr )
{
	assert( g_realDllEntry );
	g_realDllEntry( syscallptr );
}

extern "C" __declspec( dllexport ) int __cdecl vmMain ( int command, int arg1, int arg2 /*, ..., arg10); (?) no more than 2 seems to ever be used. */ )
{
	assert( g_realVmMain );
	return g_realVmMain( command, arg1, arg2 );
}
