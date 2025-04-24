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

	// create list data
    TREE_String options[] = { 
					"Option 1", "Option 2", "Option 3", "Option 4", "Option 5", 
					"Option 6", "Option 7", "Option 8", "Option 9", "Option 10", 
					"Option 11", "Option 12", "Option 13", "Option 14", "Option 15", 
					"Option 16", "Option 17", "Option 18", "Option 19", "Option 20" 
    };
	TREE_Control_ListData listData;
	result = TREE_Control_ListData_Init(&listData, TREE_CONTROL_LIST_FLAGS_MULTISELECT, options, 20, NULL, NULL);
	if (result)
	{
		printf("Failed to initialize list data: %s\n", TREE_Result_ToString(result));
		return 1;
	}
	listData.scrollbar.type = TREE_CONTROL_SCROLLBAR_TYPE_STATIC;

	// create list
	TREE_Control listControl;
	result = TREE_Control_List_Init(&listControl, NULL, &listData);
	if (result)
	{
		printf("Failed to initialize list control: %s\n", TREE_Result_ToString(result));
		return 1;
	}
	listControl.transform->localOffset.x = 5;
	listControl.transform->localOffset.y = 5;
	listControl.transform->localExtent.height = 15;

	// link controls
	result = TREE_Control_Link(&quitButton, TREE_DIRECTION_SOUTH, TREE_CONTROL_LINK_DOUBLE, &listControl);
	if (result)
	{
		printf("Failed to link controls: %s\n", TREE_Result_ToString(result));
		return 1;
	}

	// add text input to application
	result = TREE_Application_AddControl(&app, &listControl);
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
	TREE_Control_Free(&listControl);
	TREE_Control_ListData_Free(&listData);
	TREE_Control_ButtonData_Free(&quitButtonData);
	TREE_Application_Free(&app);
	TREE_Surface_Free(&surface);
	TREE_Free();

	printf("Application ran successfully!\n");

	return 0;
}