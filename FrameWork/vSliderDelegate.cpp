#include "vSliderDelegate.h"

vSliderDelegate::vSliderDelegate(QObject *parent /* = 0 */)
{

}

vSliderDelegate::~vSliderDelegate()
{

}

QWidget* vSliderDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	QSlider* slider = new QSlider(Qt::Horizontal,parent);
	slider->autoFillBackground();
	slider->setMaximum(100);
	slider->setMinimum(0);
	slider->installEventFilter(const_cast<vSliderDelegate*>(this));
	connect(slider,SIGNAL(valueChanged(int)),this,SLOT(onValueChanged(int)));
	return slider;
}

void vSliderDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
	QSlider* slider = static_cast<QSlider*>(editor);
	slider->setValue(index.data().toDouble()*100);
}

void vSliderDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
	QString tmpstr;
	QSlider *slider = static_cast<QSlider*>(editor);
	tmpstr.setNum((double)slider->value()/100.0);
	model->setData(index, tmpstr);
}

void vSliderDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	editor->setGeometry(option.rect);
}

void vSliderDelegate::onValueChanged(int arg)
{
	QSlider* slider = static_cast<QSlider*>(sender());
	emit commitData(slider);
}



