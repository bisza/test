#ifndef vStatusBar_h__
#define vStatusBar_h__

#include <QStatusBar>
#include <QProgressBar>
#include <QLabel>
#include "vFrameWorkDll.h"

class V_FRAME_WORK_EXPORT vStatusBar : public QStatusBar
{
	Q_OBJECT
public:
	vStatusBar(QWidget* parent = 0);
	~vStatusBar();

	public slots:
		void SetProgress(int progress);

private:
	QProgressBar* m_ProgressBar;
};
#endif // vStatusBar_h__