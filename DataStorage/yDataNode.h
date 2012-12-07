/*****************************************************************************
** File			: yDataNode.h
** CopyRight(c) : Yorktal
** Writer		: ZhangLei
** Time			: 2011.2.13
** Description	: This class inherited from yDataNodeBase, which is a interface
** with Controller. And this class is the basic unit in data storage, it storage 
** the property and data itself..
** Vesion		: 1.1
** Info			: Modified this class as subclass of itkobject to use the 
** smart pointer (2010.11.5).
*****************************************************************************/
#ifndef YDATANODE_H
#define YDATANODE_H

#include "itkObject.h"
#include "itkObjectFactory.h"

#include "vtkObject.h"
#include "vtkSmartPointer.h"
#include "yDataNodeBase.h"


class yDataNode : public yDataNodeBase
{
public:
	typedef yDataNode						Self;
	typedef yDataNodeBase                   Superclass;
	typedef itk::SmartPointer<Self>			Pointer;
	typedef itk::SmartPointer<const Self>	ConstPointer;

	itkNewMacro( Self );
	itkTypeMacro(yDataNode,yDataNodeBase);

public:
	//General property
	void			SetDataNodeType(DataNodeType node_type);
	DataNodeType	GetDataNodeType();

	// Set(Get) vtkDataSet
	virtual void	SetData(vtkSmartPointer<vtkObject> data);
	virtual void	GetData(vtkSmartPointer<vtkObject> &data){data = m_VtkData;};
	virtual vtkSmartPointer<vtkObject> GetData(){return m_VtkData;};


protected:
	yDataNode();
	 ~yDataNode();

private:
	vtkSmartPointer<vtkObject>	m_VtkData;

private:
	yDataNode(const Self&);
	void operator=(const Self&);
};

#endif