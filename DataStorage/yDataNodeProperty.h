#ifndef yDataNodeProperty_h__
#define yDataNodeProperty_h__

#include "itkObject.h"
#include "itkObjectFactory.h"
#include "yDataStorageConfigure.h"

class  yDataNodeProperty : public itk::Object
{
public:
	typedef yDataNodeProperty				Self;
	typedef itk::Object                     Superclass;
	typedef itk::SmartPointer<Self>			Pointer;
	typedef itk::SmartPointer<const Self>	ConstPointer;	
	
	itkTypeMacro(yDataNodeProperty,itk::Object);
	itkNewMacro(yDataNodeProperty);

public:
	inline  void			SetPropertyType(PropertyType type){m_PropertyType = type;};
	virtual void			GetPropertyType(PropertyType &type){type = m_PropertyType;};
	virtual PropertyType	GetPropertyType(){return m_PropertyType;};

	inline  void			SetPropertyName(std::string name){m_PropertyName = name;};
	virtual void			GetPropertyName(std::string &name){name = m_PropertyName;};
	virtual std::string		GetPropertyName(){return m_PropertyName;};

	inline  void			SetPropertyValue(std::string value){m_PropertyValue = value;};
	virtual void			GetPropertyValue(std::string &value){value = m_PropertyValue;};
	virtual std::string		GetPropertyValue(){return m_PropertyValue;};

	inline  void			SetPropertyCaption(std::string caption){m_PropertyCaption = caption;};
	virtual void			GetPropertyCaption(std::string &caption){caption = m_PropertyCaption;};
	virtual std::string		GetPropertyCaption(){return m_PropertyCaption;};

	inline  void			SetPropertyUnit(std::string unit){m_PropertyUnit = unit;};
	virtual void			GetPropertyUnit(std::string &unit){unit = m_PropertyUnit;};
	virtual std::string		GetPropertyUnit(){return m_PropertyUnit;};

	inline  void			SetPropertyModifiable(std::string unit){m_PropertyUnit = unit;};
	virtual void			GetPropertyModifiable(std::string &unit){unit = m_PropertyUnit;};
	virtual std::string		GetPropertyModifiable(){return m_PropertyUnit;};

protected:
	yDataNodeProperty():m_PropertyType(OTHER_),m_PropertyName(""),m_PropertyValue(""),m_PropertyUnit(""),m_PropertyCaption(""),m_Modifiable("false"){};
	~yDataNodeProperty(){};

private:
	PropertyType m_PropertyType;
	std::string	 m_PropertyName;
	std::string  m_PropertyValue;
	std::string  m_PropertyUnit;
	std::string  m_PropertyCaption;
	std::string  m_Modifiable;

private:
	yDataNodeProperty(const Self&);
	void operator=(const Self&);
};

#endif // yDataNodeProperty_h__
