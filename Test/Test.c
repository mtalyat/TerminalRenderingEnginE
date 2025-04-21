#include "TREE.h"
#include <stdio.h>
#include <time.h>

TREE_Application* g_application = NULL;

TREE_Result ApplicationEventHandler(TREE_Event const* event)
{
	switch (event->type)
	{
	case TREE_EVENT_TYPE_KEY_DOWN:
	case TREE_EVENT_TYPE_KEY_HELD:
	{
		TREE_EventData_Key const* eventData = (TREE_EventData_Key const*)event->data;
		// quit on escape
		if (eventData->key == TREE_KEY_ESCAPE)
		{
			TREE_Application_Quit(g_application);
		}
		// update label with key pressed
		TREE_Control* control = g_application->controls[0];

		switch (eventData->key)
		{
		case TREE_KEY_LEFT_ARROW:
			if (control->transform->localExtent.width > 0)
			{
				control->transform->localExtent.width--;
			}
			control->transform->dirty = TREE_TRUE;
			break;
		case TREE_KEY_RIGHT_ARROW:
			control->transform->localExtent.width++;
			control->transform->dirty = TREE_TRUE;
			break;
		case TREE_KEY_UP_ARROW:
			if (control->transform->localExtent.height > 0)
			{
				control->transform->localExtent.height--;
			}
			control->transform->dirty = TREE_TRUE;
			break;
		case TREE_KEY_DOWN_ARROW:
			control->transform->localExtent.height++;
			control->transform->dirty = TREE_TRUE;
			break;
		}

		TREE_Control_Label_SetText(
			control,
			TREE_Key_ToString(eventData->key),
			TREE_ColorPair_CreateDefault()
		);
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
	result = TREE_Application_Init(&app, &surface, 1, ApplicationEventHandler);
	if (result)
	{
		printf("Failed to initialize application: %s\n", TREE_Result_ToString(result));
		return 1;
	}

	// create the single control
	TREE_Control_LabelData labelData;
	result = TREE_Control_LabelData_Init(&labelData, "Hello world!", TREE_ALIGNMENT_TOPLEFT, TREE_ColorPair_Create(TREE_COLOR_BRIGHT_WHITE, TREE_COLOR_RED));
	if (result)
	{
		printf("Failed to initialize label data: %s\n", TREE_Result_ToString(result));
		return 1;
	}

	TREE_Control control;
	result = TREE_Control_Label_Init(&control, NULL, &labelData);
	if (result)
	{
		printf("Failed to initialize control: %s\n", TREE_Result_ToString(result));
		return 1;
	}

	control.transform->localOffset = (TREE_Offset){ 2, 2 };
	control.transform->localExtent = (TREE_Extent){ 10, 3 };
	result = TREE_Transform_Dirty(control.transform);
	if (result)
	{
		printf("Failed to dirty transform: %s\n", TREE_Result_ToString(result));
		return 1;
	}

	// add the control to the application
	result = TREE_Application_AddControl(&app, &control);
	if (result)
	{
		printf("Failed to add control to application: %s\n", TREE_Result_ToString(result));
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
	TREE_Control_Free(&control);
	TREE_Control_LabelData_Free(&labelData);
	TREE_Application_Free(&app);
	TREE_Surface_Free(&surface);

	printf("Application ran successfully!\n");

	return 0;
}