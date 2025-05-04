#ifndef __TREE_H__
#define __TREE_H__

///////////////////////////////////////
// Types, Constants and Macros       //
///////////////////////////////////////

typedef int TREE_Bool;
typedef int TREE_Int;
typedef unsigned TREE_UInt;
typedef double TREE_Float;
typedef unsigned char TREE_Byte;
typedef char TREE_Char;
typedef TREE_Char const* TREE_String;
typedef unsigned long long TREE_Size;
typedef void* TREE_Data;
typedef long TREE_Long;
typedef long long TREE_Time;

#define TREE_FALSE 0
#define TREE_TRUE 1
#define TREE_COLOR_STRING_LENGTH 6

#define TREE_NEW(type) ((type*)malloc(sizeof(type)))
#define TREE_NEW_ARRAY(type, count) ((type*)malloc((count) * sizeof(type)))
#define TREE_REPLACE(ptr, newPtr) do { free(ptr); ptr = newPtr; } while (0)
#define TREE_DELETE(ptr) TREE_REPLACE(ptr, NULL)
#define TREE_DELETE_ARRAY(ptr, count) do { for(TREE_Size i = 0; i < (count); i++) { free((ptr)[i]); } TREE_DELETE(ptr); } while (0)
#define TREE_COPY(dest, src, type) memcpy(dest, src, sizeof(type))
#define TREE_COPY_ARRAY(dest, src, type, count) memcpy(dest, src, (count) * sizeof(type))

#define TREE_EXTERN extern

///////////////////////////////////////
// Result                            //
///////////////////////////////////////

/// <summary>
/// An error code. Non zero implies an error.
/// </summary>
typedef enum _TREE_Result
{
	// General results
	TREE_OK = 0,
	TREE_CANCEL = 1,
	TREE_NOT_IMPLEMENTED = 2,

	// General errors
	TREE_ERROR = 100,
	TREE_ERROR_OVERFLOW = 101,
	TREE_ERROR_FULL = 102,
	TREE_ERROR_INVALID_STATE = 103,

	// Argument errors
	TREE_ERROR_ARG_NULL = 200,
	TREE_ERROR_ARG_OUT_OF_RANGE = 201,
	TREE_ERROR_ARG_INVALID = 202,

	// Allocation errors
	TREE_ERROR_ALLOC = 300,

	// Presentation errors
	TREE_ERROR_PRESENTATION = 400,

	// File errors
	TREE_ERROR_FILE_OPEN = 500,
	TREE_ERROR_FILE_DELETE = 501,

	// Directory errors
	TREE_ERROR_DIRECTORY_CREATE = 600,
	TREE_ERROR_DIRECTORY_DELETE = 601,
	TREE_ERROR_DIRECTORY_ENUMERATE = 602,

	// Clipboard errors
	TREE_ERROR_CLIPBOARD_SET_TEXT = 700,
	TREE_ERROR_CLIPBOARD_GET_TEXT = 701,

	// Word wrapping
	TREE_ERROR_WORD_WRAPPING = 800,
	TREE_ERROR_WORD_WRAPPING_OFFSETS = 801,

	// Application
	TREE_ERROR_APPLICATION_MULTIPLE_ACTIVE_CONTROLS = 900,

	// Window
	TREE_ERROR_WINDOW_SET_TITLE = 1000,

	//		Windows specific errors

	// Global
	TREE_ERROR_WINDOWS_GLOBAL_ALLOC = 10000,
	TREE_ERROR_WINDOWS_GLOBAL_LOCK = 10001,
	TREE_ERROR_WINDOWS_GLOBAL_UNLOCK = 10002,

	// Handlers
	TREE_ERROR_WINDOWS_SET_CONTROL_HANDLER = 10100,

	// Clipboard
	TREE_ERROR_WINDOWS_CLIPBOARD_OPEN = 10200,
	TREE_ERROR_WINDOWS_CLIPBOARD_CLOSE = 10201,
	TREE_ERROR_WINDOWS_CLIPBOARD_CLEAR = 10202,

	// Console
	TREE_ERROR_WINDOWS_CONSOLE_GET_CURSOR_INFO = 10300,
	TREE_ERROR_WINDOWS_CONSOLE_SET_CURSOR_INFO = 10301,
	TREE_ERROR_WINDOWS_CONSOLE_INIT = 10302,

	//		Linux

	// Clipboard
	TREE_ERROR_LINUX_CLIPBOARD_OPEN = 20000,
	TREE_ERROR_LINUX_CLIPBOARD_CLOSE = 20001,

	// Console
	TREE_ERROR_LINUX_CONSOLE_INIT = 20100,

	// Input
	TREE_ERROR_LINUX_INPUT_INIT = 20200,

	// Keyboard
	TREE_ERROR_LINUX_KEYBOARD_NOT_FOUND = 20300,
	TREE_ERROR_LINUX_KEYBOARD_OPEN = 20301,
	TREE_ERROR_LINUX_KEYBOARD_READ = 20302,
	TREE_ERROR_LINUX_KEYBOARD_POLL = 20303,

} TREE_Result;

TREE_EXTERN TREE_String TREE_Result_ToString(TREE_Result code);

///////////////////////////////////////
// Time                              //
///////////////////////////////////////

/// <summary>
/// Gets the current time in milliseconds.
/// </summary>
/// <returns></returns>
TREE_Time TREE_Time_Now();

///////////////////////////////////////
// TREE                              //
///////////////////////////////////////

/// <summary>
/// Initializes the TREE library.
/// </summary>
TREE_EXTERN TREE_Result TREE_Init();

/// <summary>
/// Closes the TREE library.
/// </summary>
TREE_EXTERN void TREE_Free();

///////////////////////////////////////
// String                            //
///////////////////////////////////////

/// <summary>
/// Allocates a new string and copies the contents of the source string.
/// </summary>
/// <param name="dest">The destination to copy the String from.</param>
/// <param name="src">The source String.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_String_CreateCopy(TREE_Char** dest, TREE_String src);

/// <summary>
/// Allocates a new string and copies the contents of the source string, clamping the size.
/// </summary>
/// <param name="dest">The destination to copy the String from.</param>
/// <param name="src">The source String.</param>
/// <param name="maxSize">The maximum size of the destination String.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_String_CreateClampedCopy(TREE_Char** dest, TREE_String src, TREE_Size maxSize);

///////////////////////////////////////
// Clipboard                         //
///////////////////////////////////////

/// <summary>
/// Sets the system clipboard text.
/// </summary>
/// <param name="text">The text to set.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Clipboard_SetText(TREE_String text);

/// <summary>
/// Gets the system clipboard text.
/// </summary>
/// <param name="text">The text to get.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Clipboard_GetText(TREE_Char** text);

///////////////////////////////////////
// Color and ColorPair               //
///////////////////////////////////////

/// <summary>
/// A terminal color code.
/// </summary>
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

/// <summary>
/// A pair of two colors. The 4 most significant bits are the foreground color, and the 4 least significant bits are the background color.
/// </summary>
typedef TREE_Byte TREE_ColorPair;

/// <summary>
/// Packs the two given colors into a ColorPair.
/// </summary>
/// <param name="foreground">The text color.</param>
/// <param name="background">The background color.</param>
/// <returns>A ColorPair.</returns>
TREE_EXTERN TREE_ColorPair TREE_ColorPair_Create(TREE_Color foreground, TREE_Color background);

/// <summary>
/// Creates the default ColorPair. The default is fg=white, bg=black.
/// </summary>
/// <returns>The default ColorPair.</returns>
TREE_EXTERN TREE_ColorPair TREE_ColorPair_CreateDefault();

/// <summary>
/// Gets the foreground component of the given ColorPair.
/// </summary>
/// <param name="colorPair">The ColorPair.</param>
/// <returns>The foreground color.</returns>
TREE_EXTERN TREE_Color TREE_ColorPair_GetForeground(TREE_ColorPair colorPair);

/// <summary>
/// Gets the background component of the given ColorPair.
/// </summary>
/// <param name="colorPair">The ColorPair.</param>
/// <returns>The background color.</returns>
TREE_EXTERN TREE_Color TREE_ColorPair_GetBackground(TREE_ColorPair colorPair);

/// <summary>
/// Gets the ANSI escape code sequence for the given text color.
/// </summary>
/// <param name="color">The Color to get.</param>
/// <returns>A String of the escape sequence.</returns>
TREE_EXTERN TREE_String TREE_Color_GetForegroundString(TREE_Color color);

/// <summary>
/// Gets the ANSI escape code sequence for the given background color.
/// </summary>
/// <param name="color">The Color to get.</param>
/// <returns>A String of the escape sequence.</returns>
TREE_EXTERN TREE_String TREE_Color_GetBackgroundString(TREE_Color color);

/// <summary>
/// Gets the ANSI escape code sequence to reset the terminal.
/// </summary>
/// <returns>A String of the escape sequence.</returns>
TREE_EXTERN TREE_String TREE_Color_GetResetString();

///////////////////////////////////////
// Path                              //
///////////////////////////////////////

/// <summary>
/// Gets the absolute path of the given path.
/// </summary>
/// <param name="path">The path.</param>
/// <returns>A newly allocated absolute path string.</returns>
TREE_EXTERN TREE_String TREE_Path_Absolute(TREE_String path);

/// <summary>
/// Gets the base name of the given path.
/// </summary>
/// <param name="path">The path.</param>
/// <returns>A newly allocated parent path string.</returns>
TREE_EXTERN TREE_String TREE_Path_Parent(TREE_String path);

///////////////////////////////////////
// File                              //
///////////////////////////////////////

/// <summary>
/// Determines the type of file.
/// </summary>
typedef enum _TREE_FileTypeFlags
{
	/// <summary>
	/// No file types.
	/// </summary>
	TREE_FILE_TYPE_FLAGS_NONE = 0x0,

	/// <summary>
	/// File file types.
	/// </summary>
	TREE_FILE_TYPE_FLAGS_FILE = 0x1,

	/// <summary>
	/// Directory file types.
	/// </summary>
	TREE_FILE_TYPE_FLAGS_DIRECTORY = 0x2,

	/// <summary>
	/// Hidden file types. All files or directories starting with a dot (.) are considered hidden.
	/// </summary>
	TREE_FILE_TYPE_FLAGS_HIDDEN = 0x4,

	/// <summary>
	/// All file types.
	/// </summary>
	TREE_FILE_TYPE_FLAGS_ALL = 0x7,
} TREE_FileTypeFlags;

/// <summary>
/// Checks if a File exists at the given path.
/// </summary>
/// <param name="path">The path to the File.</param>
/// <returns>True if the File exists.</returns>
TREE_EXTERN TREE_Bool TREE_File_Exists(TREE_String path);

/// <summary>
/// Gets the size of the File at the given path.
/// </summary>
/// <param name="path">The path to the File.</param>
/// <returns>The size of the File in bytes.</returns>
TREE_EXTERN TREE_Size TREE_File_Size(TREE_String path);

/// <summary>
/// Reads the contents of the File at the given path into the given buffer.
/// </summary>
/// <param name="path">The path to the File.</param>
/// <param name="text">The buffer to read the text into.</param>
/// <param name="size">The size of the buffer.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_File_Read(TREE_String path, TREE_Char* text, TREE_Size size);

/// <summary>
/// Writes the given text to the File at the given path.
/// </summary>
/// <param name="path">The path to the File.</param>
/// <param name="text">The buffer to read the text from.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_File_Write(TREE_String path, TREE_String text);

/// <summary>
/// Creates a File at the given path.
/// </summary>
/// <param name="path">The path to create the File at.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_File_Create(TREE_String path);

/// <summary>
/// Deletes the File at the given path.
/// </summary>
/// <param name="path">The path to the File to destroy.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_File_Delete(TREE_String path);

///////////////////////////////////////
// Directory                         //
///////////////////////////////////////

/// <summary>
/// Checks if a Directory exists at the given path.
/// </summary>
/// <param name="path">The path to the directory.</param>
/// <returns>True if the directory exists.</returns>
TREE_EXTERN TREE_Bool TREE_Directory_Exists(TREE_String path);

/// <summary>
/// Creates a Directory at the given path.
/// </summary>
/// <param name="path">The path.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Directory_Create(TREE_String path);

/// <summary>
/// Deletes a Directory at the given path.
/// </summary>
/// <param name="path">The path.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Directory_Delete(TREE_String path);

/// <summary>
/// Collects a list of files, sub-directories, and other things within a directory.
/// </summary>
/// <param name="path">The path to the directory.</param>
/// <param name="files">The output location of the list of elements.</param>
/// <param name="count">The output number of elements.</param>
/// <param name="flags">The file types to check for and return with. If zero, defaults to FILE and DIRECTORY types.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Directory_Enumerate(TREE_String path, TREE_Char*** files, TREE_Size* count, TREE_FileTypeFlags flags);

///////////////////////////////////////
// Offset                            //
///////////////////////////////////////

/// <summary>
/// An offset in 2D space.
/// </summary>
typedef struct _TREE_Offset
{
	/// <summary>
	/// The X coordinate of the offset.
	/// </summary>
	TREE_Int x;

	/// <summary>
	/// The Y coordinate of the offset.
	/// </summary>
	TREE_Int y;
} TREE_Offset;

///////////////////////////////////////
// Coords                            //
///////////////////////////////////////

/// <summary>
/// A pivot point in 2D space.
/// </summary>
typedef struct _TREE_Pivot
{
	/// <summary>
	/// The X percentage of the pivot.
	/// </summary>
	TREE_Float x;

	/// <summary>
	/// The Y percentage of the pivot.
	/// </summary>
	TREE_Float y;
} TREE_Pivot;

///////////////////////////////////////
// Extent                            //
///////////////////////////////////////

/// <summary>
/// An extent in 2D space.
/// </summary>
typedef struct _TREE_Extent
{
	/// <summary>
	/// The width of the extent.
	/// </summary>
	TREE_Int width;

	/// <summary>
	/// The height of the extent.
	/// </summary>
	TREE_Int height;
} TREE_Extent;

///////////////////////////////////////
// Rect                              //
///////////////////////////////////////

/// <summary>
/// A rectangle in 2D space.
/// </summary>
typedef struct _TREE_Rect
{
	/// <summary>
	/// The offset of the rectangle.
	/// </summary>
	TREE_Offset offset;

	/// <summary>
	/// The extent of the rectangle.
	/// </summary>
	TREE_Extent extent;
} TREE_Rect;

/// <summary>
/// Checks if two rectangles are overlapping.
/// </summary>
/// <param name="rectA">The first rectangle.</param>
/// <param name="rectB">The second rectangle.</param>
/// <returns>True if they are overlapping.</returns>
TREE_EXTERN TREE_Bool TREE_Rect_IsOverlapping(TREE_Rect const* rectA, TREE_Rect const* rectB);

/// <summary>
/// Combines the two rectangles into a single rectangle.
/// </summary>
/// <param name="rectA">The first rectangle.</param>
/// <param name="rectB">The second rectangle.</param>
/// <returns>A new rectangle that extends to encapulate both rectangles.</returns>
TREE_EXTERN TREE_Rect TREE_Rect_Combine(TREE_Rect const* rectA, TREE_Rect const* rectB);

/// <summary>
/// Gets the intersection of the two rectangles.
/// </summary>
/// <param name="rectA">The first rectangle.</param>
/// <param name="rectB">The second rectangle.</param>
/// <returns>A rectangle of the intersecting area, if any.</returns>
TREE_EXTERN TREE_Rect TREE_Rect_GetIntersection(TREE_Rect const* rectA, TREE_Rect const* rectB);

///////////////////////////////////////
// Pixel                             //
///////////////////////////////////////

/// <summary>
/// A pixel in the terminal.
/// </summary>
typedef struct _TREE_Pixel
{
	/// <summary>
	/// The text character.
	/// </summary>
	TREE_Char character;

	/// <summary>
	/// The ColorPair.
	/// </summary>
	TREE_ColorPair colorPair;
} TREE_Pixel;

/// <summary>
/// Creates a new pixel with the given character and color.
/// </summary>
/// <param name="character">The text character.</param>
/// <param name="foreground">The foreground color.</param>
/// <param name="background">The background color.</param>
/// <returns>A pixel packed with the given values.</returns>
TREE_EXTERN TREE_Pixel TREE_Pixel_Create(TREE_Char character, TREE_Color foreground, TREE_Color background);

/// <summary>
/// Creates a new pixel with the default character and ColorPair.
/// </summary>
/// <returns>A pixel packed with the default values.</returns>
TREE_EXTERN TREE_Pixel TREE_Pixel_CreateDefault();

///////////////////////////////////////
// Pattern                           //
///////////////////////////////////////

/// <summary>
/// A pattern of pixels.
/// </summary>
typedef struct _TREE_Pattern
{
	/// <summary>
	/// The number of pixels in this pattern.
	/// </summary>
	TREE_UInt size;

	/// <summary>
	/// The pixels in this pattern.
	/// </summary>
	TREE_Pixel* pixels;
} TREE_Pattern;

/// <summary>
/// Initializes a pattern with the given size.
/// </summary>
/// <param name="pattern">The Pattern to initialize.</param>
/// <param name="size">The number of elements in the Pattern.</param>
/// <returns></returns>
TREE_EXTERN TREE_Result TREE_Pattern_Init(TREE_Pattern* pattern, TREE_UInt size);

/// <summary>
/// Initializes a pattern from a string.
/// </summary>
/// <param name="pattern">The Pattern to initialize.</param>
/// <param name="string">The String of characters to use for the Pattern.</param>
/// <param name="colorPair">The ColorPair to use for the Pattern.</param>
/// <returns></returns>
TREE_EXTERN TREE_Result TREE_Pattern_InitFromString(TREE_Pattern* pattern, TREE_String string, TREE_ColorPair colorPair);

/// <summary>
/// Sets the element at the given index in the Pattern.
/// </summary>
/// <param name="pattern">The Pattern.</param>
/// <param name="index">The index of the pixel.</param>
/// <param name="pixel">The Pixel to set.</param>
/// <returns></returns>
TREE_EXTERN TREE_Result TREE_Pattern_Set(TREE_Pattern* pattern, TREE_UInt index, TREE_Pixel pixel);

/// <summary>
/// Gets the element at the given index in the Pattern.
/// </summary>
/// <param name="pattern">The Pattern.</param>
/// <param name="index">The index of the pixel.</param>
/// <returns></returns>
TREE_EXTERN TREE_Pixel TREE_Pattern_Get(TREE_Pattern const* pattern, TREE_UInt index);

/// <summary>
/// Disposes of the given Pattern.
/// </summary>
/// <param name="pattern">The Pattern.</param>
TREE_EXTERN void TREE_Pattern_Free(TREE_Pattern* pattern);

///////////////////////////////////////
// Theme                             //
///////////////////////////////////////

/// <summary>
/// An ID for a Theme character.
/// </summary>
typedef enum _TREE_ThemeCharacterID
{
	TREE_THEME_CID_EMPTY = 0,
	TREE_THEME_CID_SCROLL_V_AREA,
	TREE_THEME_CID_SCROLL_H_AREA,
	TREE_THEME_CID_SCROLL_V_BAR,
	TREE_THEME_CID_SCROLL_H_BAR,
	TREE_THEME_CID_UP,
	TREE_THEME_CID_DOWN,
	TREE_THEME_CID_LEFT,
	TREE_THEME_CID_RIGHT,
	TREE_THEME_CID_CHECKBOX_UNCHECKED,
	TREE_THEME_CID_CHECKBOX_CHECKED,
	TREE_THEME_CID_CHECKBOX_LEFT,
	TREE_THEME_CID_CHECKBOX_RIGHT,
	TREE_THEME_CID_RADIOBOX_UNCHECKED,
	TREE_THEME_CID_RADIOBOX_CHECKED,
	TREE_THEME_CID_RADIOBOX_LEFT,
	TREE_THEME_CID_RADIOBOX_RIGHT,

	TREE_THEME_CID_COUNT
} TREE_ThemeCharacterID;

/// <summary>
/// An ID for a Theme pixel.
/// </summary>
typedef enum _TREE_ThemePixelID
{
	TREE_THEME_PID_NORMAL = 0,
	TREE_THEME_PID_FOCUSED,
	TREE_THEME_PID_ACTIVE,
	TREE_THEME_PID_HOVERED,
	TREE_THEME_PID_NORMAL_SELECTED,
	TREE_THEME_PID_FOCUSED_SELECTED,
	TREE_THEME_PID_ACTIVE_SELECTED,
	TREE_THEME_PID_HOVERED_SELECTED,
	TREE_THEME_PID_NORMAL_TEXT,
	TREE_THEME_PID_FOCUSED_TEXT,
	TREE_THEME_PID_NORMAL_SCROLL_AREA,
	TREE_THEME_PID_FOCUSED_SCROLL_AREA,
	TREE_THEME_PID_ACTIVE_SCROLL_AREA,
	TREE_THEME_PID_NORMAL_SCROLL_BAR,
	TREE_THEME_PID_FOCUSED_SCROLL_BAR,
	TREE_THEME_PID_ACTIVE_SCROLL_BAR,
	TREE_THEME_PID_CURSOR,
	TREE_THEME_PID_PROGRESS_BAR,
	TREE_THEME_PID_BACKGROUND,

	TREE_THEME_PID_COUNT
} TREE_ThemePixelID;

/// <summary>
/// A collection of characters and pixels used for the defaults of Controls.
/// </summary>
typedef struct _TREE_Theme
{
	/// <summary>
	/// A collection of characters used for this Theme.
	/// </summary>
	TREE_Char characters[TREE_THEME_CID_COUNT];

	/// <summary>
	/// A collection of pixels used for this Theme.
	/// </summary>
	TREE_Pixel pixels[TREE_THEME_PID_COUNT];
} TREE_Theme;

/// <summary>
/// Initializes the Theme with the default values.
/// </summary>
/// <param name="theme">The Theme.</param>
/// <returns></returns>
TREE_EXTERN TREE_Result TREE_Theme_Init(TREE_Theme* theme);

/// <summary>
/// Disposes of the given Theme.
/// </summary>
/// <param name="theme">The Theme.</param>
TREE_EXTERN void TREE_Theme_Free(TREE_Theme* theme);

///////////////////////////////////////
// Image                             //
///////////////////////////////////////

/// <summary>
/// An image made of characters and colors.
/// </summary>
typedef struct _TREE_Image
{
	/// <summary>
	/// The size of the image.
	/// </summary>
	TREE_Extent extent;

	/// <summary>
	/// The text characters in the image.
	/// </summary>
	TREE_Char* text;

	/// <summary>
	/// The ColorPairs in the image.
	/// </summary>
	TREE_ColorPair* colors;
} TREE_Image;

/// <summary>
/// Initializes an image with the given size.
/// </summary>
/// <param name="image">The Image.</param>
/// <param name="size">The size of the image.</param>
/// <returns></returns>
TREE_EXTERN TREE_Result TREE_Image_Init(TREE_Image* image, TREE_Extent size);

/// <summary>
/// Disposes of the given Image.
/// </summary>
/// <param name="image">The Image.</param>
TREE_EXTERN void TREE_Image_Free(TREE_Image* image);

/// <summary>
/// Sets the pixel at the given offset in the image.
/// </summary>
/// <param name="image">The Image.</param>
/// <param name="offset">The X and Y coordinate within the Image. (0, 0) is the top left.</param>
/// <param name="pixel">The Pixel value to set.</param>
/// <returns></returns>
TREE_EXTERN TREE_Result TREE_Image_Set(TREE_Image* image, TREE_Offset offset, TREE_Pixel pixel);

/// <summary>
/// Gets the pixel at the given offset in the image.
/// </summary>
/// <param name="image">The Image.</param>
/// <param name="offset">The X and Y coordinate within the Image. (0, 0) is the top left.</param>
/// <returns>The Pixel.</returns>
TREE_EXTERN TREE_Pixel TREE_Image_Get(TREE_Image* image, TREE_Offset offset);

/// <summary>
/// Resizes the given image to the given size.
/// </summary>
/// <param name="image">The Image.</param>
/// <param name="extent">The new Size.</param>
/// <returns></returns>
TREE_EXTERN TREE_Result TREE_Image_Resize(TREE_Image* image, TREE_Extent extent);

/// <summary>
/// Draws the given other Image onto the given Image.
/// </summary>
/// <param name="image">The destination Image.</param>
/// <param name="offset">The offset within the destination Image.</param>
/// <param name="other">The source Image.</param>
/// <param name="otherOffset">The offset within the source Image.</param>
/// <param name="extent">The size to draw.</param>
/// <returns></returns>
TREE_EXTERN TREE_Result TREE_Image_DrawImage(TREE_Image* image, TREE_Offset offset, TREE_Image const* other, TREE_Offset otherOffset, TREE_Extent extent);

/// <summary>
/// Draws the given string onto the given Image.
/// </summary>
/// <param name="image">The destination Image.</param>
/// <param name="offset">The offset within the destination Image.</param>
/// <param name="string">The source String.</param>
/// <param name="colorPair">The ColorPair of the String.</param>
/// <returns></returns>
TREE_EXTERN TREE_Result TREE_Image_DrawString(TREE_Image* image, TREE_Offset offset, TREE_String string, TREE_ColorPair colorPair);

/// <summary>
/// Draws a line from the start to the end offset in the given Image.
/// </summary>
/// <param name="image">The destination Image.</param>
/// <param name="start">The starting coordinates of the line.</param>
/// <param name="end">The ending coordinates of the line.</param>
/// <param name="pattern">The Pattern to draw the line with.</param>
/// <returns></returns>
TREE_EXTERN TREE_Result TREE_Image_DrawLine(TREE_Image* image, TREE_Offset start, TREE_Offset end, TREE_Pattern const* pattern);

/// <summary>
/// Draws the given Rect in the given Image.
/// </summary>
/// <param name="image">The destination Image.</param>
/// <param name="rect">The Rect to draw.</param>
/// <param name="pattern">The Pattern to draw the lines with.</param>
/// <returns></returns>
TREE_EXTERN TREE_Result TREE_Image_DrawRect(TREE_Image* image, TREE_Rect const* rect, TREE_Pattern const* pattern);

/// <summary>
/// Fills the given Rect in the given Image with the given Pixel.
/// </summary>
/// <param name="image">The destination Image.</param>
/// <param name="rect">The Rect to fill.</param>
/// <param name="pixel">The Pixel to fill the Rect with.</param>
/// <returns></returns>
TREE_EXTERN TREE_Result TREE_Image_FillRect(TREE_Image* image, TREE_Rect const* rect, TREE_Pixel pixel);

/// <summary>
/// Clears the given Image with the given Pixel.
/// </summary>
/// <param name="image">The Image.</param>
/// <param name="pixel">The Pixel to use.</param>
/// <returns></returns>
TREE_EXTERN TREE_Result TREE_Image_Clear(TREE_Image* image, TREE_Pixel pixel);

///////////////////////////////////////
// Surface                           //
///////////////////////////////////////

/// <summary>
/// The final Image that can be printed to the terminal.
/// </summary>
typedef struct _TREE_Surface
{
	/// <summary>
	/// The Image data.
	/// </summary>
	TREE_Image image;

	/// <summary>
	/// The final printable String.
	/// </summary>
	TREE_Char* text;
} TREE_Surface;

/// <summary>
/// Initializes the given Surface with the given size.
/// </summary>
/// <param name="surface">The Surface.</param>
/// <param name="size">The size of the Surface.</param>
/// <returns></returns>
TREE_EXTERN TREE_Result TREE_Surface_Init(TREE_Surface* surface, TREE_Extent size);

/// <summary>
/// Disposes of the given Surface.
/// </summary>
/// <param name="surface">The Surface.</param>
/// <returns></returns>
TREE_EXTERN void TREE_Surface_Free(TREE_Surface* surface);

/// <summary>
/// Refreshes the given Surface. After this operation, the text field will be updated.
/// </summary>
/// <param name="surface">The Surface.</param>
/// <returns></returns>
TREE_EXTERN TREE_Result TREE_Surface_Refresh(TREE_Surface* surface);

///////////////////////////////////////
// Window                            //
///////////////////////////////////////

/// <summary>
/// Sets the title of the window.
/// </summary>
/// <param name="title">The title to use.</param>
/// <returns></returns>
TREE_EXTERN TREE_Result TREE_Window_SetTitle(TREE_String title);

/// <summary>
/// Presents the given Surface to the terminal.
/// </summary>
/// <param name="surface">The Surface.</param>
/// <returns></returns>
TREE_EXTERN TREE_Result TREE_Window_Present(TREE_Surface* surface);

/// <summary>
/// Gets the current Extent of the Window.
/// </summary>
/// <returns>The Extent of the terminal.</returns>
TREE_EXTERN TREE_Extent TREE_Window_GetExtent();

/// <summary>
/// Produces a beep sound in the terminal.
/// </summary>
TREE_EXTERN void TREE_Window_Beep();

///////////////////////////////////////
// Cursor                            //
///////////////////////////////////////

/// <summary>
/// Sets the visibility of the cursor.
/// </summary>
/// <param name="visible">The visibility.</param>
/// <returns></returns>
TREE_EXTERN TREE_Result TREE_Cursor_SetVisible(TREE_Bool visible);

///////////////////////////////////////
// Key                               //
///////////////////////////////////////

/// <summary>
/// A keyboard key.
/// </summary>
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
	TREE_KEY_LEFT_COMMAND = 91, // Left Command (Windows key)
	TREE_KEY_RIGHT_COMMAND = 92, // Right Command (Windows key)
	TREE_KEY_APPLICATION = 93, // Application key (context menu key)
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
	TREE_KEY_LEFT_SHIFT = 160, // Left Shift
	TREE_KEY_RIGHT_SHIFT = 161, // Right Shift
	TREE_KEY_LEFT_CONTROL = 162, // Left Control
	TREE_KEY_RIGHT_CONTROL = 163, // Right Control
	TREE_KEY_LEFT_ALT = 164, // Left Alt
	TREE_KEY_RIGHT_ALT = 165, // Right Alt
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

/// <summary>
/// Gets the String representation of the given key.
/// </summary>
/// <param name="key">The Key.</param>
/// <returns>The name of the Key.</returns>
TREE_EXTERN TREE_String TREE_Key_ToString(TREE_Key key);

///////////////////////////////////////
// Char                              //
///////////////////////////////////////

/// <summary>
/// A character type.
/// </summary>
typedef enum _TREE_CharType
{
	/// <summary>
	/// No character type.
	/// </summary>
	TREE_CHAR_TYPE_NONE,

	/// <summary>
	/// Letters, including upper and lower case letters.
	/// </summary>
	TREE_CHAR_TYPE_LETTER,

	/// <summary>
	/// Digits.
	/// </summary>
	TREE_CHAR_TYPE_NUMBER,

	/// <summary>
	/// Whitespace characters, including spaces, tabs, and newlines.
	/// </summary>
	TREE_CHAR_TYPE_WHITESPACE,

	/// <summary>
	/// Symbols, including punctuation and special characters.
	/// </summary>
	TREE_CHAR_TYPE_SYMBOL,
} TREE_CharType;

/// <summary>
/// Gets the type of the given character.
/// </summary>
/// <param name="character">The character to check.</param>
/// <returns>The CharType.</returns>
TREE_EXTERN TREE_CharType TREE_Char_GetType(TREE_Char character);

///////////////////////////////////////
// Key Modifier Flags                //
///////////////////////////////////////

/// <summary>
/// Flags for modifier keys.
/// </summary>
typedef enum TREE_KeyModifierFlags
{
	/// <summary>
	/// No modifier keys.
	/// </summary>
	TREE_KEY_MODIFIER_FLAGS_NONE = 0x0,

	/// <summary>
	/// Shift key.
	/// </summary>
	TREE_KEY_MODIFIER_FLAGS_SHIFT = 0x1,

	/// <summary>
	/// Control key.
	/// </summary>
	TREE_KEY_MODIFIER_FLAGS_CONTROL = 0x2,

	/// <summary>
	/// Alt key.
	/// </summary>
	TREE_KEY_MODIFIER_FLAGS_ALT = 0x4,

	/// <summary>
	/// Command/Windows key.
	/// </summary>
	TREE_KEY_MODIFIER_FLAGS_COMMAND = 0x8,

	/// <summary>
	/// Number lock.
	/// </summary>
	TREE_KEY_MODIFIER_FLAGS_NUM_LOCK = 0x10,

	/// <summary>
	/// Scroll lock.
	/// </summary>
	TREE_KEY_MODIFIER_FLAGS_SCROLL_LOCK = 0x20,

	/// <summary>
	/// Caps lock.
	/// </summary>
	TREE_KEY_MODIFIER_FLAGS_CAPS_LOCK = 0x40,
} TREE_KeyModifierFlags;

/// <summary>
/// Converts the given key to a character, using the given modifier flags.
/// </summary>
/// <param name="key">The Key.</param>
/// <param name="modifierFlags">The modifier flags.</param>
/// <returns>The appropriate character.</returns>
TREE_EXTERN TREE_Char TREE_Key_ToChar(TREE_Key key, TREE_KeyModifierFlags modifierFlags);

///////////////////////////////////////
// Input                             //
///////////////////////////////////////

/// <summary>
/// The state of an input.
/// </summary>
typedef enum _TREE_InputState
{
	/// <summary>
	/// The input is not pressed.
	/// </summary>
	TREE_INPUT_STATE_RELEASED = 0,

	/// <summary>
	/// The input is being held down.
	/// </summary>
	TREE_INPUT_STATE_HELD = 1,

	/// <summary>
	/// The input is first pressed.
	/// </summary>
	TREE_INPUT_STATE_PRESSED = 2,

	/// <summary>
	/// Number of ticks before the key is in "held" state.
	/// </summary>
	TREE_INPUT_STATE_COOLDOWN = 10,
} TREE_InputState;

/// <summary>
/// The Input state data.
/// </summary>
typedef struct _TREE_Input
{
	/// <summary>
	/// The keys being monitored.
	/// </summary>
	TREE_Key keys[TREE_KEY_COUNT];

	/// <summary>
	/// The states of the keys.
	/// </summary>
	TREE_Byte states[TREE_KEY_MAX+1];

	/// <summary>
	/// The active modifier keys.
	/// </summary>
	TREE_KeyModifierFlags modifiers;
} TREE_Input;

/// <summary>
/// Initializes the given Input.
/// </summary>
/// <param name="input">The Input.</param>
/// <returns></returns>
TREE_EXTERN TREE_Result TREE_Input_Init(TREE_Input* input);

/// <summary>
/// Disposes of the given Input.
/// </summary>
/// <param name="input">The Input.</param>
TREE_EXTERN void TREE_Input_Free(TREE_Input* input);

/// <summary>
/// Updates the given Input with the current state of the keyboard.
/// </summary>
/// <param name="input">The Input.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Input_Refresh(TREE_Input* input);

///////////////////////////////////////
// Direction                         //
///////////////////////////////////////

/// <summary>
/// A cardinal direction.
///		North is up (0, -1).
/// 	East is right (1, 0).
/// 	South is down (0, 1).
/// 	West is left (-1, 0).
/// </summary>
typedef enum _TREE_Direction
{
	TREE_DIRECTION_NONE,
	TREE_DIRECTION_EAST,
	TREE_DIRECTION_NORTH,
	TREE_DIRECTION_WEST,
	TREE_DIRECTION_SOUTH,
} TREE_Direction;

/// <summary>
/// Gets the opposite direction of the given direction.
/// </summary>
/// <param name="direction">The direction.</param>
/// <returns>The opposite direction.</returns>
TREE_EXTERN TREE_Direction TREE_Direction_Opposite(TREE_Direction direction);

///////////////////////////////////////
// Axis                              //
///////////////////////////////////////

/// <summary>
/// A cardinal axis.
/// </summary>
typedef enum _TREE_Axis
{
	/// <summary>
	/// No axis.
	/// </summary>
	TREE_AXIS_NONE = 0x0,
	/// <summary>
	/// The horizontal axis.
	/// </summary>
	TREE_AXIS_HORIZONTAL = 0x1,
	/// <summary>
	/// The vertical axis.
	/// </summary>
	TREE_AXIS_VERTICAL = 0x2,
	/// <summary>
	/// All axes.
	/// </summary>
	TREE_AXIS_ALL = 0x3,
} TREE_Axis;

///////////////////////////////////////
// Alignment                         //
///////////////////////////////////////

/// <summary>
/// Alignment of an object within its container.
/// </summary>
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

/// <summary>
/// The type of Event.
/// </summary>
typedef enum _TREE_EventType
{
	/// <summary>
	/// No type.
	/// </summary>
	TREE_EVENT_TYPE_NONE = 0,

	/// <summary>
	/// Refresh Event.
	/// </summary>
	TREE_EVENT_TYPE_REFRESH,

	/// <summary>
	/// Draw Event.
	/// </summary>
	TREE_EVENT_TYPE_DRAW,

	/// <summary>
	/// Key down Event.
	/// </summary>
	TREE_EVENT_TYPE_KEY_DOWN,

	/// <summary>
	/// Key held Event.
	/// </summary>
	TREE_EVENT_TYPE_KEY_HELD,

	/// <summary>
	/// Key up Event.
	/// </summary>
	TREE_EVENT_TYPE_KEY_UP,

	/// <summary>
	/// Window resize Event.
	/// </summary>
	TREE_EVENT_TYPE_WINDOW_RESIZE,
} TREE_EventType;

typedef struct _TREE_Application TREE_Application;
typedef struct _TREE_Control TREE_Control;

/// <summary>
/// The data for an event.
/// </summary>
typedef struct _TREE_Event
{
	/// <summary>
	/// The type of event.
	/// </summary>
	TREE_EventType type;

	/// <summary>
	/// The application that the event is for.
	/// </summary>
	TREE_Application* application;

	/// <summary>
	/// The control that the event is for.
	/// </summary>
	TREE_Control* control;

	/// <summary>
	/// The data for the event.
	/// </summary>
	TREE_Data data;
} TREE_Event;

/// <summary>
/// A function that handles an event.
/// </summary>
typedef TREE_Result(*TREE_EventHandler)(TREE_Event const* event);

/// <summary>
/// Draw Event data.
/// </summary>
typedef struct _TREE_EventData_Draw
{
	/// <summary>
	/// The target Image to draw to.
	/// </summary>
	TREE_Image* target;

	/// <summary>
	/// The target Rect to draw within.
	/// </summary>
	TREE_Rect dirtyRect;
} TREE_EventData_Draw;

/// <summary>
/// Key down/held/up Event data.
/// </summary>
typedef struct _TREE_EventData_Key
{
	/// <summary>
	/// The key that changed states.
	/// </summary>
	TREE_Key key;

	/// <summary>
	/// The active modifier flags.
	/// </summary>
	TREE_KeyModifierFlags modifiers;
} TREE_EventData_Key;

/// <summary>
/// Window resize Event data.
/// </summary>
typedef struct _TREE_EventData_WindowResize
{
	/// <summary>
	/// The new size of the window.
	/// </summary>
	TREE_Extent extent;
} TREE_EventData_WindowResize;

///////////////////////////////////////
// Transform                         //
///////////////////////////////////////

typedef struct _TREE_Transform TREE_Transform;

/// <summary>
/// A transform in 2D space.
/// </summary>
typedef struct _TREE_Transform
{
	/// <summary>
	/// The local Offset of the Transform.
	/// </summary>
	TREE_Offset localOffset;

	/// <summary>
	/// The local Pivot of the Transform.
	/// </summary>
	TREE_Pivot localPivot;

	/// <summary>
	/// The local Extent of the Transform.
	/// </summary>
	TREE_Extent localExtent;

	/// <summary>
	/// The local alignment of the Transform.
	/// </summary>
	TREE_Alignment localAlignment;

	/// <summary>
	/// The parent Transform of this Transform.
	/// </summary>
	TREE_Transform* parent;

	/// <summary>
	/// The first child Transform of this Transform.
	/// </summary>
	TREE_Transform* child;

	/// <summary>
	/// The next sibling Transform of this Transform.
	/// </summary>
	TREE_Transform* sibling;

	/// <summary>
	/// If true, the Transform is dirty and needs to be refreshed.
	/// </summary>
	TREE_Bool dirty;

	/// <summary>
	/// The global Rect of this Transform.
	/// </summary>
	TREE_Rect globalRect;
} TREE_Transform;

/// <summary>
/// Initializes the given Transform with the given parameters.
/// </summary>
/// <param name="transform">The Transform.</param>
/// <param name="localOffset">The local Offset.</param>
/// <param name="localPivot">The local Pivot.</param>
/// <param name="localExtent">The local Extent.</param>
/// <param name="localAlignment">The local Alignment.</param>
/// <returns></returns>
TREE_EXTERN TREE_Result TREE_Transform_Init(TREE_Transform* transform, TREE_Offset localOffset, TREE_Pivot localPivot, TREE_Extent localExtent, TREE_Alignment localAlignment);

/// <summary>
/// Disposes of the given Transform.
/// </summary>
/// <param name="transform">The Transform.</param>
TREE_EXTERN void TREE_Transform_Free(TREE_Transform* transform);

/// <summary>
/// Marks the given Transform and its children as dirty.
/// </summary>
/// <param name="transform">The Transform.</param>
/// <returns></returns>
TREE_EXTERN TREE_Result TREE_Transform_Dirty(TREE_Transform* transform);

/// <summary>
/// Sets the parent Transform of the given Transform.
/// </summary>
/// <param name="transform">The child Transform.</param>
/// <param name="parent">The parent Transform.</param>
/// <returns></returns>
TREE_EXTERN TREE_Result TREE_Transform_SetParent(TREE_Transform* transform, TREE_Transform* parent);

/// <summary>
/// Removes all children from the given Transform.
/// </summary>
/// <param name="transform">The Transform.</param>
/// <returns></returns>
TREE_EXTERN TREE_Result TREE_Transform_DisconnectChildren(TREE_Transform* transform);

// calculate the global rectangle based on the local transform and parent

/// <summary>
/// Recalculates the global Rect of the given Transform.
/// </summary>
/// <param name="transform">The Transform.</param>
/// <param name="surfaceExtent">The extent of the Window.</param>
/// <returns></returns>
TREE_EXTERN TREE_Result TREE_Transform_Refresh(TREE_Transform* transform, TREE_Extent windowExtent);

///////////////////////////////////////
// Control                           //
///////////////////////////////////////

/// <summary>
/// The type of Control.
/// </summary>
typedef enum _TREE_ControlType
{
	/// <summary>
	/// No type.
	/// </summary>
	TREE_CONTROL_TYPE_NONE,

	/// <summary>
	/// Label control.
	/// </summary>
	TREE_CONTROL_TYPE_LABEL,

    /// <summary>
    ///	Button control.
    /// </summary>
    TREE_CONTROL_TYPE_BUTTON,

    /// <summary>
    ///	Text input control.
    /// </summary>
    TREE_CONTROL_TYPE_TEXT_INPUT,

    /// <summary>
    ///	Dropdown control.
    /// </summary>
    TREE_CONTROL_TYPE_DROPDOWN,

    /// <summary>
    ///	List control.
    /// </summary>
    TREE_CONTROL_TYPE_LIST,

    /// <summary>
    ///	Checkbox control.
    /// </summary>
    TREE_CONTROL_TYPE_CHECKBOX,

    /// <summary>
    ///	Number input control.
    /// </summary>
    TREE_CONTROL_TYPE_NUMBER_INPUT,

    /// <summary>
    ///	Progress bar control.
    /// </summary>
    TREE_CONTROL_TYPE_PROGRESS_BAR,
} TREE_ControlType;

/// <summary>
/// Control flags. Determines how a Control behaves.
/// </summary>
typedef enum _TREE_ControlFlags
{
	/// <summary>
	/// No flags.
	/// </summary>
	TREE_CONTROL_FLAGS_NONE = 0x0,

	/// <summary>
	/// The Control is able to be focused.
	/// </summary>
	TREE_CONTROL_FLAGS_FOCUSABLE = 0x1,
} TREE_ControlFlag;

/// <summary>
/// Control state flags. Defines the active state of a Control.
/// </summary>
typedef enum _TREE_ControlStateFlags
{
	/// <summary>
	/// No state flags.
	/// </summary>
	TREE_CONTROL_STATE_FLAGS_NONE = 0x0,

	/// <summary>
	/// The Control is dirty and needs to be redrawn.
	/// </summary>
	TREE_CONTROL_STATE_FLAGS_DIRTY = 0x1,

	/// <summary>
	/// The Control is focused.
	/// </summary>
	TREE_CONTROL_STATE_FLAGS_FOCUSED = 0x2,

	/// <summary>
	/// The Control is active, and is taking input.
	/// </summary>
	TREE_CONTROL_STATE_FLAGS_ACTIVE = 0x4,
} TREE_ControlStateFlags;

/// <summary>
/// Defines the link type between two Controls.
/// </summary>
typedef enum _TREE_ControlLink
{
	/// <summary>
	/// No link.
	/// </summary>
	TREE_CONTROL_LINK_NONE,

	/// <summary>
	/// Single direction link from one Control to another.
	/// </summary>
	TREE_CONTROL_LINK_SINGLE,

	/// <summary>
	/// Two way link between two Controls.
	/// </summary>
	TREE_CONTROL_LINK_DOUBLE
} TREE_ControlLink;

typedef struct _TREE_Control
{
	/// <summary>
	/// The type of Control.
	/// </summary>
	TREE_ControlType type;

	/// <summary>
	/// The Control flags.
	/// </summary>
	TREE_ControlFlag flags;

	/// <summary>
	/// The Control state flags.
	/// </summary>
	TREE_ControlStateFlags stateFlags;

	/// <summary>
	/// The Transform.
	/// </summary>
	TREE_Transform* transform;

	/// <summary>
	/// The Image.
	/// </summary>
	TREE_Image* image;

	/// <summary>
	/// The adjacent Controls that can be navigated to.
	/// </summary>
	TREE_Control* adjacent[4];

	/// <summary>
	/// The event handler for this Control.
	/// </summary>
	TREE_EventHandler eventHandler;

	/// <summary>
	/// The data for this Control.
	/// </summary>
	TREE_Data data;
} TREE_Control;

/// <summary>
/// A function that handles a Control event, such as OnSubmit, OnChange, etc. The first parameter is the sender, and the second parameter is the value that changed, otherwise NULL if not applicable.
/// </summary>
typedef void(*TREE_ControlEventHandler)(void*, void const*); // sender, value

/// <summary>
/// Initializes the given Control.
/// </summary>
/// <param name="control">The Control.</param>
/// <param name="parent">The parent Control.</param>
/// <param name="eventHandler">The EventHandler.</param>
/// <param name="data">The data.</param>
/// <returns></returns>
TREE_EXTERN TREE_Result TREE_Control_Init(TREE_Control* control, TREE_Transform* parent, TREE_EventHandler eventHandler, TREE_Data data);

/// <summary>
/// Disposes of the given Control.
/// </summary>
/// <param name="control">The Control.</param>
TREE_EXTERN void TREE_Control_Free(TREE_Control* control);

/// <summary>
/// Links two Controls together for navigation. Required in order to navigate from one Control to another with the keyboard.
/// </summary>
/// <param name="control">The source Control.</param>
/// <param name="direction">The direction from the source Control.</param>
/// <param name="link">The type of link.</param>
/// <param name="other">The destination Control.</param>
/// <returns></returns>
TREE_EXTERN TREE_Result TREE_Control_Link(TREE_Control* control, TREE_Direction direction, TREE_ControlLink link, TREE_Control* other);

/// <summary>
/// Handles the given event for the given Control.
/// </summary>
/// <param name="control">The Control.</param>
/// <param name="event">The Event.</param>
/// <returns></returns>
TREE_EXTERN TREE_Result TREE_Control_HandleEvent(TREE_Control* control, TREE_Event const* event);

///////////////////////////////////////
// Control: Label                    //
///////////////////////////////////////

/// <summary>
/// The data for a Label Control.
/// </summary>
typedef struct _TREE_Control_LabelData
{
	/// <summary>
	/// The text to display.
	/// </summary>
	TREE_Char* text;

	/// <summary>
	/// The alignment of the text.
	/// </summary>
	TREE_Alignment alignment;

	/// <summary>
	/// A reference to the Theme to use for the appearance.
	/// </summary>
	TREE_Theme const* theme;
} TREE_Control_LabelData;

/// <summary>
/// Initializes the given Label data.
/// </summary>
/// <param name="data">The Label data.</param>
/// <param name="text">The text.</param>
/// <param name="theme">The Theme.</param>
/// <returns></returns>
TREE_EXTERN TREE_Result TREE_Control_LabelData_Init(TREE_Control_LabelData* data, TREE_String text, TREE_Theme const* theme);

/// <summary>
/// Disposes of the given Label data.
/// </summary>
/// <param name="data">The Label data.</param>
TREE_EXTERN void TREE_Control_LabelData_Free(TREE_Control_LabelData* data);

/// <summary>
/// Initializes the given Label Control with the specified data.
/// </summary>
/// <param name="control">The Label Control to initialize.</param>
/// <param name="parent">The parent Transform of the Label Control.</param>
/// <param name="data">The data for the Label Control.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Control_Label_Init(TREE_Control* control, TREE_Transform* parent, TREE_Control_LabelData* data);

/// <summary>
/// Sets the text of the given Label Control.
/// </summary>
/// <param name="control">The Label Control.</param>
/// <param name="text">The text to set.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Control_Label_SetText(TREE_Control* control, TREE_String text);

/// <summary>
/// Gets the text of the given Label Control.
/// </summary>
/// <param name="control">The Label Control.</param>
/// <returns>The text of the Label Control.</returns>
TREE_EXTERN TREE_String TREE_Control_Label_GetText(TREE_Control* control);

/// <summary>
/// Sets the alignment of the text in the given Label Control.
/// </summary>
/// <param name="control">The Label Control.</param>
/// <param name="alignment">The alignment to set.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Control_Label_SetAlignment(TREE_Control* control, TREE_Alignment alignment);

/// <summary>
/// Gets the alignment of the text in the given Label Control.
/// </summary>
/// <param name="control">The Label Control.</param>
/// <returns>The alignment of the text.</returns>
TREE_EXTERN TREE_Alignment TREE_Control_Label_GetAlignment(TREE_Control* control);

/// <summary>
/// Handles events for the given Label Control.
/// </summary>
/// <param name="event">The event to handle.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Control_Label_EventHandler(TREE_Event const* event);

///////////////////////////////////////
// Control: Button                   //
///////////////////////////////////////

/// <summary>
/// The data for a Button Control.
/// </summary>
typedef struct _TREE_Control_ButtonData
{
	/// <summary>
	/// The text to display on the button.
	/// </summary>
	TREE_Char* text;

	/// <summary>
	/// The alignment of the text on the button.
	/// </summary>
	TREE_Alignment alignment;

	/// <summary>
	/// A reference to the Theme to use for the appearance.
	/// </summary>
	TREE_Theme const* theme;

	/// <summary>
	/// Called when a button is pressed and released.
	/// </summary>
	TREE_ControlEventHandler onSubmit;
} TREE_Control_ButtonData;

/// <summary>
/// Initializes the given Button data.
/// </summary>
/// <param name="data">The Button data to initialize.</param>
/// <param name="text">The text to display on the button.</param>
/// <param name="onSubmit">The event handler to call when the button is submitted.</param>
/// <param name="theme">The Theme to use for the button's appearance.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Control_ButtonData_Init(TREE_Control_ButtonData* data, TREE_String text, TREE_ControlEventHandler onSubmit, TREE_Theme const* theme);

/// <summary>
/// Disposes of the given Button data.
/// </summary>
/// <param name="data">The Button data to dispose of.</param>
TREE_EXTERN void TREE_Control_ButtonData_Free(TREE_Control_ButtonData* data);

/// <summary>
/// Initializes the given Button Control with the specified data.
/// </summary>
/// <param name="control">The Button Control to initialize.</param>
/// <param name="parent">The parent Transform of the Button Control.</param>
/// <param name="data">The data for the Button Control.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Control_Button_Init(TREE_Control* control, TREE_Transform* parent, TREE_Control_ButtonData* data);

/// <summary>
/// Sets the text of the given Button Control.
/// </summary>
/// <param name="control">The Button Control.</param>
/// <param name="text">The text to set.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Control_Button_SetText(TREE_Control* control, TREE_String text);

/// <summary>
/// Gets the text of the given Button Control.
/// </summary>
/// <param name="control">The Button Control.</param>
/// <returns>The text of the Button Control.</returns>
TREE_EXTERN TREE_String TREE_Control_Button_GetText(TREE_Control* control);

/// <summary>
/// Sets the alignment of the text in the given Button Control.
/// </summary>
/// <param name="control">The Button Control.</param>
/// <param name="alignment">The alignment to set.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Control_Button_SetAlignment(TREE_Control* control, TREE_Alignment alignment);

/// <summary>
/// Gets the alignment of the text in the given Button Control.
/// </summary>
/// <param name="control">The Button Control.</param>
/// <returns>The alignment of the text.</returns>
TREE_EXTERN TREE_Alignment TREE_Control_Button_GetAlignment(TREE_Control* control);

/// <summary>
/// Sets the event handler for the given Button Control when it is submitted.
/// </summary>
/// <param name="control">The Button Control.</param>
/// <param name="onSubmit">The event handler to set.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Control_Button_SetOnSubmit(TREE_Control* control, TREE_ControlEventHandler onSubmit);

/// <summary>
/// Gets the event handler for the given Button Control when it is submitted.
/// </summary>
/// <param name="control">The Button Control.</param>
/// <returns>The event handler for the Button Control.</returns>
TREE_EXTERN TREE_ControlEventHandler TREE_Control_Button_GetOnSubmit(TREE_Control* control);

/// <summary>
/// Handles events for the given Button Control.
/// </summary>
/// <param name="event">The event to handle.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Control_Button_EventHandler(TREE_Event const* event);

///////////////////////////////////////
// Control: TextInput                //
///////////////////////////////////////

/// <summary>
/// The type of TextInput.
/// </summary>
typedef enum _TREE_Control_TextInputType
{
	/// <summary>
	/// No type. TextInput is unable to be modified.
	/// </summary>
	TREE_CONTROL_TEXT_INPUT_TYPE_NONE,

	/// <summary>
	/// Normal text input.
	/// </summary>
	TREE_CONTROL_TEXT_INPUT_TYPE_NORMAL,

	/// <summary>
	/// Password text input. Characters are hidden, using asterisks.
	/// </summary>
	TREE_CONTROL_TEXT_INPUT_TYPE_PASSWORD,
} TREE_Control_TextInputType;

/// <summary>
/// The data for a TextInput Control.
/// </summary>
typedef struct _TREE_Control_TextInputData
{
	/// <summary>
	/// The type of TextInput.
	/// </summary>
	TREE_Control_TextInputType type;

	/// <summary>
	/// The text to display.
	/// </summary>
	TREE_Char* text;

	/// <summary>
	/// The maximum capacity of the TextInput.
	/// </summary>
	TREE_Size capacity;

	/// <summary>
	/// The placeholder text to display when the TextInput is empty, and not active.
	/// </summary>
	TREE_Char* placeholder;

	/// <summary>
	/// If true, the characters are overwritten when typing.
	/// </summary>
	TREE_Byte inserting;

	/// <summary>
	/// The location of the cursor within the text.
	/// </summary>
	TREE_Size cursorPosition;

	/// <summary>
	/// The offset of the cursor within the text.
	/// </summary>
	TREE_Offset cursorOffset;

	/// <summary>
	/// The timer for the cursor blink.
	/// </summary>
	TREE_Byte cursorTimer;

	/// <summary>
	/// The scroll offset of the TextInput.
	/// </summary>
	TREE_Size scroll;

	/// <summary>
	/// The selection origin.
	/// </summary>
	TREE_Size selectionOrigin;

	/// <summary>
	/// The selection starting index.
	/// </summary>
	TREE_Size selectionStart;

	/// <summary>
	/// The selection ending index.
	/// </summary>
	TREE_Size selectionEnd;

	/// <summary>
	/// A reference to the Theme to use for the appearance.
	/// </summary>
	TREE_Theme const* theme;

	/// <summary>
	/// Called when the text changes.
	/// </summary>
	TREE_ControlEventHandler onChange;

	/// <summary>
	/// Called when the TextInput is submitted.
	/// </summary>
	TREE_ControlEventHandler onSubmit;
} TREE_Control_TextInputData;

/// <summary>
/// Initializes the given TextInput data.
/// </summary>
/// <param name="data">The TextInput data to initialize.</param>
/// <param name="text">The initial text to display in the TextInput.</param>
/// <param name="capacity">The maximum capacity of the TextInput.</param>
/// <param name="placeholder">The placeholder text to display when the TextInput is empty and not active.</param>
/// <param name="type">The type of the TextInput (e.g., normal or password).</param>
/// <param name="onChange">The event handler to call when the text changes.</param>
/// <param name="onSubmit">The event handler to call when the TextInput is submitted.</param>
/// <param name="theme">The Theme to use for the TextInput's appearance.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Control_TextInputData_Init(TREE_Control_TextInputData* data, TREE_String text, TREE_Size capacity, TREE_String placeholder, TREE_Control_TextInputType type, TREE_ControlEventHandler onChange, TREE_ControlEventHandler onSubmit, TREE_Theme const* theme);

/// <summary>
/// Disposes of the given TextInput data.
/// </summary>
/// <param name="data">The TextInput data to dispose of.</param>
TREE_EXTERN void TREE_Control_TextInputData_Free(TREE_Control_TextInputData* data);

/// <summary>
/// Gets the currently selected text in the TextInput.
/// </summary>
/// <param name="data">The TextInput data.</param>
/// <returns>The selected text, or NULL if no text is selected.</returns>
TREE_Char* TREE_Control_TextInputData_GetSelectedText(TREE_Control_TextInputData* data);

/// <summary>
/// Removes the currently selected text in the TextInput.
/// </summary>
/// <param name="data">The TextInput data.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Control_TextInputData_RemoveSelectedText(TREE_Control_TextInputData* data);

/// <summary>
/// Inserts the given text into the TextInput at the current cursor position.
/// </summary>
/// <param name="data">The TextInput data.</param>
/// <param name="text">The text to insert.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Control_TextInputData_InsertText(TREE_Control_TextInputData* data, TREE_String text);

/// <summary>
/// Initializes the given TextInput Control with the specified data.
/// </summary>
/// <param name="control">The TextInput Control to initialize.</param>
/// <param name="parent">The parent Transform of the TextInput Control.</param>
/// <param name="data">The data for the TextInput Control.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Control_TextInput_Init(TREE_Control* control, TREE_Transform* parent, TREE_Control_TextInputData* data);

/// <summary>
/// Sets the type of the given TextInput Control.
/// </summary>
/// <param name="control">The TextInput Control.</param>
/// <param name="type">The type to set (e.g., normal or password).</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Control_TextInput_SetType(TREE_Control* control, TREE_Control_TextInputType type);

/// <summary>
/// Gets the type of the given TextInput Control.
/// </summary>
/// <param name="control">The TextInput Control.</param>
/// <returns>The type of the TextInput Control.</returns>
TREE_EXTERN TREE_Control_TextInputType TREE_Control_TextInput_GetType(TREE_Control* control);

/// <summary>
/// Sets the text of the given TextInput Control.
/// </summary>
/// <param name="control">The TextInput Control.</param>
/// <param name="text">The text to set.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Control_TextInput_SetText(TREE_Control* control, TREE_String text);

/// <summary>
/// Gets the text of the given TextInput Control.
/// </summary>
/// <param name="control">The TextInput Control.</param>
/// <returns>The text of the TextInput Control.</returns>
TREE_EXTERN TREE_String TREE_Control_TextInput_GetText(TREE_Control* control);

/// <summary>
/// Sets the capacity of the given TextInput Control.
/// </summary>
/// <param name="control">The TextInput Control.</param>
/// <param name="capacity">The maximum capacity to set.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Control_TextInput_SetCapacity(TREE_Control* control, TREE_Size capacity);

/// <summary>
/// Gets the capacity of the given TextInput Control.
/// </summary>
/// <param name="control">The TextInput Control.</param>
/// <returns>The maximum capacity of the TextInput Control.</returns>
TREE_EXTERN TREE_Size TREE_Control_TextInput_GetCapacity(TREE_Control* control);

/// <summary>
/// Sets the placeholder text of the given TextInput Control.
/// </summary>
/// <param name="control">The TextInput Control.</param>
/// <param name="placeholder">The placeholder text to set.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Control_TextInput_SetPlaceholder(TREE_Control* control, TREE_String placeholder);

/// <summary>
/// Gets the placeholder text of the given TextInput Control.
/// </summary>
/// <param name="control">The TextInput Control.</param>
/// <returns>The placeholder text of the TextInput Control.</returns>
TREE_EXTERN TREE_String TREE_Control_TextInput_GetPlaceholder(TREE_Control* control);

/// <summary>
/// Sets the event handler for the given TextInput Control when the text changes.
/// </summary>
/// <param name="control">The TextInput Control.</param>
/// <param name="onChange">The event handler to set.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Control_TextInput_SetOnChange(TREE_Control* control, TREE_ControlEventHandler onChange);

/// <summary>
/// Gets the event handler for the given TextInput Control when the text changes.
/// </summary>
/// <param name="control">The TextInput Control.</param>
/// <returns>The event handler for the TextInput Control.</returns>
TREE_EXTERN TREE_ControlEventHandler TREE_Control_TextInput_GetOnChange(TREE_Control* control);

/// <summary>
/// Sets the event handler for the given TextInput Control when it is submitted.
/// </summary>
/// <param name="control">The TextInput Control.</param>
/// <param name="onSubmit">The event handler to set.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Control_TextInput_SetOnSubmit(TREE_Control* control, TREE_ControlEventHandler onSubmit);

/// <summary>
/// Gets the event handler for the given TextInput Control when it is submitted.
/// </summary>
/// <param name="control">The TextInput Control.</param>
/// <returns>The event handler for the TextInput Control.</returns>
TREE_EXTERN TREE_ControlEventHandler TREE_Control_TextInput_GetOnSubmit(TREE_Control* control);

/// <summary>
/// Handles events for the given TextInput Control.
/// </summary>
/// <param name="event">The event to handle.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Control_TextInput_EventHandler(TREE_Event const* event);

///////////////////////////////////////
// Control: Scrollbar                //
///////////////////////////////////////

/// <summary>
/// The type of Scrollbar.
/// </summary>
typedef enum _TREE_Control_ScrollbarType
{
	/// <summary>
	/// Never show the scrollbar.
	/// </summary>
	TREE_CONTROL_SCROLLBAR_TYPE_NONE,
	/// <summary>
	/// Always show the scrollbar.
	/// </summary>
	TREE_CONTROL_SCROLLBAR_TYPE_STATIC,
	/// <summary>
	/// Only show the scrollbar when needed.
	/// </summary>
	TREE_CONTROL_SCROLLBAR_TYPE_DYNAMIC,
} TREE_Control_ScrollbarType;

/// <summary>
/// The data for a Scrollbar Control.
/// </summary>
typedef struct _TREE_Control_ScrollbarData
{
	/// <summary>
	/// The type of Scrollbar.
	/// </summary>
	TREE_Control_ScrollbarType type;

	/// <summary>
	/// The orientation of the Scrollbar.
	/// </summary>
	TREE_Axis axis;

	/// <summary>
	/// A reference to the Theme to use for the appearance.
	/// </summary>
	TREE_Theme const* theme;
} TREE_Control_ScrollbarData;

/// <summary>
/// Initializes the given Scrollbar data with the specified type and orientation.
/// </summary>
/// <param name="data">The Scrollbar data to initialize.</param>
/// <param name="type">The type of the Scrollbar (e.g., static, dynamic).</param>
/// <param name="axis">The axis of the Scrollbar. Expects VERTICAL or HORIZONTAL only.</param>
/// <param name="theme">The Theme to use for the Scrollbar's appearance.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Control_ScrollbarData_Init(TREE_Control_ScrollbarData* data, TREE_Control_ScrollbarType type, TREE_Axis axis, TREE_Theme const* theme);

/// <summary>
/// Disposes of the given Scrollbar data.
/// </summary>
/// <param name="data">The Scrollbar data to dispose of.</param>
TREE_EXTERN void TREE_Control_ScrollbarData_Free(TREE_Control_ScrollbarData* data);

///////////////////////////////////////
// Control: List                     //
///////////////////////////////////////

/// <summary>
/// The flags for a List Control.
/// </summary>
typedef enum _TREE_Control_ListFlags
{
	/// <summary>
	/// No flags.
	/// </summary>
	TREE_CONTROL_LIST_FLAGS_NONE = 0x0,

	/// <summary>
	/// Allows for multiple items to be selected at once.
	/// </summary>
	TREE_CONTROL_LIST_FLAGS_MULTISELECT = 0x1
} TREE_Control_ListFlags;

/// <summary>
/// The data for a List Control.
/// </summary>
typedef struct _TREE_Control_ListData
{
	/// <summary>
	/// The flags for the List Control.
	/// </summary>
	TREE_Control_ListFlags flags;

	/// <summary>
	/// The options to display in the List Control.
	/// </summary>
	TREE_Char** options;

	/// <summary>
	/// The number of options in the List Control.
	/// </summary>
	TREE_Size optionsSize;

	/// <summary>
	/// The index of the currently selected item.
	/// </summary>
	TREE_Size selectedIndex;

	/// <summary>
	/// The state of each item in the List Control, if it is selected or not. NULL if no multiselect flag.
	/// </summary>
	TREE_Byte* selectedIndices;

	/// <summary>
	/// The index of the currently hovered item.
	/// </summary>
	TREE_Size hoverIndex;

	/// <summary>
	/// The scroll offset of the List Control.
	/// </summary>
	TREE_Size scroll;

	/// <summary>
	/// The Scrollbar data for the List Control.
	/// </summary>
	TREE_Control_ScrollbarData scrollbar;

	/// <summary>
	/// A reference to the Theme to use for the appearance.
	/// </summary>
	TREE_Theme const* theme;

	/// <summary>
	/// Called when the selection changes.
	/// </summary>
	TREE_ControlEventHandler onChange;

	/// <summary>
	/// Called when the List Control is submitted.
	/// </summary>
	TREE_ControlEventHandler onSubmit;
} TREE_Control_ListData;

/// <summary>
/// Initializes the given List data with the specified options and flags.
/// </summary>
/// <param name="data">The List data to initialize.</param>
/// <param name="flags">The flags for the List Control (e.g., multiselect).</param>
/// <param name="options">The options to display in the List Control.</param>
/// <param name="optionsSize">The number of options in the List Control.</param>
/// <param name="onChange">The event handler to call when the selection changes.</param>
/// <param name="onSubmit">The event handler to call when the List Control is submitted.</param>
/// <param name="theme">The Theme to use for the List Control's appearance.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Control_ListData_Init(TREE_Control_ListData* data, TREE_Control_ListFlags flags, TREE_String* options, TREE_Size optionsSize, TREE_ControlEventHandler onChange, TREE_ControlEventHandler onSubmit, TREE_Theme const* theme);

/// <summary>
/// Disposes of the given List data.
/// </summary>
/// <param name="data">The List data to dispose of.</param>
TREE_EXTERN void TREE_Control_ListData_Free(TREE_Control_ListData* data);

/// <summary>
/// Sets the options for the given List data.
/// </summary>
/// <param name="data">The List data.</param>
/// <param name="options">The new options to display in the List Control.</param>
/// <param name="optionsSize">The number of new options.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Control_ListData_SetOptions(TREE_Control_ListData* data, TREE_String* options, TREE_Size optionsSize);

/// <summary>
/// Sets the selection state of the specified index in the List data.
/// </summary>
/// <param name="data">The List data.</param>
/// <param name="index">The index of the item to modify.</param>
/// <param name="selected">True to select the item, false to deselect it.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Control_ListData_SetSelected(TREE_Control_ListData* data, TREE_Size index, TREE_Bool selected);

/// <summary>
/// Gets the indices of the selected items in the List data.
/// </summary>
/// <param name="data">The List data.</param>
/// <param name="indices">The output array of selected indices.</param>
/// <param name="indexCount">The output number of selected indices.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Control_ListData_GetSelected(TREE_Control_ListData* data, TREE_Size** indices, TREE_Size* indexCount);

/// <summary>
/// Checks if the specified index in the List data is selected.
/// </summary>
/// <param name="data">The List data.</param>
/// <param name="index">The index to check.</param>
/// <returns>True if the item is selected, false otherwise.</returns>
TREE_EXTERN TREE_Bool TREE_Control_ListData_IsSelected(TREE_Control_ListData* data, TREE_Size index);

/// <summary>
/// Initializes the given List Control with the specified data.
/// </summary>
/// <param name="control">The List Control to initialize.</param>
/// <param name="parent">The parent Transform of the List Control.</param>
/// <param name="data">The data for the List Control.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Control_List_Init(TREE_Control* control, TREE_Transform* parent, TREE_Control_ListData* data);

/// <summary>
/// Sets the flags for the given List Control.
/// </summary>
/// <param name="control">The List Control.</param>
/// <param name="flags">The flags to set (e.g., multiselect).</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Control_List_SetFlags(TREE_Control* control, TREE_Control_ListFlags flags);

/// <summary>
/// Gets the flags of the given List Control.
/// </summary>
/// <param name="control">The List Control.</param>
/// <returns>The flags of the List Control.</returns>
TREE_EXTERN TREE_Control_ListFlags TREE_Control_List_GetFlags(TREE_Control* control);

/// <summary>
/// Sets the options for the given List Control.
/// </summary>
/// <param name="control">The List Control.</param>
/// <param name="options">The new options to display in the List Control.</param>
/// <param name="optionsSize">The number of new options.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Control_List_SetOptions(TREE_Control* control, TREE_String* options, TREE_Size optionsSize);

/// <summary>
/// Gets the options of the given List Control.
/// </summary>
/// <param name="control">The List Control.</param>
/// <returns>The options of the List Control.</returns>
TREE_EXTERN TREE_String* TREE_Control_List_GetOptions(TREE_Control* control);

/// <summary>
/// Gets the number of options in the given List Control.
/// </summary>
/// <param name="control">The List Control.</param>
/// <returns>The number of options in the List Control.</returns>
TREE_EXTERN TREE_Size TREE_Control_List_GetOptionsSize(TREE_Control* control);

/// <summary>
/// Sets the selection state of the specified index in the List Control.
/// </summary>
/// <param name="control">The List Control.</param>
/// <param name="index">The index of the item to modify.</param>
/// <param name="selected">True to select the item, false to deselect it.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Control_List_SetSelected(TREE_Control* control, TREE_Size index, TREE_Bool selected);

/// <summary>
/// Checks if the specified index in the List Control is selected.
/// </summary>
/// <param name="control">The List Control.</param>
/// <param name="index">The index to check.</param>
/// <returns>True if the item is selected, false otherwise.</returns>
TREE_EXTERN TREE_Bool TREE_Control_List_IsSelected(TREE_Control* control, TREE_Size index);

/// <summary>
/// Gets the index of the selected item in the List Control.
/// </summary>
/// <param name="control">The List Control.</param>
/// <returns>The index of the selected item, or -1 if no item is selected.</returns>
TREE_EXTERN TREE_Size TREE_Control_List_GetSelected(TREE_Control* control);

/// <summary>
/// Sets the event handler for the given List Control when the selection changes.
/// </summary>
/// <param name="control">The List Control.</param>
/// <param name="onChange">The event handler to set.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Control_List_SetOnChange(TREE_Control* control, TREE_ControlEventHandler onChange);

/// <summary>
/// Gets the event handler for the given List Control when the selection changes.
/// </summary>
/// <param name="control">The List Control.</param>
/// <returns>The event handler for the List Control.</returns>
TREE_EXTERN TREE_ControlEventHandler TREE_Control_List_GetOnChange(TREE_Control* control);

/// <summary>
/// Sets the event handler for the given List Control when it is submitted.
/// </summary>
/// <param name="control">The List Control.</param>
/// <param name="onSubmit">The event handler to set.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Control_List_SetOnSubmit(TREE_Control* control, TREE_ControlEventHandler onSubmit);

/// <summary>
/// Gets the event handler for the given List Control when it is submitted.
/// </summary>
/// <param name="control">The List Control.</param>
/// <returns>The event handler for the List Control.</returns>
TREE_EXTERN TREE_ControlEventHandler TREE_Control_List_GetOnSubmit(TREE_Control* control);

/// <summary>
/// Handles events for the given List Control.
/// </summary>
/// <param name="event">The event to handle.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Control_List_EventHandler(TREE_Event const* event);

///////////////////////////////////////
// Control: Dropdown                 //
///////////////////////////////////////

/// <summary>
/// The data for a Dropdown Control.
/// </summary>
typedef struct _TREE_Control_DropdownData
{
	/// <summary>
	/// The options to display in the Dropdown Control.
	/// </summary>
	TREE_Char** options;

	/// <summary>
	/// The number of options in the Dropdown Control.
	/// </summary>
	TREE_Size optionsSize;
	
	/// <summary>
	/// The index of the currently selected item.
	/// </summary>
	TREE_Size selectedIndex;

	/// <summary>
	/// The index of the currently hovered item.
	/// </summary>
	TREE_Size hoverIndex;

	/// <summary>
	/// The scroll offset of the Dropdown Control. Used when the Dropdown is active.
	/// </summary>
	TREE_Size scroll;

	/// <summary>
	/// The origin offset of the Dropdown Control. Used when the Dropdown is active.
	/// </summary>
	TREE_Offset origin;

	/// <summary>
	/// The direction and size of the Dropdown Control. Used when the Dropdown is active.
	/// Negative for up, positive for down.
	/// </summary>
	TREE_Int drop;

	/// <summary>
	/// A reference to the Theme to use for the appearance.
	/// </summary>
	TREE_Theme const* theme;

	/// <summary>
	/// Called when the Dropdown Control is submitted.
	/// </summary>
	TREE_ControlEventHandler onSubmit;
} TREE_Control_DropdownData;

/// <summary>
/// Initializes the given Dropdown data with the specified options and selected index.
/// </summary>
/// <param name="data">The Dropdown data to initialize.</param>
/// <param name="options">The options to display in the Dropdown Control.</param>
/// <param name="optionsSize">The number of options in the Dropdown Control.</param>
/// <param name="selectedIndex">The index of the currently selected item.</param>
/// <param name="onSubmit">The event handler to call when the Dropdown Control is submitted.</param>
/// <param name="theme">The Theme to use for the Dropdown Control's appearance.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Control_DropdownData_Init(TREE_Control_DropdownData* data, TREE_String* options, TREE_Size optionsSize, TREE_Size selectedIndex, TREE_ControlEventHandler onSubmit, TREE_Theme const* theme);

/// <summary>
/// Disposes of the given Dropdown data.
/// </summary>
/// <param name="data">The Dropdown data to dispose of.</param>
TREE_EXTERN void TREE_Control_DropdownData_Free(TREE_Control_DropdownData* data);

/// <summary>
/// Sets the options for the given Dropdown data.
/// </summary>
/// <param name="data">The Dropdown data.</param>
/// <param name="options">The new options to display in the Dropdown Control.</param>
/// <param name="optionsSize">The number of new options.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Control_DropdownData_SetOptions(TREE_Control_DropdownData* data, TREE_String* options, TREE_Size optionsSize);

/// <summary>
/// Initializes the given Dropdown Control with the specified data.
/// </summary>
/// <param name="control">The Dropdown Control to initialize.</param>
/// <param name="parent">The parent Transform of the Dropdown Control.</param>
/// <param name="data">The data for the Dropdown Control.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Control_Dropdown_Init(TREE_Control* control, TREE_Transform* parent, TREE_Control_DropdownData* data);

/// <summary>
/// Sets the options for the given Dropdown Control.
/// </summary>
/// <param name="control">The Dropdown Control.</param>
/// <param name="options">The new options to display in the Dropdown Control.</param>
/// <param name="optionsSize">The number of new options.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Control_Dropdown_SetOptions(TREE_Control* control, TREE_String* options, TREE_Size optionsSize);

/// <summary>
/// Gets the options of the given Dropdown Control.
/// </summary>
/// <param name="control">The Dropdown Control.</param>
/// <returns>The options of the Dropdown Control.</returns>
TREE_EXTERN TREE_String* TREE_Control_Dropdown_GetOptions(TREE_Control* control);

/// <summary>
/// Gets the number of options in the given Dropdown Control.
/// </summary>
/// <param name="control">The Dropdown Control.</param>
/// <returns>The number of options in the Dropdown Control.</returns>
TREE_EXTERN TREE_Size TREE_Control_Dropdown_GetOptionsSize(TREE_Control* control);

/// <summary>
/// Sets the selected index of the given Dropdown Control.
/// </summary>
/// <param name="control">The Dropdown Control.</param>
/// <param name="index">The index to set as selected.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Control_Dropdown_SetSelected(TREE_Control* control, TREE_Size index);

/// <summary>
/// Checks if the specified index in the Dropdown Control is selected.
/// </summary>
/// <param name="control">The Dropdown Control.</param>
/// <param name="index">The index to check.</param>
/// <returns>True if the item is selected, false otherwise.</returns>
TREE_EXTERN TREE_Bool TREE_Control_Dropdown_IsSelected(TREE_Control* control, TREE_Size index);

/// <summary>
/// Gets the index of the selected item in the Dropdown Control.
/// </summary>
/// <param name="control">The Dropdown Control.</param>
/// <returns>The index of the selected item, or -1 if no item is selected.</returns>
TREE_EXTERN TREE_Size TREE_Control_Dropdown_GetSelected(TREE_Control* control);

/// <summary>
/// Sets the event handler for the given Dropdown Control when it is submitted.
/// </summary>
/// <param name="control">The Dropdown Control.</param>
/// <param name="onSubmit">The event handler to set.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Control_Dropdown_SetOnSubmit(TREE_Control* control, TREE_ControlEventHandler onSubmit);

/// <summary>
/// Gets the event handler for the given Dropdown Control when it is submitted.
/// </summary>
/// <param name="control">The Dropdown Control.</param>
/// <returns>The event handler for the Dropdown Control.</returns>
TREE_EXTERN TREE_ControlEventHandler TREE_Control_Dropdown_GetOnSubmit(TREE_Control* control);

/// <summary>
/// Handles events for the given Dropdown Control.
/// </summary>
/// <param name="event">The event to handle.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Control_Dropdown_EventHandler(TREE_Event const* event);

///////////////////////////////////////
// Control: Checkbox                 //
///////////////////////////////////////

/// <summary>
/// The flags for a Checkbox Control.
/// </summary>
typedef enum _TREE_Control_CheckboxFlags
{
	/// <summary>
	/// No flags.
	/// </summary>
	TREE_CONTROL_CHECKBOX_FLAGS_NONE = 0x0,

	/// <summary>
	/// The checkbox is checked.
	/// </summary>
	TREE_CONTROL_CHECKBOX_FLAGS_CHECKED = 0x1,

	/// <summary>
	/// The checkbox is reversed, meaning the checkbox is on the right side of the text.
	/// </summary>
	TREE_CONTROL_CHECKBOX_FLAGS_REVERSE = 0x2,

	/// <summary>
	/// The checkbox is a radio button, indicating only one checkbox can be checked at a time. Changes the appearance of the checkbox.
	/// </summary>
	TREE_CONTROL_CHECKBOX_FLAGS_RADIO = 0x4,
} TREE_Control_CheckboxFlags;

/// <summary>
/// The data for a Checkbox Control.
/// </summary>
typedef struct _TREE_Control_CheckboxData
{
	/// <summary>
	/// The text to display next to the checkbox.
	/// </summary>
	TREE_Char* text;

	/// <summary>
	/// The flags for the Checkbox Control.
	/// </summary>
	TREE_Control_CheckboxFlags flags;

	/// <summary>
	/// A reference to the Theme to use for the appearance.
	/// </summary>
	TREE_Theme const* theme;

	/// <summary>
	/// Called when the checkbox is checked or unchecked.
	/// </summary>
	TREE_ControlEventHandler onCheck;
} TREE_Control_CheckboxData;

/// <summary>
/// Initializes the given Checkbox data with the specified parameters.
/// </summary>
/// <param name="data">The Checkbox data to initialize.</param>
/// <param name="text">The text to display next to the checkbox.</param>
/// <param name="checked">The initial checked state of the checkbox.</param>
/// <param name="onCheck">The event handler to call when the checkbox is checked or unchecked.</param>
/// <param name="theme">The Theme to use for the checkbox's appearance.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Control_CheckboxData_Init(TREE_Control_CheckboxData* data, TREE_String text, TREE_Control_CheckboxFlags flags, TREE_ControlEventHandler onCheck, TREE_Theme const* theme);

/// <summary>
/// Disposes of the given Checkbox data.
/// </summary>
/// <param name="data">The Checkbox data to dispose of.</param>
TREE_EXTERN void TREE_Control_CheckboxData_Free(TREE_Control_CheckboxData* data);

/// <summary>
/// Initializes the given Checkbox Control with the specified data.
/// </summary>
/// <param name="control">The Checkbox Control to initialize.</param>
/// <param name="parent">The parent Transform of the Checkbox Control.</param>
/// <param name="data">The data for the Checkbox Control.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Control_Checkbox_Init(TREE_Control* control, TREE_Transform* parent, TREE_Control_CheckboxData* data);

/// <summary>
/// Sets the checked state of the given Checkbox Control.
/// </summary>
/// <param name="control">The Checkbox Control.</param>
/// <param name="checked">The checked state to set (1 for checked, 0 for unchecked).</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Control_Checkbox_SetChecked(TREE_Control* control, TREE_Byte checked);

/// <summary>
/// Gets the checked state of the given Checkbox Control.
/// </summary>
/// <param name="control">The Checkbox Control.</param>
/// <returns>True if the checkbox is checked, false otherwise.</returns>
TREE_EXTERN TREE_Bool TREE_Control_Checkbox_GetChecked(TREE_Control* control);

/// <summary>
/// Handles events for the given Checkbox Control.
/// </summary>
/// <param name="event">The event to handle.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Control_Checkbox_EventHandler(TREE_Event const* event);

///////////////////////////////////////
// Control: NumberInput              //
///////////////////////////////////////

/// <summary>
/// The data for a NumberInput Control.
/// </summary>
typedef struct _TREE_Control_NumberInputData
{
	/// <summary>
	/// The current value of the NumberInput Control.
	/// </summary>
	TREE_Float value;

	/// <summary>
	/// The minimum value of the NumberInput Control.
	/// </summary>
	TREE_Float minValue;

	/// <summary>
	/// The maximum value of the NumberInput Control.
	/// </summary>
	TREE_Float maxValue;
	
	/// <summary>
	/// The amount to increment or decrement the value by.
	/// </summary>
	TREE_Float increment;
	
	/// <summary>
	/// The number of decimal places to display.
	/// </summary>
	TREE_Int decimalPlaces;

	/// <summary>
	/// A reference to the Theme to use for the appearance.
	/// </summary>
	TREE_Theme const* theme;

	/// <summary>
	/// Called when the value changes.
	/// </summary>
	TREE_ControlEventHandler onChange;

	/// <summary>
	/// Called when the NumberInput Control is submitted.
	/// </summary>
	TREE_ControlEventHandler onSubmit;
} TREE_Control_NumberInputData;

/// <summary>
/// Initializes the given NumberInput data with the specified parameters.
/// </summary>
/// <param name="data">The NumberInput data to initialize.</param>
/// <param name="value">The initial value of the NumberInput Control.</param>
/// <param name="min">The minimum value of the NumberInput Control.</param>
/// <param name="max">The maximum value of the NumberInput Control.</param>
/// <param name="increment">The amount to increment or decrement the value by.</param>
/// <param name="decimalPlaces">The number of decimal places to display.</param>
/// <param name="onChange">The event handler to call when the value changes.</param>
/// <param name="onSubmit">The event handler to call when the NumberInput Control is submitted.</param>
/// <param name="theme">The Theme to use for the NumberInput Control's appearance.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Control_NumberInputData_Init(TREE_Control_NumberInputData* data, TREE_Float value, TREE_Float min, TREE_Float max, TREE_Float increment, TREE_Int decimalPlaces, TREE_ControlEventHandler onChange, TREE_ControlEventHandler onSubmit, TREE_Theme const* theme);

/// <summary>
/// Disposes of the given NumberInput data.
/// </summary>
/// <param name="data">The NumberInput data to dispose of.</param>
TREE_EXTERN void TREE_Control_NumberInputData_Free(TREE_Control_NumberInputData* data);

/// <summary>
/// Initializes the given NumberInput Control with the specified data.
/// </summary>
/// <param name="control">The NumberInput Control to initialize.</param>
/// <param name="parent">The parent Transform of the NumberInput Control.</param>
/// <param name="data">The data for the NumberInput Control.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Control_NumberInput_Init(TREE_Control* control, TREE_Transform* parent, TREE_Control_NumberInputData* data);

/// <summary>
/// Sets the value of the given NumberInput Control.
/// </summary>
/// <param name="control">The NumberInput Control.</param>
/// <param name="value">The value to set.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Control_NumberInput_SetValue(TREE_Control* control, TREE_Float value);

/// <summary>
/// Gets the value of the given NumberInput Control.
/// </summary>
/// <param name="control">The NumberInput Control.</param>
/// <returns>The current value of the NumberInput Control.</returns>
TREE_EXTERN TREE_Float TREE_Control_NumberInput_GetValue(TREE_Control* control);

/// <summary>
/// Sets the minimum value of the given NumberInput Control.
/// </summary>
/// <param name="control">The NumberInput Control.</param>
/// <param name="minValue">The minimum value to set.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Control_NumberInput_SetMin(TREE_Control* control, TREE_Float minValue);

/// <summary>
/// Gets the minimum value of the given NumberInput Control.
/// </summary>
/// <param name="control">The NumberInput Control.</param>
/// <returns>The minimum value of the NumberInput Control.</returns>
TREE_EXTERN TREE_Float TREE_Control_NumberInput_GetMin(TREE_Control* control);

/// <summary>
/// Sets the maximum value of the given NumberInput Control.
/// </summary>
/// <param name="control">The NumberInput Control.</param>
/// <param name="maxValue">The maximum value to set.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Control_NumberInput_SetMax(TREE_Control* control, TREE_Float maxValue);

/// <summary>
/// Gets the maximum value of the given NumberInput Control.
/// </summary>
/// <param name="control">The NumberInput Control.</param>
/// <returns>The maximum value of the NumberInput Control.</returns>
TREE_EXTERN TREE_Float TREE_Control_NumberInput_GetMax(TREE_Control* control);

/// <summary>
/// Sets the increment value of the given NumberInput Control.
/// </summary>
/// <param name="control">The NumberInput Control.</param>
/// <param name="increment">The increment value to set.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Control_NumberInput_SetIncrement(TREE_Control* control, TREE_Float increment);

/// <summary>
/// Gets the increment value of the given NumberInput Control.
/// </summary>
/// <param name="control">The NumberInput Control.</param>
/// <returns>The increment value of the NumberInput Control.</returns>
TREE_EXTERN TREE_Float TREE_Control_NumberInput_GetIncrement(TREE_Control* control);

/// <summary>
/// Sets the number of decimal places for the given NumberInput Control.
/// </summary>
/// <param name="control">The NumberInput Control.</param>
/// <param name="decimalPlaces">The number of decimal places to set.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Control_NumberInput_SetDecimalPlaces(TREE_Control* control, TREE_Int decimalPlaces);

/// <summary>
/// Gets the number of decimal places for the given NumberInput Control.
/// </summary>
/// <param name="control">The NumberInput Control.</param>
/// <returns>The number of decimal places of the NumberInput Control.</returns>
TREE_EXTERN TREE_Int TREE_Control_NumberInput_GetDecimalPlaces(TREE_Control* control);

/// <summary>
/// Sets the event handler for the given NumberInput Control when the value changes.
/// </summary>
/// <param name="control">The NumberInput Control.</param>
/// <param name="onChange">The event handler to set.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Control_NumberInput_SetOnChange(TREE_Control* control, TREE_ControlEventHandler onChange);

/// <summary>
/// Gets the event handler for the given NumberInput Control when the value changes.
/// </summary>
/// <param name="control">The NumberInput Control.</param>
/// <returns>The event handler for the NumberInput Control.</returns>
TREE_EXTERN TREE_ControlEventHandler TREE_Control_NumberInput_GetOnChange(TREE_Control* control);

/// <summary>
/// Sets the event handler for the given NumberInput Control when it is submitted.
/// </summary>
/// <param name="control">The NumberInput Control.</param>
/// <param name="onSubmit">The event handler to set.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Control_NumberInput_SetOnSubmit(TREE_Control* control, TREE_ControlEventHandler onSubmit);

/// <summary>
/// Gets the event handler for the given NumberInput Control when it is submitted.
/// </summary>
/// <param name="control">The NumberInput Control.</param>
/// <returns>The event handler for the NumberInput Control.</returns>
TREE_EXTERN TREE_ControlEventHandler TREE_Control_NumberInput_GetOnSubmit(TREE_Control* control);

/// <summary>
/// Handles events for the given NumberInput Control.
/// </summary>
/// <param name="event">The event to handle.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Control_NumberInput_EventHandler(TREE_Event const* event);

///////////////////////////////////////
// Control: ProgressBar              //
///////////////////////////////////////

/// <summary>
/// The data for a ProgressBar Control.
/// </summary>
typedef struct _TREE_Control_ProgressBarData
{
	/// <summary>
	/// The current value of the ProgressBar Control. Must be between 0.0 and 1.0.
	/// </summary>
	TREE_Float value;

	/// <summary>
	/// The direction the ProgressBar Control moves.
	/// </summary>
	TREE_Direction direction;

	/// <summary>
	/// A reference to the Theme to use for the appearance.
	/// </summary>
	TREE_Theme const* theme;
} TREE_Control_ProgressBarData;

/// <summary>
/// Initializes the given ProgressBar data with the specified theme.
/// </summary>
/// <param name="data">The ProgressBar data to initialize.</param>
/// <param name="theme">The Theme to use for the ProgressBar's appearance.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Control_ProgressBarData_Init(TREE_Control_ProgressBarData* data, TREE_Theme const* theme);

/// <summary>
/// Disposes of the given ProgressBar data.
/// </summary>
/// <param name="data">The ProgressBar data to dispose of.</param>
TREE_EXTERN void TREE_Control_ProgressBarData_Free(TREE_Control_ProgressBarData* data);

/// <summary>
/// Initializes the given ProgressBar Control with the specified data.
/// </summary>
/// <param name="control">The ProgressBar Control to initialize.</param>
/// <param name="parent">The parent Transform of the ProgressBar Control.</param>
/// <param name="data">The data for the ProgressBar Control.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Control_ProgressBar_Init(TREE_Control* control, TREE_Transform* parent, TREE_Control_ProgressBarData* data);

/// <summary>
/// Sets the value of the given ProgressBar Control.
/// </summary>
/// <param name="control">The ProgressBar Control.</param>
/// <param name="value">The value to set. Must be between 0.0 and 1.0.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Control_ProgressBar_SetValue(TREE_Control* control, TREE_Float value);

/// <summary>
/// Gets the value of the given ProgressBar Control.
/// </summary>
/// <param name="control">The ProgressBar Control.</param>
/// <returns>The current value of the ProgressBar Control.</returns>
TREE_EXTERN TREE_Float TREE_Control_ProgressBar_GetValue(TREE_Control* control);

/// <summary>
/// Sets the direction of the given ProgressBar Control.
/// </summary>
/// <param name="control">The ProgressBar Control.</param>
/// <param name="direction">The direction to set (e.g., left-to-right, top-to-bottom).</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Control_ProgressBar_SetDirection(TREE_Control* control, TREE_Direction direction);

/// <summary>
/// Gets the direction of the given ProgressBar Control.
/// </summary>
/// <param name="control">The ProgressBar Control.</param>
/// <returns>The direction of the ProgressBar Control.</returns>
TREE_EXTERN TREE_Direction TREE_Control_ProgressBar_GetDirection(TREE_Control* control);

/// <summary>
/// Handles events for the given ProgressBar Control.
/// </summary>
/// <param name="event">The event to handle.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Control_ProgressBar_EventHandler(TREE_Event const* event);

///////////////////////////////////////
// Application                       //
///////////////////////////////////////

/// <summary>
/// Maintains and manages the state of an application.
/// </summary>
typedef struct _TREE_Application
{
	/// <summary>
	/// The Controls within this Application.
	/// </summary>
	TREE_Control** controls;
	
	/// <summary>
	/// The number of Controls in this Application.
	/// </summary>
	TREE_Size controlsSize;

	/// <summary>
	/// The maximum number of Controls this Application can hold.
	/// </summary>
	TREE_Size controlsCapacity;

	/// <summary>
	/// The currently focused Control.
	/// </summary>
	TREE_Control* focusedControl;

	/// <summary>
	/// True when the application is running.
	/// </summary>
	TREE_Bool running;

	/// <summary>
	/// The Input state of the application.
	/// </summary>
	TREE_Input input;

	/// <summary>
	/// The EventHandler for this Application.
	/// </summary>
	TREE_EventHandler eventHandler;

	/// <summary>
	/// The Surface this Application draws to.
	/// </summary>
	TREE_Surface* surface;
} TREE_Application;

/// <summary>
/// Initializes the given Application with the specified capacity and event handler.
/// </summary>
/// <param name="application">The Application to initialize.</param>
/// <param name="capacity">The maximum number of Controls the Application can hold.</param>
/// <param name="eventHandler">The EventHandler for the Application.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Application_Init(TREE_Application* application, TREE_Size capacity, TREE_EventHandler eventHandler);

/// <summary>
/// Disposes of the given Application and its resources.
/// </summary>
/// <param name="application">The Application to dispose of.</param>
TREE_EXTERN void TREE_Application_Free(TREE_Application* application);

/// <summary>
/// Adds a Control to the given Application.
/// </summary>
/// <param name="application">The Application to add the Control to.</param>
/// <param name="control">The Control to add.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Application_AddControl(TREE_Application* application, TREE_Control* control);

/// <summary>
/// Sets the focus to the specified Control in the given Application.
/// </summary>
/// <param name="application">The Application to set the focus in.</param>
/// <param name="control">The Control to focus.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Application_SetFocus(TREE_Application* application, TREE_Control* control);

/// <summary>
/// Dispatches the given Event to the appropriate handler in the Application.
/// </summary>
/// <param name="application">The Application to dispatch the Event to.</param>
/// <param name="event">The Event to dispatch.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Application_DispatchEvent(TREE_Application* application, TREE_Event const* event);

/// <summary>
/// Runs the main loop of the given Application.
/// </summary>
/// <param name="application">The Application to run.</param>
/// <returns>A TREE_Result code.</returns>
TREE_EXTERN TREE_Result TREE_Application_Run(TREE_Application* application);

/// <summary>
/// Quits the given Application, stopping its main loop.
/// </summary>
/// <param name="application">The Application to quit.</param>
TREE_EXTERN void TREE_Application_Quit(TREE_Application* application);

#endif // __TREE_H__