//-----------------------------------------------------------------------------
// File: magic.h
//
// Copyright (c) Astralax. All rights reserved.
// Author: Sedov Alexey
//-----------------------------------------------------------------------------

#ifndef MAGIC_PARTICLES_LIBRARY
#define MAGIC_PARTICLES_LIBRARY

#define MAGIC_API	"3.43"

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
// rus: MAGIC_POSITION - структура для хранения координат
struct MAGIC_POSITION
{
	float x;
	float y;
	#ifdef MAGIC_3D
	float z;
	#endif
};

// eng: MAGIC_DIRECTION - structure to define direction
// rus: MAGIC_DIRECTION - структура для хранения направления
struct MAGIC_DIRECTION
{
	#ifdef MAGIC_3D
	float x,y,z,w;
	#else
	float angle;
	#endif
};

// eng: MAGIC_FIND_DATA - structure that is used in searching emitters and directories
// rus: MAGIC_FIND_DATA - cтруктура для перебора папок и эмиттеров в текущей папке
struct MAGIC_FIND_DATA
{
	// eng: result
	// rus: результат
	int type;
	const char* name;
	int animate;

	int mode;

	// eng: additional data
	// rus: дополнительные данные
	void* folder;
	int index;
};
#define MAGIC_FOLDER	1
#define MAGIC_EMITTER	2

// eng: MAGIC_PARTICLE - particle structure, containing its properties
// rus: MAGIC_PARTICLE - структура частицы, описывающая ее характеристики. 
struct MAGIC_PARTICLE
{
	MAGIC_POSITION position;
	float size;
	float size_factor;
	float angle;
	unsigned int color;
};

// eng: MAGIC_TEXTURE - structure, containing texture frame-file information
// rus: MAGIC_TEXTURE - структура, хранящая информацию о текстуре
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
// rus: MAGIC_ATLAS - структура, хранящая информацию о расположении файл-кадров на текстурном атласе
struct MAGIC_ATLAS
{
	int width;
	int height;
	int count;
	MAGIC_TEXTURE** textures;
};

// eng: MAGIC_STATIC_ATLAS - structure, containing information of static textural atlas 
// rus: MAGIC_STATIC_ATLAS - структура, хранящая информацию о статическом текстурном атласе
struct MAGIC_STATIC_ATLAS
{
	const char* file;
	const char* path;
	int width, height;
	unsigned int ptc_id;
};

// eng: MAGIC_CHANGE_ATLAS - structure, containing information on changes in textural atlas
// rus: MAGIC_CHANGE_ATLAS - структура, хранящая информацию об изменении в текстурном атласе
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
// rus: MAGIC_KEY - структура, позволяющая модифицировать ключи на Шкале времени
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
// rus: MAGIC_CAMERA - структура, описывающая камеру
enum MAGIC_CAMERA_ENUM {MAGIC_CAMERA_FREE, MAGIC_CAMERA_PERSPECTIVE, MAGIC_CAMERA_ORTHO, MAGIC_CAMERA__MAX};
struct MAGIC_CAMERA
{
	MAGIC_CAMERA_ENUM mode;
	MAGIC_POSITION pos;
	MAGIC_POSITION dir;
};

// eng: MAGIC_VIEW - structure which contains all settings for Projection Matrix and View Matrix
// rus: MAGIC_VIEW - структура, описывающая все настройки для матрицы проекции и матрицы вида
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
// rus: MAGIC_TAIL - структура для построения "хвоста" из частиц
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
// rus: MAGIC_BBOX - структура для хранения Bounds Box
struct MAGIC_BBOX
{
	MAGIC_POSITION corner1;
	MAGIC_POSITION corner2;
};

// eng: MAGIC_LIMITER - structure to restrict the region of "creating" of new particles
// rus: MAGIC_LIMITER - структура для ограничения области "рождения" новых частиц
enum MAGIC_LIMITER_ENUM {MAGIC_LIMITER_DISABLED, MAGIC_LIMITER_RECTANGLE, MAGIC_LIMITER_CIRCLE, MAGIC_LIMITER_PARALLELEPIPED, MAGIC_LIMITER_SPHERE, MAGIC_LIMITER__MAX};
struct MAGIC_LIMITER
{
	MAGIC_LIMITER_ENUM type;

	MAGIC_POSITION position;
	MAGIC_POSITION size;
	float radius;
};

// eng: MAGIC_ORIENTATION - structure for storing particles type orientation
// rus: MAGIC_ORIENTATION - структура для храниения ориентации типа частиц
enum MAGIC_ORIENTATION_ENUM {MAGIC_ORIENTATION_X, MAGIC_ORIENTATION_Y, MAGIC_ORIENTATION_Z, MAGIC_ORIENTATION_AXIS, MAGIC_ORIENTATION_CAMERA, MAGIC_ORIENTATION_CAMERA_X, MAGIC_ORIENTATION_CAMERA_Y, MAGIC_ORIENTATION_CAMERA_Z, MAGIC_ORIENTATION_CAMERA_AXIS, MAGIC_ORIENTATION_DIRECTION, MAGIC_ORIENTATION__MAX};
struct MAGIC_ORIENTATION
{
	MAGIC_ORIENTATION_ENUM orientation;
	float x,y,z;
};

// eng: MAGIC_VARIABLE - structure for variables
// rus: MAGIC_VARIABLE - структура для переменных
enum MAGIC_VARIABLE_ENUM {MAGIC_VARIABLE_BOOL, MAGIC_VARIABLE_INT, MAGIC_VARIABLE_FLOAT, MAGIC_VARIABLE_STRING, MAGIC_VARIABLE_DIAGRAM, MAGIC_VARIABLE__MAX};
struct MAGIC_VARIABLE
{
	const char* name;
	MAGIC_VARIABLE_ENUM type;
	char value[8];
};

// eng: MAGIC_TRIANGLE - structure to define a triangle
// rus: MAGIC_TRIANGLE - структура для хранения треугольника
struct MAGIC_TRIANGLE
{
	MAGIC_POSITION vertex1;
	MAGIC_POSITION vertex2;
	MAGIC_POSITION vertex3;
};

// eng: MAGIC_WIND - structure that defines wind
// rus: MAGIC_WIND - структура, которая описывает ветер
struct MAGIC_WIND
{
	MAGIC_POSITION direction;
	float velocity;
};

// eng: MAGIC_SEGMENT - structure to store line segment coordinates.
// rus: MAGIC_SEGMENT - структура для хранения координат отрезка.
struct MAGIC_SEGMENT
{
	MAGIC_POSITION vertex1;
	MAGIC_POSITION vertex2;
};

// eng: MAGIC_RECT - structure to store rectangle
// rus: MAGIC_RECT - структура для хранения прямоугольника
struct MAGIC_RECT
{
	int left;
	int top;
	int right;
	int bottom;
};

// eng: MAGIC_ACTION - structure for actions
// rus: MAGIC_ACTION - структура для действий
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
// rus: MAGIC_EVENT - структура для получения информации о событии
struct MAGIC_EVENT
{
	MAGIC_EVENT_ENUM event;
	HM_PARTICLE hmParticle;
	MAGIC_POSITION position1;
	MAGIC_POSITION position2;
	MAGIC_POSITION reflection;
};

// eng: MAGIC_OBSTACLE - structure defining the shape of obstacle
// rus: MAGIC_OBSTACLE - структура, которая описывает форму препятствия
#if defined(EXE) || defined(SCREENSAVER)
enum MAGIC_OBSTACLE_ENUM {MAGIC_OBSTACLE_CIRCLE, MAGIC_OBSTACLE_SEGMENT, MAGIC_OBSTACLE_SPHERE, MAGIC_OBSTACLE_TRIANGLE, MAGIC_OBSTACLE__MAX};
#else
#ifdef MAGIC_3D
enum MAGIC_OBSTACLE_ENUM {MAGIC_OBSTACLE_SPHERE=3, MAGIC_OBSTACLE_TRIANGLE, MAGIC_OBSTACLE__MAX};
#else
enum MAGIC_OBSTACLE_ENUM {MAGIC_OBSTACLE_CIRCLE, MAGIC_OBSTACLE_SEGMENT, MAGIC_OBSTACLE__MAX};
#endif
#endif
// MAGIC_OBSTACLE_CIRCLE		circle/окружность (2D)
// MAGIC_OBSTACLE_SEGMENT		segment/отрезок (2D)
// MAGIC_OBSTACLE_SPHERE		sphere/сфера (3D)
// MAGIC_OBSTACLE_TRIANGLE		triangle/треугольник (3D)
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
// rus: MAGIC_VERTEX_FORMAT - структура для хранения формата вершинного буфера
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
// rus: MAGIC_RENDERING_START - структура для визуализации эмиттера
struct MAGIC_RENDERING_START
{
	int particles;				// количество частиц
	int arrays;					// количество массивов
	MAGIC_VERTEX_FORMAT format;	// суммарный формат вершин
	int vertices;				// количество вершин
	int indexes;				// количество индексов
	int textures;				// количество текстур
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
// rus: MAGIC_ARRAY_INFO - структура для описания одного массива атрибута
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
// rus: MAGIC_RENDER_STATE - структура, содержащая статус изменения рендера.
enum MAGIC_RENDER_STATE_ENUM {MAGIC_RENDER_STATE_BLENDING, MAGIC_RENDER_STATE_TEXTURE_COUNT, MAGIC_RENDER_STATE_TEXTURE, MAGIC_RENDER_STATE_ADDRESS_U, MAGIC_RENDER_STATE_ADDRESS_V, MAGIC_RENDER_STATE_OPERATION_RGB, MAGIC_RENDER_STATE_ARGUMENT1_RGB, MAGIC_RENDER_STATE_ARGUMENT2_RGB, MAGIC_RENDER_STATE_OPERATION_ALPHA, MAGIC_RENDER_STATE_ARGUMENT1_ALPHA, MAGIC_RENDER_STATE_ARGUMENT2_ALPHA, MAGIC_RENDER_STATE_ZENABLE, MAGIC_RENDER_STATE_ZWRITE, MAGIC_RENDER_STATE_ALPHATEST_INIT, MAGIC_RENDER_STATE_ALPHATEST, MAGIC_RENDER_STATE_TECHNIQUE_ON, MAGIC_RENDER_STATE_TECHNIQUE_OFF, MAGIC_RENDER_STATE__MAX, MAGIC_RENDER_STATE__ERROR=-1};
struct MAGIC_RENDER_STATE
{
	MAGIC_RENDER_STATE_ENUM state;		// флаг
	int value;							// значение
	int index;							// дополнительный индекс, который обычно определяет текстурный слот
};

// eng: MAGIC_RENDER_VERTICES - structure for visualization of one portion of particles
// rus: MAGIC_RENDER_VERTICES - структура для визуализации одной порции частиц
struct MAGIC_RENDER_VERTICES
{
	int starting_index;
	int indexes_count;
	int material;
};

// eng: MAGIC_TEXTURE_STATES - structure that describes texture parameters
// rus: MAGIC_TEXTURE_STATES - структура, описывающая параметры текстуры
struct MAGIC_TEXTURE_STATES
{
	int address_u, address_v;				// адресация текстуры по U и V
	int operation_rgb;						// текстурная операция rgb
	int argument_rgb1, argument_rgb2;		// аргументы текстурной операции rgb
	int operation_alpha;					// текстурная операция alpha
	int argument_alpha1, argument_alpha2;	// аргументы текстурной операции alpha
};

// eng: MAGIC_MATERIAL - structure that describes the material
// rus: MAGIC_MATERIAL - структура, описывающая параметры материала
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
// rus: Возвращает версию API
const char* Magic_GetVersion();

#ifdef MAGIC_3D

// eng: Returns the camera that is set for API
// rus: Возвращает камеру, установленную для API
void Magic_GetCamera(MAGIC_CAMERA* camera);

// eng: Sets the camera for API
// rus: Устанавливает камеру для API
int Magic_SetCamera(MAGIC_CAMERA* camera);

#endif

#ifdef MAGIC_3D
enum MAGIC_AXIS_ENUM {MAGIC_pXpYpZ, MAGIC_pYpXpZ, MAGIC_pZpXpY, MAGIC_pXpZpY, MAGIC_pYpZpX, MAGIC_pZpYpX, MAGIC_nXpYpZ, MAGIC_pYnXpZ, MAGIC_pZnXpY, MAGIC_nXpZpY, MAGIC_pYpZnX, MAGIC_pZpYnX, MAGIC_pXnYpZ, MAGIC_nYpXpZ, MAGIC_pZpXnY, MAGIC_pXpZnY, MAGIC_nYpZpX, MAGIC_pZnYpX, MAGIC_pXpYnZ, MAGIC_pYpXnZ, MAGIC_nZpXpY, MAGIC_pXnZpY, MAGIC_pYnZpX, MAGIC_nZpYpX, MAGIC_nXnYpZ, MAGIC_nYnXpZ, MAGIC_pZnXnY, MAGIC_nXpZnY, MAGIC_nYpZnX, MAGIC_pZnYnX, MAGIC_nXpYnZ, MAGIC_pYnXnZ, MAGIC_nZnXpY, MAGIC_nXnZpY, MAGIC_pYnZnX, MAGIC_nZpYnX, MAGIC_pXnYnZ, MAGIC_nYpXnZ, MAGIC_nZpXnY, MAGIC_pXnZnY, MAGIC_nYnZpX, MAGIC_nZnYpX, MAGIC_nXnYnZ, MAGIC_nYnXnZ, MAGIC_nZnXnY, MAGIC_nXnZnY, MAGIC_nYnZnX, MAGIC_nZnYnX, MAGIC_AXIS__MAX};
#else
enum MAGIC_AXIS_ENUM {MAGIC_pXpY, MAGIC_pYpX, MAGIC_nXpY, MAGIC_pYnX, MAGIC_pXnY, MAGIC_nYpX, MAGIC_nXnY, MAGIC_nYnX, MAGIC_AXIS__MAX};
#endif

// eng: Returns the direction of coordinate axes
// rus: Возвращает направление координатных осей
MAGIC_AXIS_ENUM Magic_GetAxis();

// eng: Sets the direction of coordinate axes
// rus: Устанавливает направление координатных осей
int Magic_SetAxis(MAGIC_AXIS_ENUM axis_index);

#ifdef MAGIC_3D
// eng: Returns the flag of the usage of Z-buffer during the visualization
// rus: Возвращает признак использования Z-buffer-а во время визуализации
bool Magic_IsZBufferEnabled();

// eng: Enables/Disables the usage of Z-buffer during the visualization
// rus: Разрешает/запрещает использование Z-buffer-а во время визуализации
void Magic_EnableZBuffer(bool enable);
#endif

// eng: Sets filter on the states of render changes
// rus: Устанавливает фильтр на статусы изменения рендера
void Magic_SetRenderStateFilter(bool* filters, bool optimization);

// eng: Returns the number of materials
// rus: Возвращает количество материалов
int Magic_GetMaterialCount();

// eng: Returns the info about material
// rus: Возвращает информацию о материале
int Magic_GetMaterial(int index, MAGIC_MATERIAL* material);

// eng: Converts UTF8 string into UTF16
// rus: Конвертирует строку типа UTF8 в строку типа UTF16
const unsigned short* Magic_UTF8to16(const unsigned char* str);

// eng: Converts UTF8 string into UTF32
// rus: Конвертирует строку типа UTF8 в строку типа UTF32
const unsigned int* Magic_UTF8to32(const unsigned char* str);

// eng: Converts UTF16 string into UTF8
// rus: Конвертирует строку типа UTF16 в строку типа UTF8
const unsigned char* Magic_UTF16to8(const unsigned short* str);

// eng: Converts UTF16 string into UTF32
// rus: Конвертирует строку типа UTF16 в строку типа UTF32
const unsigned int* Magic_UTF16to32(const unsigned short* str);

// eng: Converts UTF32 string into UTF8
// rus: Конвертирует строку типа UTF32 в строку типа UTF8
const unsigned char* Magic_UTF32to8(const unsigned int* str);

// eng: Converts UTF32 string into UTF16
// rus: Конвертирует строку типа UTF32 в строку типа UTF16
const unsigned short* Magic_UTF32to16(const unsigned int* str);

// eng: Populates MAGIC_ACTION structure with default values
// rus: Заполняет структуру MAGIC_ACTION значениями по умолчанию
void MAGIC_ACTION_Identity(MAGIC_ACTION* action);

// --------------------------------------------------------------------------------------


// eng: Loads the ptc-file from the path specified
// rus: Загружает ptc-файл по указанному пути
HM_FILE Magic_OpenFile(const char* file_name);

// eng: Loads the ptc-file image from the memory
// rus: Открытие образа ptc-файла из памяти
HM_FILE Magic_OpenFileInMemory(const char* buffer);

// eng: Loads the ptc-file from the stream
// rus: Загружает ptc-файл из указанного потока
int Magic_OpenStream(HM_STREAM hmStream);

// eng: Closes the file, opened earlier by use of Magic_OpenFile, Magic_OpenFileInMemory or Magic_OpenStream
// rus: Закрывает файл, открытый ранее через Magic_OpenFile, Magic_OpenFileInMemory или Magic_OpenStream
int Magic_CloseFile(HM_FILE hmFile);

// eng: Closing all the opened files
// rus: Закрытие всех открытых файлов
void Magic_CloseAllFiles();

// eng: Returns the current folder path
// rus: Возвращает полный путь к текущей папке
const char* Magic_GetCurrentFolder(HM_FILE hmFile);

// eng: Sets the new current folder
// rus: Установить новый путь к текущей папке
int Magic_SetCurrentFolder(HM_FILE hmFile, const char* path);

// eng: Searches for the first folder or emitter within the current folder and returns the type of the object found
// rus: Ищет первую папку или первый эмиттер в текущей папке и возвращает имя и тип найденного объекта
const char* Magic_FindFirst(HM_FILE hmFile,MAGIC_FIND_DATA* data,int mode);

// eng: Searches for the next folder or emitter within the current folder and returns the type of the object found
// rus: Ищет очередную папку или очередной эмиттер в текущей папке и возвращает имя и тип найденного объекта
const char* Magic_FindNext(HM_FILE hmFile, MAGIC_FIND_DATA* data);

// eng: Returns the name of the file that was opened through the Magic_OpenFile
// rus: Возвращает имя файла, открытого через Magic_OpenFile
const char* Magic_GetFileName(HM_FILE hmFile);

// eng: Returns the flag indicating that textures are stored within the file
// rus: Возвращает признак того, что файл содержит текстуры
bool Magic_HasTextures(HM_FILE hmFile);

// eng: Returns the number of static textural atlases attached to specified file
// rus: Возвращает количество статических текстурных атласов, прикрепленных к указанному файлу
int Magic_GetStaticAtlasCount(HM_FILE hmFile);

// eng: Returns information on static textural atlas attached to specified file
// rus: Возвращает информацию о статическом текстурном атласе, прикрепленному к указанному файлу
int Magic_GetStaticAtlas(HM_FILE hmFile, int index, MAGIC_STATIC_ATLAS* atlas);

// --------------------------------------------------------------------------------

// eng: Creates the emitter object and loads its data
// rus: Создает эмиттер и загружает в него данные
HM_EMITTER Magic_LoadEmitter(HM_FILE hmFile, const char* name);

// eng: Gets the copy of the emitter
// rus: Дублирует эмиттер
HM_EMITTER Magic_DuplicateEmitter(HM_EMITTER hmEmitter);

// eng: Unloads the emitter data and destroys it
// rus: Выгружает данные из эмиттера и уничтожает его
int Magic_UnloadEmitter(HM_EMITTER hmEitter);

// eng: Unloads all loaded emitters
// rus: Выгружает все загруженные эмиттеры
void Magic_UnloadAllEmitters();

// eng: Processes the emitter. Creates, displaces and removes the particles
// rus: Осуществляет обработку эмиттера: создает, перемещает и уничтожает частицы
bool Magic_Update(HM_EMITTER hmEmitter, double time);

// eng: Stops the emitter
// rus: Останавливает работу эмиттера
int Magic_Stop(HM_EMITTER hmEmitter);

// eng: Restarts the emitter from the beginning
// rus: Перезапускает эмиттер с нулевой позиции
int Magic_Restart(HM_EMITTER hmEmitter);

// eng: Returns the flag showing that emitter is in interrupted mode
// rus: Возврашает признак того, что эмиттер прерывается
bool Magic_IsInterrupt(HM_EMITTER hmEmitter);

// eng: Interrupts/Starts emitter work
// rus: Прерывает или запускает работу эмиттера
int Magic_SetInterrupt(HM_EMITTER hmEmitter, bool interrupt);

// eng: Returns the Magic Particles (Dev) time increment, used for the animation
// rus: Возвращает заданное в Magic Particles приращение времени, используемое для анимации эмиттера
double Magic_GetUpdateTime(HM_EMITTER hmEmitter);

// eng: Returns current animation position
// rus: Возвращает текущую позицию анимации
double Magic_GetPosition(HM_EMITTER hmEmitter);

// eng: Sets the current animation position
// rus: Устанавливает текущую позицию анимации
int Magic_SetPosition(HM_EMITTER hmEmitter,double position);

// eng: Returns animation duration
// rus: Возвращает продолжительность анимации
double Magic_GetDuration(HM_EMITTER hmEmitter);

// eng: Returns the left position of the visibility range
// rus: Возвращает левую позицию интервала видимости
double Magic_GetInterval1(HM_EMITTER hmEmitter);

// eng: Sets the left position of the visibility range
// rus: Устанавливает левую позицию интервала видимости
int Magic_SetInterval1(HM_EMITTER hmEmitter,double position);

// eng: Returns the right position of the visibility range
// rus: Возвращает правую позицию интервала видимости
double Magic_GetInterval2(HM_EMITTER hmEmitter);

// eng: Sets the right position of the visibility range
// rus: Устанавливает правую позицию интервала видимости
int Magic_SetInterval2(HM_EMITTER hmEmitter,double position);

// eng: Figures out if the current animation position is within the visibility range
// rus: Определяет, попадает ли текущая позиция анимации в интервал видимости
bool Magic_InInterval(HM_EMITTER hmEmitter);

// eng: Sets the animation position at the left position of visibility range
// rus: Устанавливает эмиттер на первую границу интервала видимости
int Magic_EmitterToInterval1(HM_EMITTER hmEmitter, float speed_factor, const char* file);

// eng: Returns the flag of the animation of emitter that begins not from 0 position
// rus: Возвращает признак того, что анимация эмиттера начинается не с начала
bool Magic_IsInterval1(HM_EMITTER hmEmitter);

// eng: Copying the particles array into emitter from the file
// rus: Копирование пространства частиц в эмиттер из файла.
int Magic_LoadArrayFromFile(HM_EMITTER hmEmitter, const char* file);

// eng: Copying the particles array from the emitter into the file
// rus: Копирование пространства частиц эмиттера в файл
int Magic_SaveArrayToFile(HM_EMITTER hmEmitter, const char* file);

// eng: Returns the particle positions interpolation usage flag
// rus: Возвращает признак режима интерполяции эмиттера
bool Magic_IsInterpolationMode(HM_EMITTER hmEmitter);

// eng: Sets/resets the particle positions interpolation usage flag
// rus: Включает/отключает режим интреполяции положения частиц
int Magic_SetInterpolationMode(HM_EMITTER hmEmitter, bool mode);

// eng: Returns the flag of stability/randomness of the emitter behaviour
// rus: Возвращает признак стабильности/случайности поведения эмиттера
bool Magic_IsRandomMode(HM_EMITTER hmEmitter);

// eng: Sets the flag of stability/randomness of the emitter behaviour
// rus: Устанавливает признак стабильности/случайности поведения эмиттера
int Magic_SetRandomMode(HM_EMITTER hmEmitter,bool mode);


enum MAGIC_LOOP_ENUM {MAGIC_NOLOOP, MAGIC_LOOP, MAGIC_FOREVER, MAGIC_LOOP__MAX, MAGIC_LOOP__ERROR=MAGIC_ERROR};
// eng: Returns the emitter behaviour mode at the end of the animation
// rus: Возвращает режим поведения эмиттера после окончания анимации
MAGIC_LOOP_ENUM Magic_GetLoopMode(HM_EMITTER hmEmitter);

// eng: Sets the emitter behaviour mode at the end of the animation
// rus: Устанавливает режим поведения эмиттера после окончания анимации
int Magic_SetLoopMode(HM_EMITTER hmEmitter, MAGIC_LOOP_ENUM mode);

enum MAGIC_COLOR_ENUM {MAGIC_COLOR_STANDARD, MAGIC_COLOR_TINT, MAGIC_COLOR_USER, MAGIC_COLOR__MAX, MAGIC_COLOR__ERROR=MAGIC_ERROR};
// eng: Returns the color management mode
// rus: Возвращает режим управления цветом частиц
MAGIC_COLOR_ENUM Magic_GetColorMode(HM_EMITTER hmEmitter);

// eng: Sets the color management mode
// rus: Устанавливает режим управления цветом частиц
int Magic_SetColorMode(HM_EMITTER hmEmitter, MAGIC_COLOR_ENUM mode);

// eng: Returns the user defined tint
// rus: Возвращает оттенок пользователя
int Magic_GetTint(HM_EMITTER hmEmitter);

// eng: Sets the user defined tint
// rus: Устанавливает оттенок пользователя
int Magic_SetTint(HM_EMITTER hmEmitter,int tint);

// eng: Returns the user defined tint strength
// rus: Возвращает силу оттенка пользователя
float Magic_GetTintStrength(HM_EMITTER hmEmitter);

// eng: Sets the user defined tint strength
// rus: Устанавливает силу оттенка пользователя
int Magic_SetTintStrength(HM_EMITTER hmEmitter,float tint_strength);

// eng: Returns the emitter scale
// rus: Возвращает масштаб эмиттера
float Magic_GetScale(HM_EMITTER hmEmitter);

// eng: Sets the emitter scale
// rus: Устанавливает масштаб эмиттера
int Magic_SetScale(HM_EMITTER hmEmitter, float scale);

// eng: Sets the user data
// rus: Устанавливает пользовательские данные
int Magic_SetData(HM_EMITTER hmEmitter, int data);

// eng: Returns the user data
// rus: Возвращает пользовательские данные
int Magic_GetData(HM_EMITTER hmEmitter);

// rus: Возвращает область рождения частиц
int Magic_GetLimiter(HM_EMITTER hmEmitter, MAGIC_LIMITER* place);

// rus: Устанавливает область рождения частиц
int Magic_SetLimiter(HM_EMITTER hmEmitter, MAGIC_LIMITER* place);

// eng: Returns the name of the emitter
// rus: Возвращает имя эмиттера
const char* Magic_GetEmitterName(HM_EMITTER hmEmitter);

// eng: Returns the shape of the emitter itself or the shape of the emitter for the specified particles type
// rus: Возвращает форму эмиттера или форму эмиттера для указанного типа частиц
enum MAGIC_EMITTER_ENUM {MAGIC_EMITTER_POINT, MAGIC_EMITTER_LINE, MAGIC_EMITTER_CIRCLE, MAGIC_EMITTER_ELLIPSE, MAGIC_EMITTER_SQUARE, MAGIC_EMITTER_RECTANGLE, MAGIC_EMITTER_IMAGE, MAGIC_EMITTER_TEXT, MAGIC_EMITTER_MODEL, MAGIC_EMITTER__MAX, MAGIC_EMITTER__ERROR=MAGIC_ERROR};
MAGIC_EMITTER_ENUM Magic_GetEmitterType(HM_EMITTER hmEmitter, int index);

// eng: Returns the mode of the emitter
// rus: Возвращает режим работы эмиттера
enum MAGIC_MODE_ENUM {MAGIC_MODE_FOLDER, MAGIC_MODE_EMITTER, MAGIC_MODE_PICTURE, MAGIC_MODE_TEXT, MAGIC_MODE_MODEL, MAGIC_MODE__MAX, MAGIC_MODE__ERROR=MAGIC_ERROR};
MAGIC_MODE_ENUM Magic_GetEmitterMode(HM_EMITTER hmEmitter);

// eng: Returns the ID of emitter
// rus: Возвращает идентификатор эмиттера
unsigned int Magic_GetEmitterID(HM_EMITTER hmEmitter);

// eng: Returns the value of "Speed" that was set in Magic Particles (Dev)
// rus: Возвращает заданное в Magic Particles (Dev) значение "коэффициент темпа"
float Magic_GetUpdateSpeed(HM_EMITTER hmEmitter);

// eng: Returns information about background image of emitter
// rus: Возвращает информацию о фоновом изображении эмиттера
float Magic_GetBackgroundRect(HM_EMITTER hmEmitter, MAGIC_RECT* rect);

#ifdef MAGIC_3D

// eng: Returns the flag indicating that emitter emits only 3D-particles
// rus: Возвращает признак того, что эмиттер излучает только 3D-частицы
bool Magic_Is3d(HM_EMITTER hmEmitter);

// eng: Returns the default camera and perspective of emitter that were set using the Magic Particles (Dev)
// rus: Возвращает камеру и перспективу, которые были установлены для эмиттера в Magic Particles (Dev)
int Magic_GetView(HM_EMITTER hmEmitter, MAGIC_VIEW* view);

#else

// eng: Transforms coordinates of 2D-emitter from Magic Particles (Dev) into scene coordinates
// rus: Преобразует координаты 2D-эмиттера из Magic Particles (Dev) в координаты сцены
int Magic_CorrectEmitterPosition(HM_EMITTER hmEmitter, int scene_width, int scene_height);
#endif

// eng: Returns coordinates of the emitter
// rus: Возвращает координаты эмиттера
int Magic_GetEmitterPosition(HM_EMITTER hmEmitter,MAGIC_POSITION* pos);

// eng: Sets the coordinates of the emitter
// rus: Устанавливает координаты эмиттера
int Magic_SetEmitterPosition(HM_EMITTER hmEmitter,MAGIC_POSITION* pos);

// eng: Sets the emitter position. "Tail" from particles is formed between old and new position
// rus: Устанавливает координаты эмиттера. Между старой и новой позицией эмиттера образуется "хвост" из частиц
int Magic_SetEmitterPositionWithTail(HM_EMITTER hmEmitter,MAGIC_POSITION* pos);

// eng: Sets the coordinates of the emitter (attached physic objects are moved too)
// rus: Устанавливает координаты эмиттера (происходит перемещение всех прицепленных физ. объектов)
int Magic_SetEmitterPositionWithAttachedPhysicObjects(HM_EMITTER hmEmitter, MAGIC_POSITION* pos);

// eng: Returns the mode of the emitter coordinates
// rus: Возвращает режим координат эмиттера
bool Magic_GetEmitterPositionMode(HM_EMITTER hmEmitter);

// eng: Sets the mode of the emitter coordinates
// rus: Устанавливает режим координат эмиттера
int Magic_SetEmitterPositionMode(HM_EMITTER hmEmitter,bool mode);

// eng: Moves particles
// rus: Перемещает частицы
int Magic_MoveEmitterParticles(HM_EMITTER hmEmitter,MAGIC_POSITION* offset);

// eng: Returns emitter direction
// rus: Возвращает направление эмиттера
int Magic_GetEmitterDirection(HM_EMITTER hmEmitter,MAGIC_DIRECTION* direction);

// eng: Sets the direction of the emitter
// rus: Устанавливает направление эмиттера
int Magic_SetEmitterDirection(HM_EMITTER hmEmitter,MAGIC_DIRECTION* direction);

// eng: Gets the emitter's direction mode
// rus: Возвращает режим вращения эмиттера
bool Magic_GetEmitterDirectionMode(HM_EMITTER hmEmitter);

// eng: Sets emitter's rotation mode
// rus: Устанавливает режим вращения эмиттера
int Magic_SetEmitterDirectionMode(HM_EMITTER hmEmitter,bool mode);

// eng: Rotates particles
// rus: Вращает частицы
int Magic_RotateEmitterParticles(HM_EMITTER hmEmitter, MAGIC_DIRECTION* offset);

// eng: Returns the animate folder flag
// rus: Возвращает признак анимированной папки
bool Magic_IsFolder(HM_EMITTER hmEmitter);

// eng: Returns the number of emitters contained in animate folder. 1 is returned for emitter
// rus: Возвращает количество эмиттеров внутри эмиттера
int Magic_GetEmitterCount(HM_EMITTER hmEmitter);

// eng: Returns the specified emitter from animate folder. Returns itself for emitter
// rus: Возвращает дескриптор эмиттера внутри эмиттера
HM_EMITTER Magic_GetEmitter(HM_EMITTER hmEmitter,int index);

// eng: Prepares the information about render arrays
// rus: Подготавливает информацию о массивах рендера
enum MAGIC_ARGB_ENUM {MAGIC_ABGR, MAGIC_ARGB, MAGIC_BGRA, MAGIC_RGBA, MAGIC_ARGB__MAX};
void* Magic_PrepareRenderArrays(HM_EMITTER hmEmitter, MAGIC_RENDERING_START* start, int max_arrays_streams, MAGIC_ARGB_ENUM argb_format, bool index32);

// eng: Returns the info about one of render arrays
// rus: Возвращает информацию об одном из массивов рендера
int Magic_GetRenderArrayData(void* context, int index, MAGIC_ARRAY_INFO* info);

// eng: Sets the render array
// rus: Устанавливает массив рендера
int Magic_SetRenderArrayData(void* context, int index, void* buffer, int offset, int stride);

// eng: Fills the render buffers by info about vertices
// rus: Заполняет буфера визуализации информацией о вершинах
void Magic_FillRenderArrays(void* context);

// eng: Returns the portion of particles
// rus: Возвращает порцию вершин
int Magic_GetVertices(void* context, MAGIC_RENDER_VERTICES* vrts);

// eng: Returns a next render state
// rus: Возвращает очередной статус рендера
int Magic_GetNextRenderState(void* context, MAGIC_RENDER_STATE* state);


enum MAGIC_SORT_ENUM {MAGIC_NOSORT, MAGIC_SORT_MIX, MAGIC_SORT_MIX_INV, MAGIC_SORT_CAMERA_NEAR, MAGIC_SORT_CAMERA_FAR, MAGIC_SORT__MAX};

// eng: Returns particles sorting mode
// rus: Возвращает режим сортировки частиц
MAGIC_SORT_ENUM Magic_GetSortingMode(HM_EMITTER hmEmitter);

// eng: Sets particles sorting mode
// rus: Устанавливает режим сортировки частиц
int Magic_SetSortingMode(HM_EMITTER hmEmitter, MAGIC_SORT_ENUM mode);

// eng: Returns maximal Bounding Box
// rus: Возвращает максимальный Bounding Box
int Magic_GetBBoxMax(HM_EMITTER hmEmitter, MAGIC_BBOX* bbox);

// eng: Returnes Bounds Box
// rus: Возвращает BBox
int Magic_GetBBox(HM_EMITTER hmEmitter,MAGIC_BBOX* bbox);

// eng: Returns Bounds Box recalculation period
// rus: Возвращает период перерасчета Bounds Box
int Magic_GetBBoxPeriod(HM_EMITTER hmEmitter);

// eng: Sets Bounds Box recalculation period
// rus: Устанавливает период перерасчета Bounds Box
int Magic_SetBBoxPeriod(HM_EMITTER hmEmitter,int period);

// eng: Forces Bounds Box recalculation
// rus: Принудительно пересчитывает Bounds Box
int Magic_RecalcBBox(HM_EMITTER hmEmitter);

// eng: Returns the count of user defined variables of emitter or animated folder
// rus: Возвращает количество пользовательских переменных внутри эмиттера или анимированной папки
int Magic_GetEmitterVariableCount(HM_EMITTER hmEmitter);

// eng: Returns information about user defined variable of emitter or animated folder
// rus: Возвращает информацию о пользовательской переменной из эмиттера или анимированной папки
int Magic_GetEmitterVariable(HM_EMITTER hmEmitter, int index, MAGIC_VARIABLE* variable);

// --------------------------------------------------------------------------------

// eng: Returns the name of the particles type
// rus: Возвращает имя типа частиц
const char* Magic_GetParticlesTypeName(HM_EMITTER hmEmitter, int index);

// eng: Returns the number particles type contained in emitter
// rus: Возвращает количество типов частиц внутри эмиттера
int Magic_GetParticlesTypeCount(HM_EMITTER hmEmitter);

// eng: Locks the specified particles type for the further processing
// rus: Захватывает для дальнейшей обработки указанный тип частиц
int Magic_LockParticlesType(HM_EMITTER hmEmitter, int index);

// eng: Releases previously locked particles type
// rus: Освобождает захваченный ранее тип частиц
int Magic_UnlockParticlesType();

#ifdef MAGIC_3D

// eng: Returns particle type orientation for 3D-emitters
// rus: Возвращает ориентацию типа частиц для 3D-эмиттеров
int Magic_GetOrientation(MAGIC_ORIENTATION* orientation);

// eng: Sets particle type orientation for 3D-emitters
// rus: Устанавливает ориентацию типа частиц для 3D-эмиттеров
int Magic_SetOrientation(MAGIC_ORIENTATION* orientation);

#endif

// eng: Returns "tail" properties
// rus: Возвращает свойства "хвоста"
int Magic_GetTailProperties(MAGIC_TAIL* tail);

// eng: Sets "tail" properties
// rus: Устанавливает свойства "хвоста"
int Magic_SetTailProperties(MAGIC_TAIL* tail);

// eng: Returns the next particle. Is used to go through all the existing particles
// rus: Возвращает информацию об очередной частице. Используется для перебора всех существующих частиц
int Magic_GetNextParticle(MAGIC_PARTICLE* particle);

// eng: Changes the position of the particle that is got by Magic_GetNextParticle
// rus: Изменяет координаты частицы, полученной через Magic_GetNextParticle
void Magic_MoveParticle(MAGIC_POSITION* offset);

// eng: Rotates the particle that was obtained by Magic_GetNextParticle around the emitter
// rus: Вращает частицу полученную через Magic_GetNextParticle вокруг эмиттера
void Magic_RotateParticle(MAGIC_DIRECTION* offset);

// eng: Returns count of user defined variables of particles type
// rus: Возвращает количество пользовательских переменных внутри типа частиц
int Magic_GetParticlesTypeVariableCount();

// eng: Returns information about user defined variable of particles type
// rus: Возвращает информацию о пользовательской переменной из типа частиц
int Magic_GetParticlesTypeVariable(int index, MAGIC_VARIABLE* variable);

// eng: Returns the count of actions
// rus: Возвращает количество действий
int Magic_GetActionCount();

// eng: Returns MAGIC_VARIABLE structure that contains action information
// rus: Возвращает структуру MAGIC_ACTION с информацией об указанном действии
int Magic_GetAction(int index, MAGIC_ACTION* action);

// eng: Creates new action
// rus: Создает новое действие
int Magic_CreateAction(MAGIC_ACTION* action);

// eng: Deletes specified action
// rus: Удаляет указанное действие
int Magic_DestroyAction(int index);


enum MAGIC_PHYSIC_TYPE_ENUM {MAGIC_TYPE_OBSTACLE, MAGIC_TYPE_WIND, MAGIC_TYPE_MAGNET, MAGIC_PHYSIC_TYPE__MAX};

// eng: Returns count of attached physical objects of specified type
// rus: Возвращает количество присоединенных физических объектов указанного типа
int Magic_GetAttachedPhysicObjectsCount(MAGIC_PHYSIC_TYPE_ENUM type);

// eng: Returns list of attached physical objects of specified type
// rus: Возвращает список присоединенных физических объектов указанного типа
int Magic_GetAttachedPhysicObjects(MAGIC_PHYSIC_TYPE_ENUM type, int* HMs);


enum MAGIC_PARTICLES_TYPE_ENUM {MAGIC_PARTICLES_TYPE_USUAL, MAGIC_PARTICLES_TYPE_PATH, MAGIC_PARTICLES_TYPE_BEAM, MAGIC_PARTICLES_TYPE_TRAIL, MAGIC_PARTICLES_TYPE__MAX, MAGIC_PARTICLES_TYPE__ERROR=MAGIC_ERROR};

// eng: Returns the mode of particles type
// rus: Возвращает режим работы типа частиц
MAGIC_PARTICLES_TYPE_ENUM Magic_GetParticlesTypeMode();

// eng: Returns the number of keys in the path for Path/Beam
// rus: Возвращает количество ключей в пути для Path/Beam
int Magic_GetPathLength();

// eng: Returns the path for Path/Beam
// rus: Возвращает путь для Path/Beam
int Magic_GetPath(MAGIC_KEY* keys);

// eng: Changes the path for Path/Beam
// rus: Изменяет путь для Path/Beam
int Magic_SetPath(int count, MAGIC_KEY* keys);

// --------------------------------------------------------------------------

// eng: Returns the information on next change in textural atlas
// rus: Возвращает информацию об очередном изменении в текстурном атласе
int Magic_GetNextAtlasChange(MAGIC_CHANGE_ATLAS* change);

// eng: Creates textural atlases for all loaded emitters
// rus: Создает текстурные атласы для всех загруженных эмиттеров
float Magic_CreateAtlases(int width,int height,int step,float scale_step);

// eng: Creates textural atlases for specified emitters
// rus: Создает текстурные атласы для указанных эмиттеров
float Magic_CreateAtlasesForEmitters(int width,int height,int count, HM_EMITTER* emitters,int step,float scale_step);

// eng: Sets up the initial scale for atlas creation
// rus: Устанавливает стартовый масштаб для постороения атласа
void Magic_SetStartingScaleForAtlas(float scale);

// eng: Returns the initial scale for atlas creation
// rus: Возвращает стартовый масштаб для постороения атласа
float Magic_GetStartingScaleForAtlas();

// --------------------------------------------------------------------------

enum MAGIC_DIAGRAM_ENUM {MAGIC_DIAGRAM_LIFE, MAGIC_DIAGRAM_NUMBER, MAGIC_DIAGRAM_SIZE, MAGIC_DIAGRAM_VELOCITY, MAGIC_DIAGRAM_WEIGHT, MAGIC_DIAGRAM_SPIN, MAGIC_DIAGRAM_ANGULAR_VELOCITY, MAGIC_DIAGRAM_MOTION_RAND, MAGIC_DIAGRAM_VISIBILITY, MAGIC_DIAGRAM_DIRECTION, MAGIC_DIAGRAM__MAX};

// eng: Figures out if the diagram is managable
// rus: Определяет, доступен ли указанный график
bool Magic_IsDiagramEnabled(HM_EMITTER hmEmitter,int type_index, MAGIC_DIAGRAM_ENUM diagram_index);

// eng: Returns the specified diagram factor
// rus: Возвращает множитель для указанного графика
float Magic_GetDiagramFactor(HM_EMITTER hmEmitter,int type_index, MAGIC_DIAGRAM_ENUM diagram_index);

// eng: Sets the specified diagram factor
// rus: Устанавливает множитель для указанного графика
int Magic_SetDiagramFactor(HM_EMITTER hmEmitter,int type_index, MAGIC_DIAGRAM_ENUM diagram_index, float factor);

// eng: Returns the factor for emitter form diagram
// rus: Возвращает множитель для графика формы эмиттера
float Magic_GetDiagramEmitterFactor(HM_EMITTER hmEmitter, int type_index, bool line);

// eng: Sets the factor for emitter form diagram
// rus: Устанавливает множитель для графика формы эмиттера
int Magic_SetDiagramEmitterFactor(HM_EMITTER hmEmitter, int type_index, bool line, float factor);

// eng: Returns the offset for the specified diagram
// rus: Возвращает смещение для указанного графика
float Magic_GetDiagramAddition(HM_EMITTER hmEmitter,int type_index, MAGIC_DIAGRAM_ENUM diagram_index);

// eng: Sets the offset for the specified diagram
// rus: Устанавливает смещение для указанного графика
int Magic_SetDiagramAddition(HM_EMITTER hmEmitter,int type_index, MAGIC_DIAGRAM_ENUM diagram_index, float addition);

// eng: Returns the offset for emitter form diagram
// rus: Возвращает смещение для графика формы эмиттера
float Magic_GetDiagramEmitterAddition(HM_EMITTER hmEmitter, int type_index, bool line);

// eng: Sets the offset for emitter form diagram
// rus: Устанавливает смещение для графика формы эмиттера
int Magic_SetDiagramEmitterAddition(HM_EMITTER hmEmitter, int type_index, bool line, float addition);

// eng: Allows substituting a graphical pattern which is used to generate particles of "Image" and "Text" formed emitters
// rus: Позволяет заменить графический образ, по которому происходит генерация частиц у эмиттеров типа "Картинка" и "Текст"
int Magic_ChangeImage(HM_EMITTER hmEmitter, int type_index, int width, int height, unsigned char* data, int bytes_per_pixel);

// eng: Allows changing the triangle based model by which particle generation occurs in "Model" formed emitters
// rus: Позволяет заменить модель из треугольников, по которой происходит генерация частиц у эмиттера типа "Модель"
int Magic_ChangeModel(HM_EMITTER hmEmitter, int type_index, int count, MAGIC_TRIANGLE* triangles);

// --------------------------------------------------------------------------------------

// eng: Creates a new key on a Timeline
// rus: Создает новый ключ указанного типа на Шкале времени
int Magic_CreateKey(HM_EMITTER hmEmitter, MAGIC_KEY_ENUM type, MAGIC_KEY* key);

// eng: Deletes the specified key of desired type from Timeline
// rus: Удаляет выбранный ключ указанного типа со Шкалы времени
int Magic_DeleteKey(HM_EMITTER hmEmitter, MAGIC_KEY_ENUM type, int index);

// eng: Returns the number of keys of specified type from Timeline
// rus: Возвращает количество ключей указанного типа на Шкале времени
int Magic_GetKeyCount(HM_EMITTER hmEmitter, MAGIC_KEY_ENUM type);

// eng: Returns information for the key of specified type
// rus: Возвращает информацию о ключе указанного типа
int Magic_GetKey(HM_EMITTER hmEmitter, MAGIC_KEY_ENUM type, MAGIC_KEY* key, int index);

// eng: Sets the new data for the key of specified type
// rus: Устанавливает новые данные для ключа указанного типа
int Magic_SetKey(HM_EMITTER hmEmitter, MAGIC_KEY_ENUM type, MAGIC_KEY* key, int index);

// --------------------------------------------------------------------------------------

// eng: Creates obstacle
// rus: Создает препятствие
HM_OBSTACLE Magic_CreateObstacle(MAGIC_OBSTACLE* data, MAGIC_POSITION* position, int cell);

// eng: Returns information about shape of obstacle
// rus: Возвращает информацию о форме препятствия
int Magic_GetObstacleData(HM_OBSTACLE hmObstacle, MAGIC_OBSTACLE* data);

// eng: Sets new shape of obstacle
// rus: Устанавливает новую форму для препятствия
int Magic_SetObstacleData(HM_OBSTACLE hmObstacle, MAGIC_OBSTACLE* data, int cell);

// eng: Returns position of obstacle
// rus: Возвращает позицию препятствия
int Magic_GetObstaclePosition(HM_OBSTACLE hmObstacle,MAGIC_POSITION* pos);

// eng: Sets new position of obstacle
// rus: Устанавливает новую позицию для препятствия
int Magic_SetObstaclePosition(HM_OBSTACLE hmObstacle,MAGIC_POSITION* pos);

// eng: Creates wind
// rus: Создает ветер
HM_WIND Magic_CreateWind(MAGIC_WIND* data);

// eng: Returns information about wind
// rus: Возвращает информацию о ветре
int Magic_GetWindData(HM_WIND hmWind, MAGIC_WIND* data);

// eng: Sets new properties for wind
// rus: Устанавливает новые свойства для ветра
int Magic_SetWindData(HM_WIND hmWind, MAGIC_WIND* data);

// eng: Deletes obstacle or wind
// rus: Уничтожает препятствие или ветер
int Magic_DestroyPhysicObject(MAGIC_PHYSIC_TYPE_ENUM type, int HM);

// eng: Deletes all obstacles and winds
// rus: Уничтожает все препятствия или ветры
int Magic_DestroyAllPhysicObjects(MAGIC_PHYSIC_TYPE_ENUM type);

// eng: Duplicates specified obstacle or wind
// rus: Снимает копию с указанного препятствия или ветра
int Magic_DuplicatePhysicObject(MAGIC_PHYSIC_TYPE_ENUM type, int HM);

// --------------------------------------------------------------------------------------

// eng: Returns information about subsequent event
// rus: Возвращает информацию об очередном событии
int Magic_GetNextEvent(MAGIC_EVENT* evt);

// eng: Returns user data of particle
// rus: Возвращает пользовательские данные частицы
void* Magic_ParticleGetData(HM_PARTICLE hmParticle);

// eng: Sets user data of particle
// rus: Устанавливает пользовательские данные частицы
void Magic_ParticleSetData(HM_PARTICLE hmParticle, void* data);

// eng: Returns scene coordinates of particle
// rus: Возвращает координаты частицы на сцене
void Magic_ParticleGetPosition(HM_PARTICLE hmParticle, MAGIC_POSITION* pos);

// eng: Sets coordinates of particle
// rus: Устанавливает координаты частицы
void Magic_ParticleSetPosition(HM_PARTICLE hmParticle, MAGIC_POSITION* pos);


enum MAGIC_PROPERTY_ENUM {MAGIC_PROPERTY_ANGLE, MAGIC_PROPERTY_SIZE, MAGIC_PROPERTY_VELOCITY, MAGIC_PROPERTY_WEIGHT, MAGIC_PROPERTY_SPIN, MAGIC_PROPERTY_ANGULAR_VELOCITY, MAGIC_PROPERTY_MOTION_RAND, MAGIC_PROPERTY_VISIBILITY, MAGIC_PROPERTY__MAX};

// eng: Returns specified property of particle
// rus: Возвращает указанное свойство частицы
float Magic_ParticleGetProperty(HM_PARTICLE hmParticle, MAGIC_PROPERTY_ENUM property);

// eng: Sets specified property of particle
// rus: Устанавливает указанное свойство частицы
void Magic_ParticleSetProperty(HM_PARTICLE hmParticle, MAGIC_PROPERTY_ENUM property, float value);

// eng: Returns several specified properties of particle
// rus: Возвращает несколько указанных свойств частицы
void Magic_ParticleGetProperties(HM_PARTICLE hmParticle, int count, MAGIC_PROPERTY_ENUM* properties, float* values);

// eng: Sets several specified properties of particle
// rus: Устанавливает несколько указанных свойств частицы
void Magic_ParticleSetProperties(HM_PARTICLE hmParticle, int count, MAGIC_PROPERTY_ENUM* properties, float* values);

// eng: Returns physical radius of particle
// rus: Возвращает физический радиус частицы
float Magic_ParticleGetRadius(HM_PARTICLE hmParticle);

// eng: Detaches particle from emitter
// rus: Отсоединяет частицу от эмиттера
void Magic_ParticleDetaching(HM_PARTICLE hmParticle);

// eng: Deletes particle
// rus: Уничтожает частицу
void Magic_ParticleDestruction(HM_PARTICLE hmParticle);

// --------------------------------------------------------------------------------------

enum MAGIC_STREAM_ENUM {MAGIC_STREAM_READ, MAGIC_STREAM_WRITE, MAGIC_STREAM_ADD, MAGIC_STREAM__MAX, MAGIC_STREAM__ERROR=MAGIC_ERROR};

// eng: Opens stream from file
// rus: Открывает поток из файла
int Magic_StreamOpenFile(const char* file_name, MAGIC_STREAM_ENUM mode);

// eng: Opens stream in memory
// rus: Открывает поток в памяти
int Magic_StreamOpenMemory(const char* address, unsigned int length, MAGIC_STREAM_ENUM mode);

// eng: Closes stream that was previously opened by Magic_StreamOpenFile or Magic_StreamOpenMemory
// rus: Закрывает поток, открытый ранее через Magic_StreamOpenFile или Magic_StreamOpenMemory
int Magic_StreamClose(HM_STREAM hmStream);

// eng: Closing all opened streams
// rus: Закрытие всех открытых поток
void Magic_StreamCloseAll();

// eng: Returns the length of stream
// rus: Возвращает размер потока
unsigned int Magic_StreamGetLength(HM_STREAM hmStream);

// eng: Returns current stream position
// rus: Возвращает текущую позицию в потоке
unsigned int Magic_StreamGetPosition(HM_STREAM hmStream);

// eng: Sets the position of stream that was opened in read only mode
// rus: Устанавливает позицию потока, открытого в режиме чтения
int Magic_StreamSetPosition(HM_STREAM hmStream, unsigned int position);

// eng: Returns current stream mode
// rus: Возвращает текущий режим потока
MAGIC_STREAM_ENUM Magic_StreamGetMode(HM_STREAM hmStream);

// eng: Sets new stream mode
// rus: Устанавливает новый режим потока
int Magic_StreamSetMode(HM_STREAM hmStream, MAGIC_STREAM_ENUM mode);

// eng: Returns file name of stream opened from file
// rus: Возвращает имя файла для потока, открытого из файла
const char* Magic_StreamGetFileName(HM_STREAM hmStream);

// eng: Reads specified number of bytes from stream
// rus: Читает из потока указанное количество байт
int Magic_StreamRead(HM_STREAM hmStream, char* data, unsigned int count);

// eng: Writes specified number of bytes into stream
// rus: Сохраняет в поток указанное количество байт
int Magic_StreamWrite(HM_STREAM hmStream, const char* data, unsigned int count);

// eng: Copying particle space from stream into emitter
// rus: Копирование пространства частиц в эмиттер из потока
int Magic_LoadArrayFromStream(HM_EMITTER hmEmitter, HM_STREAM hmStream);

// eng: Copying particle space from emitter into stream
// rus: Копирование пространства частиц из эмиттера а поток
int Magic_SaveArrayToStream(HM_EMITTER hmEmitter, HM_STREAM hmStream);

// eng: Sets emitter animation position to left position of visibility range
// rus: Устанавливает позицию анимации эмиттера на левую позицию интервала видимости
int Magic_EmitterToInterval1_Stream(HM_EMITTER hmEmitter, float speed_factor, HM_STREAM hmStream);

// eng: Sets the coordinates of the emitter (attached physic objects are moved too)
// rus: Устанавливает координаты эмиттера (происходит перемещение всех прицепленных физ. объектов)
int Magic_SetEmitterPositionWithAttachedPhysicObjects(HM_EMITTER hmEmitter, MAGIC_POSITION* pos);

void Magic_DestroyAll();


// *****************************************************************************************************************************
// ******* rus: Следующие данные не участвуют в работе API и сохранены для совместимости с редактором и будущими версиями ******
// *****************************************************************************************************************************

#define MAGIC_CHANGE_ATLAS_SHADER	4
#define MAGIC_ADDITIONAL_TYPE_TRAIL_BEAM	4

#define MAGIC_HLSL	0
#define MAGIC_GLSL	1
#define MAGIC_CG	2

// Структура с информацией о шейдере
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

// Структура с информацией о параметрах шейдера
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
	int shader;					// индекс загруженного шейдера
	const char* name;			// имя uniform-переменной (это не имя типа, а имя самой переменной)
	const char* path;			// путь к uniform (это путь именно к типу)
	int type;					// тип данных (bool, int , float)
	int count;					// количество данных
	int size;					// размер данных в байтах
	const char* data;			// массив с инициализацией данных
	int function;				// индекс функции, которая установит переменную
	void* handle;				// переменная в шейдере
};

// ---------------------------------------------------------------------------------


#ifdef __cplusplus
}
#endif

#endif
