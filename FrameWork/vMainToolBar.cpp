#include "vMainToolBar.h"
#include "vLogoToolBar.h"

#include "vMultiView.h"
#include "vLogoToolBar.h"
#include "yDataManagement.h"
#include "vStatusBar.h"
#include "vStatusBarNotify.h"
#include "vTreeModel.h"
#include "vTreeView.h"
#include "vMainWindow.h"
#include "vToolBarBase.h"

#include <QDir>
#include <QPushButton>
#include <QGridLayout>
#include <QStringList>
#include <QFileInfoList>
#include <QLibrary>

vMainToolBar::vMainToolBar(QWidget* parent):QToolBar(parent)
{
	this->setMovable(false);
	this->setContentsMargins(0,0,0,0);
}

vMainToolBar::~vMainToolBar()
{

}

void vMainToolBar::InitializeToolBar()
{
	QWidget* mainWidget = new QWidget(this);
	mainWidget->setContentsMargins(0,0,0,0);

	QGridLayout* mainLayout = new QGridLayout();
	mainWidget->setLayout(mainLayout);

	this->addWidget(mainWidget);

	//////////////////////////////////////////////////////////////////////////
	QPushButton* imageWidget = new QPushButton(this);
	imageWidget->setStyleSheet("background-color:#484848");
	imageWidget->setContentsMargins(0,0,0,0);
	imageWidget->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
	imageWidget->setFixedSize(64,64);
	imageWidget->setIcon(QIcon(QPixmap("./icon/logo.png")));
	imageWidget->setIconSize(QSize(64,64));

	this->connect(imageWidget,SIGNAL(clicked()),m_MainWindow,SLOT(changeLanguage()));

	mainLayout->addWidget(imageWidget,0,0,2,1);

	vLogoToolBar* logoTool = new vLogoToolBar(this);
	logoTool->SetMainWindow(m_MainWindow);

	mainLayout->addWidget(logoTool,0,1);

	//////////////////////////////////////////////////////////////////////////
	QDir pluginDir("./tool");
	const QFileInfoList list = pluginDir.entryInfoList(); 
	QHBoxLayout* layoutH= new QHBoxLayout();

	for (int i = 0; i < list.size(); i++)
	{
		QFileInfo fileInfo = list.at(i);

		QLibrary lib( fileInfo.absoluteFilePath());
		typedef vToolBarBase* (*MyPrototype)(int*,int*);

		lib.load();

		if (lib.isLoaded())
		{
			int position[1] = {-1};
			int squence[1] = {-1};
			MyPrototype  myFunction = (MyPrototype) lib.resolve("GetToolBar");
			vToolBarBase* tool = (*myFunction)(position,squence);
			if (tool)
			{
				tool->SetDataManagement(m_Dm);
				tool->SetMultiView(m_Views);
				tool->SetStatusBar(m_StatusBar);
				tool->SetTreeModel(m_TreeModel);
				tool->setParent(mainWidget);

				this->connect(m_Views,SIGNAL(layoutChanged()),tool,SLOT(OnLayoutChanged()));
				this->connect(m_Views,SIGNAL(dataChanged()),tool,SLOT(OnDataChanged()));
				this->connect(m_Views,SIGNAL(focusChanged()),tool,SLOT(OnFocusChanged()));

				layoutH->addWidget(tool);
			}				

		}
	}

	layoutH->addStretch(1);

	mainLayout->addLayout(layoutH,1,1);
}