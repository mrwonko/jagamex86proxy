#include <assert.h>
#include <Windows.h>

#include "types.h"

/*
	Sys_LoadGameDll() // or whatever it may be called
		LoadLibrary(jagamex86.dll);
		search for GetGameAPI()
			none found? Sys_UnloadGameDll(); return 0;
		return GetGameAPI();

	Shutdown()
		gameAPI->Shutdown(); // the only place this happens!
		VM_Call(1);
		FreeLibrary(jagamex86.dll);
*/

static HMODULE g_originalJasp;

static game_export_t* g_exports;
static game_import_t g_imports;
/**
	@brief Set to true once imports->Shutdown() has been called.

	That means we're shutting down, so vmMain( CG_SHUTDOWN ) should cause FreeLibrary.
**/
static qboolean g_inShutdown;

typedef void (__cdecl *shutdown_t) ( void );
static shutdown_t g_originalShutdown;
typedef int (__cdecl *vmMain_t) ( int command, int ... );
static vmMain_t g_originalVmMain;
typedef void (__cdecl *dllEntry_t) ( syscallptr_t );
static dllEntry_t g_originalDllEntry;
typedef game_export_t* (__cdecl *GetGameAPI_t) ( game_import_t* );
static GetGameAPI_t g_originalGetGameAPI;

void ShutdownWrapper( void )
{
	assert( g_originalShutdown );
	g_originalShutdown();
	g_inShutdown = qtrue;
}

/**
	@brief First function called when the DLL is loaded: Server API?
**/
extern "C" __declspec( dllexport ) game_export_t* __cdecl GetGameAPI( game_import_t* imports )
{
	// static global variables are guaranteed to be 0, I heard. Let's assert that.
	assert( !g_exports );
	assert( !g_originalJasp );
	assert( !g_inShutdown );
	assert( !g_originalDllEntry );
	assert( !g_originalGetGameAPI );
	assert( !g_originalJasp );
	assert( !g_originalShutdown );
	assert( !g_originalVmMain );
	imports->Printf( "^5Loading original_jagamex86.dll\n" );
	g_originalJasp = LoadLibrary( "original_jagamex86.dll" );
	g_inShutdown = qfalse;
	if( !g_originalJasp )
	{
		// imports->Error would be a bad idea because that'd jmp out of the cleanup code for failed dll loading, causing "Sys_GetGameAPI without Sys_UnloadingGame"
		imports->Printf( "^1Error: Could not load original_jagamex86.dll!\n" );
		return 0;
	}
	if( ! (g_originalGetGameAPI = (GetGameAPI_t) GetProcAddress( g_originalJasp, "GetGameAPI" ) ) )
	{
		imports->Printf( "^1Error: No GetGameAPI in original_jagamex86.dll!\n" );
		return 0;
	}
	if( ! (g_originalDllEntry = (dllEntry_t) GetProcAddress( g_originalJasp, "dllEntry" ) ) )
	{
		imports->Printf( "^1Error: No dllEntry in original_jagamex86.dll!\n" );
		return 0;
	}
	if( ! (g_originalVmMain = (vmMain_t) GetProcAddress( g_originalJasp, "vmMain" ) ) )
	{
		imports->Printf( "^1Error: No vmMain in original_jagamex86.dll!\n" );
		return 0;
	}
	g_imports = *imports;
	g_exports = g_originalGetGameAPI( &g_imports );
	if( !g_exports )
	{
		g_originalGetGameAPI = 0;
		g_originalDllEntry = 0;
		g_originalVmMain = 0;
		assert( FreeLibrary( g_originalJasp ) );
		g_originalJasp = 0;
		return 0;
	}
	// Change Shutdown to our own wrapper which prepares for library freeing
	g_originalShutdown = g_exports->Shutdown;
	g_exports->Shutdown = ShutdownWrapper;
	g_imports.Printf( "^5Successfully loaded original_jagamex86.dll\n" );
	return g_exports;
}

/**
	@brief Second function called when the DLL is loaded: Client API?
**/
extern "C" __declspec( dllexport ) void __cdecl dllEntry ( syscallptr_t syscallptr )
{
	assert( g_originalDllEntry );
	g_originalDllEntry( syscallptr );
}

extern "C" __declspec( dllexport ) int __cdecl vmMain ( int command, int arg1, int arg2 /*, ... arg10? no more than 2 seems to ever be used. */ )
{
	assert( g_originalVmMain );
	int retVal = g_originalVmMain( command, arg1, arg2 );
	if( command == 1 && g_inShutdown )
	{
		g_inShutdown = qfalse;

		g_originalDllEntry = 0;
		g_originalGetGameAPI = 0;
		g_originalShutdown = 0;
		g_originalVmMain = 0;

		g_imports.Printf( "^5Unloading original_jagamex86.dll\n" );

		g_exports = 0;
		assert( FreeLibrary( g_originalJasp ) );
		g_originalJasp = 0;
	}
	return retVal;
}
