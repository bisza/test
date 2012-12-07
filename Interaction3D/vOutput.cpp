#include "vInteraction3D.h"
#include "vInteraction3DDll.h"

extern "C" V_INTERACTION3D_EXPORT vToolBarBase* GetToolBar(int *position, int *squence)
{
	return new vInteraction3D();
}