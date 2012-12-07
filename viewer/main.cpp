#include <QApplication>
#include "vMainWindow.h"
#include <QPlastiqueStyle>
#include <QTextCodec>

int main(int argc, char** argv)
{
	QApplication app(argc, argv);

	QApplication::addLibraryPath("./plugins");
	QTextCodec *codec = QTextCodec::codecForName("gb2312");
	QTextCodec::setCodecForLocale(codec);
	QTextCodec::setCodecForTr(codec);
	QTextCodec::setCodecForCStrings(codec);

	mainWindow window; 
	window.showMaximized();
	window.setWindowFlags(Qt::FramelessWindowHint);
	window.show();	
	app.exec();
}
