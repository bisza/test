
#include "vInteractorStyleRubberBandPick.h"

#include "vtkObjectFactory.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkUnsignedCharArray.h"
#include "vtkPolyData.h"
#include "vtkAbstractPropPicker.h"
#include "vtkAssemblyPath.h"
#include "vtkAreaPicker.h"
#include "vtkMapper.h"
#include "vtkProperty.h"
#include "vtkCommand.h"
#include "yDataManagement.h"
#include "vtkPolyDataMapper.h"
#include "vtkOutlineSource.h"
#include "vtkCellPicker.h"
#include "vtkSphereSource.h"
#include "vtkSeedWidget.h"
#include "vtkSeedRepresentation.h"
#include "vtkPointHandleRepresentation3D.h"
#include "vtkSeedRepresentation.h"
#include "vtkProperty.h"
#include "vtkCellPicker.h"
#include "vViewBase.h"

vtkStandardNewMacro(vInteractorStyleRubberBandPick);

#define VTKISRBP_ORIENT 0
#define VTKISRBP_SELECT 1

//--------------------------------------------------------------------------
vInteractorStyleRubberBandPick::vInteractorStyleRubberBandPick()
{
    this->CurrentMode = VTKISRBP_ORIENT;
    this->StartPosition[0] = this->StartPosition[1] = 0;
    this->EndPosition[0] = this->EndPosition[1] = 0;
    this->Moving = 0;
    this->PixelArray = vtkUnsignedCharArray::New();
}

//--------------------------------------------------------------------------
vInteractorStyleRubberBandPick::~vInteractorStyleRubberBandPick()
{
    this->PixelArray->Delete();
}

//--------------------------------------------------------------------------
void vInteractorStyleRubberBandPick::StartSelect()
{
    this->CurrentMode = VTKISRBP_SELECT;
}

//--------------------------------------------------------------------------
void vInteractorStyleRubberBandPick::OnChar()
{
	switch (this->Interactor->GetKeyCode())
	{
	case 'r':
	case 'R':
		//r toggles the rubber band selection mode for mouse button 1
		if (this->CurrentMode == VTKISRBP_ORIENT)
		{
			this->CurrentMode = VTKISRBP_SELECT;
		}
		else
		{ 
			this->CurrentMode = VTKISRBP_ORIENT;
		}
		break;
	case 'p' :
	case 'P' :
		{
			vtkRenderWindowInteractor *rwi = this->Interactor;
			int *eventPos = rwi->GetEventPosition();
			this->FindPokedRenderer(eventPos[0], eventPos[1]);
			this->StartPosition[0] = eventPos[0];
			this->StartPosition[1] = eventPos[1];
			this->EndPosition[0] = eventPos[0];
			this->EndPosition[1] = eventPos[1];
			this->Pick();
			break;
		}
	default:
		this->Superclass::OnChar();
	}
}

//--------------------------------------------------------------------------
void vInteractorStyleRubberBandPick::OnTimer()
{

}

//--------------------------------------------------------------------------
void vInteractorStyleRubberBandPick::OnLeftButtonDown()
{
	if (!m_View->isFoucs())
	{	
		m_View->SetFoucs();
	}

	vtkRenderWindow *renWin = this->Interactor->GetRenderWindow();
	this->StartPosition[0] = this->Interactor->GetEventPosition()[0];
	this->StartPosition[1] = this->Interactor->GetEventPosition()[1];
	this->EndPosition[0] = this->StartPosition[0];
	this->EndPosition[1] = this->StartPosition[1];

	this->FindPokedRenderer(this->StartPosition[0], this->StartPosition[1]);

	switch(CurrentMode)
	{
	case VTKISRBP_ORIENT:
		this->Superclass::OnLeftButtonDown();
		break;
	case VTKISRBP_SELECT:
		if (!this->Interactor)
		    {
		    return;
		    }
		 
		//otherwise record the rubber band starting coordinate
		 
		this->Moving = 1;

		this->PixelArray->Initialize();
		this->PixelArray->SetNumberOfComponents(4);
		int *size = renWin->GetSize();
		this->PixelArray->SetNumberOfTuples(size[0]*size[1]);
		   
		renWin->GetRGBACharPixelData(0, 0, size[0]-1, size[1]-1, 1, this->PixelArray);

		break;
	}
}

//--------------------------------------------------------------------------
void vInteractorStyleRubberBandPick::OnMouseMove()
{
	this->EndPosition[0] = this->Interactor->GetEventPosition()[0];
	this->EndPosition[1] = this->Interactor->GetEventPosition()[1];  

	switch (CurrentMode)
	{
	case VTKISRBP_ORIENT:
		this->Superclass::OnMouseMove();
		break;
	case VTKISRBP_SELECT:
		if (!this->Moving)
		{
			return;
		}

		int *size = this->Interactor->GetRenderWindow()->GetSize();  
		if (this->EndPosition[0] > (size[0]-1))
		{
			this->EndPosition[0] = size[0]-1;
		}
		if (this->EndPosition[0] < 0)
		{
			this->EndPosition[0] = 0;
		}
		if (this->EndPosition[1] > (size[1]-1))
		{
			this->EndPosition[1] = size[1]-1;
		}
		if (this->EndPosition[1] < 0)
		{
			this->EndPosition[1] = 0;
		}
		this->RedrawRubberBand();
		break;
	}
	if (this->State != VTKIS_PAN && this->State != VTKIS_ROTATE && this->State != VTKIS_DOLLY && this->State != VTKIS_SPIN )
	{
	}
}

//--------------------------------------------------------------------------
void vInteractorStyleRubberBandPick::OnLeftButtonUp()
{
	switch (CurrentMode)
	{
	case VTKISRBP_ORIENT:
		this->Superclass::OnLeftButtonUp();

		if (   (this->StartPosition[0] == this->EndPosition[0])
			|| (this->StartPosition[1] == this->EndPosition[1]) )
		{

		}

		break;
	case VTKISRBP_SELECT:
		if (!this->Interactor || !this->Moving)
		{
			return;
		}

		//otherwise record the rubber band end coordinate and then fire off a pick
 		if (   (this->StartPosition[0] != this->EndPosition[0])
 			|| (this->StartPosition[1] != this->EndPosition[1]) )
 		{
			this->Pick();
		}
		this->Moving = 0;
		break;
	}
}

//--------------------------------------------------------------------------
void vInteractorStyleRubberBandPick::OnMouseWheelBackward()
{
	if (!m_View->isFoucs())
	{
		m_View->SetFoucs();
	}

	Superclass::OnMouseWheelBackward();
}

//--------------------------------------------------------------------------
void vInteractorStyleRubberBandPick::OnMiddleButtonDown()
{
	if (!m_View->isFoucs())
	{
		m_View->SetFoucs();
	}

	Superclass::OnMiddleButtonDown();
}

//--------------------------------------------------------------------------
void vInteractorStyleRubberBandPick::OnMiddleButtonUp()
{
	Superclass::OnMiddleButtonUp();
}

//--------------------------------------------------------------------------
void vInteractorStyleRubberBandPick::OnMouseWheelForward()
{
	Superclass::OnMouseWheelForward();
}

//--------------------------------------------------------------------------
void vInteractorStyleRubberBandPick::OnRightButtonDown()
{
	Superclass::OnRightButtonDown();
}

//--------------------------------------------------------------------------
void vInteractorStyleRubberBandPick::OnRightButtonUp()
{	
	Superclass::OnRightButtonUp();
}

//--------------------------------------------------------------------------
void vInteractorStyleRubberBandPick::RedrawRubberBand()
{
  //update the rubber band on the screen
  int *size = this->Interactor->GetRenderWindow()->GetSize();  

  vtkUnsignedCharArray *tmpPixelArray = vtkUnsignedCharArray::New();
  tmpPixelArray->DeepCopy(this->PixelArray);  
  unsigned char *pixels = tmpPixelArray->GetPointer(0);

  int min[2], max[2];

  min[0] = this->StartPosition[0] <= this->EndPosition[0] ?
    this->StartPosition[0] : this->EndPosition[0];
  if (min[0] < 0) { min[0] = 0; }
  if (min[0] >= size[0]) { min[0] = size[0] - 1; }

  min[1] = this->StartPosition[1] <= this->EndPosition[1] ?
    this->StartPosition[1] : this->EndPosition[1];
  if (min[1] < 0) { min[1] = 0; }
  if (min[1] >= size[1]) { min[1] = size[1] - 1; }

  max[0] = this->EndPosition[0] > this->StartPosition[0] ?
    this->EndPosition[0] : this->StartPosition[0];
  if (max[0] < 0) { max[0] = 0; }
  if (max[0] >= size[0]) { max[0] = size[0] - 1; }

  max[1] = this->EndPosition[1] > this->StartPosition[1] ?
    this->EndPosition[1] : this->StartPosition[1];
  if (max[1] < 0) { max[1] = 0; }
  if (max[1] >= size[1]) { max[1] = size[1] - 1; }

  int i;
  for (i = min[0]; i <= max[0]; i++)
    {
    pixels[4*(min[1]*size[0]+i)] = 255 ^ pixels[4*(min[1]*size[0]+i)];
    pixels[4*(min[1]*size[0]+i)+1] = 255 ^ pixels[4*(min[1]*size[0]+i)+1];
    pixels[4*(min[1]*size[0]+i)+2] = 255 ^ pixels[4*(min[1]*size[0]+i)+2];
    pixels[4*(max[1]*size[0]+i)] = 255 ^ pixels[4*(max[1]*size[0]+i)];
    pixels[4*(max[1]*size[0]+i)+1] = 255 ^ pixels[4*(max[1]*size[0]+i)+1];
    pixels[4*(max[1]*size[0]+i)+2] = 255 ^ pixels[4*(max[1]*size[0]+i)+2];
    }
  for (i = min[1]+1; i < max[1]; i++)
    {
    pixels[4*(i*size[0]+min[0])] = 255 ^ pixels[4*(i*size[0]+min[0])];
    pixels[4*(i*size[0]+min[0])+1] = 255 ^ pixels[4*(i*size[0]+min[0])+1];
    pixels[4*(i*size[0]+min[0])+2] = 255 ^ pixels[4*(i*size[0]+min[0])+2];
    pixels[4*(i*size[0]+max[0])] = 255 ^ pixels[4*(i*size[0]+max[0])];
    pixels[4*(i*size[0]+max[0])+1] = 255 ^ pixels[4*(i*size[0]+max[0])+1];
    pixels[4*(i*size[0]+max[0])+2] = 255 ^ pixels[4*(i*size[0]+max[0])+2];
    }
  
  this->Interactor->GetRenderWindow()->SetRGBACharPixelData(0, 0, size[0]-1, size[1]-1, pixels, 0);
  this->Interactor->GetRenderWindow()->Frame();
  
  tmpPixelArray->Delete();
}

//--------------------------------------------------------------------------
void vInteractorStyleRubberBandPick::Pick()
{
  //find rubber band lower left, upper right and center  
  double rbcenter[3];
  int *size = this->Interactor->GetRenderWindow()->GetSize();  
  int min[2], max[2];
  min[0] = this->StartPosition[0] <= this->EndPosition[0] ?
    this->StartPosition[0] : this->EndPosition[0];
  if (min[0] < 0) { min[0] = 0; }
  if (min[0] >= size[0]) { min[0] = size[0] - 2; }

  min[1] = this->StartPosition[1] <= this->EndPosition[1] ?
    this->StartPosition[1] : this->EndPosition[1];
  if (min[1] < 0) { min[1] = 0; }
  if (min[1] >= size[1]) { min[1] = size[1] - 2; }

  max[0] = this->EndPosition[0] > this->StartPosition[0] ?
    this->EndPosition[0] : this->StartPosition[0];
  if (max[0] < 0) { max[0] = 0; }
  if (max[0] >= size[0]) { max[0] = size[0] - 2; }

  max[1] = this->EndPosition[1] > this->StartPosition[1] ?
    this->EndPosition[1] : this->StartPosition[1];
  if (max[1] < 0) { max[1] = 0; }
  if (max[1] >= size[1]) { max[1] = size[1] - 2; }

  rbcenter[0] = (min[0] + max[0])/2.0;
  rbcenter[1] = (min[1] + max[1])/2.0;
  rbcenter[2] = 0;
  
  if (this->State == VTKIS_NONE) 
    {
    //tell the RenderWindowInteractor's picker to make it happen
    vtkRenderWindowInteractor *rwi = this->Interactor;

    vtkAssemblyPath *path = NULL;
    rwi->StartPickCallback();
    vtkAbstractPropPicker *picker = 
      vtkAbstractPropPicker::SafeDownCast(rwi->GetPicker());
    if ( picker != NULL )
      {
      vtkAreaPicker *areaPicker = vtkAreaPicker::SafeDownCast(picker);
      if (areaPicker != NULL)
        {
        areaPicker->AreaPick(min[0], min[1], max[0], max[1], 
                             this->CurrentRenderer);
        }
      else
        {
        picker->Pick(rbcenter[0], rbcenter[1], 
                     0.0, this->CurrentRenderer);
        }
      path = picker->GetPath();
      }
	if ( path == NULL )
	{
		//this->HighlightProp(NULL);
		this->PropPicked = 0;
	}
    else
      {
      //highlight the one prop that the picker saved in the path
      vtkActor* actor = vtkActor::SafeDownCast(path->GetFirstNode()->GetViewProp());
	  if (CurrentMode == VTKISRBP_ORIENT)
	  {
		  if (actor)
		  {
			  vtkMapper* mapper = actor->GetMapper();
			  if (mapper)
			  {
				  vtkDataSet*  data     = mapper->GetInput();
				  vtkPolyData* polyData = vtkPolyData::SafeDownCast(data);

				  if (polyData)
				  {
				  }
			  }
		  }
	  }
      this->PropPicked = 1;
      }
    rwi->EndPickCallback();
    }
  
  this->Interactor->Render();
}

//--------------------------------------------------------------------------
void vInteractorStyleRubberBandPick::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

void vInteractorStyleRubberBandPick::HighlightProp3D(vtkProp3D *prop3D) 
{
	//no prop picked now 
	if ( ! prop3D) 
	{
		//was there previously?
		if (this->PickedRenderer != NULL && this->OutlineActor) 
		{
			this->PickedRenderer->RemoveActor(this->OutlineActor);
			this->PickedRenderer = NULL;
		}
	}
	//prop picked now 
	else
	{
		if ( ! this->OutlineActor ) 
		{
			// have to defer creation to get right type
			this->OutlineActor = vtkActor::New();
			this->OutlineActor->PickableOff();
			this->OutlineActor->DragableOff();
			OutlineMapper->ScalarVisibilityOff();
			this->OutlineActor->SetMapper(OutlineMapper);
			this->OutlineActor->GetProperty()->SetAmbient(1.0);
			this->OutlineActor->GetProperty()->SetDiffuse(0.0);
		}

		if (PickedRenderer)
		{
			if (!PickedRenderer->HasViewProp(OutlineActor))
			{
				PickedRenderer->AddActor(OutlineActor);
			}
			this->Outline->SetBounds(prop3D->GetBounds());
		}
	}
}

