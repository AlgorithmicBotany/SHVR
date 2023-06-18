#include "SettingScreenshotWidget.h"

SettingScreenshotWidget::SettingScreenshotWidget(ScreenshotParams* _ssParams, QWidget *parent)
	: QWidget(parent, Qt::Window),
	ssParams(_ssParams)
{
	ui.setupUi(this);

	initWidgetList();
	initConnections();	

	//QString filename = fileIO->getFileNameWithDialog_Loading("Group File (*.group)");
	//getFileDirectoryWithDialog
}

SettingScreenshotWidget::~SettingScreenshotWidget()
{
}

void SettingScreenshotWidget::initWidgetList() {
	widgetList.push_back(ui.checkBox_keepAspectRatio);
	widgetList.push_back(ui.spinBox_height);
	widgetList.push_back(ui.spinBox_width);
}

void SettingScreenshotWidget::initConnections() {
	connect(ui.checkBox_keepAspectRatio, SIGNAL(stateChanged(int)), this, SLOT(updateKeepAspectRatio()));
	connect(ui.spinBox_width, SIGNAL(valueChanged(int)), this, SLOT(updateWidth(int)));
	connect(ui.spinBox_height, SIGNAL(valueChanged(int)), this, SLOT(updateHeight(int)));

	connect(ui.pushButton_takeSS, SIGNAL(clicked()), this, SLOT(takeScreenshot()));
	connect(ui.pushButton_openSSDirectory, SIGNAL(clicked()), this, SLOT(openSSDirectory()));
	connect(ui.pushButton_modifySSDirectory, SIGNAL(clicked()), this, SLOT(modifySSDirectory()));
}

void SettingScreenshotWidget::updateWidgets() {
	widgetList.blockSignals(true);
	ui.checkBox_keepAspectRatio->setChecked(ssParams->keepAspectRatio);
	ui.spinBox_width->setValue(ssParams->ssWidth);
	ui.spinBox_height->setValue(ssParams->ssHeight);
	widgetList.blockSignals(false);
}

void updateSpinBoxValueWithoutSignal(QSpinBox* sb, int value) {
	sb->blockSignals(true);
	sb->setValue(value);
	sb->blockSignals(false);
}

void SettingScreenshotWidget::updateSSDimensions() {
	updateSpinBoxValueWithoutSignal(ui.spinBox_width, ssParams->ssWidth);
	updateSpinBoxValueWithoutSignal(ui.spinBox_height, ssParams->ssHeight);
}


void SettingScreenshotWidget::updateKeepAspectRatio() {
	 ssParams->keepAspectRatio = ui.checkBox_keepAspectRatio->isChecked();
}

void SettingScreenshotWidget::updateWidth(int width) {
	ssParams->ssWidth = ui.spinBox_width->value();
	if (ssParams->keepAspectRatio) {
		ssParams->ssHeight = (float)ssParams->ssWidth * ssParams->getAspectRatio();
		updateSpinBoxValueWithoutSignal(ui.spinBox_height, ssParams->ssHeight);
	}
}

void SettingScreenshotWidget::updateHeight(int width) {
	ssParams->ssHeight = ui.spinBox_height->value();
	if (ssParams->keepAspectRatio) {
		ssParams->ssWidth = (float)ssParams->ssHeight / ssParams->getAspectRatio();
		updateSpinBoxValueWithoutSignal(ui.spinBox_width, ssParams->ssWidth);
	}
}

void SettingScreenshotWidget::takeScreenshot() {
	emit screenshot();
}

void SettingScreenshotWidget::openSSDirectory() {
	//QUrl url("file:///C:/Users/Home/Desktop/Documents/Projects/shvr/SHVR/screenshot/", QUrl::TolerantMode);
	QString urlStr = "file:///" + QString(ssParams->fileDir.c_str());
	QUrl url(urlStr, QUrl::TolerantMode);
	QDesktopServices::openUrl(url);
}

void SettingScreenshotWidget::modifySSDirectory() {	
	std::string fileDir = FileIO::getFileDirectoryWithDialog().toStdString();
	if (fileDir != "") { ssParams->fileDir = fileDir + "/"; }
	qDebug() << "FileDir: " << fileDir.c_str();
}