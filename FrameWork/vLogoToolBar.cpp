#include "vLogoToolBar.h"
#include "vMainWindow.h"

#include <QAction>
#include <QLabel>
#include <QMessageBox>
#include <QEvent>

vLogoToolBar::vLogoToolBar(QWidget* parent /* = 0 */):QToolBar(parent)
{
	m_Title = new QLabel(this);
	m_Title->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
	this->addWidget(m_Title);

	about = new QAction(this);
	about->setIcon(QIcon(QPixmap("./icon/about.png")));
	this->connect(about,SIGNAL(triggered()),this,SLOT(OnAboutDialog()));
	this->addAction(about);

	min = new QAction(this);
	min->setIcon(QIcon(QPixmap("./icon/minimize.png")));
	this->connect(min,SIGNAL(triggered()),this,SLOT(OnMinWindow()));
	this->addAction(min);

	close = new QAction(this);
	close->setIcon(QIcon(QPixmap("./icon/close.png")));
	this->connect(close,SIGNAL(triggered()),this,SLOT(OnCloseWindow()));
	this->addAction(close);

	this->setMovable(false);
	this->setIconSize(QSize(13,13));

	this->translateLanguage();
}

vLogoToolBar::~vLogoToolBar()
{

}

void vLogoToolBar::changeEvent(QEvent *event)
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

void vLogoToolBar::translateLanguage()
{
	m_Title->setText(tr("旭东阅图软件"));
	about->setToolTip(tr("关于本软件"));
	min->setToolTip(tr("最小化"));
	close->setToolTip(tr("最大化"));
}


void vLogoToolBar::OnCloseWindow()
{
	m_MainWindow->close();
}

void vLogoToolBar::OnMinWindow()
{
	m_MainWindow->showMinimized();
}

void vLogoToolBar::OnAboutDialog()
{
	QMessageBox* msgBox = new QMessageBox(this);
	msgBox->setStyleSheet("background-color:#484848;color:#FFFFFF");
	msgBox->setWindowTitle(tr("关于旭东阅图软件"));
	msgBox->setText(tr("旭东阅图软件\n版本1.00\n\n版权所有 @2010-2012旭东数字医学影像技术有限公司 保留所有权利。\n\n本软件使用的第三方软件库列举如下：\nThe Insight Segmentation and Registration Toolkit(ITK)\nThe Qt libraries\nThe Visualization Toolkit(VTK)"));
	msgBox->setStandardButtons(QMessageBox::Ok);
	msgBox->setIcon(QMessageBox::Information);
	msgBox->exec();

	delete msgBox;
}