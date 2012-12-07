#include "vModelLabel.h"
#include "vtkViewport.h"
#include "vtkProperty2D.h"
#include "sstream"
#include "vtkTextMapper.h"
#include "vtkTextProperty.h"
#include "vtkRenderWindowInteractor.h"
#include "yDataStorageConfigure.h"

vModelLabel::vModelLabel(void)
	:m_Renderer(NULL),
	m_Number(0)
{
	m_Callback = vtkSmartPointer<vtkCallbackCommand>::New();
	m_Callback->SetCallback(vModelLabel::ProcessEvent);
	m_Callback->SetClientData(this);

	m_Mapper = vtkSmartPointer<vtkTextMapper>::New();
	m_Mapper->GetTextProperty()->SetFontFamilyToTimes();
	m_Mapper->GetTextProperty()->SetFontSize(12);	

	m_Actor = vtkSmartPointer<vtkActor2D>::New();
	m_Actor->SetMapper(m_Mapper);
}


vModelLabel::~vModelLabel(void)
{
	m_Renderer->GetRenderWindow()->GetInteractor()->RemoveObservers(vtkCommand::RenderEvent,m_Callback);
}

void vModelLabel::SetLabel(std::string label)
{
	m_Label = label;
}

void vModelLabel::SetColor(std::string color)
{
	ConvertNFromString(m_Color,color);
}

void vModelLabel::SetRenderer(vtkSmartPointer<vtkRenderer> renderer)
{
	if (m_Renderer == renderer)
	{
		return;	
	}

	if (m_Renderer)
	{
		m_Renderer->GetRenderWindow()->GetInteractor()->RemoveObservers(vtkCommand::RenderEvent,m_Callback);
	}
	
	m_Renderer = renderer;
	m_Renderer->AddActor(m_Actor);
	m_Renderer->GetRenderWindow()->GetInteractor()->AddObserver(vtkCommand::RenderEvent,m_Callback);
};

void vModelLabel::Update()
{
	if (!m_Renderer)
	{
		return;
	}
	if (!m_Renderer->GetRenderWindow())
	{
		return;
	}
	//Update Label
	m_Mapper->SetInput((m_Label.c_str()));
	m_Mapper->GetTextProperty()->SetColor(m_Color);

	//Update position
	int *size = m_Renderer->GetRenderWindow()->GetSize();
	m_Actor->SetDisplayPosition(5,size[1]-15*(m_Number+1));
	m_Actor->SetVisibility(true);
}

void vModelLabel::ProcessEvent(vtkObject* vtkNotUsed(object),unsigned long event,void* clientdata,void* vtkNotUsed(calldata))
{
	vModelLabel* self = reinterpret_cast<vModelLabel*>(clientdata);
	switch (event)
	{
	case vtkCommand::RenderEvent:          
		self->Update();
		break;
	default:
		break;
	}
}

void vModelLabel::DeleteLabel()
{
	m_Renderer->RemoveActor(m_Actor);
}
