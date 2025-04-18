#include "TREE.h"
#include <stdio.h>
#include <time.h>

int main()
{
	while (1)
	{
		TREE_Key key = TREE_Input_GetKey();

		if (key == TREE_KEY_NULL)
		{
			continue;
		}

		printf("Pressed: %d->%c (%s)\n", key, TREE_Key_GetChar(key), TREE_Key_GetString(key));

		if (key == TREE_KEY_ESCAPE)
		{
			break;
		}
	}
}