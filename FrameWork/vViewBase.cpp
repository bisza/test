#include "vViewBase.h"
#include "vtkRenderWindow.h"
#include "vtkCamera.h"
#include "vtkWindowToImageFilter.h"
#include "vtkPNGWriter.h"

#include <QMenu>
#include <QAction>
#include <QColorDialog>
#include <QFileDialog>
#include <QEvent>
#include <QGraphicsProxyWidget>

vViewBase* vViewBase::m_LastView = NULL;

vViewBase::vViewBase(QWidget* parent /* = 0 */):QVTKWidget(parent)
{
	vtkSmartPointer<vtkRenderWindow> renWin = 
		vtkSmartPointer<vtkRenderWindow>::New();
	this->SetRenderWindow(renWin);

	m_Render = vtkSmartPointer<vtkRenderer>::New();
	renWin->AddRenderer(m_Render);

	renWin->SetAlphaBitPlanes(1);
	renWin->SetMultiSamples(0);

	m_Render->SetUseDepthPeeling(1);
	m_Render->SetMaximumNumberOfPeels(1);
	m_Render->SetOcclusionRatio(1);

	m_Menu = new QMenu(this);
	m_Menu->setStyleSheet("QMenu { background-color: #ABABAB; border: 1px solid black;}" \
		"QMenu::item {background-color: transparent;}" \
		"QMenu::item:selected {background-color: #654321;}");

	background = new QAction(m_Menu);
	this->connect(background,SIGNAL(triggered()),this,SLOT(OnBackGround()));
	m_Menu->addAction(background);

	image = new QAction(m_Menu);
	this->connect(image,SIGNAL(triggered()),this,SLOT(OnSaveImage()));
	m_Menu->addAction(image);

	m_Actor = NULL;
	foucs = false;

	this->translateLanguage();
}

vViewBase::~vViewBase()
{
}

void vViewBase::changeEvent(QEvent *event)
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

void vViewBase::translateLanguage()
{
	background->setText(tr("ÐÞ¸Ä±³¾°"));
	image->setText(tr("±£´æÍ¼Æ¬"));
}


void vViewBase::SetFoucs()
{
	if (m_LastView)
	{
		m_LastView->ClearFoucs();
	}

	vViewBase::m_LastView = this;	

	container->setStyleSheet("background-color:red");
	container->repaint();

	foucs = true;

	emit focusChanged();
}

void vViewBase::ClearFoucs()
{
	container->setStyleSheet("background-color:black");
	container->repaint();

	foucs = false;
}

bool vViewBase::isFoucs()
{
	return foucs;
}

void vViewBase::OnBackGround()
{
	double color[3] = {0,0,0};
	m_Render->GetBackground(color);

	QColorDialog* colorDialog = new QColorDialog(QColor((int)color[0]*255.0,(int)color[1]*255.0,(int)color[2]*255.0),this);
	colorDialog->setStyleSheet("background-color:#484848;color:#FFFFFF");
	colorDialog->setWindowTitle(tr("ÇëÑ¡Ôñ±³¾°É«"));
	colorDialog->exec();

	QColor newColor; 
	switch (colorDialog->result())
	{
	case  QColorDialog::Accepted:
		newColor = 	colorDialog->selectedColor();
		delete colorDialog;
		break;
	case  QColorDialog::Rejected:
	default:
		delete colorDialog;
		return;
	}

	m_Render->SetBackground(newColor.red()/255.0,newColor.green()/255.0,newColor.blue()/255.0);
	m_Render->GetRenderWindow()->Render();
}

void vViewBase::OnSaveImage()
{
	QString fileName;

	QFileDialog* dialog = new QFileDialog(this);
	dialog->setWindowTitle(tr("±£´æÍ¼Æ¬"));
	dialog->setNameFilter(tr("Í¼Æ¬(*.png)"));
	dialog->setAcceptMode(QFileDialog::AcceptSave);
	dialog->setDefaultSuffix("png");
	dialog->setFileMode(QFileDialog::AnyFile);
	dialog->setStyleSheet("background-color:#484848;color:#FFFFFF;");
	dialog->exec();

	if (dialog->result() ==  QDialog::Accepted)
	{
		fileName = dialog->selectedFiles()[0];
		delete dialog;
	}
	else
	{
		delete dialog;
		return;
	}

	this->GetRenderWindow()->SetOffScreenRendering(true);
	this->OnCornerInformation(true);
	this->GetRenderWindow()->Render();

	vtkSmartPointer<vtkWindowToImageFilter> windowToImageSnap 
		= vtkSmartPointer<vtkWindowToImageFilter>::New();
	windowToImageSnap->SetInput(this->GetRenderWindow());

	vtkSmartPointer<vtkPNGWriter> writer 
		= vtkSmartPointer<vtkPNGWriter>::New();
	writer->SetFileName(fileName.toStdString().c_str());
	writer->SetInputConnection(windowToImageSnap->GetOutputPort());
	writer->Write();

	this->OnCornerInformation(false);
	this->GetRenderWindow()->SetOffScreenRendering(false);
	this->GetRenderWindow()->Render();
}

void vViewBase::UnInstallPipeLine()
{
	if (m_Actor == NULL)
	{
		return;
	}
	else
	{
		if (m_Render)
		{
			m_Render->RemoveViewProp(m_Actor);
		}
	}
}

void vViewBase::OnFoucs()
{	
	this->SetFoucs();
}

void vViewBase::ResetCamera()
{
	if (m_Render)
	{
		m_Render->ResetCamera();
		m_Render->GetActiveCamera()->Zoom(1.25);
	}
}

void vViewBase::contextMenuEvent(QContextMenuEvent*)
{
	this->OnFoucs();

	m_Menu->exec(QCursor::pos());
}

void vViewBase::mouseDoubleClickEvent(QMouseEvent *)
{
	this->SetFoucs();

	emit layoutOne();
}