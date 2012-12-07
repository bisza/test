#include "vStatusBarNotify.h"

vStatusBarNotify::vStatusBarNotify(QObject* parent /* = 0 */):QObject(parent)
{

}

vStatusBarNotify::~vStatusBarNotify()
{

}

void vStatusBarNotify::SendPermentMessage(QString str)
{
	emit permentMessage(str);
}


void vStatusBarNotify::ProgressNotify(int progress)
{
	emit progressNotify(progress);
}