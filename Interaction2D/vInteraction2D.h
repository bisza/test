#ifndef vInteraction2D_h__
#define vInteraction2D_h__

#include "vToolBarBase.h"
#include "vtkAbstractWidget.h"

class QAction;

class vInteraction2D : public vToolBarBase
{
	Q_OBJECT

public:
	vInteraction2D(QWidget* parent = 0);
	~vInteraction2D();

	public slots:
		virtual void OnDataChanged();
		virtual void OnLayoutChanged();
		virtual void OnFocusChanged();

		void OnPan();
		void OnDolly();
		void OnWindowLevel();
		void OnReslice();
		void OnDistance();
		void OnAngle();
		void OnReset();

		void OnWindowLevelSet();

protected:
	void ReadDefaultProperty();
	void ClearNoDealedWidget();

	virtual void translateLanguage();

protected:
	std::vector<std::string> m_Window;
	std::vector<std::string> m_Level;
	std::vector<std::string> m_Name;

	std::list<vtkSmartPointer<vtkAbstractWidget>> m_Widget;

	QAction* pan;
	QAction* dolly;
	QAction* window;
	QAction* reslice;
	QAction* distance;
	QAction* angle;
	QAction* reset;
};
#endif // vInteraction2D_h__