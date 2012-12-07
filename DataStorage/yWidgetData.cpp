#include "yWidgetData.h"

yWidgetData::yWidgetData(void)
{
}


yWidgetData::~yWidgetData(void)
{
}

void yWidgetData::AddPoint(double *point)
{
	std::vector<double> pointContainer;
	pointContainer.push_back(point[0]);
	pointContainer.push_back(point[1]);
	pointContainer.push_back(point[2]);

	this->m_Points.insert(std::map<int,std::vector<double> >::value_type((int)m_Points.size(),pointContainer));
}

void yWidgetData::AddPoint(double point1, double point2, double point3)
{
	double points[3] = {point1,point2,point3};
	this->AddPoint(points);
}

void yWidgetData::GetPoint(int id, double points[3])
{
	if (id >= (int)m_Points.size() || id < 0)
	{
		return;
	}

	std::vector<double> pointContainer = m_Points.at(id);
	points[0] = pointContainer[0];
	points[1] = pointContainer[1];
	points[2] = pointContainer[2];
}

void yWidgetData::GetSeedPoint(int id, double points[3])
{
	if (id >= (int)(m_Points.size()/2) || id < 0)
	{
		return;
	}

	std::vector<double> pointContainer = m_Points.at(2*id + 1);
	points[0] = pointContainer[0];
	points[1] = pointContainer[1];
	points[2] = pointContainer[2];
}

void yWidgetData::RemoveSeedPoint(int id)
{
	if (id >= (int)(m_Points.size()/2) || id < 0)
	{
		return;
	}
	
	std::map<int,std::vector<double> > points;
	points.clear();
	std::vector<double> currentPoint;

	for (int i = 0;i < (int)(m_Points.size());i++)
	{
		if (i < 2*id || i > 2*id+1)
		{
			currentPoint.clear();
			currentPoint.push_back(m_Points[i][0]);
			currentPoint.push_back(m_Points[i][1]);
			currentPoint.push_back(m_Points[i][2]);
			points[points.size()] = currentPoint;
		}
	}
	m_Points.clear();

	for (int j = 0;j < (int)(points.size());j++)
	{
		currentPoint.clear();
		currentPoint.push_back(points[j][0]);
		currentPoint.push_back(points[j][1]);
		currentPoint.push_back(points[j][2]);
		m_Points[j] = currentPoint;
	}
}

void yWidgetData::InsertPoint(int id, double* point)
{
	std::vector<double> pointContainer;
	pointContainer.push_back(point[0]);
	pointContainer.push_back(point[1]);
	pointContainer.push_back(point[2]);

	this->m_Points[id] = pointContainer;
}

void yWidgetData::InsertPoint(int id, double point1, double point2, double point3)
{
	double point[3] = {point1,point2,point3};
	this->InsertPoint(id,point);
}
