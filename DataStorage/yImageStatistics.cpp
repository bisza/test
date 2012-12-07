#include "yImageStatistics.h"
#include "yDataStorageConfigure.h"

yImageStatistics::yImageStatistics()
{

}

yImageStatistics::~yImageStatistics()
{

}

void yImageStatistics::SetDataNode(yDataNode::Pointer node)
{
	if (IMAGE_DATA == node->GetDataNodeType())
	{
		m_DataNode = node;
	}

	vtkSmartPointer<vtkImageData> tempImage = 
		vtkImageData::SafeDownCast(m_DataNode->GetData());
	if (!tempImage)
	{
		itkWarningMacro("No Image Data Exist In This Node!");
	}
}

void yImageStatistics::Update()
{
	if (m_DataNode)
	{
		vtkSmartPointer<vtkImageData> tempImage = 
			vtkImageData::SafeDownCast(m_DataNode->GetData());

		if (tempImage)
		{
			tempImage->GetScalarRange(m_Range);
			int scalarType = tempImage->GetScalarType();

			if (m_Range[0] + 2048 < 0 || m_Range[1] + 2048 > 4096 )
			{
				return;
			}			
			
			int satistics[4096];
			memset(satistics,0,4096*4);

			switch (scalarType)
			{
			case VTK_UNSIGNED_SHORT:
				this->GetStatistics<unsigned short>(satistics,tempImage);
				break;
			case VTK_SHORT:
				this->GetStatistics<short>(satistics,tempImage);
				break;
			case VTK_UNSIGNED_CHAR:
				this->GetStatistics<unsigned char>(satistics,tempImage);
				break;
			}

			for(int i = 0; i < 4096; i++)
			{
				m_Satistics[i - 2048] = satistics[i]; 
			}
		}
	}
}

template<typename T>
void yImageStatistics::GetStatistics(int *container, vtkSmartPointer<vtkImageData> data)
{
	T *pType;
				
	int dim[3];
	data->GetDimensions(dim);
	int size = dim[0]*dim[1]*dim[2];

	pType = (T *)data->GetScalarPointer();
	for (int i = 0; i <= size; i++)
	{
		container[*pType + 2048]++;
		pType++;
	}
}

std::map<int,int> yImageStatistics::GetOutput()
{
	if (m_Satistics.size() == 0)
	{
		this->Update();
	}
	return m_Satistics;
}

int yImageStatistics::GetYMax()
{
	if (m_Satistics.size() != 0)
	{
		std::map<int,int> tempMap;
		std::map<int,int>::iterator it = m_Satistics.begin();
		while(it != m_Satistics.end())
		{
			tempMap[it->second] = 0;
			it++;
		}

		it = tempMap.end();
		--it;
		return it->first;
	}
 	return 0;
}

int yImageStatistics::GetXMin()
{
	return static_cast<int>(m_Range[0]);
}

int yImageStatistics::GetXMax()
{
	return static_cast<int>(m_Range[1]);
}