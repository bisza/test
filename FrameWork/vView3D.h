#ifndef vView3D_h__
#define vView3D_h__

#include "vViewBase.h"
#include "vFrameWorkDll.h"
#include "vModelLabel.h"

class V_FRAME_WORK_EXPORT vView3D : public vViewBase
{
	Q_OBJECT
public:
	vView3D(QWidget* parent = 0);
	~vView3D();

	virtual void InstallPipeLine(int index);
	virtual void UnInstallPipeLine();

	virtual void UpdateModelProperty(std::list<int> &indexes);
	virtual void UpdateModelProperty(int node);

	void SetTextVisibility(bool visible);
	bool GetTextVisibility(){return isTextShow;};
	virtual void UpdateModelText(int index);


protected:
	virtual void translateLanguage();

	void GetText(std::string& str);

	void CalculateVolume(int index,double &volume);

protected:
	vtkSmartPointer<vModelLabel>  m_Label; 
	std::list<vtkSmartPointer<vtkActor>> m_Actors;
	bool isTextShow;
};
#endif // vView3D_h__