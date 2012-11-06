#ifndef JAGAMEX86_TYPES_H
#define JAGAMEX86_TYPES_H

extern "C"
{

typedef int ( *syscallptr_t ) ( int call, ... );

typedef void gentity_s; // exact structure to be determined

typedef void cvar_t; // exact structure to be determined, probably unchanged

typedef void usercmd_t; // exact structure to be determined

typedef void* SavedGameJustLoaded_e; // exact structure to be determined

typedef enum {
	ERR_FATAL,					// exit the entire game with a popup window
	ERR_DROP,					// print to console and disconnect from game
	ERR_DISCONNECT,				// don't kill server
	ERR_NEED_CD					// pop up the need-cd dialog
} errorTypes;

typedef enum
{
	qfalse = 0,
	qtrue = 1
} qboolean;

// game_export_t
// functions exported by the gamecode dll
typedef struct 
{
	// API Version - 8
	int apiversion;

	// shared functions

	void		(*Init)( char* unknown1, char* mapname, void* unknown2 /*, ... */ ); // unknown1 = "]%s\n", map includes leading map, e.g. "map mp/duel1" - of no interest to modders
	void		(*Shutdown) (void);

	void		(*WriteLevel) (qboolean qbAutosave);
	void		(*ReadLevel)  (qboolean qbAutosave, qboolean qbLoadTransition); // SpawnEntities() already done at this point
	qboolean	(*GameAllowedToSaveHere)(void);

	char		*(*ClientConnect)( int clientNum, qboolean firstTime, SavedGameJustLoaded_e eSavedGameJustLoaded ); // return ( mayConnect ? 0 : errorMessage );
	void		(*ClientBegin)( int clientNum, usercmd_t *cmd, SavedGameJustLoaded_e eSavedGameJustLoaded);
	void		(*ClientUserinfoChanged)( int clientNum );
	void		(*ClientDisconnect)( int clientNum );
	void		(*ClientCommand)( int clientNum );
	void		(*ClientThink)( int clientNum, usercmd_t *cmd );

	void		(*RunFrame)( int levelTime );
	void*		unknown1; // some function, calls two nav-related functions, the first doing stuff with "atst", "func_door", "func_usable", "func_wall" and navigation, the second with point_combat
	void		(*ConsoleCommand)( void );
	void*		unknown2; // some function, calls G_ParseSpawnVars and if that succeeds SP_Worldspawn

	// shared globals

	gentity_s*	gentities;
	int			gentitySize;
	int			num_entities;
} game_export_t;

// game_import_t
// contains 131 function pointers. 131!
// functions from the engine, to be used by the gamecode dll
typedef struct
{
	void		(*Printf)( const char *fmt, ... ); // 0x0041a1f0
	void		(*WriteCam)( const char *text ); // 0x0041a380
	void		(*FlushCam)( int clientNum ); // 0x0041a3e0, called on "flushcam" command
	
	void		(*Error)( int, const char *fmt, ... ); // 0x0041a440 Com_Error()
	void*		unknown002; // 0x00440ba0
	
	cvar_t*		(*cvar)( const char *var_name, const char *value, int flags ); // 0x0041bc70
	void		(*cvar_set)( const char *var_name, const char *value ); // 0x0041c0a0
	int			(*Cvar_VariableIntegerValue)( const char *var_name ); // 0x0041baf0
	void		(*Cvar_VariableStringBuffer)( const char *var_name, char *buffer, int bufsize ); // 0x0041bb30
	
	int			(*argc)( void ); // 0x00419b50
	char*		(*argv)( int index ); // 0x00419b60
	
	void*		unknown003; // 0x0041efc0 FS_FOpenFile
	void*		unknown004; // 0x0041d900 FS_Read
	void*		unknown005; // 0x0041d9f0 FS_Write
	void*		unknown006; // 0x0041d1c0 FS_FCloseFile
	void*		unknown007; // 0x0041dbf0 FS_ReadFile
	void*		unknown008; // 0x0041dcb0 FS_FreeFile
	void*		unknown009; // 0x0041e3a0 FS_GetFileList
	void*		unknown010; // 0x0043bdf0
	void*		unknown011; // 0x0043c260
	void*		unknown012; // 0x0043c280
	void*		unknown013; // 0x00419810
	void*		unknown014; // 0x00438cd0
	void*		unknown015; // 0x00438c50
	void*		unknown016; // 0x00439950
	void*		unknown017; // 0x00439a00
	void		(*GetUserinfo)( int num, char *buffer, int bufferSize ); // 0x00439ac0
	void*		unknown018; // 0x00439a70
	void*		unknown019; // 0x00439180
	void*		unknown020; // 0x00438d00
	void*		unknown021; // 0x0043d9a0
	void*		unknown022; // 0x0043dbb0
	void*		unknown023; // 0x0040d870
	void*		unknown024; // 0x00438f10
	void*		unknown025; // 0x00439000
	void*		unknown026; // 0x004390c0
	void*		unknown027; // 0x00417940
	void*		unknown028; // 0x0043cf90
	void*		unknown029; // 0x0043cf20
	void*		unknown030; // 0x0043d4b0
	void*		unknown031; // 0x00439100
	void*		unknown032; // 0x008adba0
	void*		unknown033; // 0x00439200
	void*		unknown034; // 0x00442b10
	void*		unknown035; // 0x00442aa0
	void*		unknown036; // 0x004ce900
	void*		unknown037; // 0x004ce910
	void*		unknown038; // 0x004ced00
	void*		unknown039; // 0x004cf200
	void*		unknown040; // 0x004cf760
	void*		unknown041; // 0x004cf6b0
	void*		unknown042; // 0x004cf860
	void*		unknown043; // 0x004d0420
	void*		unknown044; // 0x004cf0e0
	void*		unknown045; // 0x004cece0
	void*		unknown046; // 0x004ced30
	void*		unknown047; // 0x004cee80
	void*		unknown048; // 0x004ced50
	void*		unknown049; // 0x004ced80
	void*		unknown050; // 0x004cee00
	void*		unknown051; // 0x004cedc0
	void*		unknown052; // 0x004cf2e0
	void*		unknown053; // 0x004cf340
	void*		unknown054; // 0x004cf450
	void*		unknown055; // 0x004cf4a0
	void*		unknown056; // 0x004cf510
	void*		unknown057; // 0x004cf560
	void*		unknown058; // 0x004cf5e0
	void*		unknown059; // 0x004cf670
	void*		unknown060; // 0x004cf920
	void*		unknown061; // 0x004cf970
	void*		unknown062; // 0x004cfce0
	void*		unknown063; // 0x004cfd10
	void*		unknown064; // 0x004cfd50
	void*		unknown065; // 0x004cfd80
	void*		unknown066; // 0x004cfe00
	void*		unknown067; // 0x004cfe20
	void*		unknown068; // 0x004cfea0
	void*		unknown069; // 0x004cfeb0
	void*		unknown070; // 0x004d0030
	void*		unknown071; // 0x004d0050
	void*		unknown072; // 0x004d0070
	void*		unknown073; // 0x004d02d0
	void*		unknown074; // 0x004d0300
	void*		unknown075; // 0x004d0190
	void*		unknown076; // 0x004d01e0
	void*		unknown077; // 0x004d0320
	void*		unknown078; // 0x004ce8d0
	void*		unknown079; // 0x004ce870
	void*		unknown080; // 0x004cee30
	void*		unknown081; // 0x004d0540
	void*		unknown082; // 0x004d04f0
	void*		unknown083; // 0x004d0570
	void*		unknown084; // 0x004d0590
	void*		unknown085; // 0x004d05e0
	void*		unknown086; // 0x004cf8c0
	void*		unknown087; // 0x004cf610
	void*		unknown088; // 0x004cf7d0
	void*		unknown089; // 0x004cf060
	void*		unknown090; // 0x004cf050
	void*		unknown091; // 0x004d0620
	void*		unknown092; // 0x004d0630
	void*		unknown093; // 0x004d0640
	void*		unknown094; // 0x004d0150
	void*		unknown095; // 0x004d0170
	void*		unknown096; // 0x004cee50
	void*		unknown097; // 0x004cf960
	void*		unknown098; // 0x004cf9b0
	void*		unknown099; // 0x004cfab0
	void*		unknown100; // 0x004cfb20
	void*		unknown101; // 0x004cfb60
	void*		unknown102; // 0x004cfbc0
	void*		unknown103; // 0x004cfbd0
	void*		unknown104; // 0x004cfc40
	void*		unknown105; // 0x004cfc90
	void*		unknown106; // 0x004cfcc0
	void*		unknown107; // 0x004d0730
	void*		unknown108; // 0x004d0670
	void*		unknown109; // 0x00439220
	void*		unknown110; // 0x00439280
	void*		unknown111; // 0x00438ed0
	void*		unknown112; // 0x004676b0
	void*		unknown113; // 0x00467350
	void*		unknown114; // 0x00451600
	void*		unknown115; // 0x00451820
	void*		unknown116; // 0x004524e0
	void*		unknown117; // 0x00452560
	void*		unknown118; // 0x00454600
	void*		unknown119; // 0x00452510
	void*		unknown120; // 0x004524c0
	void*		unknown121; // 0x004525c0
} game_import_t;

}

#endif
