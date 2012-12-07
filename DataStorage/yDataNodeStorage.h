/*****************************************************************************
** File			: yDataNodeStorage.h
** CopyRight(c) : Yorktal
** Writer		: ZhangLei
** Time			: 2010.11.3
** Description	: This class will storage all the data node in the tree mode, the 
** tree will be build in Data Management UI based on the relation of these nodes.
** Vesion		: 1.1
** Info			: Modified this class as subclass of itk::Object to use the 
** smart pointer (2010.11.5).
*****************************************************************************/

#ifndef YDATANODESTORAGE_H
#define YDATANODESTORAGE_H

#include "yDataStorageDll.h"
#include "yDataNode.h"

#include "itkObject.h"
#include "itkSmartPointer.h"
#include "itkObjectFactory.h"

class yDataNodeStorage : public itk::Object
{
public:
	typedef yDataNodeStorage				Self;
	typedef itk::Object                     Superclass;
	typedef itk::SmartPointer<Self>			Pointer;
	typedef itk::SmartPointer<const Self>	ConstPointer;

	itkNewMacro( Self );
	itkTypeMacro(yDataNodeStorage,itk::Object);

	typedef std::vector<yDataNode::Pointer>			DataNodeContainer;
	typedef std::vector<int>						IndexContainer;
	typedef std::list<int>							NodeEventContainer;

public:
	//Add and remove node in DataStorage
	virtual bool		AddNode(yDataNode::Pointer node);
	virtual bool		AddNode(yDataNode::Pointer child, yDataNode::Pointer parent);
	bool				RemoveNode(yDataNode::Pointer node);
	void				RemoveAllNodes();

	//different method of Get node(nodes)
	DataNodeContainer	GetDataStorageNodes();

	DataNodeContainer	GetChildrenNodes(yDataNode::Pointer parent);
	yDataNode::Pointer	GetParentNode(yDataNode::Pointer node);

	yDataNode::Pointer	GetNodeFromName(std::string name);  // return the first node with property Name as "name"
	yDataNode::Pointer	GetNodeFromIndex(int index);

	//Get index of node
	int					GetIndexFormNode(yDataNode::Pointer node);
	//Get number of node
	virtual int			GetNumOfNodes();
	virtual int			GetNumOfNodes(DataNodeType type);
	//Get DataSize of node
	unsigned long		GetDataSizeFromIndex(int index);

	inline NodeEventContainer	GetNodeEvents() {return m_Events;};
	inline void			ResetNodeEvents() {m_Events.clear();};

protected:
	yDataNodeStorage();
	~yDataNodeStorage();

protected:
	void				GetAddNodeIndex(int &index);

private:
	DataNodeContainer	m_Nodes;
	NodeEventContainer  m_Events;

private:
	yDataNodeStorage(const Self&);
	void operator=(const Self&);

};

#endif