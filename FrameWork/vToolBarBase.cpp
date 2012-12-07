#include "vToolBarBase.h"
#include <QMessageBox>
#include <QEvent>

vToolBarBase::vToolBarBase(QWidget* parent):QToolBar(parent)
{
	this->setContentsMargins(0,0,0,0);
}

vToolBarBase::~vToolBarBase()
{

}

int vToolBarBase::ParameterCheck()
{
	if(!m_Views)
	{ 
		return 0;
	}

	if(m_Dm.IsNull())
	{
		return 0;
	}

	if (!m_Status)
	{
		return 0;
	}

	return 1;
}

void vToolBarBase::changeEvent(QEvent *event)
{
	QWidget::changeEvent(event);  
	switch (event->type())  {  
	case QEvent::LanguageChange:
		this->translateLanguage();
		break;  
	default:  
		break;  
	}  
}