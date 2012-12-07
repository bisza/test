#include "vInteraction2D.h"
#include "vView2D.h"
#include "vInteractorStyle.h"
#include "vtkRenderWindow.h"
#include "yDataStorageConfigure.h"
#include "vtkDistanceWidget.h"
#include "vtkAngleWidget.h"
#include "Markup.h"
#include <QAction>
#include <QMenu>
#include <QEvent>

vInteraction2D::vInteraction2D(QWidget* parent /* = 0 */):vToolBarBase(parent)
{
	pan = new QAction(this);
	pan->setIcon(QIcon(QPixmap("./icon/pan.png")));
	pan->setText("Pan");
	pan->setCheckable(true);
	pan->setChecked(true);
	this->connect(pan,SIGNAL(triggered()),this,SLOT(OnPan()));
	this->addAction(pan);

	dolly = new QAction(this);
	dolly->setIcon(QIcon(QPixmap("./icon/dolly.png")));
	dolly->setText("Dolly");
	dolly->setCheckable(true);
	this->connect(dolly,SIGNAL(triggered()),this,SLOT(OnDolly()));
	this->addAction(dolly);

	window = new QAction(this);
	window->setIcon(QIcon(QPixmap("./icon/windowLevel.png")));
	window->setText("W/L");
	window->setCheckable(true);
	this->connect(window,SIGNAL(triggered()),this,SLOT(OnWindowLevel()));

	QMenu* menu = new QMenu(this);
	menu->setStyleSheet("QMenu { background-color: #ABABAB; border: 1px solid black;}" \
		"QMenu::item {background-color: transparent;}" \
	    "QMenu::item:selected {background-color: #654321;}");

	window->setMenu(menu);	
	this->addAction(window);

	this->ReadDefaultProperty();

	int a = 0;
	for (int i = 0; i < (int)m_Name.size(); i++)
	{
		QAction* action1 = new QAction(menu);
		std::string str = m_Name[i] + "(" + m_Window[i] + "," + m_Level[i] + ")";
		action1->setText(QString::fromStdString(str));
		action1->setData(a++);
		connect(action1,SIGNAL(triggered()),this,SLOT(OnWindowLevelSet()));

		menu->addAction(action1);
	}
	
	reslice = new QAction(this);
	reslice->setIcon(QIcon(QPixmap("./icon/reslice.png")));
	reslice->setText("Reslice");
	reslice->setCheckable(true);
	this->connect(reslice,SIGNAL(triggered()),this,SLOT(OnReslice()));
	this->addAction(reslice);

	distance = new QAction(this);
	distance->setIcon(QIcon(QPixmap("./icon/distance.png")));
	distance->setText("Distance");
	this->connect(distance,SIGNAL(triggered()),this,SLOT(OnDistance()));
	this->addAction(distance);

	angle = new QAction(this);
	angle->setIcon(QIcon(QPixmap("./icon/angle.png")));
	angle->setText("Angle");
	this->connect(angle,SIGNAL(triggered()),this,SLOT(OnAngle()));
	this->addAction(angle);

	reset = new QAction(this);
	reset->setIcon(QIcon(QPixmap("./icon/reset.png")));
	reset->setText("Reset");
	this->connect(reset,SIGNAL(triggered()),this,SLOT(OnReset()));
	this->addAction(reset);

	QActionGroup* group = new QActionGroup(this);
	group->addAction(pan);
	group->addAction(dolly);
	group->addAction(window);
	group->addAction(reslice);

	this->setDisabled(true);
	this->translateLanguage();
}

vInteraction2D::~vInteraction2D()
{

}

void vInteraction2D::translateLanguage()
{
	QString str(tr("����϶����ƶ�ͼ��Ŀ������Ϊ��ά����"));
	pan->setToolTip(tr("�ƶ�ͼ��"));
	pan->setStatusTip(str);

	str = tr("����϶�����ͼ��Ŀ������Ϊ��ά����");
	dolly->setToolTip(tr("����ͼ��"));
	dolly->setStatusTip(str);

	str = tr("����϶����ڴ���λ�������ƶ����ڴ��������ƶ����ڴ�λ��Ŀ������Ϊ��ά����");
	window->setToolTip(tr("����λ"));
	window->setStatusTip(str);

	str = tr("����϶�ͼ��ҳ���������»��ߴ������ϣ�Ŀ������Ϊ��ά����");
	reslice->setToolTip(tr("���·�ҳ"));
	reslice->setStatusTip(str);

	str = tr("����һ��ά���������һ�����������");
	distance->setToolTip(tr("�������"));
	distance->setStatusTip(str);

	str = tr("����һ��ά���������һ�ǶȲ�������");
	angle->setToolTip(tr("�ǶȲ���"));
	angle->setStatusTip(str);

	str = tr("����λ��ͼ��λ������,ɾ�����еĲ�������");
	reset->setToolTip(tr("�ָ�Ĭ��"));
	reset->setStatusTip(str);
}

void vInteraction2D::OnWindowLevelSet()
{
	QAction* action = dynamic_cast<QAction*>(sender());

	if (action)
	{
		int i = action->data().toInt();
		double window = .0;
		double level = .0;
		if (i < (int)m_Window.size())
		{
			ConvertFromString(window,m_Window[i]);
			ConvertFromString(level,m_Level[i]);

			std::list<vViewBase*> views = m_Views->GetViews();
			std::list<vViewBase*>::iterator it = views.begin();

			vView2D* view = NULL;
			while(it != views.end())
			{
				view = dynamic_cast<vView2D*>(*it++);
				if (view)
				{
					view->SetWindowLevel(window,level);
				}
			}
		}
	}

}

void vInteraction2D::ReadDefaultProperty()
{
	CMarkup xml;

	if(xml.Load("./default/WindowLevelPreset.xml") == false)
		return;

	if (xml.FindElem("WindowLevelList"))
	{
		xml.IntoElem();
		while(xml.FindElem("WindowLevel"))
		{
			std::string window = xml.GetAttrib("Window");
			m_Window.push_back(window);

			std::string level = xml.GetAttrib("Level");
			m_Level.push_back(level);

			std::string name = xml.GetAttrib("Comment");
			m_Name.push_back(name);
		}
	}
}

void vInteraction2D::OnReset()
{
	if(!this->ParameterCheck())
		return;

	std::list<vViewBase*> views = m_Views->GetViews();
	std::list<vViewBase*>::iterator it = views.begin();

	vView2D* view = NULL;
	while(it != views.end())
	{
		view = dynamic_cast<vView2D*>(*it++);
		if (view)
		{
			view->SetWindowLevel(400,0);
		}
	}

	if (m_Widget.size() == 0)
		return;
	
	std::list<vtkSmartPointer<vtkAbstractWidget> >::iterator itW = m_Widget.begin();
	while(itW != m_Widget.end())
	{
		(*itW++)->Off();
	}
	
	m_Widget.clear();
}

void vInteraction2D::OnDataChanged()
{
	if(!this->ParameterCheck())
		return;

	int node = m_Dm->GetVisibleRootIndex();

	if (node < 0)
	{
		this->setDisabled(true);
	}
	else
	{
		this->setDisabled(false);
	}

	if (this->isEnabled())
	{
		if (vViewBase::m_LastView == NULL)
		{
			return;
		}

		vView2D* view = dynamic_cast<vView2D*>(vViewBase::m_LastView);
		if (!view)
		{
			this->setDisabled(true);
		}
	}
}

void vInteraction2D::OnLayoutChanged()
{
	if (m_Views->GetLayout() == vMultiView::THREED)
	{
		this->setDisabled(true);
	}
	else
	{
		if (m_Views->GetLayout() == vMultiView::TWO_2D ||
			m_Views->GetLayout() == vMultiView::THREE_2D ||
			m_Views->GetLayout() == vMultiView::FOUR_2D)
		{
			reslice->setEnabled(false);
		}
		else
		{
			reslice->setEnabled(true);
		}

		this->OnDataChanged();
	}
}

void vInteraction2D::OnFocusChanged()
{
	this->OnDataChanged();
}

void vInteraction2D::OnPan()
{
	if(!this->ParameterCheck())
		return;

	std::list<vViewBase*> views = m_Views->GetViews();
	std::list<vViewBase*>::iterator it = views.begin();
	vtkSmartPointer<vInteractorStyle> stlye = NULL;

	while(it != views.end())
	{
		vtkSmartPointer<vtkInteractorObserver> temp = (*it++)->GetRenderWindow()->GetInteractor()->GetInteractorStyle();
		stlye = vInteractorStyle::SafeDownCast(temp);
		if (stlye)
		{
			stlye->SetInteractionMode(vInteractorStyle::PAN);
		}
	}

	this->ClearNoDealedWidget();
}

void vInteraction2D::OnReslice()
{
	if(!this->ParameterCheck())
		return;

	std::list<vViewBase*> views = m_Views->GetViews();
	std::list<vViewBase*>::iterator it = views.begin();
	vtkSmartPointer<vInteractorStyle> stlye = NULL;

	while(it != views.end())
	{
		vtkSmartPointer<vtkInteractorObserver> temp = (*it++)->GetRenderWindow()->GetInteractor()->GetInteractorStyle();
		stlye = vInteractorStyle::SafeDownCast(temp);
		if (stlye)
		{
			stlye->SetInteractionMode(vInteractorStyle::_RESLICE);
		}
	}

	this->ClearNoDealedWidget();
}

void vInteraction2D::OnWindowLevel()
{
	if(!this->ParameterCheck())
		return;

	std::list<vViewBase*> views = m_Views->GetViews();
	std::list<vViewBase*>::iterator it = views.begin();

	vtkSmartPointer<vInteractorStyle> stlye = NULL;
	vView2D* view = NULL;
	vtkSmartPointer<vtkLookupTable> lut = NULL;
	vtkSmartPointer<vtkImagePlaneWidget> plane;

	while(it != views.end())
	{
		view = dynamic_cast<vView2D*>(*it++);
		if (view)
		{
			vtkSmartPointer<vtkInteractorObserver> temp = 
				view->GetRenderWindow()->GetInteractor()->GetInteractorStyle();
			stlye = vInteractorStyle::SafeDownCast(temp);
			if (stlye)
			{
				stlye->SetInteractionMode(vInteractorStyle::_WINDOW_LEVEL);
			}
		}
	}

	this->ClearNoDealedWidget();
}

void vInteraction2D::OnAngle()
{
	vtkSmartPointer<vtkAngleWidget> widget = vtkSmartPointer<vtkAngleWidget>::New();
	widget->CreateDefaultRepresentation();
	std::list<vViewBase*> views = m_Views->GetViews();
	std::list<vViewBase*>::iterator it = views.begin();

	vView2D* view = NULL;
	while (it != views.end())
	{
		view = dynamic_cast<vView2D*>(*it++);
		if (view)
		{
			view->AddWidgetOberser(widget);
		}
	}

	m_Widget.push_back(widget);
}

void vInteraction2D::ClearNoDealedWidget()
{
	std::list<vViewBase*> views = m_Views->GetViews();
	std::list<vViewBase*>::iterator it = views.begin();

	vView2D* view = NULL;
	while (it != views.end())
	{
		view = dynamic_cast<vView2D*>(*it++);
		if (view)
		{
			view->AddWidgetOberser(NULL);
		}
	}
}

void vInteraction2D::OnDistance()
{
	vtkSmartPointer<vtkDistanceWidget> widget = vtkSmartPointer<vtkDistanceWidget>::New();
	widget->CreateDefaultRepresentation();
	std::list<vViewBase*> views = m_Views->GetViews();
	std::list<vViewBase*>::iterator it = views.begin();

	vView2D* view = NULL;
	while (it != views.end())
	{
		view = dynamic_cast<vView2D*>(*it++);
		if (view)
		{
			view->AddWidgetOberser(widget);
		}
	}

	m_Widget.push_back(widget);
}

void vInteraction2D::OnDolly()
{
	if(!this->ParameterCheck())
		return;

	std::list<vViewBase*> views = m_Views->GetViews();
	std::list<vViewBase*>::iterator it = views.begin();
	vtkSmartPointer<vInteractorStyle> stlye = NULL;

	while(it != views.end())
	{
		vtkSmartPointer<vtkInteractorObserver> temp = (*it++)->GetRenderWindow()->GetInteractor()->GetInteractorStyle();
		stlye = vInteractorStyle::SafeDownCast(temp);
		if (stlye)
		{
			stlye->SetInteractionMode(vInteractorStyle::DOLLY);
		}
	}
	this->ClearNoDealedWidget();
}
