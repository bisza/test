/*****************************************************************************
** File			: yDataManagement.h
** CopyRight(c) : Yorktal
** Writer		: ZhangLei
** Time			: 2010.11.3
** Description	: Data Storage parser, this class mainly use to get data node 
** information from data storage, and it also could be used as the interface 
** with Pulgin and Data Managment UI...
** Version		: 1.1
** Info			: Modified this class as subclass of itkobject to use the 
** smart pointer (2010.11.5).
*****************************************************************************/
#ifndef yDataManagement_h
#define yDataManagement_h

#include "vtkSmartPointer.h"
#include "yDataStorageDll.h"
#include "yDataNodeStorage.h"

class Y_DATA_STORAGE_EXPORT yDataManagement : public itk::Object
{
public:
	typedef yDataManagement					Self;
	typedef itk::Object                     Superclass;
	typedef itk::SmartPointer<Self>			Pointer;
	typedef itk::SmartPointer<const Self>	ConstPointer;

	itkNewMacro(yDataManagement);
	itkTypeMacro(yDataManagement,itk::Object);

public:
	//Nodes will be created in data management, use the following two method, return value is the node index
	virtual int			CreateNode(DataNodeType type,std::string name,vtkSmartPointer<vtkObject> data);
	virtual int			CreateNode(DataNodeType type,std::string name,vtkSmartPointer<vtkObject> data,int parent);

	//After the node created, set the data object
	virtual vtkSmartPointer<vtkObject>	GetData(const int node_index);
	virtual void		GetData(const int node_index, vtkSmartPointer<vtkObject> &data);
	virtual void        SetData(const int node_index, vtkSmartPointer<vtkObject> data);

	bool				IsNodeExist(int node_index);

	//Insert property, Get property, check the number of property
	virtual void		SetNodeProperty(int node_index, std::string property_name, std::string property_value,std::string is_modify,PropertyType property_type);
	virtual std::string	GetNodeProperty(int node_index, std::string property_name,PropertyType& property_type);
	virtual std::string	GetNodeProperty(int node_index, std::string property_name);
	bool     			IsPropertyModifiable(int node_index, std::string property_name);  //return true and false in string

	bool			    IsPropertyExist(int node_index,std::string property_name);
	int					GetNumberOfProperty(int node_index);
	std::string			GetPropertyFromPropertyIndex(int node_index, int property_index, std::string &property_name, PropertyType& property_type);

	//index operation
	int                 GetIndexFromName(std::string node_name);
	int					GetParentIndexFromChildIndex(int child_index);
	std::vector<int>	GetChildrenIndexFromParentIndex(int parent_index);

	//Add mask data
	virtual int			AddMaskNode(std::string name, int parent_index = -1);

	//Remove operation
	void				RemoveAllNodes();
	void                RemoveAllCheckNodes();
	void                RemoveAllCheckNodesByType(DataNodeType type);
	bool				RemoveTreeByParentIndex(int index);

	// Management UI interface
	virtual int			GetNumOfNodes();
	virtual int         GetNumOfNodes(DataNodeType type);

	std::vector<int>	GetAllNodeIndex();

	bool				IsParentIndex(int index);
	bool				IsChildIndex(int index);

	std::vector<int>	GetRootIndexes();    //use this function to find all the root index..

	// Get name, visible, select, data size property 
	void				SetNameByIndex(std::string name, int index);
	std::string			GetNameFromIndex(int index);
	DataNodeType		GetDataNodeType(int index);

	bool				RemoveNodeByIndex(int index);

	int					GetSelectNodeIndex();
	void				SetSelectNodeIndex(int index);

	unsigned long		GetDataSizeByIndex(int index);

	std::vector<int>	GetVisibleNodeIndex();
	bool				GetNodeVisible(int index);
	int					GetVisibleRootIndex();

	std::vector<int>    GetCheckedNodeIndex();
	void				SetDataNodeVisible(int index, bool visible);

	virtual int         DuplicateDataNode(int index,std::string name);

	std::list<int>      GetNodesFromType(DataNodeType type);

	yDataNodeStorage::NodeEventContainer GetNodeEvents();
	inline void			ResetNodeEvents(){return m_DataStorage->ResetNodeEvents();};

protected:
	yDataManagement();
	~yDataManagement();

	virtual bool		AddNode(yDataNode::Pointer node);
	virtual bool		AddNode(yDataNode::Pointer child, yDataNode::Pointer parent);
	bool				RemoveNode(yDataNode::Pointer node);

	yDataNode::Pointer	GetNodeFromIndex(int index);
	std::vector<yDataNode::Pointer> GetDataStorageNodes();

	virtual yDataNode::Pointer  DuplicateDataNode(yDataNode::Pointer node,std::string name);

	yDataNode::Pointer  GetVisilbeRootNode();
	yDataNode::Pointer  GetSelectNode();

	yDataNode::Pointer	AddMaskNode(std::string name, yDataNode::Pointer parent_node = NULL);

private:

	yDataNodeStorage::Pointer m_DataStorage;
	static std::vector<int>   treeNodeIndexContainer;

	vtkSmartPointer<vtkObject> CreateSegmentImageFromSoureImage(vtkSmartPointer<vtkObject> source);

	int					CountMaskVNode();
	int					CountMaskVNodeFromImageNode(yDataNode::Pointer node,int& last_mask_bit);

	void				GetAllChildNodeIndexFromParentIndex(int parent_index);

	yDataNode::Pointer  GetParentNode(yDataNode::Pointer node);
	std::vector<yDataNode::Pointer> GetChildrenNodeFromParentNode(yDataNode::Pointer parent_node);

private:
	yDataManagement(const Self&);
	void operator=(const Self&);

};

#endif // yDataManagement_h