#include "TREE.h"
#include <stdio.h>
#include <time.h>

TREE_Application* g_application = NULL;

void Button_Quit(void* sender)
{
	TREE_Application_Quit(g_application);
}

TREE_Result ApplicationEventHandler(TREE_Event const* event)
{
	switch (event->type)
	{
	case TREE_EVENT_TYPE_KEY_DOWN:
	{
		// get the event data
		TREE_EventData_Key const* eventData = (TREE_EventData_Key const*)event->data;

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
	for (TREE_Size i = 0; i < BUTTON_COUNT; i++)
	{
		TREE_Control_ButtonData* buttonData = &buttonDatas[i];
		result = TREE_Control_ButtonData_Init(buttonData, i == 0 ? "Quit" : "Button", i == 0 ? Button_Quit : NULL);
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

	// create text input data
	TREE_Control_TextInputData textInputData;
	result = TREE_Control_TextInputData_Init(&textInputData, "", 256, "Enter text", TREE_CONTROL_TEXT_INPUT_TYPE_NORMAL, NULL, NULL);
	if (result)
	{
		printf("Failed to initialize text input data: %s\n", TREE_Result_ToString(result));
		return 1;
	}

	// create text input control
	TREE_Control textInput;
	result = TREE_Control_TextInput_Init(&textInput, NULL, &textInputData);
	if (result)
	{
		printf("Failed to initialize text input control: %s\n", TREE_Result_ToString(result));
		return 1;
	}
	textInput.transform->localOffset = (TREE_Offset){ 2, 12 };

	// link buttons to text input
	for (TREE_Size i = 0; i < BUTTON_COUNT; i++)
	{
		TREE_Control* button = &buttons[i];
		result = TREE_Control_Link(button, TREE_DIRECTION_SOUTH, TREE_CONTROL_LINK_SINGLE, &textInput);
		if (result)
		{
			printf("Failed to link button %zu to text input: %s\n", i, TREE_Result_ToString(result));
			return 1;
		}
	}

	// link text input to first button
	result = TREE_Control_Link(&textInput, TREE_DIRECTION_NORTH, TREE_CONTROL_LINK_SINGLE, &buttons[0]);
	if (result)
	{
		printf("Failed to link text input to button 0: %s\n", TREE_Result_ToString(result));
		return 1;
	}

	// add text input to application
	result = TREE_Application_AddControl(&app, &textInput);
	if (result)
	{
		printf("Failed to add text input to application: %s\n", TREE_Result_ToString(result));
		return 1;
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