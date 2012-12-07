#ifndef vLogoToolBar_h__
#define vLogoToolBar_h__

#include <QToolBar>

class mainWindow;
class QLabel;
class QAction;

class vLogoToolBar : public QToolBar
{
	Q_OBJECT
public:
	vLogoToolBar(QWidget* parent = 0);
	~vLogoToolBar();

public:
	void SetMainWindow(mainWindow* mainWin){m_MainWindow = mainWin;};

	protected slots:
		void OnCloseWindow();
		void OnMinWindow();
		void OnAboutDialog();
protected:
	mainWindow* m_MainWindow;
	QLabel* m_Title;

protected:
	void changeEvent(QEvent *event);
	void translateLanguage();

	QAction* about;
	QAction* min;
	QAction* close;

};
#endif // vLogoToolBar_h__