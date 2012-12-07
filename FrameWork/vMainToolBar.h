#ifndef vMainToolBar_h__
#define vMainToolBar_h__

#include "yDataManagement.h"
#include <QToolBar>

class vMultiView;
class vTreeModel;
class vStatusBarNotify;
class mainWindow;

class vMainToolBar : public QToolBar
{
public:
	vMainToolBar(QWidget* parent = 0);
	~vMainToolBar();

	void SetDataManagement(yDataManagement::Pointer dm){m_Dm = dm;};
	void SetMultiView(vMultiView* views){m_Views = views;};
	void SetStatusBar(vStatusBarNotify* toolBar){m_StatusBar = toolBar;};
	void SetTreeModel(vTreeModel* model) {m_TreeModel = model;};
	void SetMainWindow(mainWindow* mainwindow) {m_MainWindow = mainwindow;};

	void InitializeToolBar();

private:
	yDataManagement::Pointer m_Dm;
	vMultiView* m_Views;
	vStatusBarNotify* m_StatusBar;
	vTreeModel* m_TreeModel;
	mainWindow* m_MainWindow;
};

#endif // vMainToolBar_h__