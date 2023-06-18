#include "StatusBar.h"
//==== Static ====//

StatusBar StatusBar::instance;


//==== Function definitions ====//
StatusBar::StatusBar()
{
	qDebug() << "Status bar initiated";
}

StatusBar::~StatusBar()
{
	// Note: sBar and pBar are deleted by MainWindow
}
