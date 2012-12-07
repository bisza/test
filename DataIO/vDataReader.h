#ifndef vDataReader_h__
#define vDataReader_h__

#include "vToolBarBase.h"

class QAction;

class  vDataReader : public vToolBarBase
{
	Q_OBJECT
public:
	vDataReader(QWidget* parent = 0);
	~vDataReader();

public slots:
	virtual void OnLayoutChanged();
	virtual void OnDataChanged();
	virtual void OnFocusChanged();

	void ReadFile();
	void CloseProject();

protected:
	void ReadProjectFile(const QString &str);
	void ReadStlFile(const QString &str);
	void ReadDICOMFiles(const QString &str);

	virtual void translateLanguage();

	QAction* close;
	QAction* file;
};



#endif // vDataReader_h__