#include "vLayout.h"
#include "vView3D.h"
#include <QAction>
#include <QEvent>

vLayout::vLayout(QWidget* parent /* = 0 */):vToolBarBase(parent)
{
    one = new QAction(this);
	one->setIcon(QIcon(QPixmap("./icon/layout1.png")));
	one->setText("One");
	one->setCheckable(true);
	this->connect(one,SIGNAL(triggered()),this,SLOT(SetLayoutOne()));
	this->addAction(one);

	two = new QAction(this);
	two->setIcon(QIcon(QPixmap("./icon/layout2.png")));
	two->setText("Two");
	two->setCheckable(true);
	this->connect(two,SIGNAL(triggered()),this,SLOT(SetLayoutTwo()));
	this->addAction(two);

	three = new QAction(this);
	three->setIcon(QIcon(QPixmap("./icon/layout3.png")));
	three->setText("Three");
	three->setCheckable(true);
	this->connect(three,SIGNAL(triggered()),this,SLOT(SetLayoutThree()));
	this->addAction(three);

	four = new QAction(this);
	four->setIcon(QIcon(QPixmap("./icon/layout4.png")));
	four->setText("Four");
	four->setCheckable(true);
	this->connect(four,SIGNAL(triggered()),this,SLOT(SetLayoutFour()));
	this->addAction(four);

	project = new QAction(this);
	project->setIcon(QIcon(QPixmap("./icon/layoutProject.png")));
	project->setText("Project");
	project->setCheckable(true);
	this->connect(project,SIGNAL(triggered()),this,SLOT(SetLayoutProject()));
	this->addAction(project);

	threeD = new QAction(this);
	threeD->setIcon(QIcon(QPixmap("./icon/layout3D.png")));
	threeD->setText("ThreeD");
	threeD->setCheckable(true);
	this->connect(threeD,SIGNAL(triggered()),this,SLOT(SetLayout3D()));
	this->addAction(threeD);

	QActionGroup* group = new QActionGroup(this);
	group->addAction(one);
	group->addAction(two);
	group->addAction(three);
	group->addAction(four);
	group->addAction(threeD);
	group->addAction(project);

	this->setDisabled(true);
	this->translateLanguage();
}

vLayout::~vLayout()
{

}

void vLayout::translateLanguage()
{
	QString str(tr("阅图区域视图布局为二维窗口一"));
	one->setToolTip(tr("二维布局一"));
	one->setStatusTip(str);

	str = tr("阅图区域视图布局二维窗口二");
	two->setToolTip(tr("二维布局二"));
	two->setStatusTip(str);

	str = tr("阅图区域视图布局二维窗口三");
	three->setToolTip(tr("二维布局三"));
	three->setStatusTip(str);

	str = tr("阅图区域视图布局二维窗口四");
	four->setToolTip(tr("二维布局四"));
	four->setStatusTip(str);

	str = tr("阅图区域视图布局工程视图");
	project->setToolTip(tr("工程布局"));
	project->setStatusTip(str);

	str = tr("阅图区域视图布局三维视图");
	threeD->setToolTip(tr("三维布局"));
	threeD->setStatusTip(str);
}

void vLayout::OnDataChanged()
{
	if (m_Dm)
	{
		if (m_Dm->GetNumOfNodes() == 0)
		{
			this->setEnabled(false);
		}
		else
		{
			this->setEnabled(true);

			if (m_Dm->GetVisibleRootIndex() >= 0)
			{
				one->setEnabled(true);
				two->setEnabled(true);
				three->setEnabled(true);
				four->setEnabled(true);
				project->setEnabled(true);
			}
			else
			{
				one->setEnabled(false);
				two->setEnabled(false);
				three->setEnabled(false);
				four->setEnabled(false);
				project->setEnabled(false);
			}

			std::list<int> indexes = m_Dm->GetNodesFromType(SURFACE_DATA);
			if (!indexes.empty())
			{
				std::list<int>::iterator it = indexes.begin();
				int i = 0;
				while(it != indexes.end())
				{
					bool b = m_Dm->GetNodeVisible(*it++);
					if (b)
					{
						i++;
						break;
					}
				}

				if (i)
				{
					project->setEnabled(true);
					threeD->setEnabled(true);
				}
				else
				{
					threeD->setEnabled(false);
				}
			}
			else
			{
				threeD->setEnabled(false);
			}
		}

	}
}

void vLayout::OnLayoutChanged()
{

}

void vLayout::SetLayoutOne()
{
	if(!this->ParameterCheck())
		return;

	m_Views->SetLayout(vMultiView::ONE_2D);

}

void vLayout::OnFocusChanged()
{

}

void vLayout::SetLayoutTwo()
{
	if(!this->ParameterCheck())
		return;

	m_Views->SetLayout(vMultiView::TWO_2D);

}

void vLayout::SetLayoutThree()
{
	if(!this->ParameterCheck())
		return;

	m_Views->SetLayout(vMultiView::THREE_2D);
}

void vLayout::SetLayoutFour()
{
	if(!this->ParameterCheck())
		return;

	m_Views->SetLayout(vMultiView::FOUR_2D);
}

void vLayout::SetLayoutProject()
{
	if(!this->ParameterCheck())
		return;

	m_Views->SetLayout(vMultiView::PROJECT);
}

void vLayout::SetLayout3D()
{
	if(!this->ParameterCheck())
		return;

	m_Views->SetLayout(vMultiView::THREED);
}