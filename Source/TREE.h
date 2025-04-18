#ifndef __TREE_H__
#define __TREE_H__

///////////////////////////////////////
// Constants and Macros              //
///////////////////////////////////////

typedef int TREE_Bool;
typedef int TREE_Int;
typedef unsigned TREE_UInt;
typedef unsigned char TREE_Byte;
typedef char TREE_Char;
typedef TREE_Char const* TREE_String;
typedef unsigned long long TREE_Size;
typedef int(*TREE_Function)();

#define TREE_FALSE 0
#define TREE_TRUE 1
#define TREE_COLOR_STRING_LENGTH 6

///////////////////////////////////////
// Error Handling                    //
///////////////////////////////////////

typedef enum _TREE_ErrorCode
{
	TREE_OK = 0,
	TREE_ERROR = 1,

	TREE_ERROR_ARG_NULL = 100,
	TREE_ERROR_ARG_OUT_OF_RANGE = 101,
	TREE_ERROR_ARG_INVALID = 102,

	TREE_ERROR_ALLOC = 200,
} TREE_ErrorCode;

TREE_String TREE_GetErrorString(TREE_ErrorCode code);

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
// Extent                            //
///////////////////////////////////////

typedef struct _TREE_Extent
{
	TREE_UInt width;
	TREE_UInt height;
} TREE_Extent;

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

TREE_ErrorCode TREE_Pattern_Init(TREE_Pattern* pattern, TREE_UInt size);

TREE_ErrorCode TREE_Pattern_InitFromString(TREE_Pattern* pattern, TREE_String string, TREE_ColorPair colorPair);

TREE_ErrorCode TREE_Pattern_Set(TREE_Pattern* pattern, TREE_UInt index, TREE_Pixel pixel);

TREE_ErrorCode TREE_Pattern_Get(TREE_Pattern* pattern, TREE_UInt index, TREE_Pixel* pixel);

void TREE_Pattern_Free(TREE_Pattern* pattern);

///////////////////////////////////////
// Image                             //
///////////////////////////////////////

typedef struct _TREE_Image
{
	TREE_Extent size;
	TREE_Char* text;
	TREE_ColorPair* colors;
} TREE_Image;

TREE_ErrorCode TREE_Image_Init(TREE_Image* image, TREE_Extent size);

void TREE_Image_Free(TREE_Image* image);

TREE_ErrorCode TREE_Image_Set(TREE_Image* image, TREE_Offset offset, TREE_Char character, TREE_ColorPair colorPair);

TREE_ErrorCode TREE_Image_Get(TREE_Image* image, TREE_Offset offset, TREE_Char* character, TREE_ColorPair* colorPair);

TREE_ErrorCode TREE_Image_DrawImage(TREE_Image* image, TREE_Offset offset, TREE_Image* other);

TREE_ErrorCode TREE_Image_DrawString(TREE_Image* image, TREE_Offset offset, TREE_String string, TREE_ColorPair colorPair);

TREE_ErrorCode TREE_Image_DrawLine(TREE_Image* image, TREE_Offset start, TREE_Offset end, TREE_Pattern* pattern);

TREE_ErrorCode TREE_Image_DrawRect(TREE_Image* image, TREE_Offset start, TREE_Extent size, TREE_Pattern* pattern);

TREE_ErrorCode TREE_Image_FillRect(TREE_Image* image, TREE_Offset start, TREE_Extent size, TREE_Pixel pixel);

TREE_ErrorCode TREE_Image_Clear(TREE_Image* image, TREE_Pixel pixel);

///////////////////////////////////////
// Surface                           //
///////////////////////////////////////

typedef struct _TREE_Surface
{
	TREE_Image image;
	TREE_Char* text;
} TREE_Surface;

TREE_ErrorCode TREE_Surface_Init(TREE_Surface* surface, TREE_Extent size);

void TREE_Surface_Free(TREE_Surface* surface);

TREE_ErrorCode TREE_Surface_Refresh(TREE_Surface* surface);

///////////////////////////////////////
// Window                            //
///////////////////////////////////////

TREE_ErrorCode TREE_Window_Present(TREE_Surface* surface);

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

TREE_Char TREE_Key_GetChar(TREE_Key key);

TREE_String TREE_Key_GetString(TREE_Key key);

///////////////////////////////////////
// Input                             //
///////////////////////////////////////

TREE_Key TREE_Input_GetKey();

///////////////////////////////////////
// Control                           //
///////////////////////////////////////

typedef enum _TREE_ControlType
{
	TREE_CONTROL_TYPE_NONE,
	TREE_CONTROL_TYPE_LABEL
} TREE_ControlType;

typedef struct _TREE_Control
{
	TREE_ControlType type;
	TREE_Bool focused;
	TREE_Offset offset;
	TREE_Image image;
	void* data;
} TREE_Control;

TREE_ErrorCode TREE_Control_Init(TREE_Control* control, TREE_Offset offset, TREE_Extent extent);

void TREE_Control_Free(TREE_Control* control);

///////////////////////////////////////
// Control: Label                    //
///////////////////////////////////////

TREE_ErrorCode TREE_Control_Label_Init(TREE_Control* control, TREE_Offset offset, TREE_String text, TREE_ColorPair colorPair);

TREE_ErrorCode TREE_Control_Label_SetText(TREE_Control* control, TREE_String text, TREE_ColorPair colorPair);

TREE_String TREE_Control_Label_GetText(TREE_Control* control);

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

	TREE_Surface* surface;
} TREE_Application;

TREE_ErrorCode TREE_Application_Init(TREE_Application* application, TREE_Surface* surface, TREE_Size capacity);

void TREE_Application_Free(TREE_Application* application);

TREE_ErrorCode TREE_Application_AddControl(TREE_Application* application, TREE_Control* control);

TREE_ErrorCode TREE_Application_Run(TREE_Application* application);

///////////////////////////////////////
//                                   //
///////////////////////////////////////

#endif // __TREE_H__