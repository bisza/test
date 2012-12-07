#include "yDataNodeBase.h"
#include "yDataStorageConfigure.h"

yDataNodeBase::yDataNodeBase()
{
	m_PropertyList.clear();
}

yDataNodeBase::~yDataNodeBase()
{

}

bool yDataNodeBase::RemoveProperty(std::string key)
{
	PropertyListType::iterator it = m_PropertyList.begin();
	while (it != m_PropertyList.end())
	{
		if (key == (*it)->GetPropertyName())
		{
			m_PropertyList.erase(it);	
			return true;
		}
		it++;
	}
	return false;
}

void yDataNodeBase::RemoveAllProperty()
{
	m_PropertyList.clear();
}

void yDataNodeBase::InsertProperty(std::string key,PropertyType type, std::string arg_value, std::string is_modify)
{
	PropertyListType::iterator it = m_PropertyList.begin();
	while (it != m_PropertyList.end())
	{
		if (key == (*it)->GetPropertyName())
		{
			(*it)->SetPropertyType(type);
			(*it)->SetPropertyValue(arg_value);
			(*it)->SetPropertyModifiable(is_modify);

			if (key != "node_modified")
			{
				this->InsertProperty("node_modified",STRING_,"true","false");
			} 
			return;
		}
		it++;
	}

	yDataNodeProperty::Pointer tempProperty = yDataNodeProperty::New();
	tempProperty->SetPropertyName(key);
	tempProperty->SetPropertyType(type);
	tempProperty->SetPropertyValue(arg_value);
	tempProperty->SetPropertyModifiable(is_modify);
	m_PropertyList.push_back(tempProperty);	

	this->InsertProperty("node_modified",STRING_,"true","false");
	return;
}

PropertyType yDataNodeBase::GetPropertyType(std::string key)
{
	PropertyListType::iterator it = m_PropertyList.begin();
	while (it != m_PropertyList.end())
	{
		if (key == (*it)->GetPropertyName())
		{
			return (*it)->GetPropertyType();
		}
		it++;
	}

	return OTHER_;
}

std::string yDataNodeBase::GetPropertyValue(std::string key)
{
	PropertyListType::iterator it = m_PropertyList.begin();
	while (it != m_PropertyList.end())
	{
		if (key == (*it)->GetPropertyName())
		{
			return (*it)->GetPropertyValue();
		}
		it++;
	}

	return "";
}

void yDataNodeBase::InsertProperty(yDataNodeProperty::Pointer property)
{
	std::string tempStr = property->GetPropertyName();
	this->RemoveProperty(tempStr);
	m_PropertyList.push_back(property);
	return;
}

bool yDataNodeBase::CheckPropertyExist(std::string key)
{
	PropertyListType::iterator it = m_PropertyList.begin();
	while (it != m_PropertyList.end())
	{
		if (key == (*it)->GetPropertyName())
		{
			return true;
		}
		it++;
	}
	return false;
}

yDataNodeProperty::Pointer yDataNodeBase::GetProperty(std::string key)
{
	PropertyListType::iterator it = m_PropertyList.begin();
	while (it != m_PropertyList.end())
	{
		if (key == (*it)->GetPropertyName())
		{
			return *it;
		}
		it++;
	}

	return NULL;
}


void yDataNodeBase::InsertProperty(std::string key, std::string arg,std::string is_modify)
{
	this->InsertProperty(key,STRING_,arg);
}

void yDataNodeBase::InsertProperty(std::string key, bool arg,std::string is_modify)
{
	std::string tempStr = ConvertToString(arg);
	this->InsertProperty(key,BOOL_,tempStr,is_modify);
}

void yDataNodeBase::InsertProperty(std::string key, double arg,std::string is_modify)
{
	std::string tempStr = ConvertToString(arg);
	this->InsertProperty(key,DOUBLE_,tempStr);
}

void yDataNodeBase::InsertProperty(std::string key, double arg1,double arg2, double arg3,std::string is_modify)
{
	std::string tempStr;
	std::string tempStr1;
	std::string tempStr2;
	std::string tempStr3;

	tempStr1 = ConvertToString(arg1);
	tempStr2 = ConvertToString(arg2);
	tempStr3 = ConvertToString(arg3);

	tempStr = tempStr1 + "," + tempStr2 + "," + tempStr3;

	this->InsertProperty(key,COLOR_,tempStr,is_modify);
}

void yDataNodeBase::InsertProperty(std::string key, int arg1, int arg2,int arg3, int arg4, int arg5,int arg6,std::string is_modify)
{
	std::string tempStr;
	std::string tempStr1;
	std::string tempStr2;
	std::string tempStr3;
	std::string tempStr4;
	std::string tempStr5;
	std::string tempStr6;

	tempStr1 = ConvertToString(arg1);
	tempStr2 = ConvertToString(arg2);
	tempStr3 = ConvertToString(arg3);
	tempStr4 = ConvertToString(arg4);
	tempStr5 = ConvertToString(arg5);
	tempStr6 = ConvertToString(arg6);

	tempStr = tempStr1 + "," + tempStr2 + "," + tempStr3 + ","+ tempStr4 + "," + tempStr5 + "," + tempStr6;
	this->InsertProperty(key,tempStr,is_modify);
}

void yDataNodeBase::InsertProperty(std::string key, int arg,std::string is_modify)
{
	std::string tempStr = ConvertToString(arg);
	this->InsertProperty(key, INT_, tempStr,is_modify);
}

void yDataNodeBase::InsertProperty(std::string key, DataNodeType arg,std::string is_modify)
{
	std::string tempStr = ConvertToString(arg);
	this->InsertProperty(key, INT_, tempStr,is_modify);
}

void yDataNodeBase::InsertProperty(std::string key, unsigned long arg,std::string is_modify)
{
	std::string tempStr = ConvertToString(arg);
	this->InsertProperty(key, UNSIGNED_LONG, tempStr,is_modify);
}

void yDataNodeBase::InsertStringProperty(std::string key, std::string arg,std::string is_modify)
{
	this->InsertProperty(key,arg,is_modify);
}

void yDataNodeBase::InsertBoolProperty(std::string key, bool arg,std::string is_modify)
{
	this->InsertProperty(key,arg,is_modify);
}

void yDataNodeBase::InsertIntProperty(std::string key, int arg,std::string is_modify)
{
	this->InsertProperty(key,arg,is_modify);
}

void yDataNodeBase::InsertColorProperty(std::string key, double value_r, double value_g, double value_b,std::string is_modify)
{
	this->InsertProperty(key,value_r,value_g,value_b,is_modify);
}

void yDataNodeBase::InsertDoubleProperty(std::string key, double arg,std::string is_modify)
{
	this->InsertProperty(key,arg,is_modify);
}

void yDataNodeBase::InsertUnsignedLongProperty(std::string key, unsigned long arg,std::string is_modify)
{
	this->InsertProperty(key,arg,is_modify);
}

void yDataNodeBase::GetColorProperty(std::string key, double &value_r, double &value_g, double &value_b)
{
	std::string tempStr;

	PropertyListType::iterator it = m_PropertyList.begin();
	while (it != m_PropertyList.end())
	{
		if (key == (*it)->GetPropertyName())
		{
			tempStr = (*it)->GetPropertyValue();
			break;
		}
		it++;
	}

	if (it == m_PropertyList.end())
	{
		return;
	}

	std::string tempSubStr;
	double		returnDouble[3];
	double		tempDouble = 0;
	int			count = 0;

	returnDouble[0] = 1;
	returnDouble[1] = 0;
	returnDouble[2] = 0;

	//Find out how many double variables exist in this string 
	std::stringstream ss(tempStr);
	while(getline(ss,tempSubStr,','))
	{
		ConvertFromString(tempDouble,tempSubStr);
		returnDouble[count] = tempDouble;
		count++;
	}

	value_r = returnDouble[0];
	value_g = returnDouble[1];
	value_b = returnDouble[2];
}

void yDataNodeBase::GetBorderProperty(std::string key, int &arg1, int &arg2,int &arg3, int &arg4, int &arg5,int &arg6)
{
	std::string tempStr;

	PropertyListType::iterator it = m_PropertyList.begin();
	while (it != m_PropertyList.end())
	{
		if (key == (*it)->GetPropertyName())
		{
			tempStr = (*it)->GetPropertyValue();
			break;
		}
		it++;
	}

	if (it == m_PropertyList.end())
	{
		return;
	}

	std::string tempSubStr;
	double		returnDouble[6];
	double		tempDouble = 0;
	int			count = 0;

	returnDouble[0] = 0;
	returnDouble[1] = 0;
	returnDouble[2] = 0;
	returnDouble[3] = 0;
	returnDouble[4] = 0;
	returnDouble[5] = 0;

	//Find out how many double variables exist in this string 
	std::stringstream ss(tempStr);
	while(getline(ss,tempSubStr,','))
	{
		ConvertFromString(tempDouble,tempSubStr);
		returnDouble[count] = tempDouble;
		count++;
	}

	arg1 = returnDouble[0];
	arg2 = returnDouble[1];
	arg3 = returnDouble[2];
	arg4 = returnDouble[3];
	arg5 = returnDouble[4];
	arg6 = returnDouble[5];
}

void  yDataNodeBase::InsertNodeTypeProperty(std::string key, DataNodeType arg,std::string is_modify)
{
	int tempInt = static_cast<int>(arg);
	this->InsertIntProperty(key,tempInt,is_modify);
}

void yDataNodeBase::GetStringProperty(std::string key, std::string &arg)
{
	PropertyListType::iterator it = m_PropertyList.begin();

	while (it != m_PropertyList.end())
	{
		if (key == (*it)->GetPropertyName())
		{
			arg = (*it)->GetPropertyValue();
		}
		it++;
	}
}

std::string yDataNodeBase::GetStringProperty(std::string key)
{
	std::string tempStr = "";

	this->GetStringProperty(key,tempStr);

	return tempStr;
}

void yDataNodeBase::GetUnsignedLongProperty(std::string key, unsigned long &arg)
{
	this->GetTemplateProperty(key,arg);
}

unsigned long yDataNodeBase::GetUnsignedLongProperty(std::string key)
{
	unsigned long tempUnsignedLong = 0;
	this->GetTemplateProperty(key,tempUnsignedLong);
	return tempUnsignedLong;
}

void yDataNodeBase::GetBoolProperty(std::string key, bool &arg)
{
	this->GetTemplateProperty(key,arg);
}

bool yDataNodeBase::GetBoolProperty(std::string key)
{
	bool tempBool = true;
	this->GetBoolProperty(key,tempBool);
	return tempBool;
}

void yDataNodeBase::GetDoubleProperty(std::string key, double &arg)
{
	this->GetTemplateProperty(key,arg);
}

double yDataNodeBase::GetDoubleProperty(std::string key)
{
	double tempDouble  = 0;
	this->GetDoubleProperty(key, tempDouble);
	return tempDouble;
}

void yDataNodeBase::GetIntProperty(std::string key,int &arg)
{
	this->GetTemplateProperty(key,arg);
}

int yDataNodeBase::GetIntProperty(std::string key)
{
	int tempInt = -1;
	this->GetIntProperty(key,tempInt);
	return tempInt;
}

void yDataNodeBase::GetNodeTypeProperty(std::string key, DataNodeType &arg)
{
	int tempInt = -1;
	this->GetIntProperty(key,tempInt);

	arg = static_cast<DataNodeType>(tempInt);
}

DataNodeType yDataNodeBase::GetNodeTypeProperty(std::string key)
{
	DataNodeType tempNodeType = WITHOUT_DATA;

	this->GetNodeTypeProperty(key,tempNodeType);
	return tempNodeType;
}


template<typename T>
void yDataNodeBase::GetTemplateProperty(std::string key, T &value)
{
	PropertyListType::iterator it = m_PropertyList.begin();
	while (it != m_PropertyList.end())
	{
		if (key == (*it)->GetPropertyName())
		{
			std::string tempBool = (*it)->GetPropertyValue();
			ConvertFromString(value,tempBool);
			return;
		}
		it++;
	}
	return;
}
