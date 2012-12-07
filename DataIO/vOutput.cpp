#include "vDataReader.h"
#include "vDataReaderDll.h"

extern "C"	V_DATA_IO_EXPORT vToolBarBase* GetToolBar(int *position, int *squence)
{
	position[0] = 0;
	squence[0] = 0;	
	return new vDataReader();
};
