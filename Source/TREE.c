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

TREE_Char const* TREE_Result_ToString(TREE_Result code)
{
	switch (code)
	{
	case TREE_OK:
		return "OK";
	case TREE_CANCEL:
		return "Cancel";
	case TREE_ERROR:
		return "General error";
	case TREE_NOT_IMPLEMENTED:
		return "Not implemented";
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
	return (TREE_Size)offset.y * image->size.width + offset.x;
}

TREE_Result TREE_Image_Init(TREE_Image* image, TREE_Extent size)
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

TREE_Result TREE_Image_Set(TREE_Image* image, TREE_Offset offset, TREE_Char character, TREE_ColorPair colorPair)
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

TREE_Result TREE_Image_Get(TREE_Image* image, TREE_Offset offset, TREE_Char* character, TREE_ColorPair* colorPair)
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

TREE_Result TREE_Image_DrawImage(TREE_Image* image, TREE_Offset offset, TREE_Image* other)
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

TREE_Result TREE_Image_DrawString(TREE_Image* image, TREE_Offset offset, TREE_String string, TREE_ColorPair colorPair)
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
	TREE_Size pixelCount = (TREE_Size)(image->size.width * image->size.height);
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

TREE_Char TREE_Key_ToChar(TREE_Key key)
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

TREE_String TREE_Key_ToString(TREE_Key key)
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
	case TREE_KEY_DOUBLE_QUOTE: return "DOUBLE QUOTE";
	case TREE_KEY_HASH: return "HASH";
	case TREE_KEY_DOLLAR: return "DOLLAR";
	case TREE_KEY_PERCENT: return "PERCENT";
	case TREE_KEY_AMPERSAND: return "AMPERSAND";
	case TREE_KEY_SINGLE_QUOTE: return "SINGLE QUOTE";
	case TREE_KEY_LEFT_PARENTHESIS: return "LEFT PARENTHESIS";
	case TREE_KEY_RIGHT_PARENTHESIS: return "RIGHT PARENTHESIS";
	case TREE_KEY_ASTERISK: return "ASTERISK";
	case TREE_KEY_PLUS: return "PLUS";
	case TREE_KEY_COMMA: return "COMMA";
	case TREE_KEY_MINUS: return "MINUS";
	case TREE_KEY_PERIOD: return "PERIOD";
	case TREE_KEY_SLASH: return "SLASH";
	case TREE_KEY_0: return "ZERO";
	case TREE_KEY_1: return "ONE";
	case TREE_KEY_2: return "TWO";
	case TREE_KEY_3: return "THREE";
	case TREE_KEY_4: return "FOUR";
	case TREE_KEY_5: return "FIVE";
	case TREE_KEY_6: return "SIX";
	case TREE_KEY_7: return "SEVEN";
	case TREE_KEY_8: return "EIGHT";
	case TREE_KEY_9: return "NINE";
	case TREE_KEY_COLON: return "COLON";
	case TREE_KEY_SEMICOLON: return "SEMICOLON";
	case TREE_KEY_LESS_THAN: return "LESS THAN";
	case TREE_KEY_EQUAL: return "EQUAL";
	case TREE_KEY_GREATER_THAN: return "GREATER THAN";
	case TREE_KEY_QUESTION: return "QUESTION";
	case TREE_KEY_AT: return "AT";
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
	case TREE_KEY_LEFT_BRACKET: return "LEFT BRACKET";
	case TREE_KEY_BACKSLASH: return "BACKSLASH";
	case TREE_KEY_RIGHT_BRACKET: return "RIGHT BRACKET";
	case TREE_KEY_CARET: return "CARET";
	case TREE_KEY_UNDERSCORE: return "UNDERSCORE";
	case TREE_KEY_GRAVE_ACCENT: return "ACCENT";
	case TREE_KEY_a: return "a";
	case TREE_KEY_b: return "b";
	case TREE_KEY_c: return "c";
	case TREE_KEY_d: return "d";
	case TREE_KEY_e: return "e";
	case TREE_KEY_f: return "f";
	case TREE_KEY_g: return "g";
	case TREE_KEY_h: return "h";
	case TREE_KEY_i: return "i";
	case TREE_KEY_j: return "j";
	case TREE_KEY_k: return "k";
	case TREE_KEY_l: return "l";
	case TREE_KEY_m: return "m";
	case TREE_KEY_n: return "n";
	case TREE_KEY_o: return "o";
	case TREE_KEY_p: return "p";
	case TREE_KEY_q: return "q";
	case TREE_KEY_r: return "r";
	case TREE_KEY_s: return "s";
	case TREE_KEY_t: return "t";
	case TREE_KEY_u: return "u";
	case TREE_KEY_v: return "v";
	case TREE_KEY_w: return "w";
	case TREE_KEY_x: return "x";
	case TREE_KEY_y: return "y";
	case TREE_KEY_z: return "z";
	case TREE_KEY_LEFT_CURLY_BRACE: return "LEFT CURLY BRACE";
	case TREE_KEY_PIPE: return "PIPE";
	case TREE_KEY_RIGHT_CURLY_BRACE: return "RIGHT CURLY BRACE";
	case TREE_KEY_TILDE: return "TILDE";
	case TREE_KEY_DELETE: return "DELETE";
	case TREE_KEY_UP: return "UP";
	case TREE_KEY_DOWN: return "DOWN";
	case TREE_KEY_LEFT: return "LEFT";
	case TREE_KEY_RIGHT: return "RIGHT";
	case TREE_KEY_PAGE_UP: return "PAGE UP";
	case TREE_KEY_PAGE_DOWN: return "PAGE DOWN";
	case TREE_KEY_HOME: return "HOME";
	case TREE_KEY_END: return "END";
	case TREE_KEY_INSERT: return "INSERT";
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
	case TREE_KEY_NUMPAD_0: return "NUMPAD 0";
	case TREE_KEY_NUMPAD_1: return "NUMPAD 1";
	case TREE_KEY_NUMPAD_2: return "NUMPAD 2";
	case TREE_KEY_NUMPAD_3: return "NUMPAD 3";
	case TREE_KEY_NUMPAD_4: return "NUMPAD 4";
	case TREE_KEY_NUMPAD_5: return "NUMPAD 5";
	case TREE_KEY_NUMPAD_6: return "NUMPAD 6";
	case TREE_KEY_NUMPAD_7: return "NUMPAD 7";
	case TREE_KEY_NUMPAD_8: return "NUMPAD 8";
	case TREE_KEY_NUMPAD_9: return "NUMPAD 9";
	case TREE_KEY_NUMPAD_MULTIPLY: return "NUMPAD MULTIPLY";
	case TREE_KEY_NUMPAD_ADD: return "NUMPAD ADD";
	case TREE_KEY_NUMPAD_SUBTRACT: return "NUMPAD SUBTRACT";
	case TREE_KEY_NUMPAD_DECIMAL: return "NUMPAD DECIMAL";
	case TREE_KEY_NUMPAD_DIVIDE: return "NUMPAD DIVIDE";
	case TREE_KEY_NUMPAD_ENTER: return"NUMPAD ENTER";
	default: return "";
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

	// set data
	control->type = TREE_CONTROL_TYPE_NONE;
	control->flags = TREE_CONTROL_FLAGS_NONE;
	control->stateFlags = TREE_CONTROL_STATE_FLAGS_NONE;
	TREE_Transform_Init(&control->transform, (TREE_Offset) { 0, 0 }, (TREE_Coords) { 0.0f, 0.0f }, (TREE_Extent) { 0, 0 }, TREE_ALIGNMENT_TOPLEFT);
	memset(control->adjacent, 0, 4 * sizeof(TREE_Control*));
	control->eventHandler = eventHandler;
	control->data = data;

	if (parent)
	{
		TREE_Result result = TREE_Transform_SetParent(&control->transform, parent);
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
	TREE_Transform_Free(&control->transform);
	memset(control->adjacent, 0, 4 * sizeof(TREE_Control*));
	control->eventHandler = NULL;
	control->data = NULL;
}

TREE_Result TREE_Control_Refresh(TREE_Control* control)
{
	// validate
	if (!control)
	{
		return TREE_ERROR_ARG_NULL;
	}

	// ignore if clean
	if (!(control->stateFlags & TREE_CONTROL_STATE_FLAGS_DIRTY))
	{
		return TREE_OK;
	}

	// set as clean
	control->stateFlags &= ~TREE_CONTROL_STATE_FLAGS_DIRTY;

	// refresh the transform
	TREE_Result result = TREE_Transform_Refresh(&control->transform);
	if (result)
	{
		return result;
	}

	return TREE_OK;
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
	if (!control || !event || !event->data)
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

TREE_Result TREE_Control_Dirty(TREE_Control* control)
{
	// validate
	if (!control)
	{
		return TREE_ERROR_ARG_NULL;
	}

	// set as dirty
	control->stateFlags |= TREE_CONTROL_STATE_FLAGS_DIRTY;

	return TREE_OK;
}

TREE_Result TREE_Control_LabelData_Init(TREE_Control_LabelData* data, TREE_String text, TREE_ColorPair normalColor)
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
	data->normalColor = normalColor;

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
	data->normalColor = TREE_ColorPair_CreateDefault();
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
	control->transform.localExtent.width = (TREE_UInt)strlen(data->text);
	control->transform.localExtent.height = 1;
	control->type = TREE_CONTROL_TYPE_LABEL;

	return TREE_OK;
}

TREE_Result TREE_Control_Label_SetText(TREE_Control* control, TREE_String text, TREE_ColorPair colorPair)
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

	// dirty the control so it gets redrawn
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

TREE_Result TREE_Control_Label_EventHandler(TREE_Event const* event)
{
	// validate
	if (!event || !event->control || event->control->type != TREE_CONTROL_TYPE_LABEL)
	{
		return TREE_ERROR_ARG_NULL;
	}

	// get the data
	TREE_Control_LabelData* labelData = (TREE_Control_LabelData*)event->control->data;

	// handle the event
	switch (event->type)
	{
	case TREE_EVENT_TYPE_DRAW:
	{
		// get the event data
		TREE_EventData_Draw* drawData = (TREE_EventData_Draw*)event->data;
		TREE_Image* target = drawData->target;

		// draw the label
		TREE_Transform* transform = &event->control->transform;
		TREE_Result result = TREE_Image_DrawString(
			target,
			transform->globalRect.offset,
			labelData->text,
			labelData->normalColor
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
	application->controls = (TREE_Control**)malloc(capacity * sizeof(TREE_Control*));
	if (!application->controls)
	{
		return TREE_ERROR_ALLOC;
	}

	// set data
	application->controlsCapacity = capacity;
	application->controlsSize = 0;
	application->running = TREE_FALSE;
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
	application->running = TREE_FALSE;
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
		return TREE_ERROR;
	}

	// add to the application
	application->controls[application->controlsSize] = control;
	application->controlsSize++;

	return TREE_OK;
}

TREE_Result TREE_Application_DispatchEvent(TREE_Application const* application, TREE_Event const* event)
{
	// validate
	if (!application || !event)
	{
		return TREE_ERROR_ARG_NULL;
	}
	
	TREE_Event e = *event; // local copy to edit

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

	TREE_Result result;
	while (application->running)
	{
		// get the next key
		TREE_Key key = TREE_Input_GetKey();

		// if key pressed, process it
		if (key != TREE_KEY_NULL)
		{
			// create event data
			TREE_EventData_InputKey eventData;
			eventData.key = key;

			// create event
			TREE_Event event;
			event.type = TREE_EVENT_TYPE_INPUT_KEY;
			event.data = &eventData;
			event.control = NULL;

			// trigger for each event
			result = TREE_Application_DispatchEvent(application, &event);
			if (result)
			{
				application->running = TREE_FALSE;
				return result;
			}

			// if the application is not running, break the loop
			if (!application->running)
			{
				break;
			}
		}

		// draw the controls
		{
			// create event data
			TREE_EventData_Draw eventData;
			eventData.target = &application->surface->image;

			// create event
			TREE_Event event;
			event.type = TREE_EVENT_TYPE_DRAW;
			event.data = &eventData;
			event.control = NULL;

			// check each control
			// if it is dirty, refresh it and redraw it
			for (TREE_Size i = 0; i < application->controlsSize; ++i)
			{
				TREE_Control* control = application->controls[i];
				TREE_Bool redraw = TREE_FALSE;
				if (control->transform.dirty)
				{
					result = TREE_Transform_Refresh(&control->transform);
					if (result)
					{
						application->running = TREE_FALSE;
						return result;
					}
					control->transform.dirty = TREE_FALSE; // clear dirty flag

					redraw = TREE_TRUE;
				}
				if (control->stateFlags & TREE_CONTROL_STATE_FLAGS_DIRTY)
				{
					control->stateFlags &= ~TREE_CONTROL_STATE_FLAGS_DIRTY; // clear dirty flag
					redraw = TREE_TRUE;
				}
				if (redraw)
				{
					event.control = control; // set the control for the event
					result = TREE_Control_HandleEvent(control, &event);
					if (result)
					{
						application->running = TREE_FALSE;
						return result;
					}
				}
			}
		}

		// present the surface
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
