#include "yDataNodeStorage.h"
#include "yDataStorageConfigure.h"

yDataNodeStorage::yDataNodeStorage()
{
	m_Nodes.clear();
	m_Events.clear();
}

yDataNodeStorage::~yDataNodeStorage()
{
	DataNodeContainer::iterator it = m_Nodes.begin();
	while(it != m_Nodes.end())
	{
		(*it)->RemoveAllProperty();
		it++;
	}

	this->RemoveAllNodes();
}
bool yDataNodeStorage::AddNode(yDataNode::Pointer node)
{
	if (node.IsNull())
	{
		return false;
	}

	//Make sure the node does not exist in the tree...
	if (0 != m_Nodes.size())
	{	
		yDataNode::Pointer tempNode = NULL;
		DataNodeContainer::iterator it = m_Nodes.begin();
		while(it != m_Nodes.end())
		{
			tempNode = *it;
			if (node == tempNode)
			{	
				return false;  // if this node have exit in the data storage, return;
			}
			it++;
		}
	}

	if (IMAGE_DATA == node->GetDataNodeType())
	{
		//Make sure the other node invisible...
		DataNodeContainer tempDataNodes = this->GetDataStorageNodes();
		yDataNode::Pointer tempDataNode = NULL;

		if (0 != tempDataNodes.size())
		{
			for(DataNodeContainer::iterator it = tempDataNodes.begin();
				it != tempDataNodes.end(); it++)
			{
				tempDataNode = *it;
				tempDataNode->InsertBoolProperty("visible",false);
			}
		}
	}

	int index = -1;
	this->GetAddNodeIndex(index);
	index++;
	node->InsertIntProperty("index",index,"false");

	// add this node in data storage
	m_Nodes.push_back(node);
	m_Events.push_back(index);
	return true;
}

void yDataNodeStorage::GetAddNodeIndex(int &index)
{
	DataNodeContainer::iterator it = m_Nodes.begin();

	while(it != m_Nodes.end())
	{
		int tempIndex = 0;
		tempIndex = (*it)->GetIntProperty("index");
		if (tempIndex > index)
		{
			index = tempIndex;
		}
		it++;
	}
}

bool yDataNodeStorage::AddNode(yDataNode::Pointer child, yDataNode::Pointer parent)
{
	if (child.IsNull())
	{
		return false;
	}

	// check if the node exist in the data storage, if in, return false
	yDataNode::Pointer tempNode = NULL;
	DataNodeContainer::iterator it = m_Nodes.begin();
	while (it != m_Nodes.end())
	{
		tempNode = *it;
		if (child == tempNode)
		{	
			return false;
		}
		it++;
	}

	if (parent.IsNull())
	{
		this->AddNode(child);
		return true;
	}

	// check if the parent exist in the data storage, if in, add the child node to parent node
	it = m_Nodes.begin();
	while (it != m_Nodes.end())
	{
		tempNode = *it;
		if (parent == tempNode)
		{	
			//parent index
			int parentIndex = parent->GetIntProperty("index");
			child->InsertIntProperty("parent_index",parentIndex,"false");

			//insert node's index
			int index = -1;
			this->GetAddNodeIndex(index);
			index++;
			child->InsertIntProperty("index",index,"false");

			m_Nodes.push_back(child);
			m_Events.push_back(index);
			return true;
		}
		it++;
	}

	// if not in ,return false
	return false;
}



bool yDataNodeStorage::RemoveNode(yDataNode::Pointer node)
{
	if (node.IsNull())
	{
		return false;
	}

	// check if the node exist in the data storage, if in, delete it and it's relation ship
	yDataNode::Pointer					tempNode = NULL;
	DataNodeContainer::iterator			it = m_Nodes.begin();

	int parentIndex = node->GetIntProperty("parent_index"); 
	int index = node->GetIntProperty("index");

	while (it != m_Nodes.end())
	{
		tempNode = *it;
		int tempParent = tempNode->GetIntProperty("parent_index");

		if (tempParent == index)
		{	
			// delete the relation ship between parent and child
			tempNode->InsertIntProperty("parent_index",parentIndex);
		}

		if (tempNode == node)
		{
			// erase the iterate
			m_Events.push_back(node->GetIntProperty("index"));
			it = m_Nodes.erase(it);
			continue;
		}

		it++;
	}

	return true;
}

void yDataNodeStorage::RemoveAllNodes()
{
	//m_Nodes.clear();
	yDataNode::Pointer tempNode = NULL;
	DataNodeContainer::iterator	it = m_Nodes.begin();

	while(m_Nodes.size() > 0)
	{
		tempNode = *it;
		this->RemoveNode(tempNode);
		it = m_Nodes.begin();
	}
}

std::vector<yDataNode::Pointer> yDataNodeStorage::GetDataStorageNodes()
{
	return m_Nodes;
}

yDataNode::Pointer yDataNodeStorage::GetNodeFromIndex(int index)
{
	yDataNode::Pointer tempNode = NULL;
	DataNodeContainer::iterator	it = m_Nodes.begin();

	while(it != m_Nodes.end())
	{
		tempNode = *it;
		if (tempNode->GetIntProperty("index") == index)
		{
			return tempNode;
		}
		it++;
	}

	return NULL;
}

yDataNode::Pointer yDataNodeStorage::GetNodeFromName(std::string name)
{
	yDataNode::Pointer tempNode = NULL;
	DataNodeContainer::iterator it = m_Nodes.begin();
	while (it != m_Nodes.end())
	{
		tempNode = *it;
		if (tempNode->GetStringProperty("name") == name)
		{
			return tempNode;
		}
		it++;
	}

	return NULL;
}

yDataNode::Pointer yDataNodeStorage::GetParentNode(yDataNode::Pointer node)
{
	if (node.IsNull())
	{
		return NULL;
	}

	int tempParentIndex = node->GetIntProperty("parent_index");
	return this->GetNodeFromIndex(tempParentIndex);
}

std::vector<yDataNode::Pointer> yDataNodeStorage::GetChildrenNodes(yDataNode::Pointer parent)
{
	std::vector<yDataNode::Pointer> childNodes;

	if (parent.IsNull())
	{
		return childNodes;
	}

	yDataNode::Pointer tempNode = NULL;
	DataNodeContainer::iterator	it = m_Nodes.begin();
	int parentIndex = parent->GetIntProperty("index");

	while(it != m_Nodes.end())
	{
		tempNode = *it;
		if (tempNode->GetIntProperty("parent_index") == parentIndex)
		{
			childNodes.push_back(tempNode);
		}
		it++;
	}

	return childNodes;
}

int yDataNodeStorage::GetIndexFormNode(yDataNode::Pointer node)
{
	if (node.IsNull())
	{
		return -1;  // if node is empty, return -1
	}

	return node->GetIntProperty("index");
}

int yDataNodeStorage::GetNumOfNodes()
{
	return (int)m_Nodes.size();
}

int	yDataNodeStorage::GetNumOfNodes(DataNodeType type)
{
	int number = 0;
	yDataNode::Pointer tempNode = NULL;
	DataNodeContainer::iterator it = m_Nodes.begin();
	while(it != m_Nodes.end())
	{
		tempNode = *it;
		if (tempNode->GetDataNodeType() == type)
		{
			number++;
		}
		it++;
	}
	return number;
}

unsigned long yDataNodeStorage::GetDataSizeFromIndex(int index)
{
	yDataNode::Pointer tempNode = GetNodeFromIndex(index);

	if (tempNode.IsNotNull())
	{
		return tempNode->GetUnsignedLongProperty("data_size");
	}

	return 0;
}
