#include "vView3D.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkProperty.h"
#include "vtkRenderWindow.h"
#include "vtkFeatureEdges.h"
#include "vtkMassProperties.h"
#include "vtkFillHolesFilter.h"
#include "vtkAssembly.h"
#include "vtkProp3DCollection.h"
#include "yDataStorageConfigure.h"
#include "vInteractorStyleRubberBandPick.h"
#include <QMessageBox>


vView3D::vView3D(QWidget* parent):vViewBase(parent)
{
	isTextShow = false;

	vtkSmartPointer<vInteractorStyleRubberBandPick> style = 
		vtkSmartPointer<vInteractorStyleRubberBandPick>::New();
	this->GetRenderWindow()->GetInteractor()->SetInteractorStyle(style);
	style->SetView(this);

	m_Label = NULL;
}

vView3D::~vView3D()
{

}

void vView3D::translateLanguage()
{
	vViewBase::translateLanguage();

	int node = m_Dm->GetSelectNodeIndex();
	this->UpdateModelText(node);
}

void vView3D::InstallPipeLine(int index)
{
	if (index < 0)
		return;

	vtkSmartPointer<vtkPolyData> polyData = 
		vtkPolyData::SafeDownCast(m_Dm->GetData(index));
	if (polyData == NULL)
		return;

	double rgb[3] = {0.0,0.0,0.0};
	bool visible = false;
	double opacity = 0.0;
	std::string str;

	str = m_Dm->GetNodeProperty(index,"color");
	ConvertNFromString(rgb,str);
	str = m_Dm->GetNodeProperty(index,"visible");
	ConvertFromString(visible,str);
	str = m_Dm->GetNodeProperty(index,"opacity");
	ConvertFromString(opacity,str);

	vtkSmartPointer<vtkPolyDataMapper> mapper = 
		vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInput(polyData);
	mapper->ScalarVisibilityOff();
	mapper->ReleaseDataFlagOn();

	vtkSmartPointer<vtkActor> actor = 
		vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);
	actor->GetProperty()->SetColor(rgb);
	actor->GetProperty()->SetOpacity(opacity);
	actor->GetProperty()->SetDiffuse(0.1);
	actor->GetProperty()->SetInterpolationToPhong();
	actor->GetProperty()->SetSpecular(1.0);
	actor->SetVisibility(visible);

	m_Actors.push_back(actor);
	m_Render->AddActor(actor);
}

void vView3D::UnInstallPipeLine()
{
	vViewBase::UnInstallPipeLine();
	std::list<vtkSmartPointer<vtkActor>>::iterator it = m_Actors.begin();
	while(it != m_Actors.end())
	{
		m_Render->RemoveActor(*it++);
	}
	m_Render->Render();
	m_Actors.clear();

	if (m_Label)
	{
		m_Label->DeleteLabel();
		this->GetRenderWindow()->GetInteractor()->Render();
	}
}

void vView3D::UpdateModelProperty(std::list<int> &indexes)
{
	if (indexes.size() == m_Actors.size())
	{
		std::list<int>::iterator itIndex = indexes.begin();
		std::list<vtkSmartPointer<vtkActor>>::iterator itActor = m_Actors.begin();
		vtkSmartPointer<vtkActor> actor = NULL;
		int index = -1;

		while(itIndex != indexes.end())
		{
			actor = *itActor++;
			index = *itIndex++;

			double rgb[3] = {0.0,0.0,0.0};
			bool visible = false;
			double opacity = 0.0;
			std::string str;

			str = m_Dm->GetNodeProperty(index,"color");
			ConvertNFromString(rgb,str);
			str = m_Dm->GetNodeProperty(index,"visible");
			ConvertFromString(visible,str);
			str = m_Dm->GetNodeProperty(index,"opacity");
			ConvertFromString(opacity,str);

			actor->GetProperty()->SetColor(rgb);
			actor->GetProperty()->SetOpacity(opacity);
			actor->SetVisibility(visible);
		}
	}

	int node = m_Dm->GetSelectNodeIndex();
	this->UpdateModelText(node);
}

void vView3D::UpdateModelProperty(int node)
{
	std::list<int> indexes = m_Dm->GetNodesFromType(SURFACE_DATA);
	if (indexes.size() == m_Actors.size())
	{
		std::list<int>::iterator itIndex = indexes.begin();
		std::list<vtkSmartPointer<vtkActor>>::iterator itActor = m_Actors.begin();
		vtkSmartPointer<vtkActor> actor = NULL;
		int index = -1;

		for (int i = 0; i < (int)indexes.size(); i++)
		{
			actor = *itActor++;
			index = *itIndex++;

			if(node == index)
			{
				double rgb[3] = {0.0,0.0,0.0};
				bool visible = false;
				double opacity = 0.0;
				std::string str;

				str = m_Dm->GetNodeProperty(index,"color");
				ConvertNFromString(rgb,str);
				str = m_Dm->GetNodeProperty(index,"visible");
				ConvertFromString(visible,str);
				str = m_Dm->GetNodeProperty(index,"opacity");
				ConvertFromString(opacity,str);

				actor->GetProperty()->SetColor(rgb);
				actor->GetProperty()->SetOpacity(opacity);
				actor->SetVisibility(visible);
				this->GetRenderWindow()->Render();
				break;
			}
		}
	}
}

void vView3D::SetTextVisibility(bool visible)
{
	isTextShow = visible;
	std::list<int> nodes = m_Dm->GetNodesFromType(SURFACE_DATA);
	this->UpdateModelProperty(nodes);
}

void vView3D::UpdateModelText(int index)
{
	if (m_Label)
	{
		m_Label->DeleteLabel();
		m_Label = NULL;
	}

	if (isTextShow && m_Dm->GetNumOfNodes() != 0)
	{
		vtkSmartPointer<vModelLabel> label = 
			vtkSmartPointer<vModelLabel>::New();
		label->SetColor("1,1,1");
		label->SetNumber(0);
		label->SetRenderer(m_Render);			
		
		std::string labelText;
		m_Label = label;

		std::string str = m_Dm->GetNodeProperty(index,"selected");
		int select = 0;
		ConvertFromString(select,str);
	
		if (m_Dm->GetDataNodeType(index) == IMAGE_DATA)
		{
			QString str2(tr("体数据无体积信息！"));		
			labelText= str2.toStdString();
		}
		else 
		{
			bool exist = m_Dm->IsPropertyExist(index,"volume");	
			std::string str;
		
			if (exist)
			{
				str = m_Dm->GetNodeProperty(index,"volume");
			}
			else
			{
				double volume = 0;
				this->CalculateVolume(index,volume);
				str = ConvertToString(volume);
		
				m_Dm->SetNodeProperty(index,"volume",str,"true",DOUBLE_);
			}

			if (m_Dm->GetNodeVisible(index))
			{
				QString str2(tr("数据:"));
				QString str1(tr("体积:"));

				labelText = str2.toStdString() + m_Dm->GetNodeProperty(index,"name") + " "+ str1.toStdString() + str; 
			}
			else
			{
				QString str(tr("该数据不可见！"));
				labelText = str.toStdString();
			}
		}
		m_Label->SetLabel(labelText);
	}

	this->GetRenderWindow()->GetInteractor()->Render();
	this->GetRenderWindow()->Render();
}


void vView3D::CalculateVolume(int index,double &volume)
{
	vtkSmartPointer<vtkPolyData> data = 
		vtkPolyData::SafeDownCast(m_Dm->GetData(index));

	vtkSmartPointer<vtkFeatureEdges> featureEdges = 
		vtkSmartPointer<vtkFeatureEdges>::New();
	vtkSmartPointer<vtkMassProperties> massPropertis = 
		vtkSmartPointer<vtkMassProperties>::New();

	featureEdges->BoundaryEdgesOn();	featureEdges->FeatureEdgesOff();	featureEdges->NonManifoldEdgesOff();	featureEdges->ManifoldEdgesOff();
	featureEdges->SetInput(data);
	featureEdges->Update();

	int numberOfOpenEdges = featureEdges->GetOutput()->GetNumberOfCells();

	if(numberOfOpenEdges > 0)
	{
		vtkSmartPointer<vtkFillHolesFilter> fillHolesFilter =
			vtkSmartPointer<vtkFillHolesFilter>::New();
		fillHolesFilter->SetInput(data);

		double bound[6];
		data->GetBounds(bound);
		double x = abs(bound[1] - bound[0]);
		double y = abs(bound[3] - bound[2]);
		double z = abs(bound[5] - bound[4]);
		double holeSize = x;
		if (y > holeSize)
		{
			holeSize = y;
		}
		if (z > holeSize)
		{
			holeSize = z;
		}
		holeSize = holeSize*5;

		fillHolesFilter->SetHoleSize(holeSize);
		fillHolesFilter->Update();

		massPropertis->SetInputConnection(fillHolesFilter->GetOutputPort());
	}
	else
	{
		massPropertis->SetInput(data);
	}

	massPropertis->Update();
	volume = massPropertis->GetVolume();

	volume = volume/100;
	int volumeInt = (int)volume;
	volume = volumeInt/10.0;
}



