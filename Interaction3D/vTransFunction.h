#ifndef vTransFunction_h__
#define vTransFunction_h__

#include <QObject>
#include "vtkColorTransferFunction.h"
#include "vtkPiecewiseFunction.h"
#include "vtkSmartPointer.h"

class vTransFunction : public QObject
{
	Q_OBJECT
public:
	vTransFunction(QObject* parent);
	~vTransFunction();

	vtkSmartPointer<vtkColorTransferFunction> GetColorTunction(){return m_ColorFunction;};
	vtkSmartPointer<vtkPiecewiseFunction> GetGradientFunction(){return m_GradientFunction;};
	vtkSmartPointer<vtkPiecewiseFunction> GetOpacityFunction(){return m_OpacityFucniton;};

protected:
	vtkSmartPointer<vtkColorTransferFunction> m_ColorFunction;
	vtkSmartPointer<vtkPiecewiseFunction>     m_GradientFunction;
	vtkSmartPointer<vtkPiecewiseFunction>     m_OpacityFucniton;
};
#endif // vTransFunction_h__