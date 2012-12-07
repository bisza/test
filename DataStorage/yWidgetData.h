#ifndef yWidgetData_h__
#define yWidgetData_h__

#include "yDataStorageDll.h"
#include "vtkDataObject.h"
#include "vtkPolyData.h"
#include <map>
#include <vector>

class  Y_DATA_STORAGE_EXPORT yWidgetData : public vtkDataObject
{
public:
	vtkTypeMacro(yWidgetData,vtkPolyData);
	static yWidgetData* New()
	{
		return new yWidgetData();
	}

public:
	std::map<int,std::vector<double> >	GetPoints(){return m_Points;};

	virtual void			AddPoint(double *point);
	virtual void			AddPoint(double point1, double point2, double point3);
			
	virtual void			GetPoint(int id, double points[3]);

	virtual double          GetScalars(int id){return m_Scalars[id];};
	virtual void            SetScalars(std::map<int,double> scalars){m_Scalars = scalars;};
	virtual void            GetSeedPoint(int id, double points[3]);
	virtual void            RemoveSeedPoint(int id);

	virtual void			InsertPoint(int id, double* point);
	virtual void			InsertPoint(int id, double point1, double point2, double point3);

	inline void				ClearPoints() {m_Points.clear();};

	inline int				GetNumberOfPoints(){return int(m_Points.size());}
protected:
	yWidgetData(void);
	~yWidgetData(void);

private:
	std::map<int,std::vector<double> >  m_Points;
	std::map<int,double>               m_Scalars;
	double m_TempPoint[3];
private:
	yWidgetData(const yWidgetData&);
	yWidgetData& operator=(const yWidgetData&);
};

#endif // yWidgetData_h__
