#ifndef QWidgetLayout_h__
#define QWidgetLayout_h__

#include <QWidget>

#include "yDataManagement.h"
#include "vViewBase.h"
#include "vView2D.h"
#include "vStatusBarNotify.h"

#include "vtkEventQtSlotConnect.h"
#include "vFrameWorkDll.h"

class vTreeView;
class QHBoxLayout;
class QSplitter;

class V_FRAME_WORK_EXPORT vMultiView : public QWidget
{
	Q_OBJECT

public:
	enum LayoutType
	{
		ONE_2D = 0,
		TWO_2D,
		THREE_2D,
		FOUR_2D,
		PROJECT,
		THREED
	};

public:
	vMultiView(QWidget *parent = 0,yDataManagement::Pointer dm = 0);
	~vMultiView();

	void SetLayout(LayoutType type);

	void DataChanged();
	void ClearView();

	void SetDataManagement(yDataManagement::Pointer dm){m_Dm = dm;};
	void SetStatusBar(vStatusBarNotify* status) {m_Status = status;};
	LayoutType GetLayout(){return m_Layout;};
	std::list<vViewBase*> GetViews(){return m_Views;};
	void SetTreeView(vTreeView* treeView){m_TreeView = treeView;};

	void ExpandAllNodeInTreeView();
	void OnPropertyChanged(int node);	

public slots:
	void FoucsChanged();
	void OnSelectChanged();

	void LayoutFull();


protected slots:
	void OnLayoutChanged();
	void OnDataChanged();

	void OnMouseWheelEvent(vtkObject* caller, unsigned long vtk_event, void* client_data, void* call_data, vtkCommand*);
	void OnMouseWheelDeal(int num,unsigned long events);


signals:
	void layoutChanged();
	void dataChanged();
	void focusChanged();

protected:
	void Create2DView(int num);
	void Create3DView();

	void Layout2D(int num);
	void LayoutProject();
	void Layout3D();
	void Render();

private:
	void AddImageData(int rootIndex);
	void UpdateImageData(int rootIndex);

	void AddSurfaceData(std::list<int> &indexes);
	void UpdateSurfaceData(std::list<int> &indexes);

	void UpdateCornerText(vView2D* view);

private:
	std::list<vViewBase*> m_Views;
	std::list<QWidget*> m_ViewContainer;

	LayoutType m_Layout;
	yDataManagement::Pointer m_Dm;
	vStatusBarNotify*        m_Status;

	QHBoxLayout* m_MainLayout;
	QSplitter*   m_MainSplitter;

	vTreeView* m_TreeView;

	std::list<vtkSmartPointer<vtkEventQtSlotConnect> > m_Connect;
	int m_CurrentPosition;

	int m_LastRootIndex;
	int m_Dim[3];
	std::list<int>  m_LastModelIndexes;

	int isFull;
	LayoutType FullLayoutType;
	vViewBase* FullLastView;
};
#endif // QWidgetLayout_h__


