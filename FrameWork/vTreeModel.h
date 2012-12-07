#ifndef vTreeModel_h__
#define vTreeModel_h__

#include <QAbstractListModel>
#include "yDataManagement.h"
#include "vMultiView.h"
#include <vector>
#include <string>
#include <QList>

class  vTreeModel : public QAbstractItemModel
{
public:
  vTreeModel(yDataManagement::Pointer _DataStorage
                            , QObject* parent = 0);
  ~vTreeModel();

public:
  int GetNode(const QModelIndex &index) const;
  virtual std::vector<int> GetNodeSet() const;

  Qt::ItemFlags flags(const QModelIndex& index) const;
  QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
  int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
  int columnCount ( const QModelIndex & parent = QModelIndex() ) const;

  QModelIndex index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
  QModelIndex parent ( const QModelIndex & index ) const;
  //# editable model
  bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
  bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole);
  //bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);
  //Qt::DropActions supportedDropActions() const;
  //Qt::DropActions supportedDragActions() const;
  //QStringList mimeTypes() const;
  //QMimeData * mimeData(const QModelIndexList & indexes) const;

public:
  void SetDataStorage(yDataManagement::Pointer _DataStorage);
  yDataManagement::Pointer GetDataStorage(){return m_Dm;};
  void SetViews(vMultiView* views);
  //void SetDataStorageDeleted(const itk::Object* _DataStorage);

  virtual void AddNode(const int node);
  virtual void RemoveNode(const int node);
  //virtual void SetNodeModified(const int node);
  //QModelIndex GetIndex(const int) const;

protected:

  class TreeItem
  {
  public:

    TreeItem(int _DataNode, TreeItem* _Parent=0);
    virtual ~TreeItem();

    int IndexOfChild(const TreeItem* item) const;
    TreeItem* GetChild(int index) const;
    TreeItem* Find( const int _DataNode) const;
    int GetChildCount() const;
    int GetIndex() const;
    TreeItem* GetParent() const;
    int GetDataNode() const;
    std::vector<TreeItem*> GetChildren() const;
    void AddChild( TreeItem* item);
    void RemoveChild( TreeItem* item);
    void InsertChild( TreeItem* item, int index=-1 );
    void SetParent(TreeItem* _Parent);
    void Delete();
  protected:
    TreeItem* m_Parent;
    std::vector<TreeItem*> m_Children;
    int m_DataNode;
  };

  TreeItem* TreeItemFromIndex(const QModelIndex &index) const;
  QModelIndex IndexFromTreeItem(TreeItem*) const;
  int GetParentNode(const int node) const;
  void TreeToVector(TreeItem* parent, std::vector<TreeItem*>& vec) const;
  void TreeToNodeSet(TreeItem* parent, std::vector<int>& vec) const;

  //# ATTRIBUTES
protected:
  yDataManagement::Pointer m_Dm;
  vMultiView* m_Views;
  TreeItem* m_Root;
};
#endif // vTreeModel_h__
