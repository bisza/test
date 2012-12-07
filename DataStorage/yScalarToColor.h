#ifndef yScalerToColorMap_h__
#define yScalerToColorMap_h__

#include "vector"
#include "itkObject.h"
#include "itkObjectFactory.h"
#include "yDataStorageDll.h"

class yScalarToColor :public itk::Object
{
public:
	typedef yScalarToColor					Self;
	typedef itk::Object                     Superclass;
	typedef itk::SmartPointer<Self>			Pointer;
	typedef itk::SmartPointer<const Self>	ConstPointer;

	itkNewMacro(yScalarToColor);
	itkTypeMacro(yScalarToColor,itk::Object);

public:
	virtual double* GetColorFromScalar(double scaler);
	virtual void	GetColorFromScalar(double scaler,double* &rgb);

	virtual void SetScalerToColor(double scaler,double* rgb);
	virtual void SetScalerToColor(double scaler,double r, double g, double b);

	std::string MapScalerToColorName(double scaler);
	

protected: 
	yScalarToColor(void);
	~yScalarToColor(void);

protected:
	double White [3];
	double LightestGrey [3];
	double LighterGrey [3];
	double LightGrey [3];
	double MediumGrey [3];
	double DarkGrey [3];
	double DarkOchre [3];    
	double MediumOchre [3];
	double BrightOchre [3];
	double SliceYellow [3];
	double LightOchre [3];
	double DarkOrange [3];
	double MediumOrange [3];
	double SliceOrange [3];
	double BrightOrange [3];
	double LightOrange [3];
	double LightestOrange [3];
	double DarkBrown [3];    
	double MediumBrown [3];
	double Brown [3];
	double LightBrown [3];
	double LightestBrown [3];
	double DarkRed [3];
	double MediumRed [3];
	double SliceRed [3];
	double LightRed [3];
	double LightestRed [3];
	double DarkGreyGreen [3];
	double MediumGreyGreen [3];
	double SliceGreen [3];
	double LightGreyGreen [3];
	double LightestGreyGreen [3];
	double DarkGreen [3];
	double MediumGreen [3];
	double Green [3];
	double LightGreen [3];    
	double LightestGreen [3];
	double DarkGreyBlue [3];    
	double MediumGreyBlue [3];
	double GreyBlue [3];
	double SlicerBlue [3];
	double LightGreyBlue [3];    
	double DarkBlue [3];
	double MediumBlue [3];
	double Blue [3]; 

	double LightBlue [3];    
	double LightestBlue [3];
	double ErrorRed [3];
	double SystemBlue [3];
	double WarningYellow [3];
	double Magenta [3];
	double Purple [3];
	double DarkPurple [3];
	double LogoGreyBlue [3];    
	double IGTGrey [3];
	double LogoDarkGrey [3];
	double LogoLightYellow [3];
	double LogoMediumYellow [3];
	double LogoOrange [3];
	double LogoRed [3];

	double NAMICBlue [3];
	double SPLGreen [3];
	double LightestGreyBlue [3];
	double BurntOrange [3];
	double DarkStone [3];
	double MediumCoolStone [3];
	double LightCoolStone [3];
	double LightStone [3];
	double LightestStone [3];
	double Cornsilk2 [3];
	double Cornsilk3 [3];
	double Cornsilk4 [3];
	double Seashell2 [3];
	double Seashell3 [3]; 
	double Seashell4 [3];
	double BrightRed [3];
	double MediumYellow [3];
	double DarkYellow [3];
	double BrightYellow [3];

	std::vector<double *> m_ColorContainer;

protected:
	void SetColor (double *color, double r, double g, double b);
	void DefineDefaultColorPalette();

private:
	yScalarToColor(const yScalarToColor&);     //Not implemented
	void operator=(const yScalarToColor&); 
};

#endif // yScalerToColorMap_h__



