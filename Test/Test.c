#include "TREE.h"
#include <stdio.h>
#include <time.h>

TREE_Application* g_application = NULL;

TREE_Result ApplicationEventHandler(TREE_Event const* event)
{
	switch (event->type)
	{
	case TREE_EVENT_TYPE_KEY_DOWN:
	{
		// get the event data
		TREE_EventData_Key const* eventData = (TREE_EventData_Key const*)event->data;

		// quit on escape
		if (eventData->key == TREE_KEY_ESCAPE)
		{
			TREE_Application_Quit(g_application);
		}

		break;
	}
	}

	return TREE_OK;
}

int main()
{
	TREE_Result result;

	// create the surface to draw to
	TREE_Surface surface;
	result = TREE_Surface_Init(&surface, TREE_Window_GetExtent());
	if (result)
	{
		printf("Failed to initialize surface: %s\n", TREE_Result_ToString(result));
		return 1;
	}

	// create the application
	TREE_Application app;
	g_application = &app; // Store the application globally for the event handler
	result = TREE_Application_Init(&app, &surface, 10, ApplicationEventHandler);
	if (result)
	{
		printf("Failed to initialize application: %s\n", TREE_Result_ToString(result));
		return 1;
	}
	
	// create button datas
#define BUTTON_COUNT 3
	TREE_Control_ButtonData buttonDatas[BUTTON_COUNT];
	TREE_Pixel normalPixel = { ' ', TREE_ColorPair_Create(TREE_COLOR_BRIGHT_WHITE, TREE_COLOR_BRIGHT_BLACK) };
	TREE_Pixel focusedPixel = { ' ', TREE_ColorPair_Create(TREE_COLOR_BRIGHT_BLACK, TREE_COLOR_BRIGHT_WHITE) };
	TREE_Pixel pressedPixel = { ' ', TREE_ColorPair_Create(TREE_COLOR_BRIGHT_BLACK, TREE_COLOR_WHITE) };
	for (TREE_Size i = 0; i < BUTTON_COUNT; i++)
	{
		result = TREE_Control_ButtonData_Init(&buttonDatas[i], "Button", normalPixel, focusedPixel, pressedPixel, NULL);
		if (result)
		{
			printf("Failed to initialize button data %zu: %s\n", i, TREE_Result_ToString(result));
			return 1;
		}
	}

	// create buttons
	TREE_Control buttons[BUTTON_COUNT];
	for (TREE_Int i = 0; i < BUTTON_COUNT; i++)
	{
		result = TREE_Control_Button_Init(&buttons[i], NULL, &buttonDatas[i]);
		if (result)
		{
			printf("Failed to initialize button %d: %s\n", i, TREE_Result_ToString(result));
			return 1;
		}

		// move button
		TREE_Transform* transform = buttons[i].transform;
		transform->localOffset.y = 1 + i;
		transform->localOffset.x = 22 * i;
	}

	// link buttons together
	for (TREE_Size i = 0; i < BUTTON_COUNT - 1; i++)
	{
		TREE_Control* button = &buttons[i];
		TREE_Control* nextButton = &buttons[i + 1];
		result = TREE_Control_Link(button, TREE_DIRECTION_EAST, TREE_CONTROL_LINK_DOUBLE, nextButton);
		if (result)
		{
			printf("Failed to link button %zu to button %zu: %s\n", i, i + 1, TREE_Result_ToString(result));
			return 1;
		}
	}

	// add buttons to application
	for (TREE_Size i = 0; i < BUTTON_COUNT; i++)
	{
		result = TREE_Application_AddControl(&app, &buttons[i]);
		if (result)
		{
			printf("Failed to add button %zu to application: %s\n", i, TREE_Result_ToString(result));
			return 1;
		}
	}

	// run the application
	result = TREE_Application_Run(&app);
	if (result)
	{
		printf("Failed to run application: %s\n", TREE_Result_ToString(result));
		return 1;
	}

	// cleanup
	for (TREE_Size i = 0; i < BUTTON_COUNT; i++)
	{
		TREE_Control_Free(&buttons[i]);
		TREE_Control_ButtonData_Free(&buttonDatas[i]);
	}
	TREE_Application_Free(&app);
	TREE_Surface_Free(&surface);

	printf("Application ran successfully!\n");

	return 0;
}