#include "pch.h"
#include "framework.h"
#include "TREE.h"
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#define TREE_WINDOWS
#endif // WIN32

#ifdef TREE_WINDOWS
#include <windows.h>
#include <conio.h>
#endif // TREE_WINDOWS

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

TREE_Char const* TREE_GetErrorString(TREE_ErrorCode code)
{
	switch (code)
	{
	case TREE_OK:
		return "OK";
	case TREE_ERROR:
		return "Error";
	default:
		return "Unknown error";
	}
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

TREE_Pixel TREE_Pixel_CreateDefault()
{
	TREE_Pixel pixel;
	pixel.character = ' ';
	pixel.colorPair = TREE_ColorPair_CreateDefault();
	return pixel;
}

TREE_ErrorCode TREE_Pattern_Init(TREE_Pattern* pattern, TREE_UInt size)
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

TREE_ErrorCode TREE_Pattern_InitFromString(TREE_Pattern* pattern, TREE_String string, TREE_ColorPair colorPair)
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

TREE_ErrorCode TREE_Pattern_Set(TREE_Pattern* pattern, TREE_UInt index, TREE_Pixel pixel)
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

TREE_ErrorCode TREE_Pattern_Get(TREE_Pattern* pattern, TREE_UInt index, TREE_Pixel* pixel)
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
	return (TREE_Size)offset.y * image->size.width + offset.x;
}

TREE_ErrorCode TREE_Image_Init(TREE_Image* image, TREE_Extent size)
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

	// calculate sizes
	TREE_Size imageSize = (TREE_Size)(size.width * size.height);
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
	image->size = size;

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
	image->size.width = 0;
	image->size.height = 0;
}

TREE_ErrorCode TREE_Image_Set(TREE_Image* image, TREE_Offset offset, TREE_Char character, TREE_ColorPair colorPair)
{
	// validate
	if (!image)
	{
		return TREE_ERROR_ARG_NULL;
	}
	if (offset.x < 0 || offset.y < 0 ||
		offset.x >= (TREE_Int)image->size.width ||
		offset.y >= (TREE_Int)image->size.height)
	{
		return TREE_ERROR_ARG_OUT_OF_RANGE;
	}

	// set data
	TREE_Size index = _TREE_Image_GetIndex(image, offset);
	image->text[index] = character;
	image->colors[index] = colorPair;

	return TREE_OK;
}

TREE_ErrorCode TREE_Image_Get(TREE_Image* image, TREE_Offset offset, TREE_Char* character, TREE_ColorPair* colorPair)
{
	// validate
	if (!image || !character || !colorPair)
	{
		return TREE_ERROR_ARG_NULL;
	}
	if (offset.x < 0 || offset.y < 0 ||
		offset.x >= (TREE_Int)image->size.width ||
		offset.y >= (TREE_Int)image->size.height)
	{
		return TREE_ERROR_ARG_OUT_OF_RANGE;
	}

	// get data
	TREE_Size index = _TREE_Image_GetIndex(image, offset);
	*character = image->text[index];
	*colorPair = image->colors[index];

	return TREE_OK;
}

TREE_ErrorCode TREE_Image_DrawImage(TREE_Image* image, TREE_Offset offset, TREE_Image* other)
{
	// validate
	if (!image || !other)
	{
		return TREE_ERROR_ARG_NULL;
	}

	// ignore if out of bounds
	if (offset.x + other->size.width <= 0 || offset.y + other->size.height <= 0 ||
		offset.x >= (TREE_Int)image->size.width ||
		offset.y >= (TREE_Int)image->size.height)
	{
		return TREE_OK;
	}

	// calculate sizes
	TREE_UInt otherOffsetX = offset.x < 0 ? -offset.x : 0;
	TREE_UInt otherOffsetY = offset.y < 0 ? -offset.y : 0;
	TREE_UInt offsetX = offset.x < 0 ? 0 : offset.x;
	TREE_UInt offsetY = offset.y < 0 ? 0 : offset.y;
	TREE_UInt width = MIN(other->size.width, image->size.width - offsetX);
	TREE_UInt height = MIN(other->size.height, image->size.height - offsetY);
	TREE_Size textCopySize = width * sizeof(TREE_Char);
	TREE_Size colorCopySize = width * sizeof(TREE_ColorPair);

	// draw the image
	TREE_UInt index, otherIndex;
	for (TREE_UInt row = 0; row < height; ++row)
	{
		// get indexes to "pixel"
		index = (row + offsetY) * image->size.width + offsetX;
		otherIndex = (row + otherOffsetY) * other->size.width + otherOffsetX;

		// copy data over from other
		memcpy(&image->text[index], &other->text[otherIndex], textCopySize);
		memcpy(&image->colors[index], &other->colors[otherIndex], colorCopySize);
	}

	return TREE_OK;
}

TREE_ErrorCode TREE_Image_DrawString(TREE_Image* image, TREE_Offset offset, TREE_String string, TREE_ColorPair colorPair)
{
	if (!image || !string)
	{
		return TREE_ERROR_ARG_NULL;
	}

	TREE_Size stringLength = strlen(string);

	// ignore if out of bounds
	if (offset.x + stringLength <= 0 || offset.y < 0 ||
		offset.x >= (TREE_Int)image->size.width ||
		offset.y >= (TREE_Int)image->size.height)
	{
		return TREE_OK;
	}

	// calculate sizes
	TREE_UInt stringOffsetX = offset.x < 0 ? -offset.x : 0;
	TREE_UInt offsetX = offset.x < 0 ? 0 : offset.x;
	TREE_UInt width = MIN((TREE_UInt)stringLength, image->size.width - offsetX);

	// draw the string
	TREE_UInt index = offset.y * image->size.width + offsetX;
	TREE_UInt stringIndex = stringOffsetX;
	memcpy(&image->text[index], &string[stringIndex], width * sizeof(TREE_Char));
	memset(&image->colors[index], colorPair, width * sizeof(TREE_Byte));

	return TREE_OK;
}

TREE_ErrorCode TREE_Image_DrawLine(TREE_Image* image, TREE_Offset start, TREE_Offset end, TREE_Pattern* pattern)
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
			pixel.character,
			pixel.colorPair
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

TREE_ErrorCode TREE_Image_DrawRect(TREE_Image* image, TREE_Offset start, TREE_Extent size, TREE_Pattern* pattern)
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
	TREE_ErrorCode result = TREE_Image_DrawLine(image, p0, p1, pattern);
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

TREE_ErrorCode TREE_Image_FillRect(TREE_Image* image, TREE_Offset start, TREE_Extent size, TREE_Pixel pixel)
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
	TREE_Int endX = MIN(start.x + (TREE_Int)size.width, (TREE_Int)image->size.width);
	TREE_Int endY = MIN(start.y + (TREE_Int)size.height, (TREE_Int)image->size.height);

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

TREE_ErrorCode TREE_Image_Clear(TREE_Image* image, TREE_Pixel pixel)
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
	TREE_Size pixelCount = (TREE_Size)(image->size.width * image->size.height);
	memset(image->text, pixel.character, pixelCount * sizeof(TREE_Char));
	memset(image->colors, pixel.colorPair, pixelCount * sizeof(TREE_ColorPair));

	return TREE_OK;
}

TREE_ErrorCode TREE_Surface_Init(TREE_Surface* surface, TREE_Extent size)
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
	TREE_ErrorCode code = TREE_Image_Init(&surface->image, size);
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

TREE_ErrorCode TREE_Surface_Refresh(TREE_Surface* surface)
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
	TREE_Size pixelCount = (TREE_Size)(image->size.width * image->size.height);
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
		return TREE_ERROR;
	}

	return TREE_OK;
}

TREE_ErrorCode TREE_Window_Present(TREE_Surface* surface)
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

	// print to the console
	int result = printf("%s", surface->text);
	if (result)
	{
		return TREE_ERROR;
	}

	// flush the output
	result = fflush(stdout);
	if (result)
	{
		return TREE_ERROR;
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

TREE_Char TREE_Key_GetChar(TREE_Key key)
{
	if (key >= 128)
	{
		return '\0';
	}

	switch (key)
	{
	case TREE_KEY_NULL: return '\0';
	case TREE_KEY_BACKSPACE: return '\b';
	case TREE_KEY_TAB: return '\t';
	case TREE_KEY_ENTER: return '\n';
	case TREE_KEY_ESCAPE: return '\033';
	default:
		return (TREE_Char)key;
	};
}

TREE_String TREE_Key_GetString(TREE_Key key)
{
	switch (key)
	{
	case TREE_KEY_NULL: return "NULL";
	case TREE_KEY_BACKSPACE: return "BACKSPACE";
	case TREE_KEY_TAB: return "TAB";
	case TREE_KEY_ENTER: return "ENTER";
	case TREE_KEY_ESCAPE: return "ESCAPE";
	case TREE_KEY_SPACE: return "SPACE";
	case TREE_KEY_EXCLAMATION: return "EXCLAMATION";

	};
}

TREE_Key TREE_Input_GetKey()
{
#ifdef TREE_WINDOWS
	// check if key pressed
	if (_kbhit())
	{
		int ch = _getch();
		if (ch == 0 || ch == 224)
		{
			// special key
			ch = _getch();
			switch (ch)
			{
			case 72: return TREE_KEY_UP;
			case 80: return TREE_KEY_DOWN;
			case 75: return TREE_KEY_LEFT;
			case 77: return TREE_KEY_RIGHT;
			default: return TREE_KEY_NULL;
			}
		}
		else
		{
			// check if the key is a numpad key
			SHORT state = GetAsyncKeyState(VK_NUMLOCK); // check if Num Lock is on
			int isNumLockOn = (state & 0x0001) != 0;

			if (isNumLockOn)
			{
				for (int i = VK_NUMPAD0; i <= VK_NUMPAD9; ++i)
				{
					// check if numpad key pressed
					if (GetAsyncKeyState(i) & 0x8000)
					{
						return TREE_KEY_NUMPAD_0 + (i - VK_NUMPAD0);
					}
				}
			}

			// normal key
			return (TREE_Key)ch;
		}
	}
#endif // TREE_WINDOWS

	// no key pressed
	return TREE_KEY_NULL;
}
