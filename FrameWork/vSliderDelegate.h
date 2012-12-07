#ifndef vSliderDelegate_h__
#define vSliderDelegate_h__

#include <QItemDelegate>

class vSliderDelegate : public QItemDelegate
{
	Q_OBJECT
public:
	vSliderDelegate(QObject *parent = 0);
	~vSliderDelegate();

	QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option,
		const QModelIndex &index) const;
	void setEditorData(QWidget *editor, const QModelIndex &index) const;
	void setModelData(QWidget *editor, QAbstractItemModel *model,
		const QModelIndex &index) const;
	void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, 
		const QModelIndex &index) const;

protected slots:
	void onValueChanged(int arg);
};
#endif // vSliderDelegate_h__
