#ifndef vView2D_h__
#define vView2D_h__

#include "vViewBase.h"
#include "vtkImagePlaneWidget.h"
#include "vFrameWorkDll.h"
#include "vtkCornerAnnotation.h"
#include "vtkCallbackCommand.h"
#include "vtkAbstractWidget.h"
#include "vtkImageMapToColors.h"
#include "vInteractorStyle.h"

class QMenu;

class V_FRAME_WORK_EXPORT vView2D : public vViewBase
{
	Q_OBJECT
public:
	vView2D(QWidget* parent = 0);
	~vView2D();

public:
	virtual void InstallPipeLine(int root_index);
	virtual void UnInstallPipeLine();

	void SetSliceIndex(int index);
	int GetSliceIndex(){return m_Index;};
	void SetSliceOrientation(int orientation);
	int GetSliceOrientation(){return m_Orientation;};

	void HideImage();
	void ShowImage();

	void UpdateImagePlaneProperty(int rootIndex);
	vtkSmartPointer<vtkImagePlaneWidget> GetPlane() {return m_Plane;};

	void Render();

	void AddOberserWheel();
	void DeleteOberserWheel();

	void SetWindowLevel(double window, double level);

	void AddWidgetOberser(vtkSmartPointer<vtkAbstractWidget> widget);
	void ChangeWidgetWindowToCurrent();

protected:
	static void func(vtkObject* vtkNotUsed(object), unsigned long eid, void* clientdata, void *calldata);
	void OnForwardWheel();
	void OnBackwardWheel();

	virtual void OnCornerInformation(bool arg);
	virtual void translateLanguage();

protected:
	vtkSmartPointer<vtkImagePlaneWidget>            m_Plane;
	vtkSmartPointer<vtkCornerAnnotation>            m_CornerText;
	vtkSmartPointer<vtkImageMapToColors>            m_Mapper;
	vtkSmartPointer<vInteractorStyle>			    m_Style;

	vtkSmartPointer<vtkCallbackCommand>             m_CallBack;
	static vtkSmartPointer<vtkAbstractWidget>		m_TempWidget;
	int m_Index;
	int m_Orientation;
};
#endif // vView2D_h__