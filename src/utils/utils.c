#include "utils.h"


/******************************************************************************
 * @brief block delay in ms
 ******************************************************************************/
void delay_ms(int xms)
{
	int i, j;

	for (i = 0; i < xms; i++)
	{
		for (j = 0; j < 1300; j++) ;
	}
}
