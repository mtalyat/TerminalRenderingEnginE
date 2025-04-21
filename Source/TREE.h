#ifndef __TREE_H__
#define __TREE_H__

///////////////////////////////////////
// Constants and Macros              //
///////////////////////////////////////

typedef int TREE_Bool;
typedef int TREE_Int;
typedef unsigned TREE_UInt;
typedef float TREE_Float;
typedef unsigned char TREE_Byte;
typedef char TREE_Char;
typedef TREE_Char const* TREE_String;
typedef unsigned long long TREE_Size;
typedef void* TREE_Data;

#define TREE_FALSE 0
#define TREE_TRUE 1
#define TREE_COLOR_STRING_LENGTH 6

#define TREE_NEW(type) ((type*)malloc(sizeof(type)))
#define TREE_NEW_ARRAY(type, count) ((type*)malloc((count) * sizeof(type)))
#define TREE_REPLACE(ptr, newPtr) do { free(ptr); ptr = newPtr; } while (0)
#define TREE_DELETE(ptr) TREE_REPLACE(ptr, NULL)
#define TREE_COPY(dest, src, type) memcpy(dest, src, sizeof(type))
#define TREE_COPY_ARRAY(dest, src, type, count) memcpy(dest, src, (count) * sizeof(type))

///////////////////////////////////////
// Error Handling                    //
///////////////////////////////////////

typedef enum _TREE_Result
{
	TREE_OK = 0,
	TREE_CANCEL = 1,
	TREE_NOT_IMPLEMENTED = 2,

	TREE_ERROR = 100,

	TREE_ERROR_ARG_NULL = 200,
	TREE_ERROR_ARG_OUT_OF_RANGE = 201,
	TREE_ERROR_ARG_INVALID = 202,

	TREE_ERROR_ALLOC = 300,

	TREE_ERROR_PRESENTATION = 400,
} TREE_Result;

TREE_String TREE_Result_ToString(TREE_Result code);

///////////////////////////////////////
// Color and ColorPair               //
///////////////////////////////////////

typedef enum _TREE_Color
{
	TREE_COLOR_BLACK = 0,
	TREE_COLOR_RED = 1,
	TREE_COLOR_GREEN = 2,
	TREE_COLOR_YELLOW = 3,
	TREE_COLOR_BLUE = 4,
	TREE_COLOR_MAGENTA = 5,
	TREE_COLOR_CYAN = 6,
	TREE_COLOR_WHITE = 7,
	TREE_COLOR_BRIGHT_BLACK = 8,
	TREE_COLOR_BRIGHT_RED = 9,
	TREE_COLOR_BRIGHT_GREEN = 10,
	TREE_COLOR_BRIGHT_YELLOW = 11,
	TREE_COLOR_BRIGHT_BLUE = 12,
	TREE_COLOR_BRIGHT_MAGENTA = 13,
	TREE_COLOR_BRIGHT_CYAN = 14,
	TREE_COLOR_BRIGHT_WHITE = 15,

	TREE_COLOR_DEFAULT_FOREGROUND = TREE_COLOR_BRIGHT_WHITE,
	TREE_COLOR_DEFAULT_BACKGROUND = TREE_COLOR_BLACK,
} TREE_Color;

typedef TREE_Byte TREE_ColorPair;

TREE_ColorPair TREE_ColorPair_Create(TREE_Color foreground, TREE_Color background);

TREE_ColorPair TREE_ColorPair_CreateDefault();

TREE_Color TREE_ColorPair_GetForeground(TREE_ColorPair colorPair);

TREE_Color TREE_ColorPair_GetBackground(TREE_ColorPair colorPair);

TREE_String TREE_Color_GetForegroundString(TREE_Color color);

TREE_String TREE_Color_GetBackgroundString(TREE_Color color);

TREE_String TREE_Color_GetResetString();

///////////////////////////////////////
// Offset                            //
///////////////////////////////////////

typedef struct _TREE_Offset
{
	TREE_Int x;
	TREE_Int y;
} TREE_Offset;

///////////////////////////////////////
// Coords                            //
///////////////////////////////////////

typedef struct _TREE_Coords
{
	TREE_Float x;
	TREE_Float y;
} TREE_Coords;

///////////////////////////////////////
// Extent                            //
///////////////////////////////////////

typedef struct _TREE_Extent
{
	TREE_UInt width;
	TREE_UInt height;
} TREE_Extent;

///////////////////////////////////////
// Rect                              //
///////////////////////////////////////

typedef struct _TREE_Rect
{
	TREE_Offset offset;
	TREE_Extent extent;
} TREE_Rect;

TREE_Bool TREE_Rect_IsOverlapping(TREE_Rect const* rectA, TREE_Rect const* rectB);

TREE_Rect TREE_Rect_Combine(TREE_Rect const* rectA, TREE_Rect const* rectB);

TREE_Rect TREE_Rect_GetIntersection(TREE_Rect const* rectA, TREE_Rect const* rectB);

///////////////////////////////////////
// Pixel                             //
///////////////////////////////////////

typedef struct _TREE_Pixel
{
	TREE_Char character;
	TREE_ColorPair colorPair;
} TREE_Pixel;

TREE_Pixel TREE_Pixel_CreateDefault();

///////////////////////////////////////
// Pattern                           //
///////////////////////////////////////

typedef struct _TREE_Pattern
{
	TREE_UInt size;
	TREE_Pixel* pixels;
} TREE_Pattern;

TREE_Result TREE_Pattern_Init(TREE_Pattern* pattern, TREE_UInt size);

TREE_Result TREE_Pattern_InitFromString(TREE_Pattern* pattern, TREE_String string, TREE_ColorPair colorPair);

TREE_Result TREE_Pattern_Set(TREE_Pattern* pattern, TREE_UInt index, TREE_Pixel pixel);

TREE_Result TREE_Pattern_Get(TREE_Pattern* pattern, TREE_UInt index, TREE_Pixel* pixel);

void TREE_Pattern_Free(TREE_Pattern* pattern);

///////////////////////////////////////
// Image                             //
///////////////////////////////////////

typedef struct _TREE_Image
{
	TREE_Extent extent;
	TREE_Char* text;
	TREE_ColorPair* colors;
} TREE_Image;

TREE_Result TREE_Image_Init(TREE_Image* image, TREE_Extent size);

void TREE_Image_Free(TREE_Image* image);

TREE_Result TREE_Image_Set(TREE_Image* image, TREE_Offset offset, TREE_Char character, TREE_ColorPair colorPair);

TREE_Result TREE_Image_Get(TREE_Image* image, TREE_Offset offset, TREE_Char* character, TREE_ColorPair* colorPair);

TREE_Result TREE_Image_DrawImage(TREE_Image* image, TREE_Offset offset, TREE_Image* other, TREE_Offset otherOffset, TREE_Extent extent);

TREE_Result TREE_Image_DrawString(TREE_Image* image, TREE_Offset offset, TREE_String string, TREE_ColorPair colorPair);

TREE_Result TREE_Image_DrawLine(TREE_Image* image, TREE_Offset start, TREE_Offset end, TREE_Pattern* pattern);

TREE_Result TREE_Image_DrawRect(TREE_Image* image, TREE_Offset start, TREE_Extent size, TREE_Pattern* pattern);

TREE_Result TREE_Image_FillRect(TREE_Image* image, TREE_Offset start, TREE_Extent size, TREE_Pixel pixel);

TREE_Result TREE_Image_Clear(TREE_Image* image, TREE_Pixel pixel);

///////////////////////////////////////
// Surface                           //
///////////////////////////////////////

typedef struct _TREE_Surface
{
	TREE_Image image;
	TREE_Char* text;
} TREE_Surface;

TREE_Result TREE_Surface_Init(TREE_Surface* surface, TREE_Extent size);

void TREE_Surface_Free(TREE_Surface* surface);

TREE_Result TREE_Surface_Refresh(TREE_Surface* surface);

///////////////////////////////////////
// Window                            //
///////////////////////////////////////

TREE_Result TREE_Window_Present(TREE_Surface* surface);

TREE_Extent TREE_Window_GetExtent();

///////////////////////////////////////
// Key                               //
///////////////////////////////////////

typedef enum _TREE_Key
{
	TREE_KEY_NULL = 0,
	TREE_KEY_BACKSPACE = 8,
	TREE_KEY_TAB = 9,
	TREE_KEY_ENTER = 10,
	TREE_KEY_ESCAPE = 27,
	TREE_KEY_SPACE = 32,
	TREE_KEY_EXCLAMATION = 33,
	TREE_KEY_DOUBLE_QUOTE = 34,
	TREE_KEY_HASH = 35,
	TREE_KEY_DOLLAR = 36,
	TREE_KEY_PERCENT = 37,
	TREE_KEY_AMPERSAND = 38,
	TREE_KEY_SINGLE_QUOTE = 39,
	TREE_KEY_LEFT_PARENTHESIS = 40,
	TREE_KEY_RIGHT_PARENTHESIS = 41,
	TREE_KEY_ASTERISK = 42,
	TREE_KEY_PLUS = 43,
	TREE_KEY_COMMA = 44,
	TREE_KEY_MINUS = 45,
	TREE_KEY_PERIOD = 46,
	TREE_KEY_SLASH = 47,
	TREE_KEY_0 = 48,
	TREE_KEY_1 = 49,
	TREE_KEY_2 = 50,
	TREE_KEY_3 = 51,
	TREE_KEY_4 = 52,
	TREE_KEY_5 = 53,
	TREE_KEY_6 = 54,
	TREE_KEY_7 = 55,
	TREE_KEY_8 = 56,
	TREE_KEY_9 = 57,
	TREE_KEY_COLON = 58,
	TREE_KEY_SEMICOLON = 59,
	TREE_KEY_LESS_THAN = 60,
	TREE_KEY_EQUAL = 61,
	TREE_KEY_GREATER_THAN = 62,
	TREE_KEY_QUESTION = 63,
	TREE_KEY_AT = 64,
	TREE_KEY_A = 65,
	TREE_KEY_B = 66,
	TREE_KEY_C = 67,
	TREE_KEY_D = 68,
	TREE_KEY_E = 69,
	TREE_KEY_F = 70,
	TREE_KEY_G = 71,
	TREE_KEY_H = 72,
	TREE_KEY_I = 73,
	TREE_KEY_J = 74,
	TREE_KEY_K = 75,
	TREE_KEY_L = 76,
	TREE_KEY_M = 77,
	TREE_KEY_N = 78,
	TREE_KEY_O = 79,
	TREE_KEY_P = 80,
	TREE_KEY_Q = 81,
	TREE_KEY_R = 82,
	TREE_KEY_S = 83,
	TREE_KEY_T = 84,
	TREE_KEY_U = 85,
	TREE_KEY_V = 86,
	TREE_KEY_W = 87,
	TREE_KEY_X = 88,
	TREE_KEY_Y = 89,
	TREE_KEY_Z = 90,
	TREE_KEY_LEFT_BRACKET = 91,
	TREE_KEY_BACKSLASH = 92,
	TREE_KEY_RIGHT_BRACKET = 93,
	TREE_KEY_CARET = 94,
	TREE_KEY_UNDERSCORE = 95,
	TREE_KEY_GRAVE_ACCENT = 96,
	TREE_KEY_a = 97,
	TREE_KEY_b = 98,
	TREE_KEY_c = 99,
	TREE_KEY_d = 100,
	TREE_KEY_e = 101,
	TREE_KEY_f = 102,
	TREE_KEY_g = 103,
	TREE_KEY_h = 104,
	TREE_KEY_i = 105,
	TREE_KEY_j = 106,
	TREE_KEY_k = 107,
	TREE_KEY_l = 108,
	TREE_KEY_m = 109,
	TREE_KEY_n = 110,
	TREE_KEY_o = 111,
	TREE_KEY_p = 112,
	TREE_KEY_q = 113,
	TREE_KEY_r = 114,
	TREE_KEY_s = 115,
	TREE_KEY_t = 116,
	TREE_KEY_u = 117,
	TREE_KEY_v = 118,
	TREE_KEY_w = 119,
	TREE_KEY_x = 120,
	TREE_KEY_y = 121,
	TREE_KEY_z = 122,
	TREE_KEY_LEFT_CURLY_BRACE = 123,
	TREE_KEY_PIPE = 124,
	TREE_KEY_RIGHT_CURLY_BRACE = 125,
	TREE_KEY_TILDE = 126,
	TREE_KEY_DELETE = 127,

	TREE_KEY_UP = 1'000,
	TREE_KEY_DOWN = 1'001,
	TREE_KEY_LEFT = 1'002,
	TREE_KEY_RIGHT = 1'003,
	TREE_KEY_PAGE_UP = 1'004,
	TREE_KEY_PAGE_DOWN = 1'005,
	TREE_KEY_HOME = 1'006,
	TREE_KEY_END = 1'007,
	TREE_KEY_INSERT = 1'008,

	TREE_KEY_F1 = 2'000,
	TREE_KEY_F2 = 2'001,
	TREE_KEY_F3 = 2'002,
	TREE_KEY_F4 = 2'003,
	TREE_KEY_F5 = 2'004,
	TREE_KEY_F6 = 2'005,
	TREE_KEY_F7 = 2'006,
	TREE_KEY_F8 = 2'007,
	TREE_KEY_F9 = 2'008,
	TREE_KEY_F10 = 2'009,
	TREE_KEY_F11 = 2'010,
	TREE_KEY_F12 = 2'011,

	TREE_KEY_NUMPAD_0 = 3'000,
	TREE_KEY_NUMPAD_1 = 3'001,
	TREE_KEY_NUMPAD_2 = 3'002,
	TREE_KEY_NUMPAD_3 = 3'003,
	TREE_KEY_NUMPAD_4 = 3'004,
	TREE_KEY_NUMPAD_5 = 3'005,
	TREE_KEY_NUMPAD_6 = 3'006,
	TREE_KEY_NUMPAD_7 = 3'007,
	TREE_KEY_NUMPAD_8 = 3'008,
	TREE_KEY_NUMPAD_9 = 3'009,
	TREE_KEY_NUMPAD_MULTIPLY = 3'010,
	TREE_KEY_NUMPAD_ADD = 3'011,
	TREE_KEY_NUMPAD_SUBTRACT = 3'012,
	TREE_KEY_NUMPAD_DECIMAL = 3'013,
	TREE_KEY_NUMPAD_DIVIDE = 3'014,
	TREE_KEY_NUMPAD_ENTER = 3'015,
} TREE_Key;

TREE_Char TREE_Key_ToChar(TREE_Key key);

TREE_String TREE_Key_ToString(TREE_Key key);

///////////////////////////////////////
// Input                             //
///////////////////////////////////////

TREE_Key TREE_Input_GetKey();

///////////////////////////////////////
// Direction                         //
///////////////////////////////////////

typedef enum _TREE_Direction
{
	TREE_DIRECTION_NONE,
	TREE_DIRECTION_EAST,
	TREE_DIRECTION_NORTH,
	TREE_DIRECTION_WEST,
	TREE_DIRECTION_SOUTH,
} TREE_Direction;

TREE_Direction TREE_Direction_Opposite(TREE_Direction direction);

///////////////////////////////////////
// Alignment                         //
///////////////////////////////////////

typedef enum _TREE_Alignment
{
	TREE_ALIGNMENT_NONE = 0x0,
	TREE_ALIGNMENT_LEFT = 0x1,
	TREE_ALIGNMENT_CENTER = 0x2,
	TREE_ALIGNMENT_RIGHT = 0x4,
	TREE_ALIGNMENT_TOP = 0x8,
	TREE_ALIGNMENT_MIDDLE = 0x10,
	TREE_ALIGNMENT_BOTTOM = 0x20,

	TREE_ALIGNMENT_TOPLEFT = (TREE_ALIGNMENT_LEFT | TREE_ALIGNMENT_TOP),
	TREE_ALIGNMENT_TOPCENTER = (TREE_ALIGNMENT_CENTER | TREE_ALIGNMENT_TOP),
	TREE_ALIGNMENT_TOPRIGHT = (TREE_ALIGNMENT_RIGHT | TREE_ALIGNMENT_TOP),
	TREE_ALIGNMENT_MIDDLELEFT = (TREE_ALIGNMENT_LEFT | TREE_ALIGNMENT_MIDDLE),
	TREE_ALIGNMENT_MIDDLECENTER = (TREE_ALIGNMENT_CENTER | TREE_ALIGNMENT_MIDDLE),
	TREE_ALIGNMENT_MIDDLERIGHT = (TREE_ALIGNMENT_RIGHT | TREE_ALIGNMENT_MIDDLE),
	TREE_ALIGNMENT_BOTTOMLEFT = (TREE_ALIGNMENT_LEFT | TREE_ALIGNMENT_BOTTOM),
	TREE_ALIGNMENT_BOTTOMCENTER = (TREE_ALIGNMENT_CENTER | TREE_ALIGNMENT_BOTTOM),
	TREE_ALIGNMENT_BOTTOMRIGHT = (TREE_ALIGNMENT_RIGHT | TREE_ALIGNMENT_BOTTOM),

	TREE_ALIGNMENT_VERTICALSTRETCH = (TREE_ALIGNMENT_TOP | TREE_ALIGNMENT_BOTTOM),
	TREE_ALIGNMENT_HORIZONTALSTRETCH = (TREE_ALIGNMENT_LEFT | TREE_ALIGNMENT_RIGHT),
	TREE_ALIGNMENT_STRETCH = (TREE_ALIGNMENT_VERTICALSTRETCH | TREE_ALIGNMENT_HORIZONTALSTRETCH),

	TREE_ALIGNMENT_TOPSTRETCH = (TREE_ALIGNMENT_HORIZONTALSTRETCH | TREE_ALIGNMENT_TOP),
	TREE_ALIGNMENT_MIDDLESTRETCH = (TREE_ALIGNMENT_HORIZONTALSTRETCH | TREE_ALIGNMENT_MIDDLE),
	TREE_ALIGNMENT_BOTTOMSTRETCH = (TREE_ALIGNMENT_HORIZONTALSTRETCH | TREE_ALIGNMENT_BOTTOM),
	TREE_ALIGNMENT_LEFTSTRETCH = (TREE_ALIGNMENT_VERTICALSTRETCH | TREE_ALIGNMENT_LEFT),
	TREE_ALIGNMENT_CENTERSTRETCH = (TREE_ALIGNMENT_VERTICALSTRETCH | TREE_ALIGNMENT_CENTER),
	TREE_ALIGNMENT_RIGHTSTRETCH = (TREE_ALIGNMENT_VERTICALSTRETCH | TREE_ALIGNMENT_RIGHT),

	TREE_ALIGNMENT_ALL = (TREE_ALIGNMENT_LEFT | TREE_ALIGNMENT_CENTER | TREE_ALIGNMENT_RIGHT |
		TREE_ALIGNMENT_TOP | TREE_ALIGNMENT_MIDDLE | TREE_ALIGNMENT_BOTTOM),
} TREE_Alignment;

///////////////////////////////////////
// Event                             //
///////////////////////////////////////

typedef enum _TREE_EventType
{
	TREE_EVENT_TYPE_NONE = 000,
	TREE_EVENT_TYPE_REFRESH = 001,
	TREE_EVENT_TYPE_DRAW = 002,

	TREE_EVENT_TYPE_INPUT_KEY = 1'000,
} TREE_EventType;

typedef struct _TREE_Application TREE_Application;
typedef struct _TREE_Control TREE_Control;

typedef struct _TREE_Event
{
	TREE_EventType type;
	TREE_Application* application;
	TREE_Control* control;
	TREE_Data data;
} TREE_Event;

typedef TREE_Result(*TREE_EventHandler)(TREE_Event const* event);

typedef struct _TREE_EventData_Draw
{
	TREE_Image* target;
	TREE_Rect dirtyRect;
} TREE_EventData_Draw;

typedef struct _TREE_EventData_InputKey
{
	TREE_Key key;
} TREE_EventData_InputKey;

///////////////////////////////////////
// Transform                         //
///////////////////////////////////////

typedef struct _TREE_Transform TREE_Transform;

typedef struct _TREE_Transform
{
	TREE_Offset localOffset;
	TREE_Coords localPivot;
	TREE_Extent localExtent;
	TREE_Alignment localAlignment;

	TREE_Transform* parent;
	TREE_Transform* child;
	TREE_Transform* sibling;

	TREE_Bool dirty;
	TREE_Rect globalRect;
} TREE_Transform;

TREE_Result TREE_Transform_Init(TREE_Transform* transform, TREE_Offset localOffset, TREE_Coords localPivot, TREE_Extent localExtent, TREE_Alignment localAlignment);

void TREE_Transform_Free(TREE_Transform* transform);

// marks the transform (and children) as dirty
TREE_Result TREE_Transform_Dirty(TREE_Transform* transform);

TREE_Result TREE_Transform_SetParent(TREE_Transform* transform, TREE_Transform* parent);

TREE_Result TREE_Transform_DisconnectChildren(TREE_Transform* transform);

// calculate the global rectangle based on the local transform and parent
TREE_Result TREE_Transform_Refresh(TREE_Transform* transform);

///////////////////////////////////////
// Control                           //
///////////////////////////////////////

typedef enum _TREE_ControlType
{
	TREE_CONTROL_TYPE_NONE,
	TREE_CONTROL_TYPE_LABEL
} TREE_ControlType;

typedef enum _TREE_ControlFlags
{
	TREE_CONTROL_FLAGS_NONE = 0x0,
	TREE_CONTROL_FLAGS_FOCUSABLE = 0x1,
} TREE_ControlFlag;

typedef enum _TREE_ControlStateFlags
{
	TREE_CONTROL_STATE_FLAGS_NONE = 0x0,
	TREE_CONTROL_STATE_FLAGS_DIRTY = 0x1,
	TREE_CONTROL_STATE_FLAGS_FOCUSED = 0x2,
} TREE_ControlStateFlags;

typedef enum _TREE_ControlLink
{
	TREE_CONTROL_LINK_NONE,
	TREE_CONTROL_LINK_SINGLE,
	TREE_CONTROL_LINK_DOUBLE,
	TREE_CONTROL_LINK_UNIQUE
} TREE_ControlLink;

typedef struct _TREE_Control
{
	TREE_ControlType type;
	TREE_ControlFlag flags;
	TREE_ControlStateFlags stateFlags;
	TREE_Transform* transform;
	TREE_Image* image;
	TREE_Control* adjacent[4];
	TREE_EventHandler eventHandler;
	TREE_Data data;
} TREE_Control;

TREE_Result TREE_Control_Init(TREE_Control* control, TREE_Transform* parent, TREE_EventHandler eventHandler, TREE_Data data);

void TREE_Control_Free(TREE_Control* control);

TREE_Result TREE_Control_Link(TREE_Control* control, TREE_Direction direction, TREE_ControlLink link, TREE_Control* other);

TREE_Result TREE_Control_HandleEvent(TREE_Control* control, TREE_Event const* event);

///////////////////////////////////////
// Control: Label                    //
///////////////////////////////////////

typedef struct _TREE_Control_LabelData
{
	TREE_Char* text;
	TREE_Alignment alignment;
	TREE_ColorPair normalColor;
} TREE_Control_LabelData;

TREE_Result TREE_Control_LabelData_Init(TREE_Control_LabelData* data, TREE_String text, TREE_Alignment alignment, TREE_ColorPair normalColor);

void TREE_Control_LabelData_Free(TREE_Control_LabelData* data);

TREE_Result TREE_Control_Label_Init(TREE_Control* control, TREE_Transform* parent, TREE_Control_LabelData* data);

TREE_Result TREE_Control_Label_SetText(TREE_Control* control, TREE_String text, TREE_ColorPair colorPair);

TREE_String TREE_Control_Label_GetText(TREE_Control* control);

TREE_Result TREE_Control_Label_Refresh(TREE_Control* control);

TREE_Result TREE_Control_Label_EventHandler(TREE_Event const* event);

///////////////////////////////////////
// Control: Button                   //
///////////////////////////////////////

typedef struct _TREE_Control_ButtonData
{
	TREE_Char* text;
	TREE_ColorPair normalColor;
	TREE_ColorPair focusedColor;
	TREE_ColorPair pressedColor;
} TREE_Control_ButtonData;

///////////////////////////////////////
// Control: ?????                    //
///////////////////////////////////////

///////////////////////////////////////
// Application                       //
///////////////////////////////////////

typedef struct _TREE_Application
{
	TREE_Control** controls;
	TREE_Size controlsSize;
	TREE_Size controlsCapacity;

	TREE_Bool running;

	TREE_EventHandler eventHandler;

	TREE_Surface* surface;
} TREE_Application;

TREE_Result TREE_Application_Init(TREE_Application* application, TREE_Surface* surface, TREE_Size capacity, TREE_EventHandler eventHandler);

void TREE_Application_Free(TREE_Application* application);

TREE_Result TREE_Application_AddControl(TREE_Application* application, TREE_Control* control);

TREE_Result TREE_Application_DispatchEvent(TREE_Application* application, TREE_Event const* event);

TREE_Result TREE_Application_Run(TREE_Application* application);

void TREE_Application_Quit(TREE_Application* application);

///////////////////////////////////////
//                                   //
///////////////////////////////////////

#endif // __TREE_H__