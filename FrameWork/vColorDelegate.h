
#ifndef vColorDelegate_h__
#define vColorDelegate_h__


#include <QItemDelegate>

class vColorDelegate : public QItemDelegate
{
	Q_OBJECT
public:
	vColorDelegate(QObject *parent = 0);
	~vColorDelegate();

	QWidget* createEditor(QWidget *parent, const QStyleOptionViewItem &option,
		const QModelIndex &index) const;
	void setEditorData(QWidget *editor, const QModelIndex &index) const;
	void setModelData(QWidget *editor, QAbstractItemModel *model,
		const QModelIndex &index) const;
	void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, 
		const QModelIndex &index) const;

protected slots:
	void OnColorChanged();
};
#endif // vColorDelegate_h__
