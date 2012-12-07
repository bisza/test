#ifndef vModelLabel_h__
#define vModelLabel_h__

#include "vtkSmartPointer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "vtkTextActor.h"
#include "vtkCallbackCommand.h"
#include "vtkTextMapper.h"

class vModelLabel : public vtkObject
{

public:
	static vModelLabel* New() {  return  new vModelLabel;};

public:
	void SetLabel(std::string label);
	void SetColor(std::string color);
	void SetRenderer(vtkSmartPointer<vtkRenderer> renderer);
	//Œª÷√
	inline void SetNumber(int number){m_Number = number;};

	std::string GetLabel() {return m_Label;};

	void DeleteLabel();

protected:
	vModelLabel(void);
	~vModelLabel(void);

protected:
	static void  ProcessEvent(vtkObject* vtkNotUsed(object),unsigned long event,void* clientdata,void* vtkNotUsed(calldata));
	void         Update();

private:
	vtkSmartPointer<vtkCallbackCommand> m_Callback;
	vtkSmartPointer<vtkRenderer>        m_Renderer;
	vtkSmartPointer<vtkActor2D>         m_Actor;
	vtkSmartPointer<vtkTextMapper>      m_Mapper;

	std::string m_Label;
	double      m_Color[3];
	int         m_Number;

private:
	vModelLabel(const vModelLabel&);     //Not implemented
	void operator=(const vModelLabel&);
};
#endif // vModelLabel_h__

