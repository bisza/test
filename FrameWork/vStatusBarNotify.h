#ifndef vStatusBarNotify_h__
#define vStatusBarNotify_h__

#include <QObject>
#include <QString>
#include "vFrameWorkDll.h"

class V_FRAME_WORK_EXPORT vStatusBarNotify : public QObject
{
	Q_OBJECT
public:
	vStatusBarNotify(QObject* parent = 0);
	~vStatusBarNotify();

public:
	void SendPermentMessage(QString str);
	void ProgressNotify(int progress);

signals:
	void permentMessage(QString str);
	void progressNotify(int progress);
};


#endif // vStatusBarNotify_h__