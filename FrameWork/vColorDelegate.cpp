#include "vColorDelegate.h"
#include <QColorDialog>

vColorDelegate::vColorDelegate(QObject *parent /* = 0 */)
{
}

vColorDelegate::~vColorDelegate()
{

}

QWidget* vColorDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QColor value = (index.data(Qt::DecorationRole)).value<QColor>();
	QColorDialog* color = new QColorDialog(value,parent);
	color->setStyleSheet("background-color:#484848;color:#FFFFFF");
	connect(color,SIGNAL(currentColorChanged (const QColor &)),this,SLOT(OnColorChanged()));
	return color;
}

void vColorDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	QColorDialog* color = static_cast<QColorDialog*>(editor);
	QColor value = (index.data(Qt::DecorationRole)).value<QColor>();
	color->setCurrentColor(value);
}

void vColorDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
	QString tmpstr;
	QString str;
	QColorDialog* colorDialog = static_cast<QColorDialog*>(editor);

	QColor color = colorDialog->currentColor();
	tmpstr.setNum(color.red()/255.0);
	str.append(tmpstr);
	str.append(",");
	tmpstr.setNum(color.green()/255.0);
	str.append(tmpstr);
	str.append(",");
	tmpstr.setNum(color.blue()/255.0);
	str.append(tmpstr);

	model->setData(index,str);
}

void vColorDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	editor->setGeometry(option.rect);
}

void vColorDelegate::OnColorChanged()
{
	QColorDialog* color = static_cast<QColorDialog*>(sender());
	emit commitData(color);
}