#include "vDICOMReader.h"
#include "../DataIO/vProgressEvent.h"
#include <itkVTKImageImport.h>
#include <itkVTKImageExport.h>
#include "vtkImageExport.h"
#include "vtkImageImport.h" 
#include "itkImageSeriesReader.h"
#include "itkGDCMImageIO.h"
#include "itkMetaDataObject.h"
#include "itkGDCMSeriesFileNames.h"
#include "itkGDCMImageIO.h"
#include "vtkDICOMImageReader.h"
#include <QDir>
#include <QString>
#include <QMessageBox>

template <typename ITK_Exporter, typename VTK_Importer>
void ConnectPipelines(ITK_Exporter exporter, vtkSmartPointer<VTK_Importer> importer)
{
	importer->SetUpdateInformationCallback(exporter->GetUpdateInformationCallback());
	importer->SetPipelineModifiedCallback(exporter->GetPipelineModifiedCallback());
	importer->SetWholeExtentCallback(exporter->GetWholeExtentCallback());
	importer->SetSpacingCallback(exporter->GetSpacingCallback());
	importer->SetOriginCallback(exporter->GetOriginCallback());
	importer->SetScalarTypeCallback(exporter->GetScalarTypeCallback());
	importer->SetNumberOfComponentsCallback(exporter->GetNumberOfComponentsCallback());
	importer->SetPropagateUpdateExtentCallback(exporter->GetPropagateUpdateExtentCallback());
	importer->SetUpdateDataCallback(exporter->GetUpdateDataCallback());
	importer->SetDataExtentCallback(exporter->GetDataExtentCallback());
	importer->SetBufferPointerCallback(exporter->GetBufferPointerCallback());
	importer->SetCallbackUserData(exporter->GetCallbackUserData());
}

template <typename VTK_Exporter, typename ITK_Importer>
void ConnectPipelines(vtkSmartPointer<VTK_Exporter> exporter, ITK_Importer importer)
{
	importer->SetUpdateInformationCallback(exporter->GetUpdateInformationCallback());
	importer->SetPipelineModifiedCallback(exporter->GetPipelineModifiedCallback());
	importer->SetWholeExtentCallback(exporter->GetWholeExtentCallback());
	importer->SetSpacingCallback(exporter->GetSpacingCallback());
	importer->SetOriginCallback(exporter->GetOriginCallback());
	importer->SetScalarTypeCallback(exporter->GetScalarTypeCallback());
	importer->SetNumberOfComponentsCallback(exporter->GetNumberOfComponentsCallback());
	importer->SetPropagateUpdateExtentCallback(exporter->GetPropagateUpdateExtentCallback());
	importer->SetUpdateDataCallback(exporter->GetUpdateDataCallback());
	importer->SetDataExtentCallback(exporter->GetDataExtentCallback());
	importer->SetBufferPointerCallback(exporter->GetBufferPointerCallback());
	importer->SetCallbackUserData(exporter->GetCallbackUserData());
}


vDICOMReader::vDICOMReader(QObject* parent /* = 0 */):QObject(parent)
{

}

vDICOMReader::~vDICOMReader()
{

}

void vDICOMReader::Update()
{
    m_Status->SendPermentMessage(QObject::tr("正在读取DICOM文件夹..."));
	//Find out the Type of image type
	QDir dicomDir(m_Dir.c_str());
	const QFileInfoList list = dicomDir.entryInfoList();
	int type = -1;

	for (int i = 0; i < list.size(); i++)
	{
		QFileInfo fileInfo = list.at(i);
		vtkSmartPointer<vtkDICOMImageReader> reader = 
			vtkSmartPointer<vtkDICOMImageReader>::New();
		reader->SetFileName(fileInfo.absoluteFilePath().toStdString().c_str());
		reader->SetDataByteOrderToLittleEndian();
		reader->Update();

		int extent[6];
		reader->GetOutput()->GetExtent(extent);

		if(extent[1] >= 0)
		{
			type = reader->GetOutput()->GetScalarType();
			break;
		}
	}

	switch (type)
	{
	case VTK_UNSIGNED_SHORT:
		this->ReadDICOMByItk<unsigned short>();
		break;
	case VTK_SHORT:
		this->ReadDICOMByItk<short>();
		break;
	case VTK_UNSIGNED_CHAR:
		this->ReadDICOMByItk<unsigned char>();
		break;
	}

    m_Status->SendPermentMessage(QObject::tr("读取DICOM文件夹完成！"));
}

template<typename T>
void vDICOMReader::ReadDICOMByItk()
{
	typedef itk::Image<T,3>                   ImageType;
	typedef itk::ImageSeriesReader<ImageType> ReaderType;
	typedef itk::GDCMImageIO                  ImageIOType;
	typedef itk::GDCMSeriesFileNames          NamesGeneratorType;

	ImageIOType::Pointer gdcmIO = ImageIOType::New();
	gdcmIO->SetMaxSizeLoadEntry(0xffff);

	NamesGeneratorType::Pointer namesGenerator = NamesGeneratorType::New();
	namesGenerator->SetInputDirectory(m_Dir.c_str());

	const ReaderType::FileNamesContainer & filenames = 
		namesGenerator->GetInputFileNames();

	vReadProgressItk::Pointer progressEvent = vReadProgressItk::New();
	progressEvent->SetProgressBar(m_Status);

	ReaderType::Pointer reader = ReaderType::New();
	reader->SetImageIO( gdcmIO );
	reader->SetFileNames( filenames );
	reader->AddObserver(itk::ProgressEvent(),progressEvent);

	try
	{
		reader->Update();
	}
	catch (...)
	{
		return;
	}

	typedef itk::VTKImageExport<ImageType> ITKExportType;
	ITKExportType::Pointer itkExport = ITKExportType::New();

	itkExport->SetInput(reader->GetOutput());
	itkExport->Update();

	vtkSmartPointer<vtkImageImport> vtkImport 
		= vtkSmartPointer<vtkImageImport>::New();
	ConnectPipelines( itkExport, vtkImport );
	vtkImport->Update();

	m_Data = vtkImport->GetOutput();

	try
	{
		m_Data = vtkSmartPointer<vtkImageData>::New();
		m_Data->DeepCopy(vtkImport->GetOutput());
		m_Data->Update();
		m_Data->SetOrigin(0,0,0);
	}
	catch (...)
	{
		m_Data = NULL;
		m_Status->SendPermentMessage(tr("读取DICOM文件夹出错！"));
		return;
	}
}

