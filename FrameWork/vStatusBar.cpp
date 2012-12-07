#include "vStatusBar.h"

vStatusBar::vStatusBar(QWidget* parent /* = 0 */):QStatusBar(parent)
{
	m_ProgressBar = new QProgressBar(this);
	m_ProgressBar->setMinimum(0);
	m_ProgressBar->setMaximum(100);
	this->addPermanentWidget(m_ProgressBar);

	this->m_ProgressBar->hide();
	this->setMaximumHeight(22);
}

vStatusBar::~vStatusBar()
{

}


void vStatusBar::SetProgress(int progress)
{
	if (progress > 0 && progress < 100)
	{
		if (!m_ProgressBar->isVisible())
		{
			m_ProgressBar->show();
		}
	}

	if (progress == 100)
	{
		m_ProgressBar->setValue(progress);
		m_ProgressBar->hide();
		return;
	}

	m_ProgressBar->setValue(progress);
}