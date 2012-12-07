#ifndef vInteraction3D_h__
#define vInteraction3D_h__

#include "vToolBarBase.h"
#include "vtkVolume.h"

class QAction;
class vTransFunction;

class vInteraction3D : public vToolBarBase
{
	Q_OBJECT
public:
	vInteraction3D(QWidget* parent  = 0);
	~vInteraction3D();

	public slots:
		virtual void OnDataChanged();
		virtual void OnLayoutChanged();
		virtual void OnFocusChanged();

	protected slots:
		void OnText();
		void OnVolumeRendering();
		void OnRealColor();

		void OnVolumeProperty();

protected:
	void ReadDefaultProperty();

	virtual void translateLanguage();

protected:
	int lastRootIndex;

private:
	QAction* m_Text;
	QAction* m_VolumeRendering;
	QAction* m_RealColor;
	vtkSmartPointer<vtkVolume> m_Volume;
	std::map<std::string,vTransFunction*> m_Trans;
	QAction* m_DisableAction;

};
#endif // vInteraction3D_h__