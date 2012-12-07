
#include "vTreeModel.h"
#include "yDataStorageConfigure.h"

#include <QIcon>
#include <QPixmap>
#include <QMimeData>
#include <QTextStream>
#include <QMessageBox>

vTreeModel::vTreeModel( yDataManagement::Pointer _DataStorage
	, QObject* parent )
	: QAbstractItemModel(parent)
	, m_Dm(NULL)
	, m_Views(NULL)
	, m_Root(NULL)
{
	this->SetDataStorage(_DataStorage);
	this->reset();
}

vTreeModel::~vTreeModel()
{
  this->SetDataStorage(0);
  m_Root->Delete(); m_Root = 0;
}

int vTreeModel::GetNode( const QModelIndex &index ) const
{
	return this->TreeItemFromIndex(index)->GetDataNode();
}

QModelIndex vTreeModel::index( int row, int column, const QModelIndex & parent ) const
{
	TreeItem* parentItem;

	if (!parent.isValid())
		parentItem = m_Root;
	else
		parentItem = static_cast<TreeItem*>(parent.internalPointer());

	TreeItem *childItem = parentItem->GetChild(row);
	if (childItem)
		return createIndex(row, column, childItem);
	else
		return QModelIndex();

}

int vTreeModel::rowCount(const QModelIndex &parent) const
{
	TreeItem *parentTreeItem = this->TreeItemFromIndex(parent);
	return parentTreeItem->GetChildCount();
}


Qt::ItemFlags vTreeModel::flags( const QModelIndex& index ) const
{
  if (index.isValid())
	  return Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable
	  | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
  else
    return Qt::ItemIsDropEnabled;
}

int vTreeModel::columnCount( const QModelIndex& /* parent = QModelIndex() */ ) const
{
  return 4;
}

QModelIndex vTreeModel::parent(const QModelIndex &index) const
{
	if (!index.isValid())
		return QModelIndex();

	TreeItem *childItem = this->TreeItemFromIndex(index);
	TreeItem *parentItem = childItem->GetParent();

	if (parentItem == m_Root)
		return QModelIndex();

	return this->createIndex(parentItem->GetIndex(), 0, parentItem);
}

vTreeModel::TreeItem* vTreeModel::TreeItemFromIndex( const QModelIndex &index ) const
{
	if (index.isValid())
		return static_cast<TreeItem *>(index.internalPointer());
	else
		return m_Root;
}



QVariant vTreeModel::data( const QModelIndex & index, int role ) const
{
  int dataNode = this->TreeItemFromIndex(index)->GetDataNode();
  bool mask = false;
  if (m_Dm->GetDataNodeType(dataNode) == MASKV_DATA)
	  mask = true;

  if (index.column() == 0)
  {
	  QString nodeName = QString::fromLocal8Bit(m_Dm->GetNameFromIndex(dataNode).c_str());
	  DataNodeType type_ = m_Dm->GetDataNodeType(dataNode); 
	  if (role == Qt::DisplayRole)
		  return nodeName;
	  else if(role == Qt::ToolTipRole)
		  return nodeName;
	  else if (role == Qt::DecorationRole)
	  {
		  switch(type_)
		  {
		  case  IMAGE_DATA:
			  return QIcon(QPixmap("./icon/image.png"));
		  case MASKV_DATA:
			  return QIcon(QPixmap("./icon/mask.png"));
		  case SURFACE_DATA:
			  return QIcon(QPixmap("./icon/model.png"));
		  }
	  }
  }
  else if(index.column() == 1)
  {
	  if (mask)
	  {
		  if (role == Qt::CheckStateRole)
			  return 0;
	  }

	  QString nodeVisible = QString::fromStdString(m_Dm->GetNodeProperty(dataNode,"visible"));

	  if(role == Qt::CheckStateRole)
	  {
		  return nodeVisible.toInt();
	  }
  }
  else if (index.column() == 2)
  {
	  if (mask)
	  {
		  if (role == Qt::DisplayRole)
			  return "";
	  }

	  bool exist = m_Dm->IsPropertyExist(dataNode,"opacity");
	  if (exist)
	  {
		  QString nodeOpacity = QString::fromStdString(m_Dm->GetNodeProperty(dataNode,"opacity"));
		  if (role == Qt::DisplayRole)
		  {
			  return nodeOpacity.left(4);
		  }
	  }
	  else 
	  {
		  if (role == Qt::DisplayRole)
			  return "";
	  }
  }
  else if (index.column() == 3)
  {
	  if (mask)
	  {
		  if (role == Qt::DecorationRole)
			  return 0;
	  }

		bool exist = m_Dm->IsPropertyExist(dataNode,"color");

		if (role == Qt::DecorationRole)
		{
			if (exist)
			{		
				std::string str = m_Dm->GetNodeProperty(dataNode,"color");
				double color[3] = {1,1,1};			
				ConvertNFromString(color,str);

				QColor color1((int)(color[0]*255),(int)(color[1]*255),(int)(color[2]*255));
				return color1;
			}
			else
				return 0;
		}
  }

  return QVariant();
}

bool vTreeModel::setData( const QModelIndex &index, const QVariant &value, int role )
{
	int dataNode = this->TreeItemFromIndex(index)->GetDataNode();
	if(dataNode < 0)
		return false;
	if (m_Dm->GetDataNodeType(dataNode) == MASKV_DATA)
		return false;

	if (index.column() == 0)
	{
		if(role == Qt::EditRole && !value.toString().isEmpty())
		{
			m_Dm->SetNameByIndex(value.toString().toStdString().c_str(),dataNode);
		}
	}
	else if(index.column() == 1)
	{
        if (role == Qt::CheckStateRole && !value.toString().isEmpty())
		{		
			QVariant qcheckstate = index.data(Qt::CheckStateRole);
			int checkstate = qcheckstate.toInt();
			bool isVisible = bool(checkstate);
			m_Dm->SetDataNodeVisible(dataNode,!isVisible);
			if (m_Views)
			{
				m_Views->OnPropertyChanged(dataNode);
			}
		}
	}
	else if (index.column() == 2)
	{
		if(role == Qt::EditRole && !value.toString().isEmpty())
		{
			bool exist = m_Dm->IsPropertyExist(dataNode,"opacity");
			if (exist)
			{
				m_Dm->SetNodeProperty(dataNode,"opacity",value.toString().toStdString().c_str(),"true",DOUBLE_);
				m_Views->OnPropertyChanged(dataNode);
			}
		}
	}
	else if (index.column() == 3)
	{
		if(role == Qt::EditRole && !value.toString().isEmpty())
		{
			bool exist = m_Dm->IsPropertyExist(dataNode,"color");
			if (exist)
			{
				m_Dm->SetNodeProperty(dataNode,"color",value.toString().toStdString().c_str(),"true",COLOR_);
				m_Views->OnPropertyChanged(dataNode);
			} 
		}
	}
	// inform listeners about changes
	emit dataChanged(index, index);
	return true;
}

QVariant vTreeModel::headerData(int section,
                                 Qt::Orientation orientation,
                                 int role) const
{
   if (orientation == Qt::Horizontal && role == Qt::DisplayRole && m_Root)
   {
	   if (section == 0) {
           return QObject::tr("名称");
	   } else if (section == 1) {
           return QObject::tr("可见性");
	   } else if (section == 2){
           return QObject::tr("不透明度");
	   } else if (section == 3){
           return QObject::tr("颜色");
	   }
   }
   return QVariant();
}

void vTreeModel::SetDataStorage( yDataManagement::Pointer _DataStorage )
{
  if(m_Dm != _DataStorage) // dont take the same again
  {
    // take over the new data storage
    m_Dm = _DataStorage;

	// delete the old root (if necessary, create new)
	if(m_Root)
		m_Root->Delete();
	int rootDataNode = -1;
	m_Root = new TreeItem(rootDataNode, 0);
	this->reset();
  }
}

void vTreeModel::SetViews(vMultiView* views)
{

	m_Views = views;
}

void vTreeModel::AddNode( const int node)
{
	if(node < 0
		|| !m_Dm
		|| m_Root->Find(node) != 0)
		return;

  // find out if we have a root node
  TreeItem* parentTreeItem = m_Root;
  QModelIndex index;
  int parentDataNode = this->GetParentNode(node);

  if(parentDataNode >= 0) // no top level data node
  {
    parentTreeItem = m_Root->Find(parentDataNode); // find the corresponding tree item
    if(!parentTreeItem)
    {
      this->AddNode(parentDataNode);
      parentTreeItem = m_Root->Find(parentDataNode);
      if(!parentTreeItem)
        return;
    }

    // get the index of this parent with the help of the grand parent
    index = this->createIndex(parentTreeItem->GetIndex(), 0, parentTreeItem);
  }

  // add node

   beginInsertRows(index, parentTreeItem->GetChildCount()
                    , parentTreeItem->GetChildCount());
   new TreeItem(node, parentTreeItem);

  // emit endInsertRows event
  endInsertRows();

}

void vTreeModel::RemoveNode( const int node )
{
  if(!m_Root) return;

  TreeItem* treeItem = m_Root->Find(node);
  if(!treeItem)
    return; // return because there is no treeitem containing this node

  TreeItem* parentTreeItem = treeItem->GetParent();
  QModelIndex parentIndex = this->IndexFromTreeItem(parentTreeItem);

  // emit beginRemoveRows event (QModelIndex is empty because we dont have a tree model)
  this->beginRemoveRows(parentIndex, treeItem->GetIndex(), treeItem->GetIndex());

  // remove node
  std::vector<TreeItem*> children = treeItem->GetChildren();
  delete treeItem;

  // emit endRemoveRows event
  endRemoveRows();

  // move all children of deleted node into its parent
  for ( std::vector<TreeItem*>::iterator it = children.begin()
    ; it != children.end(); it++)
  {
    // emit beginInsertRows event
    beginInsertRows(parentIndex, parentTreeItem->GetChildCount(), parentTreeItem->GetChildCount());

    // add nodes again
    parentTreeItem->AddChild(*it);

    // emit endInsertRows event
    endInsertRows();
  }

  m_Dm->RemoveNodeByIndex(node);

  if(m_Dm->GetNumOfNodes() == 0)
	  this->reset();

}

int vTreeModel::GetParentNode( const int node ) const
{
   return  m_Dm->GetParentIndexFromChildIndex(node);
}


bool vTreeModel::setHeaderData( int /*section*/, Qt::Orientation /*orientation*/, const QVariant& /* value */, int /*role = Qt::EditRole*/ )
{
  return false;
}


void vTreeModel::TreeToVector(TreeItem* parent, std::vector<TreeItem*>& vec) const
{
  TreeItem* current;
  for(int i = 0; i<parent->GetChildCount(); ++i)
  {
    current = parent->GetChild(i);
    this->TreeToVector(current, vec);
    vec.push_back(current);
  }
}

QModelIndex vTreeModel::IndexFromTreeItem( TreeItem* item ) const
{
  if(item == m_Root)
    return QModelIndex();
  else
    return this->createIndex(item->GetIndex(), 0, item);
}

std::vector<int> vTreeModel::GetNodeSet() const
{
  std::vector<int> vec;
  if(m_Root)
	  this->TreeToNodeSet(m_Root, vec);

  return vec;
}

void vTreeModel::TreeToNodeSet( TreeItem* parent, std::vector<int>& vec ) const
{
	TreeItem* current;
	for(int i = 0; i<parent->GetChildCount(); ++i)
	{
		current = parent->GetChild(i);
		vec.push_back(current->GetDataNode());
		this->TreeToNodeSet(current, vec);
	}
}

// QModelIndex vTreeModel::GetIndex( const int node ) const
// {
// 	if(m_Root)
// 	{
// 		TreeItem* item = m_Root->Find(node);
// 		if(item)
// 			return this->IndexFromTreeItem(item);
// 	}
// 	return QModelIndex();
// }

vTreeModel::TreeItem::TreeItem( int _DataNode, TreeItem* _Parent )
	: m_Parent(_Parent)
	, m_DataNode(_DataNode)
{
	if(m_Parent)
		m_Parent->AddChild(this);
}

vTreeModel::TreeItem::~TreeItem()
{
	if(m_Parent)
		m_Parent->RemoveChild(this);
}

void vTreeModel::TreeItem::Delete()
{
  while(!m_Children.empty())
    delete m_Children.back();

  delete this;
}

vTreeModel::TreeItem* vTreeModel::TreeItem::Find( const int _DataNode ) const
{
	vTreeModel::TreeItem* item = 0;
	if(_DataNode >= 0)
	{
		if(m_DataNode == _DataNode)
			item = const_cast<TreeItem*>(this);
		else
		{
			for(std::vector<TreeItem*>::const_iterator it = m_Children.begin(); it != m_Children.end(); ++it)
			{
				if(item)
					break;
				item = (*it)->Find(_DataNode);
			}
		}
	}
	return item;
}

int vTreeModel::TreeItem::IndexOfChild( const TreeItem* item ) const
{
	std::vector<TreeItem*>::const_iterator it = m_Children.begin();
	while(it != m_Children.end())
	{
		if (item == *it)
		{
			break;
		}
		it++;
	}
	return it != m_Children.end() ? std::distance(m_Children.begin(), it): -1;
}

vTreeModel::TreeItem* vTreeModel::TreeItem::GetChild( int index ) const
{
  return ((!m_Children.empty()) && index >= 0 && index < (int)m_Children.size())? m_Children.at(index): 0;
}

void vTreeModel::TreeItem::AddChild( TreeItem* item )
{
  this->InsertChild(item);
}

void vTreeModel::TreeItem::RemoveChild( TreeItem* item )
{
	std::vector<TreeItem*>::iterator it = m_Children.begin();
	while(it != m_Children.end())
	{
		if (item == *it)
		{
			break;
		}
		it++;
	}

	if(it != m_Children.end())
	{
		m_Children.erase(it);
		item->SetParent(0);
	}
}

int vTreeModel::TreeItem::GetChildCount() const
{
	return (int)m_Children.size();
}

int vTreeModel::TreeItem::GetIndex() const
{
	if (m_Parent)
		return m_Parent->IndexOfChild(this);

	return 0;
}

vTreeModel::TreeItem* vTreeModel::TreeItem::GetParent() const
{
	return m_Parent;
}

int vTreeModel::TreeItem::GetDataNode() const
{
	return m_DataNode;
}

void vTreeModel::TreeItem::InsertChild( TreeItem* item, int index )
{
	std::vector<TreeItem*>::iterator it = m_Children.begin();
	while(it != m_Children.end())
	{
		if (item == *it)
		{
			break;
		}
		it++;
	}

	if(it == m_Children.end())
	{
		if(!m_Children.empty() && index >= 0 && index < (int)m_Children.size())
		{
			it = m_Children.begin();
			std::advance(it, index);
			m_Children.insert(it, item);
		}
		else
			m_Children.push_back(item);

    // add parent if necessary
    if(item->GetParent() != this)
      item->SetParent(this);
  }
}

std::vector<vTreeModel::TreeItem*> vTreeModel::TreeItem::GetChildren() const
{
  return m_Children;
}

void vTreeModel::TreeItem::SetParent( TreeItem* _Parent )
{
  m_Parent = _Parent;
  if(m_Parent)
    m_Parent->AddChild(this);
}
