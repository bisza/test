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
	m_Title->setText(tr("����ͼ���"));
	about->setToolTip(tr("���ڱ����"));
	min->setToolTip(tr("��С��"));
	close->setToolTip(tr("���"));
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
	msgBox->setWindowTitle(tr("��������ͼ���"));
	msgBox->setText(tr("����ͼ���\n�汾1.00\n\n��Ȩ���� @2010-2012������ҽѧӰ�������޹�˾ ��������Ȩ����\n\n�����ʹ�õĵ�����������о����£�\nThe Insight Segmentation and Registration Toolkit(ITK)\nThe Qt libraries\nThe Visualization Toolkit(VTK)"));
	msgBox->setStandardButtons(QMessageBox::Ok);
	msgBox->setIcon(QMessageBox::Information);
	msgBox->exec();

	delete msgBox;
}