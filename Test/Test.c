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

	result = TREE_Init();
	if (result)
	{
		printf("Failed to initialize TREE: %s\n", TREE_Result_ToString(result));
		return 1;
	}

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
	
	// create button data
	TREE_Control_ButtonData quitButtonData;
	result = TREE_Control_ButtonData_Init(&quitButtonData, "Quit", Button_Quit);
	if (result)
	{
		printf("Failed to initialize quit button data: %s\n", TREE_Result_ToString(result));
		return 1;
	}

	// create button
	TREE_Control quitButton;
	result = TREE_Control_Button_Init(&quitButton, NULL, &quitButtonData);
	if (result)
	{
		printf("Failed to initialize quit button: %s\n", TREE_Result_ToString(result));
		return 1;
	}
	TREE_Transform* transform = quitButton.transform;
	transform->localOffset.y = 1;
	transform->localOffset.x = 1;

	// add button
	result = TREE_Application_AddControl(&app, &quitButton);
	if (result)
	{
		printf("Failed to add quit button to application: %s\n", TREE_Result_ToString(result));
		return 1;
	}

	// create text input data
	TREE_Control_TextInputData textInputData;
	result = TREE_Control_TextInputData_Init(&textInputData, "\nThis is some example text!\nYes!\n\nNo!\nOk!\n\n", 256, "Enter text", TREE_CONTROL_TEXT_INPUT_TYPE_NORMAL, NULL, NULL);
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
	textInput.transform->localExtent.width = 90;

	// link button to text input
	result = TREE_Control_Link(&quitButton, TREE_DIRECTION_SOUTH, TREE_CONTROL_LINK_DOUBLE, &textInput);
	if (result)
	{
		printf("Failed to link text input to quit button: %s\n", TREE_Result_ToString(result));
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
	TREE_Control_Free(&quitButton);
	TREE_Control_Free(&textInput);
	TREE_Control_TextInputData_Free(&textInputData);
	TREE_Control_ButtonData_Free(&quitButtonData);
	TREE_Application_Free(&app);
	TREE_Surface_Free(&surface);
	TREE_Free();

	printf("Application ran successfully!\n");

	return 0;
}