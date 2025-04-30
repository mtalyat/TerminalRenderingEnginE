#include "TREE.h"
#include <stdio.h>
#include <time.h>

TREE_Application* g_application = NULL;

void Button_Quit(void* sender, void const* value)
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

	TREE_Theme theme;
	result = TREE_Theme_Init(&theme);
	if (result)
	{
		printf("Failed to initialize theme: %s\n", TREE_Result_ToString(result));
		return 1;
	}

	result = TREE_Window_SetTitle("TREE Test");
	if (result)
	{
		printf("Failed to set window title: %s\n", TREE_Result_ToString(result));
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
	result = TREE_Application_Init(&app, &surface, 32, ApplicationEventHandler);
	if (result)
	{
		printf("Failed to initialize application: %s\n", TREE_Result_ToString(result));
		return 1;
	}

	// create button data
	TREE_Control_ButtonData quitButtonData;
	result = TREE_Control_ButtonData_Init(&quitButtonData, "Quit", Button_Quit, &theme);
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

	// create single line text box
	TREE_Control_TextInputData textInputData;
	result = TREE_Control_TextInputData_Init(&textInputData, "Enter text here", 256, "Placeholder", TREE_CONTROL_TEXT_INPUT_TYPE_NORMAL, NULL, NULL, &theme);
	if (result)
	{
		printf("Failed to initialize text input data: %s\n", TREE_Result_ToString(result));
		return 1;
	}

	// create text input
	TREE_Control textInput;
	result = TREE_Control_TextInput_Init(&textInput, NULL, &textInputData);
	if (result)
	{
		printf("Failed to initialize text input: %s\n", TREE_Result_ToString(result));
		return 1;
	}
	textInput.transform->localOffset.x = 50;
	textInput.transform->localOffset.y = 1;

	// create list data
	TREE_String options[] = {
		"Option 1", "Option 2", "Option 3", "Option 4", "Option 5",
		"Option 6", "Option 7", "Option 8", "Option 9", "Option 10",
		"Option 11", "Option 12", "Option 13", "Option 14", "Option 15",
		"Option 16", "Option 17", "Option 18", "Option 19", "Option 20",
		"Option 21", "Option 22", "Option 23", "Option 24", "Option 25",
		"Option 26", "Option 27", "Option 28", "Option 29", "Option 30"
	};

	// create list data
	TREE_Control_ListData listData;
	result = TREE_Control_ListData_Init(&listData, TREE_CONTROL_LIST_FLAGS_MULTISELECT, options, 30, NULL, NULL, &theme);
	if (result)
	{
		printf("Failed to initialize list data: %s\n", TREE_Result_ToString(result));
		return 1;
	}

	// create list
	TREE_Control listControl;
	result = TREE_Control_List_Init(&listControl, NULL, &listData);
	if (result)
	{
		printf("Failed to initialize list control: %s\n", TREE_Result_ToString(result));
		return 1;
	}
	listControl.transform->localOffset.x = 1;
	listControl.transform->localOffset.y = 6;
	listControl.transform->localExtent.height = 20;

	// create multi line text box
	TREE_Control_TextInputData multiLineTextInputData;
	result = TREE_Control_TextInputData_Init(&multiLineTextInputData, "Enter multi-line text here.\n\nHello world!", 256, "Placeholder", TREE_CONTROL_TEXT_INPUT_TYPE_NORMAL, NULL, NULL, &theme);
	if (result)
	{
		printf("Failed to initialize multi-line text input data: %s\n", TREE_Result_ToString(result));
		return 1;
	}

	// create multi-line text input
	TREE_Control multiLineTextInput;
	result = TREE_Control_TextInput_Init(&multiLineTextInput, NULL, &multiLineTextInputData);
	if (result)
	{
		printf("Failed to initialize multi-line text input: %s\n", TREE_Result_ToString(result));
		return 1;
	}
	multiLineTextInput.transform->localOffset.x = 50;
	multiLineTextInput.transform->localOffset.y = 6;
	multiLineTextInput.transform->localExtent.height = 20;
	multiLineTextInput.transform->localExtent.width = 30;

	// create dropdown datas
	TREE_Control_DropdownData dropDatas[3];
	for (TREE_Size i = 0; i < 3; i++)
	{
		result = TREE_Control_DropdownData_Init(&dropDatas[i], options, 30, 0, 0, NULL, &theme);
		if (result)
		{
			printf("Failed to initialize dropdown data: %s\n", TREE_Result_ToString(result));
			return 1;
		}
	}

	// create dropdowns
	TREE_Control dropControls[3];
	for (TREE_Size i = 0; i < 3; i++)
	{
		result = TREE_Control_Dropdown_Init(&dropControls[i], NULL, &dropDatas[i]);
		if (result)
		{
			printf("Failed to initialize dropdown: %s\n", TREE_Result_ToString(result));
			return 1;
		}
		dropControls[i].transform->localOffset.x = 23;
		dropControls[i].transform->localOffset.y = 3 + (TREE_Int)i * 11;
	}

	// create checkbox datas
#define CHECKBOX_COUNT 8
	TREE_Control_CheckboxData checkboxDatas[CHECKBOX_COUNT];
	for (TREE_Size i = 0; i < CHECKBOX_COUNT; i++)
	{
		result = TREE_Control_CheckboxData_Init(&checkboxDatas[i], (i & 2) == 0 ? " Normal" : "Reversed ", i & 1, i & 4, NULL, &theme);
		if (result)
		{
			printf("Failed to initialize checkbox data: %s\n", TREE_Result_ToString(result));
			return 1;
		}
		checkboxDatas[i].reverse = i & 2;
	}

	// create checkboxes
	TREE_Control checkboxes[CHECKBOX_COUNT];
	for (TREE_Size i = 0; i < CHECKBOX_COUNT; i++)
	{
		result = TREE_Control_Checkbox_Init(&checkboxes[i], NULL, &checkboxDatas[i]);
		if (result)
		{
			printf("Failed to initialize checkbox: %s\n", TREE_Result_ToString(result));
			return 1;
		}
		checkboxes[i].transform->localOffset.x = 82;
		checkboxes[i].transform->localOffset.y = 3 + (TREE_Int)i;
	}

	// create label data
	TREE_Control_LabelData labelData;
	result = TREE_Control_LabelData_Init(&labelData, "Checkboxes:", &theme);
	if (result)
	{
		printf("Failed to initialize label data: %s\n", TREE_Result_ToString(result));
		return 1;
	}

	// create label
	TREE_Control label;
	result = TREE_Control_Label_Init(&label, NULL, &labelData);
	if (result)
	{
		printf("Failed to initialize label: %s\n", TREE_Result_ToString(result));
		return 1;
	}
	label.transform->localOffset.x = 82;
	label.transform->localOffset.y = 2;

	// create number input
	TREE_Control_NumberInputData numberInputData;
	result = TREE_Control_NumberInputData_Init(&numberInputData, 0, 0, 100, 1, 0, NULL, NULL, &theme);
	if (result)
	{
		printf("Failed to initialize number input data: %s\n", TREE_Result_ToString(result));
		return 1;
	}

	// create number input
	TREE_Control numberInput;
	result = TREE_Control_NumberInput_Init(&numberInput, NULL, &numberInputData);
	if (result)
	{
		printf("Failed to initialize number input: %s\n", TREE_Result_ToString(result));
		return 1;
	}
	numberInput.transform->localOffset.x = 100;
	numberInput.transform->localOffset.y = 3;

	// link controls
	result = TREE_Control_Link(&listControl, TREE_DIRECTION_NORTH, TREE_CONTROL_LINK_DOUBLE, &quitButton);
	if (result)
	{
		printf("Failed to link controls: %s\n", TREE_Result_ToString(result));
		return 1;
	}
	result = TREE_Control_Link(&quitButton, TREE_DIRECTION_EAST, TREE_CONTROL_LINK_DOUBLE, dropControls);
	if (result)
	{
		printf("Failed to link controls: %s\n", TREE_Result_ToString(result));
		return 1;
	}
	for (TREE_Size i = 0; i < 3; i++)
	{
		TREE_Control* control = &dropControls[i];
		TREE_Control* nextControl = &dropControls[(i + 1) % 3];
		result = TREE_Control_Link(control, TREE_DIRECTION_SOUTH, TREE_CONTROL_LINK_DOUBLE, nextControl);
		if (result)
		{
			printf("Failed to link controls: %s\n", TREE_Result_ToString(result));
			return 1;
		}
	}
	result = TREE_Control_Link(&dropControls[0], TREE_DIRECTION_EAST, TREE_CONTROL_LINK_DOUBLE, &textInput);
	if (result)
	{
		printf("Failed to link controls: %s\n", TREE_Result_ToString(result));
		return 1;
	}
	result = TREE_Control_Link(&textInput, TREE_DIRECTION_SOUTH, TREE_CONTROL_LINK_DOUBLE, &multiLineTextInput);
	if (result)
	{
		printf("Failed to link controls: %s\n", TREE_Result_ToString(result));
		return 1;
	}
	result = TREE_Control_Link(&textInput, TREE_DIRECTION_EAST, TREE_CONTROL_LINK_DOUBLE, checkboxes);
	if (result)
	{
		printf("Failed to link controls: %s\n", TREE_Result_ToString(result));
		return 1;
	}
	for (TREE_Size i = 0; i < CHECKBOX_COUNT; i++)
	{
		result = TREE_Control_Link(&checkboxes[i], TREE_DIRECTION_SOUTH, TREE_CONTROL_LINK_DOUBLE, &checkboxes[(i + 1) % CHECKBOX_COUNT]);
		if (result)
		{
			printf("Failed to link controls: %s\n", TREE_Result_ToString(result));
			return 1;
		}
	}
	result = TREE_Control_Link(&checkboxes[0], TREE_DIRECTION_EAST, TREE_CONTROL_LINK_DOUBLE, &numberInput);
	if (result)
	{
		printf("Failed to link controls: %s\n", TREE_Result_ToString(result));
		return 1;
	}

	// add controls to application
	result = TREE_Application_AddControl(&app, &quitButton);
	if (result)
	{
		printf("Failed to add quit button to application: %s\n", TREE_Result_ToString(result));
		return 1;
	}
	result = TREE_Application_AddControl(&app, &listControl);
	if (result)
	{
		printf("Failed to add list control to application: %s\n", TREE_Result_ToString(result));
		return 1;
	}
	for (TREE_Size i = 0; i < 3; i++)
	{
		result = TREE_Application_AddControl(&app, &dropControls[i]);
		if (result)
		{
			printf("Failed to add dropdown to application: %s\n", TREE_Result_ToString(result));
			return 1;
		}
	}
	result = TREE_Application_AddControl(&app, &textInput);
	if (result)
	{
		printf("Failed to add text input to application: %s\n", TREE_Result_ToString(result));
		return 1;
	}
	result = TREE_Application_AddControl(&app, &multiLineTextInput);
	if (result)
	{
		printf("Failed to add multi-line text input to application: %s\n", TREE_Result_ToString(result));
		return 1;
	}
	for (TREE_Size i = 0; i < CHECKBOX_COUNT; i++)
	{
		result = TREE_Application_AddControl(&app, &checkboxes[i]);
		if (result)
		{
			printf("Failed to add checkbox to application: %s\n", TREE_Result_ToString(result));
			return 1;
		}
	}
	result = TREE_Application_AddControl(&app, &label);
	if (result)
	{
		printf("Failed to add label to application: %s\n", TREE_Result_ToString(result));
		return 1;
	}
	result = TREE_Application_AddControl(&app, &numberInput);
	if (result)
	{
		printf("Failed to add number input to application: %s\n", TREE_Result_ToString(result));
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
	TREE_Control_NumberInputData_Free(&numberInputData);
	TREE_Control_Free(&numberInput);
	TREE_Control_LabelData_Free(&labelData);
	TREE_Control_Free(&label);
	TREE_Control_ListData_Free(&listData);
	TREE_Control_Free(&listControl);
	TREE_Control_TextInputData_Free(&textInputData);
	TREE_Control_Free(&textInput);
	TREE_Control_TextInputData_Free(&multiLineTextInputData);
	TREE_Control_Free(&multiLineTextInput);
	for (TREE_Size i = 0; i < CHECKBOX_COUNT; i++)
	{
		TREE_Control_Free(&checkboxes[i]);
		TREE_Control_CheckboxData_Free(&checkboxDatas[i]);
	}
	TREE_Control_Free(&quitButton);
	TREE_Control_ButtonData_Free(&quitButtonData);
	for (TREE_Size i = 0; i < 3; i++)
	{
		TREE_Control_Free(&dropControls[i]);
		TREE_Control_DropdownData_Free(&dropDatas[i]);
	}
	TREE_Application_Free(&app);
	TREE_Surface_Free(&surface);
	TREE_Theme_Free(&theme);
	TREE_Free();

	printf("Application ran successfully!\n");

	return 0;
}