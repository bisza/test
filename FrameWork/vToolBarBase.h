#ifndef vToolBarBase_h__
#define vToolBarBase_h__

#include <QToolBar>
#include "yDataManagement.h"
#include "vMultiView.h"
#include "vStatusBarNotify.h"
#include "vFrameWorkDll.h"
#include "vTreeModel.h"


class V_FRAME_WORK_EXPORT vToolBarBase : public QToolBar
{
	Q_OBJECT
public:
	vToolBarBase(QWidget* parent);
	~vToolBarBase();

	void SetDataManagement(yDataManagement::Pointer dm){m_Dm = dm;};
	void SetMultiView(vMultiView* views){m_Views = views;};
	void SetStatusBar(vStatusBarNotify* status){m_Status = status;};
	void SetTreeModel(vTreeModel* model) {m_Model = model;};

public slots:
	virtual	void OnDataChanged() = 0;
	virtual	void OnLayoutChanged() = 0;
	virtual void OnFocusChanged() = 0;

protected:
	int ParameterCheck();
	virtual void changeEvent(QEvent *event);

	virtual void translateLanguage() = 0;

protected:
	yDataManagement::Pointer m_Dm;
	vMultiView*              m_Views;
	vStatusBarNotify*        m_Status;
	vTreeModel*              m_Model;
};

#endif // vToolBarBase_h__