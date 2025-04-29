#include "pch.h"
#include "framework.h"
#include "TREE.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

#ifdef _WIN32
#define TREE_WINDOWS
#endif // WIN32

#ifdef TREE_WINDOWS
#include <windows.h>
#include <conio.h>
#endif // TREE_WINDOWS

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define CALL_ACTION(action, ...) do { if (action) { action(__VA_ARGS__); } } while (0)

TREE_Char const* TREE_Result_ToString(TREE_Result code)
{
	switch (code)
	{
	case TREE_OK:
		return "OK";
	case TREE_CANCEL:
		return "Cancel";
	case TREE_NOT_IMPLEMENTED:
		return "Not implemented";
	case TREE_ERROR:
		return "Generic error";
	case TREE_ERROR_OVERFLOW:
		return "Overflow error";
	case TREE_ERROR_FULL:
		return "The collection is at capacity";
	case TREE_ERROR_ARG_NULL:
		return "Argument is null";
	case TREE_ERROR_ARG_OUT_OF_RANGE:
		return "Argument is out of range";
	case TREE_ERROR_ARG_INVALID:
		return "Argument is invalid";
	case TREE_ERROR_ALLOC:
		return "Memory allocation failed";
	case TREE_ERROR_PRESENTATION:
		return "Presentation failed";
	case TREE_ERROR_FILE_OPEN:
		return "Failed to open file";
	case TREE_ERROR_FILE_DELETE:
		return "Failed to delete file";
	case TREE_ERROR_DIRECTORY_CREATE:
		return "Failed to create directory";
	case TREE_ERROR_DIRECTORY_DELETE:
		return "Failed to delete directory";
	case TREE_ERROR_DIRECTORY_ENUMERATE:
		return "Failed to enumerate directory";
	case TREE_ERROR_CLIPBOARD_SET_TEXT:
		return "Failed to set clipboard text";
	case TREE_ERROR_CLIPBOARD_GET_TEXT:
		return "Failed to get clipboard text";
	case TREE_ERROR_WORD_WRAPPING:
		return "Word wrapping failed";
	case TREE_ERROR_WORD_WRAPPING_OFFSETS:
		return "Failed to generate offsets from word wrapping";
	case TREE_ERROR_APPLICATION_MULTIPLE_ACTIVE_CONTROLS:
		return "Multiple active controls are not allowed";
	case TREE_ERROR_WINDOW_SET_TITLE:
		return "Failed to set window title";

	case TREE_ERROR_WINDOWS_GLOBAL_ALLOC:
		return "Failed to allocate global memory";
	case TREE_ERROR_WINDOWS_GLOBAL_LOCK:
		return "Failed to lock global memory";
	case TREE_ERROR_WINDOWS_GLOBAL_UNLOCK:
		return "Failed to unlock global memory";
	case TREE_ERROR_WINDOWS_SET_CONTROL_HANDLER:
		return "Failed to set control handler";
	case TREE_ERROR_WINDOWS_CLIPBOARD_OPEN:
		return "Failed to open clipboard";
	case TREE_ERROR_WINDOWS_CLIPBOARD_CLOSE:
		return "Failed to close clipboard";
	case TREE_ERROR_WINDOWS_CLIPBOARD_CLEAR:
		return "Failed to clear clipboard";
	case TREE_ERROR_WINDOWS_CONSOLE_GET_CURSOR_INFO:
		return "Failed to get console cursor info";
	case TREE_ERROR_WINDOWS_CONSOLE_SET_CURSOR_INFO:
		return "Failed to set console cursor info";
	default:
		return "Unknown error";
	}
}

#ifdef TREE_WINDOWS

BOOL WINAPI _ConsoleCtrlHandler(DWORD dwCtrlType)
{
	switch (dwCtrlType)
	{
	case CTRL_C_EVENT:
	case CTRL_BREAK_EVENT:
	case CTRL_CLOSE_EVENT:
	case CTRL_LOGOFF_EVENT:
	case CTRL_SHUTDOWN_EVENT:
		return TRUE;
	default:
		return FALSE;
	}
}

#endif // TREE_WINDOWS

TREE_Result TREE_Init()
{
	TREE_Result result;

#ifdef TREE_WINDOWS
	// handle CTRL +C, etc.
	if (!SetConsoleCtrlHandler(_ConsoleCtrlHandler, TRUE))
	{
		return TREE_ERROR_WINDOWS_SET_CONTROL_HANDLER;
	}
#endif

	// hide cursor
	result = TREE_Cursor_SetVisible(TREE_FALSE);
	if (result)
	{
		return result;
	}

	return TREE_OK;
}

void TREE_Free()
{
	// show cursor
	TREE_Cursor_SetVisible(TREE_TRUE);
}

TREE_Result TREE_String_CreateCopy(TREE_Char** dest, TREE_String src)
{
	// validate
	if (!dest || !src)
	{
		return TREE_ERROR_ARG_NULL;
	}

	// allocate memory for the string
	TREE_Size length = strlen(src);
	*dest = TREE_NEW_ARRAY(TREE_Char, length + 1);
	if (!*dest)
	{
		return TREE_ERROR_ALLOC;
	}

	// copy the string
	memcpy(*dest, src, length);
	(*dest)[length] = '\0';

	return TREE_OK;
}

TREE_Result TREE_String_CreateClampedCopy(TREE_Char** dest, TREE_String src, TREE_Size maxSize)
{
	// validate
	if (!dest || !src)
	{
		return TREE_ERROR_ARG_NULL;
	}

	// allocate memory for the string
	TREE_Size length = strlen(src);
	if (length > maxSize)
	{
		length = maxSize;
	}
	*dest = TREE_NEW_ARRAY(TREE_Char, length + 1);
	if (!*dest)
	{
		return TREE_ERROR_ALLOC;
	}

	// copy the string
	memcpy(*dest, src, length);
	(*dest)[length] = '\0';

	return TREE_OK;
}

TREE_Result TREE_Clipboard_SetText(TREE_String text)
{
	// validate
	if (!text)
	{
		return TREE_ERROR_ARG_NULL;
	}

#ifdef TREE_WINDOWS
	// open the clipboard
	if (!OpenClipboard(NULL))
	{
		return TREE_ERROR_WINDOWS_CLIPBOARD_OPEN;
	}

	// clear the clipboard
	if (!EmptyClipboard())
	{
		CloseClipboard();
		return TREE_ERROR_WINDOWS_CLIPBOARD_CLEAR;
	}

	// allocate global memory
	size_t textLength = strlen(text) + 1;
	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, textLength);
	if (!hGlobal)
	{
		CloseClipboard();
		return TREE_ERROR_WINDOWS_GLOBAL_ALLOC;
	}

	// copy the text to the global memory
	char* pGlobal = (char*)GlobalLock(hGlobal);
	if (!pGlobal)
	{
		GlobalFree(hGlobal);
		CloseClipboard();
		return TREE_ERROR_WINDOWS_GLOBAL_LOCK;
	}
	memcpy(pGlobal, text, textLength);
	GlobalUnlock(hGlobal);

	// set the clipboard data
	if (!SetClipboardData(CF_TEXT, hGlobal))
	{
		GlobalFree(hGlobal);
		CloseClipboard();
		return TREE_ERROR_CLIPBOARD_SET_TEXT;
	}

	// close the clipboard
	if (!CloseClipboard())
	{
		GlobalFree(hGlobal);
		return TREE_ERROR_WINDOWS_CLIPBOARD_CLOSE;
	}

	return TREE_OK;
#else
	return TREE_NOT_IMPLEMENTED;
#endif // TREE_WINDOWS
}

TREE_Result TREE_Clipboard_GetText(TREE_Char** text)
{
	// validate
	if (!text)
	{
		return TREE_ERROR_ARG_NULL;
	}

	*text = NULL;

#ifdef TREE_WINDOWS
	// open the clipboard
	if (!OpenClipboard(NULL))
	{
		return TREE_ERROR_WINDOWS_CLIPBOARD_OPEN;
	}

	// check if there is text to paste
	if (!IsClipboardFormatAvailable(CF_TEXT))
	{
		CloseClipboard();
		return TREE_OK;
	}

	// get the clipboard data
	HANDLE hData = GetClipboardData(CF_TEXT);
	if (!hData)
	{
		CloseClipboard();
		return TREE_ERROR_CLIPBOARD_GET_TEXT;
	}

	// lock the global memory
	char* pGlobal = (char*)GlobalLock(hData);
	if (!pGlobal)
	{
		CloseClipboard();
		return TREE_ERROR_WINDOWS_GLOBAL_LOCK;
	}

	// allocate memory for the text
	size_t textLength = GlobalSize(hData);
	*text = TREE_NEW_ARRAY(TREE_Char, textLength + 1);
	if (!*text)
	{
		GlobalUnlock(hData);
		CloseClipboard();
		return TREE_ERROR_ALLOC;
	}

	// copy the text to the allocated memory
	memcpy(*text, pGlobal, textLength);
	(*text)[textLength] = '\0';

	// unlock the global memory
	GlobalUnlock(hData);

	// close the clipboard
	CloseClipboard();

	return TREE_OK;
#else
	return TREE_NOT_IMPLEMENTED;
#endif // TREE_WINDOWS
}

TREE_ColorPair TREE_ColorPair_Create(TREE_Color foreground, TREE_Color background)
{
	return (foreground & 0xF) << 4 | (background & 0xF);
}

TREE_ColorPair TREE_ColorPair_CreateDefault()
{
	return TREE_ColorPair_Create(
		TREE_COLOR_DEFAULT_FOREGROUND,
		TREE_COLOR_DEFAULT_BACKGROUND);
}

TREE_Color TREE_ColorPair_GetForeground(TREE_ColorPair colorPair)
{
	return colorPair >> 4;
}

TREE_Color TREE_ColorPair_GetBackground(TREE_ColorPair colorPair)
{
	return colorPair & 0xF;
}

TREE_String TREE_Color_GetForegroundString(TREE_Color color)
{
	switch (color)
	{
	case TREE_COLOR_BLACK: return "\033[030m";
	case TREE_COLOR_RED: return "\033[031m";
	case TREE_COLOR_GREEN: return "\033[032m";
	case TREE_COLOR_YELLOW: return "\033[033m";
	case TREE_COLOR_BLUE: return "\033[034m";
	case TREE_COLOR_MAGENTA: return "\033[035m";
	case TREE_COLOR_CYAN: return "\033[036m";
	case TREE_COLOR_WHITE: return "\033[037m";
	case TREE_COLOR_BRIGHT_BLACK: return "\033[090m";
	case TREE_COLOR_BRIGHT_RED: return "\033[091m";
	case TREE_COLOR_BRIGHT_GREEN: return "\033[092m";
	case TREE_COLOR_BRIGHT_YELLOW: return "\033[093m";
	case TREE_COLOR_BRIGHT_BLUE: return "\033[094m";
	case TREE_COLOR_BRIGHT_MAGENTA: return "\033[095m";
	case TREE_COLOR_BRIGHT_CYAN: return "\033[096m";
	case TREE_COLOR_BRIGHT_WHITE: return "\033[097m";
	default: return NULL;
	};
}

TREE_String TREE_Color_GetBackgroundString(TREE_Color color)
{
	switch (color)
	{
	case TREE_COLOR_BLACK: return "\033[040m";
	case TREE_COLOR_RED: return "\033[041m";
	case TREE_COLOR_GREEN: return "\033[042m";
	case TREE_COLOR_YELLOW: return "\033[043m";
	case TREE_COLOR_BLUE: return "\033[044m";
	case TREE_COLOR_MAGENTA: return "\033[045m";
	case TREE_COLOR_CYAN: return "\033[046m";
	case TREE_COLOR_WHITE: return "\033[047m";
	case TREE_COLOR_BRIGHT_BLACK: return "\033[100m";
	case TREE_COLOR_BRIGHT_RED: return "\033[101m";
	case TREE_COLOR_BRIGHT_GREEN: return "\033[102m";
	case TREE_COLOR_BRIGHT_YELLOW: return "\033[103m";
	case TREE_COLOR_BRIGHT_BLUE: return "\033[104m";
	case TREE_COLOR_BRIGHT_MAGENTA: return "\033[105m";
	case TREE_COLOR_BRIGHT_CYAN: return "\033[106m";
	case TREE_COLOR_BRIGHT_WHITE: return "\033[107m";
	default: return NULL;
	}
}

TREE_String TREE_Color_GetResetString()
{
	return "\033[000m";
}

TREE_String TREE_Path_Absolute(TREE_String path)
{
	// validate
	if (!path)
	{
		return NULL;
	}

	// get full path
#ifdef TREE_WINDOWS
	// get the absolute path
	TREE_Char buffer[MAX_PATH];
	if (_fullpath(buffer, path, sizeof(buffer)) == NULL)
	{
		return NULL;
	}
	// allocate memory for the string
	TREE_Size absoluteSize = strlen(buffer) + 1;
	TREE_Char* absolutePath = (TREE_Char*)malloc(absoluteSize);
	if (!absolutePath)
	{
		return NULL;
	}
	// copy the string
	memcpy(absolutePath, buffer, absoluteSize);
	return absolutePath;
#else
	return NULL;
#endif // TREE_WINDOWS
}

TREE_String TREE_Path_Parent(TREE_String path)
{
	// validate
	if (!path)
	{
		return NULL;
	}

	// find the last slash
	TREE_Char* lastSlash = strrchr(path, '/');
	if (!lastSlash)
	{
		// try the last backslash instead
		lastSlash = strrchr(path, '\\');
		if (!lastSlash)
		{
			// no parent
			return NULL;
		}
	}

	// allocate memory for the parent path
	TREE_Size parentSize = lastSlash - path + 1;
	TREE_Char* parentPath = (TREE_Char*)malloc(parentSize);
	if (!parentPath)
	{
		return NULL;
	}

	// copy the parent path
	memcpy(parentPath, path, parentSize - 1);
	parentPath[parentSize - 1] = '\0';

	return parentPath;
}

TREE_Bool TREE_File_Exists(TREE_String path)
{
	// validate
	if (!path)
	{
		return TREE_FALSE;
	}

	// check if the path is a directory
	struct stat buffer;
	if (stat(path, &buffer) == 0 && (buffer.st_mode & S_IFREG))
	{
		return TREE_TRUE;
	}

	return TREE_FALSE;
}

TREE_Size TREE_File_Size(TREE_String path)
{
	// validate
	if (!TREE_File_Exists(path))
	{
		return 0;
	}

	// open the file
	FILE* file = fopen(path, "rb");
	if (!file)
	{
		return 0;
	}

	// seek to the end of the file to get size
	fseek(file, 0, SEEK_END);
	TREE_Size size = ftell(file);
	fclose(file);
	if (size == -1)
	{
		return 0;
	}

	return size;
}

TREE_Result TREE_File_Read(TREE_String path, TREE_Char* text, TREE_Size size)
{
	// validate
	if (!path || !text)
	{
		return TREE_ERROR_ARG_NULL;
	}

	// check if file exists
	if (!TREE_File_Exists(path))
	{
		return TREE_ERROR_ARG_INVALID;
	}

	// open the file
	FILE* file = fopen(path, "rb");
	if (!file)
	{
		return TREE_ERROR_FILE_OPEN;
	}

	// get the size of the file
	fseek(file, 0, SEEK_END);
	TREE_Size fileSize = ftell(file);
	fseek(file, 0, SEEK_SET);

	// read the minimum between count and size, if a size given
	TREE_Size readSize = size > fileSize ? fileSize : size;

	// read the file into memory
	fread(text, sizeof(TREE_Char), readSize, file);
	text[readSize] = '\0';

	fclose(file);

	return TREE_OK;
}

TREE_Result TREE_File_Write(TREE_String path, TREE_String text)
{
	// validate
	if (!path || !text)
	{
		return TREE_ERROR_ARG_NULL;
	}

	// open the file
	FILE* file = fopen(path, "wb");
	if (!file)
	{
		return TREE_ERROR_FILE_OPEN;
	}

	// write the text to the file
	fwrite(text, sizeof(TREE_Char), strlen(text), file);
	fclose(file);

	return TREE_OK;
}

TREE_Result TREE_File_Create(TREE_String path)
{
	// validate
	if (!path)
	{
		return TREE_ERROR_ARG_NULL;
	}

	// create the file
	FILE* file = fopen(path, "wb");
	if (!file)
	{
		return TREE_ERROR_FILE_OPEN;
	}

	// close the file
	fclose(file);

	return TREE_OK;
}

TREE_Result TREE_File_Delete(TREE_String path)
{
	// validate
	if (!path)
	{
		return TREE_ERROR_ARG_NULL;
	}

	// check if file exists
	if (!TREE_File_Exists(path))
	{
		return TREE_ERROR_ARG_INVALID;
	}

	// delete the file
	if (remove(path) != 0)
	{
		return TREE_ERROR_FILE_DELETE;
	}

	return TREE_OK;
}

TREE_Bool TREE_Directory_Exists(TREE_String path)
{
	// validate
	if (!path)
	{
		return TREE_FALSE;
	}

	// check if the path is a directory
	struct stat buffer;
	if (stat(path, &buffer) == 0 && (buffer.st_mode & S_IFDIR))
	{
		return TREE_TRUE;
	}

	return TREE_FALSE;
}

TREE_Result TREE_Directory_Create(TREE_String path)
{
#ifdef TREE_WINDOWS
	if (!CreateDirectoryA(path, NULL))
	{
		if (GetLastError() != ERROR_ALREADY_EXISTS)
		{
			return TREE_ERROR_DIRECTORY_CREATE;
		}
	}
	return TREE_OK;
#else
	return TREE_NOT_IMPLEMENTED;
#endif // TREE_WINDOWS
}

TREE_Result TREE_Directory_Delete(TREE_String path)
{
	// validate
	if (!path)
	{
		return TREE_ERROR_ARG_NULL;
	}

	// check if directory exists
	if (!TREE_Directory_Exists(path))
	{
		return TREE_ERROR_ARG_INVALID;
	}

#ifdef TREE_WINDOWS
	if (!RemoveDirectoryA(path))
	{
		return TREE_ERROR_DIRECTORY_DELETE;
	}
	return TREE_OK;
#else
	return TREE_NOT_IMPLEMENTED;
#endif // TREE_WINDOWS
}

TREE_Bool _TREE_Directory_Filter(TREE_String name, TREE_Bool isDirectory, TREE_FileTypeFlags flags)
{
	// ignore "." and ".."
	if (strcmp(name, ".") == 0 ||
		strcmp(name, "..") == 0)
	{
		return TREE_FALSE;
	}

	// filter out files, directories, etc. based on flags
	// directories
	if (!(flags & TREE_FILE_TYPE_FLAGS_DIRECTORY) && isDirectory)
	{
		return TREE_FALSE;
	}
	// files
	if (!(flags & TREE_FILE_TYPE_FLAGS_FILE) && !isDirectory)
	{
		return TREE_FALSE;
	}
	// hidden files or directories
	if (!(flags & TREE_FILE_TYPE_FLAGS_HIDDEN) && (isDirectory && name[0] == '.'))
	{
		return TREE_FALSE;
	}

	return TREE_TRUE;
}

TREE_Result TREE_Directory_Enumerate(TREE_String path, TREE_Char*** files, TREE_Size* count, TREE_FileTypeFlags flags)
{
	// validate  
	if (!path || !files || !count)
	{
		return TREE_ERROR_ARG_NULL;
	}
	if (*files)
	{
		return TREE_ERROR_ARG_INVALID;
	}

	// if no flags given, default to files and directories  
	if (flags == TREE_FILE_TYPE_FLAGS_NONE)
	{
		flags = TREE_FILE_TYPE_FLAGS_FILE | TREE_FILE_TYPE_FLAGS_DIRECTORY;
	}

	// check if directory exists  
	if (!TREE_Directory_Exists(path))
	{
		return TREE_ERROR_ARG_INVALID;
	}

#ifdef TREE_WINDOWS  
	char searchPath[MAX_PATH];
	snprintf(searchPath, sizeof(searchPath), "%s/*", path);

	// get number of files  
	TREE_Size fileCount = 0;
	{
		WIN32_FIND_DATAA findData;
		HANDLE hFind = FindFirstFileA(searchPath, &findData);
		if (hFind == INVALID_HANDLE_VALUE)
		{
			return TREE_ERROR_DIRECTORY_ENUMERATE;
		}
		do {
			if (!_TREE_Directory_Filter(findData.cFileName, findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY, flags))
			{
				continue;
			}
			fileCount++;
		} while (FindNextFileA(hFind, &findData));
		FindClose(hFind);
	}

	// allocate memory for the file names  
	*files = (TREE_Char**)malloc(fileCount * sizeof(TREE_Char*));
	if (!*files)
	{
		return TREE_ERROR_ALLOC;
	}

	// iterate through the files again and store the names  
	WIN32_FIND_DATAA findData;
	HANDLE hFind = FindFirstFileA(searchPath, &findData);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		free(*files);
		*files = NULL;
		return TREE_ERROR_DIRECTORY_ENUMERATE;
	}

	TREE_Size i = 0;
	do {
		TREE_Bool isDirectory = findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
		if (!_TREE_Directory_Filter(findData.cFileName, isDirectory, flags))
		{
			continue;
		}

		// Allocate memory for the new file name  
		TREE_Size fileNameSize = strlen(findData.cFileName) + 1;
		// if directory, add 1 for / at end
		if (isDirectory)
		{
			fileNameSize++;
		}
		TREE_Char* fileName = (TREE_Char*)malloc(fileNameSize);
		if (!fileName) {
			TREE_DELETE_ARRAY(*files, i);
			FindClose(hFind);
			return TREE_ERROR_ALLOC;
		}
		memcpy(fileName, findData.cFileName, fileNameSize);
		// if directory, add / at end
		if (isDirectory)
		{
			fileName[fileNameSize - 2] = '/';
			fileName[fileNameSize - 1] = '\0';
		}

		// add the file name to the array  
		(*files)[i] = fileName;
		i++;
	} while (FindNextFileA(hFind, &findData));

	FindClose(hFind);
	*count = fileCount;
	return TREE_OK;
#else  
	return TREE_NOT_IMPLEMENTED;
#endif // TREE_WINDOWS  
}

TREE_Bool TREE_Rect_IsOverlapping(TREE_Rect const* rectA, TREE_Rect const* rectB)
{
	return (
		rectA->offset.x < rectB->offset.x + (TREE_Int)rectB->extent.width &&
		rectA->offset.x + (TREE_Int)rectA->extent.width > rectB->offset.x &&
		rectA->offset.y < rectB->offset.y + (TREE_Int)rectB->extent.height &&
		rectA->offset.y + (TREE_Int)rectA->extent.height > rectB->offset.y
		);
}

TREE_Rect TREE_Rect_Combine(TREE_Rect const* rectA, TREE_Rect const* rectB)
{
	TREE_Rect result;
	result.offset.x = MIN(rectA->offset.x, rectB->offset.x);
	result.offset.y = MIN(rectA->offset.y, rectB->offset.y);
	result.extent.width = MAX(rectA->offset.x + (TREE_Int)rectA->extent.width, rectB->offset.x + (TREE_Int)rectB->extent.width) - result.offset.x;
	result.extent.height = MAX(rectA->offset.y + (TREE_Int)rectA->extent.height, rectB->offset.y + (TREE_Int)rectB->extent.height) - result.offset.y;
	return result;
}

TREE_Rect TREE_Rect_GetIntersection(TREE_Rect const* rectA, TREE_Rect const* rectB)
{
	// ignore if not overlapping
	if (!rectA || !rectB || !TREE_Rect_IsOverlapping(rectA, rectB))
	{
		TREE_Rect emptyRect = { {0, 0}, {0, 0} };
		return emptyRect;
	}

	TREE_Rect result;
	result.offset.x = MAX(rectA->offset.x, rectB->offset.x);
	result.offset.y = MAX(rectA->offset.y, rectB->offset.y);
	result.extent.width = MIN(rectA->offset.x + (TREE_Int)rectA->extent.width, rectB->offset.x + (TREE_Int)rectB->extent.width) - result.offset.x;
	result.extent.height = MIN(rectA->offset.y + (TREE_Int)rectA->extent.height, rectB->offset.y + (TREE_Int)rectB->extent.height) - result.offset.y;
	return result;
}

TREE_Pixel TREE_Pixel_CreateDefault()
{
	TREE_Pixel pixel;
	pixel.character = ' ';
	pixel.colorPair = TREE_ColorPair_CreateDefault();
	return pixel;
}

TREE_Result TREE_Pattern_Init(TREE_Pattern* pattern, TREE_UInt size)
{
	// validate
	if (!pattern)
	{
		return TREE_ERROR_ARG_NULL;
	}
	if (size == 0)
	{
		return TREE_ERROR_ARG_OUT_OF_RANGE;
	}

	// allocate data
	pattern->pixels = (TREE_Pixel*)malloc(size * sizeof(TREE_Pixel));
	if (!pattern->pixels)
	{
		return TREE_ERROR_ALLOC;
	}
	pattern->size = size;

	// set data
	TREE_Pixel defaultPixel;
	defaultPixel.character = ' ';
	defaultPixel.colorPair = TREE_ColorPair_CreateDefault();
	for (TREE_UInt i = 0; i < size; ++i)
	{
		pattern->pixels[i] = defaultPixel;
	}

	return TREE_OK;
}

TREE_Result TREE_Pattern_InitFromString(TREE_Pattern* pattern, TREE_String string, TREE_ColorPair colorPair)
{
	// validate
	if (!pattern || !string)
	{
		return TREE_ERROR_ARG_NULL;
	}
	TREE_Size stringLength = strlen(string);
	if (stringLength == 0)
	{
		return TREE_ERROR_ARG_INVALID;
	}

	// allocate data
	pattern->pixels = (TREE_Pixel*)malloc(stringLength * sizeof(TREE_Pixel));
	if (!pattern->pixels)
	{
		return TREE_ERROR_ALLOC;
	}

	// set data
	pattern->size = (TREE_UInt)stringLength;
	for (TREE_UInt i = 0; i < stringLength; ++i)
	{
		pattern->pixels[i].character = string[i];
		pattern->pixels[i].colorPair = colorPair;
	}

	return TREE_OK;
}

TREE_Result TREE_Pattern_Set(TREE_Pattern* pattern, TREE_UInt index, TREE_Pixel pixel)
{
	// validate
	if (!pattern)
	{
		return TREE_ERROR_ARG_NULL;
	}
	if (index >= pattern->size)
	{
		return TREE_ERROR_ARG_OUT_OF_RANGE;
	}
	if (pixel.character == '\0')
	{
		return TREE_ERROR_ARG_INVALID;
	}

	// set data
	pattern->pixels[index].character = pixel.character;

	return TREE_OK;
}

TREE_Result TREE_Pattern_Get(TREE_Pattern* pattern, TREE_UInt index, TREE_Pixel* pixel)
{
	// validate
	if (!pattern || !pixel)
	{
		return TREE_ERROR_ARG_NULL;
	}
	if (index >= pattern->size)
	{
		return TREE_ERROR_ARG_OUT_OF_RANGE;
	}

	// get data
	*pixel = pattern->pixels[index];

	return TREE_OK;
}

void TREE_Pattern_Free(TREE_Pattern* pattern)
{
	// validate
	if (!pattern)
	{
		return;
	}

	// free data
	if (pattern->pixels)
	{
		free(pattern->pixels);
		pattern->pixels = NULL;
	}
	pattern->size = 0;
}

static TREE_Size _TREE_Image_GetIndex(TREE_Image* image, TREE_Offset offset)
{
	// calculate index
	return (TREE_Size)offset.y * image->extent.width + offset.x;
}

TREE_Result TREE_Image_Init(TREE_Image* image, TREE_Extent extent)
{
	// validate
	if (!image)
	{
		return TREE_ERROR_ARG_NULL;
	}

	// if size is 0, set to defaults
	if (extent.width == 0 || extent.height == 0)
	{
		image->text = NULL;
		image->colors = NULL;
		image->extent.width = 0;
		image->extent.height = 0;

		return TREE_OK;
	}

	// calculate sizes
	TREE_Size imageSize = (TREE_Size)(extent.width * extent.height);
	TREE_Size textSize = (imageSize + 1) * sizeof(TREE_Char); // +1 for null terminator
	TREE_Size colorSize = imageSize * sizeof(TREE_ColorPair);

	// allocate data
	image->text = (TREE_Char*)malloc(textSize);
	if (!image->text)
	{
		return TREE_ERROR_ALLOC;
	}
	image->colors = (TREE_ColorPair*)malloc(colorSize);
	if (!image->colors)
	{
		free(image->text);
		image->text = NULL;
		return TREE_ERROR_ALLOC;
	}

	// set data
	memset(image->text, ' ', textSize);
	image->text[imageSize] = '\0'; // null terminator
	memset(image->colors, TREE_ColorPair_CreateDefault(), colorSize);
	image->extent = extent;

	return TREE_OK;
}

void TREE_Image_Free(TREE_Image* image)
{
	// validate
	if (!image)
	{
		return;
	}

	// free data
	if (image->text)
	{
		free(image->text);
		image->text = NULL;
	}
	if (image->colors)
	{
		free(image->colors);
		image->colors = NULL;
	}
	image->extent.width = 0;
	image->extent.height = 0;
}

TREE_Result TREE_Image_Set(TREE_Image* image, TREE_Offset offset, TREE_Pixel pixel)
{
	// validate
	if (!image)
	{
		return TREE_ERROR_ARG_NULL;
	}
	if (offset.x < 0 || offset.y < 0 ||
		offset.x >= (TREE_Int)image->extent.width ||
		offset.y >= (TREE_Int)image->extent.height)
	{
		return TREE_ERROR_ARG_OUT_OF_RANGE;
	}

	// set data
	TREE_Size index = _TREE_Image_GetIndex(image, offset);
	image->text[index] = pixel.character;
	image->colors[index] = pixel.colorPair;

	return TREE_OK;
}

TREE_Pixel TREE_Image_Get(TREE_Image* image, TREE_Offset offset)
{
	TREE_Pixel pixel = { 0, 0 };

	// validate
	if (!image)
	{
		return pixel;
	}
	if (offset.x < 0 || offset.y < 0 ||
		offset.x >= (TREE_Int)image->extent.width ||
		offset.y >= (TREE_Int)image->extent.height)
	{
		return pixel;
	}

	// get data
	TREE_Size index = _TREE_Image_GetIndex(image, offset);
	pixel.character = image->text[index];
	pixel.colorPair = image->colors[index];

	return pixel;
}

TREE_Result TREE_Image_Resize(TREE_Image* image, TREE_Extent extent)
{
	// validate
	if (!image)
	{
		return TREE_ERROR_ARG_NULL;
	}

	// do nothing if size is the same
	if (image->extent.width == extent.width && image->extent.height == extent.height)
	{
		return TREE_OK;
	}

	TREE_Image_Free(image);
	TREE_Result result = TREE_Image_Init(image, extent);
	if (result)
	{
		return result;
	}

	return TREE_OK;
}

TREE_Result TREE_Image_DrawImage(TREE_Image* image, TREE_Offset offset, TREE_Image const* other, TREE_Offset otherOffset, TREE_Extent extent)
{
	// validate
	if (!image || !other)
	{
		return TREE_ERROR_ARG_NULL;
	}

	// ignore if out of bounds
	if (offset.x + extent.width <= 0 || offset.y + extent.height <= 0 ||
		offset.x >= (TREE_Int)image->extent.width ||
		offset.y >= (TREE_Int)image->extent.height)
	{
		return TREE_OK;
	}

	// calculate sizes
	TREE_UInt otherOffsetX = offset.x < -otherOffset.x ? otherOffset.x - offset.x : otherOffset.x;
	TREE_UInt otherOffsetY = offset.y < -otherOffset.y ? otherOffset.y - offset.y : otherOffset.y;
	TREE_UInt offsetX = offset.x < 0 ? 0 : offset.x;
	TREE_UInt offsetY = offset.y < 0 ? 0 : offset.y;
	TREE_UInt width = MIN(extent.width, image->extent.width - offsetX);
	TREE_UInt height = MIN(extent.height, image->extent.height - offsetY);
	TREE_Size textCopySize = width * sizeof(TREE_Char);
	TREE_Size colorCopySize = width * sizeof(TREE_ColorPair);

	// draw the image
	TREE_UInt index, otherIndex;
	for (TREE_UInt row = 0; row < height; ++row)
	{
		// get indexes to "pixel"
		index = (row + offsetY) * image->extent.width + offsetX;
		otherIndex = (row + otherOffsetY) * extent.width + otherOffsetX;

		// copy data over from other
		memcpy(&image->text[index], &other->text[otherIndex], textCopySize);
		memcpy(&image->colors[index], &other->colors[otherIndex], colorCopySize);
	}

	return TREE_OK;
}

TREE_Result TREE_Image_DrawString(TREE_Image* image, TREE_Offset offset, TREE_String string, TREE_ColorPair colorPair)
{
	if (!image || !string)
	{
		return TREE_ERROR_ARG_NULL;
	}

	TREE_Size stringLength = strlen(string);

	// ignore if out of bounds
	if (offset.x + stringLength <= 0 || offset.y < 0 ||
		offset.x >= (TREE_Int)image->extent.width ||
		offset.y >= (TREE_Int)image->extent.height)
	{
		return TREE_OK;
	}

	// calculate sizes
	TREE_UInt stringOffsetX = offset.x < 0 ? -offset.x : 0;
	TREE_UInt offsetX = offset.x < 0 ? 0 : offset.x;
	TREE_UInt width = MIN((TREE_UInt)stringLength, image->extent.width - offsetX);

	// draw the string
	TREE_UInt index = offset.y * image->extent.width + offsetX;
	TREE_UInt stringIndex = stringOffsetX;
	memcpy(&image->text[index], &string[stringIndex], width * sizeof(TREE_Char));
	memset(&image->colors[index], colorPair, width * sizeof(TREE_Byte));

	return TREE_OK;
}

TREE_Result TREE_Image_DrawLine(TREE_Image* image, TREE_Offset start, TREE_Offset end, TREE_Pattern* pattern)
{
	// validate
	if (!image || !pattern)
	{
		return TREE_ERROR_ARG_NULL;
	}

	// calculate differences
	TREE_Int dx = abs(end.x - start.x);
	TREE_Int dy = abs(end.y - start.y);
	TREE_Int sx = (start.x < end.x) ? 1 : -1;
	TREE_Int sy = (start.y < end.y) ? 1 : -1;
	TREE_Int err = dx - dy;

	TREE_UInt patternIndex = 0;
	TREE_Pixel pixel;

	// draw the line
	while (1)
	{
		// draw the current point
		TREE_Pattern_Get(
			pattern,
			patternIndex,
			&pixel
		);
		TREE_Image_Set(
			image,
			start,
			pixel
		);

		// move to the next point in the pattern
		patternIndex = (patternIndex + 1) % pattern->size;

		// check if we've reached the end point
		if (start.x == end.x && start.y == end.y)
		{
			// done
			break;
		}

		// calculate the next point
		TREE_Int e2 = 2 * err;
		if (e2 > -dy)
		{
			err -= dy;
			start.x += sx;
		}
		if (e2 < dx)
		{
			err += dx;
			start.y += sy;
		}
	}

	return TREE_OK;
}

TREE_Result TREE_Image_DrawRect(TREE_Image* image, TREE_Offset start, TREE_Extent size, TREE_Pattern* pattern)
{
	// validate
	if (!image || !pattern)
	{
		return TREE_ERROR_ARG_NULL;
	}

	// draw lines for each side of the rectangle
	TREE_Offset p0 = start;
	TREE_Offset p1 = { start.x + (TREE_Int)size.width - 1, start.y };
	TREE_Offset p2 = { start.x + (TREE_Int)size.width - 1, start.y + (TREE_Int)size.height - 1 };
	TREE_Offset p3 = { start.x, start.y + (TREE_Int)size.height - 1 };

	// draw the lines
	TREE_Result result = TREE_Image_DrawLine(image, p0, p1, pattern);
	if (result)
	{
		return result;
	}
	result = TREE_Image_DrawLine(image, p1, p2, pattern);
	if (result)
	{
		return result;
	}
	result = TREE_Image_DrawLine(image, p2, p3, pattern);
	if (result)
	{
		return result;
	}
	result = TREE_Image_DrawLine(image, p3, p0, pattern);
	if (result)
	{
		return result;
	}

	return TREE_OK;
}

TREE_Result TREE_Image_FillRect(TREE_Image* image, TREE_Offset start, TREE_Extent size, TREE_Pixel pixel)
{
	// validate
	if (!image)
	{
		return TREE_ERROR_ARG_NULL;
	}
	if (size.width == 0 || size.height == 0)
	{
		return TREE_ERROR_ARG_OUT_OF_RANGE;
	}
	if (pixel.character == '\0')
	{
		return TREE_ERROR_ARG_INVALID;
	}

	// calculate bounds
	TREE_Int startX = MAX(start.x, 0);
	TREE_Int startY = MAX(start.y, 0);
	TREE_Int endX = MIN(start.x + (TREE_Int)size.width, (TREE_Int)image->extent.width);
	TREE_Int endY = MIN(start.y + (TREE_Int)size.height, (TREE_Int)image->extent.height);

	// fill the rectangle
	for (TREE_Int y = startY; y < endY; ++y)
	{
		for (TREE_Int x = startX; x < endX; ++x)
		{
			TREE_Offset offset = { x, y };
			TREE_Size index = _TREE_Image_GetIndex(image, offset);
			image->text[index] = pixel.character;
			image->colors[index] = pixel.colorPair;
		}
	}

	return TREE_OK;
}

TREE_Result TREE_Image_Clear(TREE_Image* image, TREE_Pixel pixel)
{
	// validate
	if (!image)
	{
		return TREE_ERROR_ARG_NULL;
	}
	if (pixel.character == '\0')
	{
		return TREE_ERROR_ARG_INVALID;
	}

	// fill the image with the pixel
	TREE_Size pixelCount = (TREE_Size)(image->extent.width * image->extent.height);
	memset(image->text, pixel.character, pixelCount * sizeof(TREE_Char));
	memset(image->colors, pixel.colorPair, pixelCount * sizeof(TREE_ColorPair));

	return TREE_OK;
}

TREE_Result TREE_Surface_Init(TREE_Surface* surface, TREE_Extent size)
{
	// validate
	if (!surface)
	{
		return TREE_ERROR_ARG_NULL;
	}
	if (size.width == 0 || size.height == 0)
	{
		return TREE_ERROR_ARG_OUT_OF_RANGE;
	}

	// initialize image
	TREE_Result code = TREE_Image_Init(&surface->image, size);
	if (code)
	{
		return code;
	}

	// initialize text
	TREE_Size textSize = (size.width * size.height + 1) * sizeof(TREE_Char); // +1 for null terminator
	surface->text = malloc(textSize);
	if (!surface->text)
	{
		TREE_Image_Free(&surface->image);
		return TREE_ERROR_ALLOC;
	}

	// set data
	memset(surface->text, ' ', textSize);
	surface->text[size.width * size.height] = '\0'; // null terminator

	return TREE_OK;
}

void TREE_Surface_Free(TREE_Surface* surface)
{
	// validate
	if (!surface)
	{
		return;
	}

	// free data
	TREE_Image_Free(&surface->image);
	if (surface->text)
	{
		free(surface->text);
		surface->text = NULL;
	}
}

TREE_Result TREE_Surface_Refresh(TREE_Surface* surface)
{
	if (!surface)
	{
		return TREE_ERROR_ARG_NULL;
	}

	// destroy old data, if any
	if (surface->text)
	{
		free(surface->text);
		surface->text = NULL;
	}

	TREE_Image* image = &surface->image;

	// count number of times the color changes
	TREE_Color lastFgColor = TREE_ColorPair_GetForeground(image->colors[0]) + 1;
	TREE_Color lastBgColor = TREE_ColorPair_GetBackground(image->colors[0]) + 1;
	TREE_Size fgCount = 1;
	TREE_Size bgCount = 1;
	TREE_Size pixelCount = (TREE_Size)(image->extent.width * image->extent.height);
	for (TREE_Size i = 1; i < pixelCount; ++i)
	{
		TREE_Color fgColor = TREE_ColorPair_GetForeground(image->colors[i]);
		TREE_Color bgColor = TREE_ColorPair_GetBackground(image->colors[i]);

		if (fgColor != lastFgColor)
		{
			lastFgColor = fgColor;
			++fgCount;
		}
		if (bgColor != lastBgColor)
		{
			lastBgColor = bgColor;
			++bgCount;
		}
	}

	// calculate total size of the final text string
	TREE_Size textSize = (pixelCount + 1) * sizeof(TREE_Char); // +1 for null terminator
	TREE_Size colorSize = (fgCount + bgCount + 1) * TREE_COLOR_STRING_LENGTH * sizeof(TREE_Char); // +1 for the reset
	TREE_Size totalSize = textSize + colorSize;

	// allocate data
	surface->text = (TREE_Char*)malloc(totalSize);
	if (!surface->text)
	{
		return TREE_ERROR_ALLOC;
	}

	// set data
	lastFgColor = TREE_ColorPair_GetForeground(image->colors[0]) + 1;
	lastBgColor = TREE_ColorPair_GetBackground(image->colors[0]) + 1;
	TREE_Size index = 0;
	for (TREE_Size i = 0; i < pixelCount; ++i)
	{
		TREE_ColorPair color = image->colors[i];
		TREE_Color fgColor = TREE_ColorPair_GetForeground(color);
		TREE_Color bgColor = TREE_ColorPair_GetBackground(color);

		// update colors
		if (fgColor != lastFgColor)
		{
			lastFgColor = fgColor;
			memcpy(&surface->text[index], TREE_Color_GetForegroundString(fgColor), TREE_COLOR_STRING_LENGTH * sizeof(TREE_Char));
			index += TREE_COLOR_STRING_LENGTH;
		}
		if (bgColor != lastBgColor)
		{
			lastBgColor = bgColor;
			memcpy(&surface->text[index], TREE_Color_GetBackgroundString(bgColor), TREE_COLOR_STRING_LENGTH * sizeof(TREE_Char));
			index += TREE_COLOR_STRING_LENGTH;
		}

		// copy character
		memcpy(&surface->text[index], &image->text[i], sizeof(TREE_Char));
		index++;
	}

	// add reset string
	memcpy(&surface->text[index], TREE_Color_GetResetString(), TREE_COLOR_STRING_LENGTH * sizeof(TREE_Char));
	index += TREE_COLOR_STRING_LENGTH;

	// add null terminator
	surface->text[index] = '\0';

	if (index >= totalSize)
	{
		free(surface->text);
		surface->text = NULL;
		return TREE_ERROR_OVERFLOW;
	}

	return TREE_OK;
}

TREE_Result TREE_Window_SetTitle(TREE_String title)
{
#ifdef TREE_WINDOWS
	if(!SetConsoleTitleA(title))
	{
		return TREE_ERROR_WINDOW_SET_TITLE;
	}
	return TREE_OK;
#else
	return TREE_NOT_IMPLEMENTED;
#endif // TREE_WINDOWS
}

TREE_Result TREE_Window_Present(TREE_Surface* surface)
{
	// validate
	if (!surface)
	{
		return TREE_ERROR_ARG_NULL;
	}
	if (!surface->text)
	{
		return TREE_ERROR_ARG_NULL;
	}

	// move the cursor to the top left corner
#ifdef TREE_WINDOWS
	{
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		COORD coord = { 0, 0 };
		SetConsoleCursorPosition(hConsole, coord);
	}
#endif // TREE_WINDOWS

	// print to the console
	int result = printf("%s", surface->text);
	if (result < 0)
	{
		return TREE_ERROR_PRESENTATION;
	}

	// flush the output
	result = fflush(stdout);
	if (result)
	{
		return TREE_ERROR_PRESENTATION;
	}

	return TREE_OK;
}

TREE_Extent TREE_Window_GetExtent()
{
	TREE_Extent extent;
	extent.width = 0;
	extent.height = 0;

#ifdef TREE_WINDOWS
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi))
	{
		extent.width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
		extent.height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
	}
#endif

	return extent;
}

TREE_Result TREE_Cursor_SetVisible(TREE_Bool visible)
{
#ifdef TREE_WINDOWS
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO cursorInfo;
	if (!GetConsoleCursorInfo(hConsole, &cursorInfo))
	{
		return TREE_ERROR_WINDOWS_CONSOLE_GET_CURSOR_INFO;
	}
	cursorInfo.bVisible = visible;
	if (!SetConsoleCursorInfo(hConsole, &cursorInfo))
	{
		return TREE_ERROR_WINDOWS_CONSOLE_SET_CURSOR_INFO;
	}

	return TREE_OK;
#else
	return TREE_NOT_IMPLEMENTED;
#endif
}

TREE_String TREE_Key_ToString(TREE_Key key)
{
	switch (key)
	{
	case TREE_KEY_NONE: return "None";
	case TREE_KEY_BACKSPACE: return "Backspace";
	case TREE_KEY_TAB: return "Tab";
	case TREE_KEY_ENTER: return "Enter";
	case TREE_KEY_SHIFT: return "Shift";
	case TREE_KEY_CONTROL: return "Control";
	case TREE_KEY_ALT: return "Alt";
	case TREE_KEY_PAUSE: return "Pause";
	case TREE_KEY_CAPS_LOCK: return "Caps Lock";
	case TREE_KEY_ESCAPE: return "Escape";
	case TREE_KEY_SPACE: return "Space";
	case TREE_KEY_PAGE_UP: return "Page Up";
	case TREE_KEY_PAGE_DOWN: return "Page Down";
	case TREE_KEY_END: return "End";
	case TREE_KEY_HOME: return "Home";
	case TREE_KEY_LEFT_ARROW: return "Left Arrow";
	case TREE_KEY_UP_ARROW: return "Up Arrow";
	case TREE_KEY_RIGHT_ARROW: return "Right Arrow";
	case TREE_KEY_DOWN_ARROW: return "Down Arrow";
	case TREE_KEY_PRINT_SCREEN: return "Print Screen";
	case TREE_KEY_INSERT: return "Insert";
	case TREE_KEY_DELETE: return "Delete";
	case TREE_KEY_0: return "0";
	case TREE_KEY_1: return "1";
	case TREE_KEY_2: return "2";
	case TREE_KEY_3: return "3";
	case TREE_KEY_4: return "4";
	case TREE_KEY_5: return "5";
	case TREE_KEY_6: return "6";
	case TREE_KEY_7: return "7";
	case TREE_KEY_8: return "8";
	case TREE_KEY_9: return "9";
	case TREE_KEY_A: return "A";
	case TREE_KEY_B: return "B";
	case TREE_KEY_C: return "C";
	case TREE_KEY_D: return "D";
	case TREE_KEY_E: return "E";
	case TREE_KEY_F: return "F";
	case TREE_KEY_G: return "G";
	case TREE_KEY_H: return "H";
	case TREE_KEY_I: return "I";
	case TREE_KEY_J: return "J";
	case TREE_KEY_K: return "K";
	case TREE_KEY_L: return "L";
	case TREE_KEY_M: return "M";
	case TREE_KEY_N: return "N";
	case TREE_KEY_O: return "O";
	case TREE_KEY_P: return "P";
	case TREE_KEY_Q: return "Q";
	case TREE_KEY_R: return "R";
	case TREE_KEY_S: return "S";
	case TREE_KEY_T: return "T";
	case TREE_KEY_U: return "U";
	case TREE_KEY_V: return "V";
	case TREE_KEY_W: return "W";
	case TREE_KEY_X: return "X";
	case TREE_KEY_Y: return "Y";
	case TREE_KEY_Z: return "Z";
	case TREE_KEY_NUMPAD_0: return "Numpad 0";
	case TREE_KEY_NUMPAD_1: return "Numpad 1";
	case TREE_KEY_NUMPAD_2: return "Numpad 2";
	case TREE_KEY_NUMPAD_3: return "Numpad 3";
	case TREE_KEY_NUMPAD_4: return "Numpad 4";
	case TREE_KEY_NUMPAD_5: return "Numpad 5";
	case TREE_KEY_NUMPAD_6: return "Numpad 6";
	case TREE_KEY_NUMPAD_7: return "Numpad 7";
	case TREE_KEY_NUMPAD_8: return "Numpad 8";
	case TREE_KEY_NUMPAD_9: return "Numpad 9";
	case TREE_KEY_MULTIPLY: return "Multiply";
	case TREE_KEY_ADD: return "Add";
	case TREE_KEY_SUBTRACT: return "Subtract";
	case TREE_KEY_DECIMAL: return "Decimal";
	case TREE_KEY_DIVIDE: return "Divide";
	case TREE_KEY_F1: return "F1";
	case TREE_KEY_F2: return "F2";
	case TREE_KEY_F3: return "F3";
	case TREE_KEY_F4: return "F4";
	case TREE_KEY_F5: return "F5";
	case TREE_KEY_F6: return "F6";
	case TREE_KEY_F7: return "F7";
	case TREE_KEY_F8: return "F8";
	case TREE_KEY_F9: return "F9";
	case TREE_KEY_F10: return "F10";
	case TREE_KEY_F11: return "F11";
	case TREE_KEY_F12: return "F12";
	case TREE_KEY_NUM_LOCK: return "Num Lock";
	case TREE_KEY_SCROLL_LOCK: return "Scroll Lock";
	case TREE_KEY_SEMICOLON: return "Semicolon";
	case TREE_KEY_EQUALS: return "Equals";
	case TREE_KEY_COMMA: return "Comma";
	case TREE_KEY_MINUS: return "Minus";
	case TREE_KEY_PERIOD: return "Period";
	case TREE_KEY_SLASH: return "Slash";
	case TREE_KEY_TILDE: return "Tilde";
	case TREE_KEY_LEFT_BRACKET: return "Left Bracket";
	case TREE_KEY_BACKSLASH: return "Backslash";
	case TREE_KEY_RIGHT_BRACKET: return "Right Bracket";
	case TREE_KEY_APOSTROPHE: return "Apostrophe";
	default: return "Unknown";
	}
}

TREE_CharType TREE_Char_GetType(TREE_Char character)
{
	// letters
	if (isalpha(character))
	{
		return TREE_CHAR_TYPE_LETTER;
	}

	// numbers
	if (isdigit(character))
	{
		return TREE_CHAR_TYPE_NUMBER;
	}

	// whitespace
	if (isspace(character))
	{
		return TREE_CHAR_TYPE_WHITESPACE;
	}

	// symbols
	if (ispunct(character))
	{
		return TREE_CHAR_TYPE_SYMBOL;
	}

	return TREE_CHAR_TYPE_NONE;
}

TREE_Char TREE_Key_ToChar(TREE_Key key, TREE_KeyModifierFlags modifiers)
{
	TREE_Bool isShiftPressed = (modifiers & TREE_KEY_MODIFIER_FLAGS_SHIFT) != 0;
	TREE_Bool isCapsLockOn = (modifiers & TREE_KEY_MODIFIER_FLAGS_CAPS_LOCK) != 0;

	// handle alphabetic characters
	if (key >= TREE_KEY_A && key <= TREE_KEY_Z)
	{
		TREE_Bool isUpperCase = isShiftPressed ^ isCapsLockOn; // XOR: Shift or Caps Lock toggles case
		return isUpperCase ? (TREE_Char)key : (TREE_Char)(key + 32); // Convert to lowercase if not uppercase
	}

	// handle number keys (0-9) and their shifted symbols
	if (key >= TREE_KEY_0 && key <= TREE_KEY_9)
	{
		if (isShiftPressed)
		{
			// Shifted symbols for number keys
			static const TREE_Char shiftedSymbols[] = { ')', '!', '@', '#', '$', '%', '^', '&', '*', '(' };
			return shiftedSymbols[key - TREE_KEY_0];
		}
		return (TREE_Char)key;
	}

	// handle special characters and symbols
	if (key >= TREE_KEY_SEMICOLON && key <= TREE_KEY_TILDE)
	{
		static const TREE_Char normalChars[] = {
			';', '=', ',', '-', '.', '/', '`'
		};
		static const TREE_Char shiftedChars[] = {
			':', '+', '<', '_', '>', '?', '~'
		};
		TREE_Size index = key - TREE_KEY_SEMICOLON;
		return isShiftPressed ? shiftedChars[index] : normalChars[index];
	}
	if (key >= TREE_KEY_LEFT_BRACKET && key <= TREE_KEY_APOSTROPHE)
	{
		static const TREE_Char normalChars[] = { '[', '\\', ']', '\'', };
		static const TREE_Char shiftedChars[] = { '{', '|', '}', '"', };
		TREE_Size index = key - TREE_KEY_LEFT_BRACKET;
		return isShiftPressed ? shiftedChars[index] : normalChars[index];
	}

	// handle numpad keys (if Num Lock is on)
	if (key >= TREE_KEY_NUMPAD_0 && key <= TREE_KEY_NUMPAD_9)
	{
		TREE_Bool isNumLockOn = (modifiers & TREE_KEY_MODIFIER_FLAGS_NUM_LOCK) != 0;
		if (isNumLockOn)
		{
			return (TREE_Char)(key - TREE_KEY_NUMPAD_0 + '0'); // Convert to corresponding number
		}
	}
	if (key >= TREE_KEY_MULTIPLY && key <= TREE_KEY_DIVIDE)
	{
		static const TREE_Char numpadSymbols[] = { '*', '+', ' ', '-', '.', '/' };
		return numpadSymbols[key - TREE_KEY_MULTIPLY];
	}

	// handle other keys
	switch (key)
	{
	case TREE_KEY_SPACE: return ' ';
	case TREE_KEY_TAB: return '\t';
	case TREE_KEY_BACKSPACE: return '\b';
	case TREE_KEY_ENTER: return '\n';
	case TREE_KEY_ESCAPE: return '\033';
	default: return '\0'; // Return null for unhandled keys
	}
}

TREE_Result TREE_Input_Init(TREE_Input* input)
{
	// validate
	if (!input)
	{
		return TREE_ERROR_ARG_NULL;
	}

	// set data  
	input->keys[0] = TREE_KEY_TAB;
	input->keys[1] = TREE_KEY_BACKSPACE;
	input->keys[2] = TREE_KEY_SHIFT;
	input->keys[3] = TREE_KEY_ENTER;
	input->keys[4] = TREE_KEY_CONTROL;
	input->keys[5] = TREE_KEY_ALT;
	input->keys[6] = TREE_KEY_PAUSE;
	input->keys[7] = TREE_KEY_CAPS_LOCK;
	input->keys[8] = TREE_KEY_ESCAPE;
	input->keys[9] = TREE_KEY_SPACE;
	input->keys[10] = TREE_KEY_PAGE_UP;
	input->keys[11] = TREE_KEY_PAGE_DOWN;
	input->keys[12] = TREE_KEY_END;
	input->keys[13] = TREE_KEY_HOME;
	input->keys[14] = TREE_KEY_LEFT_ARROW;
	input->keys[15] = TREE_KEY_UP_ARROW;
	input->keys[16] = TREE_KEY_RIGHT_ARROW;
	input->keys[17] = TREE_KEY_DOWN_ARROW;
	input->keys[18] = TREE_KEY_PRINT_SCREEN;
	input->keys[19] = TREE_KEY_INSERT;
	input->keys[20] = TREE_KEY_DELETE;
	input->keys[21] = TREE_KEY_0;
	input->keys[22] = TREE_KEY_1;
	input->keys[23] = TREE_KEY_2;
	input->keys[24] = TREE_KEY_3;
	input->keys[25] = TREE_KEY_4;
	input->keys[26] = TREE_KEY_5;
	input->keys[27] = TREE_KEY_6;
	input->keys[28] = TREE_KEY_7;
	input->keys[29] = TREE_KEY_8;
	input->keys[30] = TREE_KEY_9;
	input->keys[31] = TREE_KEY_A;
	input->keys[32] = TREE_KEY_B;
	input->keys[33] = TREE_KEY_C;
	input->keys[34] = TREE_KEY_D;
	input->keys[35] = TREE_KEY_E;
	input->keys[36] = TREE_KEY_F;
	input->keys[37] = TREE_KEY_G;
	input->keys[38] = TREE_KEY_H;
	input->keys[39] = TREE_KEY_I;
	input->keys[40] = TREE_KEY_J;
	input->keys[41] = TREE_KEY_K;
	input->keys[42] = TREE_KEY_L;
	input->keys[43] = TREE_KEY_M;
	input->keys[44] = TREE_KEY_N;
	input->keys[45] = TREE_KEY_O;
	input->keys[46] = TREE_KEY_P;
	input->keys[47] = TREE_KEY_Q;
	input->keys[48] = TREE_KEY_R;
	input->keys[49] = TREE_KEY_S;
	input->keys[50] = TREE_KEY_T;
	input->keys[51] = TREE_KEY_U;
	input->keys[52] = TREE_KEY_V;
	input->keys[53] = TREE_KEY_W;
	input->keys[54] = TREE_KEY_X;
	input->keys[55] = TREE_KEY_Y;
	input->keys[56] = TREE_KEY_Z;
	input->keys[57] = TREE_KEY_F1;
	input->keys[58] = TREE_KEY_F2;
	input->keys[59] = TREE_KEY_F3;
	input->keys[60] = TREE_KEY_F4;
	input->keys[61] = TREE_KEY_F5;
	input->keys[62] = TREE_KEY_F6;
	input->keys[63] = TREE_KEY_F7;
	input->keys[64] = TREE_KEY_F8;
	input->keys[65] = TREE_KEY_F9;
	input->keys[66] = TREE_KEY_F10;
	input->keys[67] = TREE_KEY_F11;
	input->keys[68] = TREE_KEY_F12;
	input->keys[69] = TREE_KEY_NUM_LOCK;
	input->keys[70] = TREE_KEY_SCROLL_LOCK;
	input->keys[71] = TREE_KEY_NUMPAD_0;
	input->keys[72] = TREE_KEY_NUMPAD_1;
	input->keys[73] = TREE_KEY_NUMPAD_2;
	input->keys[74] = TREE_KEY_NUMPAD_3;
	input->keys[75] = TREE_KEY_NUMPAD_4;
	input->keys[76] = TREE_KEY_NUMPAD_5;
	input->keys[77] = TREE_KEY_NUMPAD_6;
	input->keys[78] = TREE_KEY_NUMPAD_7;
	input->keys[79] = TREE_KEY_NUMPAD_8;
	input->keys[80] = TREE_KEY_NUMPAD_9;
	input->keys[81] = TREE_KEY_MULTIPLY;
	input->keys[82] = TREE_KEY_ADD;
	input->keys[83] = TREE_KEY_SUBTRACT;
	input->keys[84] = TREE_KEY_DECIMAL;
	input->keys[85] = TREE_KEY_DIVIDE;
	input->keys[86] = TREE_KEY_SEMICOLON;
	input->keys[87] = TREE_KEY_EQUALS;
	input->keys[88] = TREE_KEY_COMMA;
	input->keys[89] = TREE_KEY_MINUS;
	input->keys[90] = TREE_KEY_PERIOD;
	input->keys[91] = TREE_KEY_SLASH;
	input->keys[92] = TREE_KEY_TILDE;
	input->keys[93] = TREE_KEY_LEFT_BRACKET;
	input->keys[94] = TREE_KEY_BACKSLASH;
	input->keys[95] = TREE_KEY_RIGHT_BRACKET;
	input->keys[96] = TREE_KEY_APOSTROPHE;

	for (TREE_Size i = 0; i < TREE_KEY_MAX; i++)
	{
		input->states[i] = TREE_INPUT_STATE_RELEASED;
	}

	input->modifiers = TREE_KEY_MODIFIER_FLAGS_NONE;

	return TREE_OK;
}

void TREE_Input_Free(TREE_Input* input)
{
	for (TREE_Size i = 0; i < TREE_KEY_COUNT; i++)
	{
		input->keys[i] = TREE_KEY_NONE;
	}
	for (TREE_Size i = 0; i < TREE_KEY_MAX; i++)
	{
		input->states[i] = TREE_INPUT_STATE_RELEASED;
	}
	input->modifiers = TREE_KEY_MODIFIER_FLAGS_NONE;
}

TREE_Direction TREE_Direction_Opposite(TREE_Direction direction)
{
	// if none, no opposite
	if (direction == TREE_DIRECTION_NONE)
	{
		return TREE_DIRECTION_NONE;
	}

	// add 2 to invert it
	return (TREE_Direction)(((TREE_Size)direction + 1) % 4 + 1);
}

TREE_Result TREE_Transform_Init(TREE_Transform* transform, TREE_Offset localOffset, TREE_Coords localPivot, TREE_Extent localExtent, TREE_Alignment localAlignment)
{
	// validate
	if (!transform)
	{
		return TREE_ERROR_ARG_NULL;
	}

	// set data
	transform->localOffset = localOffset;
	transform->localPivot = localPivot;
	transform->localExtent = localExtent;
	transform->localAlignment = localAlignment;
	transform->parent = NULL;
	transform->child = NULL;
	transform->sibling = NULL;
	transform->dirty = TREE_TRUE;
	transform->globalRect.offset.x = 0;
	transform->globalRect.offset.y = 0;
	transform->globalRect.extent.width = 0;
	transform->globalRect.extent.height = 0;

	return TREE_OK;
}

void TREE_Transform_Free(TREE_Transform* transform)
{
	// validate
	if (!transform)
	{
		return;
	}

	// remove from family
	TREE_Transform_SetParent(transform, NULL);

	// disconnect children
	TREE_Transform_DisconnectChildren(transform);

	// free data
	transform->localOffset.x = 0;
	transform->localOffset.y = 0;
	transform->localPivot.x = 0.0f;
	transform->localPivot.y = 0.0f;
	transform->localExtent.width = 0;
	transform->localExtent.height = 0;
	transform->localAlignment = TREE_ALIGNMENT_NONE;
	transform->parent = NULL;
	transform->child = NULL;
	transform->sibling = NULL;
	transform->dirty = TREE_FALSE;
	transform->globalRect.offset.x = 0;
	transform->globalRect.offset.y = 0;
	transform->globalRect.extent.width = 0;
	transform->globalRect.extent.height = 0;
}

TREE_Result TREE_Transform_Dirty(TREE_Transform* transform)
{
	// validate
	if (!transform)
	{
		return TREE_ERROR_ARG_NULL;
	}

	// mark as dirty
	transform->dirty = TREE_TRUE;

	// mark children as dirty
	TREE_Transform* child = transform->child;
	while (child)
	{
		TREE_Transform_Dirty(child);
		child = child->sibling;
	}

	return TREE_OK;
}

TREE_Result TREE_Transform_SetParent(TREE_Transform* transform, TREE_Transform* parent)
{
	// validate
	if (!transform)
	{
		return TREE_ERROR_ARG_NULL;
	}

	// ignore if no change
	if (transform->parent == parent)
	{
		return TREE_OK;
	}

	// unconnect from old family
	if (transform->parent)
	{
		TREE_Transform* firstSibling = transform->parent->child;

		if (firstSibling == transform)
		{
			// if this is the first child, set the parent's child to the next sibling
			transform->parent->child = transform->sibling;
		}
		else
		{
			// find the previous sibling
			TREE_Transform* sibling = firstSibling;
			while (sibling && sibling->sibling != transform)
			{
				sibling = sibling->sibling;
			}
			// connect to next sibling
			if (sibling)
			{
				sibling->sibling = transform->sibling;
			}
		}
	}

	// set new parent
	transform->sibling = NULL;
	transform->parent = parent;

	// connect to new family
	if (parent)
	{
		// if the parent has no children, set this as the first child
		if (!parent->child)
		{
			parent->child = transform;
		}
		else
		{
			// otherwise, find the last sibling and set this as the next sibling
			TREE_Transform* lastSibling = parent->child;
			while (lastSibling->sibling)
			{
				lastSibling = lastSibling->sibling;
			}
			lastSibling->sibling = transform;
		}
	}

	// mark as dirty
	return TREE_Transform_Dirty(transform);
}

TREE_Result TREE_Transform_DisconnectChildren(TREE_Transform* transform)
{
	// validate
	if (!transform)
	{
		return TREE_ERROR_ARG_NULL;
	}

	// if no children, nothing to do
	if (!transform->child)
	{
		return TREE_OK;
	}

	// disconnect all children from this parent, and each sibling
	TREE_Transform* child = transform->child;
	while (child)
	{
		TREE_Transform* nextSibling = child->sibling;
		child->parent = NULL; // disconnect from parent
		child->sibling = NULL; // disconnect from siblings
		TREE_Result result = TREE_Transform_Dirty(child);
		if (result)
		{
			return result;
		}
		child = nextSibling; // move to next sibling
	}

	// set child to NULL
	transform->child = NULL;

	// mark as dirty
	TREE_Transform_Dirty(transform);

	return TREE_OK;
}

TREE_Result TREE_Transform_Refresh(TREE_Transform* transform)
{
	// validate
	if (!transform)
	{
		return TREE_ERROR_ARG_NULL;
	}

	// calculate global rectangle

	// parent: use parent data
	TREE_Transform* parent = transform->parent;
	if (parent)
	{
		TREE_Alignment alignment = transform->localAlignment;
		TREE_Rect result;

		// top<-->bottom
		if (alignment & TREE_ALIGNMENT_VERTICALSTRETCH)
		{
			result.offset.y = parent->globalRect.offset.y - transform->localOffset.y;
			result.extent.height = parent->globalRect.extent.height + transform->localOffset.y * 2;
		}
		else if (alignment & TREE_ALIGNMENT_TOP)
		{
			result.offset.y = parent->globalRect.offset.y + transform->localOffset.y - (TREE_Int)(transform->localPivot.y * transform->localExtent.height);
			result.extent.height = transform->localExtent.height;
		}
		else if (alignment & TREE_ALIGNMENT_BOTTOM)
		{
			result.offset.y = parent->globalRect.offset.y + parent->globalRect.extent.height + transform->localOffset.y - (TREE_Int)(transform->localPivot.y * transform->localExtent.height);
			result.extent.height = transform->localExtent.height;
		}
		else
		{
			result.offset.y = parent->globalRect.offset.y + (parent->globalRect.extent.height + transform->localExtent.height) / 2 + transform->localOffset.y - (TREE_Int)(transform->localPivot.y * transform->localExtent.height);
			result.extent.height = transform->localExtent.height;
		}

		// left<-->right
		if (alignment & TREE_ALIGNMENT_HORIZONTALSTRETCH)
		{
			result.offset.x = parent->globalRect.offset.x - transform->localOffset.x;
			result.extent.width = parent->globalRect.extent.width + transform->localOffset.x * 2;
		}
		else if (alignment & TREE_ALIGNMENT_LEFT)
		{
			result.offset.x = parent->globalRect.offset.x + transform->localOffset.x - (TREE_Int)(transform->localPivot.x * transform->localExtent.width);
			result.extent.width = transform->localExtent.width;
		}
		else if (alignment & TREE_ALIGNMENT_RIGHT)
		{
			result.offset.x = parent->globalRect.offset.x + parent->globalRect.extent.width + transform->localOffset.x - (TREE_Int)(transform->localPivot.x * transform->localExtent.width);
			result.extent.width = transform->localExtent.width;
		}
		else
		{
			result.offset.x = parent->globalRect.offset.x + (parent->globalRect.extent.width + transform->localExtent.width) / 2 + transform->localOffset.x - (TREE_Int)(transform->localPivot.x * transform->localExtent.width);
			result.extent.width = transform->localExtent.width;
		}

		// set global rectangle
		transform->globalRect = result;

		return TREE_OK;
	}

	// no parent: use local data
	transform->globalRect.offset.y = transform->localOffset.y - (TREE_Int)(transform->localPivot.y * transform->localExtent.height);
	transform->globalRect.offset.x = transform->localOffset.x - (TREE_Int)(transform->localPivot.x * transform->localExtent.width);
	transform->globalRect.extent = transform->localExtent;

	return TREE_OK;
}

TREE_Result TREE_Control_Init(TREE_Control* control, TREE_Transform* parent, TREE_EventHandler eventHandler, TREE_Data data)
{
	// validate
	if (!control || !eventHandler)
	{
		return TREE_ERROR_ARG_NULL;
	}

	TREE_Result result;

	// allocate data
	control->transform = TREE_NEW(TREE_Transform);
	if (!control->transform)
	{
		return TREE_ERROR_ALLOC;
	}
	control->image = TREE_NEW(TREE_Image);
	if (!control->image)
	{
		TREE_DELETE(control->transform);
		return TREE_ERROR_ALLOC;
	}

	// set data
	control->type = TREE_CONTROL_TYPE_NONE;
	control->flags = TREE_CONTROL_FLAGS_NONE;
	control->stateFlags = TREE_CONTROL_STATE_FLAGS_DIRTY;
	result = TREE_Transform_Init(control->transform, (TREE_Offset) { 0, 0 }, (TREE_Coords) { 0.0f, 0.0f }, (TREE_Extent) { 0, 0 }, TREE_ALIGNMENT_TOPLEFT);
	if (result)
	{
		TREE_DELETE(control->image);
		TREE_DELETE(control->transform);
		return result;
	}
	result = TREE_Image_Init(control->image, (TREE_Extent) { 0, 0 });
	if (result)
	{
		TREE_Transform_Free(control->transform);
		TREE_DELETE(control->image);
		TREE_DELETE(control->transform);
		return result;
	}
	memset(control->adjacent, 0, 4 * sizeof(TREE_Control*));
	control->eventHandler = eventHandler;
	control->data = data;

	if (parent)
	{
		TREE_Result result = TREE_Transform_SetParent(control->transform, parent);
		if (result)
		{
			return result;
		}
	}

	return TREE_OK;
}

void TREE_Control_Free(TREE_Control* control)
{
	if (!control)
	{
		return;
	}

	// free data
	control->type = TREE_CONTROL_TYPE_NONE;
	control->flags = TREE_CONTROL_FLAGS_NONE;
	control->stateFlags = TREE_CONTROL_STATE_FLAGS_NONE;
	TREE_Transform_Free(control->transform);
	TREE_DELETE(control->transform);
	TREE_Image_Free(control->image);
	TREE_DELETE(control->image);
	memset(control->adjacent, 0, 4 * sizeof(TREE_Control*));
	control->eventHandler = NULL;
	control->data = NULL;
}

TREE_Result TREE_Control_Link(TREE_Control* control, TREE_Direction direction, TREE_ControlLink link, TREE_Control* other)
{
	// validate
	if (!control)
	{
		return TREE_ERROR_ARG_NULL;
	}
	if (direction == TREE_DIRECTION_NONE)
	{
		return TREE_ERROR_ARG_INVALID;
	}
	if (link != TREE_CONTROL_LINK_NONE && !other)
	{
		return TREE_ERROR_ARG_NULL;
	}
	if (!(other->flags & TREE_CONTROL_FLAGS_FOCUSABLE))
	{
		return TREE_ERROR_ARG_INVALID;
	}

	// calculate indices
	TREE_Size index = (TREE_Size)direction - 1;
	TREE_Direction opposite = TREE_Direction_Opposite(direction);
	TREE_Size oppositeIndex = (TREE_Size)opposite - 1;
	TREE_Control* old = control->adjacent[index];

	// perform linking
	switch (link)
	{
	case TREE_CONTROL_LINK_NONE:
		if (old && old->adjacent[oppositeIndex] == control)
		{
			old->adjacent[oppositeIndex] = NULL;
		}
		control->adjacent[index] = NULL;
		break;
	case TREE_CONTROL_LINK_SINGLE:
		control->adjacent[index] = other;
		break;
	case TREE_CONTROL_LINK_DOUBLE:
		control->adjacent[index] = other;
		other->adjacent[oppositeIndex] = control;
		break;
	case TREE_CONTROL_LINK_UNIQUE:
		if (old && old->adjacent[oppositeIndex] == control)
		{
			old->adjacent[oppositeIndex] = NULL;
		}
		control->adjacent[index] = other;
		other->adjacent[oppositeIndex] = control;
		break;
	default:
		return TREE_NOT_IMPLEMENTED;
	}

	return TREE_OK;
}

TREE_Result TREE_Control_HandleEvent(TREE_Control* control, TREE_Event const* event)
{
	// validate
	if (!control || !event)
	{
		return TREE_ERROR_ARG_NULL;
	}

	// if event handler, call it
	if (control->eventHandler)
	{
		return control->eventHandler(event);
	}

	// no event handler, so ignore the event
	return TREE_OK;
}

TREE_Size _TREE_ClampScroll(TREE_Size scroll, TREE_Size offset, TREE_Size extent)
{
	// adjust scroll
	if (scroll + extent <= offset)
	{
		// scroll towards END
		return offset + 1 - extent;
	}
	else if (scroll > offset)
	{
		// scroll towards HOME
		return offset;
	}
	return scroll;
}

TREE_Size _TREE_WordWrapPass(TREE_String text, TREE_Size width, TREE_Char*** result, TREE_Size lineCount)
{
	if (!text || !*text || width == 0)
	{
		return 0;
	}

	// allocate lines, if count given
	TREE_Char** lines = NULL;
	if (result && lineCount > 0)
	{
		lines = TREE_NEW_ARRAY(TREE_Char*, lineCount);
		if (!lines)
		{
			return 0;
		}
	}

	// get number of lines
	TREE_Size count = 0;
	TREE_Size lastSpace = 0;
	TREE_Size lastLine = 0;
	TREE_Size textLength = strlen(text);
	TREE_Char ch;
	for (TREE_Size i = 0; i < textLength; i++)
	{
		ch = text[i];

		if (ch == '\n')
		{
			// normal line end

			// add to lines
			if (lines && count < lineCount)
			{
				TREE_Size lineLength = i - lastLine + 1;
				TREE_Char* line = TREE_NEW_ARRAY(TREE_Char, lineLength + 1);
				lines[count] = line;
				if (!line)
				{
					for (TREE_Size j = 0; j < count; j++)
					{
						TREE_DELETE(lines[j]);
					}
					TREE_DELETE(lines);
					return 0;
				}
				memcpy(line, &text[lastLine], lineLength);
				line[lineLength] = '\0'; // null terminator
			}

			// update markers
			count++;
			lastLine = i + 1;
			lastSpace = lastLine;
		}
		else if (i - lastLine + 1 > width)
		{
			// reached max line width

			// go back to last space, if it is not the last line
			if (lastSpace != lastLine)
			{
				i = lastSpace;
			}

			// add to lines
			if (lines && count < lineCount)
			{
				TREE_Size lineLength = i - lastLine + 1;
				TREE_Char* line = TREE_NEW_ARRAY(TREE_Char, lineLength + 1);
				lines[count] = line;
				if (!line)
				{
					for (TREE_Size j = 0; j < count; j++)
					{
						TREE_DELETE(lines[j]);
					}
					TREE_DELETE(lines);
					return 0;
				}
				memcpy(line, &text[lastLine], lineLength);
				line[lineLength] = '\0'; // null terminator
			}

			// update markers
			count++;
			lastLine = i + 1;
			lastSpace = lastLine;
		}
		else if (isspace(ch))
		{
			// space found
			lastSpace = i;
		}
	}

	// add last line
	if (lastLine < textLength)
	{
		// add to lines
		if (lines && count < lineCount)
		{
			TREE_Size lineLength = textLength - lastLine + 1;
			TREE_Char* line = TREE_NEW_ARRAY(TREE_Char, lineLength + 1);
			lines[count] = line;
			if (!line)
			{
				for (TREE_Size j = 0; j < count; j++)
				{
					TREE_DELETE(lines[j]);
				}
				TREE_DELETE(lines);
				return 0;
			}
			memcpy(line, &text[lastLine], lineLength);
			line[lineLength] = '\0'; // null terminator
		}
		count++;
	}

	// if last line ends a line end, add an empty line
	if (text[textLength - 1] == '\n')
	{
		// add to lines
		if (lines)
		{
			lines[count] = TREE_NEW_ARRAY(TREE_Char, 1);
			if (!lines[count])
			{
				for (TREE_Size j = 0; j < count; j++)
				{
					TREE_DELETE(lines[j]);
				}
				TREE_DELETE(lines);
				return 0;
			}
			lines[count][0] = '\0'; // empty line
		}
		count++;
	}

	if (result)
	{
		*result = lines;
	}
	return count;
}

TREE_Result _TREE_WordWrap(TREE_String text, TREE_Size width, TREE_Char*** lines, TREE_Size* lineCount)
{
	*lines = NULL;
	*lineCount = _TREE_WordWrapPass(text, width, lines, 0);
	if (*lineCount == 0)
	{
		return TREE_OK;
	}
	_TREE_WordWrapPass(text, width, lines, *lineCount);
	if (!*lines)
	{
		return TREE_ERROR_WORD_WRAPPING;
	}
	return TREE_OK;
}

TREE_Size* _TREE_LineOffsets(TREE_String* lines, TREE_Size lineCount)
{
	TREE_Size* result = TREE_NEW_ARRAY(TREE_Size, lineCount);
	if (!result)
	{
		return NULL;
	}
	TREE_Size offset = 0;
	for (TREE_Size i = 0; i < lineCount; i++)
	{
		result[i] = offset;
		offset += strlen(lines[i]);
	}
	return result;
}

TREE_Result _TREE_WordWrapAndOffsets(TREE_String text, TREE_Size width, TREE_Char*** lines, TREE_Size* lineCount, TREE_Size** lineOffsets)
{
	// word wrap the text
	*lineCount = _TREE_WordWrapPass(text, width, NULL, 0);

	// if no count
	if (!*lineCount)
	{
		*lines = NULL;
		*lineOffsets = NULL;
		return TREE_OK;
	}

	_TREE_WordWrapPass(text, width, lines, *lineCount);
	if (!*lines)
	{
		*lineOffsets = NULL;
		*lineCount = 0;
		return TREE_ERROR_WORD_WRAPPING;
	}

	// get the index offsets for each line
	*lineOffsets = _TREE_LineOffsets(*lines, *lineCount);
	if (!*lineOffsets)
	{
		TREE_DELETE_ARRAY(*lines, *lineCount);
		*lineCount = 0;
		return TREE_ERROR_WORD_WRAPPING_OFFSETS;
	}

	return TREE_OK;
}

TREE_Offset _TREE_CalculateCursorOffset(TREE_Size cursorPosition, TREE_Size* lineOffsets, TREE_Size lineCount)
{
	// if no lines, return 0
	if (!lineCount)
	{
		return (TREE_Offset) { 0, 0 };
	}

	TREE_Offset result;
	result.y = (TREE_Int)lineCount - 1;
	TREE_Size offset;
	for (TREE_Size i = 1; i < lineCount; i++)
	{
		offset = lineOffsets[i];
		if (offset > cursorPosition)
		{
			result.y = (TREE_Int)i - 1;
			break;
		}
	}
	result.x = (TREE_Int)(cursorPosition - lineOffsets[result.y]);
	return result;
}

TREE_Result _TREE_Control_Refresh_Text(TREE_Image* target, TREE_Offset controlOffset, TREE_Extent controlExtent, TREE_String text, TREE_Alignment alignment, TREE_Pixel design)
{
	TREE_Result result;

	// resize image if needed
	if (controlExtent.width != target->extent.width ||
		controlExtent.height != target->extent.height)
	{
		TREE_Image_Free(target);
		result = TREE_Image_Init(target, controlExtent);
		if (result)
		{
			return result;
		}
	}

	// draw onto the image
	// draw the rect
	result = TREE_Image_Clear(target, design);
	if (result)
	{
		return result;
	}

	TREE_Char** lines;
	TREE_Size lineCount = _TREE_WordWrapPass(text, controlExtent.width, NULL, 0);
	_TREE_WordWrapPass(text, controlExtent.width, &lines, lineCount);
	if (!lines)
	{
		return result;
	}

	// draw each line, using the alignment to determine the position within the rect
	TREE_Int top;
	TREE_Offset offset;
	if (alignment & TREE_ALIGNMENT_TOP)
	{
		top = 0;
	}
	else if (alignment & TREE_ALIGNMENT_CENTER)
	{
		top = (TREE_Int)(controlExtent.height - lineCount) / 2;
	}
	else
	{
		top = (TREE_Int)(controlExtent.height - lineCount);
	}

	if (alignment & TREE_ALIGNMENT_LEFT)
	{
		offset.x = 0;
		for (TREE_Int i = 0; i < lineCount; i++)
		{
			offset.y = top + i;
			result = TREE_Image_DrawString(
				target,
				offset,
				lines[i],
				design.colorPair
			);
			if (result)
			{
				break;
			}
		}
	}
	else if (alignment & TREE_ALIGNMENT_CENTER)
	{
		for (TREE_Int i = 0; i < lineCount; i++)
		{
			offset.x = (TREE_Int)(controlExtent.width - strlen(lines[i])) / 2;
			offset.y = top + i;
			result = TREE_Image_DrawString(
				target,
				offset,
				lines[i],
				design.colorPair
			);
			if (result)
			{
				break;
			}
		}
	}
	else
	{
		for (TREE_Int i = 0; i < lineCount; i++)
		{
			offset.x = (TREE_Int)(controlExtent.width - strlen(lines[i]));
			offset.y = top + i;
			result = TREE_Image_DrawString(
				target,
				offset,
				lines[i],
				design.colorPair
			);
			if (result)
			{
				break;
			}
		}
	}

	// free all the lines
	for (TREE_Size i = 0; i < lineCount; i++)
	{
		TREE_DELETE(lines[i]);
	}
	TREE_DELETE(lines);

	// check for result: would be an error if broke out of loop early
	if (result)
	{
		return result;
	}

	return TREE_OK;
}

TREE_Result TREE_Control_LabelData_Init(TREE_Control_LabelData* data, TREE_String text)
{
	// validate
	if (!data || !text)
	{
		return TREE_ERROR_ARG_NULL;
	}

	// allocate data
	TREE_Size textLength = strlen(text);
	TREE_Size textSize = (textLength + 1) * sizeof(TREE_Char); // +1 for null terminator
	data->text = TREE_NEW_ARRAY(TREE_Char, textSize);
	if (!data->text)
	{
		return TREE_ERROR_ALLOC;
	}

	// set data
	memcpy(data->text, text, textSize);
	data->text[textLength] = '\0'; // null terminator
	data->alignment = TREE_ALIGNMENT_TOPLEFT;
	data->normal.character = ' ';
	data->normal.colorPair = TREE_ColorPair_Create(TREE_COLOR_WHITE, TREE_COLOR_BLACK);

	return TREE_OK;
}

void TREE_Control_LabelData_Free(TREE_Control_LabelData* data)
{
	// validate
	if (!data)
	{
		return;
	}

	// free data
	TREE_DELETE(data->text);
	data->alignment = TREE_ALIGNMENT_NONE;
	data->normal.character = ' ';
	data->normal.colorPair = TREE_ColorPair_CreateDefault();
}

TREE_Result TREE_Control_Label_Init(TREE_Control* control, TREE_Transform* parent, TREE_Control_LabelData* data)
{
	// validate
	if (!control || !data)
	{
		return TREE_ERROR_ARG_NULL;
	}

	// initialize control
	TREE_Result result = TREE_Control_Init(control, parent, TREE_Control_Label_EventHandler, data);
	if (result)
	{
		return result;
	}

	// set data
	control->transform->localExtent.width = (TREE_UInt)strlen(data->text);
	control->transform->localExtent.height = 1;
	control->type = TREE_CONTROL_TYPE_LABEL;

	return TREE_OK;
}

TREE_Result TREE_Control_Label_SetText(TREE_Control* control, TREE_String text)
{
	// validate
	if (!control || !text)
	{
		return TREE_ERROR_ARG_NULL;
	}
	if (control->type != TREE_CONTROL_TYPE_LABEL)
	{
		return TREE_ERROR_ARG_INVALID;
	}

	// get data
	TREE_Control_LabelData* labelData = (TREE_Control_LabelData*)control->data;

	// allocate new text
	TREE_Size textLength = strlen(text);
	TREE_Size textSize = (textLength + 1) * sizeof(TREE_Char); // +1 for null terminator
	TREE_REPLACE(labelData->text, TREE_NEW_ARRAY(TREE_Char, textLength + 1));
	if (!labelData->text)
	{
		return TREE_ERROR_ALLOC;
	}

	// set data
	memcpy(labelData->text, text, textSize);

	// mark as dirty to get redrawn
	control->stateFlags |= TREE_CONTROL_STATE_FLAGS_DIRTY;

	return TREE_OK;
}

TREE_String TREE_Control_Label_GetText(TREE_Control* control)
{
	// validate
	if (!control)
	{
		return NULL;
	}
	if (control->type != TREE_CONTROL_TYPE_LABEL)
	{
		return NULL;
	}

	// get data
	TREE_Control_LabelData* labelData = (TREE_Control_LabelData*)control->data;

	// return text
	return labelData->text;
}

TREE_Result _TREE_Control_Draw(TREE_Image* target, TREE_Rect const* dirtyRect, TREE_Rect const* globalRect, TREE_Image const* image)
{
	TREE_Result result;

	// get the intersecting area
	TREE_Rect intersection = TREE_Rect_GetIntersection(
		globalRect,
		dirtyRect
	);

	// if no intersection, nothing to do
	if (intersection.extent.width == 0 || intersection.extent.height == 0)
	{
		return TREE_OK;
	}

	// draw just that area
	TREE_Offset imageOffset;
	imageOffset.x = intersection.offset.x - globalRect->offset.x;
	imageOffset.y = intersection.offset.y - globalRect->offset.y;
	result = TREE_Image_DrawImage(
		target,
		intersection.offset,
		image,
		imageOffset,
		intersection.extent
	);
	if (result)
	{
		return result;
	}

	return TREE_OK;
}

TREE_Result TREE_Control_Label_EventHandler(TREE_Event const* event)
{
	// validate
	if (!event || !event->control || event->control->type != TREE_CONTROL_TYPE_LABEL)
	{
		return TREE_ERROR_ARG_NULL;
	}

	// get the data
	TREE_Control* control = event->control;
	TREE_Control_LabelData* labelData = (TREE_Control_LabelData*)event->control->data;
	TREE_Result result;

	// handle the event
	switch (event->type)
	{
	case TREE_EVENT_TYPE_REFRESH:
	{
		TREE_Offset offset = { 0, 0 };
		result = _TREE_Control_Refresh_Text(
			control->image,
			offset,
			control->transform->globalRect.extent,
			labelData->text,
			labelData->alignment,
			labelData->normal
		);
		if (result)
		{
			return result;
		}

		break;
	}
	case TREE_EVENT_TYPE_DRAW:
	{
		// get the event data
		TREE_EventData_Draw* drawData = (TREE_EventData_Draw*)event->data;
		TREE_Image* target = drawData->target;
		TREE_Rect const* dirtyRect = &drawData->dirtyRect;

		result = _TREE_Control_Draw(
			target,
			dirtyRect,
			&control->transform->globalRect,
			control->image
		);
		if (result)
		{
			return result;
		}

		break;
	}
	}

	return TREE_OK;
}

TREE_Result TREE_Control_ButtonData_Init(TREE_Control_ButtonData* data, TREE_String text, TREE_ControlEventHandler onSubmit)
{
	// validate
	if (!data || !text)
	{
		return TREE_ERROR_ARG_NULL;
	}

	// allocate data
	TREE_Size textLength = strlen(text);
	data->text = TREE_NEW_ARRAY(TREE_Char, textLength + 1);
	if (!data->text)
	{
		return TREE_ERROR_ALLOC;
	}

	// set data
	memcpy(data->text, text, (textLength + 1) * sizeof(TREE_Char)); // +1 for null terminator
	data->text[textLength] = '\0'; // null terminator
	data->alignment = TREE_ALIGNMENT_CENTER;
	data->normal.character = ' ';
	data->normal.colorPair = TREE_ColorPair_Create(TREE_COLOR_BRIGHT_WHITE, TREE_COLOR_BRIGHT_BLACK);
	data->focused.character = ' ';
	data->focused.colorPair = TREE_ColorPair_Create(TREE_COLOR_BRIGHT_BLACK, TREE_COLOR_BRIGHT_WHITE);
	data->active.character = ' ';
	data->active.colorPair = TREE_ColorPair_Create(TREE_COLOR_BRIGHT_BLACK, TREE_COLOR_WHITE);
	data->onSubmit = onSubmit;

	return TREE_OK;
}

void TREE_Control_ButtonData_Free(TREE_Control_ButtonData* data)
{
	// validate
	if (!data)
	{
		return;
	}

	// free data
	TREE_DELETE(data->text);
	data->alignment = TREE_ALIGNMENT_CENTER;
	data->normal.character = ' ';
	data->normal.colorPair = TREE_ColorPair_CreateDefault();
	data->focused.character = ' ';
	data->focused.colorPair = TREE_ColorPair_CreateDefault();
	data->active.character = ' ';
	data->active.colorPair = TREE_ColorPair_CreateDefault();
	data->onSubmit = NULL;
}

TREE_Result TREE_Control_Button_Init(TREE_Control* control, TREE_Transform* parent, TREE_Control_ButtonData* data)
{
	// validate
	if (!control || !data)
	{
		return TREE_ERROR_ARG_NULL;
	}

	// initialize control
	TREE_Result result = TREE_Control_Init(control, parent, TREE_Control_Button_EventHandler, data);
	if (result)
	{
		return result;
	}

	// set data
	control->transform->localExtent.width = 20;
	control->transform->localExtent.height = 3;
	control->type = TREE_CONTROL_TYPE_BUTTON;
	control->flags = TREE_CONTROL_FLAGS_FOCUSABLE;

	return TREE_OK;
}

TREE_Result TREE_Control_Button_SetText(TREE_Control* control, TREE_String text, TREE_ColorPair colorPair)
{
	// validate
	if (!control || !text)
	{
		return TREE_ERROR_ARG_NULL;
	}

	// get data
	TREE_Control_ButtonData* buttonData = (TREE_Control_ButtonData*)control->data;

	// allocate new text
	TREE_Size textLength = strlen(text);
	TREE_Char* newText = TREE_NEW_ARRAY(TREE_Char, textLength + 1);
	if (!newText)
	{
		return TREE_ERROR_ALLOC;
	}

	// set data
	memcpy(newText, text, (textLength + 1) * sizeof(TREE_Char)); // +1 for null terminator
	newText[textLength] = '\0'; // null terminator
	TREE_REPLACE(buttonData->text, newText);

	// mark as dirty to get redrawn
	control->stateFlags |= TREE_CONTROL_STATE_FLAGS_DIRTY;

	return TREE_OK;
}

TREE_String TREE_Control_Button_GetText(TREE_Control* control)
{
	// validate
	if (!control)
	{
		return NULL;
	}

	// get data
	TREE_Control_ButtonData* buttonData = (TREE_Control_ButtonData*)control->data;

	// return text
	return buttonData->text;
}

TREE_Result TREE_Control_Button_EventHandler(TREE_Event const* event)
{
	// validate
	if (!event)
	{
		return TREE_ERROR_ARG_NULL;
	}
	if (event->control->type != TREE_CONTROL_TYPE_BUTTON)
	{
		return TREE_ERROR_ARG_INVALID;
	}

	// get the data
	TREE_Control* control = event->control;
	TREE_Control_ButtonData* buttonData = (TREE_Control_ButtonData*)event->control->data;
	TREE_Result result;

	// handle the event
	switch (event->type)
	{
	case TREE_EVENT_TYPE_KEY_DOWN:
	{
		// ignore if not focused
		if (!(control->stateFlags & TREE_CONTROL_STATE_FLAGS_FOCUSED))
		{
			break;
		}

		// get the event data
		TREE_EventData_Key* keyData = (TREE_EventData_Key*)event->data;
		TREE_Key key = keyData->key;
		if (key == TREE_KEY_ENTER || key == TREE_KEY_SPACE)
		{
			// mark as active and dirty
			control->stateFlags |= TREE_CONTROL_STATE_FLAGS_ACTIVE | TREE_CONTROL_STATE_FLAGS_DIRTY;
		}
		break;
	}
	case TREE_EVENT_TYPE_KEY_UP:
	{
		// ignore if not focused
		if (!(control->stateFlags & TREE_CONTROL_STATE_FLAGS_FOCUSED))
		{
			break;
		}

		// get the event data
		TREE_EventData_Key* keyData = (TREE_EventData_Key*)event->data;
		TREE_Key key = keyData->key;
		if (key == TREE_KEY_ENTER || key == TREE_KEY_SPACE)
		{
			// mark as inactive and dirty
			control->stateFlags &= ~TREE_CONTROL_STATE_FLAGS_ACTIVE;
			control->stateFlags |= TREE_CONTROL_STATE_FLAGS_DIRTY;

			// call the onSubmit function
			CALL_ACTION(buttonData->onSubmit, control, NULL);
		}

		break;
	}
	case TREE_EVENT_TYPE_REFRESH:
	{
		// determine pixel from state
		TREE_Pixel const* pixel = &buttonData->normal;
		if (control->stateFlags & TREE_CONTROL_STATE_FLAGS_ACTIVE)
		{
			pixel = &buttonData->active;
		}
		else if (control->stateFlags & TREE_CONTROL_STATE_FLAGS_FOCUSED)
		{
			pixel = &buttonData->focused;
		}

		TREE_Offset offset = { 0, 0 };
		result = _TREE_Control_Refresh_Text(
			control->image,
			offset,
			control->transform->globalRect.extent,
			buttonData->text,
			buttonData->alignment,
			*pixel
		);
		if (result)
		{
			return result;
		}

		break;
	}
	case TREE_EVENT_TYPE_DRAW:
	{
		// get the event data
		TREE_EventData_Draw* drawData = (TREE_EventData_Draw*)event->data;
		TREE_Image* target = drawData->target;
		TREE_Rect const* dirtyRect = &drawData->dirtyRect;

		result = _TREE_Control_Draw(
			target,
			dirtyRect,
			&control->transform->globalRect,
			control->image
		);
		if (result)
		{
			return result;
		}

		break;
	}
	}
	return TREE_OK;
}

TREE_Result TREE_Control_TextInputData_Init(TREE_Control_TextInputData* data, TREE_String text, TREE_Size capacity, TREE_String placeholder, TREE_Control_TextInputType type, TREE_ControlEventHandler onChange, TREE_ControlEventHandler onSubmit)
{
	// validate
	if (!data || !text || !placeholder)
	{
		return TREE_ERROR_ARG_NULL;
	}

	// allocate data
	TREE_Size textLength = strlen(text);
	TREE_Size placeholderLength = strlen(placeholder);
	data->text = TREE_NEW_ARRAY(TREE_Char, capacity + 1);
	if (!data->text)
	{
		return TREE_ERROR_ALLOC;
	}
	data->placeholder = TREE_NEW_ARRAY(TREE_Char, placeholderLength + 1);
	if (!data->placeholder)
	{
		TREE_DELETE(data->text);
		return TREE_ERROR_ALLOC;
	}

	// set data
	data->type = type;
	memcpy(data->text, text, textLength * sizeof(TREE_Char));
	data->text[textLength] = '\0'; // null terminator
	data->capacity = capacity;
	memcpy(data->placeholder, placeholder, placeholderLength * sizeof(TREE_Char));
	data->placeholder[placeholderLength] = '\0'; // null terminator

	data->normal.character = ' ';
	data->normal.colorPair = TREE_ColorPair_Create(TREE_COLOR_BLACK, TREE_COLOR_BRIGHT_BLACK);
	data->focused.character = ' ';
	data->focused.colorPair = TREE_ColorPair_Create(TREE_COLOR_BRIGHT_BLACK, TREE_COLOR_BRIGHT_WHITE);
	data->active.character = ' ';
	data->active.colorPair = TREE_ColorPair_Create(TREE_COLOR_BRIGHT_BLACK, TREE_COLOR_WHITE);
	data->cursor = TREE_ColorPair_Create(TREE_COLOR_WHITE, TREE_COLOR_BRIGHT_BLACK);
	// set cursor position to end of starting text
	data->cursorPosition = 0;
	data->cursorOffset = (TREE_Offset){ 0, 0 };
	data->cursorTimer = 0;
	data->scroll = 0;
	data->selection = TREE_ColorPair_Create(TREE_COLOR_BRIGHT_WHITE, TREE_COLOR_BRIGHT_BLUE);
	data->selectionOrigin = 0;
	data->selectionStart = textLength;
	data->selectionEnd = textLength;

	data->onChange = onChange;
	data->onSubmit = onSubmit;

	return TREE_OK;
}

void TREE_Control_TextInputData_Free(TREE_Control_TextInputData* data)
{
	// validate
	if (!data)
	{
		return;
	}

	// free data
	TREE_DELETE(data->text);
	TREE_DELETE(data->placeholder);
	data->type = TREE_CONTROL_TEXT_INPUT_TYPE_NONE;
	data->capacity = 0;
	data->normal.character = ' ';
	data->normal.colorPair = TREE_ColorPair_CreateDefault();
	data->focused.character = ' ';
	data->focused.colorPair = TREE_ColorPair_CreateDefault();
	data->active.character = ' ';
	data->active.colorPair = TREE_ColorPair_CreateDefault();
	data->cursor = TREE_ColorPair_CreateDefault();
	data->cursorPosition = 0;
	data->cursorOffset = (TREE_Offset){ 0, 0 };
	data->cursorTimer = 0;
	data->scroll = 0;
	data->selection = TREE_ColorPair_CreateDefault();
	data->selectionOrigin = 0;
	data->selectionStart = 0;
	data->selectionEnd = 0;
	data->onChange = NULL;
	data->onSubmit = NULL;
}

TREE_Char* TREE_Control_TextInputData_GetSelectedText(TREE_Control_TextInputData* data)
{
	// validate
	if (!data)
	{
		return NULL;
	}

	// if no selection, return NULL
	if (data->selectionStart == data->selectionEnd)
	{
		return NULL;
	}

	TREE_Size selectionLength = data->selectionEnd - data->selectionStart;
	TREE_Char* text = TREE_NEW_ARRAY(TREE_Char, selectionLength + 1);
	if (!text)
	{
		return NULL;
	}
	memcpy(text, &data->text[data->selectionStart], selectionLength * sizeof(TREE_Char));
	text[selectionLength] = '\0'; // null terminator

	return text;
}

TREE_Result TREE_Control_TextInputData_RemoveSelectedText(TREE_Control_TextInputData* data)
{
	// validate
	if (!data)
	{
		return TREE_ERROR_ARG_NULL;
	}

	// if no selection, do nothing
	if (data->selectionStart == data->selectionEnd)
	{
		return TREE_OK;
	}

	// remove selection
	TREE_Size textLength = strlen(data->text);
	TREE_Size selectionLength = data->selectionEnd - data->selectionStart;
	TREE_Size moveSize = textLength - data->selectionEnd;
	if (moveSize)
	{
		memmove(&data->text[data->selectionStart], &data->text[data->selectionEnd], (textLength - data->selectionEnd) * sizeof(TREE_Char));
	}
	data->text[textLength - selectionLength] = '\0'; // null terminator

	// update cursor position and clear selection
	data->cursorPosition = data->selectionStart;
	data->selectionEnd = data->selectionStart;
	data->selectionOrigin = 0;

	return TREE_OK;
}

TREE_Result TREE_Control_TextInputData_InsertText(TREE_Control_TextInputData* data, TREE_String text)
{
	// validate
	if (!data || !text)
	{
		return TREE_ERROR_ARG_NULL;
	}

	// cap the length, if it goes beyond the capacity
	TREE_Size textLength = strlen(data->text);
	TREE_Size clipboardLength = strlen(text);
	if (clipboardLength + textLength > data->capacity)
	{
		clipboardLength = data->capacity - textLength;
	}

	// shift existing text over, if needed
	if (data->cursorPosition < textLength)
	{
		memmove(&data->text[data->cursorPosition + clipboardLength], &data->text[data->cursorPosition], (textLength - data->cursorPosition) * sizeof(TREE_Char));
	}

	// insert clipboard text
	memcpy(&data->text[data->cursorPosition], text, clipboardLength * sizeof(TREE_Char));
	data->text[textLength + clipboardLength] = '\0'; // null terminator

	// move the cursor
	data->cursorPosition += clipboardLength;

	return TREE_OK;
}

TREE_Result TREE_Control_TextInput_Init(TREE_Control* control, TREE_Transform* parent, TREE_Control_TextInputData* data)
{
	// validate
	if (!control || !data)
	{
		return TREE_ERROR_ARG_NULL;
	}

	// initialize control
	TREE_Result result = TREE_Control_Init(control, parent, TREE_Control_TextInput_EventHandler, data);
	if (result)
	{
		return result;
	}

	// set data
	control->transform->localExtent.width = 20;
	control->transform->localExtent.height = 1;
	control->type = TREE_CONTROL_TYPE_TEXT_INPUT;
	control->flags = TREE_CONTROL_FLAGS_FOCUSABLE;

	return TREE_OK;
}

TREE_Result TREE_Control_TextInput_SetText(TREE_Control* control, TREE_String text)
{
	// validate
	if (!control || !text)
	{
		return TREE_ERROR_ARG_NULL;
	}
	if (control->type != TREE_CONTROL_TYPE_TEXT_INPUT)
	{
		return TREE_ERROR_ARG_INVALID;
	}

	// get data
	TREE_Control_TextInputData* textInputData = (TREE_Control_TextInputData*)control->data;

	// allocate new text
	TREE_Size textLength = strlen(text);
	TREE_Char* newText = TREE_NEW_ARRAY(TREE_Char, textLength + 1);
	if (!newText)
	{
		return TREE_ERROR_ALLOC;
	}

	// set data
	memcpy(newText, text, textLength * sizeof(TREE_Char));
	newText[textLength] = '\0'; // null terminator
	TREE_REPLACE(textInputData->text, newText);

	// mark as dirty to get redrawn
	control->stateFlags |= TREE_CONTROL_STATE_FLAGS_DIRTY;

	return TREE_OK;
}

TREE_String TREE_Control_TextInput_GetText(TREE_Control* control)
{
	// validate
	if (!control)
	{
		return NULL;
	}

	// get data
	TREE_Control_TextInputData* textInputData = (TREE_Control_TextInputData*)control->data;

	// return text
	return textInputData->text;
}

TREE_Bool _TREE_IsCharSafe(TREE_Char ch)
{
	return ch >= 32 && ch <= 126;
}

void _TREE_MakeSafe(TREE_Char* text, TREE_Size size)
{
	// make the text safe
	for (TREE_Size i = 0; i < size; i++)
	{
		if (!_TREE_IsCharSafe(text[i]))
		{
			text[i] = ' ';
		}
	}
}

TREE_Char* _TREE_MakeSafeCopy(TREE_Char* text, TREE_Size size)
{
	// allocate new text
	TREE_Char* newText = TREE_NEW_ARRAY(TREE_Char, size + 1);
	if (!newText)
	{
		return NULL;
	}

	// copy the text
	memcpy(newText, text, size * sizeof(TREE_Char));
	newText[size] = '\0'; // null terminator

	// make it safe
	_TREE_MakeSafe(newText, size);

	return newText;
}

TREE_Size _TREE_SeekDifferentCharType(TREE_Char* text, TREE_Size textLength, TREE_Size index)
{
	TREE_CharType type = TREE_Char_GetType(text[index]);
	for (TREE_Size i = index; i < textLength; i++)
	{
		if (TREE_Char_GetType(text[i]) != type)
		{
			return i + 1;
		}
	}
	return textLength;
}

TREE_Size _TREE_SeekDifferentCharTypeReverse(TREE_Char* text, TREE_Size textLength, TREE_Size index)
{
	TREE_CharType type = TREE_Char_GetType(text[index]);
	for (TREE_Size i = index; i > 0; i--)
	{
		if (TREE_Char_GetType(text[i]) != type)
		{
			return i;
		}
	}
	return 0;
}

TREE_Result TREE_Control_TextInput_EventHandler(TREE_Event const* event)
{
	// validate
	if (!event)
	{
		return TREE_ERROR_ARG_NULL;
	}
	if (event->control->type != TREE_CONTROL_TYPE_TEXT_INPUT)
	{
		return TREE_ERROR_ARG_INVALID;
	}

	// get the data
	TREE_Control* control = event->control;
	TREE_Control_TextInputData* data = (TREE_Control_TextInputData*)event->control->data;
	TREE_Result result;

	// determine if multiline or single line
	TREE_Extent const* extent = &control->transform->globalRect.extent;
	TREE_Bool multiline = extent->height > 1;

	// handle the event
	switch (event->type)
	{
	case TREE_EVENT_TYPE_KEY_DOWN:
	case TREE_EVENT_TYPE_KEY_HELD:
	{
		// ignore if not focused
		if (!(control->stateFlags & TREE_CONTROL_STATE_FLAGS_FOCUSED))
		{
			break;
		}

		// get the event data
		TREE_EventData_Key* keyData = (TREE_EventData_Key*)event->data;
		TREE_Key key = keyData->key;

		// if not active, do nothing
		if (!(control->stateFlags & TREE_CONTROL_STATE_FLAGS_ACTIVE))
		{
			// if not active but a submit key is pressed, become active
			if (key == TREE_KEY_ENTER || key == TREE_KEY_SPACE)
			{
				control->stateFlags |= TREE_CONTROL_STATE_FLAGS_ACTIVE | TREE_CONTROL_STATE_FLAGS_DIRTY;
			}
			break;
		}

		// get text length for calculations
		TREE_Size textLength = strlen(data->text);

		TREE_Bool cursorMoved = TREE_FALSE;
		TREE_Bool updateCursorOffset = TREE_FALSE;

		// reset cursor timer, as long as the input is not a modifier key
		if (key != TREE_KEY_SHIFT && key != TREE_KEY_ALT && key != TREE_KEY_CONTROL)
		{
			data->cursorTimer = 0;
		}

		// handle key inputs
		switch (key)
		{
		case TREE_KEY_ESCAPE: // exit out of active state, call submit function
			control->stateFlags &= ~TREE_CONTROL_STATE_FLAGS_ACTIVE;
			control->stateFlags |= TREE_CONTROL_STATE_FLAGS_DIRTY;
			CALL_ACTION(data->onSubmit, control, &data->text);
			break;
		case TREE_KEY_BACKSPACE: // remove character before cursor
			// if control held, "select" until a space found
			if (data->cursorPosition > 0 && data->selectionStart == data->selectionEnd && keyData->modifiers & TREE_KEY_MODIFIER_FLAGS_CONTROL)
			{
				// find the last space before the cursor
				data->selectionEnd = data->cursorPosition;
				data->selectionStart = _TREE_SeekDifferentCharTypeReverse(data->text, textLength, data->cursorPosition - 1);
			}
			if (data->selectionStart != data->selectionEnd)
			{
				// remove selected text
				TREE_Result result = TREE_Control_TextInputData_RemoveSelectedText(data);
				if (result)
				{
					return result;
				}
				control->stateFlags |= TREE_CONTROL_STATE_FLAGS_DIRTY;
				CALL_ACTION(data->onChange, control, &data->text);
				updateCursorOffset = TREE_TRUE;
			}
			else if (data->cursorPosition > 0)
			{
				// shift string over
				memmove(&data->text[data->cursorPosition - 1], &data->text[data->cursorPosition], (textLength - data->cursorPosition) * sizeof(TREE_Char));
				data->text[textLength - 1] = '\0'; // null terminator
				data->cursorPosition--;
				control->stateFlags |= TREE_CONTROL_STATE_FLAGS_DIRTY;
				CALL_ACTION(data->onChange, control, &data->text);
				updateCursorOffset = TREE_TRUE;
			}
			break;
		case TREE_KEY_DELETE: // remove character at cursor
			// if control held, "select" until a space found
			if (data->cursorPosition < textLength && data->selectionStart == data->selectionEnd && keyData->modifiers & TREE_KEY_MODIFIER_FLAGS_CONTROL)
			{
				// find the next space after the cursor
				data->selectionStart = data->cursorPosition;
				data->selectionEnd = _TREE_SeekDifferentCharType(data->text, textLength, data->cursorPosition);
			}
			if (data->selectionStart != data->selectionEnd)
			{
				// remove selected text
				TREE_Result result = TREE_Control_TextInputData_RemoveSelectedText(data);
				if (result)
				{
					return result;
				}
				control->stateFlags |= TREE_CONTROL_STATE_FLAGS_DIRTY;
				CALL_ACTION(data->onChange, control, &data->text);
				updateCursorOffset = TREE_TRUE;
			}
			else if (data->cursorPosition < textLength)
			{
				// shift string over
				memmove(&data->text[data->cursorPosition], &data->text[data->cursorPosition + 1], (textLength - data->cursorPosition) * sizeof(TREE_Char));
				data->text[textLength - 1] = '\0'; // null terminator
				control->stateFlags |= TREE_CONTROL_STATE_FLAGS_DIRTY;
				CALL_ACTION(data->onChange, control, &data->text);
				updateCursorOffset = TREE_TRUE;
			}
			break;
		case TREE_KEY_LEFT_ARROW: // move cursor left 1
			if (data->cursorPosition > 0)
			{
				// if control held, move until a space found
				if (keyData->modifiers & TREE_KEY_MODIFIER_FLAGS_CONTROL)
				{
					data->cursorPosition = _TREE_SeekDifferentCharTypeReverse(data->text, textLength, data->cursorPosition - 1);
				}
				else
				{
					data->cursorPosition--;
				}
				control->stateFlags |= TREE_CONTROL_STATE_FLAGS_DIRTY;
				cursorMoved = TREE_TRUE;
				updateCursorOffset = TREE_TRUE;
			}
			break;
		case TREE_KEY_RIGHT_ARROW: // move cursor right 1
			if (data->cursorPosition < textLength)
			{
				// if control held, move until a space found
				if (keyData->modifiers & TREE_KEY_MODIFIER_FLAGS_CONTROL)
				{
					data->cursorPosition = _TREE_SeekDifferentCharType(data->text, textLength, data->cursorPosition);
				}
				else
				{
					data->cursorPosition++;
				}
				control->stateFlags |= TREE_CONTROL_STATE_FLAGS_DIRTY;
				cursorMoved = TREE_TRUE;
				updateCursorOffset = TREE_TRUE;
			}
			break;
		case TREE_KEY_UP_ARROW: // move cursor up 1
			if (data->cursorPosition > 0)
			{
				if (multiline)
				{
					// if on first line, move to start of line
					if (data->cursorOffset.y == 0)
					{
						data->cursorPosition = 0;
						data->cursorOffset.x = 0;
					}
					else
					{
						// move cursor up 1 line
						data->cursorOffset.y -= 1;

						// get the line offsets
						TREE_Size lineCount = 0;
						TREE_Size* lineOffsets = NULL;
						TREE_Char** lines = NULL;
						result = _TREE_WordWrapAndOffsets(data->text, extent->width, &lines, &lineCount, &lineOffsets);
						if (result)
						{
							return result;
						}

						// set position based on line
						TREE_Size lineSize = strlen(lines[data->cursorOffset.y]);
						data->cursorPosition = lineOffsets[data->cursorOffset.y] + MIN(lineSize - 1, (TREE_Size)data->cursorOffset.x);
					}
				}
				else
				{
					// act like HOME if single line
					data->cursorPosition = 0;
				}
				control->stateFlags |= TREE_CONTROL_STATE_FLAGS_DIRTY;
				cursorMoved = TREE_TRUE;
			}
			break;
		case TREE_KEY_DOWN_ARROW: // move cursor down 1
			if (data->cursorPosition < textLength)
			{
				if (multiline)
				{
					// get the line offsets
					TREE_Size lineCount = 0;
					TREE_Size* lineOffsets = NULL;
					TREE_Char** lines = NULL;
					result = _TREE_WordWrapAndOffsets(data->text, extent->width, &lines, &lineCount, &lineOffsets);
					if (result)
					{
						return result;
					}

					// if on last line, move to end of line
					if (data->cursorOffset.y == lineCount - 1)
					{
						data->cursorPosition = textLength;
						TREE_Size lastLineLength = strlen(lines[lineCount - 1]);
						if (lastLineLength)
						{
							data->cursorOffset.x = (TREE_Int)lastLineLength;
						}
						else
						{
							data->cursorOffset.x = 0;
						}
					}
					else
					{
						// move to next line
						data->cursorOffset.y += 1;

						// set position based on line
						TREE_Size lineSize = strlen(lines[data->cursorOffset.y]);
						if (lineSize)
						{
							TREE_Size adjustment = data->cursorOffset.y == lineCount - 1 ? 0 : 1;
							data->cursorPosition = lineOffsets[data->cursorOffset.y] + MIN(lineSize - adjustment, (TREE_Size)data->cursorOffset.x);
						}
						else
						{
							data->cursorPosition = lineOffsets[data->cursorOffset.y];
						}
					}
				}
				else
				{
					// act like END if single line
					data->cursorPosition = textLength;
				}
				control->stateFlags |= TREE_CONTROL_STATE_FLAGS_DIRTY;
				cursorMoved = TREE_TRUE;
			}
			break;
		case TREE_KEY_HOME: // move cursor to start
			if (multiline)
			{
				// move to start of line
				if (keyData->modifiers & TREE_KEY_MODIFIER_FLAGS_CONTROL)
				{
					// set position based on total text
					data->cursorPosition = 0;
					data->cursorOffset.x = 0;
					data->cursorOffset.y = 0;
				}
				else
				{
					// get the line offsets
					TREE_Size lineCount = 0;
					TREE_Size* lineOffsets = NULL;
					TREE_Char** lines = NULL;
					result = _TREE_WordWrapAndOffsets(data->text, extent->width, &lines, &lineCount, &lineOffsets);
					if (result)
					{
						return result;
					}

					// set position based on line
					TREE_Size lineSize = strlen(lines[data->cursorOffset.y]);
					data->cursorPosition = lineOffsets[data->cursorOffset.y];
					data->cursorOffset.x = 0;
				}
			}
			else
			{
				// move to start of text
				data->cursorPosition = 0;
				updateCursorOffset = TREE_TRUE;
			}
			control->stateFlags |= TREE_CONTROL_STATE_FLAGS_DIRTY;
			cursorMoved = TREE_TRUE;
			break;
		case TREE_KEY_END: // move cursor to end
			if (multiline)
			{
				// move to end of line

				// get the line offsets
				TREE_Size lineCount = 0;
				TREE_Size* lineOffsets = NULL;
				TREE_Char** lines = NULL;
				result = _TREE_WordWrapAndOffsets(data->text, extent->width, &lines, &lineCount, &lineOffsets);
				if (result)
				{
					return result;
				}

				if (keyData->modifiers & TREE_KEY_MODIFIER_FLAGS_CONTROL || data->cursorOffset.y == lineCount - 1)
				{
					// set position based on total text
					data->cursorPosition = textLength;
					data->cursorOffset.x = (TREE_Int)strlen(lines[lineCount - 1]);
					data->cursorOffset.y = (TREE_Int)lineCount - 1;
				}
				else
				{
					// set position based on line
					TREE_Size lineSize = strlen(lines[data->cursorOffset.y]);
					data->cursorPosition = lineOffsets[data->cursorOffset.y] + lineSize - 1;
					data->cursorOffset.x = (TREE_Int)lineSize - 1;
				}
			}
			else
			{
				// move to end of text
				data->cursorPosition = textLength;
				updateCursorOffset = TREE_TRUE;
			}
			control->stateFlags |= TREE_CONTROL_STATE_FLAGS_DIRTY;
			cursorMoved = TREE_TRUE;
			break;
		default:
		{
			// if single line, and enter key pressed, submit
			if (key == TREE_KEY_ENTER && !multiline)
			{
				// if singleline, treat enter as a submit
				control->stateFlags &= ~TREE_CONTROL_STATE_FLAGS_ACTIVE;
				control->stateFlags |= TREE_CONTROL_STATE_FLAGS_DIRTY;
				CALL_ACTION(data->onSubmit, control, &data->text);
				break;
			}

			// if at capacity, ignore new characters
			if (textLength >= data->capacity)
			{
				break;
			}

			// get the character to add
			TREE_Char ch = TREE_Key_ToChar(key, keyData->modifiers);

			// if invalid, ignore it, with exceptions
			if (!_TREE_IsCharSafe(ch) && (!multiline || ch != '\n'))
			{
				break;
			}

			updateCursorOffset = TREE_TRUE;

			control->stateFlags |= TREE_CONTROL_STATE_FLAGS_DIRTY;
			data->cursorTimer = 0;

			// check for special inputs
			TREE_Bool ctrl = (keyData->modifiers & TREE_KEY_MODIFIER_FLAGS_CONTROL) != 0;
			if (ctrl && key == TREE_KEY_C)
			{
				// get selection text
				TREE_Char* text = TREE_Control_TextInputData_GetSelectedText(data);

				// copy selection to clipboard, if there is something selected
				if (text)
				{
					// set the clipboard text
					result = TREE_Clipboard_SetText(text);
					TREE_DELETE(text);
					if (result)
					{
						return result;
					}
				}
				break;
			}
			if (ctrl && key == TREE_KEY_V)
			{
				// remove selection, if any
				result = TREE_Control_TextInputData_RemoveSelectedText(data);
				if (result)
				{
					return result;
				}

				// paste from clipboard
				TREE_Char* text = NULL;
				result = TREE_Clipboard_GetText(&text);
				if (result)
				{
					return result;
				}
				TREE_Size textLength = strlen(text);

				// insert clipboard text
				result = TREE_Control_TextInputData_InsertText(data, text);
				TREE_DELETE(text);
				if (result)
				{
					return result;
				}

				// mark as dirty
				control->stateFlags |= TREE_CONTROL_STATE_FLAGS_DIRTY;

				// call onChange
				CALL_ACTION(data->onChange, control, &data->text);

				break;
			}
			if (ctrl && key == TREE_KEY_X)
			{
				// copy selection, and remove it

				// get selection text
				TREE_Char* text = TREE_Control_TextInputData_GetSelectedText(data);

				// copy selection to clipboard, if there is something selected
				if (text)
				{
					// set the clipboard text
					result = TREE_Clipboard_SetText(text);
					TREE_DELETE(text);
					if (result)
					{
						return result;
					}

					// remove selected text
					result = TREE_Control_TextInputData_RemoveSelectedText(data);
					if (result)
					{
						return result;
					}

					// mark as dirty
					control->stateFlags |= TREE_CONTROL_STATE_FLAGS_DIRTY;

					// call onChange
					CALL_ACTION(data->onChange, control, &data->text);
				}
				break;
			}
			if (ctrl && key == TREE_KEY_A)
			{
				// select all text
				data->selectionOrigin = 0;
				data->selectionStart = 0;
				data->selectionEnd = textLength;
				data->cursorPosition = textLength;

				// mark as dirty
				control->stateFlags |= TREE_CONTROL_STATE_FLAGS_DIRTY;

				break;
			}

			// type normal character

			// remove selection, if any
			result = TREE_Control_TextInputData_RemoveSelectedText(data);
			if (result)
			{
				return result;
			}

			// shift string over, if needed
			if (data->cursorPosition < textLength)
			{
				memmove(&data->text[data->cursorPosition + 1], &data->text[data->cursorPosition], (textLength - data->cursorPosition) * sizeof(TREE_Char));
			}
			data->text[textLength + 1] = '\0'; // null terminator

			// insert character
			data->text[data->cursorPosition] = ch;
			data->cursorPosition++;
			CALL_ACTION(data->onChange, control, &data->text);
			break;
		}
		}

		// if cursor moved, update the scroll region
		if (cursorMoved)
		{
			if (keyData->modifiers & TREE_KEY_MODIFIER_FLAGS_SHIFT)
			{
				// if cursor moved, update selection

				// set origin if cursor at origin
				if (data->selectionStart == data->selectionEnd)
				{
					data->selectionOrigin = data->selectionStart;
				}

				// update selection
				if (data->cursorPosition <= data->selectionOrigin)
				{
					data->selectionStart = data->cursorPosition;
					data->selectionEnd = data->selectionOrigin;
				}
				if (data->cursorPosition >= data->selectionOrigin)
				{
					data->selectionEnd = data->cursorPosition;
					data->selectionStart = data->selectionOrigin;
				}
			}
			else
			{
				// cursor moved, but no shift pressed
				data->selectionStart = data->cursorPosition;
				data->selectionEnd = data->cursorPosition;
			}
		}
		else if (data->selectionStart == data->selectionEnd && data->selectionStart != data->cursorPosition)
		{
			// cursor moved due to a modification
			data->selectionStart = data->cursorPosition;
			data->selectionEnd = data->cursorPosition;
		}

		// clamp the scroll to follow the cursor
		TREE_Size scrollExtent;
		TREE_Size scrollOffset;
		if (multiline)
		{
			// scroll is up and down on multiline

			// get word wrap offsets
			TREE_Char** lines;
			TREE_Size lineCount;
			TREE_Size* lineOffsets;
			result = _TREE_WordWrapAndOffsets(
				data->text,
				extent->width,
				&lines,
				&lineCount,
				&lineOffsets
			);
			if (result)
			{
				return result;
			}

			// get cursor offset
			TREE_Offset cursorOffset = _TREE_CalculateCursorOffset(
				data->cursorPosition,
				lineOffsets,
				lineCount
			);

			if (updateCursorOffset)
			{
				// update the cursor offset
				data->cursorOffset.x = cursorOffset.x;
				data->cursorOffset.y = cursorOffset.y;
			}

			// calculate the line number
			scrollOffset = cursorOffset.y;
			scrollExtent = extent->height;
		}
		else
		{
			scrollOffset = data->cursorPosition;
			scrollExtent = extent->width;
		}

		// adjust scroll
		data->scroll = _TREE_ClampScroll(data->scroll, scrollOffset, scrollExtent);

		break;
	}
	case TREE_EVENT_TYPE_REFRESH:
	{
		result = TREE_Image_Resize(control->image, *extent);
		if (result)
		{
			return result;
		}

		TREE_Bool active = (control->stateFlags & TREE_CONTROL_STATE_FLAGS_ACTIVE);

		// determine pixel from state
		TREE_Pixel const* pixel = &data->normal;
		if (active)
		{
			pixel = &data->active;
		}
		else if (control->stateFlags & TREE_CONTROL_STATE_FLAGS_FOCUSED)
		{
			pixel = &data->focused;
		}

		// draw the background
		result = TREE_Image_Clear(control->image, *pixel);
		if (result)
		{
			return result;
		}

		// determine what text to draw
		TREE_Char* text = data->text;
		TREE_Bool disposeText = TREE_FALSE;

		// if not active and no text, use placeholder
		if (!(active) && !text[0])
		{
			text = data->placeholder;
		}
		// if not placeholder and is a password, replace with stars
		else if (data->type == TREE_CONTROL_TEXT_INPUT_TYPE_PASSWORD)
		{
			TREE_Size textLength = strlen(text);
			TREE_Char* passwordText = TREE_NEW_ARRAY(TREE_Char, textLength + 1);
			if (!passwordText)
			{
				return TREE_ERROR_ALLOC;
			}
			memset(passwordText, '*', textLength);
			passwordText[textLength] = '\0'; // null terminator
			text = passwordText;
			disposeText = TREE_TRUE;
		}

		TREE_Size textLength = strlen(text);

		if (multiline)
		{
			// multiline

			// get word wrap data

			TREE_Char** lines;
			TREE_Size lineCount;
			TREE_Size* lineOffsets;
			result = _TREE_WordWrapAndOffsets(
				text,
				extent->width,
				&lines,
				&lineCount,
				&lineOffsets
			);
			if (result)
			{
				return result;
			}

			// calculate the cursor position
			TREE_Offset cursorOffset = _TREE_CalculateCursorOffset(
				data->cursorPosition,
				lineOffsets,
				lineCount
			);
			// adjust for scrolling
			cursorOffset.y -= (TREE_Int)data->scroll;

			// draw the lines based on the scroll
			TREE_Size count = MIN(lineCount, extent->height);
			TREE_Size scroll = data->scroll;

			if (!active)
			{
				// no scroll when inactive
				scroll = 0;
			}

			// draw each line
			TREE_ColorPair lineColor;
			for (TREE_Size i = 0; i < count && data->scroll + i < lineCount; i++)
			{
				// replace unsafe characters
				TREE_Char* line = lines[data->scroll + i];
				TREE_Size lineLength = strlen(line);
				_TREE_MakeSafe(line, lineLength);

				// determine if whole line is selected or not
				TREE_Size lineBeginIndex = lineOffsets[data->scroll + i];
				TREE_Size lineEndIndex = lineBeginIndex + lineLength;
				if (lineBeginIndex >= data->selectionStart &&
					lineEndIndex <= data->selectionEnd)
				{
					// all selected
					lineColor = data->selection;
				}
				else
				{
					// none/partial selected
					lineColor = pixel->colorPair;
				}

				// draw the string
				TREE_Offset offset;
				offset.x = 0;
				offset.y = (TREE_Int)i;
				result = TREE_Image_DrawString(
					control->image,
					offset,
					line,
					lineColor
				);
				if (result)
				{
					break;
				}

				if (data->selectionStart != data->selectionEnd &&
					((data->selectionStart >= lineBeginIndex &&
						data->selectionStart <= lineEndIndex) ||
					(data->selectionEnd >= lineBeginIndex &&
						data->selectionEnd <= lineEndIndex)))
				{ 
					// draw just the selected part, over the existing text
					TREE_Size selectionStart = MAX(data->selectionStart, lineBeginIndex);
					TREE_Size selectionEnd = MIN(data->selectionEnd, lineEndIndex);
					TREE_Size selectionLength = selectionEnd - selectionStart;
					TREE_Size selectionOffset = selectionStart - lineBeginIndex;
					TREE_Char* selectionText = TREE_NEW_ARRAY(TREE_Char, selectionLength + 1);
					if (!selectionText)
					{
						return TREE_ERROR_ALLOC;
					}
					memcpy(selectionText, &text[selectionStart], selectionLength * sizeof(TREE_Char));
					selectionText[selectionLength] = '\0'; // null terminator

					// only render safe chars
					_TREE_MakeSafe(selectionText, selectionLength);

					// draw the selection
					TREE_Offset selectionPos;
					selectionPos.x = (TREE_Int)selectionOffset;
					selectionPos.y = (TREE_Int)i;
					result = TREE_Image_DrawString(
						control->image,
						selectionPos,
						selectionText,
						data->selection
					);
					TREE_DELETE(selectionText);
					if (result)
					{
						return result;
					}
				}
			}

			// draw cursor if active
			if (active)
			{
				// calculate the cursor character
				TREE_Pixel cursorPixel;
				cursorPixel.colorPair = data->cursor;
				if (data->cursorPosition == textLength || !_TREE_IsCharSafe(text[data->cursorPosition]))
				{
					cursorPixel.character = ' ';
				}
				else
				{
					cursorPixel.character = text[data->cursorPosition];
				}

				// draw the cursor
				result = TREE_Image_Set(
					control->image,
					cursorOffset,
					cursorPixel
				);
				if (result)
				{
					return result;
				}
			}
		}
		else
		{
			// single line

			// find the offset
			TREE_Size offset = data->scroll;
			if (active)
			{
				if (data->cursorPosition - offset == extent->width)
				{
					offset++;
				}
			}
			else
			{
				// no scroll when inactive
				offset = 0;
			}

			// get size of text using the offset
			TREE_Size length = MIN(extent->width, textLength - offset);

			// get a copy of the text to draw
			TREE_Char* textCopy = TREE_NEW_ARRAY(TREE_Char, length + 1);
			if (!textCopy)
			{
				return TREE_ERROR_ALLOC;
			}
			memcpy(textCopy, &text[offset], length * sizeof(TREE_Char));
			textCopy[length] = '\0'; // null terminator

			// only render safe chars
			_TREE_MakeSafe(textCopy, length);

			// draw the text
			TREE_Offset imageOffset;
			imageOffset.x = 0;
			imageOffset.y = 0;
			result = TREE_Image_DrawString(
				control->image,
				imageOffset,
				textCopy,
				pixel->colorPair
			);
			TREE_DELETE(textCopy);
			if (result)
			{
				return result;
			}

			// if active
			if (active)
			{
				// draw selection, if any, and if visible
				if (data->selectionStart != data->selectionEnd &&
					data->selectionEnd > offset &&
					data->selectionStart < offset + length)
				{
					// get the start and end of the selection
					TREE_Size selectionStart = MAX(data->selectionStart, offset);
					TREE_Size selectionEnd = MIN(data->selectionEnd, offset + length);
					TREE_Size selectionLength = selectionEnd - selectionStart;

					// create a copy of the text to draw
					TREE_Char* selectionText = TREE_NEW_ARRAY(TREE_Char, selectionLength + 1);
					if (!selectionText)
					{
						return TREE_ERROR_ALLOC;
					}
					memcpy(selectionText, &text[selectionStart], selectionLength * sizeof(TREE_Char));
					selectionText[selectionLength] = '\0'; // null terminator

					// only render safe chars
					_TREE_MakeSafe(selectionText, selectionLength);

					// draw the selection
					TREE_Offset selectionOffset;
					selectionOffset.x = (TREE_Int)(selectionStart - offset);
					selectionOffset.y = 0;
					result = TREE_Image_DrawString(
						control->image,
						selectionOffset,
						selectionText,
						data->selection
					);
					if (result)
					{
						return result;
					}
				}

				// draw cursor
				// calculate the cursor character
				TREE_Pixel cursorPixel;
				cursorPixel.colorPair = data->cursor;
				if (data->cursorPosition == textLength || !_TREE_IsCharSafe(text[data->cursorPosition]))
				{
					cursorPixel.character = ' ';
				}
				else
				{
					cursorPixel.character = text[data->cursorPosition];
				}

				// draw the cursor
				TREE_Offset cursorOffset;
				cursorOffset.x = (TREE_Int)(data->cursorPosition - offset);
				cursorOffset.y = 0;
				result = TREE_Image_Set(
					control->image,
					cursorOffset,
					cursorPixel
				);
				if (result)
				{
					return result;
				}
			}
		}

		// if needed, dispose of text
		if (disposeText)
		{
			TREE_DELETE(text);
		}

		break;
	}
	case TREE_EVENT_TYPE_DRAW:
	{
		// get the event data
		TREE_EventData_Draw* drawData = (TREE_EventData_Draw*)event->data;
		TREE_Image* target = drawData->target;
		TREE_Rect const* dirtyRect = &drawData->dirtyRect;

		result = _TREE_Control_Draw(
			target,
			dirtyRect,
			&control->transform->globalRect,
			control->image
		);
		if (result)
		{
			return result;
		}

		break;
	}
	}
	return TREE_OK;
}

TREE_Result TREE_Control_ScrollbarData_Init(TREE_Control_ScrollbarData* data, TREE_Control_ScrollbarType type, TREE_Bool vertical)
{
	// validate
	if (!data)
	{
		return TREE_ERROR_ARG_NULL;
	}

	// set data
	data->type = type;
	if (vertical)
	{
		data->top = '^';
		data->bottom = 'v';
		data->line = '|';
	}
	else
	{
		data->top = '<';
		data->bottom = '>';
		data->line = '-';
	}
	data->bar = '#';
	data->showEnds = TREE_TRUE;

	return TREE_OK;
}

TREE_Result TREE_Control_Scrollbar_Draw(TREE_Image* target, TREE_Offset scrollbarOffset, TREE_Extent scrollbarExtent, TREE_Control_ScrollbarData* data, TREE_Size scroll, TREE_Size maxScroll, TREE_ColorPair colorPair, TREE_ColorPair barColorPair)
{
	// validate
	if (!target || !data)
	{
		return TREE_ERROR_ARG_NULL;
	}
	if (scroll > maxScroll)
	{
		scroll = maxScroll;
	}
	if (scrollbarExtent.width <= 1 && scrollbarExtent.height <= 1)
	{
		return TREE_ERROR_ARG_OUT_OF_RANGE;
	}
	// at least one extent should be 1
	if (scrollbarExtent.width != 1 && scrollbarExtent.height != 1)
	{
		return TREE_ERROR_ARG_INVALID;
	}

	TREE_Offset offset = scrollbarOffset;
	TREE_Extent extent = scrollbarExtent;

	// determine if horizontal or vertical
	TREE_Bool vertical = extent.width == 1;

	// draw the scrollbar
	TREE_Pixel scrollbarPixel;
	scrollbarPixel.character = data->line;
	scrollbarPixel.colorPair = colorPair;
	TREE_Result result = TREE_Image_FillRect(
		target,
		offset,
		extent,
		scrollbarPixel
	);
	if (result)
	{
		return result;
	}

	// if supposed to, draw the top and bottom
	if (data->showEnds)
	{
		// draw the top
		scrollbarPixel.character = data->top;
		result = TREE_Image_Set(
			target,
			offset,
			scrollbarPixel
		);
		if (result)
		{
			return result;
		}

		// draw the bottom
		scrollbarPixel.character = data->bottom;
		if (vertical)
		{
			offset.y = scrollbarOffset.y + (TREE_Int)(scrollbarExtent.height - 1);
		}
		else
		{
			offset.x = scrollbarOffset.x + (TREE_Int)(scrollbarExtent.width - 1);
		}

		result = TREE_Image_Set(
			target,
			offset,
			scrollbarPixel
		);
		if (result)
		{
			return result;
		}
	}

	// draw the bar, if it is needed
	if (!maxScroll)
	{
		// not enough
		return TREE_OK;
	}

	TREE_Size barSize;
	TREE_Size barOffset;
	if (maxScroll < extent.height)
	{
		// basic scrollbar
		barSize = extent.height - maxScroll;
		barOffset = scroll;
	}
	else
	{
		// complex/tiny scrollbar
		barSize = 1;
		barOffset = scroll * (extent.height - 1) / maxScroll;
	}
	scrollbarPixel.character = data->bar;
	scrollbarPixel.colorPair = barColorPair;
	if (vertical)
	{
		offset.y = scrollbarOffset.y + (TREE_Int)barOffset;
		extent.height = (TREE_UInt)barSize;
	}
	else
	{
		offset.x = scrollbarOffset.x + (TREE_Int)barOffset;
		extent.width = (TREE_UInt)barSize;
	}
	result = TREE_Image_FillRect(
		target,
		offset,
		extent,
		scrollbarPixel
	);
	if (result)
	{
		return result;
	}

	return TREE_OK;
}

TREE_Result TREE_Control_ListData_Init(TREE_Control_ListData* data, TREE_Control_ListFlags flags, TREE_String* options, TREE_Size optionsSize, TREE_ControlEventHandler onChange, TREE_ControlEventHandler onSubmit)
{
	// validate
	if (!data || !options)
	{
		return TREE_ERROR_ARG_NULL;
	}

	data->flags = flags;
	data->options = NULL;
	data->optionsSize = 0;
	data->selectedIndices = NULL;

	TREE_Result result = TREE_Control_ListData_SetOptions(data, options, optionsSize);
	if (result)
	{
		return result;
	}

	// set data
	data->normalSelected.character = ' ';
	data->normalSelected.colorPair = TREE_ColorPair_Create(TREE_COLOR_WHITE, TREE_COLOR_BRIGHT_BLUE);
	data->normalUnselected.character = ' ';
	data->normalUnselected.colorPair = TREE_ColorPair_Create(TREE_COLOR_WHITE, TREE_COLOR_BRIGHT_BLACK);
	data->normalScrollbarColorPair = TREE_ColorPair_Create(TREE_COLOR_WHITE, TREE_COLOR_BRIGHT_BLACK);
	data->normalScrollbarBarColorPair = TREE_ColorPair_Create(TREE_COLOR_WHITE, TREE_COLOR_BRIGHT_BLACK);

	data->focusedSelected.character = ' ';
	data->focusedSelected.colorPair = TREE_ColorPair_Create(TREE_COLOR_WHITE, TREE_COLOR_BRIGHT_BLUE);
	data->focusedUnselected.character = ' ';
	data->focusedUnselected.colorPair = TREE_ColorPair_Create(TREE_COLOR_BRIGHT_BLACK, TREE_COLOR_WHITE);
	data->focusedScrollbarColorPair = TREE_ColorPair_Create(TREE_COLOR_BRIGHT_BLACK, TREE_COLOR_WHITE);
	data->focusedScrollbarBarColorPair = TREE_ColorPair_Create(TREE_COLOR_BRIGHT_BLACK, TREE_COLOR_WHITE);

	data->activeSelected.character = ' ';
	data->activeSelected.colorPair = TREE_ColorPair_Create(TREE_COLOR_BRIGHT_WHITE, TREE_COLOR_BLUE);
	data->activeUnselected.character = ' ';
	data->activeUnselected.colorPair = TREE_ColorPair_Create(TREE_COLOR_BLACK, TREE_COLOR_WHITE);
	data->activeScrollbarColorPair = TREE_ColorPair_Create(TREE_COLOR_BRIGHT_BLACK, TREE_COLOR_BRIGHT_WHITE);
	data->activeScrollbarBarColorPair = TREE_ColorPair_Create(TREE_COLOR_WHITE, TREE_COLOR_BRIGHT_BLACK);

	data->hoveredSelected.character = ' ';
	data->hoveredSelected.colorPair = TREE_ColorPair_Create(TREE_COLOR_BRIGHT_WHITE, TREE_COLOR_BRIGHT_BLUE);
	data->hoveredUnselected.character = ' ';
	data->hoveredUnselected.colorPair = TREE_ColorPair_Create(TREE_COLOR_BLACK, TREE_COLOR_BRIGHT_WHITE);

	result = TREE_Control_ScrollbarData_Init(&data->scrollbar, TREE_CONTROL_SCROLLBAR_TYPE_DYNAMIC, TREE_TRUE);
	if (result)
	{
		return result;
	}

	data->onChange = onChange;
	data->onSubmit = onSubmit;

	return TREE_OK;
}

void TREE_Control_ListData_Free(TREE_Control_ListData* data)
{
	if (!data)
	{
		return;
	}

	// free the options
	TREE_DELETE_ARRAY(data->options, data->optionsSize);
	data->optionsSize = 0;
	data->selectedIndex = 0;
	data->selectedIndices = NULL;
	data->hoverIndex = 0;
	data->scroll = 0;
	data->normalSelected.character = ' ';
	data->normalSelected.colorPair = TREE_ColorPair_CreateDefault();
	data->normalUnselected.character = ' ';
	data->normalUnselected.colorPair = TREE_ColorPair_CreateDefault();
	data->normalScrollbarColorPair = TREE_ColorPair_CreateDefault();
	data->focusedSelected.character = ' ';
	data->focusedSelected.colorPair = TREE_ColorPair_CreateDefault();
	data->focusedUnselected.character = ' ';
	data->focusedUnselected.colorPair = TREE_ColorPair_CreateDefault();
	data->focusedScrollbarColorPair = TREE_ColorPair_CreateDefault();
	data->activeSelected.character = ' ';
	data->activeSelected.colorPair = TREE_ColorPair_CreateDefault();
	data->activeUnselected.character = ' ';
	data->activeUnselected.colorPair = TREE_ColorPair_CreateDefault();
	data->activeScrollbarColorPair = TREE_ColorPair_CreateDefault();
	data->hoveredSelected.character = ' ';
	data->hoveredSelected.colorPair = TREE_ColorPair_CreateDefault();
	data->hoveredUnselected.character = ' ';
	data->hoveredUnselected.colorPair = TREE_ColorPair_CreateDefault();
	data->onChange = NULL;
	data->onSubmit = NULL;
}

TREE_Result TREE_Control_ListData_SetOptions(TREE_Control_ListData* data, TREE_String* options, TREE_Size optionsSize)
{
	// validate
	if (!data)
	{
		return TREE_ERROR_ARG_NULL;
	}

	// if old options, free them
	if (data->options)
	{
		TREE_DELETE_ARRAY(data->options, data->optionsSize);
	}
	data->selectedIndex = 0;
	data->hoverIndex = 0;
	data->scroll = 0;
	if (data->selectedIndices)
	{
		TREE_DELETE(data->selectedIndices);
	}
	data->optionsSize = optionsSize;

	// if new data, allocate and copy over
	if (options && optionsSize)
	{
		// allocate data
		data->options = TREE_NEW_ARRAY(TREE_Char*, optionsSize);
		if (!data->options)
		{
			return TREE_ERROR_ALLOC;
		}

		// copy over options
		for (TREE_Size i = 0; i < optionsSize; i++)
		{
			TREE_String oldOption = options[i];
			TREE_Size oldOptionSize = strlen(oldOption);
			TREE_Char* option = TREE_NEW_ARRAY(TREE_Char, oldOptionSize + 1);
			if (!option)
			{
				TREE_DELETE_ARRAY(data->options, i);
				return TREE_ERROR_ALLOC;
			}
			memcpy(option, oldOption, oldOptionSize * sizeof(TREE_Char));
			option[oldOptionSize] = '\0'; // null terminator
			data->options[i] = option;
		}

		data->optionsSize = optionsSize;

		// allocate selection data if needed
		if (data->flags & TREE_CONTROL_LIST_FLAGS_MULTISELECT)
		{
			data->selectedIndices = TREE_NEW_ARRAY(TREE_Byte, optionsSize);
			if (!data->selectedIndices)
			{
				TREE_DELETE_ARRAY(data->options, optionsSize);
				return TREE_ERROR_ALLOC;
			}

			for (TREE_Size i = 0; i < optionsSize; i++)
			{
				data->selectedIndices[i] = TREE_FALSE;
			}
		}
	}

	return TREE_OK;
}

TREE_Result TREE_Control_ListData_SetSelected(TREE_Control_ListData* data, TREE_Size index, TREE_Bool selected)
{
	// validate
	if (!data)
	{
		return TREE_ERROR_ARG_NULL;
	}
	if (index >= data->optionsSize)
	{
		return TREE_ERROR_ARG_OUT_OF_RANGE;
	}

	if (data->flags & TREE_CONTROL_LIST_FLAGS_MULTISELECT)
	{
		// multiselect
		data->selectedIndices[index] = selected;
	}
	else
	{
		// single select
		data->selectedIndex = index;
	}

	return TREE_OK;
}

TREE_Result TREE_Control_ListData_GetSelected(TREE_Control_ListData* data, TREE_Size** indices, TREE_Size* indexCount)
{
	// validate
	if (!data || !indices || !indexCount)
	{
		return TREE_ERROR_ARG_NULL;
	}
	*indices = NULL;
	*indexCount = 0;

	if (data->flags & TREE_CONTROL_LIST_FLAGS_MULTISELECT)
	{
		// compile indices
		TREE_Size selectedCount = 0;
		for (TREE_Size i = 0; i < data->optionsSize; i++)
		{
			if (data->selectedIndices[i])
			{
				selectedCount++;
			}
		}
		if (selectedCount == 0)
		{
			return TREE_OK;
		}
		TREE_Size* selected = TREE_NEW_ARRAY(TREE_Size, selectedCount);
		if (!selected)
		{
			return TREE_ERROR_ALLOC;
		}
		TREE_Size j = 0;
		for (TREE_Size i = 0; i < data->optionsSize; i++)
		{
			if (data->selectedIndices[i])
			{
				selected[j] = i;
				j++;
			}
		}
		*indices = selected;
		*indexCount = selectedCount;
	}
	else
	{
		// single index
		*indices = TREE_NEW(TREE_Size);
		if (!*indices)
		{
			return TREE_ERROR_ALLOC;
		}
		**indices = data->selectedIndex;
		*indexCount = 1;
	}
	return TREE_OK;
}

TREE_Bool TREE_Control_ListData_IsSelected(TREE_Control_ListData* data, TREE_Size index)
{
	// validate
	if (!data)
	{
		return TREE_FALSE;
	}

	if (index >= data->optionsSize)
	{
		return TREE_FALSE;
	}

	if (data->flags & TREE_CONTROL_LIST_FLAGS_MULTISELECT)
	{
		// multiselect
		return data->selectedIndices[index];
	}
	else
	{
		// single select
		return data->selectedIndex == index;
	}
}

TREE_Result TREE_Control_List_Init(TREE_Control* control, TREE_Transform* parent, TREE_Control_ListData* data)
{
	// validate
	if (!control || !data)
	{
		return TREE_ERROR_ARG_NULL;
	}

	// init control
	TREE_Result result = TREE_Control_Init(control, parent, TREE_Control_List_EventHandler, data);
	if (result)
	{
		return result;
	}

	// set data
	control->type = TREE_CONTROL_TYPE_LIST;
	control->flags = TREE_CONTROL_FLAGS_FOCUSABLE;
	control->transform->localExtent.width = 16;
	control->transform->localExtent.height = 10;

	return TREE_OK;
}

TREE_Result _TREE_Control_List_Draw(TREE_Image* target, TREE_Offset controlOffset, TREE_Extent controlExtent, TREE_ControlStateFlags stateFlags, TREE_Control_ListData* data)
{
	TREE_Result result;
	TREE_Bool active = (stateFlags & TREE_CONTROL_STATE_FLAGS_ACTIVE);

	// determine pixels from state
	TREE_Pixel const* unselectedPixel = &data->normalUnselected;
	TREE_Pixel const* selectedPixel = &data->normalSelected;
	TREE_ColorPair scrollbarColorPair = data->normalScrollbarColorPair;
	TREE_ColorPair scrollbarBarColorPair = data->normalScrollbarBarColorPair;
	if (active)
	{
		unselectedPixel = &data->activeUnselected;
		selectedPixel = &data->activeSelected;
		scrollbarColorPair = data->activeScrollbarColorPair;
		scrollbarBarColorPair = data->activeScrollbarBarColorPair;
	}
	else if (stateFlags & TREE_CONTROL_STATE_FLAGS_FOCUSED)
	{
		unselectedPixel = &data->focusedUnselected;
		selectedPixel = &data->focusedSelected;
		scrollbarColorPair = data->focusedScrollbarColorPair;
		scrollbarBarColorPair = data->focusedScrollbarBarColorPair;
	}

	// determine if there is a scrollbar
	TREE_Size scrollbar = 0;
	switch (data->scrollbar.type)
	{
	case TREE_CONTROL_SCROLLBAR_TYPE_NONE:
		scrollbar = 0;
		break;
	case TREE_CONTROL_SCROLLBAR_TYPE_STATIC:
		scrollbar = 1;
		break;
	case TREE_CONTROL_SCROLLBAR_TYPE_DYNAMIC:
		scrollbar = (controlExtent.height < data->optionsSize) ? 1 : 0;
		break;
	}

	// get width of options
	TREE_Size optionsWidth = controlExtent.width - scrollbar;

	// draw the options
	TREE_Size count = controlExtent.height;
	TREE_Size scroll = data->scroll;
	TREE_Size index;
	for (TREE_Size i = 0; i < count; i++)
	{
		index = scroll + i;

		TREE_Size fillerOffset;
		TREE_Size fillerLength;
		TREE_Pixel const* pixel;

		if (index < data->optionsSize)
		{
			// draw option
			// get option and info about it
			TREE_Char* option = data->options[scroll + i];
			TREE_Size optionLength = strlen(option);
			TREE_Size optionWidth = MIN(optionsWidth, optionLength);
			fillerOffset = optionWidth;
			fillerLength = optionsWidth - optionWidth;
			TREE_Bool selected = TREE_Control_ListData_IsSelected(data, scroll + i);

			// copy the option
			TREE_Char* optionCopy = _TREE_MakeSafeCopy(option, optionWidth);

			// get pixel to use for drawing
			if (active && data->hoverIndex == scroll + i)
			{
				if (selected)
				{
					pixel = &data->hoveredSelected;
				}
				else
				{
					pixel = &data->hoveredUnselected;
				}
			}
			else
			{
				if (selected)
				{
					pixel = selectedPixel;
				}
				else
				{
					pixel = unselectedPixel;
				}
			}

			// draw option text
			TREE_Offset offset;
			offset.x = controlOffset.x;
			offset.y = controlOffset.y + (TREE_Int)i;
			result = TREE_Image_DrawString(
				target,
				offset,
				optionCopy,
				pixel->colorPair
			);
			if (result)
			{
				return result;
			}

			free(optionCopy);
		}
		else
		{
			// no option to draw
			fillerOffset = 0;
			fillerLength = optionsWidth;
			pixel = unselectedPixel;
		}

		// draw filler
		if (fillerLength)
		{
			TREE_Offset offset;
			offset.x = controlOffset.x + (TREE_Int)fillerOffset;
			offset.y = controlOffset.y + (TREE_Int)i;
			TREE_Extent extent;
			extent.width = (TREE_UInt)fillerLength;
			extent.height = 1;
			result = TREE_Image_FillRect(
				target,
				offset,
				extent,
				*pixel
			);
			if (result)
			{
				return result;
			}
		}
	}

	// draw the scrollbar
	if (scrollbar)
	{

		// get the scrollbar extent
		TREE_Extent scrollbarExtent;
		scrollbarExtent.width = 1;
		scrollbarExtent.height = controlExtent.height;

		// get the scrollbar offset
		TREE_Offset scrollbarOffset;
		scrollbarOffset.x = controlOffset.x + (TREE_Int)optionsWidth;
		scrollbarOffset.y = controlOffset.y;

		// draw the scrollbar
		result = TREE_Control_Scrollbar_Draw(
			target,
			scrollbarOffset,
			scrollbarExtent,
			&data->scrollbar,
			data->scroll,
			data->optionsSize - controlExtent.height,
			scrollbarColorPair,
			scrollbarBarColorPair
		);
		if (result)
		{
			return result;
		}
	}

	return TREE_OK;
}

TREE_Result TREE_Control_List_EventHandler(TREE_Event const* event)
{
	// validate
	if (!event || !event->control)
	{
		return TREE_ERROR_ARG_NULL;
	}
	if (event->control->type != TREE_CONTROL_TYPE_LIST)
	{
		return TREE_ERROR_ARG_INVALID;
	}

	TREE_Result result;

	TREE_Control* control = event->control;
	TREE_Control_ListData* data = (TREE_Control_ListData*)control->data;

	switch (event->type)
	{
	case TREE_EVENT_TYPE_KEY_DOWN:
	case TREE_EVENT_TYPE_KEY_HELD:
	{
		// ignore if not focused
		if (!(control->stateFlags & TREE_CONTROL_STATE_FLAGS_FOCUSED))
		{
			break;
		}

		// get the event data
		TREE_EventData_Key* keyData = (TREE_EventData_Key*)event->data;
		TREE_Key key = keyData->key;

		// activate if not active
		if (!(control->stateFlags & TREE_CONTROL_STATE_FLAGS_ACTIVE))
		{
			// if not active but a submit key is pressed, become active
			if (key == TREE_KEY_ENTER || key == TREE_KEY_SPACE)
			{
				control->stateFlags |= TREE_CONTROL_STATE_FLAGS_ACTIVE | TREE_CONTROL_STATE_FLAGS_DIRTY;
			}
			break;
		}

		// handle key events
		switch (key)
		{
		case TREE_KEY_DOWN_ARROW: // move to next option
			if (data->hoverIndex < data->optionsSize - 1)
			{
				data->hoverIndex++;
				control->stateFlags |= TREE_CONTROL_STATE_FLAGS_DIRTY;
			}
			break;
		case TREE_KEY_UP_ARROW: // move to previous option
			if (data->hoverIndex > 0)
			{
				data->hoverIndex--;
				control->stateFlags |= TREE_CONTROL_STATE_FLAGS_DIRTY;
			}
			break;
		case TREE_KEY_HOME: // move to first option
			if (data->hoverIndex != 0)
			{
				data->hoverIndex = 0;
				control->stateFlags |= TREE_CONTROL_STATE_FLAGS_DIRTY;
			}
			break;
		case TREE_KEY_END: // move to last option
			if (data->hoverIndex != data->optionsSize - 1)
			{
				data->hoverIndex = data->optionsSize - 1;
				control->stateFlags |= TREE_CONTROL_STATE_FLAGS_DIRTY;
			}
			break;
		case TREE_KEY_PAGE_UP: // move up a page
			if (data->hoverIndex > control->transform->localExtent.height)
			{
				data->hoverIndex -= control->transform->localExtent.height;
			}
			else
			{
				data->hoverIndex = 0;
			}
			control->stateFlags |= TREE_CONTROL_STATE_FLAGS_DIRTY;
			break;
		case TREE_KEY_PAGE_DOWN: // move down a page
			if (data->hoverIndex < data->optionsSize - control->transform->localExtent.height)
			{
				data->hoverIndex += control->transform->localExtent.height;
			}
			else
			{
				data->hoverIndex = data->optionsSize - 1;
			}
			control->stateFlags |= TREE_CONTROL_STATE_FLAGS_DIRTY;
			break;
		case TREE_KEY_SPACE: // select option
		case TREE_KEY_ENTER:
		{
			if (data->flags & TREE_CONTROL_LIST_FLAGS_MULTISELECT)
			{
				// multiselect
				data->selectedIndices[data->hoverIndex] = !data->selectedIndices[data->hoverIndex];
				CALL_ACTION(data->onChange, control, &data->selectedIndices);
			}
			else
			{
				// single select
				data->selectedIndex = data->hoverIndex;
				CALL_ACTION(data->onChange, control, &data->selectedIndex);
			}
			control->stateFlags |= TREE_CONTROL_STATE_FLAGS_DIRTY;
			break;
		}
		case TREE_KEY_ESCAPE: // exit
			control->stateFlags &= ~TREE_CONTROL_STATE_FLAGS_ACTIVE;
			control->stateFlags |= TREE_CONTROL_STATE_FLAGS_DIRTY;
			if (data->flags & TREE_CONTROL_LIST_FLAGS_MULTISELECT)
			{
				// multiselect
				CALL_ACTION(data->onChange, control, &data->selectedIndices);
			}
			else
			{
				// single select
				CALL_ACTION(data->onChange, control, &data->selectedIndex);
			}
			break;
		}

		// if needed, update the scroll
		data->scroll = _TREE_ClampScroll(data->scroll, data->hoverIndex, control->transform->localExtent.height);

		break;
	}
	case TREE_EVENT_TYPE_REFRESH:
	{
		// resize the image if needed
		TREE_Result result = TREE_Image_Resize(control->image, control->transform->globalRect.extent);
		if (result)
		{
			return result;
		}

		// draw the list
		TREE_Offset offset = { 0, 0 };
		result = _TREE_Control_List_Draw(
			control->image,
			offset,
			control->transform->globalRect.extent,
			control->stateFlags,
			data);
		if (result)
		{
			return result;
		}

		break;
	}
	case TREE_EVENT_TYPE_DRAW:
	{
		// get the event data
		TREE_EventData_Draw* drawData = (TREE_EventData_Draw*)event->data;
		TREE_Image* target = drawData->target;
		TREE_Rect const* dirtyRect = &drawData->dirtyRect;

		// draw the control
		result = _TREE_Control_Draw(
			target,
			dirtyRect,
			&control->transform->globalRect,
			control->image
		);
		if (result)
		{
			return result;
		}
		break;
	}
	}

	return TREE_OK;
}

TREE_Result TREE_Control_DropdownData_Init(TREE_Control_DropdownData* data, TREE_String* options, TREE_Size optionsSize, TREE_Size selectedIndex, TREE_Int drop, TREE_ControlEventHandler onSubmit)
{
	// validate
	if (!data || !options)
	{
		return TREE_ERROR_ARG_NULL;
	}
	if (!optionsSize)
	{
		return TREE_ERROR_ARG_INVALID;
	}

	data->options = NULL;
	data->optionsSize = 0;
	TREE_Result result = TREE_Control_DropdownData_SetOptions(data, options, optionsSize);
	if (result)
	{
		return result;
	}

	// set data
	data->selectedIndex = selectedIndex;
	data->hoverIndex = selectedIndex;
	data->scroll = 0;
	data->origin.x = 0;
	data->origin.y = 0;
	data->dropType = drop == 0 ? TREE_CONTROL_DROPDOWN_TYPE_DYNAMIC : TREE_CONTROL_DROPDOWN_TYPE_STATIC;
	data->drop = drop;

	data->normal.character = ' ';
	data->normal.colorPair = TREE_ColorPair_Create(TREE_COLOR_BLACK, TREE_COLOR_BRIGHT_BLACK);

	data->focused.character = ' ';
	data->focused.colorPair = TREE_ColorPair_Create(TREE_COLOR_BRIGHT_BLACK, TREE_COLOR_BRIGHT_WHITE);

	data->active.character = ' ';
	data->active.colorPair = TREE_ColorPair_Create(TREE_COLOR_BRIGHT_BLACK, TREE_COLOR_WHITE);

	data->selected.character = ' ';
	data->selected.colorPair = TREE_ColorPair_Create(TREE_COLOR_BRIGHT_WHITE, TREE_COLOR_BLUE);
	data->unselected.character = ' ';
	data->unselected.colorPair = TREE_ColorPair_Create(TREE_COLOR_BLACK, TREE_COLOR_WHITE);

	data->hoveredSelected.character = ' ';
	data->hoveredSelected.colorPair = TREE_ColorPair_Create(TREE_COLOR_BRIGHT_WHITE, TREE_COLOR_BRIGHT_BLUE);
	data->hoveredUnselected.character = ' ';
	data->hoveredUnselected.colorPair = TREE_ColorPair_Create(TREE_COLOR_BLACK, TREE_COLOR_BRIGHT_WHITE);

	data->onSubmit = onSubmit;

	return TREE_OK;
}

void TREE_Control_DropdownData_Free(TREE_Control_DropdownData* data)
{
	if (!data)
	{
		return;
	}
	// free the options
	TREE_DELETE_ARRAY(data->options, data->optionsSize);
	data->optionsSize = 0;
	data->selectedIndex = 0;
	data->hoverIndex = 0;
	data->normal.character = ' ';
	data->normal.colorPair = TREE_ColorPair_CreateDefault();
	data->focused.character = ' ';
	data->focused.colorPair = TREE_ColorPair_CreateDefault();
	data->active.character = ' ';
	data->active.colorPair = TREE_ColorPair_CreateDefault();
	data->selected.character = ' ';
	data->selected.colorPair = TREE_ColorPair_CreateDefault();
	data->unselected.character = '/';
	data->unselected.colorPair = TREE_ColorPair_CreateDefault();
	data->onSubmit = NULL;
}

TREE_Result TREE_Control_DropdownData_SetOptions(TREE_Control_DropdownData* data, TREE_String* options, TREE_Size optionsSize)
{
	// validate
	if (!data || !options)
	{
		return TREE_ERROR_ARG_NULL;
	}

	// if old options, free them
	if (data->options)
	{
		TREE_DELETE_ARRAY(data->options, data->optionsSize);
	}

	// allocate and copy over options
	data->options = TREE_NEW_ARRAY(TREE_Char*, optionsSize);
	if (!data->options)
	{
		return TREE_ERROR_ALLOC;
	}
	for (TREE_Size i = 0; i < optionsSize; i++)
	{
		TREE_String oldOption = options[i];
		TREE_Size oldOptionSize = strlen(oldOption);
		TREE_Char* option = TREE_NEW_ARRAY(TREE_Char, oldOptionSize + 1);
		if (!option)
		{
			TREE_DELETE_ARRAY(data->options, i);
			return TREE_ERROR_ALLOC;
		}
		memcpy(option, oldOption, oldOptionSize * sizeof(TREE_Char));
		option[oldOptionSize] = '\0'; // null terminator
		data->options[i] = option;
	}
	data->optionsSize = optionsSize;

	// reset selection
	data->selectedIndex = 0;
	data->hoverIndex = 0;
	data->scroll = 0;

	return TREE_OK;
}

TREE_Result TREE_Control_Dropdown_Init(TREE_Control* control, TREE_Transform* parent, TREE_Control_DropdownData* data)
{
	// validate
	if (!control || !data)
	{
		return TREE_ERROR_ARG_NULL;
	}

	// control init
	TREE_Result result = TREE_Control_Init(control, parent, TREE_Control_Dropdown_EventHandler, data);
	if (result)
	{
		return result;
	}

	// set data
	control->type = TREE_CONTROL_TYPE_DROPDOWN;
	control->flags = TREE_CONTROL_FLAGS_FOCUSABLE;
	control->transform->localExtent.width = 22;
	control->transform->localExtent.height = 1;

	return TREE_OK;
}

TREE_Result TREE_Control_Dropdown_EventHandler(TREE_Event const* event)
{
	// validate
	if (!event || !event->control)
	{
		return TREE_ERROR_ARG_NULL;
	}
	if (event->control->type != TREE_CONTROL_TYPE_DROPDOWN)
	{
		return TREE_ERROR_ARG_INVALID;
	}

	TREE_Result result;
	TREE_Control* control = event->control;
	TREE_Control_DropdownData* data = (TREE_Control_DropdownData*)control->data;

	switch (event->type)
	{
	case TREE_EVENT_TYPE_KEY_DOWN:
	case TREE_EVENT_TYPE_KEY_HELD:
	{
		// ignore if not focused
		if (!(control->stateFlags & TREE_CONTROL_STATE_FLAGS_FOCUSED))
		{
			break;
		}

		// get the event data
		TREE_EventData_Key* keyData = (TREE_EventData_Key*)event->data;
		TREE_Key key = keyData->key;

		// if not active, do nothing
		if (!(control->stateFlags & TREE_CONTROL_STATE_FLAGS_ACTIVE))
		{
			// if not active but a submit key is pressed, become active
			if (key == TREE_KEY_ENTER || key == TREE_KEY_SPACE)
			{
				control->stateFlags |= TREE_CONTROL_STATE_FLAGS_ACTIVE | TREE_CONTROL_STATE_FLAGS_DIRTY;

				// set origin
				data->origin = control->transform->localOffset;

				// calulate drop size if needed
				if (data->dropType == TREE_CONTROL_DROPDOWN_TYPE_DYNAMIC)
				{
					// get window size
					TREE_Offset dropdownOffset = control->transform->globalRect.offset;
					TREE_Extent windowExtent = TREE_Window_GetExtent();

					// get space above and below the dropdown
					TREE_Int optionsCount = (TREE_Int)data->optionsSize;
					TREE_Int belowSpace = (TREE_Int)windowExtent.height - dropdownOffset.y;
					TREE_Int aboveSpace = (TREE_Int)dropdownOffset.y + 1;

					// if can fit down, go down
					// if not, go up
					// if cannot fit in either, pick larger one
					if (optionsCount < belowSpace)
					{
						// go down
						data->drop = optionsCount;
					}
					else if (optionsCount < aboveSpace)
					{
						// go up
						data->drop = -optionsCount;
					}
					else
					{
						// go bigger direction, cap at space size
						if (aboveSpace > belowSpace)
						{
							data->drop = -aboveSpace;
						}
						else
						{
							data->drop = belowSpace;
						}
					}
				}

				// move transform (if needed) to account for dropdown
				if (data->drop < 0)
				{
					control->transform->localOffset.y += data->drop + 1;
				}

				// resize transform as well
				TREE_UInt dropSize = (TREE_UInt)(data->drop > 0 ? data->drop : -data->drop);
				control->transform->localExtent.height = dropSize;
				control->transform->dirty = TREE_TRUE;
			}
			break;
		}

		TREE_Bool cursorMoved = TREE_FALSE;

		// handle key events
		switch (key)
		{
		case TREE_KEY_UP_ARROW: // move to previous option
			if (data->hoverIndex > 0)
			{
				data->hoverIndex--;
				control->stateFlags |= TREE_CONTROL_STATE_FLAGS_DIRTY;
				cursorMoved = TREE_TRUE;
			}
			break;
		case TREE_KEY_DOWN_ARROW: // move to next option
			if (data->hoverIndex < data->optionsSize - 1)
			{
				data->hoverIndex++;
				control->stateFlags |= TREE_CONTROL_STATE_FLAGS_DIRTY;
				cursorMoved = TREE_TRUE;
			}
			break;
		case TREE_KEY_HOME: // move to first option
			if (data->hoverIndex != 0)
			{
				data->hoverIndex = 0;
				control->stateFlags |= TREE_CONTROL_STATE_FLAGS_DIRTY;
				cursorMoved = TREE_TRUE;
			}
			break;
		case TREE_KEY_END: // move to last option
			if (data->hoverIndex != data->optionsSize - 1)
			{
				data->hoverIndex = data->optionsSize - 1;
				control->stateFlags |= TREE_CONTROL_STATE_FLAGS_DIRTY;
				cursorMoved = TREE_TRUE;
			}
			break;
		case TREE_KEY_PAGE_UP: // move up a page
			if (data->hoverIndex > control->transform->localExtent.height)
			{
				data->hoverIndex -= control->transform->localExtent.height;
			}
			else
			{
				data->hoverIndex = 0;
			}
			control->stateFlags |= TREE_CONTROL_STATE_FLAGS_DIRTY;
			cursorMoved = TREE_TRUE;
			break;
		case TREE_KEY_PAGE_DOWN: // move down a page
			if (data->hoverIndex < data->optionsSize - control->transform->localExtent.height)
			{
				data->hoverIndex += control->transform->localExtent.height;
			}
			else
			{
				data->hoverIndex = data->optionsSize - 1;
			}
			control->stateFlags |= TREE_CONTROL_STATE_FLAGS_DIRTY;
			cursorMoved = TREE_TRUE;
			break;
		case TREE_KEY_ENTER: // submit
		{
			// move back to origin
			control->transform->localOffset = data->origin;

			// shrink transform
			control->transform->localExtent.height = 1;
			control->transform->dirty = TREE_TRUE;

			data->selectedIndex = data->hoverIndex;
			control->stateFlags &= ~TREE_CONTROL_STATE_FLAGS_ACTIVE;
			control->stateFlags |= TREE_CONTROL_STATE_FLAGS_DIRTY;
			CALL_ACTION(data->onSubmit, control, &data->selectedIndex);
			break;
		}
		case TREE_KEY_ESCAPE: // cancel
		{
			// move back to origin
			control->transform->localOffset = data->origin;

			// shrink transform
			control->transform->localExtent.height = 1;
			control->transform->dirty = TREE_TRUE;

			data->hoverIndex = data->selectedIndex;
			control->stateFlags &= ~TREE_CONTROL_STATE_FLAGS_ACTIVE;
			control->stateFlags |= TREE_CONTROL_STATE_FLAGS_DIRTY;
			break;
		}
		}

		// update scroll
		if (cursorMoved)
		{
			data->scroll = _TREE_ClampScroll(data->scroll, data->hoverIndex, control->transform->localExtent.height - 1);
		}

		break;
	}
	case TREE_EVENT_TYPE_REFRESH:
	{
		TREE_Bool active = (control->stateFlags & TREE_CONTROL_STATE_FLAGS_ACTIVE);
		
		// resize if needed
		result = TREE_Image_Resize(control->image, control->transform->globalRect.extent);
		if (result)
		{
			return result;
		}

		// get pixel from state
		TREE_Pixel const* pixel;
		if (active)
		{
			pixel = &data->active;
		}
		else if (control->stateFlags & TREE_CONTROL_STATE_FLAGS_FOCUSED)
		{
			pixel = &data->focused;
		}
		else
		{
			pixel = &data->normal;
		}

		// get the selected option
		TREE_Extent extent = control->transform->globalRect.extent;
		TREE_Size optionsWidth = extent.width - 2;
		TREE_Size fillerOffset;
		TREE_Size fillerWidth;
		TREE_Offset offset;
		TREE_Int mainOffset = data->drop < 0 ? extent.height - 1 : 0;
		if (data->optionsSize)
		{
			TREE_Char* option = data->options[data->selectedIndex];
			TREE_Size optionLength = strlen(option);
			TREE_Size optionWidth = MIN(optionsWidth, optionLength);
			TREE_Char* optionCopy = _TREE_MakeSafeCopy(option, optionWidth);
			if (!optionCopy)
			{
				return TREE_ERROR_ALLOC;
			}

			// draw the option
			offset.x = 0;
			offset.y = mainOffset;
			result = TREE_Image_DrawString(
				control->image,
				offset,
				optionCopy,
				pixel->colorPair
			);
			free(optionCopy);
			if (result)
			{
				return result;
			}

			fillerOffset = optionWidth;
			fillerWidth = optionsWidth - optionWidth;
		}
		else
		{
			fillerOffset = 0;
			fillerWidth = optionsWidth;
		}

		// draw the filler, if any
		if (fillerWidth)
		{
			offset.x = (TREE_Int)fillerOffset;
			offset.y = mainOffset;
			TREE_Extent extent;
			extent.width = (TREE_UInt)fillerWidth;
			extent.height = 1;
			result = TREE_Image_FillRect(
				control->image,
				offset,
				extent,
				*pixel
			);
			if (result)
			{
				return result;
			}
		}

		// draw the dropdown separator
		TREE_Pixel arrowPixel;
		arrowPixel.character = '|';
		arrowPixel.colorPair = pixel->colorPair;
		offset.x = (TREE_Int)optionsWidth;
		offset.y = mainOffset;
		result = TREE_Image_Set(
			control->image,
			offset,
			arrowPixel
		);
		if (result)
		{
			return result;
		}

		// draw the dropdown arrow
		arrowPixel.character = active ? '^' : 'v';
		offset.x = (TREE_Int)optionsWidth + 1;
		offset.y = mainOffset;
		result = TREE_Image_Set(
			control->image,
			offset,
			arrowPixel
		);
		if (result)
		{
			return result;
		}

		// if active, draw the dropdown
		if (active)
		{
			// populate a list data structure using the dropdown's data
			TREE_Control_ListData listData;
			listData.flags = TREE_CONTROL_LIST_FLAGS_NONE;
			listData.options = data->options;
			listData.optionsSize = data->optionsSize;
			listData.selectedIndex = data->selectedIndex;
			listData.selectedIndices = NULL;
			listData.hoverIndex = data->hoverIndex;
			listData.scroll = data->scroll;
			result = TREE_Control_ScrollbarData_Init(
				&listData.scrollbar,
				TREE_CONTROL_SCROLLBAR_TYPE_DYNAMIC,
				TREE_TRUE
			);
			listData.activeSelected = data->selected;
			listData.activeUnselected = data->unselected;
			listData.activeScrollbarColorPair = TREE_ColorPair_Create(TREE_COLOR_BRIGHT_BLACK, TREE_COLOR_BRIGHT_WHITE);
			listData.activeScrollbarBarColorPair = TREE_ColorPair_Create(TREE_COLOR_WHITE, TREE_COLOR_BRIGHT_BLACK);
			listData.hoveredSelected = data->hoveredSelected;
			listData.hoveredUnselected = data->hoveredUnselected;
			listData.onChange = NULL;
			listData.onSubmit = NULL;

			// draw the list where the dropdown is
			TREE_Int adjustment = data->drop < 0 ? 0 : 1;
			TREE_Offset listOffset;
			listOffset.x = 0;
			listOffset.y = adjustment;
			TREE_Extent listExtent = extent;
			listExtent.height -= 1;
			result = _TREE_Control_List_Draw(
				control->image,
				listOffset,
				listExtent,
				control->stateFlags,
				&listData
			);
			if (result)
			{
				TREE_Control_ListData_Free(&listData);
				return result;
			}
		}

		break;
	}
	case TREE_EVENT_TYPE_DRAW:
	{
		// get the event data
		TREE_EventData_Draw* drawData = (TREE_EventData_Draw*)event->data;
		TREE_Image* target = drawData->target;
		TREE_Rect const* dirtyRect = &drawData->dirtyRect;

		// draw the control
		TREE_Result result = _TREE_Control_Draw(
			target,
			dirtyRect,
			&control->transform->globalRect,
			control->image
		);
		if (result)
		{
			return result;
		}

		break;
	}
	}

	return TREE_OK;
}

TREE_Result TREE_Control_CheckboxData_Init(TREE_Control_CheckboxData* data, TREE_String text, TREE_Byte checked, TREE_ControlEventHandler onCheck)
{
	// validate
	if (!data || !text)
	{
		return TREE_ERROR_ARG_NULL;
	}

	// set data
	TREE_Result result = TREE_String_CreateCopy(&data->text, text);
	if (result)
	{
		return result;
	}

	data->checked = checked;
	data->reverse = TREE_FALSE;

	data->normal = TREE_ColorPair_Create(TREE_COLOR_BLACK, TREE_COLOR_BRIGHT_BLACK);
	data->focused = TREE_ColorPair_Create(TREE_COLOR_BLACK, TREE_COLOR_BRIGHT_WHITE);

	data->uncheckedChar = ' ';
	data->checkedChar = 'X';

	data->onCheck = onCheck;

	return TREE_OK;
}

void TREE_Control_CheckboxData_Free(TREE_Control_CheckboxData* data)
{
	// validate
	if (!data)
	{
		return;
	}
	// free the text
	TREE_DELETE(data->text);
}

TREE_Result TREE_Control_Checkbox_Init(TREE_Control* control, TREE_Transform* parent, TREE_Control_CheckboxData* data)
{
	// validate
	if (!control || !data)
	{
		return TREE_ERROR_ARG_NULL;
	}

	// control init
	TREE_Result result = TREE_Control_Init(control, parent, TREE_Control_Checkbox_EventHandler, data);
	if (result)
	{
		return result;
	}

	// set data
	control->type = TREE_CONTROL_TYPE_CHECKBOX;
	control->flags = TREE_CONTROL_FLAGS_FOCUSABLE;
	control->transform->localExtent.width = 16;
	control->transform->localExtent.height = 1;

	return TREE_OK;
}

TREE_Result TREE_Control_Checkbox_SetChecked(TREE_Control* control, TREE_Byte checked)
{
	// validate
	if (!control || control->type != TREE_CONTROL_TYPE_CHECKBOX)
	{
		return TREE_ERROR_ARG_NULL;
	}

	// set the checked state
	TREE_Control_CheckboxData* data = (TREE_Control_CheckboxData*)control->data;
	data->checked = checked;

	// dirty the component to be redrawn
	control->stateFlags |= TREE_CONTROL_STATE_FLAGS_DIRTY;

	return TREE_OK;
}

TREE_Bool TREE_Control_Checkbox_GetChecked(TREE_Control* control)
{
	// validate
	if (!control || control->type != TREE_CONTROL_TYPE_CHECKBOX)
	{
		return TREE_FALSE;
	}

	// get the checked state
	TREE_Control_CheckboxData* data = (TREE_Control_CheckboxData*)control->data;
	return data->checked;
}

TREE_Result TREE_Control_Checkbox_EventHandler(TREE_Event const* event)
{
	// validate
	if (!event || !event->control)
	{
		return TREE_ERROR_ARG_NULL;
	}
	if (event->control->type != TREE_CONTROL_TYPE_CHECKBOX)
	{
		return TREE_ERROR_ARG_INVALID;
	}

	// get data
	TREE_Result result;
	TREE_Control* control = event->control;
	TREE_Control_CheckboxData* data = (TREE_Control_CheckboxData*)control->data;

	// handle events
	switch (event->type)
	{
	case TREE_EVENT_TYPE_KEY_DOWN:
	case TREE_EVENT_TYPE_KEY_HELD:
	{
		// ignore if not focused
		if (!(control->stateFlags & TREE_CONTROL_STATE_FLAGS_FOCUSED))
		{
			break;
		}

		// get the event data
		TREE_EventData_Key* keyData = (TREE_EventData_Key*)event->data;
		TREE_Key key = keyData->key;

		// check boxes are like buttons, they do not need to be active to be used
		// when submit button is pressed, toggle the checkbox
		if (key == TREE_KEY_ENTER || key == TREE_KEY_SPACE)
		{
			data->checked = !data->checked;
			control->stateFlags |= TREE_CONTROL_STATE_FLAGS_DIRTY;
			CALL_ACTION(data->onCheck, control, &data->checked);
		}
		break;
	}
	case TREE_EVENT_TYPE_REFRESH:
	{
		// resize if needed
		result = TREE_Image_Resize(control->image, control->transform->globalRect.extent);
		if (result)
		{
			return result;
		}

		// get color from state
		TREE_ColorPair color;
		if (control->stateFlags & TREE_CONTROL_STATE_FLAGS_FOCUSED)
		{
			color = data->focused;
		}
		else
		{
			color = data->normal;
		}

		// get checked character
		TREE_Char checkedChar = data->checked ? data->checkedChar : data->uncheckedChar;

		// draw the checkbox
		TREE_Offset offset;
		offset.y = 0;

		// if reverse, place the checkbox on the right
		TREE_Int checkboxLength = 3;
		TREE_Int checkOffset = data->reverse ? control->transform->globalRect.extent.width - checkboxLength : 0;
		TREE_Int textOffset = data->reverse ? 0 : checkboxLength;
		offset.x = checkOffset;

		// draw the checkbox
		TREE_Pixel pixel;
		pixel.character = '[';
		pixel.colorPair = color;
		result = TREE_Image_Set(
			control->image,
			offset,
			pixel
		);
		if (result)
		{
			return result;
		}
		offset.x++;
		pixel.character = checkedChar;
		result = TREE_Image_Set(
			control->image,
			offset,
			pixel
		);
		if (result)
		{
			return result;
		}
		offset.x++;
		pixel.character = ']';
		result = TREE_Image_Set(
			control->image,
			offset,
			pixel
		);
		if (result)
		{
			return result;
		}

		// draw the string
		offset.x = textOffset;
		offset.y = 0;
		
		// if the string is too long, cap it
		TREE_Char* text;
		result = TREE_String_CreateClampedCopy(&text, data->text, control->transform->globalRect.extent.width - (TREE_Size)checkboxLength);
		if (result)
		{
			return result;
		}

		TREE_Size textSize = strlen(text);

		// draw the string
		result = TREE_Image_DrawString(
			control->image,
			offset,
			text,
			data->normal
		);
		free(text);
		if (result)
		{
			return result;
		}

		// if there is extra space, fill it
		TREE_Size fillerSize = control->transform->globalRect.extent.width - (TREE_Size)checkboxLength - textSize;
		if (fillerSize)
		{
			TREE_Offset fillerOffset;
			fillerOffset.x = (TREE_Int)(textOffset + textSize);
			fillerOffset.y = 0;

			TREE_Extent fillerExtent;
			fillerExtent.width = (TREE_UInt)fillerSize;
			fillerExtent.height = 1;
			
			pixel.character = ' ';
			pixel.colorPair = data->normal;
			result = TREE_Image_FillRect(
				control->image,
				fillerOffset,
				fillerExtent,
				pixel
			);
			if (result)
			{
				return result;
			}
		}

		break;
	}
	case TREE_EVENT_TYPE_DRAW:
	{
		// get the event data
		TREE_EventData_Draw* drawData = (TREE_EventData_Draw*)event->data;
		TREE_Image* target = drawData->target;
		TREE_Rect const* dirtyRect = &drawData->dirtyRect;

		// draw the control
		TREE_Result result = _TREE_Control_Draw(
			target,
			dirtyRect,
			&control->transform->globalRect,
			control->image
		);
		if (result)
		{
			return result;
		}

		break;
	}
	}

	return TREE_OK;
}

TREE_Result TREE_Application_Init(TREE_Application* application, TREE_Surface* surface, TREE_Size capacity, TREE_EventHandler eventHandler)
{
	// validate
	if (!application || !surface)
	{
		return TREE_ERROR_ARG_NULL;
	}
	if (capacity == 0)
	{
		return TREE_ERROR_ARG_OUT_OF_RANGE;
	}

	// allocate data
	application->controls = TREE_NEW_ARRAY(TREE_Control*, capacity);
	if (!application->controls)
	{
		return TREE_ERROR_ALLOC;
	}

	TREE_Result result;

	// set data
	application->controlsCapacity = capacity;
	application->controlsSize = 0;
	application->focusedControl = NULL;
	application->running = TREE_FALSE;
	result = TREE_Input_Init(&application->input);
	if (result)
	{
		TREE_DELETE(application->controls);
		return result;
	}
	application->eventHandler = eventHandler;
	application->surface = surface;

	return TREE_OK;
}

void TREE_Application_Free(TREE_Application* application)
{
	if (!application)
	{
		return;
	}

	if (application->controls)
	{
		free(application->controls);
		application->controls = NULL;
	}

	application->controlsCapacity = 0;
	application->controlsSize = 0;
	application->focusedControl = NULL;
	application->running = TREE_FALSE;
	TREE_Input_Free(&application->input);
	application->eventHandler = NULL;
	application->surface = NULL;
}

TREE_Result TREE_Application_AddControl(TREE_Application* application, TREE_Control* control)
{
	// validate
	if (!application || !control)
	{
		return TREE_ERROR_ARG_NULL;
	}
	if (application->controlsSize == application->controlsCapacity)
	{
		return TREE_ERROR_FULL;
	}

	// add to the application
	application->controls[application->controlsSize] = control;
	application->controlsSize++;

	// if no focused control, and this one can be focused, set it as the focused control
	if (!application->focusedControl && (control->flags & TREE_CONTROL_FLAGS_FOCUSABLE))
	{
		TREE_Result result = TREE_Application_SetFocus(application, control);
		if (result)
		{
			return result;
		}
	}

	return TREE_OK;
}

TREE_Result TREE_Application_SetFocus(TREE_Application* application, TREE_Control* control)
{
	// validate
	if (!application)
	{
		return TREE_ERROR_ARG_NULL;
	}

	// if control already focused, take its focus away
	if (application->focusedControl)
	{
		application->focusedControl->stateFlags &= ~TREE_CONTROL_STATE_FLAGS_FOCUSED & ~TREE_CONTROL_STATE_FLAGS_ACTIVE;
		application->focusedControl->stateFlags |= TREE_CONTROL_STATE_FLAGS_DIRTY;
	}

	// set to new control
	application->focusedControl = control;

	// if new control, give it focus
	if (control)
	{
		control->stateFlags |= TREE_CONTROL_STATE_FLAGS_FOCUSED | TREE_CONTROL_STATE_FLAGS_DIRTY;
	}

	return TREE_OK;
}

TREE_Result TREE_Application_DispatchEvent(TREE_Application* application, TREE_Event const* event)
{
	// validate
	if (!application || !event)
	{
		return TREE_ERROR_ARG_NULL;
	}

	TREE_Event e = *event; // local copy to edit
	e.application = application; // set the application for the event

	// handle event on the application level
	switch (e.type)
	{
	case TREE_EVENT_TYPE_KEY_DOWN:
	case TREE_EVENT_TYPE_KEY_HELD:
	{
		// move focus if arrow keys used, if current control is not active
		if (!application->focusedControl || (application->focusedControl->stateFlags & TREE_CONTROL_STATE_FLAGS_ACTIVE))
		{
			break;
		}

		// movement allowed
		TREE_EventData_Key* keyData = (TREE_EventData_Key*)e.data;
		TREE_Key key = keyData->key;

		// move if move key pressed, and there is a control to move to
		TREE_Direction direction = TREE_DIRECTION_NONE;
		switch (key)
		{
		case TREE_KEY_UP_ARROW:
		case TREE_KEY_W:
			direction = TREE_DIRECTION_NORTH;
			break;
		case TREE_KEY_DOWN_ARROW:
		case TREE_KEY_S:
			direction = TREE_DIRECTION_SOUTH;
			break;
		case TREE_KEY_LEFT_ARROW:
		case TREE_KEY_A:
			direction = TREE_DIRECTION_WEST;
			break;
		case TREE_KEY_RIGHT_ARROW:
		case TREE_KEY_D:
			direction = TREE_DIRECTION_EAST;
			break;
		}

		// if no direction, stop
		if (direction == TREE_DIRECTION_NONE)
		{
			break;
		}

		// get next control
		TREE_Control* nextControl = application->focusedControl->adjacent[(TREE_Size)direction - 1];

		// if no control, stop
		if (!nextControl)
		{
			break;
		}

		// focus on that control
		TREE_Result result = TREE_Application_SetFocus(application, nextControl);
		if (result)
		{
			return result;
		}

		break;
	}
	}

	// dispatch the event to the application's event handler, if any
	if (application->eventHandler)
	{
		TREE_Result result = application->eventHandler(&e);
		if (result)
		{
			return result;
		}
	}

	// dispatch the event to all controls
	TREE_Control* control;
	TREE_Result result;
	for (TREE_Size i = 0; i < application->controlsSize; ++i)
	{
		control = application->controls[i];
		if (control && control->eventHandler)
		{
			// set the control for the event
			e.control = control;
			// call the event handler
			result = TREE_Control_HandleEvent(control, &e);
			// if the event handler returns an error, return it
			if (result)
			{
				return result;
			}
		}
	}

	return TREE_OK;
}

TREE_Result TREE_Application_Run(TREE_Application* application)
{
	if (!application)
	{
		return TREE_ERROR_ARG_NULL;
	}
	if (!application->surface)
	{
		return TREE_ERROR_ARG_NULL;
	}
	if (application->running)
	{
		return TREE_ERROR_ARG_INVALID;
	}
	application->running = TREE_TRUE;

	// hide cursor
	TREE_Cursor_SetVisible(TREE_FALSE);

	TREE_Result result;
	TREE_Rect dirtyRect;
	TREE_Bool shouldPresent;
	TREE_Time startTime = clock();
	TREE_Time currentTime;
	TREE_Time keyTick = startTime;
	TREE_Time const keyInterval = CLOCKS_PER_SEC / 20;
	while (application->running)
	{
		// get current time
		currentTime = clock();

		// update the dirty controls/transforms
		{
			// for refreshes
			TREE_Event event;
			event.type = TREE_EVENT_TYPE_REFRESH;
			event.data = NULL;
			event.control = NULL;
			event.application = application;

			dirtyRect.offset.x = (TREE_Int)application->surface->image.extent.width;
			dirtyRect.offset.y = (TREE_Int)application->surface->image.extent.height;
			dirtyRect.extent.width = 0;
			dirtyRect.extent.height = 0;

			TREE_Control* control;
			TREE_Bool dirty;
			TREE_Rect rect;
			for (TREE_Size i = 0; i < application->controlsSize; ++i)
			{
				control = application->controls[i];
				dirty = TREE_FALSE;

				// refresh the transform
				if (control->transform->dirty)
				{
					// keep a copy of the old global rect
					TREE_Rect oldGlobalRect = control->transform->globalRect;

					// refresh the transform
					result = TREE_Transform_Refresh(control->transform);
					if (result)
					{
						application->running = TREE_FALSE;
						return result;
					}

					// get the dirty rect (combination of old and new rects)
					rect = TREE_Rect_Combine(
						&oldGlobalRect,
						&control->transform->globalRect
					);

					// clear flag
					control->transform->dirty = TREE_FALSE;

					// update the dirty rect
					dirty = TREE_TRUE;
				}
				else
				{
					// transform did not change, so just use the global rect
					rect = control->transform->globalRect;
				}

				// refresh the control
				if (dirty || (control->stateFlags & TREE_CONTROL_STATE_FLAGS_DIRTY))
				{
					// refresh the control
					event.control = control;
					result = TREE_Control_HandleEvent(control, &event);
					if (result)
					{
						application->running = TREE_FALSE;
						return result;
					}

					// clear flag
					control->stateFlags &= ~TREE_CONTROL_STATE_FLAGS_DIRTY;

					// update the dirty rect
					dirty = TREE_TRUE;
				}

				// update the dirty rect
				if (dirty)
				{
					dirtyRect = TREE_Rect_Combine(
						&dirtyRect,
						&rect
					);
				}
			}
		}

		shouldPresent = TREE_FALSE;
		// draw the controls using the dirty rect, if there is a dirty rect
		if (dirtyRect.extent.width != 0 && dirtyRect.extent.height != 0)
		{
			// clear the surface where the dirty rect is
			TREE_Pixel pixel;
			pixel.character = ' ';
			pixel.colorPair = TREE_ColorPair_CreateDefault();
			result = TREE_Image_FillRect(
				&application->surface->image,
				dirtyRect.offset,
				dirtyRect.extent,
				pixel
			);
			if (result)
			{
				application->running = TREE_FALSE;
				return result;
			}

			// create event data
			TREE_EventData_Draw eventData;
			eventData.target = &application->surface->image;
			eventData.dirtyRect = dirtyRect;

			// create event
			TREE_Event event;
			event.type = TREE_EVENT_TYPE_DRAW;
			event.data = &eventData;
			event.control = NULL;
			event.application = application;

			// delay the drawing of the active control until the end
			TREE_Control* active = NULL;
			TREE_Control* control;

			// check each control: if it is within the dirty rect, redraw it if so
			for (TREE_Size i = 0; i < application->controlsSize; ++i)
			{
				control = application->controls[i];
				if (TREE_Rect_IsOverlapping(&dirtyRect, &control->transform->globalRect))
				{
					// if this is the active control, skip
					if (control->stateFlags & TREE_CONTROL_STATE_FLAGS_ACTIVE)
					{
						// if already one active, whoops
						if (active)
						{
							return TREE_ERROR_APPLICATION_MULTIPLE_ACTIVE_CONTROLS;
						}
						active = control;
						continue;
					}

					// set the control for the event
					event.control = control;

					// call the event handler
					result = TREE_Control_HandleEvent(control, &event);
					if (result)
					{
						application->running = TREE_FALSE;
						return result;
					}

					// there was a drawing update
					shouldPresent = TREE_TRUE;
				}
			}
			
			// draw active
			if (active)
			{
				event.control = active;
				result = TREE_Control_HandleEvent(active, &event);
				if (result)
				{
					application->running = TREE_FALSE;
					return result;
				}
				shouldPresent = TREE_TRUE;
			}
		}

		// present the surface, if there is an update to show
		if (shouldPresent)
		{
			result = TREE_Surface_Refresh(application->surface);
			if (result)
			{
				application->running = TREE_FALSE;
				return result;
			}
			result = TREE_Window_Present(application->surface);
			if (result)
			{
				application->running = TREE_FALSE;
				return result;
			}
		}

		// update key states
		{
			// update modifiers
			TREE_KeyModifierFlags modifiers = TREE_KEY_MODIFIER_FLAGS_NONE;
#ifdef TREE_WINDOWS
			if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
			{
				modifiers |= TREE_KEY_MODIFIER_FLAGS_SHIFT;
			}
			if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
			{
				modifiers |= TREE_KEY_MODIFIER_FLAGS_CONTROL;
			}
			if (GetAsyncKeyState(VK_MENU) & 0x8000)
			{
				modifiers |= TREE_KEY_MODIFIER_FLAGS_ALT;
			}
			if (GetAsyncKeyState(VK_LWIN) & 0x8000 || GetAsyncKeyState(VK_RWIN) & 0x8000)
			{
				modifiers |= TREE_KEY_MODIFIER_FLAGS_COMMAND;
			}
			if (GetKeyState(VK_CAPITAL) & 0x0001)
			{
				modifiers |= TREE_KEY_MODIFIER_FLAGS_CAPS_LOCK;
			}
			if (GetKeyState(VK_NUMLOCK) & 0x0001)
			{
				modifiers |= TREE_KEY_MODIFIER_FLAGS_NUM_LOCK;
			}
			if (GetKeyState(VK_SCROLL) & 0x0001)
			{
				modifiers |= TREE_KEY_MODIFIER_FLAGS_SCROLL_LOCK;
			}
#endif
			application->input.modifiers = modifiers;

			// create event data
			TREE_EventData_Key eventData;
			eventData.key = TREE_KEY_NONE;
			eventData.modifiers = modifiers;

			// create event
			TREE_Event event;
			event.type = TREE_EVENT_TYPE_NONE;
			event.data = &eventData;
			event.control = NULL;
			event.application = application;

			// check if key tick is active
			TREE_Bool keyTickActive = (currentTime - keyTick) >= keyInterval;
			if (keyTickActive)
			{
				// if active, reset timer
				keyTick = currentTime;
			}

			// update key states
#ifdef TREE_WINDOWS
			for (TREE_Size i = 0; i < TREE_KEY_COUNT; i++)
			{
				TREE_Key key = application->input.keys[i];
				// check if being pressed right now
				TREE_Bool pressed = (GetAsyncKeyState(key) & 0x8000) ? TREE_TRUE : TREE_FALSE;

				// get the key state
				TREE_InputState state = application->input.states[key];

				// check for changes
				if (pressed)
				{
					if (state == TREE_INPUT_STATE_RELEASED)
					{
						// key down
						application->input.states[key] = TREE_INPUT_STATE_COOLDOWN;

						// trigger key down event
						event.type = TREE_EVENT_TYPE_KEY_DOWN;
						eventData.key = key;
						result = TREE_Application_DispatchEvent(application, &event);
						if (result)
						{
							application->running = TREE_FALSE;
							return result;
						}
					}
					else if (keyTickActive)
					{
						// keep cooling down, if able
						if (state > TREE_INPUT_STATE_HELD)
						{
							state--;
							application->input.states[key] = state;
						}

						// held state
						if (state == TREE_INPUT_STATE_HELD)
						{
							// trigger key held event
							event.type = TREE_EVENT_TYPE_KEY_HELD;
							eventData.key = key;
							result = TREE_Application_DispatchEvent(application, &event);
							if (result)
							{
								application->running = TREE_FALSE;
								return result;
							}
						}
					}
				}
				else
				{
					if (state)
					{
						// key up
						application->input.states[key] = TREE_INPUT_STATE_RELEASED;

						// trigger key up event
						event.type = TREE_EVENT_TYPE_KEY_UP;
						eventData.key = key;
						result = TREE_Application_DispatchEvent(application, &event);
						if (result)
						{
							application->running = TREE_FALSE;
							return result;
						}
					}
				}
			}
#endif // TREE_WINDOWS
		}
	}

	// show cursor
	TREE_Cursor_SetVisible(TREE_TRUE);

	return TREE_OK;
}

void TREE_Application_Quit(TREE_Application* application)
{
	// validate
	if (!application)
	{
		return;
	}

	// set running to false
	application->running = TREE_FALSE;
}
