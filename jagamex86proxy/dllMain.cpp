#include <cassert>
#include <cstring>
#include <cstdio>
#include <Windows.h>

#include "../Common/types.h"

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

static HMODULE g_originalJagamex86;
static HMODULE g_modJagamex86;

static game_export_t* g_exports;
static game_import_t g_imports;
/**
	@brief Set to true once imports->Shutdown() has been called.

	That means we're shutting down, so vmMain( CG_SHUTDOWN ) should cause FreeLibrary.
**/
static qboolean g_inShutdown;

typedef void (__cdecl *shutdown_t) ( void );
typedef void (__cdecl *SetOriginalFunctions_t) ( GetGameAPI_t GetGameAPI, dllEntry_t dllEntry, vmMain_t vmMain );

static shutdown_t g_realShutdown;
static vmMain_t g_realVmMain;
static dllEntry_t g_realDllEntry;
static GetGameAPI_t g_realGetGameAPI;
static shutdown_t g_modShutdown;

void ShutdownWrapper( void )
{
	assert( g_realShutdown );
	g_realShutdown();
	g_inShutdown = qtrue;
}

template< typename FncPtr > bool FindProcAddress( char* procName, char* dllName, FncPtr& out_func )
{
	if( !( out_func = (FncPtr) GetProcAddress( g_modJagamex86, procName ) ) )
	{
		g_imports.Printf( "^5Warning: No %s in %s! Ignoring it.\n", procName, dllName );
		return false;
	}
	return true;
}

/**
	@brief First function called when the DLL is loaded: Server API?
**/
extern "C" __declspec( dllexport ) game_export_t* __cdecl GetGameAPI( game_import_t* imports )
{
	// static global variables are guaranteed to be 0, I heard. Let's assert that.
	assert( !g_exports );
	assert( !g_originalJagamex86 );
	assert( !g_modJagamex86 );
	assert( !g_inShutdown );
	assert( !g_realDllEntry );
	assert( !g_realGetGameAPI );
	assert( !g_originalJagamex86 );
	assert( !g_realShutdown );
	assert( !g_realVmMain );
	assert( !g_modShutdown );

	imports->Printf( "^5Loading original_jagamex86.dll\n" );
	g_inShutdown = qfalse;

	g_originalJagamex86 = LoadLibrary( "original_jagamex86.dll" );
	if( !g_originalJagamex86 )
	{
		// imports->Error would be a bad idea because that'd jmp out of the cleanup code for failed dll loading, causing "Sys_GetGameAPI without Sys_UnloadingGame"
		imports->Printf( "^1Error: Could not load original_jagamex86.dll!\n" );
		return 0;
	}

	// Get the original functions
	if( ! (g_realGetGameAPI = (GetGameAPI_t) GetProcAddress( g_originalJagamex86, "GetGameAPI" ) ) )
	{
		imports->Printf( "^1Error: No GetGameAPI in original_jagamex86.dll!\n" );
		return 0;
	}
	if( ! (g_realDllEntry = (dllEntry_t) GetProcAddress( g_originalJagamex86, "dllEntry" ) ) )
	{
		imports->Printf( "^1Error: No dllEntry in original_jagamex86.dll!\n" );
		return 0;
	}
	if( ! (g_realVmMain = (vmMain_t) GetProcAddress( g_originalJagamex86, "vmMain" ) ) )
	{
		imports->Printf( "^1Error: No vmMain in original_jagamex86.dll!\n" );
		return 0;
	}

	// save the imports for later usage
	g_imports = *imports;

	// Get the active mod
	char fs_game[ 256 + 1 ]; // variables can be up to 256 chars - not sure about trailing \0, so + 1
	g_imports.Cvar_VariableStringBuffer( "fs_game", fs_game, sizeof( fs_game ) / sizeof( char ) );
	if( *fs_game == '\0' )
	{
		fs_game[0] = 'b';
		fs_game[1] = 'a';
		fs_game[2] = 's';
		fs_game[3] = 'e';
		fs_game[4] = '\0';
	}
	g_imports.Printf( "fs_game = %s\n", fs_game );

	if( strcmp( fs_game, "base" ) )
	{
		char dllName[ MAX_QPATH + 19 ]; // "/mod_jagamex86.dll\0" is 19 chars.
		sprintf_s( dllName, 256 + 19, "%s/mod_jagamex86.dll", fs_game );
		g_modJagamex86 = LoadLibrary( dllName );
		if( g_modJagamex86 )
		{
			g_imports.Printf( "^5Found %s - looking for functions.\n", dllName );
			GetGameAPI_t modGetGameAPI;
			vmMain_t modVmMain;
			dllEntry_t modDllEntry;
			SetOriginalFunctions_t modSetOriginalFunctions;

			if( !FindProcAddress( "GetGameAPI", dllName, modGetGameAPI ) ||
				!FindProcAddress( "vmMain", dllName, modVmMain ) ||
				!FindProcAddress( "dllEntry", dllName, modDllEntry ) ||
				!FindProcAddress( "SetOriginalFunctions", dllName, modSetOriginalFunctions ) )
			{
				FreeLibrary( g_modJagamex86 );
				g_modJagamex86 = 0;
			}
			else
			{
				FindProcAddress( "Shutdown", dllName, g_modShutdown ); // optional - not bad if it fails.
				modSetOriginalFunctions( g_realGetGameAPI, g_realDllEntry, g_realVmMain );
				g_realGetGameAPI = modGetGameAPI;
				g_realDllEntry = modDllEntry;
				g_realVmMain = modVmMain;
			}
		}
		else
		{
			g_imports.Printf( "^5No %s available - forwarding to original_jagamex86.dll.\n", dllName );
		}
	}

	// call the "real" GetGameAPI
	g_exports = g_realGetGameAPI( &g_imports );
	if( !g_exports )
	{
		// if that failed, clean up and pass on failure
		g_realGetGameAPI = 0;
		g_realDllEntry = 0;
		g_realVmMain = 0;
		assert( FreeLibrary( g_originalJagamex86 ) );
		g_originalJagamex86 = 0;
		return 0;
	}
	// Change Shutdown to our own wrapper which prepares for library freeing
	g_realShutdown = g_exports->Shutdown;
	g_exports->Shutdown = ShutdownWrapper;

	g_imports.Printf( "^5Successfully loaded original_jagamex86.dll\n" );
	return g_exports;
}

/**
	@brief Second function called when the DLL is loaded: Client API?
**/
extern "C" __declspec( dllexport ) void __cdecl dllEntry ( syscallptr_t syscallptr )
{
	assert( g_realDllEntry );
	g_realDllEntry( syscallptr );
}

extern "C" __declspec( dllexport ) int __cdecl vmMain ( int command, int arg1, int arg2 /*, ..., arg10); (?) no more than 2 seems to ever be used. */ )
{
	assert( g_realVmMain );
	int retVal = g_realVmMain( command, arg1, arg2 );
	if( command == 1 && g_inShutdown )
	{
		g_inShutdown = qfalse;

		g_realDllEntry = 0;
		g_realGetGameAPI = 0;
		g_realShutdown = 0;
		g_realVmMain = 0;

		if( g_modShutdown )
		{
			g_imports.Printf( "^5Calling mod_jagamex86.dll's Shutdown()\n" );
			g_modShutdown();
		}
		if( g_modJagamex86 )
		{
			g_imports.Printf( "^5Unloading mod_jagamex86.dll\n" );
			FreeLibrary( g_modJagamex86 );
		}

		g_imports.Printf( "^5Unloading original_jagamex86.dll\n" );

		g_exports = 0;
		assert( FreeLibrary( g_originalJagamex86 ) );
		g_originalJagamex86 = 0;
	}
	return retVal;
}
