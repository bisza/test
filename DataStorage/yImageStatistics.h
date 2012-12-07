#ifndef yImageStatistics_h__
#define yImageStatistics_h__

#include "itkObject.h"
#include "vtkImageData.h"

#include "yDataStorageDll.h"
#include "yDataNode.h"

class Y_DATA_STORAGE_EXPORT yImageStatistics : public itk::Object
{
public:
	typedef yImageStatistics				Self;
	typedef itk::Object                     Superclass;
	typedef itk::SmartPointer<Self>			Pointer;
	typedef itk::SmartPointer<const Self>	ConstPointer;

	itkNewMacro(yImageStatistics);
	itkTypeMacro(yImageStatistics,itk::Object);

public:
	void SetDataNode(yDataNode::Pointer node);
	void Update();
	std::map<int,int> GetOutput();

	int	GetYMax();
	int GetXMin();
	int GetXMax();

protected:
	yImageStatistics();
	~yImageStatistics();

protected:
	template<typename T>
	void GetStatistics(int *container, vtkSmartPointer<vtkImageData> data);

private:
	yDataNode::Pointer m_DataNode;
	std::map<int,int> m_Satistics;
	double m_Range[2];
private:
	yImageStatistics(const Self&);
	void operator=(const Self&);

};
#endif // yImageStatistics_h__
