#ifndef vPrgressEvent_h__
#define vPrgressEvent_h__

#include "vtkCommand.h"
#include "itkCommand.h"
#include "itkProcessObject.h"
#include "vStatusBarNotify.h"
#include "vtkAlgorithm.h"

class vReadProgress: public vtkCommand
{
public:
	static vReadProgress* New()
	{
		return new vReadProgress();
	}

public:
	void SetProgressBar(vStatusBarNotify* progress){m_Progress = progress;};

protected:
	vReadProgress():m_Progress(NULL){};
	~vReadProgress(){};

	virtual void Execute(vtkObject *caller, unsigned long eventId, void *callData)
	{
		double progressD;
		int progressI;
		vtkAlgorithm* algorithm =  reinterpret_cast<vtkAlgorithm*>(caller); 
		if (eventId == ProgressEvent)
		{
			progressD = algorithm->GetProgress(); 
			progressI = static_cast<int>(progressD*100); 
			if (m_Progress)
			{
				m_Progress->ProgressNotify(progressI);
			}
		}
	};

	vStatusBarNotify* m_Progress;
};

class vReadProgressItk : public itk::Command
{
public:
	typedef vReadProgressItk				Self;
	typedef itk::Command		            Superclass;
	typedef itk::SmartPointer<Self>			Pointer;
	typedef itk::SmartPointer<const Self>	ConstPointer;

	itkTypeMacro(yItkProgressEvent,itk::Command);
	itkNewMacro( Self );

public:
	void SetProgressBar(vStatusBarNotify* progress){m_Progress = progress;};

protected:
	vReadProgressItk():m_Progress(NULL){};
	~vReadProgressItk(){};

private:
	void Execute(itk::Object *caller, const itk::EventObject & event )
	{
		itk::ProcessObject *processObject = (itk::ProcessObject*)caller;
		if (typeid(event) == typeid(itk::ProgressEvent)) {
			int progress = (int)100.0 * processObject->GetProgress();
			m_Progress->ProgressNotify(progress);
		}
	};
	void Execute(const itk::Object *caller, const itk::EventObject & event )
	{
		itk::ProcessObject *processObject = (itk::ProcessObject*)caller;
		if (typeid(event) == typeid(itk::ProgressEvent)) {
			int progress = (int)100.0 * processObject->GetProgress();
			m_Progress->ProgressNotify(progress);
		}
	};
	vStatusBarNotify* m_Progress;

private:
	vReadProgressItk(const Self&);
	void operator=(const Self&);

};
#endif // vPrgressEvent_h__