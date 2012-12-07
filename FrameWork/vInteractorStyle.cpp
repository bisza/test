#include "vInteractorStyle.h"
#include "vtkCommand.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkCamera.h"
#include "vtkDistanceWidget.h"
#include "vtkAngleWidget.h"
#include "vtkRenderWindow.h"
#include "vtkPointPicker.h"
#include "vView2D.h"

#include <iostream>
#include <sstream>
#include <QString>
#include <QObject>


vInteractorStyle::vInteractorStyle(void)
{
	this->m_StyleMode    = PAN;
	this->MotionFactor   = 10.0;
	m_LookupTable = NULL;

	m_StartPosition[0] = 0;
	m_StartPosition[0] = 1;

	m_WinLevel[0] = 400;
	m_WinLevel[1] = 0;
}


vInteractorStyle::~vInteractorStyle(void)
{

}

void vInteractorStyle::SetWindowLevel(double window, double level)
{
	m_WinLevel[0] = window;
	m_WinLevel[1] = level;

	this->OnMouseMoveDeal(0,0);
}

void vInteractorStyle::SetInteractionMode(InteractorStyleMode mode)
{
	m_StyleMode = mode;
}

void vInteractorStyle::SetLookupTable(vtkSmartPointer<vtkLookupTable> lut)
{
	m_LookupTable = vtkWindowLevelLookupTable::SafeDownCast(lut);
}

void vInteractorStyle::OnMouseMove() 
{ 
	int x = this->Interactor->GetEventPosition()[0];
	int y = this->Interactor->GetEventPosition()[1];

	switch (this->State) 
	{
	case VTKIS_PAN:
		this->FindPokedRenderer(x, y);
		this->Pan();
		this->InvokeEvent(vtkCommand::InteractionEvent, NULL);
		break;

	case VTKIS_DOLLY:
		this->FindPokedRenderer(x, y);
		this->Dolly();
		this->InvokeEvent(vtkCommand::InteractionEvent, NULL);
		break;

	case VTKIS_WINDOW_LEVEL:
		this->FindPokedRenderer(x, y);
		this->WindowLevel();
		this->InvokeEvent(vtkCommand::InteractionEvent, NULL);
		break;

	case VTKIS_RESLICE:
		this->FindPokedRenderer(x, y);
		this->Reslice();
		this->InvokeEvent(vtkCommand::InteractionEvent, NULL);
		break;
	default: 
		break;
	}

	this->OnMouseMoveDeal(x,y);

}

void vInteractorStyle::OnMouseMoveDeal(int x,int y)
{
	if (!m_Data)
	{
		m_CornerText->SetText(0,"");
		m_CornerText->SetText(1,"");
		m_CornerText->SetText(2,"");
		m_CornerText->SetText(3,"");
		return;
	}
	double spacing[3], orgin[3],position[3],scaleValue;
	int dim[3],tPoint[3],slice,total;

	m_Data->GetSpacing(spacing);
	m_Data->GetOrigin(orgin);
	m_Data->GetDimensions(dim);

	vtkSmartPointer<vtkPointPicker> picker = 
		vtkSmartPointer<vtkPointPicker>::New();
	picker->Pick(x,y,0,m_Render);
	picker->GetMapperPosition(position);

	vtkSmartPointer<vtkProp> actor = m_View->GetActor();
	if (actor == NULL)
	{
		return;
	}

	if (!actor->GetVisibility())
	{
		m_CornerText->SetText(2,"");
		m_CornerText->SetText(3,"");
		m_Render->GetRenderWindow()->Render();
		return;
	}
	;
	
	switch(m_View->GetSliceOrientation()) 
	{
	case 2:
		tPoint[0] = m_View->GetSliceIndex();
		tPoint[1] = (int)((position[0] - orgin[0])/spacing[0]);
		tPoint[2] = (int)((position[1] - orgin[2])/spacing[2]);
		slice = tPoint[0];
		total = dim[1] - 1;
		break;
	case 1:
		tPoint[0] = (int)((position[1] - orgin[0])/spacing[0]);
		tPoint[1] = m_View->GetSliceIndex();
		tPoint[2] = (int)((position[0] - orgin[2])/spacing[2]);
		slice = tPoint[1];
		total = dim[1] - 1;
		break;
	case 0:
		tPoint[0] = (int)((position[0] - orgin[0])/spacing[0]);
		tPoint[1] = (int)((position[1] - orgin[1])/spacing[1]);
		tPoint[2] = m_View->GetSliceIndex();
		slice = tPoint[2];
		total = dim[2] - 1;
		break;
	}

    scaleValue = m_Data->GetScalarComponentAsDouble(tPoint[0],tPoint[1],tPoint[2],0);

	std::ostringstream text2Stream;
	std::ostringstream text3Stream;

    QString qstr = QObject::tr("当前位置：");
    text2Stream <<qstr.toStdString().c_str()<< tPoint[0]<<","<< tPoint[1]<<","<< tPoint[2]<<"\n";
    qstr = QObject::tr("CT值：");
    text2Stream <<qstr.toStdString().c_str()<<scaleValue;
	m_CornerText->SetText(2,text2Stream.str().c_str()); 

    qstr = QObject::tr("窗宽/窗位：");
    QString qstr1 = QObject::tr("当前层/层数：");
    text3Stream <<qstr.toStdString().c_str()<<m_WinLevel[0]<<"/"<<m_WinLevel[1]<<"   "<<"\n" \
        <<qstr1.toStdString().c_str()<<slice<<"/"<< total;
	m_CornerText->SetText(3,text3Stream.str().c_str()); text2Stream;

	m_Render->GetRenderWindow()->Render();
}

void vInteractorStyle::OnLeftButtonDown() 
{ 
	if (!m_View->isFoucs())
	{
		m_View->SetFoucs();
	}

	switch (this->m_StyleMode)
	{
	case _RESLICE:
		this->StartReslice();
		break;
	case _WINDOW_LEVEL:
		this->StartWindowLevel();
		break;	
	case PAN:
		this->StartPan();
		break;
	case DOLLY:
		this->StartDolly();
		break;
	}
}

void vInteractorStyle::OnLeftButtonUp()
{
	switch (this->State) 
	{
	case VTKIS_WINDOW_LEVEL:
		this->EndWindowLevel();
		break;
	case VTKIS_RESLICE:
		this->EndReslice();
		break;
	case VTKIS_PAN:
		this->EndPan();
		break;
	case VTKIS_DOLLY:
		this->EndDolly();
		break;
	}
}

void vInteractorStyle::StartReslice()
{
	if (this->State != VTKIS_NONE) 
	{
		return;
	}
	this->StartState(VTKIS_RESLICE);
	this->InvokeEvent(vtkCommand::StartPickEvent, this);

	int x = this->Interactor->GetEventPosition()[0];
	int y = this->Interactor->GetEventPosition()[1];
	this->m_StartPosition[0] = x;
	this->m_StartPosition[1] = y;  
}

void vInteractorStyle::EndReslice()
{
	if (this->State != VTKIS_RESLICE) 
	{
		return;
	}
	this->InvokeEvent(vtkCommand::EndPickEvent, this);
	this->StopState();
}

void vInteractorStyle::StartWindowLevel()
{
	if (this->State != VTKIS_NONE) 
	{
		return;
	}
	this->StartState(VTKIS_WINDOW_LEVEL);
	this->InvokeEvent(vtkCommand::StartPickEvent, this);

	int x = this->Interactor->GetEventPosition()[0];
	int y = this->Interactor->GetEventPosition()[1];
	this->m_StartPosition[0] = x;
	this->m_StartPosition[1] = y;  
}

void vInteractorStyle::EndWindowLevel()
{
	if (this->State != VTKIS_WINDOW_LEVEL) 
	{
		return;
	}
	this->InvokeEvent(vtkCommand::EndPickEvent, this);
	this->StopState();
}

void vInteractorStyle::WindowLevel()
{
	if (m_LookupTable == NULL)
	{
		return;
	}

	vtkRenderWindowInteractor *rwi = this->Interactor;

	double x = rwi->GetEventPosition()[0];
	double y = rwi->GetEventPosition()[1];

	int *size = rwi->GetRenderWindow()->GetSize();
	double window = m_WinLevel[0];
	double level  = m_WinLevel[1];

	// Compute normalized delta

	double dx = 1.0 * ( x - this->m_StartPosition[0] ) / size[0];
	double dy = 1.0 *( this->m_StartPosition[1] - y ) / size[1];

	// Scale by current values

	if ( fabs( window ) > 10 )
	{
		dx = dx * window;
	}
	else
	{
		dx = dx * ( window < 0 ? -10 : 10 );
	}
	if ( fabs( level ) > 10 )
	{
		dy = dy * level;
	}
	else
	{
		dy = dy * ( level < 0 ? -10 : 10 );
	}

	// Abs so that direction does not flip

	if ( window < 0.0 )
	{
		dx = -1 * dx;
	}
	if ( level < 0.0 )
	{
		dy = -1 * dy;
	}

	// Compute new window level

	double newWindow = dx + window;
	double newLevel = level - dy;

	if ( fabs( newWindow ) < 0.01 )
	{
		newWindow = 0.01 * ( newWindow < 0 ? -1 : 1 );
	}
	if ( fabs( newLevel ) < 0.01 )
	{
		newLevel = 0.01 * ( newLevel < 0 ? -1 : 1 );
	}

	if (newWindow < 0)
	{
		newWindow = 0;
	}
	if (newWindow >3000 )
	{
		newWindow = 3000;
	}
	if (newLevel < -3000)
	{
		newLevel = -3000;
	}
	if (newLevel > 3000)
	{
		newLevel = 3000;
	}

	this->m_StartPosition[0] = rwi->GetEventPosition()[0];
	this->m_StartPosition[1] = rwi->GetEventPosition()[1];

	m_WinLevel[0] = newWindow;
	m_WinLevel[1] = newLevel;

	m_LookupTable->SetWindow(newWindow);
	m_LookupTable->SetLevel(newLevel);

	this->Interactor->GetRenderWindow()->Render();
}

void vInteractorStyle::Reslice()
{
	vtkRenderWindowInteractor *rwi = this->Interactor;

	double ResliceCurrentPosition[2];
	ResliceCurrentPosition[0] = rwi->GetEventPosition()[0];
	ResliceCurrentPosition[1] = rwi->GetEventPosition()[1];

	int displacement = (ResliceCurrentPosition[1] - m_StartPosition[1])/10;
	m_View->SetSliceIndex(m_View->GetSliceIndex() + displacement);

	this->Interactor->GetRenderWindow()->Render();
}

void vInteractorStyle::Pan()
{
	if (this->CurrentRenderer == NULL)
	{
		return;
	}

	vtkRenderWindowInteractor *rwi = this->Interactor;

	double viewFocus[4], focalDepth, viewPoint[3];
	double newPickPoint[4], oldPickPoint[4], motionVector[3];

	// Calculate the focal depth since we'll be using it a lot

	vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();
	camera->GetFocalPoint(viewFocus);
	this->ComputeWorldToDisplay(viewFocus[0], viewFocus[1], viewFocus[2], 
		viewFocus);
	focalDepth = viewFocus[2];

	this->ComputeDisplayToWorld(rwi->GetEventPosition()[0], 
		rwi->GetEventPosition()[1],
		focalDepth, 
		newPickPoint);

	// Has to recalc old mouse point since the viewport has moved,
	// so can't move it outside the loop

	this->ComputeDisplayToWorld(rwi->GetLastEventPosition()[0],
		rwi->GetLastEventPosition()[1],
		focalDepth, 
		oldPickPoint);

	// Camera motion is reversed

	motionVector[0] = oldPickPoint[0] - newPickPoint[0];
	motionVector[1] = oldPickPoint[1] - newPickPoint[1];
	motionVector[2] = oldPickPoint[2] - newPickPoint[2];

	camera->GetFocalPoint(viewFocus);
	camera->GetPosition(viewPoint);
	camera->SetFocalPoint(motionVector[0] + viewFocus[0],
		motionVector[1] + viewFocus[1],
		motionVector[2] + viewFocus[2]);

	camera->SetPosition(motionVector[0] + viewPoint[0],
		motionVector[1] + viewPoint[1],
		motionVector[2] + viewPoint[2]);

	if (rwi->GetLightFollowCamera()) 
	{
		this->CurrentRenderer->UpdateLightsGeometryToFollowCamera();
	}

	rwi->Render();
}

void vInteractorStyle::Dolly()
{
	if (this->CurrentRenderer == NULL)
	{
		return;
	}

	vtkRenderWindowInteractor *rwi = this->Interactor;
	double *center = this->CurrentRenderer->GetCenter();
	int dy = rwi->GetEventPosition()[1] - rwi->GetLastEventPosition()[1];
	double dyf = this->MotionFactor * dy / center[1];
	this->Dolly(pow(1.1, dyf));
}

void vInteractorStyle::Dolly(double factor)
{
	if (this->CurrentRenderer == NULL)
	{
		return;
	}

	vtkCamera *camera = this->CurrentRenderer->GetActiveCamera();
	if (camera->GetParallelProjection())
	{
		camera->SetParallelScale(camera->GetParallelScale() / factor);
	}
	else
	{
		camera->Dolly(factor);
		if (this->AutoAdjustCameraClippingRange)
		{
			this->CurrentRenderer->ResetCameraClippingRange();
		}
	}

	if (this->Interactor->GetLightFollowCamera()) 
	{
		this->CurrentRenderer->UpdateLightsGeometryToFollowCamera();
	}

	this->Interactor->Render();
}
