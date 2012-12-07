#include "vView2D.h"
#include "vtkRenderWindow.h"
#include "vInteractorStyle.h"
#include "vtkImageData.h"
#include "vtkProperty.h"
#include "vtkImageActor.h"
#include "vInteractorStyle.h"
#include "vtkTextProperty.h"
#include "vtkCamera.h"
#include "vtkWindowLevelLookupTable.h"

vtkSmartPointer<vtkAbstractWidget> vView2D::m_TempWidget = NULL;
vView2D::vView2D(QWidget* parent /* = 0 */):vViewBase(parent)
{
	m_CornerText = vtkSmartPointer<vtkCornerAnnotation>::New();
	m_CornerText->GetTextProperty()->SetColor(1,1,1);
	m_CornerText->GetTextProperty()->SetFontFamilyToTimes();

	m_Style = vtkSmartPointer<vInteractorStyle>::New();
	this->GetRenderWindow()->GetInteractor()->SetInteractorStyle(m_Style);
	m_Style->SetRender(m_Render);
	m_Style->SetCornerAnnotation(m_CornerText);
	m_Style->SetView(this);

	m_CallBack = vtkSmartPointer<vtkCallbackCommand>::New();
	m_CallBack->SetClientData(this);
	m_CallBack->SetCallback(func);

	m_Index = 0;
	m_Orientation = 0;
	this->m_Render->AddActor(m_CornerText);

	m_Plane = NULL;
	m_Mapper = NULL;

}

vView2D::~vView2D()
{
}

void vView2D::InstallPipeLine(int root_index)
{
	DataNodeType type1 = m_Dm->GetDataNodeType(root_index);	

	if ((root_index < 0) || (type1 != IMAGE_DATA))
	{
		return;
	}

	vtkSmartPointer<vtkImageData> tempImageData = 		vtkImageData::SafeDownCast(m_Dm->GetData(root_index));	if (tempImageData == NULL)
	{
		return;
	}	if (root_index == m_Dm->GetVisibleRootIndex())
	{
		vtkSmartPointer<vInteractorStyle> style = 
			vInteractorStyle::SafeDownCast(this->GetRenderWindow()->GetInteractor()->GetInteractorStyle());
		style->SetImageData(tempImageData);
	}	vtkSmartPointer<vtkImagePlaneWidget> plane = 
		vtkSmartPointer<vtkImagePlaneWidget>::New();
	plane->SetInput(tempImageData);
	plane->RestrictPlaneToVolumeOn();
	switch(m_Orientation)
	{
	case 0:
		plane->SetPlaneOrientationToZAxes();
		break;
	case 1:
		plane->SetPlaneOrientationToYAxes();
		break;
	case 2:
		plane->SetPlaneOrientationToXAxes();
		break;
	}	

	plane->SetSliceIndex(m_Index);

	vtkSmartPointer<vtkWindowLevelLookupTable> table 
		= vtkWindowLevelLookupTable::New();
	plane->SetLookupTable(table);
	plane->SetWindowLevel(400,0);

	m_Style->SetLookupTable(table);

	vtkSmartPointer<vtkImageMapToColors> mapToColor 
		= vtkSmartPointer<vtkImageMapToColors>::New();	
	mapToColor->SetInput(plane->GetResliceOutput());	
	mapToColor->SetLookupTable(plane->GetLookupTable());
	mapToColor->ReleaseDataFlagOn();

	vtkSmartPointer<vtkImageActor> imageActor = 
		vtkSmartPointer<vtkImageActor>::New();
	imageActor->SetInput(mapToColor->GetOutput());

	m_Render->AddActor(imageActor);
	m_Actor = imageActor;

	m_Plane = plane;
	m_Mapper = mapToColor;

	switch(m_Orientation)
	{
	case 0:
		imageActor->RotateZ(180);
		imageActor->RotateY(180);
		break;
	case 1:
		imageActor->RotateZ(90);
		imageActor->RotateX(180);
		break;
	case 2:
		break;
	}	

	this->ResetCamera();

	bool visible = m_Dm->GetNodeVisible(root_index);
	imageActor->SetVisibility(visible);

	this->GetRenderWindow()->Render();
}


void vView2D::func(vtkObject* vtkNotUsed(object), unsigned long eid, void* clientdata, void *calldata)
{
	vView2D* self = reinterpret_cast<vView2D*>(clientdata);
	switch (eid)
	{
	case vtkCommand::MouseWheelBackwardEvent:
		self->OnBackwardWheel();
		break;
	case vtkCommand::MouseWheelForwardEvent:
		self->OnForwardWheel();
		break;
	case vtkCommand::LeftButtonPressEvent:
		self->ChangeWidgetWindowToCurrent();
		break;
	default:
		break;
	}
}

void vView2D::OnCornerInformation(bool arg)
{
	if (arg)
	{
		m_CornerText->SetVisibility(false);
	}
	else
	{
		m_CornerText->SetVisibility(true);
	}
}

void vView2D::OnForwardWheel()
{
	this->OnFoucs();

	this->SetSliceIndex(this->GetSliceIndex() + 1);
	this->GetRenderWindow()->Render();
}

void vView2D::translateLanguage()
{
	vViewBase::translateLanguage();

	vtkSmartPointer<vInteractorStyle> style = 
		vInteractorStyle::SafeDownCast(this->GetInteractor()->GetInteractorStyle());
	vtkSmartPointer<vtkRenderWindow> renWin = this->GetRenderWindow();
	int *size,*position,cross[2];
	size = renWin->GetSize();
	position = renWin->GetPosition();
	cross[0] = position[0] + size[0]/2;
	cross[1] = position[1] + size[1]/2;
	style->OnMouseMoveDeal(cross[0],cross[1]);
}

void vView2D::OnBackwardWheel()
{
	this->OnFoucs();

	this->SetSliceIndex(this->GetSliceIndex() - 1);
	this->GetRenderWindow()->Render();
}

void vView2D::AddOberserWheel()
{
	vtkSmartPointer<vtkInteractorObserver> style = 
		this->GetRenderWindow()->GetInteractor()->GetInteractorStyle();

	style->RemoveObserver(m_CallBack);
	style->AddObserver(vtkCommand::MouseWheelBackwardEvent,m_CallBack);
	style->AddObserver(vtkCommand::MouseWheelForwardEvent,m_CallBack);
}

void vView2D::DeleteOberserWheel()
{
	vtkSmartPointer<vtkInteractorObserver> style = 
		this->GetRenderWindow()->GetInteractor()->GetInteractorStyle();

	style->RemoveObserver(m_CallBack);
}

void vView2D::SetSliceIndex(int index)
{
	if (m_Plane)
	{
		m_Plane->SetSliceIndex(index);
		m_Index = m_Plane->GetSliceIndex();
	}
	else
	{
		m_Index = index;
	}
}

void vView2D::SetSliceOrientation(int orientation)
{
	m_Orientation = orientation;
	if (m_Plane)
	{
		switch(m_Orientation)
		{
		case 0:
			m_Plane->SetPlaneOrientationToZAxes();
			break;
		case 1:
			m_Plane->SetPlaneOrientationToYAxes();
			break;
		case 2:
			m_Plane->SetPlaneOrientationToXAxes();
			break;
		}
	}
}

void vView2D::UpdateImagePlaneProperty(int rootIndex)
{
	if (rootIndex < 0 || m_Actor == NULL)
	{
		return;
	}

	bool visible = m_Dm->GetNodeVisible(rootIndex);

	m_Actor->SetVisibility(visible);

	if (!visible)
	{
		m_CornerText->SetText(0,"");
		m_CornerText->SetText(1,"");
		m_CornerText->SetText(2,"");
		m_CornerText->SetText(3,"");
	}
	else
	{
		vtkSmartPointer<vInteractorStyle> style = 
			vInteractorStyle::SafeDownCast(this->GetRenderWindow()->GetInteractor()->GetInteractorStyle());
		style->OnMouseMoveDeal(0,0);
	}

	double window, level;
	std::string str = m_Dm->GetNodeProperty(rootIndex,"window");
	ConvertFromString(window,str);
	str = m_Dm->GetNodeProperty(rootIndex,"level");
	ConvertFromString(level,str);

	vtkSmartPointer<vtkWindowLevelLookupTable> lut = 
		vtkWindowLevelLookupTable::SafeDownCast(m_Plane->GetLookupTable());
	lut->SetWindow(window);
	lut->SetLevel(level);
	this->GetRenderWindow()->Render();
}


void vView2D::SetWindowLevel(double window, double level)
{
	if (!m_Plane)
		return;

	vtkSmartPointer<vtkWindowLevelLookupTable> lut = 
		vtkWindowLevelLookupTable::SafeDownCast(m_Plane->GetLookupTable());
	if (lut)
	{
		lut->SetWindow(window);
		lut->SetLevel(level);
	}

	vtkSmartPointer<vInteractorStyle> style = 
		vInteractorStyle::SafeDownCast(this->GetRenderWindow()->GetInteractor()->GetInteractorStyle());
	if (style)
	{
		style->SetWindowLevel(window,level);
	}

	this->ResetCamera();
	this->GetRenderWindow()->Render();
}

void vView2D::AddWidgetOberser(vtkSmartPointer<vtkAbstractWidget> widget)
{
	this->GetInteractor()->AddObserver(vtkCommand::LeftButtonPressEvent,m_CallBack);
	m_TempWidget = widget;
}

void vView2D::ChangeWidgetWindowToCurrent()
{
	if (m_TempWidget)
	{
		m_TempWidget->SetCurrentRenderer(this->GetRender());
		m_TempWidget->SetInteractor(this->GetInteractor());
		m_TempWidget->On();
		this->GetInteractor()->InvokeEvent(vtkCommand::LeftButtonPressEvent);
		m_TempWidget = NULL;
	}
}


void vView2D::Render()
{
	this->GetRenderWindow()->Render();
}

void vView2D::HideImage()
{
	m_Actor->SetVisibility(false);
	this->GetRenderWindow()->Render();
}

void vView2D::ShowImage()
{
	m_Actor->SetVisibility(true);
	this->GetRenderWindow()->Render();
}

void vView2D::UnInstallPipeLine()
{
	vViewBase::UnInstallPipeLine();

	if (!m_Actor)
	{
		return;
	}

	vtkSmartPointer<vtkImageActor> actor = 
		vtkImageActor::SafeDownCast(m_Actor);
	actor->SetInput(NULL);
	m_Actor = NULL;

	m_Plane->SetInput(NULL);
	m_Plane = NULL;

	m_Mapper->SetLookupTable(NULL);
	m_Mapper = NULL;

	vtkSmartPointer<vInteractorStyle> style = 
		vInteractorStyle::SafeDownCast(this->GetRenderWindow()->GetInteractor()->GetInteractorStyle());
	style->SetImageData(NULL);

	m_CornerText->SetText(0,"");
	m_CornerText->SetText(1,"");
	m_CornerText->SetText(2,"");
	m_CornerText->SetText(3,"");
}