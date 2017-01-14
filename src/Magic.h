//-----------------------------------------------------------------------------
// File: magic.h
//
// Copyright (c) Astralax. All rights reserved.
// Author: Sedov Alexey
//-----------------------------------------------------------------------------

#ifndef MAGIC_PARTICLES_LIBRARY
#define MAGIC_PARTICLES_LIBRARY

#define MAGIC_API	"3.42"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __ARMCC_VERSION
#pragma anon_unions
#endif


typedef int HM_FILE;
typedef int HM_EMITTER;
typedef int HM_IMPORT;
typedef int HM_OBSTACLE;
typedef int HM_WIND;
typedef unsigned int HM_PARTICLE;
typedef int HM_STREAM;


#define MAGIC_PI 3.1415926535897932384626433832795028841971693993751058209

#define MAGIC_SUCCESS	-1
#define MAGIC_ERROR		-2
#define MAGIC_UNKNOWN	-3

// eng: MAGIC_POSITION - structure to define coordinates
// rus: MAGIC_POSITION - ��������� ��� �������� ���������
struct MAGIC_POSITION
{
	float x;
	float y;
	#ifdef MAGIC_3D
	float z;
	#endif
};

// eng: MAGIC_DIRECTION - structure to define direction
// rus: MAGIC_DIRECTION - ��������� ��� �������� �����������
struct MAGIC_DIRECTION
{
	#ifdef MAGIC_3D
	float x,y,z,w;
	#else
	float angle;
	#endif
};

// eng: MAGIC_FIND_DATA - structure that is used in searching emitters and directories
// rus: MAGIC_FIND_DATA - c�������� ��� �������� ����� � ��������� � ������� �����
struct MAGIC_FIND_DATA
{
	// eng: result
	// rus: ���������
	int type;
	const char* name;
	int animate;

	int mode;

	// eng: additional data
	// rus: �������������� ������
	void* folder;
	int index;
};
#define MAGIC_FOLDER	1
#define MAGIC_EMITTER	2

// eng: MAGIC_PARTICLE - particle structure, containing its properties
// rus: MAGIC_PARTICLE - ��������� �������, ����������� �� ��������������. 
struct MAGIC_PARTICLE
{
	MAGIC_POSITION position;
	float size;
	float size_factor;
	float angle;
	unsigned int color;
};

// eng: MAGIC_TEXTURE - structure, containing texture frame-file information
// rus: MAGIC_TEXTURE - ���������, �������� ���������� � ��������
struct MAGIC_TEXTURE
{
	unsigned int length;
	const char* data;
	int crc;
	const char* file;
	const char* path;

	float left,top,right,bottom;

	int frame_width;
	int frame_height;

	int texture_width;
	int texture_height;

	float pivot_x;
	float pivot_y;

	float scale;

	int source_frame_width;
	int source_frame_height;
	int optimized_frame_x;
	int optimized_frame_y;
	int optimized_frame_width;
	int optimized_frame_height;

	// -------------------------------
	unsigned int id;
	int type;
	
	void* material_properties;
	void* texture_properties;

	float material_source_frame_width;
	float material_source_frame_height;

	int optimized_frame_x_const;
	int optimized_frame_y_const;
	int optimized_frame_width_const;
	int optimized_frame_height_const;
};

// eng: MAGIC_ATLAS - structure, containing information on frame file locations within the textural atlas
// rus: MAGIC_ATLAS - ���������, �������� ���������� � ������������ ����-������ �� ���������� ������
struct MAGIC_ATLAS
{
	int width;
	int height;
	int count;
	MAGIC_TEXTURE** textures;
};

// eng: MAGIC_STATIC_ATLAS - structure, containing information of static textural atlas 
// rus: MAGIC_STATIC_ATLAS - ���������, �������� ���������� � ����������� ���������� ������
struct MAGIC_STATIC_ATLAS
{
	const char* file;
	const char* path;
	int width, height;
	unsigned int ptc_id;
};

// eng: MAGIC_CHANGE_ATLAS - structure, containing information on changes in textural atlas
// rus: MAGIC_CHANGE_ATLAS - ���������, �������� ���������� �� ��������� � ���������� ������
enum MAGIC_CHANGE_ATLAS_ENUM {MAGIC_CHANGE_ATLAS_CREATE, MAGIC_CHANGE_ATLAS_DELETE, MAGIC_CHANGE_ATLAS_LOAD, MAGIC_CHANGE_ATLAS_CLEAN, MAGIC_CHANGE_ATLAS__MAX};
struct MAGIC_CHANGE_ATLAS
{
	MAGIC_CHANGE_ATLAS_ENUM type;
	int index;
	HM_EMITTER emitter;
	int x,y;
	int width, height;
	const char* file;
	const char* path;
	unsigned int length;
	const char* data;
	unsigned int ptc_id;
};

// eng: MAGIC_KEY - structure, that allows modifying keys on a Timeline
// rus: MAGIC_KEY - ���������, ����������� �������������� ����� �� ����� �������
struct MAGIC_KEY
{
	double time;

	MAGIC_POSITION position;
	MAGIC_POSITION s1,s2;
	
	float scale;

	int number;
	MAGIC_DIRECTION direction;

	float opacity;
};
enum MAGIC_KEY_ENUM {MAGIC_KEY_POSITION, MAGIC_KEY_SCALE, MAGIC_KEY_DIRECTION, MAGIC_KEY_OPACITY, MAGIC_KEY__MAX};

// eng: MAGIC_CAMERA - structure describing the camera
// rus: MAGIC_CAMERA - ���������, ����������� ������
enum MAGIC_CAMERA_ENUM {MAGIC_CAMERA_FREE, MAGIC_CAMERA_PERSPECTIVE, MAGIC_CAMERA_ORTHO, MAGIC_CAMERA__MAX};
struct MAGIC_CAMERA
{
	MAGIC_CAMERA_ENUM mode;
	MAGIC_POSITION pos;
	MAGIC_POSITION dir;
};

// eng: MAGIC_VIEW - structure which contains all settings for Projection Matrix and View Matrix
// rus: MAGIC_VIEW - ���������, ����������� ��� ��������� ��� ������� �������� � ������� ����
struct MAGIC_VIEW
{
	MAGIC_POSITION pos;
	MAGIC_POSITION dir;
	MAGIC_POSITION up;
	float fov;
	float aspect_ratio;
	float znear;
	float zfar;
	int viewport_width;
	int viewport_height;
};

// eng: MAGIC_TAIL - structure for constructing "tail" from particles
// rus: MAGIC_TAIL - ��������� ��� ���������� "������" �� ������
enum MAGIC_TAIL_ENUM {MAGIC_TAIL_EXISTING_PARTICLES_TAIL, MAGIC_TAIL_EXISTING_PARTICLES_DESTROY, MAGIC_TAIL_EXISTING_PARTICLES_MOVE, MAGIC_TAIL_EXISTING_PARTICLES_NOMOVE, MAGIC_TAIL_EXISTING_PARTICLES_MOVE_DEFAULT, MAGIC_TAIL_EXISTING_PARTICLES__MAX};
struct MAGIC_TAIL
{
	MAGIC_TAIL_ENUM existing_particles;
	float factor;
	int count;
	float distance;
	float step;
	bool rand;
	bool single_source;
	bool direction;
	float animation_position;
	float over_life1, over_life2;
	float size1, size2;
	bool emitter_end;
};

// eng: MAGIC_BBOX - structure to define Bounds Box
// rus: MAGIC_BBOX - ��������� ��� �������� Bounds Box
struct MAGIC_BBOX
{
	MAGIC_POSITION corner1;
	MAGIC_POSITION corner2;
};

// eng: MAGIC_LIMITER - structure to restrict the region of "creating" of new particles
// rus: MAGIC_LIMITER - ��������� ��� ����������� ������� "��������" ����� ������
enum MAGIC_LIMITER_ENUM {MAGIC_LIMITER_DISABLED, MAGIC_LIMITER_RECTANGLE, MAGIC_LIMITER_CIRCLE, MAGIC_LIMITER_PARALLELEPIPED, MAGIC_LIMITER_SPHERE, MAGIC_LIMITER__MAX};
struct MAGIC_LIMITER
{
	MAGIC_LIMITER_ENUM type;

	MAGIC_POSITION position;
	MAGIC_POSITION size;
	float radius;
};

// eng: MAGIC_ORIENTATION - structure for storing particles type orientation
// rus: MAGIC_ORIENTATION - ��������� ��� ��������� ���������� ���� ������
enum MAGIC_ORIENTATION_ENUM {MAGIC_ORIENTATION_X, MAGIC_ORIENTATION_Y, MAGIC_ORIENTATION_Z, MAGIC_ORIENTATION_AXIS, MAGIC_ORIENTATION_CAMERA, MAGIC_ORIENTATION_CAMERA_X, MAGIC_ORIENTATION_CAMERA_Y, MAGIC_ORIENTATION_CAMERA_Z, MAGIC_ORIENTATION_CAMERA_AXIS, MAGIC_ORIENTATION_DIRECTION, MAGIC_ORIENTATION__MAX};
struct MAGIC_ORIENTATION
{
	MAGIC_ORIENTATION_ENUM orientation;
	float x,y,z;
};

// eng: MAGIC_VARIABLE - structure for variables
// rus: MAGIC_VARIABLE - ��������� ��� ����������
enum MAGIC_VARIABLE_ENUM {MAGIC_VARIABLE_BOOL, MAGIC_VARIABLE_INT, MAGIC_VARIABLE_FLOAT, MAGIC_VARIABLE_STRING, MAGIC_VARIABLE_DIAGRAM, MAGIC_VARIABLE__MAX};
struct MAGIC_VARIABLE
{
	const char* name;
	MAGIC_VARIABLE_ENUM type;
	char value[8];
};

// eng: MAGIC_TRIANGLE - structure to define a triangle
// rus: MAGIC_TRIANGLE - ��������� ��� �������� ������������
struct MAGIC_TRIANGLE
{
	MAGIC_POSITION vertex1;
	MAGIC_POSITION vertex2;
	MAGIC_POSITION vertex3;
};

// eng: MAGIC_WIND - structure that defines wind
// rus: MAGIC_WIND - ���������, ������� ��������� �����
struct MAGIC_WIND
{
	MAGIC_POSITION direction;
	float velocity;
};

// eng: MAGIC_SEGMENT - structure to store line segment coordinates.
// rus: MAGIC_SEGMENT - ��������� ��� �������� ��������� �������.
struct MAGIC_SEGMENT
{
	MAGIC_POSITION vertex1;
	MAGIC_POSITION vertex2;
};

// eng: MAGIC_RECT - structure to store rectangle
// rus: MAGIC_RECT - ��������� ��� �������� ��������������
struct MAGIC_RECT
{
	int left;
	int top;
	int right;
	int bottom;
};

// eng: MAGIC_ACTION - structure for actions
// rus: MAGIC_ACTION - ��������� ��� ��������
enum MAGIC_EVENT_ENUM {MAGIC_EVENT_CREATION, MAGIC_EVENT_DESTRUCTION, MAGIC_EVENT_EXISTENCE, MAGIC_EVENT_COLLISION, MAGIC_EVENT_MAGNET, MAGIC_EVENT_WIND, MAGIC_EVENT__MAX};
enum MAGIC_ACTION_ENUM {MAGIC_ACTION_EVENT, MAGIC_ACTION_DESTRUCTION, MAGIC_ACTION_DETACHING, MAGIC_ACTION_FACTOR, MAGIC_ACTION_PARTICLE, MAGIC_ACTION_MAGNET_PARTICLE, MAGIC_ACTION__MAX, MAGIC_ACTION__ERROR=-1};
enum MAGIC_ACTION_DIRECTION_ENUM {MAGIC_ACTION_DIRECTION_DISABLED, MAGIC_ACTION_DIRECTION_MOVEMENT, MAGIC_ACTION_DIRECTION_REBOUND, MAGIC_ACTION_DIRECTION__MAX};
enum MAGIC_MAGNET_POINT_ENUM {MAGIC_MAGNET_POINT_ANY, MAGIC_MAGNET_POINT_DIRECTION, MAGIC_MAGNET_POINT_OPPOSITE_DIRECTION, MAGIC_MAGNET_POINT__MAX};
struct MAGIC_ACTION
{
	MAGIC_EVENT_ENUM event;
	int HM;
	MAGIC_ACTION_ENUM action;
	float factor;

	int magnet_particles_type;
	MAGIC_MAGNET_POINT_ENUM magnet_direction;
	int magnet_distance;
	float magnet_strength1;
	float magnet_strength2;

	HM_EMITTER creating_emitter;
	int particles_type;
	MAGIC_ACTION_DIRECTION_ENUM direction;
	float angle;
	float size;
	float velocity;
	float weight;
	float spin;
	float angular_velocity;
	float motion_rand;
	float visibility;
};

// eng: MAGIC_EVENT - structure to get event information
// rus: MAGIC_EVENT - ��������� ��� ��������� ���������� � �������
struct MAGIC_EVENT
{
	MAGIC_EVENT_ENUM event;
	HM_PARTICLE hmParticle;
	MAGIC_POSITION position1;
	MAGIC_POSITION position2;
	MAGIC_POSITION reflection;
};

// eng: MAGIC_OBSTACLE - structure defining the shape of obstacle
// rus: MAGIC_OBSTACLE - ���������, ������� ��������� ����� �����������
#if defined(EXE) || defined(SCREENSAVER)
enum MAGIC_OBSTACLE_ENUM {MAGIC_OBSTACLE_CIRCLE, MAGIC_OBSTACLE_SEGMENT, MAGIC_OBSTACLE_SPHERE, MAGIC_OBSTACLE_TRIANGLE, MAGIC_OBSTACLE__MAX};
#else
#ifdef MAGIC_3D
enum MAGIC_OBSTACLE_ENUM {MAGIC_OBSTACLE_SPHERE=3, MAGIC_OBSTACLE_TRIANGLE, MAGIC_OBSTACLE__MAX};
#else
enum MAGIC_OBSTACLE_ENUM {MAGIC_OBSTACLE_CIRCLE, MAGIC_OBSTACLE_SEGMENT, MAGIC_OBSTACLE__MAX};
#endif
#endif
// MAGIC_OBSTACLE_CIRCLE		circle/���������� (2D)
// MAGIC_OBSTACLE_SEGMENT		segment/������� (2D)
// MAGIC_OBSTACLE_SPHERE		sphere/����� (3D)
// MAGIC_OBSTACLE_TRIANGLE		triangle/����������� (3D)
struct MAGIC_OBSTACLE
{
	MAGIC_OBSTACLE_ENUM type;
	float radius;
	int count;
	#if defined(EXE) || defined(SCREENSAVER)
	void* primitives;
	#else
	#ifdef MAGIC_3D
	MAGIC_TRIANGLE* primitives;
	#else
	MAGIC_SEGMENT* primitives;
	#endif
	#endif
};

// eng: MAGIC_VERTEX_FORMAT - structure that defines format of vertex buffer
// rus: MAGIC_VERTEX_FORMAT - ��������� ��� �������� ������� ���������� ������
struct MAGIC_VERTEX_FORMAT
{
	int attributes;
	int UVs;
};
#define MAGIC_ATTRIBUTE_COLOR	1
#define MAGIC_ATTRIBUTE_NORMAL	2
#define MAGIC_ATTRIBUTE_TANGENT	4
#define MAGIC_ATTRIBUTE_SHADER	8

// eng: MAGIC_RENDERING_START - structure for emitter visualization
// rus: MAGIC_RENDERING_START - ��������� ��� ������������ ��������
struct MAGIC_RENDERING_START
{
	int particles;				// ���������� ������
	int arrays;					// ���������� ��������
	MAGIC_VERTEX_FORMAT format;	// ��������� ������ ������
	int vertices;				// ���������� ������
	int indexes;				// ���������� ��������
	int textures;				// ���������� �������
};

#define MAGIC_BLENDING_NORMAL		0
#define MAGIC_BLENDING_ADD			1
#define MAGIC_BLENDING_OPACITY		2
#define MAGIC_BLENDING_MASK			3
#define MAGIC_BLENDING_SHADER		4

#define MAGIC_TEXADDRESS_WRAP		0
#define MAGIC_TEXADDRESS_MIRROR		1
#define MAGIC_TEXADDRESS_CLAMP		2
#define MAGIC_TEXADDRESS_BORDER		3

#define MAGIC_TEXOP_ARGUMENT1	0
#define MAGIC_TEXOP_ADD			1
#define MAGIC_TEXOP_SUBTRACT	2
#define MAGIC_TEXOP_MODULATE	3
#define MAGIC_TEXOP_MODULATE2X	4
#define MAGIC_TEXOP_MODULATE4X	5

#define MAGIC_TEXARG_CURRENT	0
#define MAGIC_TEXARG_DIFFUSE	1
#define MAGIC_TEXARG_TEXTURE	2

// eng: MAGIC_ARRAY_INFO - structure for description of one array of attribute
// rus: MAGIC_ARRAY_INFO - ��������� ��� �������� ������ ������� ��������
enum MAGIC_VERTEX_FORMAT_ENUM {MAGIC_VERTEX_FORMAT_INDEX, MAGIC_VERTEX_FORMAT_POSITION, MAGIC_VERTEX_FORMAT_COLOR, MAGIC_VERTEX_FORMAT_UV, MAGIC_VERTEX_FORMAT_NORMAL, MAGIC_VERTEX_FORMAT_TANGENT, MAGIC_VERTEX_FORMAT_BINORMAL, MAGIC_VERTEX_FORMAT_ALL, MAGIC_VERTEX_FORMAT__MAX};
struct MAGIC_ARRAY_INFO
{
	MAGIC_VERTEX_FORMAT_ENUM type;
	int index;
	int length;
	int bytes_per_one;
	int locked_start;
	int locked_length;
};

// eng: MAGIC_RENDER_STATE - structure that contains the change of render state
// rus: MAGIC_RENDER_STATE - ���������, ���������� ������ ��������� �������.
enum MAGIC_RENDER_STATE_ENUM {MAGIC_RENDER_STATE_BLENDING, MAGIC_RENDER_STATE_TEXTURE_COUNT, MAGIC_RENDER_STATE_TEXTURE, MAGIC_RENDER_STATE_ADDRESS_U, MAGIC_RENDER_STATE_ADDRESS_V, MAGIC_RENDER_STATE_OPERATION_RGB, MAGIC_RENDER_STATE_ARGUMENT1_RGB, MAGIC_RENDER_STATE_ARGUMENT2_RGB, MAGIC_RENDER_STATE_OPERATION_ALPHA, MAGIC_RENDER_STATE_ARGUMENT1_ALPHA, MAGIC_RENDER_STATE_ARGUMENT2_ALPHA, MAGIC_RENDER_STATE_ZENABLE, MAGIC_RENDER_STATE_ZWRITE, MAGIC_RENDER_STATE_ALPHATEST_INIT, MAGIC_RENDER_STATE_ALPHATEST, MAGIC_RENDER_STATE_TECHNIQUE_ON, MAGIC_RENDER_STATE_TECHNIQUE_OFF, MAGIC_RENDER_STATE__MAX, MAGIC_RENDER_STATE__ERROR=-1};
struct MAGIC_RENDER_STATE
{
	MAGIC_RENDER_STATE_ENUM state;		// ����
	int value;							// ��������
	int index;							// �������������� ������, ������� ������ ���������� ���������� ����
};

// eng: MAGIC_RENDER_VERTICES - structure for visualization of one portion of particles
// rus: MAGIC_RENDER_VERTICES - ��������� ��� ������������ ����� ������ ������
struct MAGIC_RENDER_VERTICES
{
	int starting_index;
	int indexes_count;
	int material;
};

// eng: MAGIC_TEXTURE_STATES - structure that describes texture parameters
// rus: MAGIC_TEXTURE_STATES - ���������, ����������� ��������� ��������
struct MAGIC_TEXTURE_STATES
{
	int address_u, address_v;				// ��������� �������� �� U � V
	int operation_rgb;						// ���������� �������� rgb
	int argument_rgb1, argument_rgb2;		// ��������� ���������� �������� rgb
	int operation_alpha;					// ���������� �������� alpha
	int argument_alpha1, argument_alpha2;	// ��������� ���������� �������� alpha
};

// eng: MAGIC_MATERIAL - structure that describes the material
// rus: MAGIC_MATERIAL - ���������, ����������� ��������� ���������
struct MAGIC_MATERIAL
{
	int blending;

	int textures;
	MAGIC_TEXTURE_STATES* states;

	int flags;

	MAGIC_VERTEX_FORMAT format;
};
#define MAGIC_MATERIAL_ALPHATEST	1
#define MAGIC_MATERIAL_ZENABLE		2
#define MAGIC_MATERIAL_ZWRITE		4



// --------------------------------------------------------------------------------------

// eng: Returns the version of API
// rus: ���������� ������ API
const char* Magic_GetVersion();

#ifdef MAGIC_3D

// eng: Returns the camera that is set for API
// rus: ���������� ������, ������������� ��� API
void Magic_GetCamera(MAGIC_CAMERA* camera);

// eng: Sets the camera for API
// rus: ������������� ������ ��� API
int Magic_SetCamera(MAGIC_CAMERA* camera);

#endif

#ifdef MAGIC_3D
enum MAGIC_AXIS_ENUM {MAGIC_pXpYpZ, MAGIC_pYpXpZ, MAGIC_pZpXpY, MAGIC_pXpZpY, MAGIC_pYpZpX, MAGIC_pZpYpX, MAGIC_nXpYpZ, MAGIC_pYnXpZ, MAGIC_pZnXpY, MAGIC_nXpZpY, MAGIC_pYpZnX, MAGIC_pZpYnX, MAGIC_pXnYpZ, MAGIC_nYpXpZ, MAGIC_pZpXnY, MAGIC_pXpZnY, MAGIC_nYpZpX, MAGIC_pZnYpX, MAGIC_pXpYnZ, MAGIC_pYpXnZ, MAGIC_nZpXpY, MAGIC_pXnZpY, MAGIC_pYnZpX, MAGIC_nZpYpX, MAGIC_nXnYpZ, MAGIC_nYnXpZ, MAGIC_pZnXnY, MAGIC_nXpZnY, MAGIC_nYpZnX, MAGIC_pZnYnX, MAGIC_nXpYnZ, MAGIC_pYnXnZ, MAGIC_nZnXpY, MAGIC_nXnZpY, MAGIC_pYnZnX, MAGIC_nZpYnX, MAGIC_pXnYnZ, MAGIC_nYpXnZ, MAGIC_nZpXnY, MAGIC_pXnZnY, MAGIC_nYnZpX, MAGIC_nZnYpX, MAGIC_nXnYnZ, MAGIC_nYnXnZ, MAGIC_nZnXnY, MAGIC_nXnZnY, MAGIC_nYnZnX, MAGIC_nZnYnX, MAGIC_AXIS__MAX};
#else
enum MAGIC_AXIS_ENUM {MAGIC_pXpY, MAGIC_pYpX, MAGIC_nXpY, MAGIC_pYnX, MAGIC_pXnY, MAGIC_nYpX, MAGIC_nXnY, MAGIC_nYnX, MAGIC_AXIS__MAX};
#endif

// eng: Returns the direction of coordinate axes
// rus: ���������� ����������� ������������ ����
MAGIC_AXIS_ENUM Magic_GetAxis();

// eng: Sets the direction of coordinate axes
// rus: ������������� ����������� ������������ ����
int Magic_SetAxis(MAGIC_AXIS_ENUM axis_index);

#ifdef MAGIC_3D
// eng: Returns the flag of the usage of Z-buffer during the visualization
// rus: ���������� ������� ������������� Z-buffer-� �� ����� ������������
bool Magic_IsZBufferEnabled();

// eng: Enables/Disables the usage of Z-buffer during the visualization
// rus: ���������/��������� ������������� Z-buffer-� �� ����� ������������
void Magic_EnableZBuffer(bool enable);
#endif

// eng: Sets filter on the states of render changes
// rus: ������������� ������ �� ������� ��������� �������
void Magic_SetRenderStateFilter(bool* filters, bool optimization);

// eng: Returns the number of materials
// rus: ���������� ���������� ����������
int Magic_GetMaterialCount();

// eng: Returns the info about material
// rus: ���������� ���������� � ���������
int Magic_GetMaterial(int index, MAGIC_MATERIAL* material);

// eng: Converts UTF8 string into UTF16
// rus: ������������ ������ ���� UTF8 � ������ ���� UTF16
const unsigned short* Magic_UTF8to16(const unsigned char* str);

// eng: Converts UTF8 string into UTF32
// rus: ������������ ������ ���� UTF8 � ������ ���� UTF32
const unsigned int* Magic_UTF8to32(const unsigned char* str);

// eng: Converts UTF16 string into UTF8
// rus: ������������ ������ ���� UTF16 � ������ ���� UTF8
const unsigned char* Magic_UTF16to8(const unsigned short* str);

// eng: Converts UTF16 string into UTF32
// rus: ������������ ������ ���� UTF16 � ������ ���� UTF32
const unsigned int* Magic_UTF16to32(const unsigned short* str);

// eng: Converts UTF32 string into UTF8
// rus: ������������ ������ ���� UTF32 � ������ ���� UTF8
const unsigned char* Magic_UTF32to8(const unsigned int* str);

// eng: Converts UTF32 string into UTF16
// rus: ������������ ������ ���� UTF32 � ������ ���� UTF16
const unsigned short* Magic_UTF32to16(const unsigned int* str);

// eng: Populates MAGIC_ACTION structure with default values
// rus: ��������� ��������� MAGIC_ACTION ���������� �� ���������
void MAGIC_ACTION_Identity(MAGIC_ACTION* action);

// --------------------------------------------------------------------------------------


// eng: Loads the ptc-file from the path specified
// rus: ��������� ptc-���� �� ���������� ����
HM_FILE Magic_OpenFile(const char* file_name);

// eng: Loads the ptc-file image from the memory
// rus: �������� ������ ptc-����� �� ������
HM_FILE Magic_OpenFileInMemory(const char* buffer);

// eng: Loads the ptc-file from the stream
// rus: ��������� ptc-���� �� ���������� ������
int Magic_OpenStream(HM_STREAM hmStream);

// eng: Closes the file, opened earlier by use of Magic_OpenFile, Magic_OpenFileInMemory or Magic_OpenStream
// rus: ��������� ����, �������� ����� ����� Magic_OpenFile, Magic_OpenFileInMemory ��� Magic_OpenStream
int Magic_CloseFile(HM_FILE hmFile);

// eng: Closing all the opened files
// rus: �������� ���� �������� ������
void Magic_CloseAllFiles();

// eng: Returns the current folder path
// rus: ���������� ������ ���� � ������� �����
const char* Magic_GetCurrentFolder(HM_FILE hmFile);

// eng: Sets the new current folder
// rus: ���������� ����� ���� � ������� �����
int Magic_SetCurrentFolder(HM_FILE hmFile, const char* path);

// eng: Searches for the first folder or emitter within the current folder and returns the type of the object found
// rus: ���� ������ ����� ��� ������ ������� � ������� ����� � ���������� ��� � ��� ���������� �������
const char* Magic_FindFirst(HM_FILE hmFile,MAGIC_FIND_DATA* data,int mode);

// eng: Searches for the next folder or emitter within the current folder and returns the type of the object found
// rus: ���� ��������� ����� ��� ��������� ������� � ������� ����� � ���������� ��� � ��� ���������� �������
const char* Magic_FindNext(HM_FILE hmFile, MAGIC_FIND_DATA* data);

// eng: Returns the name of the file that was opened through the Magic_OpenFile
// rus: ���������� ��� �����, ��������� ����� Magic_OpenFile
const char* Magic_GetFileName(HM_FILE hmFile);

// eng: Returns the flag indicating that textures are stored within the file
// rus: ���������� ������� ����, ��� ���� �������� ��������
bool Magic_HasTextures(HM_FILE hmFile);

// eng: Returns the number of static textural atlases attached to specified file
// rus: ���������� ���������� ����������� ���������� �������, ������������� � ���������� �����
int Magic_GetStaticAtlasCount(HM_FILE hmFile);

// eng: Returns information on static textural atlas attached to specified file
// rus: ���������� ���������� � ����������� ���������� ������, �������������� � ���������� �����
int Magic_GetStaticAtlas(HM_FILE hmFile, int index, MAGIC_STATIC_ATLAS* atlas);

// --------------------------------------------------------------------------------

// eng: Creates the emitter object and loads its data
// rus: ������� ������� � ��������� � ���� ������
HM_EMITTER Magic_LoadEmitter(HM_FILE hmFile, const char* name);

// eng: Gets the copy of the emitter
// rus: ��������� �������
HM_EMITTER Magic_DuplicateEmitter(HM_EMITTER hmEmitter);

// eng: Unloads the emitter data and destroys it
// rus: ��������� ������ �� �������� � ���������� ���
int Magic_UnloadEmitter(HM_EMITTER hmEitter);

// eng: Unloads all loaded emitters
// rus: ��������� ��� ����������� ��������
void Magic_UnloadAllEmitters();

// eng: Processes the emitter. Creates, displaces and removes the particles
// rus: ������������ ��������� ��������: �������, ���������� � ���������� �������
bool Magic_Update(HM_EMITTER hmEmitter, double time);

// eng: Stops the emitter
// rus: ������������� ������ ��������
int Magic_Stop(HM_EMITTER hmEmitter);

// eng: Restarts the emitter from the beginning
// rus: ������������� ������� � ������� �������
int Magic_Restart(HM_EMITTER hmEmitter);

// eng: Returns the flag showing that emitter is in interrupted mode
// rus: ���������� ������� ����, ��� ������� �����������
bool Magic_IsInterrupt(HM_EMITTER hmEmitter);

// eng: Interrupts/Starts emitter work
// rus: ��������� ��� ��������� ������ ��������
int Magic_SetInterrupt(HM_EMITTER hmEmitter, bool interrupt);

// eng: Returns the Magic Particles (Dev) time increment, used for the animation
// rus: ���������� �������� � Magic Particles ���������� �������, ������������ ��� �������� ��������
double Magic_GetUpdateTime(HM_EMITTER hmEmitter);

// eng: Returns current animation position
// rus: ���������� ������� ������� ��������
double Magic_GetPosition(HM_EMITTER hmEmitter);

// eng: Sets the current animation position
// rus: ������������� ������� ������� ��������
int Magic_SetPosition(HM_EMITTER hmEmitter,double position);

// eng: Returns animation duration
// rus: ���������� ����������������� ��������
double Magic_GetDuration(HM_EMITTER hmEmitter);

// eng: Returns the left position of the visibility range
// rus: ���������� ����� ������� ��������� ���������
double Magic_GetInterval1(HM_EMITTER hmEmitter);

// eng: Sets the left position of the visibility range
// rus: ������������� ����� ������� ��������� ���������
int Magic_SetInterval1(HM_EMITTER hmEmitter,double position);

// eng: Returns the right position of the visibility range
// rus: ���������� ������ ������� ��������� ���������
double Magic_GetInterval2(HM_EMITTER hmEmitter);

// eng: Sets the right position of the visibility range
// rus: ������������� ������ ������� ��������� ���������
int Magic_SetInterval2(HM_EMITTER hmEmitter,double position);

// eng: Figures out if the current animation position is within the visibility range
// rus: ����������, �������� �� ������� ������� �������� � �������� ���������
bool Magic_InInterval(HM_EMITTER hmEmitter);

// eng: Sets the animation position at the left position of visibility range
// rus: ������������� ������� �� ������ ������� ��������� ���������
int Magic_EmitterToInterval1(HM_EMITTER hmEmitter, float speed_factor, const char* file);

// eng: Returns the flag of the animation of emitter that begins not from 0 position
// rus: ���������� ������� ����, ��� �������� �������� ���������� �� � ������
bool Magic_IsInterval1(HM_EMITTER hmEmitter);

// eng: Copying the particles array into emitter from the file
// rus: ����������� ������������ ������ � ������� �� �����.
int Magic_LoadArrayFromFile(HM_EMITTER hmEmitter, const char* file);

// eng: Copying the particles array from the emitter into the file
// rus: ����������� ������������ ������ �������� � ����
int Magic_SaveArrayToFile(HM_EMITTER hmEmitter, const char* file);

// eng: Returns the particle positions interpolation usage flag
// rus: ���������� ������� ������ ������������ ��������
bool Magic_IsInterpolationMode(HM_EMITTER hmEmitter);

// eng: Sets/resets the particle positions interpolation usage flag
// rus: ��������/��������� ����� ������������ ��������� ������
int Magic_SetInterpolationMode(HM_EMITTER hmEmitter, bool mode);

// eng: Returns the flag of stability/randomness of the emitter behaviour
// rus: ���������� ������� ������������/����������� ��������� ��������
bool Magic_IsRandomMode(HM_EMITTER hmEmitter);

// eng: Sets the flag of stability/randomness of the emitter behaviour
// rus: ������������� ������� ������������/����������� ��������� ��������
int Magic_SetRandomMode(HM_EMITTER hmEmitter,bool mode);


enum MAGIC_LOOP_ENUM {MAGIC_NOLOOP, MAGIC_LOOP, MAGIC_FOREVER, MAGIC_LOOP__MAX, MAGIC_LOOP__ERROR=MAGIC_ERROR};
// eng: Returns the emitter behaviour mode at the end of the animation
// rus: ���������� ����� ��������� �������� ����� ��������� ��������
MAGIC_LOOP_ENUM Magic_GetLoopMode(HM_EMITTER hmEmitter);

// eng: Sets the emitter behaviour mode at the end of the animation
// rus: ������������� ����� ��������� �������� ����� ��������� ��������
int Magic_SetLoopMode(HM_EMITTER hmEmitter, MAGIC_LOOP_ENUM mode);

enum MAGIC_COLOR_ENUM {MAGIC_COLOR_STANDARD, MAGIC_COLOR_TINT, MAGIC_COLOR_USER, MAGIC_COLOR__MAX, MAGIC_COLOR__ERROR=MAGIC_ERROR};
// eng: Returns the color management mode
// rus: ���������� ����� ���������� ������ ������
MAGIC_COLOR_ENUM Magic_GetColorMode(HM_EMITTER hmEmitter);

// eng: Sets the color management mode
// rus: ������������� ����� ���������� ������ ������
int Magic_SetColorMode(HM_EMITTER hmEmitter, MAGIC_COLOR_ENUM mode);

// eng: Returns the user defined tint
// rus: ���������� ������� ������������
int Magic_GetTint(HM_EMITTER hmEmitter);

// eng: Sets the user defined tint
// rus: ������������� ������� ������������
int Magic_SetTint(HM_EMITTER hmEmitter,int tint);

// eng: Returns the user defined tint strength
// rus: ���������� ���� ������� ������������
float Magic_GetTintStrength(HM_EMITTER hmEmitter);

// eng: Sets the user defined tint strength
// rus: ������������� ���� ������� ������������
int Magic_SetTintStrength(HM_EMITTER hmEmitter,float tint_strength);

// eng: Returns the emitter scale
// rus: ���������� ������� ��������
float Magic_GetScale(HM_EMITTER hmEmitter);

// eng: Sets the emitter scale
// rus: ������������� ������� ��������
int Magic_SetScale(HM_EMITTER hmEmitter, float scale);

// eng: Sets the user data
// rus: ������������� ���������������� ������
int Magic_SetData(HM_EMITTER hmEmitter, int data);

// eng: Returns the user data
// rus: ���������� ���������������� ������
int Magic_GetData(HM_EMITTER hmEmitter);

// rus: ���������� ������� �������� ������
int Magic_GetLimiter(HM_EMITTER hmEmitter, MAGIC_LIMITER* place);

// rus: ������������� ������� �������� ������
int Magic_SetLimiter(HM_EMITTER hmEmitter, MAGIC_LIMITER* place);

// eng: Returns the name of the emitter
// rus: ���������� ��� ��������
const char* Magic_GetEmitterName(HM_EMITTER hmEmitter);

// eng: Returns the shape of the emitter itself or the shape of the emitter for the specified particles type
// rus: ���������� ����� �������� ��� ����� �������� ��� ���������� ���� ������
enum MAGIC_EMITTER_ENUM {MAGIC_EMITTER_POINT, MAGIC_EMITTER_LINE, MAGIC_EMITTER_CIRCLE, MAGIC_EMITTER_ELLIPSE, MAGIC_EMITTER_SQUARE, MAGIC_EMITTER_RECTANGLE, MAGIC_EMITTER_IMAGE, MAGIC_EMITTER_TEXT, MAGIC_EMITTER_MODEL, MAGIC_EMITTER__MAX, MAGIC_EMITTER__ERROR=MAGIC_ERROR};
MAGIC_EMITTER_ENUM Magic_GetEmitterType(HM_EMITTER hmEmitter, int index);

// eng: Returns the mode of the emitter
// rus: ���������� ����� ������ ��������
enum MAGIC_MODE_ENUM {MAGIC_MODE_FOLDER, MAGIC_MODE_EMITTER, MAGIC_MODE_PICTURE, MAGIC_MODE_TEXT, MAGIC_MODE_MODEL, MAGIC_MODE__MAX, MAGIC_MODE__ERROR=MAGIC_ERROR};
MAGIC_MODE_ENUM Magic_GetEmitterMode(HM_EMITTER hmEmitter);

// eng: Returns the ID of emitter
// rus: ���������� ������������� ��������
unsigned int Magic_GetEmitterID(HM_EMITTER hmEmitter);

// eng: Returns the value of "Speed" that was set in Magic Particles (Dev)
// rus: ���������� �������� � Magic Particles (Dev) �������� "����������� �����"
float Magic_GetUpdateSpeed(HM_EMITTER hmEmitter);

// eng: Returns information about background image of emitter
// rus: ���������� ���������� � ������� ����������� ��������
float Magic_GetBackgroundRect(HM_EMITTER hmEmitter, MAGIC_RECT* rect);

#ifdef MAGIC_3D

// eng: Returns the flag indicating that emitter emits only 3D-particles
// rus: ���������� ������� ����, ��� ������� �������� ������ 3D-�������
bool Magic_Is3d(HM_EMITTER hmEmitter);

// eng: Returns the default camera and perspective of emitter that were set using the Magic Particles (Dev)
// rus: ���������� ������ � �����������, ������� ���� ����������� ��� �������� � Magic Particles (Dev)
int Magic_GetView(HM_EMITTER hmEmitter, MAGIC_VIEW* view);

#else

// eng: Transforms coordinates of 2D-emitter from Magic Particles (Dev) into scene coordinates
// rus: ����������� ���������� 2D-�������� �� Magic Particles (Dev) � ���������� �����
int Magic_CorrectEmitterPosition(HM_EMITTER hmEmitter, int scene_width, int scene_height);
#endif

// eng: Returns coordinates of the emitter
// rus: ���������� ���������� ��������
int Magic_GetEmitterPosition(HM_EMITTER hmEmitter,MAGIC_POSITION* pos);

// eng: Sets the coordinates of the emitter
// rus: ������������� ���������� ��������
int Magic_SetEmitterPosition(HM_EMITTER hmEmitter,MAGIC_POSITION* pos);

// eng: Sets the emitter position. "Tail" from particles is formed between old and new position
// rus: ������������� ���������� ��������. ����� ������ � ����� �������� �������� ���������� "�����" �� ������
int Magic_SetEmitterPositionWithTail(HM_EMITTER hmEmitter,MAGIC_POSITION* pos);

// eng: Sets the coordinates of the emitter (attached physic objects are moved too)
// rus: ������������� ���������� �������� (���������� ����������� ���� ������������ ���. ��������)
int Magic_SetEmitterPositionWithAttachedPhysicObjects(HM_EMITTER hmEmitter, MAGIC_POSITION* pos);

// eng: Returns the mode of the emitter coordinates
// rus: ���������� ����� ��������� ��������
bool Magic_GetEmitterPositionMode(HM_EMITTER hmEmitter);

// eng: Sets the mode of the emitter coordinates
// rus: ������������� ����� ��������� ��������
int Magic_SetEmitterPositionMode(HM_EMITTER hmEmitter,bool mode);

// eng: Moves particles
// rus: ���������� �������
int Magic_MoveEmitterParticles(HM_EMITTER hmEmitter,MAGIC_POSITION* offset);

// eng: Returns emitter direction
// rus: ���������� ����������� ��������
int Magic_GetEmitterDirection(HM_EMITTER hmEmitter,MAGIC_DIRECTION* direction);

// eng: Sets the direction of the emitter
// rus: ������������� ����������� ��������
int Magic_SetEmitterDirection(HM_EMITTER hmEmitter,MAGIC_DIRECTION* direction);

// eng: Gets the emitter's direction mode
// rus: ���������� ����� �������� ��������
bool Magic_GetEmitterDirectionMode(HM_EMITTER hmEmitter);

// eng: Sets emitter's rotation mode
// rus: ������������� ����� �������� ��������
int Magic_SetEmitterDirectionMode(HM_EMITTER hmEmitter,bool mode);

// eng: Rotates particles
// rus: ������� �������
int Magic_RotateEmitterParticles(HM_EMITTER hmEmitter, MAGIC_DIRECTION* offset);

// eng: Returns the animate folder flag
// rus: ���������� ������� ������������� �����
bool Magic_IsFolder(HM_EMITTER hmEmitter);

// eng: Returns the number of emitters contained in animate folder. 1 is returned for emitter
// rus: ���������� ���������� ��������� ������ ��������
int Magic_GetEmitterCount(HM_EMITTER hmEmitter);

// eng: Returns the specified emitter from animate folder. Returns itself for emitter
// rus: ���������� ���������� �������� ������ ��������
HM_EMITTER Magic_GetEmitter(HM_EMITTER hmEmitter,int index);

// eng: Prepares the information about render arrays
// rus: �������������� ���������� � �������� �������
enum MAGIC_ARGB_ENUM {MAGIC_ABGR, MAGIC_ARGB, MAGIC_BGRA, MAGIC_RGBA, MAGIC_ARGB__MAX};
void* Magic_PrepareRenderArrays(HM_EMITTER hmEmitter, MAGIC_RENDERING_START* start, int max_arrays_streams, MAGIC_ARGB_ENUM argb_format, bool index32);

// eng: Returns the info about one of render arrays
// rus: ���������� ���������� �� ����� �� �������� �������
int Magic_GetRenderArrayData(void* context, int index, MAGIC_ARRAY_INFO* info);

// eng: Sets the render array
// rus: ������������� ������ �������
int Magic_SetRenderArrayData(void* context, int index, void* buffer, int offset, int stride);

// eng: Fills the render buffers by info about vertices
// rus: ��������� ������ ������������ ����������� � ��������
void Magic_FillRenderArrays(void* context);

// eng: Returns the portion of particles
// rus: ���������� ������ ������
int Magic_GetVertices(void* context, MAGIC_RENDER_VERTICES* vrts);

// eng: Returns a next render state
// rus: ���������� ��������� ������ �������
int Magic_GetNextRenderState(void* context, MAGIC_RENDER_STATE* state);


enum MAGIC_SORT_ENUM {MAGIC_NOSORT, MAGIC_SORT_MIX, MAGIC_SORT_MIX_INV, MAGIC_SORT_CAMERA_NEAR, MAGIC_SORT_CAMERA_FAR, MAGIC_SORT__MAX};

// eng: Returns particles sorting mode
// rus: ���������� ����� ���������� ������
MAGIC_SORT_ENUM Magic_GetSortingMode(HM_EMITTER hmEmitter);

// eng: Sets particles sorting mode
// rus: ������������� ����� ���������� ������
int Magic_SetSortingMode(HM_EMITTER hmEmitter, MAGIC_SORT_ENUM mode);

// eng: Returns maximal Bounding Box
// rus: ���������� ������������ Bounding Box
int Magic_GetBBoxMax(HM_EMITTER hmEmitter, MAGIC_BBOX* bbox);

// eng: Returnes Bounds Box
// rus: ���������� BBox
int Magic_GetBBox(HM_EMITTER hmEmitter,MAGIC_BBOX* bbox);

// eng: Returns Bounds Box recalculation period
// rus: ���������� ������ ����������� Bounds Box
int Magic_GetBBoxPeriod(HM_EMITTER hmEmitter);

// eng: Sets Bounds Box recalculation period
// rus: ������������� ������ ����������� Bounds Box
int Magic_SetBBoxPeriod(HM_EMITTER hmEmitter,int period);

// eng: Forces Bounds Box recalculation
// rus: ������������� ������������� Bounds Box
int Magic_RecalcBBox(HM_EMITTER hmEmitter);

// eng: Returns the count of user defined variables of emitter or animated folder
// rus: ���������� ���������� ���������������� ���������� ������ �������� ��� ������������� �����
int Magic_GetEmitterVariableCount(HM_EMITTER hmEmitter);

// eng: Returns information about user defined variable of emitter or animated folder
// rus: ���������� ���������� � ���������������� ���������� �� �������� ��� ������������� �����
int Magic_GetEmitterVariable(HM_EMITTER hmEmitter, int index, MAGIC_VARIABLE* variable);

// --------------------------------------------------------------------------------

// eng: Returns the name of the particles type
// rus: ���������� ��� ���� ������
const char* Magic_GetParticlesTypeName(HM_EMITTER hmEmitter, int index);

// eng: Returns the number particles type contained in emitter
// rus: ���������� ���������� ����� ������ ������ ��������
int Magic_GetParticlesTypeCount(HM_EMITTER hmEmitter);

// eng: Locks the specified particles type for the further processing
// rus: ����������� ��� ���������� ��������� ��������� ��� ������
int Magic_LockParticlesType(HM_EMITTER hmEmitter, int index);

// eng: Releases previously locked particles type
// rus: ����������� ����������� ����� ��� ������
int Magic_UnlockParticlesType();

#ifdef MAGIC_3D

// eng: Returns particle type orientation for 3D-emitters
// rus: ���������� ���������� ���� ������ ��� 3D-���������
int Magic_GetOrientation(MAGIC_ORIENTATION* orientation);

// eng: Sets particle type orientation for 3D-emitters
// rus: ������������� ���������� ���� ������ ��� 3D-���������
int Magic_SetOrientation(MAGIC_ORIENTATION* orientation);

#endif

// eng: Returns "tail" properties
// rus: ���������� �������� "������"
int Magic_GetTailProperties(MAGIC_TAIL* tail);

// eng: Sets "tail" properties
// rus: ������������� �������� "������"
int Magic_SetTailProperties(MAGIC_TAIL* tail);

// eng: Returns the next particle. Is used to go through all the existing particles
// rus: ���������� ���������� �� ��������� �������. ������������ ��� �������� ���� ������������ ������
int Magic_GetNextParticle(MAGIC_PARTICLE* particle);

// eng: Changes the position of the particle that is got by Magic_GetNextParticle
// rus: �������� ���������� �������, ���������� ����� Magic_GetNextParticle
void Magic_MoveParticle(MAGIC_POSITION* offset);

// eng: Rotates the particle that was obtained by Magic_GetNextParticle around the emitter
// rus: ������� ������� ���������� ����� Magic_GetNextParticle ������ ��������
void Magic_RotateParticle(MAGIC_DIRECTION* offset);

// eng: Returns count of user defined variables of particles type
// rus: ���������� ���������� ���������������� ���������� ������ ���� ������
int Magic_GetParticlesTypeVariableCount();

// eng: Returns information about user defined variable of particles type
// rus: ���������� ���������� � ���������������� ���������� �� ���� ������
int Magic_GetParticlesTypeVariable(int index, MAGIC_VARIABLE* variable);

// eng: Returns the count of actions
// rus: ���������� ���������� ��������
int Magic_GetActionCount();

// eng: Returns MAGIC_VARIABLE structure that contains action information
// rus: ���������� ��������� MAGIC_ACTION � ����������� �� ��������� ��������
int Magic_GetAction(int index, MAGIC_ACTION* action);

// eng: Creates new action
// rus: ������� ����� ��������
int Magic_CreateAction(MAGIC_ACTION* action);

// eng: Deletes specified action
// rus: ������� ��������� ��������
int Magic_DestroyAction(int index);


enum MAGIC_PHYSIC_TYPE_ENUM {MAGIC_TYPE_OBSTACLE, MAGIC_TYPE_WIND, MAGIC_TYPE_MAGNET, MAGIC_PHYSIC_TYPE__MAX};

// eng: Returns count of attached physical objects of specified type
// rus: ���������� ���������� �������������� ���������� �������� ���������� ����
int Magic_GetAttachedPhysicObjectsCount(MAGIC_PHYSIC_TYPE_ENUM type);

// eng: Returns list of attached physical objects of specified type
// rus: ���������� ������ �������������� ���������� �������� ���������� ����
int Magic_GetAttachedPhysicObjects(MAGIC_PHYSIC_TYPE_ENUM type, int* HMs);


enum MAGIC_PARTICLES_TYPE_ENUM {MAGIC_PARTICLES_TYPE_USUAL, MAGIC_PARTICLES_TYPE_PATH, MAGIC_PARTICLES_TYPE_BEAM, MAGIC_PARTICLES_TYPE_TRAIL, MAGIC_PARTICLES_TYPE__MAX, MAGIC_PARTICLES_TYPE__ERROR=MAGIC_ERROR};

// eng: Returns the mode of particles type
// rus: ���������� ����� ������ ���� ������
MAGIC_PARTICLES_TYPE_ENUM Magic_GetParticlesTypeMode();

// eng: Returns the number of keys in the path for Path/Beam
// rus: ���������� ���������� ������ � ���� ��� Path/Beam
int Magic_GetPathLength();

// eng: Returns the path for Path/Beam
// rus: ���������� ���� ��� Path/Beam
int Magic_GetPath(MAGIC_KEY* keys);

// eng: Changes the path for Path/Beam
// rus: �������� ���� ��� Path/Beam
int Magic_SetPath(int count, MAGIC_KEY* keys);

// --------------------------------------------------------------------------

// eng: Returns the information on next change in textural atlas
// rus: ���������� ���������� �� ��������� ��������� � ���������� ������
int Magic_GetNextAtlasChange(MAGIC_CHANGE_ATLAS* change);

// eng: Creates textural atlases for all loaded emitters
// rus: ������� ���������� ������ ��� ���� ����������� ���������
float Magic_CreateAtlases(int width,int height,int step,float scale_step);

// eng: Creates textural atlases for specified emitters
// rus: ������� ���������� ������ ��� ��������� ���������
float Magic_CreateAtlasesForEmitters(int width,int height,int count, HM_EMITTER* emitters,int step,float scale_step);

// eng: Sets up the initial scale for atlas creation
// rus: ������������� ��������� ������� ��� ����������� ������
void Magic_SetStartingScaleForAtlas(float scale);

// eng: Returns the initial scale for atlas creation
// rus: ���������� ��������� ������� ��� ����������� ������
float Magic_GetStartingScaleForAtlas();

// --------------------------------------------------------------------------

enum MAGIC_DIAGRAM_ENUM {MAGIC_DIAGRAM_LIFE, MAGIC_DIAGRAM_NUMBER, MAGIC_DIAGRAM_SIZE, MAGIC_DIAGRAM_VELOCITY, MAGIC_DIAGRAM_WEIGHT, MAGIC_DIAGRAM_SPIN, MAGIC_DIAGRAM_ANGULAR_VELOCITY, MAGIC_DIAGRAM_MOTION_RAND, MAGIC_DIAGRAM_VISIBILITY, MAGIC_DIAGRAM_DIRECTION, MAGIC_DIAGRAM__MAX};

// eng: Figures out if the diagram is managable
// rus: ����������, �������� �� ��������� ������
bool Magic_IsDiagramEnabled(HM_EMITTER hmEmitter,int type_index, MAGIC_DIAGRAM_ENUM diagram_index);

// eng: Returns the specified diagram factor
// rus: ���������� ��������� ��� ���������� �������
float Magic_GetDiagramFactor(HM_EMITTER hmEmitter,int type_index, MAGIC_DIAGRAM_ENUM diagram_index);

// eng: Sets the specified diagram factor
// rus: ������������� ��������� ��� ���������� �������
int Magic_SetDiagramFactor(HM_EMITTER hmEmitter,int type_index, MAGIC_DIAGRAM_ENUM diagram_index, float factor);

// eng: Returns the factor for emitter form diagram
// rus: ���������� ��������� ��� ������� ����� ��������
float Magic_GetDiagramEmitterFactor(HM_EMITTER hmEmitter, int type_index, bool line);

// eng: Sets the factor for emitter form diagram
// rus: ������������� ��������� ��� ������� ����� ��������
int Magic_SetDiagramEmitterFactor(HM_EMITTER hmEmitter, int type_index, bool line, float factor);

// eng: Returns the offset for the specified diagram
// rus: ���������� �������� ��� ���������� �������
float Magic_GetDiagramAddition(HM_EMITTER hmEmitter,int type_index, MAGIC_DIAGRAM_ENUM diagram_index);

// eng: Sets the offset for the specified diagram
// rus: ������������� �������� ��� ���������� �������
int Magic_SetDiagramAddition(HM_EMITTER hmEmitter,int type_index, MAGIC_DIAGRAM_ENUM diagram_index, float addition);

// eng: Returns the offset for emitter form diagram
// rus: ���������� �������� ��� ������� ����� ��������
float Magic_GetDiagramEmitterAddition(HM_EMITTER hmEmitter, int type_index, bool line);

// eng: Sets the offset for emitter form diagram
// rus: ������������� �������� ��� ������� ����� ��������
int Magic_SetDiagramEmitterAddition(HM_EMITTER hmEmitter, int type_index, bool line, float addition);

// eng: Allows substituting a graphical pattern which is used to generate particles of "Image" and "Text" formed emitters
// rus: ��������� �������� ����������� �����, �� �������� ���������� ��������� ������ � ��������� ���� "��������" � "�����"
int Magic_ChangeImage(HM_EMITTER hmEmitter, int type_index, int width, int height, unsigned char* data, int bytes_per_pixel);

// eng: Allows changing the triangle based model by which particle generation occurs in "Model" formed emitters
// rus: ��������� �������� ������ �� �������������, �� ������� ���������� ��������� ������ � �������� ���� "������"
int Magic_ChangeModel(HM_EMITTER hmEmitter, int type_index, int count, MAGIC_TRIANGLE* triangles);

// --------------------------------------------------------------------------------------

// eng: Creates a new key on a Timeline
// rus: ������� ����� ���� ���������� ���� �� ����� �������
int Magic_CreateKey(HM_EMITTER hmEmitter, MAGIC_KEY_ENUM type, MAGIC_KEY* key);

// eng: Deletes the specified key of desired type from Timeline
// rus: ������� ��������� ���� ���������� ���� �� ����� �������
int Magic_DeleteKey(HM_EMITTER hmEmitter, MAGIC_KEY_ENUM type, int index);

// eng: Returns the number of keys of specified type from Timeline
// rus: ���������� ���������� ������ ���������� ���� �� ����� �������
int Magic_GetKeyCount(HM_EMITTER hmEmitter, MAGIC_KEY_ENUM type);

// eng: Returns information for the key of specified type
// rus: ���������� ���������� � ����� ���������� ����
int Magic_GetKey(HM_EMITTER hmEmitter, MAGIC_KEY_ENUM type, MAGIC_KEY* key, int index);

// eng: Sets the new data for the key of specified type
// rus: ������������� ����� ������ ��� ����� ���������� ����
int Magic_SetKey(HM_EMITTER hmEmitter, MAGIC_KEY_ENUM type, MAGIC_KEY* key, int index);

// --------------------------------------------------------------------------------------

// eng: Creates obstacle
// rus: ������� �����������
HM_OBSTACLE Magic_CreateObstacle(MAGIC_OBSTACLE* data, MAGIC_POSITION* position, int cell);

// eng: Returns information about shape of obstacle
// rus: ���������� ���������� � ����� �����������
int Magic_GetObstacleData(HM_OBSTACLE hmObstacle, MAGIC_OBSTACLE* data);

// eng: Sets new shape of obstacle
// rus: ������������� ����� ����� ��� �����������
int Magic_SetObstacleData(HM_OBSTACLE hmObstacle, MAGIC_OBSTACLE* data, int cell);

// eng: Returns position of obstacle
// rus: ���������� ������� �����������
int Magic_GetObstaclePosition(HM_OBSTACLE hmObstacle,MAGIC_POSITION* pos);

// eng: Sets new position of obstacle
// rus: ������������� ����� ������� ��� �����������
int Magic_SetObstaclePosition(HM_OBSTACLE hmObstacle,MAGIC_POSITION* pos);

// eng: Creates wind
// rus: ������� �����
HM_WIND Magic_CreateWind(MAGIC_WIND* data);

// eng: Returns information about wind
// rus: ���������� ���������� � �����
int Magic_GetWindData(HM_WIND hmWind, MAGIC_WIND* data);

// eng: Sets new properties for wind
// rus: ������������� ����� �������� ��� �����
int Magic_SetWindData(HM_WIND hmWind, MAGIC_WIND* data);

// eng: Deletes obstacle or wind
// rus: ���������� ����������� ��� �����
int Magic_DestroyPhysicObject(MAGIC_PHYSIC_TYPE_ENUM type, int HM);

// eng: Deletes all obstacles and winds
// rus: ���������� ��� ����������� ��� �����
int Magic_DestroyAllPhysicObjects(MAGIC_PHYSIC_TYPE_ENUM type);

// eng: Duplicates specified obstacle or wind
// rus: ������� ����� � ���������� ����������� ��� �����
int Magic_DuplicatePhysicObject(MAGIC_PHYSIC_TYPE_ENUM type, int HM);

// --------------------------------------------------------------------------------------

// eng: Returns information about subsequent event
// rus: ���������� ���������� �� ��������� �������
int Magic_GetNextEvent(MAGIC_EVENT* evt);

// eng: Returns user data of particle
// rus: ���������� ���������������� ������ �������
void* Magic_ParticleGetData(HM_PARTICLE hmParticle);

// eng: Sets user data of particle
// rus: ������������� ���������������� ������ �������
void Magic_ParticleSetData(HM_PARTICLE hmParticle, void* data);

// eng: Returns scene coordinates of particle
// rus: ���������� ���������� ������� �� �����
void Magic_ParticleGetPosition(HM_PARTICLE hmParticle, MAGIC_POSITION* pos);

// eng: Sets coordinates of particle
// rus: ������������� ���������� �������
void Magic_ParticleSetPosition(HM_PARTICLE hmParticle, MAGIC_POSITION* pos);


enum MAGIC_PROPERTY_ENUM {MAGIC_PROPERTY_ANGLE, MAGIC_PROPERTY_SIZE, MAGIC_PROPERTY_VELOCITY, MAGIC_PROPERTY_WEIGHT, MAGIC_PROPERTY_SPIN, MAGIC_PROPERTY_ANGULAR_VELOCITY, MAGIC_PROPERTY_MOTION_RAND, MAGIC_PROPERTY_VISIBILITY, MAGIC_PROPERTY__MAX};

// eng: Returns specified property of particle
// rus: ���������� ��������� �������� �������
float Magic_ParticleGetProperty(HM_PARTICLE hmParticle, MAGIC_PROPERTY_ENUM property);

// eng: Sets specified property of particle
// rus: ������������� ��������� �������� �������
void Magic_ParticleSetProperty(HM_PARTICLE hmParticle, MAGIC_PROPERTY_ENUM property, float value);

// eng: Returns several specified properties of particle
// rus: ���������� ��������� ��������� ������� �������
void Magic_ParticleGetProperties(HM_PARTICLE hmParticle, int count, MAGIC_PROPERTY_ENUM* properties, float* values);

// eng: Sets several specified properties of particle
// rus: ������������� ��������� ��������� ������� �������
void Magic_ParticleSetProperties(HM_PARTICLE hmParticle, int count, MAGIC_PROPERTY_ENUM* properties, float* values);

// eng: Returns physical radius of particle
// rus: ���������� ���������� ������ �������
float Magic_ParticleGetRadius(HM_PARTICLE hmParticle);

// eng: Detaches particle from emitter
// rus: ����������� ������� �� ��������
void Magic_ParticleDetaching(HM_PARTICLE hmParticle);

// eng: Deletes particle
// rus: ���������� �������
void Magic_ParticleDestruction(HM_PARTICLE hmParticle);

// --------------------------------------------------------------------------------------

enum MAGIC_STREAM_ENUM {MAGIC_STREAM_READ, MAGIC_STREAM_WRITE, MAGIC_STREAM_ADD, MAGIC_STREAM__MAX, MAGIC_STREAM__ERROR=MAGIC_ERROR};

// eng: Opens stream from file
// rus: ��������� ����� �� �����
int Magic_StreamOpenFile(const char* file_name, MAGIC_STREAM_ENUM mode);

// eng: Opens stream in memory
// rus: ��������� ����� � ������
int Magic_StreamOpenMemory(const char* address, unsigned int length, MAGIC_STREAM_ENUM mode);

// eng: Closes stream that was previously opened by Magic_StreamOpenFile or Magic_StreamOpenMemory
// rus: ��������� �����, �������� ����� ����� Magic_StreamOpenFile ��� Magic_StreamOpenMemory
int Magic_StreamClose(HM_STREAM hmStream);

// eng: Closing all opened streams
// rus: �������� ���� �������� �����
void Magic_StreamCloseAll();

// eng: Returns the length of stream
// rus: ���������� ������ ������
unsigned int Magic_StreamGetLength(HM_STREAM hmStream);

// eng: Returns current stream position
// rus: ���������� ������� ������� � ������
unsigned int Magic_StreamGetPosition(HM_STREAM hmStream);

// eng: Sets the position of stream that was opened in read only mode
// rus: ������������� ������� ������, ��������� � ������ ������
int Magic_StreamSetPosition(HM_STREAM hmStream, unsigned int position);

// eng: Returns current stream mode
// rus: ���������� ������� ����� ������
MAGIC_STREAM_ENUM Magic_StreamGetMode(HM_STREAM hmStream);

// eng: Sets new stream mode
// rus: ������������� ����� ����� ������
int Magic_StreamSetMode(HM_STREAM hmStream, MAGIC_STREAM_ENUM mode);

// eng: Returns file name of stream opened from file
// rus: ���������� ��� ����� ��� ������, ��������� �� �����
const char* Magic_StreamGetFileName(HM_STREAM hmStream);

// eng: Reads specified number of bytes from stream
// rus: ������ �� ������ ��������� ���������� ����
int Magic_StreamRead(HM_STREAM hmStream, char* data, unsigned int count);

// eng: Writes specified number of bytes into stream
// rus: ��������� � ����� ��������� ���������� ����
int Magic_StreamWrite(HM_STREAM hmStream, const char* data, unsigned int count);

// eng: Copying particle space from stream into emitter
// rus: ����������� ������������ ������ � ������� �� ������
int Magic_LoadArrayFromStream(HM_EMITTER hmEmitter, HM_STREAM hmStream);

// eng: Copying particle space from emitter into stream
// rus: ����������� ������������ ������ �� �������� � �����
int Magic_SaveArrayToStream(HM_EMITTER hmEmitter, HM_STREAM hmStream);

// eng: Sets emitter animation position to left position of visibility range
// rus: ������������� ������� �������� �������� �� ����� ������� ��������� ���������
int Magic_EmitterToInterval1_Stream(HM_EMITTER hmEmitter, float speed_factor, HM_STREAM hmStream);

// eng: Sets the coordinates of the emitter (attached physic objects are moved too)
// rus: ������������� ���������� �������� (���������� ����������� ���� ������������ ���. ��������)
int Magic_SetEmitterPositionWithAttachedPhysicObjects(HM_EMITTER hmEmitter, MAGIC_POSITION* pos);

void Magic_DestroyAll();


// *****************************************************************************************************************************
// ******* rus: ��������� ������ �� ��������� � ������ API � ��������� ��� ������������� � ���������� � �������� �������� ******
// *****************************************************************************************************************************

#define MAGIC_CHANGE_ATLAS_SHADER	4
#define MAGIC_ADDITIONAL_TYPE_TRAIL_BEAM	4

#define MAGIC_HLSL	0
#define MAGIC_GLSL	1
#define MAGIC_CG	2

// ��������� � ����������� � �������
struct MAGIC_SHADER
{
	const char* name;
	const char* rem;
	const char* code;
	int length;
	int type;

	const char* profile;
	const char* command;
	int language;
};
#define MAGIC_SHADER_VERTEX	0
#define MAGIC_SHADER_PIXEL	1

// ��������� � ����������� � ���������� �������
struct MAGIC_SHADER_PARAMETERS
{
	int shader;
	HM_EMITTER hmEmitter;
	int particles_type;
	void* temp;

	int count_index;
	int* indexes;
	int count_parameters;
	float* parameters;
};

struct MAGIC_UNIFORM
{
	int shader;					// ������ ������������ �������
	const char* name;			// ��� uniform-���������� (��� �� ��� ����, � ��� ����� ����������)
	const char* path;			// ���� � uniform (��� ���� ������ � ����)
	int type;					// ��� ������ (bool, int , float)
	int count;					// ���������� ������
	int size;					// ������ ������ � ������
	const char* data;			// ������ � �������������� ������
	int function;				// ������ �������, ������� ��������� ����������
	void* handle;				// ���������� � �������
};

// ---------------------------------------------------------------------------------


#ifdef __cplusplus
}
#endif

#endif
