#ifndef vDICOMReader_h__
#define vDICOMReader_h__

#include <QObject>
#include "vtkImageData.h"
#include "vtkSmartPointer.h"
#include "vFrameWorkDll.h"
#include <string>

class vStatusBarNotify;

class V_FRAME_WORK_EXPORT vDICOMReader : public QObject
{
public:
	vDICOMReader(QObject* parent = 0);
	~vDICOMReader();

public:
	void SetDirectoryName(std::string dir){m_Dir = dir;};
	void SetStatusBar(vStatusBarNotify* status) {m_Status = status;};
	void Update();

	vtkSmartPointer<vtkImageData> GetData(){return m_Data;};

protected:
	vtkSmartPointer<vtkImageData> m_Data;
	std::string m_Dir;
	vStatusBarNotify* m_Status;

	template<typename T>
	void ReadDICOMByItk();
};
#endif // vDICOMReader_h__
