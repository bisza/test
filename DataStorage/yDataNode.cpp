#include "yDataNode.h"
#include "vtkDataSet.h"
#include "yDataStorageConfigure.h"

yDataNode::yDataNode()
	:m_VtkData(NULL)
{
}

yDataNode::~yDataNode()
{

}

void yDataNode::SetDataNodeType(DataNodeType node_type)
{
	Superclass::InsertNodeTypeProperty("node_type",node_type,"false");
	Superclass::InsertBoolProperty("check",false);
	

	switch (node_type)
	{
	case IMAGE_DATA:
		Superclass::InsertDoubleProperty("window",400);
		Superclass::InsertDoubleProperty("level",0);
		Superclass::InsertBoolProperty("is_volume_rendering",true,"false");
		break;
	case DISTANCE_DATA:
	case BIDIMENSIONAL_DATA:
	case ANGLE_DATA:
	case CONTOUR_DATA:
	case SEED_DATA:
	case SURFACE_DATA:	
		Superclass::InsertDoubleProperty("opacity",1);
		break;
	case MASKV_DATA:
		Superclass::InsertDoubleProperty("opacity",0.3);
		Superclass::InsertBoolProperty("is_volume_rendering",false,"false");
		break;		
	case MEASURE_DATA:
		break;
	}

	
	Superclass::InsertBoolProperty("visible",true);
}


void yDataNode::SetData(vtkSmartPointer<vtkObject> data)
{
	if (m_VtkData == data)
	{
		return;
	}

	this->m_VtkData = NULL;
	this->m_VtkData = data;
	vtkSmartPointer<vtkDataSet> tempData = vtkDataSet::SafeDownCast(data);

	if (tempData)
	{
		Superclass::InsertUnsignedLongProperty("value",tempData->GetActualMemorySize()/1024,"false");
		Superclass::InsertStringProperty("unit","M","false");
	}
}

DataNodeType yDataNode::GetDataNodeType()
{
	return Superclass::GetNodeTypeProperty("node_type");
}
