#ifndef JAGAMEX86_TYPES_H
#define JAGAMEX86_TYPES_H

extern "C"
{

#define MAX_QPATH 64

typedef int ( *syscallptr_t ) ( int call, ... );

typedef float vec3_t[3];

// exact structure to be determined
typedef void gentity_t;
typedef void cvar_t;
typedef void trace_t;
typedef void usercmd_t;
typedef void* EG2_Collision;
typedef void* SavedGameJustLoaded_e;
typedef void* qhandle_t;
typedef void* memtag_t;
typedef void* Eorientations;
typedef void* mdxaBone_t;
typedef void* fileHandle_t;
typedef void sharedSetBoneIKStateParams_t;
typedef void sharedIKMoveParams_t;
typedef void CCollisionRecord;
typedef void CMiniHeap;
typedef void CRagDollParams;
typedef void CRagDollUpdateParams;
typedef int CGhoul2Info; // can't have void references
typedef int CGhoul2Info_v; // dito
typedef int IGhoul2InfoArray; //dito
typedef int SSkinGoreData; // dito

// mode parm for FS_FOpenFile
typedef enum {
	FS_READ,
	FS_WRITE,
	FS_APPEND,
	FS_APPEND_SYNC
} fsMode_t;

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

	gentity_t*	gentities;
	int			gentitySize;
	int			num_entities;
} game_export_t;

// game_import_t
// contains 131 function pointers. 131!
// functions from the engine, to be used by the gamecode dll
typedef struct
{
    //============== general Quake services ==================
     
    // print message on the local console
    void    (*Printf)( const char *fmt, ... );
     
    // Write a camera ref_tag to cameras.map
    void    (*WriteCam)( const char *text );
    void    (*FlushCamFile)();
     
    // abort the game
    void    (*Error)( int, const char *fmt, ... );
     
    // get current time for profiling reasons
    // this should NOT be used for any game related tasks,
    // because it is not journaled
    int     (*Milliseconds)( void );
     
    // console variable interaction
    cvar_t  *(*cvar)( const char *var_name, const char *value, int flags );
    void    (*cvar_set)( const char *var_name, const char *value );
    int     (*Cvar_VariableIntegerValue)( const char *var_name );
    void    (*Cvar_VariableStringBuffer)( const char *var_name, char *buffer, int bufsize );
     
    // ClientCommand and ServerCommand parameter access
    int     (*argc)( void );
    char*   (*argv)( int n );
     
    int     (*FS_FOpenFile)( const char *qpath, fileHandle_t *file, fsMode_t mode );
    int     (*FS_Read)( void *buffer, int len, fileHandle_t f );
    int     (*FS_Write)( const void *buffer, int len, fileHandle_t f );
    void    (*FS_FCloseFile)( fileHandle_t f );
    int     (*FS_ReadFile)( const char *name, void **buf );
    void    (*FS_FreeFile)( void *buf );
    int     (*FS_GetFileList)(  const char *path, const char *extension, char *listbuf, int bufsize );
     
    // Savegame handling
    //
    qboolean (*AppendToSaveGame)(unsigned long chid, const void *data, int length);
	// ppvAddressPtr default: NULL
    int     (*ReadFromSaveGame)(unsigned long chid, void *pvAddress, int iLength, void **ppvAddressPtr);
	// ppvAddressPtr default: NULL
    int     (*ReadFromSaveGameOptional)(unsigned long chid, void *pvAddress, int iLength, void **ppvAddressPtr);

    // add commands to the console as if they were typed in
    // for map changing, etc
    void    (*SendConsoleCommand)( const char *text );
     
     
    //=========== server specific functionality =============
     
    // kick a client off the server with a message
    void    (*DropClient)( int clientNum, const char *reason );
     
    // reliably sends a command string to be interpreted by the given
    // client.  If clientNum is -1, it will be sent to all clients
    void    (*SendServerCommand)( int clientNum, const char *fmt, ... );
     
    // config strings hold all the index strings, and various other information
    // that is reliably communicated to all clients
    // All of the current configstrings are sent to clients when
    // they connect, and changes are sent to all connected clients.
    // All confgstrings are cleared at each level start.
    void    (*SetConfigstring)( int num, const char *string );
    void    (*GetConfigstring)( int num, char *buffer, int bufferSize );
     
    // userinfo strings are maintained by the server system, so they
    // are persistant across level loads, while all other game visible
    // data is completely reset
    void    (*GetUserinfo)( int num, char *buffer, int bufferSize );
    void    (*SetUserinfo)( int num, const char *buffer );
     
    // the serverinfo info string has all the cvars visible to server browsers
    void    (*GetServerinfo)( char *buffer, int bufferSize );
     
    // sets mins and maxs based on the brushmodel name
    void    (*SetBrushModel)( gentity_t *ent, const char *name );
     
    // collision detection against all linked entities
	// eG2TraceType and useLod default values should be 0
    void    (*trace)( trace_t *results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end,
                    const int passEntityNum, const int contentmask , const EG2_Collision eG2TraceType, const int useLod);
     
    // point contents against all linked entities
    int             (*pointcontents)( const vec3_t point, int passEntityNum );
    // what contents are on the map?
    int             (*totalMapContents)();
     
    qboolean        (*inPVS)( const vec3_t p1, const vec3_t p2 );
    qboolean        (*inPVSIgnorePortals)( const vec3_t p1, const vec3_t p2 );
    void            (*AdjustAreaPortalState)( gentity_t *ent, qboolean open );
    qboolean        (*AreasConnected)( int area1, int area2 );
     
    // an entity will never be sent to a client or used for collision
    // if it is not passed to linkentity.  If the size, position, or
    // solidity changes, it must be relinked.
    void    (*linkentity)( gentity_t *ent );
    void    (*unlinkentity)( gentity_t *ent );              // call before removing an interactive entity
     
    // EntitiesInBox will return brush models based on their bounding box,
    // so exact determination must still be done with EntityContact
    int             (*EntitiesInBox)( const vec3_t mins, const vec3_t maxs, gentity_t **list, int maxcount );
     
    // perform an exact check against inline brush models of non-square shape
    qboolean        (*EntityContact)( const vec3_t mins, const vec3_t maxs, const gentity_t *ent );
     
    // sound volume values
    int             *VoiceVolume;
     
    // dynamic memory allocator for things that need to be freed
    void            *(*Malloc)( int iSize, memtag_t eTag, qboolean bZeroIt);        // see qcommon/tags.h for choices
    int                     (*Free)( void *buf );
    qboolean        (*bIsFromZone)( void *buf, memtag_t eTag);      // see qcommon/tags.h for choices
     
	/*
	Ghoul2 Insert Start
	*/
	qhandle_t       (*G2API_PrecacheGhoul2Model)(const char *fileName);
     
    // defualt values: customSkin = NULL, customShader = NULL, modelFlags = 0, lodBias = 0
	int                     (*G2API_InitGhoul2Model)(CGhoul2Info_v &ghoul2, const char *fileName, int modelIndex, qhandle_t customSkin, qhandle_t customShader, int modelFlags, int lodBias);
	// default values: renderSkin = 0
	qboolean        (*G2API_SetSkin)(CGhoul2Info *ghlInfo, qhandle_t customSkin, qhandle_t renderSkin );
	// default values: setFrame = -1, blendTime = -1
	qboolean        (*G2API_SetBoneAnim)(CGhoul2Info *ghlInfo, const char *boneName, const int startFrame, const int endFrame, const int flags, const float animSpeed, const int currentTime, const float setFrame, const int blendTime);
	// default values: blendTime = 0, blendStart = 0
	qboolean        (*G2API_SetBoneAngles)(CGhoul2Info *ghlInfo, const char *boneName, const vec3_t angles, const int flags, const Eorientations up, const Eorientations right, const Eorientations forward, qhandle_t *modelList, int blendTime, int blendStart);
	// default values: blendTime = 0, currentTime = 0
	qboolean        (*G2API_SetBoneAnglesIndex)(CGhoul2Info *ghlInfo, const int index, const vec3_t angles, const int flags, const Eorientations yaw, const Eorientations pitch, const Eorientations roll, qhandle_t *modelList, int blendTime, int currentTime);
	// default values: blendTime = 0, currentTime = 0
	qboolean        (*G2API_SetBoneAnglesMatrix)(CGhoul2Info *ghlInfo, const char *boneName, const mdxaBone_t &matrix, const int flags, qhandle_t *modelList, int blendTime, int currentTime);
	// default values: modelIndex = -1
	void            (*G2API_CopyGhoul2Instance)(CGhoul2Info_v &ghoul2From, CGhoul2Info_v &ghoul2To, int modelIndex);
	// default values: setFrame = -1, blendTime = -1
	qboolean        (*G2API_SetBoneAnimIndex)(CGhoul2Info *ghlInfo, const int index, const int startFrame, const int endFrame, const int flags, const float animSpeed, const int currentTime, const float setFrame, const int blendTime);
     
	qboolean        (*G2API_SetLodBias)(CGhoul2Info *ghlInfo, int lodBias);
	qboolean        (*G2API_SetShader)(CGhoul2Info *ghlInfo, qhandle_t customShader);
	qboolean        (*G2API_RemoveGhoul2Model)(CGhoul2Info_v &ghlInfo, const int modelIndex);
	qboolean        (*G2API_SetSurfaceOnOff)(CGhoul2Info *ghlInfo, const char *surfaceName, const int flags);
	qboolean        (*G2API_SetRootSurface)(CGhoul2Info_v &ghlInfo, const int modelIndex, const char *surfaceName);
	qboolean        (*G2API_RemoveSurface)(CGhoul2Info *ghlInfo, const int index);
	int                     (*G2API_AddSurface)(CGhoul2Info *ghlInfo, int surfaceNumber, int polyNumber, float BarycentricI, float BarycentricJ, int lod );
	qboolean        (*G2API_GetBoneAnim)(CGhoul2Info *ghlInfo, const char *boneName, const int currentTime, float *currentFrame,
																int *startFrame, int *endFrame, int *flags, float *animSpeed, int *modelList);
	qboolean        (*G2API_GetBoneAnimIndex)(CGhoul2Info *ghlInfo, const int iBoneIndex, const int currentTime, float *currentFrame,
																int *startFrame, int *endFrame, int *flags, float *animSpeed, int *modelList);
	qboolean        (*G2API_GetAnimRange)(CGhoul2Info *ghlInfo, const char *boneName,       int *startFrame, int *endFrame);
	qboolean        (*G2API_GetAnimRangeIndex)(CGhoul2Info *ghlInfo, const int boneIndex,   int *startFrame, int *endFrame);
     
	qboolean        (*G2API_PauseBoneAnim)(CGhoul2Info *ghlInfo, const char *boneName, const int currentTime);
	qboolean        (*G2API_PauseBoneAnimIndex)(CGhoul2Info *ghlInfo, const int boneIndex, const int currentTime);
	qboolean        (*G2API_IsPaused)(CGhoul2Info *ghlInfo, const char *boneName);
	qboolean        (*G2API_StopBoneAnim)(CGhoul2Info *ghlInfo, const char *boneName);
	qboolean        (*G2API_StopBoneAngles)(CGhoul2Info *ghlInfo, const char *boneName);
	qboolean        (*G2API_RemoveBone)(CGhoul2Info *ghlInfo, const char *boneName);
	qboolean        (*G2API_RemoveBolt)(CGhoul2Info *ghlInfo, const int index);
	int                     (*G2API_AddBolt)(CGhoul2Info *ghlInfo, const char *boneName);
	int                     (*G2API_AddBoltSurfNum)(CGhoul2Info *ghlInfo, const int surfIndex);
	qboolean        (*G2API_AttachG2Model)(CGhoul2Info *ghlInfo, CGhoul2Info *ghlInfoTo, int toBoltIndex, int toModel);
	qboolean        (*G2API_DetachG2Model)(CGhoul2Info *ghlInfo);
	qboolean        (*G2API_AttachEnt)(int *boltInfo, CGhoul2Info *ghlInfoTo, int toBoltIndex, int entNum, int toModelNum);
	void            (*G2API_DetachEnt)(int *boltInfo);
     
	qboolean        (*G2API_GetBoltMatrix)(CGhoul2Info_v &ghoul2, const int modelIndex, const int boltIndex, mdxaBone_t *matrix, const vec3_t angles, const vec3_t position, const int frameNum, qhandle_t *modelList, const vec3_t scale);
     
	void            (*G2API_ListSurfaces)(CGhoul2Info *ghlInfo);
	void            (*G2API_ListBones)(CGhoul2Info *ghlInfo, int frame);
	qboolean        (*G2API_HaveWeGhoul2Models)(CGhoul2Info_v &ghoul2);
	qboolean        (*G2API_SetGhoul2ModelFlags)(CGhoul2Info *ghlInfo, const int flags);
	int                     (*G2API_GetGhoul2ModelFlags)(CGhoul2Info *ghlInfo);
     
	qboolean        (*G2API_GetAnimFileName)(CGhoul2Info *ghlInfo, char **filename);
	void            (*G2API_CollisionDetect)(CCollisionRecord *collRecMap, CGhoul2Info_v &ghoul2, const vec3_t angles, const vec3_t position,
																			int frameNumber, int entNum, vec3_t rayStart, vec3_t rayEnd, vec3_t scale, CMiniHeap *G2VertSpace,
																			EG2_Collision eG2TraceType, int useLod, float fRadius);
	void            (*G2API_GiveMeVectorFromMatrix)(mdxaBone_t &boltMatrix, Eorientations flags, vec3_t &vec);
	void            (*G2API_CleanGhoul2Models)(CGhoul2Info_v &ghoul2);
	IGhoul2InfoArray &              (*TheGhoul2InfoArray)();
	int                     (*G2API_GetParentSurface)(CGhoul2Info *ghlInfo, const int index);
	int                     (*G2API_GetSurfaceIndex)(CGhoul2Info *ghlInfo, const char *surfaceName);
	char            *(*G2API_GetSurfaceName)(CGhoul2Info *ghlInfo, int surfNumber);
	char            *(*G2API_GetGLAName)(CGhoul2Info *ghlInfo);
	qboolean        (*G2API_SetNewOrigin)(CGhoul2Info *ghlInfo, const int boltIndex);
	int                     (*G2API_GetBoneIndex)(CGhoul2Info *ghlInfo, const char *boneName, qboolean bAddIfNotFound);
	qboolean        (*G2API_StopBoneAnglesIndex)(CGhoul2Info *ghlInfo, const int index);
	qboolean        (*G2API_StopBoneAnimIndex)(CGhoul2Info *ghlInfo, const int index);
	qboolean        (*G2API_SetBoneAnglesMatrixIndex)(CGhoul2Info *ghlInfo, const int index, const mdxaBone_t &matrix,
																		const int flags, qhandle_t *modelList, int blendTime, int currentTime);
	qboolean        (*G2API_SetAnimIndex)(CGhoul2Info *ghlInfo, const int index);
	int                     (*G2API_GetAnimIndex)(CGhoul2Info *ghlInfo);
	void            (*G2API_SaveGhoul2Models)(CGhoul2Info_v &ghoul2);
	void            (*G2API_LoadGhoul2Models)(CGhoul2Info_v &ghoul2, char *buffer);
	void            (*G2API_LoadSaveCodeDestructGhoul2Info)(CGhoul2Info_v &ghoul2);
	char            *(*G2API_GetAnimFileNameIndex)(qhandle_t modelIndex);
	char            *(*G2API_GetAnimFileInternalNameIndex)(qhandle_t modelIndex);
	int                     (*G2API_GetSurfaceRenderStatus)(CGhoul2Info *ghlInfo, const char *surfaceName);
     
	//rww - RAGDOLL_BEGIN
	void            (*G2API_SetRagDoll)(CGhoul2Info_v &ghoul2, CRagDollParams *parms);
	void            (*G2API_AnimateG2Models)(CGhoul2Info_v &ghoul2, int AcurrentTime, CRagDollUpdateParams *params);
     
	qboolean        (*G2API_RagPCJConstraint)(CGhoul2Info_v &ghoul2, const char *boneName, vec3_t min, vec3_t max);
	qboolean        (*G2API_RagPCJGradientSpeed)(CGhoul2Info_v &ghoul2, const char *boneName, const float speed);
	qboolean        (*G2API_RagEffectorGoal)(CGhoul2Info_v &ghoul2, const char *boneName, vec3_t pos);
	qboolean        (*G2API_GetRagBonePos)(CGhoul2Info_v &ghoul2, const char *boneName, vec3_t pos, vec3_t entAngles, vec3_t entPos, vec3_t entScale);
	qboolean        (*G2API_RagEffectorKick)(CGhoul2Info_v &ghoul2, const char *boneName, vec3_t velocity);
	qboolean        (*G2API_RagForceSolve)(CGhoul2Info_v &ghoul2, qboolean force);
     
	qboolean        (*G2API_SetBoneIKState)(CGhoul2Info_v &ghoul2, int time, const char *boneName, int ikState, sharedSetBoneIKStateParams_t *params);
	qboolean        (*G2API_IKMove) (CGhoul2Info_v &ghoul2, int time, sharedIKMoveParams_t *params);
	//rww - RAGDOLL_END
     
	void            (*G2API_AddSkinGore)(CGhoul2Info_v &ghoul2,SSkinGoreData &gore);
	void            (*G2API_ClearSkinGore)( CGhoul2Info_v &ghoul2 );
     
	void            (*RMG_Init)(int terrainID);
	#ifndef _XBOX
	int                     (*CM_RegisterTerrain)(const char *info);
	#endif
	const char      *(*SetActiveSubBSP)(int index);
     
     
	int                     (*RE_RegisterSkin)(const char *name);
	int                     (*RE_GetAnimationCFG)(const char *psCFGFilename, char *psDest, int iDestSize);
     
	bool            (*WE_GetWindVector)(vec3_t windVector, vec3_t atpoint);
	bool            (*WE_GetWindGusting)(vec3_t atpoint);
	bool            (*WE_IsOutside)(vec3_t pos);
	float           (*WE_IsOutsideCausingPain)(vec3_t pos);
	float           (*WE_GetChanceOfSaberFizz)(void);
	bool            (*WE_IsShaking)(vec3_t pos);
	void            (*WE_AddWeatherZone)(vec3_t mins, vec3_t maxs);
	bool            (*WE_SetTempGlobalFogColor)(vec3_t color);
     
     
	/*
	Ghoul2 Insert End
	*/
     
     
} game_import_t;


}

#endif
