#include "vLayout.h"
#include "vLayoutDll.h"

extern "C"	V_LAYOUT_EXPORT vToolBarBase* GetToolBar(int *position, int *squence)
{
	position[0] = 0;
	squence[0] = 2;
	return new vLayout();
};
