#include <QMainWindow>
#include "vFrameWorkDll.h"
#include "vMultiView.h"
#include "yDataManagement.h"
#include "vStatusBarNotify.h"
#include "vTreeModel.h"

#include <QTranslator>

class QCloseEvent;

class V_FRAME_WORK_EXPORT mainWindow : public QMainWindow
{
    Q_OBJECT
public:
	mainWindow();
	virtual ~mainWindow();

public slots:
	void changeLanguage();

protected:
	void Initialize();
	void InitializeDataTree();
	void InitializePulgin();

	virtual QMenu *createPopupMenu(){return 0;};

protected:
	virtual void closeEvent(QCloseEvent *event);
	virtual void changeEvent(QEvent *event);

	void translateLanguage();

private:
	yDataManagement::Pointer m_Dm;
	vMultiView* m_Views;
	vStatusBarNotify* m_StatusBar;
	vTreeModel* m_TreeModel;
	QTranslator* m_Translater;
	QTranslator* m_LocalTranslater;

	bool isChinese;
};
