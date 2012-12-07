#include "vDataReader.h"
#include "vtkDICOMImageReader.h"
#include "vtkImageData.h"
#include "vtkSTLReader.h"
#include "vtkPolyData.h"
#include "vProjectIO.h"
#include "vProgressEvent.h"
#include "vDICOMReader.h"

#include <QFileDialog>
#include <QAction>
#include <QTime>
#include <QCoreApplication>
#include <QApplication>
#include <QCursor>
#include <QSplashScreen>
#include <QMessageBox>

vDataReader::vDataReader(QWidget* parent /* = 0 */):vToolBarBase(parent)
{
	file = new QAction(this);
	file->setIcon(QIcon(QPixmap("./icon/open.png")));
	file->setText("DICOM");

	this->connect(file,SIGNAL(triggered()),this,SLOT(ReadFile()));
	this->addAction(file);

	close = new QAction(this);
	close->setIcon(QIcon(QPixmap("./icon/closeP.png")));
	close->setText("DICOM");

	this->connect(close,SIGNAL(triggered()),this,SLOT(CloseProject()));
	this->addAction(close);
	close->setEnabled(false);

	this->translateLanguage();
}

vDataReader::~vDataReader()
{

}

void vDataReader::translateLanguage()
{
	QString str(tr("���ļ�����ѡ������Ҫ�򿪵��ļ���֧��DICOM,ycm,stl�ļ�"));
	file->setToolTip(tr("���ļ�"));
	file->setStatusTip(str);

	str = tr("ɾ�������ļ�����չ���");
	close->setToolTip(tr("�رչ���"));
	close->setStatusTip(str);
}

void vDataReader::OnFocusChanged()
{

}

void vDataReader::OnDataChanged()
{
	if (m_Dm)
	{
		if (m_Dm->GetNumOfNodes() == 0)
			close->setEnabled(false);
		else
			close->setEnabled(true);
	}
}

void vDataReader::OnLayoutChanged()
{

}

void vDataReader::ReadFile()
{
	if(!this->ParameterCheck())
		return;

	static QString dir = "./home";

	QFileDialog* dialog = new QFileDialog();
	dialog->setStyleSheet("background-color:#484848;color:#FFFFFF");
	dialog->setFileMode(QFileDialog::ExistingFile);
	dialog->setWindowTitle(tr("���ļ�"));
	dialog->setDirectory(dir);

	QString fileDir;
	if (dialog->exec() ==  QDialog::Accepted)
	{
		fileDir = dialog->selectedFiles()[0];
		delete dialog;
	}
	else
	{
		delete dialog;
		return;
	}


	if (!fileDir.isNull())
	{
		dir = QFileInfo (fileDir).path();
	}
	else
		return;

	QString str = fileDir.right(3);
	if (str == "ycm" || str == "YCM")
	{	
        str = tr("���ڶ�ȡ�����ļ���");
		m_Status->SendPermentMessage(str + fileDir);
		this->ReadProjectFile(fileDir);
	}
	else if (str == "stl" || str == "STL")
	{
        str = tr("���ڶ�ȡStl�ļ���");
		m_Status->SendPermentMessage(str + fileDir);
		this->ReadStlFile(fileDir);
	}
	else
	{
        int index = fileDir.lastIndexOf("/");
		QString fileDir1 = fileDir.left(index);

        str = tr("���ڶ�ȡDICOM�ļ��У�");
		m_Status->SendPermentMessage(str + fileDir1);

		this->ReadDICOMFiles(fileDir1);
	}

    m_Status->SendPermentMessage(tr("��ȡ�ļ����!"));
}

void vDataReader::CloseProject()
{
	QMessageBox* msgBox = new QMessageBox(this);
	msgBox->setStyleSheet("background-color:#484848;color:#FFFFFF");
	msgBox->setWindowTitle(tr("����ͼ���"));
	msgBox->setText(tr("ȷ��Ҫɾ���������̣�\n"));
	msgBox->setStandardButtons(QMessageBox::Yes | QMessageBox::No);
	msgBox->setDefaultButton(QMessageBox::No);
	msgBox->setIcon(QMessageBox::Information);
	int ret = msgBox->exec();

	delete msgBox;

	if ( ret == QMessageBox::Yes )
	{
		m_Views->ClearView();		
		
		std::vector<int> nodes = m_Dm->GetAllNodeIndex();
		std::vector<int>::iterator it = nodes.begin();
		while(it != nodes.end())
		{
			m_Model->RemoveNode(*it++);
		}

		m_Views->DataChanged();
	}
}

void vDataReader::ReadDICOMFiles(const QString &str)
{
	m_Views->SetLayout(vMultiView::TWO_2D);

	QTime dieTime = QTime::currentTime().addMSecs(80);
	while( QTime::currentTime() < dieTime )
		QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

	QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));

	QSplashScreen *splash = new QSplashScreen(QPixmap("./icon/demo4.png"));
	splash->show();

    QString str1(tr("���ڶ�ȡ�ļ�Ŀ¼��"));
	splash->showMessage(str1 + str);

	vDICOMReader* reader = new vDICOMReader();
	reader->SetDirectoryName(str.toStdString());
	reader->SetStatusBar(m_Status);
	reader->Update();

	vtkSmartPointer<vtkImageData> data = reader->GetData();
	delete reader;

	splash->hide();

	if (!data)
	{
		QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));

		QMessageBox* msgBox = new QMessageBox(this);
		msgBox->setStyleSheet("background-color:#484848;color:#FFFFFF");
		msgBox->setWindowTitle(tr("����ͼ���"));
		msgBox->setText(tr("DICOM�ļ��������ݲ����ڻ��߶�ȡ����"));
		msgBox->setStandardButtons(QMessageBox::Yes);
		msgBox->setIcon(QMessageBox::Warning);
		msgBox->exec();

		delete msgBox;

	    return;
	}
	int extent[6];
	data->GetExtent(extent);

	//Check if any DICOM files was been read, and deal with it
	if(extent[1] < 0)
	{
		QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));
		QMessageBox* msgBox = new QMessageBox(this);
		msgBox->setStyleSheet("background-color:#484848;color:#FFFFFF");
		msgBox->setWindowTitle(tr("����ͼ���"));
		msgBox->setText(tr("DICOM�ļ��������ݲ�����"));
		msgBox->setStandardButtons(QMessageBox::Yes);
		msgBox->setIcon(QMessageBox::Warning);
		msgBox->exec();

		delete msgBox;

		return;
	}

	int lastRoot = m_Dm->GetVisibleRootIndex();
	if (lastRoot >= 0)
	{
		m_Dm->SetData(lastRoot,NULL);
	}
	
	static int i  = 1;
	std::string str2 = ConvertToString(i);
	int index1 = m_Dm->CreateNode(IMAGE_DATA,"Image" + str2,data);
	m_Dm->SetNodeProperty(index1,"temp_d",str.toStdString(),"false",STRING_);
	++i;

	m_Model->AddNode(index1);

	m_Views->DataChanged();
	m_Views->ExpandAllNodeInTreeView();
}

void vDataReader::ReadProjectFile(const QString &str)
{
	m_Views->SetLayout(vMultiView::PROJECT);

	QTime dieTime = QTime::currentTime().addMSecs(80);
	while( QTime::currentTime() < dieTime )
	QCoreApplication::processEvents(QEventLoop::AllEvents, 100);

	QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));

	m_Views->ClearView();

	std::vector<int> nodes = m_Dm->GetAllNodeIndex();
	std::vector<int>::iterator it = nodes.begin();
	while(it != nodes.end())
	{
		m_Model->RemoveNode(*it++);
	}

	vProjectIO* ycm = new vProjectIO(this);
	ycm->SetFileName(str.toStdString());
	ycm->SetDataManagement(m_Dm);
	ycm->SetStatusBar(m_Status);
	ycm->InitializeProject();

	delete ycm;

	if (m_Dm->GetNumOfNodes() == 0)
	{
		QApplication::setOverrideCursor(QCursor(Qt::ArrowCursor));
		return;
	}
	else
	{
		std::list<int> masks = m_Dm->GetNodesFromType(MASKV_DATA);
		std::list<int>::iterator it = masks.begin();
		while(it != masks.end())
		{
			m_Dm->RemoveNodeByIndex(*it++);
		}
	}

	nodes = m_Dm->GetAllNodeIndex();

	it = nodes.begin();
	while(it != nodes.end())
	{
		m_Model->AddNode(*it++);
	}

	m_Views->DataChanged();
	m_Views->ExpandAllNodeInTreeView();
}

void vDataReader::ReadStlFile(const QString &str)
{
	vMultiView::LayoutType type = m_Views->GetLayout();
	if (type == vMultiView::ONE_2D || 
		type == vMultiView::TWO_2D ||
		type == vMultiView::THREE_2D ||
		type == vMultiView::FOUR_2D)
	{
		m_Views->SetLayout(vMultiView::THREED);

		QTime dieTime = QTime::currentTime().addMSecs(80);
		while( QTime::currentTime() < dieTime )
			QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
	}

	QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));

	QSplashScreen *splash = new QSplashScreen(QPixmap("./icon/demo4.png"));
	splash->show();

	vtkSmartPointer<vReadProgress> progress = vtkSmartPointer<vReadProgress>::New();
	progress->SetProgressBar(m_Status);

    QString stl(tr("���ڶ�ȡ�ļ���"));
	m_Status->SendPermentMessage(stl + str);
	splash->showMessage(stl + str);

	vtkSmartPointer<vtkSTLReader> reader = 
		vtkSmartPointer<vtkSTLReader>::New();
	reader->SetFileName(str.toStdString().c_str());
	reader->AddObserver(vtkCommand::ProgressEvent,progress);
	reader->Update();

    stl = tr("�ļ���ȡ���!");
	m_Status->SendPermentMessage(stl);
	splash->showMessage(stl);

	splash->hide();

	vtkSmartPointer<vtkPolyData> data = 
		vtkSmartPointer<vtkPolyData>::New();
	data->DeepCopy(reader->GetOutput());
	data->Update();

	if(data->GetPoints() == 0)
	{
		QMessageBox* msgBox = new QMessageBox(this);
		msgBox->setStyleSheet("background-color:#484848;color:#FFFFFF");
		msgBox->setWindowTitle(tr("����ͼ���"));
		msgBox->setText(tr("Stl�ļ������ݲ�����"));
		msgBox->setStandardButtons(QMessageBox::Yes);
		msgBox->setIcon(QMessageBox::Warning);
		msgBox->exec();

		delete msgBox;

		return;
	}

	m_Dm->CreateNode(SURFACE_DATA,str.toStdString().c_str(),data);
	int index = m_Dm->GetIndexFromName(str.toStdString().c_str());
	m_Dm->SetNodeProperty(index,"color","1,0,0","true",COLOR_);
	m_Dm->SetNodeProperty(index,"visible","1","true",BOOL_);
	m_Dm->SetNodeProperty(index,"opacity","0.3","true",DOUBLE_);

	m_Model->AddNode(index);

	m_Views->DataChanged();
	m_Views->ExpandAllNodeInTreeView();
}
