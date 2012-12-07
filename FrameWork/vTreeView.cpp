#include "vTreeView.h"
#include <QSlider>
#include <QTreeView>
#include <QLabel>
#include <QHeaderView>
#include "vSliderDelegate.h"
#include "vColorDelegate.h"
#include "vMultiView.h"

#include <QEvent>
#include <QScrollBar>

vTreeView::vTreeView(vTreeModel* model,QWidget* parent /* = 0 */):QDockWidget(parent)
{
	QTreeView* treeView = new QTreeView(parent);
	treeView->setModel(model);
	treeView->setAlternatingRowColors(true);

	treeView->header()->setStyleSheet("QHeaderView::section {background-color:rgb(2,141,207);color:white}");
	treeView->horizontalScrollBar()->setStyleSheet(" QScrollBar:horizontal {background:#484848;}");

	vSliderDelegate* slider = new vSliderDelegate(this);
	treeView->setItemDelegateForColumn(2,slider);

	vColorDelegate* color = new vColorDelegate(this);
	treeView->setItemDelegateForColumn(3,color);

	this->setWidget(treeView);
	this->setAllowedAreas(Qt::LeftDockWidgetArea);
	this->setFeatures(QDockWidget::NoDockWidgetFeatures);

	treeView->resizeColumnToContents(1);
	treeView->resizeColumnToContents(2);
	treeView->resizeColumnToContents(3);

	m_Title = new QLabel(this);
	m_Title->setMinimumHeight(10);

	this->setTitleBarWidget(m_Title);
	this->setMinimumWidth(270);

	this->connect(treeView,SIGNAL(clicked(const QModelIndex &)),this,SLOT(OnSelection(const QModelIndex &)));

	this->translateLanguage();

}

vTreeView::~vTreeView()
{

}

void vTreeView::changeEvent(QEvent *event)
{
	QWidget::changeEvent(event);  
	switch (event->type())  {  
	case QEvent::LanguageChange:
		this->translateLanguage();
		break;  
	default:  
		break;  
	}  
}

void vTreeView::translateLanguage()
{
	m_Title->setText(tr(" 数据管理器"));
}

void vTreeView::ExpandAll()
{
	QTreeView* treeView = dynamic_cast<QTreeView*>(this->widget());
	if (treeView)
	{
		treeView->expandAll();
	}
}

void vTreeView::OnSelection(const QModelIndex &index)
{
	QTreeView* tree = dynamic_cast<QTreeView*>(sender());
	if (tree)
	{
		vTreeModel* model = dynamic_cast<vTreeModel*>(tree->model());
		if (model)
		{
			int node = model->GetNode(index);
			
			yDataManagement::Pointer dm = model->GetDataStorage();
			if (dm)
			{
				dm->SetSelectNodeIndex(node);
				m_Views->OnSelectChanged();
			}
		}
	}
}