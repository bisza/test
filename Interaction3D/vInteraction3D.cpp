#include "vInteraction3D.h"
#include "vView3D.h"
#include "vtkVolumeRayCastMapper.h"
#include "vtkVolumeRayCastCompositeFunction.h"
#include "vtkVolumeProperty.h"
#include "vTransFunction.h"
#include "yDataStorageConfigure.h"
#include "Markup.h"
#include "vtkImageData.h"
#include "vtkImageShiftScale.h"

#include <QAction>
#include <QMenu>
#include <QMessageBox>
#include <QEvent>

vInteraction3D::vInteraction3D(QWidget* parent /* = 0 */):vToolBarBase(parent)
{
	m_Text = new QAction(this);
	m_Text->setIcon(QIcon(QPixmap("./icon/text.png")));
	m_Text->setText("Text In 3D");
	m_Text->setCheckable(true);
	m_Text->setChecked(false);
	this->connect(m_Text,SIGNAL(triggered()),this,SLOT(OnText()));
	this->addAction(m_Text);

	this->ReadDefaultProperty();

	m_VolumeRendering = new QAction(this);
	m_VolumeRendering->setIcon(QIcon(QPixmap("./icon/volume.png")));
	m_VolumeRendering->setText("Volume Rendering");;
	m_VolumeRendering->setCheckable(true);
	m_VolumeRendering->setChecked(false);
	this->connect(m_VolumeRendering,SIGNAL(triggered()),this,SLOT(OnVolumeRendering()));
	QMenu* menu = new QMenu(this);
	menu->setStyleSheet("QMenu { background-color: #ABABAB; border: 1px solid black;}" \
		"QMenu::item {background-color: transparent;}" \
		"QMenu::item:selected {background-color: #654321;}");

	m_VolumeRendering->setMenu(menu);

	int i = 0;

	std::map<std::string, vTransFunction*>::iterator it = m_Trans.begin();
	while(it != m_Trans.end())
	{
		QAction* action = new QAction(menu);
		action->setText(QString::fromStdString((*it++).first));
		action->setData(i++);
		connect(action,SIGNAL(triggered()),this,SLOT(OnVolumeProperty()));

		menu->addAction(action);
	}

	this->addAction(m_VolumeRendering);

	m_RealColor = new QAction(this);
	m_RealColor->setIcon(QIcon(QPixmap("./icon/colorR.png")));
	m_RealColor->setText("rendering use peel");
	m_RealColor->setCheckable(true);
	m_RealColor->setChecked(false);
	this->connect(m_RealColor,SIGNAL(triggered()),this,SLOT(OnRealColor()));

	this->addAction(m_RealColor);

	m_Volume = NULL;
	m_DisableAction = NULL;
	lastRootIndex = -1;

	this->setDisabled(true);
	this->translateLanguage();
}

vInteraction3D::~vInteraction3D()
{

}

void vInteraction3D::translateLanguage()
{
	QString str = tr("三维视图统计文字信息显隐");
	m_Text->setToolTip(tr("文字显隐"));
	m_Text->setStatusTip(str);

	str = tr("三维窗口中体绘制的显隐");
	m_VolumeRendering->setToolTip(tr("体绘制"));
	m_VolumeRendering->setStatusTip(str);

	str = tr("三维窗口中绘制切换");
	m_RealColor->setToolTip(tr("绘制切换"));
	m_RealColor->setStatusTip(str);
}

void vInteraction3D::OnRealColor()
{
	QAction* action = dynamic_cast<QAction*>(sender());
	if (action)
	{
		bool isChecked = action->isChecked();

		std::list<vViewBase*> views = m_Views->GetViews();
		std::list<vViewBase*>::iterator it = views.end();
		it--;
		vViewBase* view = *it;

		if (isChecked)
		{
			QMessageBox* msgBox = new QMessageBox(this);
			msgBox->setStyleSheet("background-color:#484848;color:#FFFFFF");
			msgBox->setWindowTitle(tr("旭东阅图软件"));
			msgBox->setText(tr("使用此功能会使交互速度变慢或者停滞，如果没有必要，请切换到正常模式！\n本模式只支持Nvidia显卡"));
			msgBox->setStandardButtons(QMessageBox::Yes);
			msgBox->setIcon(QMessageBox::Warning);
			msgBox->exec();

			delete msgBox;

			view->GetRenderWindow()->SetAlphaBitPlanes(1);
			view->GetRenderWindow()->SetMultiSamples(0);

			view->GetRender()->SetUseDepthPeeling(1);
			view->GetRender()->SetMaximumNumberOfPeels(30);
			view->GetRender()->SetOcclusionRatio(0.1);
		}
		else
		{
			view->GetRender()->SetUseDepthPeeling(0);
	    }

		view->GetRenderWindow()->Render();
	}
}

void vInteraction3D::ReadDefaultProperty()
{
	CMarkup xml;

	if(xml.Load("./default/GrayOpacityGradient.xml") == false)
		return;

	if (xml.FindElem("ItemList"))
	{
		xml.IntoElem();
		while(xml.FindElem("Item"))
		{
			std::string name = xml.GetAttrib("name");
			vTransFunction* transFunction = new vTransFunction(this);
			xml.IntoElem();
			if (xml.FindElem("GrayColorList"))
			{
				xml.IntoElem();
				while(xml.FindElem("GrayValueColor"))
				{
					std::string r = xml.GetAttrib("R");
					std::string g = xml.GetAttrib("G");
					std::string b = xml.GetAttrib("B");
					std::string value = xml.GetAttrib("scaler");

					double r1,g1,b1,value1;
					ConvertFromString(r1,r);
					ConvertFromString(g1,g);
					ConvertFromString(b1,b);
					ConvertFromString(value1,value);

					vtkSmartPointer<vtkColorTransferFunction> color = 
						transFunction->GetColorTunction();
					color->AddRGBPoint(value1,r1,b1,g1);
				}
				xml.OutOfElem();
			}

			if (xml.FindElem("GrayGridentList"))
			{
				xml.IntoElem();
				while(xml.FindElem("GrayGrident"))
				{
					std::string x = xml.GetAttrib("X");
					std::string y = xml.GetAttrib("Y");

					double x1,y1;
					ConvertFromString(x1,x);
					ConvertFromString(y1,y);

					vtkSmartPointer<vtkPiecewiseFunction> gradient = 
						transFunction->GetGradientFunction();
					gradient->AddPoint(x1,y1);
				}
				xml.OutOfElem();
			}

			if (xml.FindElem("GrayOpacityList"))
			{
				xml.IntoElem();
				while(xml.FindElem("GrayOpacity"))
				{
					std::string x = xml.GetAttrib("X");
					std::string y = xml.GetAttrib("Y");

					double x1,y1;
					ConvertFromString(x1,x);
					ConvertFromString(y1,y);

					vtkSmartPointer<vtkPiecewiseFunction> opacity = 
						transFunction->GetOpacityFunction();
					opacity->AddPoint(x1,y1);
				}
				xml.OutOfElem();
			}

			m_Trans.insert(std::make_pair<std::string,vTransFunction*>(name,transFunction));
			xml.OutOfElem();
		}
	}
}

void vInteraction3D::OnFocusChanged()
{
	this->OnDataChanged();
}

void vInteraction3D::OnDataChanged()
{
	if(!this->ParameterCheck())
		return;

	if (m_Dm->GetNumOfNodes())
	{
		this->setDisabled(false);
		if (m_Dm->GetNumOfNodes(SURFACE_DATA) == 0)
		{
			m_Text->setEnabled(false);
			m_RealColor->setEnabled(false);
		}
		else
		{	
			m_Text->setEnabled(true);	
			m_RealColor->setEnabled(true);
		}
		if (m_Dm->GetVisibleRootIndex() == -1)
			m_VolumeRendering->setEnabled(false);
		else
			m_VolumeRendering->setEnabled(true);
	}
	else
	{
		this->setDisabled(true);
	}

	int root = m_Dm->GetVisibleRootIndex();
	if (lastRootIndex != root)
	{
		std::list<vViewBase*> views = m_Views->GetViews();
		std::list<vViewBase*>::iterator it1 = --views.end();
		vViewBase* view = *it1;

		view->GetRender()->RemoveViewProp(m_Volume);
		view->GetRenderWindow()->Render();
	}

	if (this->isEnabled())
	{
		if (vViewBase::m_LastView == NULL)
		{
			return;
		}

		vView2D* view = dynamic_cast<vView2D*>(vViewBase::m_LastView);
		if (view)
		{
			this->setDisabled(true);
		}
	}
}

void vInteraction3D::OnVolumeProperty()
{
	int index = m_Dm->GetVisibleRootIndex();
	vtkSmartPointer<vtkImageData> data = vtkImageData::SafeDownCast(m_Dm->GetData(index));
	if (!data)
		return;

	lastRootIndex = index;

	QAction* action = dynamic_cast<QAction*>(sender());
	if (action)
	{
		if (m_DisableAction)
		{
			m_DisableAction->setDisabled(false);
		}

		QVariant v = action->data();
		int vv = v.toInt();

		if (vv > (int)m_Trans.size())
			return;

		std::list<vViewBase*> views = m_Views->GetViews();
		std::list<vViewBase*>::iterator it1 = --views.end();
		vViewBase* view = *it1;

		std::map<std::string,vTransFunction*>::iterator it = m_Trans.begin();
		for (int i = 0; i < vv; i++)
		{
			++it;
		}

		vTransFunction* transfunction = (*it).second; 

		vtkSmartPointer<vtkColorTransferFunction> color = transfunction->GetColorTunction();
		vtkSmartPointer<vtkPiecewiseFunction> gradient = transfunction->GetGradientFunction();
		vtkSmartPointer<vtkPiecewiseFunction> opacity = transfunction->GetOpacityFunction();

		if (!m_Volume)
		{
			try
			{
				vtkSmartPointer<vtkVolumeRayCastCompositeFunction> rayCastFunction = 
					vtkSmartPointer<vtkVolumeRayCastCompositeFunction>::New();
	
				vtkSmartPointer<vtkVolumeRayCastMapper> mapper = 
					vtkSmartPointer<vtkVolumeRayCastMapper>::New();
			
				mapper->SetVolumeRayCastFunction(rayCastFunction);
				mapper->ReleaseDataFlagOn();
	
				int type = data->GetScalarType();
				if (type != VTK_UNSIGNED_CHAR ||
					type != VTK_UNSIGNED_SHORT)
				{
					vtkSmartPointer<vtkImageShiftScale> shift = 
						vtkSmartPointer<vtkImageShiftScale>::New();
					shift->SetInput(data);
					shift->SetOutputScalarTypeToUnsignedShort();
					shift->ClampOverflowOn();
					shift->ReleaseDataFlagOn();
					mapper->SetInputConnection(shift->GetOutputPort());	
				}
				else
				{
					mapper->SetInput(data);
				}
	
				vtkSmartPointer<vtkVolumeProperty> propertys = vtkSmartPointer<vtkVolumeProperty>::New();
				propertys->SetColor(color);
				propertys->SetGradientOpacity(gradient);
				propertys->SetScalarOpacity(opacity);
				propertys->SetInterpolationTypeToLinear();
				propertys->ShadeOn();
				propertys->SetAmbient(0.4);
				propertys->SetDiffuse(0.6);
				propertys->SetSpecular(0.2);
	
				m_Volume = vtkSmartPointer<vtkVolume>::New();
				m_Volume->SetMapper(mapper);
				m_Volume->SetProperty(propertys);
	
				view->GetRender()->AddViewProp(m_Volume);
	
				view->ResetCamera();
				view->GetRenderWindow()->Render();
			}
			catch (...)
			{
				m_Status->SendPermentMessage(tr("体绘制失败！"));
			}

		} 
		else
		{
			vtkSmartPointer<vtkVolumeProperty> propertys = m_Volume->GetProperty();
			if (propertys)
			{
				propertys->SetColor(color);
				propertys->SetGradientOpacity(gradient);
				propertys->SetScalarOpacity(opacity);
				view->GetRender()->AddViewProp(m_Volume);

				view->GetRenderWindow()->Render();
			}
		}

		action->setDisabled(true);
		m_DisableAction = action;
	}
}

void vInteraction3D::OnLayoutChanged()
{
	if (m_Views->GetLayout() == vMultiView::THREED ||
		m_Views->GetLayout() == vMultiView::PROJECT)
	{
		this->setDisabled(false);
		this->OnDataChanged();
	}
	else
	{
		this->setDisabled(true);
	}

}

void vInteraction3D::OnText()
{
	if(!this->ParameterCheck())
		return;

	vMultiView::LayoutType type = m_Views->GetLayout();

	if (type == vMultiView::PROJECT ||
		type == vMultiView::THREED)
	{
		std::list<vViewBase*> views = m_Views->GetViews();
		std::list<vViewBase*>::iterator it = --views.end();
		vView3D* view = dynamic_cast<vView3D*>(*it);
		if (view)
		{
			bool show = view->GetTextVisibility();
			view->SetTextVisibility(!show);
		}
	}
}

void vInteraction3D::OnVolumeRendering()
{
	if (m_Volume)
	{
		std::list<vViewBase*> views = m_Views->GetViews();
		std::list<vViewBase*>::iterator it1 = --views.end();
		vViewBase* view = *it1;

		view->GetRender()->RemoveViewProp(m_Volume);
		view->GetRenderWindow()->Render();
		m_Volume = NULL;
		m_DisableAction->setEnabled(true);
		return;
	}
	else
	{
		QMenu* menu = m_VolumeRendering->menu();
		QList<QAction*> action = menu->actions();
		action.at(0)->trigger();
	}
}
