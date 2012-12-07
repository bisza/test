#ifndef vInteractorStyle_h__
#define vInteractorStyle_h__

#include "vtkInteractorStyle.h"
#include "vtkAbstractWidget.h"
#include "vtkSmartPointer.h"
#include "vtkWindowLevelLookupTable.h"
#include "vFrameWorkDll.h"
#include "vtkCornerAnnotation.h"
#include "vtkImageData.h"

class vView2D;

#define VTKIS_WINDOW_LEVEL            1024
#define VTKIS_RESLICE                 1027

class V_FRAME_WORK_EXPORT vInteractorStyle : public vtkInteractorStyle
{
public:
	static vInteractorStyle* New()
	{
		return new vInteractorStyle();
	}

	vtkTypeMacro(vInteractorStyle,vtkInteractorStyle);
	enum InteractorStyleMode{
		_WINDOW_LEVEL = 0,
		_RESLICE,
		PAN,
		DOLLY
	};

public:
	void         SetInteractionMode(InteractorStyleMode mode);

	virtual void OnMouseMove();
	virtual void OnLeftButtonDown();
	virtual void OnLeftButtonUp();

	virtual void Pan();
	virtual void Dolly();
	void         WindowLevel();
	void         Reslice();

	void         StartWindowLevel();
	void         EndWindowLevel();

	void         StartReslice();
	void         EndReslice();

	void         SetLookupTable(vtkSmartPointer<vtkLookupTable> lut);
	void         SetCornerAnnotation(vtkSmartPointer<vtkCornerAnnotation> corner) {m_CornerText = corner;};
	void		 SetImageData(vtkSmartPointer<vtkImageData> data) {m_Data = data;};
	void         SetRender(vtkSmartPointer<vtkRenderer> render) {m_Render = render;};
	void         SetView(vView2D* view) {m_View = view;};
	void         OnMouseMoveDeal(int x,int y);

	void         SetWindowLevel(double window, double level);

protected:
	vInteractorStyle(void);
	~vInteractorStyle(void);

	virtual void OnMouseWheelBackward(){};
	virtual void OnMouseWheelForward(){};

	InteractorStyleMode                   m_StyleMode;
	double                                MotionFactor;
	double                                m_WinLevel[2];
	double                                m_StartPosition[2];

	vtkSmartPointer<vtkWindowLevelLookupTable>       m_LookupTable;
	vtkSmartPointer<vtkCornerAnnotation>			 m_CornerText;
	vtkSmartPointer<vtkImageData>                    m_Data;
	vtkSmartPointer<vtkRenderer>					 m_Render;

	vView2D*                        m_View;

	void         Dolly(double factor);

private:
	vInteractorStyle(const vInteractorStyle&);  // Not implemented.
	void operator=(const vInteractorStyle&);  // Not implemented.
};
#endif // vInteractorStyle_h__

