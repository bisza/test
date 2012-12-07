#ifndef yProjectIO_h__
#define yProjectIO_h__

#include "itkObject.h"
#include "itkObjectFactory.h"

#include "yDataManagement.h"
#include "vtkImageData.h"

class vStatusBarNotify;
class QSplashScreen;

class vProjectIO :public QObject
{
public:
	inline void SetDataManagement(yDataManagement::Pointer data_management){m_DataManagement = data_management;};
	void        SetFileName(std::string file);	
	void        SetStatusBar(vStatusBarNotify* status) {m_StatusBar = status;};

	void        InitializeProject();
	void        InitializeVisibleNode();

public:
	vProjectIO(QObject* parent = 0);
	~vProjectIO(void);

private:
	void        StringToProject(std::string &str);		
	void        StringToDataNode(std::string &str, int index);
	void        StringToDataNodeProperty(const std::string &str,int nodeIndex);
	void        DeConfigureDataNodeData(std::string &str, int index);

	void        SearchAllDir(const QString& dir);

	std::string m_ProjectXML;
	std::string m_ViewXMl;

	std::string m_FileName;
	std::string m_DirectoryName;
	bool		IsProject;	

private:
	yDataManagement::Pointer			         m_DataManagement;
	std::map<int,vtkSmartPointer<vtkImageData> >  m_MaskImages;
	std::map<int,std::string>                    m_MaskNames;

	vStatusBarNotify*                            m_StatusBar;
	QSplashScreen*								 m_Splash;
	std::vector<QString>							 m_Dirs;
};
#endif // yProjectIO_h__
