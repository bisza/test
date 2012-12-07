#ifndef vLayout_h__
#define vLayout_h__

#include "vToolBarBase.h"

class QAction;

class vLayout : public vToolBarBase
{
	Q_OBJECT
public:
	vLayout(QWidget* parent = 0);
	~vLayout();

	public slots:
		virtual void OnLayoutChanged();
		virtual void OnDataChanged();
		virtual void OnFocusChanged();

		void SetLayoutOne();
		void SetLayoutTwo();
		void SetLayoutThree();
		void SetLayoutFour();
		void SetLayoutProject();
		void SetLayout3D();

protected:
	QAction* one;
	QAction* two;
	QAction* three;
	QAction* four;
	QAction* project;
	QAction* threeD;

protected:
	virtual void translateLanguage();
};

#endif // vLayout_h__