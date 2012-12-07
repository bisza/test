#ifndef vViewBase_h__
#define vViewBase_h__

#include "yDataManagement.h"
#include "QVTKWidget.h"
#include "vtkRenderer.h"
#include "vFrameWorkDll.h"
#include <list>

class QMenu;
class QAction;

class V_FRAME_WORK_EXPORT vViewBase : public QVTKWidget
{
	Q_OBJECT
public:
	vViewBase(QWidget* parent  = 0);
	~vViewBase();

public:
	void SetDataManagement(yDataManagement::Pointer dm) {m_Dm = dm;};

public:
	virtual void InstallPipeLine(int index) = 0;
	virtual void UnInstallPipeLine();

	vtkSmartPointer<vtkRenderer> GetRender() {return m_Render;};
	vtkSmartPointer<vtkProp> GetActor(){return m_Actor;};
	void ResetCamera();

	void ClearFoucs();
	void SetFoucs();
	bool isFoucs();

	void SetContainer(QWidget* widget){container = widget;};
	QWidget* GetContainer(){return container;};

signals:
	void focusChanged();
	void layoutOne();

	protected slots:
		void OnBackGround();
		void OnSaveImage();

public:
	static vViewBase* m_LastView;

protected:
	virtual void contextMenuEvent(QContextMenuEvent*);
	virtual void mouseDoubleClickEvent(QMouseEvent *);
	virtual void OnCornerInformation(bool arg){};

	virtual void changeEvent(QEvent *event);

	void OnFoucs();
	virtual void translateLanguage();

protected:
	vtkSmartPointer<vtkRenderer>            m_Render;
	vtkSmartPointer<vtkProp>                m_Actor;
	yDataManagement::Pointer                m_Dm;

	QMenu* m_Menu;
	QAction* background;
	QAction* image;

	bool foucs;
	QWidget* container;

};
#endif // vViewBase_h__