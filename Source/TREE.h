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
typedef long TREE_Long;
typedef long TREE_Time;
typedef void(*TREE_Function)(void*);

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

TREE_Result TREE_Image_Set(TREE_Image* image, TREE_Offset offset, TREE_Pixel pixel);

TREE_Pixel TREE_Image_Get(TREE_Image* image, TREE_Offset offset);

TREE_Result TREE_Image_Resize(TREE_Image* image, TREE_Extent extent);

TREE_Result TREE_Image_DrawImage(TREE_Image* image, TREE_Offset offset, TREE_Image const* other, TREE_Offset otherOffset, TREE_Extent extent);

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
// Cursor                            //
///////////////////////////////////////

TREE_Result TREE_Cursor_SetVisible(TREE_Bool visible);

///////////////////////////////////////
// Key                               //
///////////////////////////////////////

typedef enum _TREE_Key
{
	TREE_KEY_NONE = 0,
	TREE_KEY_BACKSPACE = 8,
	TREE_KEY_TAB = 9,
	TREE_KEY_ENTER = 13,
	TREE_KEY_SHIFT = 16,
	TREE_KEY_CONTROL = 17,
	TREE_KEY_ALT = 18,
	TREE_KEY_PAUSE = 19,
	TREE_KEY_CAPS_LOCK = 20,
	TREE_KEY_ESCAPE = 27,
	TREE_KEY_SPACE = 32,
	TREE_KEY_PAGE_UP = 33,
	TREE_KEY_PAGE_DOWN = 34,
	TREE_KEY_END = 35,
	TREE_KEY_HOME = 36,
	TREE_KEY_LEFT_ARROW = 37,
	TREE_KEY_UP_ARROW = 38,
	TREE_KEY_RIGHT_ARROW = 39,
	TREE_KEY_DOWN_ARROW = 40,
	TREE_KEY_PRINT_SCREEN = 44,
	TREE_KEY_INSERT = 45,
	TREE_KEY_DELETE = 46,
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
	TREE_KEY_NUMPAD_0 = 96,
	TREE_KEY_NUMPAD_1 = 97,
	TREE_KEY_NUMPAD_2 = 98,
	TREE_KEY_NUMPAD_3 = 99,
	TREE_KEY_NUMPAD_4 = 100,
	TREE_KEY_NUMPAD_5 = 101,
	TREE_KEY_NUMPAD_6 = 102,
	TREE_KEY_NUMPAD_7 = 103,
	TREE_KEY_NUMPAD_8 = 104,
	TREE_KEY_NUMPAD_9 = 105,
	TREE_KEY_MULTIPLY = 106,
	TREE_KEY_ADD = 107,
	TREE_KEY_SUBTRACT = 109,
	TREE_KEY_DECIMAL = 110,
	TREE_KEY_DIVIDE = 111,
	TREE_KEY_F1 = 112,
	TREE_KEY_F2 = 113,
	TREE_KEY_F3 = 114,
	TREE_KEY_F4 = 115,
	TREE_KEY_F5 = 116,
	TREE_KEY_F6 = 117,
	TREE_KEY_F7 = 118,
	TREE_KEY_F8 = 119,
	TREE_KEY_F9 = 120,
	TREE_KEY_F10 = 121,
	TREE_KEY_F11 = 122,
	TREE_KEY_F12 = 123,
	TREE_KEY_NUM_LOCK = 144,
	TREE_KEY_SCROLL_LOCK = 145,
	TREE_KEY_SEMICOLON = 186, // ; and :
	TREE_KEY_EQUALS = 187, // = and +
	TREE_KEY_COMMA = 188, // , and <
	TREE_KEY_MINUS = 189, // - and _
	TREE_KEY_PERIOD = 190, // . and >
	TREE_KEY_SLASH = 191, // / and ?
	TREE_KEY_TILDE = 192, // ~ and `
	TREE_KEY_LEFT_BRACKET = 219, // [ and {
	TREE_KEY_BACKSLASH = 220, // \ and |
	TREE_KEY_RIGHT_BRACKET = 221, // ] and }
	TREE_KEY_APOSTROPHE = 222, // ' and "

	TREE_KEY_MAX = 222,
	TREE_KEY_COUNT = 97,
} TREE_Key;

TREE_String TREE_Key_ToString(TREE_Key key);

///////////////////////////////////////
// Key Modifier Flags                //
///////////////////////////////////////

typedef enum TREE_KeyModifierFlags
{
	TREE_KEY_MODIFIER_FLAGS_NONE = 0x0,
	TREE_KEY_MODIFIER_FLAGS_SHIFT = 0x1,
	TREE_KEY_MODIFIER_FLAGS_CONTROL = 0x2,
	TREE_KEY_MODIFIER_FLAGS_ALT = 0x4,
	TREE_KEY_MODIFIER_FLAGS_COMMAND = 0x8,
	TREE_KEY_MODIFIER_FLAGS_NUM_LOCK = 0x10,
	TREE_KEY_MODIFIER_FLAGS_SCROLL_LOCK = 0x20,
	TREE_KEY_MODIFIER_FLAGS_CAPS_LOCK = 0x40,
} TREE_KeyModifierFlags;

TREE_Char TREE_Key_ToChar(TREE_Key key, TREE_KeyModifierFlags modifierFlags);

///////////////////////////////////////
// Input                             //
///////////////////////////////////////

typedef enum _TREE_InputState
{
	TREE_INPUT_STATE_RELEASED = 0,
	TREE_INPUT_STATE_HELD = 1,
	TREE_INPUT_STATE_PRESSED = 2,

	TREE_INPUT_STATE_COOLDOWN = 10, // number of key ticks before key is in "held" state
} TREE_InputState;

typedef struct _TREE_Input
{
	TREE_Key keys[TREE_KEY_COUNT];
	TREE_Byte states[TREE_KEY_MAX];
	TREE_KeyModifierFlags modifiers;
} TREE_Input;

TREE_Result TREE_Input_Init(TREE_Input* input);

void TREE_Input_Free(TREE_Input* input);

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
	TREE_EVENT_TYPE_NONE = 0,
	TREE_EVENT_TYPE_REFRESH = 1,
	TREE_EVENT_TYPE_DRAW = 2,
	TREE_EVENT_TYPE_KEY_DOWN = 3,
	TREE_EVENT_TYPE_KEY_HELD = 4,
	TREE_EVENT_TYPE_KEY_UP = 5,
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

typedef struct _TREE_EventData_Key
{
	TREE_Key key;
	TREE_KeyModifierFlags modifiers;
} TREE_EventData_Key;

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
	TREE_CONTROL_TYPE_LABEL,
	TREE_CONTROL_TYPE_BUTTON,
	TREE_CONTROL_TYPE_TEXT_INPUT,
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
	TREE_CONTROL_STATE_FLAGS_ACTIVE = 0x4,
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
	TREE_Pixel normal;
} TREE_Control_LabelData;

TREE_Result TREE_Control_LabelData_Init(TREE_Control_LabelData* data, TREE_String text);

void TREE_Control_LabelData_Free(TREE_Control_LabelData* data);

TREE_Result TREE_Control_Label_Init(TREE_Control* control, TREE_Transform* parent, TREE_Control_LabelData* data);

TREE_Result TREE_Control_Label_SetText(TREE_Control* control, TREE_String text);

TREE_String TREE_Control_Label_GetText(TREE_Control* control);

TREE_Result TREE_Control_Label_EventHandler(TREE_Event const* event);

///////////////////////////////////////
// Control: Button                   //
///////////////////////////////////////

typedef struct _TREE_Control_ButtonData
{
	TREE_Char* text;
	TREE_Alignment alignment;
	TREE_Pixel normal;
	TREE_Pixel focused;
	TREE_Pixel active;
	TREE_Function onSubmit;
} TREE_Control_ButtonData;

TREE_Result TREE_Control_ButtonData_Init(TREE_Control_ButtonData* data, TREE_String text, TREE_Function onSubmit);

void TREE_Control_ButtonData_Free(TREE_Control_ButtonData* data);

TREE_Result TREE_Control_Button_Init(TREE_Control* control, TREE_Transform* parent, TREE_Control_ButtonData* data);

TREE_Result TREE_Control_Button_SetText(TREE_Control* control, TREE_String text, TREE_ColorPair colorPair);

TREE_String TREE_Control_Button_GetText(TREE_Control* control);

TREE_Result TREE_Control_Button_EventHandler(TREE_Event const* event);

///////////////////////////////////////
// Control: TextInput                //
///////////////////////////////////////

typedef enum _TREE_Control_TextInputType
{
	TREE_CONTROL_TEXT_INPUT_TYPE_NONE,
	TREE_CONTROL_TEXT_INPUT_TYPE_NORMAL,
	TREE_CONTROL_TEXT_INPUT_TYPE_PASSWORD,
	TREE_CONTROL_TEXT_INPUT_TYPE_PATH,
} TREE_Control_TextInputType;

typedef struct _TREE_Control_TextInputData
{
	TREE_Control_TextInputType type;
	TREE_Char* text;
	TREE_Size capacity;
	TREE_Char* placeholder;
	TREE_Pixel normal;
	TREE_Pixel focused;
	TREE_Pixel active;
	TREE_Pixel cursor;
	TREE_Size cursorPosition;
	TREE_Byte cursorTimer;
	TREE_Size scroll;
	TREE_ColorPair selection;
	TREE_Function onChange;
	TREE_Function onSubmit;
} TREE_Control_TextInputData;

TREE_Result TREE_Control_TextInputData_Init(TREE_Control_TextInputData* data, TREE_String text, TREE_Size capacity, TREE_String placeholder, TREE_Control_TextInputType type, TREE_Function onChange, TREE_Function onSubmit);

void TREE_Control_TextInputData_Free(TREE_Control_TextInputData* data);

TREE_Result TREE_Control_TextInput_Init(TREE_Control* control, TREE_Transform* parent, TREE_Control_TextInputData* data);

TREE_Result TREE_Control_TextInput_SetText(TREE_Control* control, TREE_String text);

TREE_String TREE_Control_TextInput_GetText(TREE_Control* control);

TREE_Result TREE_Control_TextInput_EventHandler(TREE_Event const* event);

///////////////////////////////////////
// Application                       //
///////////////////////////////////////

typedef struct _TREE_Application
{
	TREE_Control** controls;
	TREE_Size controlsSize;
	TREE_Size controlsCapacity;
	TREE_Control* focusedControl;

	TREE_Bool running;

	TREE_Input input;

	TREE_EventHandler eventHandler;

	TREE_Surface* surface;
} TREE_Application;

TREE_Result TREE_Application_Init(TREE_Application* application, TREE_Surface* surface, TREE_Size capacity, TREE_EventHandler eventHandler);

void TREE_Application_Free(TREE_Application* application);

TREE_Result TREE_Application_AddControl(TREE_Application* application, TREE_Control* control);

TREE_Result TREE_Application_SetFocus(TREE_Application* application, TREE_Control* control);

TREE_Result TREE_Application_DispatchEvent(TREE_Application* application, TREE_Event const* event);

TREE_Result TREE_Application_Run(TREE_Application* application);

void TREE_Application_Quit(TREE_Application* application);

///////////////////////////////////////
//                                   //
///////////////////////////////////////

#endif // __TREE_H__