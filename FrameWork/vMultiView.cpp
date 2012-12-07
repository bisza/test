#include "vMultiView.h"
#include <QHBoxLayout>
#include <QSplitter>
#include "vView2D.h"
#include "vView3D.h"
#include "vtkEventQtSlotConnect.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkInteractorStyle.h"
#include "vtkImageData.h"
#include "yDataStorageConfigure.h"
#include "vInteractorStyle.h"
#include "vTreeView.h"
#include "vDICOMReader.h"

#include <QWheelEvent>
#include <QTime>
#include <QCoreApplication>
#include <QApplication>
#include <QSizePolicy>
#include <QMouseEvent>
#include <QMessageBox>
#include <QFileDialog>
#include <QString>

vMultiView::vMultiView(QWidget *parent,yDataManagement::Pointer dm)
	: QWidget(parent)
{
	m_MainLayout = new QHBoxLayout(this);
	this->setLayout(m_MainLayout);	
	m_MainLayout->setContentsMargins(0,0,0,0);

	m_MainSplitter = new QSplitter(this);
	m_MainLayout->addWidget(m_MainSplitter);

	QWidget* widget = new QWidget(m_MainSplitter);
	m_MainSplitter->addWidget(widget);
	widget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

	this->connect(this,SIGNAL(layoutChanged()),this,SLOT(OnLayoutChanged()));
	this->connect(this,SIGNAL(dataChanged()),this,SLOT(OnDataChanged()));

	m_LastModelIndexes.clear();
	m_Views.clear();
	m_LastRootIndex = -1;

	m_Dm = dm;
	m_Layout = THREED;
	isFull = false;
	FullLastView = NULL;

	this->Create2DView(1); //For other use,It is just an cover
	this->SetLayout(ONE_2D);
	this->setMinimumSize(400,300);
}

vMultiView::~vMultiView()
{

}

void vMultiView::ExpandAllNodeInTreeView()
{
	if (m_TreeView)
	{
		m_TreeView->ExpandAll();
	}
}

void vMultiView::SetLayout(LayoutType type)
{
	if (m_Layout == type)
		return;

	m_Layout = type;
	m_CurrentPosition = 0;

	emit layoutChanged();

	std::list<vViewBase*>::iterator it = m_Views.begin();

	if (m_Layout == ONE_2D ||
		m_Layout == TWO_2D ||
		m_Layout == THREE_2D ||
		m_Layout == FOUR_2D)
	{
		(*it)->SetFoucs();
	}
	else
	{
		it = m_Views.end();
		--it;

		(*it)->SetFoucs();
	}
}

void vMultiView::DataChanged()
{
	emit dataChanged();
}

void vMultiView::ClearView()
{
	std::list<vViewBase*>::iterator it = m_Views.begin();
	vViewBase* view = NULL;
	while(it != m_Views.end())
	{
		view =(*it++);
		view->UnInstallPipeLine();
		view->GetRenderWindow()->Render();
	}

	m_LastModelIndexes.clear();
	m_LastRootIndex = -1;
}

void vMultiView::OnLayoutChanged()
{
	//This is a trick for first time initialize
	static int first = 0;
	if (first == 1)
	{
		this->Create2DView(18);
		this->Create3DView();
	}
	first++;

	switch (m_Layout)
	{
	case ONE_2D:
		this->Layout2D(1);
		break;
	case TWO_2D:
		this->Layout2D(2);
		break;
	case THREE_2D:
		this->Layout2D(3);
		break;
	case FOUR_2D:
		this->Layout2D(4);
		break;
	case PROJECT:
		this->LayoutProject();
		break;
	case THREED:
		this->Layout3D();
		break;
	}

	emit dataChanged();
}

void vMultiView::Layout2D(int num)
{
	delete m_MainLayout;

	m_MainLayout = new QHBoxLayout(this);
	m_MainLayout->setContentsMargins(0,0,0,0);
	this->setLayout(m_MainLayout);

	std::list<QWidget*>::iterator it = m_ViewContainer.begin();

	QSplitter* splitterV = new QSplitter(Qt::Vertical,this);
	splitterV->setHandleWidth(1);

	QList<int> spaceV;

	for (int i = 0; i < num; i++)
	{
		QList<int> spaceH;
		QSplitter* splitterH = new QSplitter(Qt::Horizontal,splitterV);
		splitterH->setHandleWidth(1);
		for (int j = 0; j < num; j++)
		{
			splitterH->addWidget(*it++);
			spaceH<<1000;
		}
		splitterH->setSizes(spaceH);
		splitterV->addWidget(splitterH);
		spaceV<<1000;
	}

	splitterV->setSizes(spaceV);
	m_MainLayout->addWidget(splitterV);

	//////////////////////////////////////////////////////////////////////////
	if (m_Dm->GetVisibleRootIndex() >= 0)
	{
		std::list<vViewBase*>::iterator it1 = m_Views.begin();
		vView2D* view = dynamic_cast<vView2D*>(*it1);
		int orgin = view->GetSliceIndex();
		view->SetSliceIndex(orgin + num*num);

		int a = 0;
		if ((orgin + num*num) > (view->GetSliceIndex()))
		{
			if (orgin > num*num)
			{
				a = orgin - num*num;
				a = a - a%num;
			}
		}
		else
		{
			a = orgin;
			a = a - a%num;
		}

		view->SetSliceIndex(orgin);

		if(FullLastView)
		{
			a = dynamic_cast<vView2D*>(FullLastView)->GetSliceIndex();
		}

		if(isFull)
		{
			vViewBase* view = NULL;
			for(int i = 0; i < num*num; i++)
			{
				view = *it1++;
				if(view == FullLastView)
				{
					a = dynamic_cast<vView2D*>(FullLastView)->GetSliceIndex()- i;
					a = a - a%num;
					break;
				}
			}
		}

		it1 = m_Views.begin();

		for (int i = 0; i < num*num; i++)
		{
			vView2D* view = dynamic_cast<vView2D*>(*it1++);
			view->SetSliceIndex(a++);
		}
	}
}

void vMultiView::FoucsChanged()
{
	emit focusChanged();
}

void vMultiView::OnSelectChanged()
{
	int node = m_Dm->GetSelectNodeIndex();

	std::list<vViewBase*>::iterator it = m_Views.end();
	--it;

	vView3D* view = dynamic_cast<vView3D*>(*it);
	view->UpdateModelText(node);
}

void vMultiView::OnPropertyChanged(int node)
{
	if (m_Dm->GetDataNodeType(node) == SURFACE_DATA)
	{
		std::list<vViewBase*>::iterator it = m_Views.end();
		--it;

		vView3D* view = dynamic_cast<vView3D*>(*it);
		view->UpdateModelProperty(node);
	}
	else
	{
		this->DataChanged();
	}
}

void vMultiView::Layout3D()
{
	delete m_MainLayout;
	m_MainLayout = new QHBoxLayout(this);
	m_MainLayout->setContentsMargins(0,0,0,0);	
	this->setLayout(m_MainLayout);

	std::list<QWidget*>::iterator it = --m_ViewContainer.end();

	QSplitter* splitterH = new QSplitter(Qt::Horizontal,this);
	splitterH->setHandleWidth(1);
	splitterH->addWidget(*it);

	m_MainLayout->addWidget(splitterH);
}

/************************************************************************/
/* LayoutFull Function can be changed more quickly                      */
/************************************************************************/

void vMultiView::LayoutFull()
{	
	switch(m_Layout)
	{
	case ONE_2D:
		if(isFull)
		{
			this->SetLayout(FullLayoutType);
			FullLastView = NULL;
			isFull = false;
		}
		break;
	case TWO_2D:
	case THREE_2D:
	case FOUR_2D:
		if(!isFull)
		{
			FullLayoutType = m_Layout;
			FullLastView = vViewBase::m_LastView;
			this->SetLayout(ONE_2D);
			isFull = true;
		}
		break;
	case PROJECT:
		if(!isFull)
		{
			FullLastView = vViewBase::m_LastView;

			delete m_MainLayout;
			m_MainLayout = new QHBoxLayout(this);
			m_MainLayout->setContentsMargins(0,0,0,0);	
			this->setLayout(m_MainLayout);

			QSplitter* splitterH = new QSplitter(Qt::Horizontal,this);
			splitterH->setHandleWidth(1);
			splitterH->addWidget(vViewBase::m_LastView->GetContainer());

			m_MainLayout->addWidget(splitterH);
			isFull = true;
			m_Layout = THREED;
		}
		break;
	case THREED:
		if(isFull)
		{
			FullLastView = NULL;
			isFull = false;
			this->SetLayout(PROJECT);
		}
	default:
		break;
	}
}

void vMultiView::LayoutProject()
{
	delete m_MainLayout;
	m_MainLayout = new QHBoxLayout(this);
	m_MainLayout->setContentsMargins(0,0,0,0);
	this->setLayout(m_MainLayout);

	std::list<QWidget*>::iterator it = --m_ViewContainer.end();

	QSplitter* splitterH = new QSplitter(Qt::Horizontal,this);
	splitterH->setHandleWidth(1);

	//Add 3D View
	splitterH->addWidget(*it);

	//Add 2D View 0
	QSplitter* splitterV = new QSplitter(Qt::Vertical,this);
	splitterV->setHandleWidth(1);
	--it;
	splitterV->addWidget(*it);

	//Add 2D View 1
	--it;
	splitterV->addWidget(*it);

	//Add 2D View 2
	--it;		
	splitterV->addWidget(*it);

	splitterH->addWidget(splitterV);

	QList<int> space1;
	space1<<1000<<1000<<1000;
	splitterV->setSizes(space1);

	QList<int> space;
	space<<750<<250;
	splitterH->setSizes(space);

	m_MainLayout->addWidget(splitterH);
}


void vMultiView::OnMouseWheelDeal(int num,unsigned long events)
{
	int upBound = 0;  //unBound of slider
	int remain = 0;
	int tempL = m_Dim[2]%num;  //last image

	//Calculate the upper bound of slider, the lower bound will be set as 0
	if (tempL)
	{
		upBound = m_Dim[2]/num - num + 1 ;
		remain = num*(num - 1) + tempL;
	}
	else
	{
		upBound = m_Dim[2]/num - num;
		remain = num*num;
	}

	int lastPostion = 0;  //The first image showing in the view
	 
	int count = 0;

	//Update the current position of image
	if ((events == vtkCommand::MouseWheelBackwardEvent) && (m_CurrentPosition < upBound))
		++m_CurrentPosition;
	if (events == vtkCommand::MouseWheelForwardEvent && m_CurrentPosition > 0)
		--m_CurrentPosition;

	std::list<vViewBase*>::iterator it = m_Views.begin();
	vView2D* view = NULL;

	for (int i = 0; i < num*num; i++)
	{
		view = dynamic_cast<vView2D*>(*it);
		if (view)
		{	
			//Get the current image index
			int index = view->GetSliceIndex();
			//if the view is the first one, calculate the last position
			if (it == m_Views.begin())
				lastPostion = index/num;
	
			//if the current larger than last, then increase num
			if (m_CurrentPosition > lastPostion)
				if (m_CurrentPosition != upBound)
					view->SetSliceIndex(index + num);
				else
					if (count < remain)
						view->SetSliceIndex(index + num);
					else
						view->HideImage();
	
			//if the current smaller than last,then decrease num
			if (m_CurrentPosition < lastPostion)
				if (lastPostion != upBound)
					view->SetSliceIndex(index - num);
				else
					if (count < remain)
						view->SetSliceIndex(index - num);
					else
						view->ShowImage();
	
			this->UpdateCornerText(view);
		}
		++it;
		++count;
	}

	this->Render();
}

void vMultiView::Render()
{
	std::list<vViewBase*>::iterator it = m_Views.begin();
	while(it != m_Views.end())
	{
		(*it++)->GetRenderWindow()->Render();
	}
}

void vMultiView::OnDataChanged()
{
	QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));

	if (m_Dm.IsNull()  || m_Dm->GetNumOfNodes() == 0)
	{
		QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));
		return;
	}

	int rootIndex = m_Dm->GetVisibleRootIndex();

	if (m_LastRootIndex != rootIndex)
	{
		if (rootIndex >= 0)
		{
			this->AddImageData(rootIndex);			
			m_LastRootIndex = rootIndex;
		}
		else
		{
			this->UpdateImageData(m_LastRootIndex);
		}
	}
	else
	{
		this->UpdateImageData(rootIndex);
	}

	std::list<int> nodes = m_Dm->GetNodesFromType(SURFACE_DATA);
	if (nodes.size() != 0)
	{
		if (m_LastModelIndexes.size() == nodes.size())
		{
			this->UpdateSurfaceData(nodes);
		}
		else
		{
			this->AddSurfaceData(nodes);			
		}
	}

	QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));
}

void vMultiView::AddImageData(int rootIndex)
{
	vtkSmartPointer<vtkImageData> data = 
		vtkImageData::SafeDownCast(m_Dm->GetData(rootIndex));
	if (!data)
	{
		if (m_LastRootIndex >= 0)
		{
			QMessageBox* msgBox = new QMessageBox(this);
			msgBox->setStyleSheet("background-color:#484848;color:#FFFFFF");
			msgBox->setWindowTitle(tr("旭东阅图软件"));
			msgBox->setText(tr("需要重新读取其他文件，可能会占用一点时间\n是否继续？"));
			msgBox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
			msgBox->setDefaultButton(QMessageBox::No);
			msgBox->setIcon(QMessageBox::Information);
			int ret = msgBox->exec();

			delete msgBox;

			if (ret == QMessageBox::No)
			{
				return;
			}
		}

		bool exist = m_Dm->IsPropertyExist(rootIndex,"temp_d");
		std::string fileString = "";

		if (exist)
		{
			fileString = m_Dm->GetNodeProperty(rootIndex,"temp_d");
		}
		else
		{
			QFileDialog* dialog = new QFileDialog();
			dialog->setStyleSheet("background-color:#484848;color:#FFFFFF");
			dialog->setFileMode(QFileDialog::ExistingFile);
			dialog->setWindowTitle(tr("选择DICOM文件目录"));

			QString fileDir;
			if (dialog->exec() ==  QDialog::Accepted)
			{
				fileDir = dialog->selectedFiles()[0];
				delete dialog;
			}
			else
			{
				delete dialog;
				return;
			}

            int index = fileDir.lastIndexOf("/");
			fileString = fileDir.left(index).toStdString();
		}

		vDICOMReader* reader = new vDICOMReader();
		reader->SetDirectoryName(fileString);
		reader->SetStatusBar(m_Status);
		reader->Update();

		if (reader->GetData())
		{
			data = reader->GetData();
			m_Dm->SetData(rootIndex,data);
			m_Dm->SetData(m_LastRootIndex,NULL);

			delete reader;
		}
		else
		{
			m_Status->SendPermentMessage(QString(fileString.c_str()));

			QMessageBox* msgBox = new QMessageBox(this);
			msgBox->setStyleSheet("background-color:#484848;color:#FFFFFF");
			msgBox->setWindowTitle(tr("旭东阅图软件"));
			msgBox->setText(tr("文件读取错误，数据出错！"));
			msgBox->setStandardButtons(QMessageBox::Ok);
			msgBox->setIcon(QMessageBox::Critical);
			msgBox->exec();

			delete msgBox;

			return;
		}
	}

	data->GetDimensions(m_Dim);

	std::vector<int> child = m_Dm->GetChildrenIndexFromParentIndex(rootIndex);

	int start = 0; 
	std::list<vViewBase*>::iterator it = m_Views.begin();
	vView2D* view = NULL;

	for (int i = 0; i < 16; i++)
	{
		view = dynamic_cast<vView2D*>(*it++);
		if (view)
		{
			view->UnInstallPipeLine();
			view->SetSliceIndex(start);
			view->InstallPipeLine(rootIndex);			
			this->UpdateCornerText(view);
		}
		++start;
	}

	int index[3] = {0,0,0};
	bool exist = m_Dm->IsPropertyExist(rootIndex,"position");

	if (!exist)
	{
		vtkSmartPointer<vtkImageData> data = 
			vtkImageData::SafeDownCast(m_Dm->GetData(rootIndex));
		if (data)
		{
			data->GetDimensions(index);
			int a[2] = {0,0};
			a[0] = index[0];
			a[1] = index[2];
			index[0] = a[1]/2;
			index[1] = index[1]/2;
			index[2] = a[0]/2;
		}
	} 
	else
	{
		std::string str = m_Dm->GetNodeProperty(rootIndex,"position");
		ConvertNFromString(index,str);
	}

	int a = 0;


	for (int j = 0; j < 3; j++)
	{
		view = dynamic_cast<vView2D*>(*it++);
	
		if (view)
		{
			view->UnInstallPipeLine();
			view->SetSliceIndex(index[a]);
			view->SetSliceOrientation(a);
			view->InstallPipeLine(rootIndex);
			view->AddOberserWheel();

			for (int i = 0; i < (int)child.size(); i++)
			{
				view->InstallPipeLine(child[i]);
			}

			this->UpdateCornerText(view);

			a++;
		}
	}

}

void vMultiView::UpdateImageData(int rootIndex)
{
	if (m_Layout != THREED)
	{
		std::list<vViewBase*>::iterator it = m_Views.begin();
		vView2D* view = NULL;
		while(it != m_Views.end())
		{
			view = dynamic_cast<vView2D*>(*it++);
			if (view)
			{
				view->UpdateImagePlaneProperty(rootIndex);
			}
		}
	}
}

void vMultiView::UpdateSurfaceData(std::list<int> &indexes)
{
	std::list<vViewBase*>::iterator it = --m_Views.end();
	if (m_Layout == PROJECT || m_Layout == THREED)
	{
		vView3D* view3d = dynamic_cast<vView3D*>(*it);
		if (view3d)
		{
			view3d->UpdateModelProperty(indexes);
		}
	}
}

void vMultiView::UpdateCornerText(vView2D* view)
{
	vtkSmartPointer<vInteractorStyle> style = 
		vInteractorStyle::SafeDownCast(view->GetInteractor()->GetInteractorStyle());
	vtkSmartPointer<vtkRenderWindow> renWin = view->GetRenderWindow();
	int *size,*position,cross[2];
	size = renWin->GetSize();
	position = renWin->GetPosition();
	cross[0] = position[0] + size[0]/2;
	cross[1] = position[1] + size[1]/2;
	style->OnMouseMoveDeal(cross[0],cross[1]);
}

void vMultiView::AddSurfaceData(std::list<int> &indexes)
{
	std::list<vViewBase*>::iterator it = --m_Views.end();
	std::list<int>::iterator it_n = indexes.begin();
	vView3D* view = dynamic_cast<vView3D*>(*it);

	int a = 0;
	switch(m_Layout)
	{
	case PROJECT:
	case THREED:
		if (view)
		{
			//view->UnInstallPipeLine();
			while(it_n != indexes.end())
			{
				if (a == m_LastModelIndexes.size())
				{				
					view->InstallPipeLine(*it_n);
					m_LastModelIndexes.push_back(*it_n);
				}					
				it_n++;
				a++;
			}

			view->ResetCamera();
			view->GetRenderWindow()->Render();
		}
	}
}


void vMultiView::Create2DView(int num)
{
	for (int i = 0; i < num; i++)
	{
		vView2D* view = new vView2D(this);
		view->SetDataManagement(m_Dm);

		vtkSmartPointer<vtkInteractorObserver> style = 
			view->GetRenderWindow()->GetInteractor()->GetInteractorStyle();

		vtkSmartPointer<vtkEventQtSlotConnect> connect = 
			vtkSmartPointer<vtkEventQtSlotConnect>::New();

		connect->Connect(style,
			vtkCommand::MouseWheelBackwardEvent,
			this,
			SLOT(OnMouseWheelEvent(vtkObject*,unsigned long,void*,void*,vtkCommand*)));
		connect->Connect(style,
			vtkCommand::MouseWheelForwardEvent,
			this,
			SLOT(OnMouseWheelEvent(vtkObject*,unsigned long,void*,void*,vtkCommand*)));

		m_Views.push_back(view);
		m_Connect.push_back(connect);

		this->connect(view,SIGNAL(focusChanged()),this,SLOT(FoucsChanged()));

		QWidget* widget = new QWidget(this);
		widget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);			
		widget->setContentsMargins(2,2,2,2);
		widget->setStyleSheet("background-color: black");
		view->SetContainer(widget);

		QHBoxLayout* layout = new QHBoxLayout(widget);
		layout->addWidget(view);
		layout->setMargin(0);

		widget->setLayout(layout);
		m_ViewContainer.push_back(widget);

		this->connect(view,SIGNAL(layoutOne()),this,SLOT(LayoutFull()));
	}
}

void vMultiView::OnMouseWheelEvent(vtkObject* caller, unsigned long vtk_event, void* client_data, void* call_data, vtkCommand*)
{
	switch (m_Layout)
	{
	case ONE_2D:
		this->OnMouseWheelDeal(1,vtk_event);
		break;
	case TWO_2D:
		this->OnMouseWheelDeal(2,vtk_event);	
		break;
	case THREE_2D:
		this->OnMouseWheelDeal(3,vtk_event);	
		break;
	case FOUR_2D:
		this->OnMouseWheelDeal(4,vtk_event);	
		break;
	case PROJECT:
		break;
	case THREED:
		break;
	}
}

void vMultiView::Create3DView()
{
	vView3D* view = new vView3D(this);
	view->SetDataManagement(m_Dm);
	m_Views.push_back(view);

	QWidget* widget = new QWidget(this);
	widget->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);			
	widget->setContentsMargins(2,2,2,2);
	
	widget->setStyleSheet("background-color: black");

	view->SetContainer(widget);

	QHBoxLayout* layout = new QHBoxLayout(widget);
	layout->addWidget(view);
	layout->setMargin(0);

	widget->setLayout(layout);

	m_ViewContainer.push_back(widget);

	this->connect(view,SIGNAL(focusChanged()),this,SLOT(FoucsChanged()));
	this->connect(view,SIGNAL(layoutOne()),this,SLOT(LayoutFull()));
}
