#include "vTransFunction.h"

vTransFunction::vTransFunction(QObject* parent):QObject(parent)
{
	m_ColorFunction = vtkSmartPointer<vtkColorTransferFunction>::New();
	m_GradientFunction = vtkSmartPointer<vtkPiecewiseFunction>::New();
	m_OpacityFucniton = vtkSmartPointer<vtkPiecewiseFunction>::New();
}

vTransFunction::~vTransFunction()
{

}

