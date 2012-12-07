#include <vector>
#include <algorithm> 
#include "yDataManagement.h"
#include "vtkPolyData.h"
#include "vtkPointData.h"
#include "yScalarToColor.h"
#include "vtkImageData.h"

std::vector<int>  yDataManagement::treeNodeIndexContainer;

yDataManagement::yDataManagement()
{
	m_DataStorage = yDataNodeStorage::New();
}

yDataManagement::~yDataManagement()
{
	m_DataStorage = NULL;
}

int yDataManagement::CreateNode(DataNodeType type,std::string name,vtkSmartPointer<vtkObject> data)
{
	yDataNode::Pointer tempNode = yDataNode::New();
	tempNode->SetDataNodeType(type);
	tempNode->InsertStringProperty("name",name);
	tempNode->SetData(data);

	this->AddNode(tempNode);

	//selected
	this->SetSelectNodeIndex(m_DataStorage->GetIndexFormNode(tempNode));
	return m_DataStorage->GetIndexFormNode(tempNode);
}

int yDataManagement::CreateNode(DataNodeType type,std::string name,vtkSmartPointer<vtkObject> data,int parent)
{
	if (parent < 0)
	{
		return this->CreateNode(type,name,data);
	}

	yDataNode::Pointer tempNode = yDataNode::New();
	tempNode->SetDataNodeType(type);
	tempNode->InsertStringProperty("name",name);
	tempNode->SetData(data);

	this->AddNode(tempNode,m_DataStorage->GetNodeFromIndex(parent));

	//selected
	this->SetSelectNodeIndex(m_DataStorage->GetIndexFormNode(tempNode));

	return m_DataStorage->GetIndexFormNode(tempNode);
}


void yDataManagement::GetData(const int node_index, vtkSmartPointer<vtkObject> &data)
{
	data = this->GetData(node_index);
}

void yDataManagement::SetData(const int node_index, vtkSmartPointer<vtkObject> data)
{
	yDataNode::Pointer node = m_DataStorage->GetNodeFromIndex(node_index);
	if (node.IsNotNull())
	{
		node->SetData(data);
	}
}

vtkSmartPointer<vtkObject> yDataManagement::GetData(const int node_index)
{
	yDataNode::Pointer node = m_DataStorage->GetNodeFromIndex(node_index);

	if (node.IsNull())
	{
		return NULL; //Node does not exist
	}

	return node->GetData();
}

void yDataManagement::SetNodeProperty(int node_index, std::string property_name, std::string property_value,std::string is_modify,PropertyType property_type)
{
	yDataNode::Pointer node = m_DataStorage->GetNodeFromIndex(node_index);

	if (node.IsNotNull())
	{
		//if (property_name == "visible")
		//{
		//	bool visible;
		//	ConvertFromString(visible,property_value);
		//	this->SetDataNodeVisible(node_index,visible);
		//}

		if (property_name == "selected")
		{
			bool selected;
			ConvertFromString(selected,property_value);
			if (selected)
			{
				this->SetSelectNodeIndex(node_index);
			}
		}
		else
		{
			node->InsertProperty(property_name,property_type,property_value,is_modify);
		}
	}
}

bool yDataManagement::IsPropertyModifiable(int node_index, std::string property_name)
{
	yDataNode::Pointer node = m_DataStorage->GetNodeFromIndex(node_index);

	if (node.IsNotNull())
	{
		yDataNodeProperty::Pointer property = node->GetProperty(property_name);
		std::string modifiable = property->GetPropertyModifiable();
		if (modifiable == "true")
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	return false;
}

bool yDataManagement::IsPropertyExist(int node_index,std::string property_name)
{
	yDataNode::Pointer node = m_DataStorage->GetNodeFromIndex(node_index);

	if (node.IsNotNull())
	{
		return node->CheckPropertyExist(property_name);
	}

	return false;
}

std::string yDataManagement::GetNodeProperty(int node_index, std::string property_name,PropertyType& property_type)
{
	yDataNode::Pointer node = m_DataStorage->GetNodeFromIndex(node_index);

	if (node.IsNotNull())
	{
		bool exist = node->CheckPropertyExist(property_name);

		if (true == exist)
		{
			property_type = OTHER_;
			property_type = node->GetPropertyType(property_name);
			return node->GetPropertyValue(property_name);
		}
	}

	return "";
}

std::string yDataManagement::GetNodeProperty(int node_index, std::string property_name)
{
	yDataNode::Pointer node = m_DataStorage->GetNodeFromIndex(node_index);

	if (node.IsNotNull())
	{
		bool exist = node->CheckPropertyExist(property_name);

		if (true == exist)
		{
			return node->GetPropertyValue(property_name);
		}
	}

	return "";
}

int yDataManagement::GetNumberOfProperty(int node_index)
{
	yDataNode::Pointer node = m_DataStorage->GetNodeFromIndex(node_index);

	if (node.IsNotNull())
	{
		return (int)node->GetPropertyList().size();
	}

	return 0;
}

std::string yDataManagement::GetPropertyFromPropertyIndex(int node_index, int property_index,std::string &property_name, PropertyType& property_type)
{
	yDataNode::Pointer node = m_DataStorage->GetNodeFromIndex(node_index);

	if (node.IsNotNull())
	{
		if ((property_index >= 0)  &&
			(property_index < this->GetNumberOfProperty(node_index)))
		{
			yDataNodeBase::PropertyListType propertyList = node->GetPropertyList();
			yDataNodeBase::PropertyListType::iterator it = propertyList.begin();

			for (int i = 0; i < property_index; i++)
			{
				it++;
			}

			property_name = (*it)->GetPropertyName();
			property_type = (*it)->GetPropertyType();
			return (*it)->GetPropertyValue();
		}
	}

	return "";
}

int yDataManagement::GetIndexFromName(std::string node_name)
{
	yDataNode::Pointer tempNode = m_DataStorage->GetNodeFromName(node_name);
	if (tempNode.IsNotNull())
	{
		int index = m_DataStorage->GetIndexFormNode(tempNode);
		return index;
	}
	return -1;
}

int yDataManagement::AddMaskNode(std::string name, int parent_index)
{
	if (parent_index == -1)
	{
		parent_index = this->GetVisibleRootIndex();
	}
	yDataNode::Pointer parentNode = m_DataStorage->GetNodeFromIndex(parent_index);
	yDataNode::Pointer node = this->AddMaskNode(name,parentNode);

	//selected
	this->SetSelectNodeIndex(m_DataStorage->GetIndexFormNode(node));
	return m_DataStorage->GetIndexFormNode(node);
}

int yDataManagement::GetParentIndexFromChildIndex(int child_index)
{
	yDataNode::Pointer tempNode = m_DataStorage->GetNodeFromIndex(child_index);

	if (tempNode.IsNull())
	{	
		return -1;
	}

	int tempParentIndex = tempNode->GetIntProperty("parent_index");
	yDataNode::Pointer tempParentNode = this->GetNodeFromIndex(tempParentIndex);

	if (tempParentNode.IsNotNull())
	{
		return m_DataStorage->GetIndexFormNode(tempParentNode);
	}

	return -1;   // no parent

}

std::vector<int> yDataManagement::GetChildrenIndexFromParentIndex(int parent_index)
{
	yDataNodeStorage::IndexContainer childrenlist;
	childrenlist.clear();

	int index = -1;
	yDataNode::Pointer tempParentNode = m_DataStorage->GetNodeFromIndex(parent_index);
	std::vector<yDataNode::Pointer> tempChildNodes = m_DataStorage->GetChildrenNodes(tempParentNode);
	std::vector<yDataNode::Pointer>::iterator it = tempChildNodes.begin();
	while(it != tempChildNodes.end())
	{
		index = m_DataStorage->GetIndexFormNode(*it);
		childrenlist.push_back(index);
		it++;
	}

	return childrenlist;
}

yDataNode::Pointer yDataManagement::GetParentNode(yDataNode::Pointer node)
{
	if (node.IsNull())
	{
		return NULL;
	}

	int parentIndex = node->GetIntProperty("parent_index");
	return this->GetNodeFromIndex(parentIndex);
}

std::vector<yDataNode::Pointer> yDataManagement::GetChildrenNodeFromParentNode(yDataNode::Pointer node)
{
	return m_DataStorage->GetChildrenNodes(node);
}

bool yDataManagement::IsParentIndex(int index)
{
	yDataNode::Pointer tempNode = NULL;
	yDataNodeStorage::DataNodeContainer tempTreeNode = m_DataStorage->GetDataStorageNodes();
	yDataNodeStorage::DataNodeContainer::iterator it = tempTreeNode.begin();
	while(it != tempTreeNode.end())
	{
		tempNode = *it;
		if (tempNode->GetIntProperty("parent_index") == index)
		{
			return true;
		}
		it++;
	}

	tempNode = this->GetNodeFromIndex(index);
	if (tempNode)
	{
		if (tempNode->GetIntProperty("parent_index") == -1)
		{
			return true;
		}
	}

	return false;
}

bool yDataManagement::IsChildIndex(int index)
{
	yDataNode::Pointer tempNode =m_DataStorage->GetNodeFromIndex(index);

	if (tempNode.IsNotNull())
	{
		if (tempNode->GetIntProperty("parent_index") == -1)
		{
			return false;
		}
		else
		{
			return true;
		}
	}

	return false;
}

std::string yDataManagement::GetNameFromIndex(int index)
{
	std::string name = "";

	yDataNode::Pointer tempNode = m_DataStorage->GetNodeFromIndex(index);

	if (tempNode)
	{
		return tempNode->GetStringProperty("name");
	}

	return name;
}

void yDataManagement::SetNameByIndex(std::string name, int index)
{
	yDataNode::Pointer tempNode = m_DataStorage->GetNodeFromIndex(index);

	if (tempNode)
	{
		tempNode->InsertStringProperty("name",name);
	}
}

int yDataManagement::GetSelectNodeIndex()
{
	yDataNode::Pointer tempNode = NULL;
	yDataNodeStorage::DataNodeContainer tempTreeNode = m_DataStorage->GetDataStorageNodes();
	yDataNodeStorage::DataNodeContainer::iterator it = tempTreeNode.begin();
	while(it != tempTreeNode.end())
	{
		tempNode = *it;
		if (tempNode->GetBoolProperty("selected"))
		{
			return tempNode->GetIntProperty("index");
		}
		it++;
	}

	return -1;  // no select node;
}

void yDataManagement::SetSelectNodeIndex(int index)
{
	yDataNode::Pointer tempNode = NULL;
	yDataNodeStorage::DataNodeContainer tempTreeNode = m_DataStorage->GetDataStorageNodes();
	yDataNodeStorage::DataNodeContainer::iterator it = tempTreeNode.begin();
	while(it != tempTreeNode.end())
	{
		tempNode = *it;
		tempNode->InsertBoolProperty("selected",false);
		it++;
	}

	yDataNode::Pointer tempDataNode = m_DataStorage->GetNodeFromIndex(index);

	if (tempDataNode.IsNotNull())
	{
		tempDataNode->InsertBoolProperty("selected",true);
	}
}


vtkSmartPointer<vtkObject> yDataManagement::CreateSegmentImageFromSoureImage(vtkSmartPointer<vtkObject> source)
{
	if (source == NULL)
	{
		return NULL;
	}

	vtkSmartPointer<vtkImageData> data = vtkImageData::SafeDownCast(source);
	if (data == NULL)
	{
		return NULL;
	}

	int dimension[3];
	data->GetDimensions(dimension);

	vtkSmartPointer<vtkImageData> segmentImage = vtkSmartPointer<vtkImageData>::New();
	segmentImage->SetDimensions(data->GetDimensions());
	segmentImage->SetScalarType(VTK_UNSIGNED_CHAR);
	segmentImage->SetNumberOfScalarComponents(1);
	segmentImage->SetOrigin(data->GetOrigin());
	segmentImage->SetSpacing(data->GetSpacing());
	segmentImage->AllocateScalars();

	void *ptr = segmentImage->GetScalarPointer();
	int dim = dimension[0]*dimension[1]*dimension[2];
	memset(ptr,0,dim*sizeof(unsigned char));
	segmentImage->Modified();
	segmentImage->GetPointData()->GetScalars()->Modified();

	return segmentImage;
}

int yDataManagement::CountMaskVNode()
{
	int maskNodeCount = 0;
	yDataNodeStorage::DataNodeContainer nodes =  m_DataStorage->GetDataStorageNodes();
	yDataNodeStorage::DataNodeContainer::iterator it = nodes.begin();

	while(it != nodes.end())
	{
		if ((*it)->GetDataNodeType() == MASKV_DATA)
		{
			maskNodeCount++;
		}
		it++;
	}

	return maskNodeCount;
}

int yDataManagement::CountMaskVNodeFromImageNode(yDataNode::Pointer node,int& last_mask_bit)
{
	if (node->GetDataNodeType() != IMAGE_DATA)
	{
		return -1;
	}

	int maskNodeCount = 0;
	last_mask_bit = 0;
	yDataNodeStorage::DataNodeContainer childNodes = m_DataStorage->GetChildrenNodes(node);
	yDataNodeStorage::DataNodeContainer::iterator it = childNodes.begin();

	while(it != childNodes.end())
	{
		if ((*it)->GetDataNodeType() == MASKV_DATA)
		{
			maskNodeCount++;
			last_mask_bit = (*it)->GetIntProperty("index");
			//last_mask_bit = (*it)->GetIntProperty("mask_bit");
			//last_mask_bit++;
		}
		it++;
	}

	return maskNodeCount;
}

DataNodeType yDataManagement::GetDataNodeType(int index)
{
	yDataNode::Pointer tempNode = this->GetNodeFromIndex(index);
	if (tempNode.IsNotNull())
	{
		return tempNode->GetDataNodeType();
	}

	return WITHOUT_DATA;
}

std::vector<int> yDataManagement::GetVisibleNodeIndex()
{
	std::vector<int> visibleIndex;
	visibleIndex.clear();

	yDataNode::Pointer tempNode = NULL;
	yDataNodeStorage::DataNodeContainer tempDataNode = m_DataStorage->GetDataStorageNodes();

	int index = 0;

	for (std::vector<yDataNode::Pointer>::iterator it = tempDataNode.begin();
		it != tempDataNode.end(); it++)
	{
		tempNode = *it;
		if (tempNode->GetBoolProperty("visible"))
		{
			index = m_DataStorage->GetIndexFormNode(tempNode);
			visibleIndex.push_back(index);
		}
	}

	return visibleIndex;

}

bool yDataManagement::IsNodeExist(int node_index)
{
	yDataNode::Pointer node = m_DataStorage->GetNodeFromIndex(node_index);

	if (node.IsNull())
	{
		return false;
	}

	return true;
}

std::vector<int> yDataManagement::GetAllNodeIndex()
{
	std::vector<int> nodeIndex;

	yDataNode::Pointer node = NULL;
	std::vector<yDataNode::Pointer> nodes = m_DataStorage->GetDataStorageNodes();
	std::vector<yDataNode::Pointer>::iterator it = nodes.begin();

	while(it != nodes.end())
	{
		node = *it++;
		nodeIndex.push_back(node->GetIntProperty("index"));	
	}

	return nodeIndex;
}

std::vector<int> yDataManagement::GetCheckedNodeIndex()
{
	std::vector<int> checkIndex;
	checkIndex.clear();

	yDataNode::Pointer tempNode = NULL;
	yDataNodeStorage::DataNodeContainer tempDataNode = m_DataStorage->GetDataStorageNodes();

	int index = 0;

	for (std::vector<yDataNode::Pointer>::iterator it = tempDataNode.begin();
		it != tempDataNode.end(); it++)
	{
		tempNode = *it;
		if (true == tempNode->GetBoolProperty("check"))
		{
			index = m_DataStorage->GetIndexFormNode(tempNode);
			checkIndex.push_back(index);
		}
	}

	return checkIndex;
}

int yDataManagement::GetNumOfNodes()
{
	return m_DataStorage->GetNumOfNodes();
}

int yDataManagement::GetNumOfNodes(DataNodeType type)
{
	return m_DataStorage->GetNumOfNodes(type);
}

bool yDataManagement::RemoveNodeByIndex(int index)
{
	yDataNode::Pointer tempNode = m_DataStorage->GetNodeFromIndex(index);
	return m_DataStorage->RemoveNode(tempNode);
}

yDataNode::Pointer yDataManagement::AddMaskNode(std::string name, yDataNode::Pointer parent_node)
{
	vtkSmartPointer<vtkObject> maskData = NULL;
	int parentIndex = -1;

	yDataNode::Pointer tempParentNode = parent_node;

	if (tempParentNode.IsNull())
	{
		parentIndex = this->GetVisibleRootIndex();
		tempParentNode = m_DataStorage->GetNodeFromIndex(parentIndex);
	}

	if (tempParentNode.IsNull())
	{
		return NULL;
	}

	vtkSmartPointer<vtkImageData> data = vtkImageData::SafeDownCast(tempParentNode->GetData());

	if (data == NULL)
	{
		return NULL;
	}

	yDataNode::Pointer tempNode = yDataNode::New();
	tempNode->InsertStringProperty("name",name);
	tempNode->SetDataNodeType(MASKV_DATA);

	int lastMask = -1;
	int numberOfMask = this->CountMaskVNodeFromImageNode(tempParentNode,lastMask);
	int b = lastMask%8;

	if (b == 0)
	{	
		maskData = this->CreateSegmentImageFromSoureImage(data);
	}
	else
	{
		maskData = m_DataStorage->GetNodeFromIndex(lastMask)->GetData();
	}

	tempNode->InsertIntProperty("mask_bit",b,"false");
	tempNode->SetData(maskData);

	int maskvNodeCount = this->CountMaskVNode();

	double* rgb;
	yScalarToColor::Pointer color = yScalarToColor::New();
	color->GetColorFromScalar(maskvNodeCount + 1,rgb);

	tempNode->InsertColorProperty("color",rgb[0],rgb[1],rgb[2]);
	m_DataStorage->AddNode(tempNode,tempParentNode);

	return tempNode;
}

unsigned long yDataManagement::GetDataSizeByIndex(int index)
{
	yDataNode::Pointer tempNode = m_DataStorage->GetNodeFromIndex(index);

	if (tempNode.IsNull())
	{
		return 0;
	}

	return tempNode->GetUnsignedLongProperty("value");
}

std::vector<int> yDataManagement::GetRootIndexes()
{
	std::vector<int> rootIndexes;
	rootIndexes.clear();

	yDataNode::Pointer tempNode = NULL;
	std::vector<yDataNode::Pointer> tempNodes = m_DataStorage->GetDataStorageNodes();
	std::vector<yDataNode::Pointer>::iterator it = tempNodes.begin();
	while(it != tempNodes.end())
	{
		tempNode = *it;
		if (tempNode->GetIntProperty("parent_index") == -1)
		{
			rootIndexes.push_back(m_DataStorage->GetIndexFormNode(tempNode));
		}
		it++;
	}

	return rootIndexes;
}

void yDataManagement::GetAllChildNodeIndexFromParentIndex(int parent_index)
{
	std::vector<int> tempChildren;
	tempChildren.clear();

	tempChildren = this->GetChildrenIndexFromParentIndex(parent_index);

	std::vector<int>::iterator checkIt = find(treeNodeIndexContainer.begin(),treeNodeIndexContainer.end(),parent_index);
	if (checkIt == treeNodeIndexContainer.end())
	{
		treeNodeIndexContainer.push_back(parent_index);
	}

	std::vector<int>::iterator it = tempChildren.begin();
	while(it != tempChildren.end())
	{
		treeNodeIndexContainer.push_back(*it);
		if (this->IsParentIndex(*it))
		{
			this->GetAllChildNodeIndexFromParentIndex(*it);
		}
		it++;
	}

	return;
}

int yDataManagement::GetVisibleRootIndex()
{
	int tempIndex = -1;
	yDataNode::Pointer tempNode = NULL;
	std::vector<int> tempRootIndex = this->GetRootIndexes();

	if (0 != tempRootIndex.size())
	{
		for (std::vector<int>::iterator it = tempRootIndex.begin();
			it != tempRootIndex.end(); it++)
		{
			tempIndex = *it;
			tempNode = this->GetNodeFromIndex(tempIndex);

			if (IMAGE_DATA == tempNode->GetDataNodeType())
			{
				if (tempNode->GetBoolProperty("visible"))
				{
					return tempIndex;
				}
			}
		}
	}

	return -1;
}

yDataNode::Pointer yDataManagement::GetVisilbeRootNode()
{
	int tempIndex =  this->GetVisibleRootIndex();
	yDataNode::Pointer tempNode = NULL;

	if (tempIndex >= 0)
	{
		tempNode = this->GetNodeFromIndex(tempIndex);
		return tempNode;
	}

	return NULL;
}

yDataNode::Pointer yDataManagement::GetSelectNode()
{
	yDataNode::Pointer tempNode = NULL;
	yDataNodeStorage::DataNodeContainer tempDataNode = m_DataStorage->GetDataStorageNodes();

	for (std::vector<yDataNode::Pointer>::iterator it = tempDataNode.begin();
		it != tempDataNode.end(); it++)
	{
		tempNode = *it;
		if (true == tempNode->GetBoolProperty("selected"))
		{
			return tempNode;
		}
	}

	return NULL;
}

bool yDataManagement::GetNodeVisible(int index)
{
	yDataNode::Pointer tempNode = m_DataStorage->GetNodeFromIndex(index);

	if (tempNode)
	{
		return tempNode->GetBoolProperty("visible");
	}

	return false;
}

bool yDataManagement::AddNode(yDataNode::Pointer child, yDataNode::Pointer parent)
{
	return m_DataStorage->AddNode(child,parent);
}

bool yDataManagement::RemoveNode(yDataNode::Pointer node)
{	
	if (node.IsNull())
	{
		return false;
	}

	return m_DataStorage->RemoveNode(node);
}

bool yDataManagement::AddNode(yDataNode::Pointer node)
{
	return m_DataStorage->AddNode(node);
}

void yDataManagement::RemoveAllNodes()
{
	return m_DataStorage->RemoveAllNodes();
}

void yDataManagement::RemoveAllCheckNodes()
{
	yDataNodeStorage::DataNodeContainer allNodes = m_DataStorage->GetDataStorageNodes();
	yDataNodeStorage::DataNodeContainer::iterator it = allNodes.begin();
	while(it != allNodes.end())
	{
		if ((*it)->GetBoolProperty("check") == true)
		{
			this->RemoveNode(*it);
		}
		it++;
	}
}

void yDataManagement::RemoveAllCheckNodesByType(DataNodeType type)
{
	yDataNodeStorage::DataNodeContainer allNodes = m_DataStorage->GetDataStorageNodes();
	yDataNodeStorage::DataNodeContainer::iterator it = allNodes.begin();
	while(it != allNodes.end())
	{
		if ((*it)->GetBoolProperty("check") == true && (*it)->GetDataNodeType() == type)
		{
			this->RemoveNode(*it);
		}
		it++;	
	}
}

bool yDataManagement::RemoveTreeByParentIndex(int index)
{
	yDataNode::Pointer tempParent = this->GetNodeFromIndex(index);
	if (tempParent.IsNull())
	{
		return false;
	}
	treeNodeIndexContainer.clear();
	this->GetAllChildNodeIndexFromParentIndex(index);
	std::vector<yDataNode::Pointer> tempNodeContainer;
	std::vector<int>::iterator it = treeNodeIndexContainer.begin();
	while(it != treeNodeIndexContainer.end())
	{
		tempNodeContainer.push_back(m_DataStorage->GetNodeFromIndex(*it));
		it++;
	}

	if (tempNodeContainer.size() <= 0)
	{
		return false;
	}

	std::vector<yDataNode::Pointer>::iterator itNode = tempNodeContainer.begin();
	while(itNode != tempNodeContainer.end())
	{
		m_DataStorage->RemoveNode(*itNode);
		itNode++;
	}

	return true;
}

yDataNodeStorage::NodeEventContainer yDataManagement::GetNodeEvents()
{
	return m_DataStorage->GetNodeEvents();
}

yDataNode::Pointer yDataManagement::GetNodeFromIndex(int index)
{
	return m_DataStorage->GetNodeFromIndex(index);
}

std::vector<yDataNode::Pointer> yDataManagement::GetDataStorageNodes()
{
	return m_DataStorage->GetDataStorageNodes();
}

void yDataManagement::SetDataNodeVisible(int index, bool visible)
{
	yDataNode::Pointer tempNode = this->GetNodeFromIndex(index);
	int tempIndex = -1;

	if (tempNode.IsNull())
	{
		return;
	}
	
	if (!visible)  // if visible = false , make this node invisible...
	{
		tempNode->InsertBoolProperty("visible",false);

		if (tempNode->GetDataNodeType() == IMAGE_DATA)
		{
			treeNodeIndexContainer.clear();
			this->GetAllChildNodeIndexFromParentIndex(index);
			std::vector<int>::iterator it = treeNodeIndexContainer.begin();
			while(it != treeNodeIndexContainer.end())
			{
				yDataNode::Pointer temWidgetNode = this->GetNodeFromIndex(*it);
				if (temWidgetNode.IsNotNull())
				{
					int measure = this->GetParentIndexFromChildIndex(*it);
					yDataNode::Pointer temMeasureNode = this->GetNodeFromIndex(measure);
					if (temMeasureNode.IsNotNull())
					{
						if (temMeasureNode->GetDataNodeType() == MEASURE_DATA || temWidgetNode->GetDataNodeType() == MEASURE_DATA)
						{
							temWidgetNode->InsertBoolProperty("visible",false);
						}
					}
				}
				it++;
			}
		}
		return;
	}
	else   // else more action need to do
	{
		// find out the all root node
		std::vector<int> tempRootIndex = this->GetRootIndexes();
		if (tempRootIndex.size() <= 1)  // if one or less root node, make the node visible...whatever it is...
		{
			tempNode->InsertBoolProperty("visible",true);
			return;
		}
		else  // if more than one root node exist
		{
			//Find out all the root image data index
			for (std::vector<int>::iterator it = tempRootIndex.begin();
				it != tempRootIndex.end(); it++)
			{
				yDataNode::Pointer tempRootNode = m_DataStorage->GetNodeFromIndex(*it);

				treeNodeIndexContainer.clear();
				this->GetAllChildNodeIndexFromParentIndex(*it);
				std::vector<int>::iterator it_1 = find(treeNodeIndexContainer.begin(),treeNodeIndexContainer.end(),index);

				//Find out if this node is children of root node, if true, make this node visible
				if (it_1 != treeNodeIndexContainer.end())
				{
					yDataNode::Pointer tempChildNode = m_DataStorage->GetNodeFromIndex(*it_1);
					tempChildNode->InsertBoolProperty("visible",true);
					if (tempChildNode->GetDataNodeType() != SURFACE_DATA)
					{
						m_DataStorage->GetNodeFromIndex(*treeNodeIndexContainer.begin())->InsertBoolProperty("visible",true);
					}
				}
				else
				{
					//Else make all the node invisible
					if (tempNode->GetDataNodeType() != SURFACE_DATA)
					{
						std::vector<int>::iterator it_2 = treeNodeIndexContainer.begin();
						yDataNode::Pointer tempChildNodes = NULL;
						while(it_2 != treeNodeIndexContainer.end())
						{
							tempChildNodes = m_DataStorage->GetNodeFromIndex(*it_2);
							DataNodeType nodeType = tempChildNodes->GetDataNodeType();
							if ( nodeType != SURFACE_DATA )
							{
								tempChildNodes->InsertBoolProperty("visible",false);
							}						
							it_2++;
						}
					}
					
				}
			}
		}
	}

}

std::list<int> yDataManagement::GetNodesFromType(DataNodeType type)
{
	std::list<int> nodes;
	nodes.clear();

	yDataNode::Pointer tempNode = NULL;
	std::vector<yDataNode::Pointer> tempNodes = m_DataStorage->GetDataStorageNodes();
	std::vector<yDataNode::Pointer>::iterator it = tempNodes.begin();
	while(it != tempNodes.end())
	{
		tempNode = *it;
		if (tempNode->GetDataNodeType() == type)
		{
			nodes.push_back(m_DataStorage->GetIndexFormNode(tempNode));
		}
		it++;
	}
	return nodes;
}

yDataNode::Pointer yDataManagement::DuplicateDataNode(yDataNode::Pointer node,std::string name)
{
	if (node.IsNull())
	{
		return NULL;
	}
	
	//Only MASKV_DATA and SURFACE_DATA can be duplicated
	if (/*node->GetDataNodeType() == MASKV_DATA || */
		node->GetDataNodeType() == SURFACE_DATA)
	{
		yDataNode::Pointer newNode = yDataNode::New();

		vtkSmartPointer<vtkPolyData> polyData = vtkSmartPointer<vtkPolyData>::New();
		polyData->DeepCopy(vtkPolyData::SafeDownCast(node->GetData()));
		newNode->SetData(polyData);

		yDataNodeBase::PropertyListType properties = node->GetPropertyList();
		yDataNodeBase::PropertyListType::iterator it = properties.begin();
		while(it != properties.end())
		{
			newNode->InsertProperty((*it)->GetPropertyName(),(*it)->GetPropertyType(),(*it)->GetPropertyValue());
			it++;
		}	

		if (name == "")
		{
			newNode->InsertStringProperty("name", node->GetStringProperty("name") + "����");
		}
		else
		{
			newNode->InsertStringProperty("name", name);
		}

		int parentIndex = node->GetIntProperty("parent_index");
		if (parentIndex < 0)
		{
			m_DataStorage->AddNode(newNode);
		}
		else
		{
			m_DataStorage->AddNode(newNode,this->GetNodeFromIndex(parentIndex));
		}
		
		//Update UI data tree
		return newNode;
	}
	else
	{
		return NULL;
	}
}

int yDataManagement::DuplicateDataNode(int index,std::string name)
{
	yDataNode::Pointer newNode = this->DuplicateDataNode(m_DataStorage->GetNodeFromIndex(index),name);
	int newIndex = m_DataStorage->GetIndexFormNode(newNode);
	this->SetSelectNodeIndex(newIndex);
	return newIndex;
}