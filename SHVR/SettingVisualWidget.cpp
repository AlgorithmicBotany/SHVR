#include "SettingVisualWidget.h"

SettingVisualWidget::SettingVisualWidget(VisualParams* _visualParams,QWidget *parent)
	: QWidget(parent, Qt::Window), visualParams(_visualParams)
{
	ui.setupUi(this);	
	ui.comboBox_screenMode->addItem("Full Screen");
	ui.comboBox_screenMode->addItem("Stereoscopic");
	ui.comboBox_screenMode->addItem("Overview");

	ui.horizontalSlider_cutMinX->setMinimumDouble(0);
	ui.horizontalSlider_cutMinY->setMinimumDouble(0);
	ui.horizontalSlider_cutMinZ->setMinimumDouble(0);
	ui.horizontalSlider_cutMaxX->setMinimumDouble(0);
	ui.horizontalSlider_cutMaxY->setMinimumDouble(0);
	ui.horizontalSlider_cutMaxZ->setMinimumDouble(0);
	ui.horizontalSlider_cutMinX->setMaximumDouble(1);
	ui.horizontalSlider_cutMinY->setMaximumDouble(1);
	ui.horizontalSlider_cutMinZ->setMaximumDouble(1);
	ui.horizontalSlider_cutMaxX->setMaximumDouble(1);
	ui.horizontalSlider_cutMaxY->setMaximumDouble(1);
	ui.horizontalSlider_cutMaxZ->setMaximumDouble(1);

	initWidgetList();
	initConnections();
}

SettingVisualWidget::~SettingVisualWidget()
{
}

void SettingVisualWidget::initWidgetList() {
	widgetList.push_back(ui.checkBox_enableArrow);
	widgetList.push_back(ui.checkBox_enableHUD);
	widgetList.push_back(ui.checkBox_enableObjects);
	widgetList.push_back(ui.checkBox_enableVolume);
	widgetList.push_back(ui.doubleSpinBox_fov);

	widgetList.push_back(ui.comboBox_screenMode);
	widgetList.push_back(ui.doubleSpinBox_clipMax);
	widgetList.push_back(ui.doubleSpinBox_clipMin);

	widgetList.push_back(ui.doubleSpinBox_cutMinX);
	widgetList.push_back(ui.doubleSpinBox_cutMinY);
	widgetList.push_back(ui.doubleSpinBox_cutMinZ);
	widgetList.push_back(ui.doubleSpinBox_cutMaxX);
	widgetList.push_back(ui.doubleSpinBox_cutMaxY);
	widgetList.push_back(ui.doubleSpinBox_cutMaxZ);
	widgetList.push_back(ui.horizontalSlider_cutMinX);
	widgetList.push_back(ui.horizontalSlider_cutMinY);
	widgetList.push_back(ui.horizontalSlider_cutMinZ);
	widgetList.push_back(ui.horizontalSlider_cutMaxX);
	widgetList.push_back(ui.horizontalSlider_cutMaxY);
	widgetList.push_back(ui.horizontalSlider_cutMaxZ);
}

void SettingVisualWidget::updateWidgets() {
	widgetList.blockSignals(true);

	ui.checkBox_enableArrow->setChecked(visualParams->arrowEnabled);
	ui.checkBox_enableHUD->setChecked(visualParams->hudEnabled);
	ui.checkBox_enableObjects->setChecked(visualParams->objectEnabled);
	ui.checkBox_enableVolume->setChecked(visualParams->volumeEnabled);
	ui.checkBox_enableForceLine->setChecked(visualParams->forceLineEnabled);
	ui.doubleSpinBox_fov->setValue(visualParams->fov);

	ui.comboBox_screenMode->setCurrentIndex(visualParams->screenType);
	ui.doubleSpinBox_clipMax->setValue(visualParams->clipPlaneFar);
	ui.doubleSpinBox_clipMin->setValue(visualParams->clipPlaneNear);

	ui.doubleSpinBox_cutMinX->setValue(visualParams->cutMin.x);
	ui.doubleSpinBox_cutMinY->setValue(visualParams->cutMin.y);
	ui.doubleSpinBox_cutMinZ->setValue(visualParams->cutMin.z);
	ui.doubleSpinBox_cutMaxX->setValue(visualParams->cutMax.x);
	ui.doubleSpinBox_cutMaxY->setValue(visualParams->cutMax.y);
	ui.doubleSpinBox_cutMaxZ->setValue(visualParams->cutMax.z);
	ui.horizontalSlider_cutMinX->setValueDouble(visualParams->cutMin.x);
	ui.horizontalSlider_cutMinY->setValueDouble(visualParams->cutMin.y);
	ui.horizontalSlider_cutMinZ->setValueDouble(visualParams->cutMin.z);
	ui.horizontalSlider_cutMaxX->setValueDouble(visualParams->cutMax.x);
	ui.horizontalSlider_cutMaxY->setValueDouble(visualParams->cutMax.y);
	ui.horizontalSlider_cutMaxZ->setValueDouble(visualParams->cutMax.z);

	widgetList.blockSignals(false);
}
void SettingVisualWidget::initConnections() {
	//***Import volumeM?
	connect(ui.pushButton_cameraSave, SIGNAL(clicked()), this, SLOT(saveCamera()));
	connect(ui.pushButton_cameraLoad, SIGNAL(clicked()), this, SLOT(loadCamera()));

	connect(ui.checkBox_enableArrow, SIGNAL(stateChanged(int)), this, SLOT(updateArrowEnabled(int)));
	connect(ui.checkBox_enableHUD, SIGNAL(stateChanged(int)), this, SLOT(updateHudEnabled(int)));
	connect(ui.checkBox_enableVolume, SIGNAL(stateChanged(int)), this, SLOT(updateVolumeEnabled(int)));
	connect(ui.checkBox_enableObjects, SIGNAL(stateChanged(int)), this, SLOT(updateObjectEnabled(int)));
	connect(ui.checkBox_enableForceLine, SIGNAL(stateChanged(int)), this, SLOT(updateForceLineEnabled(int)));
	connect(ui.doubleSpinBox_fov, SIGNAL(valueChanged(double)), this, SLOT(updateFOV(double)));

	connect(ui.comboBox_screenMode, SIGNAL(currentIndexChanged(int)), this, SLOT(updateScreenType(int)));
	connect(ui.doubleSpinBox_clipMax, SIGNAL(valueChanged(double)), this, SLOT(updateClipFar(double)));
	connect(ui.doubleSpinBox_clipMin, SIGNAL(valueChanged(double)), this, SLOT(updateClipNear(double)));

	connect(ui.doubleSpinBox_cutMinX, SIGNAL(valueChanged(double)), this, SLOT(updateCutMinX(double)));
	connect(ui.doubleSpinBox_cutMinY, SIGNAL(valueChanged(double)), this, SLOT(updateCutMinY(double)));
	connect(ui.doubleSpinBox_cutMinZ, SIGNAL(valueChanged(double)), this, SLOT(updateCutMinZ(double)));
	connect(ui.doubleSpinBox_cutMaxX, SIGNAL(valueChanged(double)), this, SLOT(updateCutMaxX(double)));
	connect(ui.doubleSpinBox_cutMaxY, SIGNAL(valueChanged(double)), this, SLOT(updateCutMaxY(double)));
	connect(ui.doubleSpinBox_cutMaxZ, SIGNAL(valueChanged(double)), this, SLOT(updateCutMaxZ(double)));

	connect(ui.doubleSpinBox_cutMinX, SIGNAL(valueChanged(double)),
		ui.horizontalSlider_cutMinX, SLOT(setValueDouble(double)));
	connect(ui.horizontalSlider_cutMinX, SIGNAL(doubleValueChanged(double)),
		ui.doubleSpinBox_cutMinX, SLOT(setValue(double)));
	connect(ui.doubleSpinBox_cutMinY, SIGNAL(valueChanged(double)),
		ui.horizontalSlider_cutMinY, SLOT(setValueDouble(double)));
	connect(ui.horizontalSlider_cutMinY, SIGNAL(doubleValueChanged(double)),
		ui.doubleSpinBox_cutMinY, SLOT(setValue(double)));
	connect(ui.doubleSpinBox_cutMinZ, SIGNAL(valueChanged(double)),
		ui.horizontalSlider_cutMinZ, SLOT(setValueDouble(double)));
	connect(ui.horizontalSlider_cutMinZ, SIGNAL(doubleValueChanged(double)),
		ui.doubleSpinBox_cutMinZ, SLOT(setValue(double)));

	connect(ui.doubleSpinBox_cutMaxX, SIGNAL(valueChanged(double)),
		ui.horizontalSlider_cutMaxX, SLOT(setValueDouble(double)));
	connect(ui.horizontalSlider_cutMaxX, SIGNAL(doubleValueChanged(double)),
		ui.doubleSpinBox_cutMaxX, SLOT(setValue(double)));
	connect(ui.doubleSpinBox_cutMaxY, SIGNAL(valueChanged(double)),
		ui.horizontalSlider_cutMaxY, SLOT(setValueDouble(double)));
	connect(ui.horizontalSlider_cutMaxY, SIGNAL(doubleValueChanged(double)),
		ui.doubleSpinBox_cutMaxY, SLOT(setValue(double)));
	connect(ui.doubleSpinBox_cutMaxZ, SIGNAL(valueChanged(double)),
		ui.horizontalSlider_cutMaxZ, SLOT(setValueDouble(double)));
	connect(ui.horizontalSlider_cutMaxZ, SIGNAL(doubleValueChanged(double)),
		ui.doubleSpinBox_cutMaxZ, SLOT(setValue(double)));

}

void SettingVisualWidget::saveCamera() { visualParams->queueCameraSave = true; emit paramUpdated(); }
void SettingVisualWidget::loadCamera() { visualParams->queueCameraLoad = true; emit paramUpdated(); }
void SettingVisualWidget::updateArrowEnabled(int v) { visualParams->arrowEnabled = v; emit paramUpdated(); }
void SettingVisualWidget::updateHudEnabled(int v) { visualParams->hudEnabled = v; emit paramUpdated();}
void SettingVisualWidget::updateVolumeEnabled(int v) { visualParams->volumeEnabled = v; emit paramUpdated();}
void SettingVisualWidget::updateObjectEnabled(int v) { visualParams->objectEnabled = v; emit paramUpdated();}
void SettingVisualWidget::updateForceLineEnabled(int v) { visualParams->forceLineEnabled= v; emit paramUpdated();}
void SettingVisualWidget::updateFOV(double v) { visualParams->fov = v; emit paramUpdated();}

void SettingVisualWidget::updateScreenType(int v) { visualParams->screenType = v; emit paramUpdated();}
void SettingVisualWidget::updateClipFar(double v) { visualParams->clipPlaneFar = v; emit paramUpdated();}
void SettingVisualWidget::updateClipNear(double v) { visualParams->clipPlaneNear = v; emit paramUpdated();}

void SettingVisualWidget::updateCutMinX(double v) { visualParams->cutMin.x = v; emit cutUpdated();}
void SettingVisualWidget::updateCutMinY(double v) { visualParams->cutMin.y = v; emit cutUpdated();}
void SettingVisualWidget::updateCutMinZ(double v) { visualParams->cutMin.z = v; emit cutUpdated();}
void SettingVisualWidget::updateCutMaxX(double v) { visualParams->cutMax.x = v; emit cutUpdated();}
void SettingVisualWidget::updateCutMaxY(double v) { visualParams->cutMax.y = v; emit cutUpdated();}
void SettingVisualWidget::updateCutMaxZ(double v) { visualParams->cutMax.z = v; emit cutUpdated();}
