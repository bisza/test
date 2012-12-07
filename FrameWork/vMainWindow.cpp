#include "vMainWindow.h"
#include "vMainToolBar.h"
#include <QTreeView>
#include <QDockWidget>
#include <QMessageBox>
#include <QCloseEvent>
#include <QPushButton>
#include <QPainter>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QColor>
#include <QApplication>
#include <QLocale>

#include "vStatusBar.h"
#include "vTreeView.h"
#include "vMainToolBar.h"

mainWindow::mainWindow()
{
	//Check system language
	QLocale loc = QLocale::system();
	if (loc.language() == QLocale::Chinese)
		isChinese = false;
	else
		isChinese = true;

	m_Translater = NULL;
	m_LocalTranslater = NULL;

	m_Dm = yDataManagement::New();
	m_StatusBar = new vStatusBarNotify(this);
	m_TreeModel = new vTreeModel(m_Dm,this);  	
	vStatusBar* statusBar = new vStatusBar(this); 	
	this->connect(m_StatusBar,SIGNAL(permentMessage(QString)),statusBar,SLOT(showMessage(QString)));
	this->connect(m_StatusBar,SIGNAL(progressNotify(int)),statusBar,SLOT(SetProgress(int)));	

	m_Views = new vMultiView(this,m_Dm);
	m_Views->SetStatusBar(m_StatusBar);

	this->setCentralWidget(m_Views);
	this->setStatusBar(statusBar);
	this->Initialize();
	this->changeLanguage();
	this->translateLanguage();
}

void mainWindow::changeLanguage()
{
	isChinese = !isChinese;

	if (!m_Translater)
	{	
		m_Translater = new QTranslator(this);
	}
	if (!m_LocalTranslater)
	{
		m_LocalTranslater = new QTranslator(this);
	}
	
	if (isChinese)
	{
		m_Translater->load("./language/speedlinguist_zh_cn");
		m_LocalTranslater->load("./language/qt_zh_CN.qm");
		QApplication::instance()->installTranslator(m_LocalTranslater);
	}	
	else
	{
		m_Translater->load("./language/speedlinguist_en.qm");
		QApplication::instance()->removeTranslator(m_LocalTranslater);
		
		QFont newFont("Times New Roman",10);
		QApplication::setFont(newFont);
	}

	QApplication::instance()->installTranslator(m_Translater);
}

void mainWindow::Initialize()
{
	//设置主框架背景色
	this->setAutoFillBackground(true);
	QPalette palette;
	palette.setColor(QPalette::Window,QColor(72,72,72));
	palette.setColor(QPalette::NoRole,QColor(255,0,0));
	palette.setColor(QPalette::WindowText,QColor(255,255,255));
	palette.setColor(QPalette::ButtonText,QColor(255,255,255));
	palette.setColor(QPalette::Base,QColor(119,119,119));
	palette.setColor(QPalette::AlternateBase,QColor(72,72,72));
	palette.setColor(QPalette::Light,QColor(52,52,52));
	palette.setColor(QPalette::Button,QColor(72,72,72));
	palette.setBrush(QPalette::Inactive,QPalette::ToolTipBase,QBrush(QColor(255,0,0)));
	palette.setBrush(QPalette::Inactive,QPalette::ToolTipText,QColor(QColor(0,0,255)));

	this->setPalette(palette);	
	this->setWindowIcon(QIcon(QPixmap("./icon/logo.png")));

	this->InitializeDataTree();
	this->InitializePulgin();
}

void mainWindow::InitializeDataTree()
{
	m_TreeModel = new vTreeModel(m_Dm);
	vTreeView* treeView = new vTreeView(m_TreeModel);	

	m_TreeModel->SetViews(m_Views);
	m_Views->SetTreeView(treeView);	
	treeView->SetMultiView(m_Views);

	this->addDockWidget(Qt::LeftDockWidgetArea,treeView);
}

void mainWindow::InitializePulgin()
{
	vMainToolBar* mainToolBar = new vMainToolBar(this);
	mainToolBar->SetMainWindow(this);
	mainToolBar->SetDataManagement(m_Dm);
	mainToolBar->SetMultiView(m_Views);
	mainToolBar->SetStatusBar(m_StatusBar);
	mainToolBar->SetTreeModel(m_TreeModel);
	mainToolBar->InitializeToolBar();
	this->addToolBar(mainToolBar);
}

void mainWindow::changeEvent(QEvent *event)
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

void mainWindow::translateLanguage()
{
	this->statusBar()->showMessage(tr("准备"));
    this->setWindowTitle(tr("旭东阅图软件"));
}

mainWindow::~mainWindow()
{
}

void mainWindow::closeEvent(QCloseEvent *event)
{
	QMessageBox* msgBox = new QMessageBox(this);
	msgBox->setStyleSheet("background-color:#484848;color:#FFFFFF");
	msgBox->setWindowTitle(tr("旭东阅图软件"));
	msgBox->setText(tr("您是否真的需要退出本阅图软件?"));
	msgBox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	msgBox->setDefaultButton(QMessageBox::No);
	msgBox->setIcon(QMessageBox::Information);
	int ret = msgBox->exec();

	delete msgBox;

	switch(ret) 
	{
	case QMessageBox::Yes:
		event->accept();
		break;
	case QMessageBox::No:
	default: 
		event->ignore();
		break; 
	}
}
