#ifndef MOD_INTERFACE_H_INCLUDED
#define MOD_INTERFACE_H_INCLUDED

#include "types.h"

/**
	@brief Sends pointers to the original jagamex86.dll functions to the mod.
	@note Called once the DLL is loaded, before anything else.
**/
extern "C" __declspec( dllexport ) void __cdecl SetOriginalFunctions ( GetGameAPI_t GetGameAPI, dllEntry_t dllEntry, vmMain_t vmMain );

/**
	@brief Optional Shutdown functions
	@note Called before the DLL is freed.
**/
extern "C" __declspec( dllexport ) void __cdecl Shutdown( void );

/**
	@brief Exchange interfaces
	@note Called once before dllEntry is called.
	@param imports Functions from jasp.exe to jagamex86.dll. Guaranteed to point to a valid object from now until Shutdown(), i.e. always.
	@return Functions from jagamex86.dll to jasp.exe
**/
extern "C" __declspec( dllexport ) game_export_t* __cdecl GetGameAPI( game_import_t* imports );

/**
	@brief Sends the pointer to the syscalls function to the dll.
	@note Called once before the first call to vmMain.
**/
extern "C" __declspec( dllexport ) void __cdecl dllEntry ( syscallptr_t syscallptr );

/**
	@brief Client Commands from jasp.exe to jagamex86.dll
	@note Number of significant arguements depends on command.
**/
extern "C" __declspec( dllexport ) int __cdecl vmMain ( int command, int arg1, int arg2 /*, ..., arg10); (?) no more than 2 seems to ever be used. */ );

#endif
