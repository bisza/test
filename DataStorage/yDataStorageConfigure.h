#ifndef yDataStorageConfigure_h__
#define yDataStorageConfigure_h__

#include <string>
#include <ostream>
#include <sstream>
#include <limits>

enum DataNodeType
	{
		IMAGE_DATA = 0,       //Original DICOM Image or other format image
		MASKV_DATA,			  //Mask Node show in data tree view		
		SURFACE_DATA,         //Surface data as model
		DISTANCE_DATA,        //Distance widget data
		ANGLE_DATA,           //Angle widget data
		CONTOUR_DATA,         //Contour widget data
		BIDIMENSIONAL_DATA,	  //Bi-Dimensional widget data
		SEED_DATA,			  //Seed widget data
		CAPTION_DATA,         //Caption widget data 
		MEASURE_DATA,
		WITHOUT_DATA
	};

enum PropertyType
	{
		INT_ = 0,
		BOOL_,
		DOUBLE_,
		STRING_,
		UNSIGNED_LONG,
		COLOR_,
		DATA_TYPE,
		TRANSFER_FUNCTION,
		DICOM_META_DATA,
		OTHER_
	};

template<typename T>
std::string ConvertToString(T value)
{
	int prec = std::numeric_limits<double>::digits10;
	std::stringstream ss;
	ss.precision(prec);

	ss << value;
	return ss.str();
}

template<typename T>
void ConvertFromString(T &value, std::string &s)
{
	std::stringstream ss(s);
	ss >> value;
}

template<typename T>
void ConvertNFromString(T* rgb, std::string arg)
{
	T tempDouble;
	std::string tempSubStr;
	int count = 0;

	std::stringstream ss(arg);
	while(getline(ss,arg,','))
	{
		ConvertFromString(tempDouble,arg);
		rgb[count] = tempDouble;
		count++;
	}
}

template<typename T>
void ConvertNToString(T* value,std::string &str,int n)
{
	std::string tempStr;
	str = "";
	for (int i = 0;i < n;i++)
	{
		tempStr = ConvertToString(value[i]);
		if (str == "")
		{
			str = str + tempStr;
		}
		else
		{
			str = str + "," + tempStr; 
		}
	}
}



#endif // yDataStorageConfigure_h__
