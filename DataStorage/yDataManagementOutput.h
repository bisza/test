#ifndef yDataManagementOutput_h__
#define yDataManagementOutput_h__

#include "yDataManagement.h"
#include "itkObject.h"

class yDataManagementOutput : public itk::Object
{
public:
	typedef yDataManagementOutput			Self;
	typedef itk::Object                     Superclass;
	typedef itk::SmartPointer<Self>			Pointer;
	typedef itk::SmartPointer<const Self>	ConstPointer;

	itkNewMacro(yDataManagementOutput);
	itkTypeMacro(yDataManagementOutput,itk::Object);

public:
	virtual void SetProperty(){};
	virtual void GetProperty(){};

	virtual void* GetData(){};
	virtual void SetData(void* data){};

protected:
	yDataManagementOutput(){};
	~yDataManagementOutput(){};
private:
	yDataManagementOutput(const Self&);
	void operator=(const Self&);
};
#endif // yDataManagementOutput_h__