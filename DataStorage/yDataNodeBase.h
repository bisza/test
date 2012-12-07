/*****************************************************************************
** File			: yDataNodeBase.h
** CopyRight(c) : Yorktal
** Writer		: ZhangLei
** Time			: 2011.1.26
** Description	: Data node base, deal with property insert and remove. the property 
** exist in this class and will inherit by any data nodes.
** Vesion		: 1.0
** Info			: None
*****************************************************************************/
#ifndef yDataNodeBase_h__
#define yDataNodeBase_h__


#include "itkObject.h"
#include "yDataNodeProperty.h"

class yDataNodeBase : public itk::Object
{
public:
	typedef yDataNodeBase					Self;
	typedef itk::Object                     Superclass;
	typedef itk::SmartPointer<Self>			Pointer;
	typedef itk::SmartPointer<const Self>	ConstPointer;

	itkTypeMacro(yDataNode,itk::Object);

	//Use map to store the property
	typedef std::list<yDataNodeProperty::Pointer> PropertyListType;
	typedef bool PropertyModifiedEventsType;

public:
	bool					RemoveProperty(std::string key);
	void					RemoveAllProperty();
	bool					CheckPropertyExist(std::string key);
	PropertyListType		GetPropertyList(){return m_PropertyList;};

	virtual void					InsertProperty(std::string key,	PropertyType type, std::string arg_value,std::string is_modify = "true");
	virtual	void					InsertProperty(yDataNodeProperty::Pointer proerty);
	PropertyType					GetPropertyType(std::string key);
	std::string						GetPropertyValue(std::string key);
	yDataNodeProperty::Pointer		GetProperty(std::string key);

	virtual void			InsertProperty(std::string key, int arg,std::string is_modify = "true");
	virtual void			InsertProperty(std::string key, std::string arg,std::string is_modify = "true");
	virtual void			InsertProperty(std::string key, bool arg,std::string is_modify = "true");
	virtual void			InsertProperty(std::string key, double arg,std::string is_modify = "true");
	virtual void			InsertProperty(std::string key, double arg1, double arg2,double arg3,std::string is_modify = "true");
	virtual void            InsertProperty(std::string key, int arg1, int arg2,int arg3, int arg4, int arg5,int arg6,std::string is_modify = "true");
	virtual	void			InsertProperty(std::string key, unsigned long arg,std::string is_modify = "true");
	virtual void			InsertProperty(std::string key, DataNodeType arg,std::string is_modify = "true");

	void					InsertStringProperty(std::string key, std::string arg,std::string is_modify = "true");
	void					InsertIntProperty(std::string key, int arg,std::string is_modify = "true");
	void					InsertBoolProperty(std::string key, bool arg,std::string is_modify = "true");
	virtual void			InsertDoubleProperty(std::string key, double arg,std::string is_modify = "true");
	virtual void			InsertColorProperty(std::string key, double value_r, double value_g, double value_b,std::string is_modify = "true");
	void					InsertNodeTypeProperty(std::string key, DataNodeType arg,std::string is_modify = "true");
	void					InsertUnsignedLongProperty(std::string key, unsigned long arg,std::string is_modify = "true");

	virtual void			GetStringProperty(std::string key, std::string &arg);
	virtual std::string		GetStringProperty(std::string key);

	virtual void			GetBoolProperty(std::string key, bool &arg);
	virtual bool			GetBoolProperty(std::string key);

	virtual void			GetDoubleProperty(std::string key, double &arg);
	virtual double			GetDoubleProperty(std::string key);
	virtual void			GetColorProperty(std::string key, double &value_r, double &value_g, double &value_b);
	virtual void            GetBorderProperty(std::string key, int &arg1, int &arg2,int &arg3, int &arg4, int &arg5,int &arg6);

	virtual void			GetUnsignedLongProperty(std::string key, unsigned long &arg);
	virtual unsigned long	GetUnsignedLongProperty(std::string key);

	virtual void			GetNodeTypeProperty(std::string key,DataNodeType &arg);
	virtual DataNodeType	GetNodeTypeProperty(std::string key);

	virtual void			GetIntProperty(std::string key,int &arg);
	virtual int				GetIntProperty(std::string key);

protected:
	yDataNodeBase();
	~yDataNodeBase();

protected:	
	template<typename T>
	void GetTemplateProperty(std::string key, T &value);

protected:
	PropertyListType			m_PropertyList;

private:
	yDataNodeBase(const Self&);
	void operator=(const Self&);
};
#endif // yDataNodeBase_h__