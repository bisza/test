#include <QDockWidget>
#include "vTreeModel.h"

class vMultiView;
class QLabel;

class vTreeView : public QDockWidget
{
	Q_OBJECT
public:
	vTreeView(vTreeModel* mode,QWidget* parent = 0);
	~vTreeView();

	void ExpandAll();

	void SetMultiView(vMultiView* views){m_Views = views;}

	public slots:
		void OnSelection(const QModelIndex &index);

protected:
	vMultiView* m_Views;

	QLabel* m_Title;

protected:
	virtual void changeEvent(QEvent *event);
	void translateLanguage();
};