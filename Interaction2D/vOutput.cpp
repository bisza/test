#include "vInteraction2D.h"
#include "vInteraction2DDll.h"

extern "C"	V_INTERACTION2D_EXPORT vToolBarBase* GetToolBar(int *position, int *squence)
{
	position[0] = 0;
	squence[0] = 1;
	return new vInteraction2D();
};
