#include "Markup.h"
#include "vtkSTLReader.h"
#include "vtkDICOMImageReader.h"
#include "vtkMetaImageReader.h"
#include "vDataReader.h"

#include "vProjectIO.h"
#include "yDataStorageConfigure.h"
#include "vStatusBarNotify.h"
#include "vProgressEvent.h"
#include "vDICOMReader.h"

#include <QSplashScreen>
#include <QDir>

#ifdef WIN32
#include  <io.h>
#endif

#include  <stdio.h>
#include  <stdlib.h>

vProjectIO::vProjectIO(QObject* parent):QObject(parent),
m_DataManagement(NULL),
	m_ProjectXML(""),
	m_ViewXMl(""),
	m_FileName(""),
	m_DirectoryName(""),
	IsProject(false)
{
	m_MaskImages.clear();
	m_MaskNames.clear();
	m_StatusBar = NULL;
	m_Dirs.clear();
	m_Splash = new QSplashScreen(QPixmap("./icon/demo4.png"));
}


vProjectIO::~vProjectIO()
{
}

void vProjectIO::SetFileName(std::string file)
{
	if (file.size() < 1)
	{
		return;
	}

	m_FileName = file;
	QString str(m_FileName.c_str());
	str.replace(QString("/"),QString("\\"));
	m_FileName = str.toStdString();

	int position = (int)m_FileName.find_last_of("\\");
	if (position == std::string::npos)
	{
		return;
	}

	m_DirectoryName = m_FileName.substr(0,position);
	m_Dirs.clear();
}


void vProjectIO::InitializeProject()
{
	if (m_FileName.size() < 1)
	{
		return;
	}

	//Clear the Data Storage
	m_DataManagement->RemoveAllNodes();

	CMarkup xml;
	xml.Load(m_FileName);

	m_ProjectXML = xml.GetDoc();

	if (m_ProjectXML.size() < 1)
	{
		return;
	}

	this->StringToProject(m_ProjectXML);
}

void vProjectIO::InitializeVisibleNode()
{
	CMarkup xml;
	xml.Load(m_FileName);
	if (xml.FindElem("Project Name"))
	{
	   xml.IntoElem();
	}
	if (xml.FindElem("NodeList"))
	{
		std::string nodeNumberStr = xml.GetAttrib("NodeCount");
		int nodeNumber = -1;
		ConvertFromString(nodeNumber,nodeNumberStr);

		xml.IntoElem();
		for (int i = 0; i < nodeNumber; i++)
		{
			if (xml.FindElem("Node"))
			{
				std::string Id       = xml.GetAttrib("ID");
				std::string parentId = xml.GetAttrib("FatherID");
				std::string nodeType = xml.GetAttrib("DataNodeType");

				int parentIDInt  = -1;
				int nodeTypeInt  = -1;
				int tempIndexInt = -1;

				ConvertFromString(nodeTypeInt,nodeType);
				ConvertFromString(parentIDInt,parentId);
				ConvertFromString(tempIndexInt,Id);

				int newIndex = m_DataManagement->CreateNode(DataNodeType(nodeTypeInt),"",NULL,-1);

				//将index设置为保存的ID
				PropertyType type = INT_;
				m_DataManagement->SetNodeProperty(newIndex,"index",Id,"false",type);

				std::string nodeStr = xml.GetSubDoc();
				this->StringToDataNode(nodeStr,tempIndexInt);

				//destroy relationship of parent and child
				m_DataManagement->SetNodeProperty(tempIndexInt,"parent_index","-1","false",STRING_);

			}
		}
		xml.OutOfElem();
	}
}

void vProjectIO::StringToProject(std::string &str)
{
	if (str.size() < 5)
	{
		return;
	}

	CMarkup xml;
	xml.SetDoc(str);

	if (xml.FindElem("Project"))
	{
		m_Splash->show();

		xml.IntoElem();
		std::map<int,bool> visibles;
		//Add Nodes Into Data Storage
		if (xml.FindElem("NodeList"))
		{
			std::string nodeNumberStr = xml.GetAttrib("NodeCount");
			int nodeNumber = -1;
			ConvertFromString(nodeNumber,nodeNumberStr);

			xml.IntoElem();
			for (int i = 0; i < nodeNumber; i++)
			{
				if (xml.FindElem("Node"))
				{
					std::string Id       = xml.GetAttrib("ID");
					std::string parentId = xml.GetAttrib("FatherID");
					std::string nodeType = xml.GetAttrib("DataNodeType");

					int parentIDInt  = -1;
					int nodeTypeInt  = -1;
					int tempIndexInt = -1;

					ConvertFromString(nodeTypeInt,nodeType);
					ConvertFromString(parentIDInt,parentId);
					ConvertFromString(tempIndexInt,Id);

					int newIndex = -1;
					if (nodeTypeInt == IMAGE_DATA ||
						nodeTypeInt == MASKV_DATA ||
						nodeTypeInt == SURFACE_DATA)
					{
						newIndex = m_DataManagement->CreateNode(DataNodeType(nodeTypeInt),"",NULL,parentIDInt);					
						//将index设置为保存的ID
						PropertyType type = INT_;
						m_DataManagement->SetNodeProperty(newIndex,"index",Id,"false",type);

						//根据保存的字符串设置属性
						std::string nodeStr = xml.GetSubDoc();
						this->StringToDataNode(nodeStr,tempIndexInt);

						//保存所有节点的可见性
						type = BOOL_;
						bool visible = false;
						std::string str = m_DataManagement->GetNodeProperty(tempIndexInt,"visible",type);
						ConvertFromString(visible,str);
						visibles.insert(std::pair<int,bool>(tempIndexInt,visible));
					}
				}
				double progress = double((i + 1)*90/nodeNumber);
			} 
			xml.OutOfElem();
		}
		m_MaskNames.clear();
		m_MaskImages.clear();

		//恢复所有节点的可见性
		std::map<int,bool>::iterator it = visibles.begin();
		while (it != visibles.end())
		{
			PropertyType type = BOOL_;
			int node = (*it).first;
			std::string str = ConvertToString((*it).second);

			m_DataManagement->SetNodeProperty(node,"visible",str,"true",type);
			it++;
		}
	}

	if (xml.FindElem("Scene"))
	{
		m_ViewXMl = xml.GetSubDoc();
	}

	m_Splash->hide();
}

void vProjectIO::StringToDataNodeProperty(const std::string &str,int nodeIndex)
{
	CMarkup xml;
	xml.SetDoc(str);
	if (xml.FindElem("Property"))
	{
		PropertyType type = STRING_;

		std::string tempValue      = xml.GetData();
		std::string tempName       = xml.GetAttrib("Name");
		std::string tempType       = xml.GetAttrib("PropertyType");
		std::string tempModefiable = xml.GetAttrib("Modifiable");

		int typeInt = -1;
		ConvertFromString(typeInt,tempType);
		type = static_cast<PropertyType> (typeInt);

		m_DataManagement->SetNodeProperty(nodeIndex,tempName,tempValue,tempModefiable,type);
	}
}

void vProjectIO::SearchAllDir(const QString& dir)
{
	QDir pluginDir(dir);

	if (!pluginDir.exists())
	{
		return;
	}	
	
	pluginDir.setFilter(QDir::Dirs  | QDir::NoDotAndDotDot);

	QFileInfoList lists = pluginDir.entryInfoList();  
	if(lists.size()< 1 ) {  
		return;  
	}  

	int i=0;  

	do{  
		QFileInfo fileInfo = lists.at(i);  
		QString tt = fileInfo.fileName();  

		bool bisDir = fileInfo.isDir();  
		if(bisDir) {  
			m_Dirs.push_back(fileInfo.absoluteFilePath());
			SearchAllDir(fileInfo.filePath());  
		}  
		i++;  
	} while(i < lists.size());  
}


void vProjectIO::DeConfigureDataNodeData(std::string &str, int index)
{
	CMarkup xml;
	xml.SetDoc(str);

	if (xml.FindElem("Data"))
	{
		int tempFatherID = -1;
		DataNodeType tempDataType = m_DataManagement->GetDataNodeType(index);
		int tempDataTypeInt = -1;


		std::string directoryName;
		std::string fileName;
		std::string fileType;
		std::string tempImageNodeName;
		int position = 0;

		xml.IntoElem();

		std::string maskFile = "";
		int number  = (int)m_MaskNames.size();

		vtkSmartPointer<vReadProgress> progress = 
			vtkSmartPointer<vReadProgress>::New();
		progress->SetProgressBar(m_StatusBar);

		if (tempDataType == IMAGE_DATA)
		{
			if (xml.FindElem("DataPath"))
				directoryName = xml.GetData();

			if (xml.FindElem("FileName"))
				fileName = xml.GetData();

			if (xml.FindElem("FileType"))
				fileType = xml.GetData();

			bool isExist = false;

			if(!access(directoryName.c_str(),0))
			{
				position = (int)directoryName.find_last_of("\\");
				directoryName = directoryName.substr(0,position);
				isExist = true;
			}
			else
			{
				QDir pluginDir(m_DirectoryName.c_str());
				pluginDir.cdUp();
				m_Dirs.clear();

				this->SearchAllDir(pluginDir.absolutePath());

				for (int i = 0; i < (int)m_Dirs.size(); i++)
				{
					QFileInfo fileInfo(m_Dirs.at(i));

					QString temp = fileInfo.absoluteFilePath().replace(QString("/"),QString("\\"));
					directoryName = temp.toStdString() + "\\" + fileName;

					position = (int)directoryName.find_last_of("\\");
					directoryName = directoryName.substr(0,position);

					QFileInfo isDir(directoryName.c_str());
					if (isDir.isDir())
					{						
						isExist = true;
						break;
					}
				}
			}

			m_DataManagement->SetData(index,NULL);

			if (isExist)
			{
				m_DataManagement->SetNodeProperty(index,"temp_d",directoryName,"false",STRING_);
			}
		}
		else if(tempDataType == SURFACE_DATA)
		{
			if (xml.FindElem("FileName"))
			{
				fileName = xml.GetData();
				position = (int)fileName.find_last_of("\\");
				tempImageNodeName = fileName.substr(position+1,-1);
			}
			if (xml.FindElem("FileType"))
				fileType = xml.GetData();

			directoryName = m_DirectoryName + "\\" + tempImageNodeName + "." + fileType;

			QString str(QObject::tr("正在读取文件:"));
			QString dir = QString::fromStdString(directoryName);
			m_StatusBar->SendPermentMessage(str + dir);
			m_Splash->showMessage(str + dir);

			vtkSmartPointer<vtkSTLReader> stlReader = 
				vtkSmartPointer<vtkSTLReader>::New();
			stlReader->SetFileName(directoryName.c_str());
			stlReader->AddObserver(vtkCommand::ProgressEvent,progress);
			stlReader->Update();

			str = QObject::tr("文件读取完成!");
			m_StatusBar->SendPermentMessage(str);
			m_Splash->showMessage(str);

			vtkSmartPointer<vtkPolyData> data = vtkSmartPointer<vtkPolyData>::New();
			data->DeepCopy(stlReader->GetOutput());
			m_DataManagement->SetData(index,data);
		}
	}
}

void vProjectIO::StringToDataNode(std::string &str, int index)
{
	CMarkup xml;
	xml.SetDoc(str);
	std::string tempProperty;
	if (xml.FindElem("Node"))
	{

		xml.IntoElem();
		if (xml.FindElem("Data"))
		{		
			//Data should read and set first
            std::string str = xml.GetSubDoc();
			this->DeConfigureDataNodeData(str,index);
		}

		if (xml.FindElem("PropertyList"))
		{
			int tempInt = -1;
			std::string str = xml.GetAttrib("PropertyCount");
			ConvertFromString(tempInt,str);
			xml.IntoElem();
			for (int i = 0; i < tempInt; i++)
			{
				if (xml.FindElem("Property"))
				{
					yDataNodeProperty::Pointer property = yDataNodeProperty::New();
					tempProperty = xml.GetSubDoc();
					this->StringToDataNodeProperty(tempProperty,index);
				}
			}
			xml.OutOfElem();
		}
		xml.OutOfElem();
	}
}
