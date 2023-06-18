#pragma once
#include <qstatusbar.h>
#include <qprogressbar.h>
#include <qlabel.h>
#include <qdebug.h>

//#define statusInit() StatusBar::init()
#define statusInit(sBar) StatusBar::init(sBar);
#define statusWriteMessage(message) StatusBar::setMessage(message);
#define statusWriteTimedMessage(message) StatusBar::setMessage(message, 10000);
#define statusWriteLabel(label) StatusBar::setLabel(label);
#define statusProgressValue(value) StatusBar::setProgressValue(value);
#define statusProgressMinMax(minValue, maxValue) StatusBar::setProgressMinMax(minValue, maxValue);
#define statusProgressContinuous() StatusBar::setProgressMinMax(0,0);


/// @brief Creates messages for status bar
class StatusBar
{
public:
	StatusBar();
	~StatusBar();

	static void init(QStatusBar* _sBar) {
		instance.sBar = _sBar;

		instance.pBar = new QProgressBar();
		instance.sBar->addPermanentWidget(instance.pBar);
		instance.pBar->hide();

		instance.modeLabels = new QLabel();
		instance.modeLabels->setText("");		//Blank
		instance.sBar->addPermanentWidget(instance.modeLabels);
	}
	static QStatusBar* getQStatusBar();
	static void setMessage(QString str, int millisec = 0) {
		instance.sBar->showMessage(str, millisec);
	}
	static void setProgressValue(int value) {
		if (value >= instance.pBar->maximum()) instance.pBar->show();
		else instance.pBar->hide();

		instance.pBar->setValue(value);
	}
	static void setProgressMinMax(int minValue, int maxValue) {		
		instance.pBar->setMinimum(minValue);
		instance.pBar->setMaximum(maxValue);		
		instance.pBar->show();
	}
	static void setLabel(QString str) {
		instance.modeLabels->setText(str);
	}

private:
	QStatusBar* sBar;
	QProgressBar* pBar;
	QLabel* modeLabels;
	static StatusBar instance;
};
