#ifndef vInteractorStyleRubberBandPick_h__
#define vInteractorStyleRubberBandPick_h__

#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkSmartPointer.h"

class vtkUnsignedCharArray;
class vtkPolyData;
class vViewBase;

class vInteractorStyleRubberBandPick : public vtkInteractorStyleTrackballCamera
{
public:
  static vInteractorStyleRubberBandPick *New();
  vtkTypeMacro(vInteractorStyleRubberBandPick, vtkInteractorStyleTrackballCamera);
  void PrintSelf(ostream& os, vtkIndent indent);

  void SetInteractorStyleMode(int mode){CurrentMode = mode;};
  int  GetInteractorStyleMode(){return CurrentMode;};
  void StartSelect();

  void SetView(vViewBase* view){m_View = view;};

  // Description:
  // Event bindings
  virtual void OnMouseMove();
  virtual void OnLeftButtonDown();
  virtual void OnLeftButtonUp();
  virtual void OnRightButtonDown();
  virtual void OnRightButtonUp();
  virtual void OnMouseWheelBackward();
  virtual void OnMouseWheelForward();
  virtual void OnMiddleButtonDown();
  virtual void OnMiddleButtonUp();

  virtual void OnChar();
  virtual void OnTimer();

  virtual void HighlightProp3D(vtkProp3D *prop3D);

protected:
	vInteractorStyleRubberBandPick();
	~vInteractorStyleRubberBandPick();

	virtual void Pick();
	void RedrawRubberBand();

	vtkUnsignedCharArray *PixelArray;

	int StartPosition[2];
	int EndPosition[2];

	int CurrentMode;
	int Moving;

	vViewBase* m_View;

private:
  vInteractorStyleRubberBandPick(const vInteractorStyleRubberBandPick&);  // Not implemented
  void operator=(const vInteractorStyleRubberBandPick&);  // Not implemented
};

#endif // vInteractorStyleRubberBandPick_h__
