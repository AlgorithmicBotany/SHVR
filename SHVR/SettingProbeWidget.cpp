#include "SettingProbeWidget.h"

SettingProbeWidget::SettingProbeWidget(ProbeParams* _probeParams, QWidget* parent)
	: QWidget(parent, Qt::Window), probeParams(_probeParams)
{	
	ui.setupUi(this);
	ui.comboBox_hapticMode->addItem("Surface Sampling");
	ui.comboBox_hapticMode->addItem("Object Following");
	ui.comboBox_hapticMode->addItem("Proximity Following");

	ui.comboBox_paintMode->addItem("Quick Fill");
	ui.comboBox_paintMode->addItem("Flood Fill");
	ui.comboBox_paintMode->addItem("Speckle Fill");

	initWidgetList();
	initConnections();
}

SettingProbeWidget::~SettingProbeWidget()
{
}


void SettingProbeWidget::initWidgetList() {
	widgetList.push_back(ui.checkBox_enableHaptics);
	widgetList.push_back(ui.comboBox_hapticMode);
	widgetList.push_back(ui.comboBox_paintMode);
	widgetList.push_back(ui.doubleSpinBox_hapticDampingForce);
	widgetList.push_back(ui.doubleSpinBox_hapticSpringForce);
	widgetList.push_back(ui.doubleSpinBox_hapticSurfaceSamplingHitThreshold);
	widgetList.push_back(ui.doubleSpinBox_paintScale);
	widgetList.push_back(ui.doubleSpinBox_probeScale);
	widgetList.push_back(ui.doubleSpinBox_silhouetteScale);
	widgetList.push_back(ui.doubleSpinBox_workspaceScale);
	widgetList.push_back(ui.spinBox_selectedGroup);
}
void SettingProbeWidget::updateWidgets() {
	widgetList.blockSignals(true);

	ui.checkBox_enableHaptics->setChecked(probeParams->hapticsEnabled);
	ui.comboBox_hapticMode->setCurrentIndex(probeParams->hapticType);
	ui.comboBox_paintMode->setCurrentIndex(probeParams->paintType);
	ui.doubleSpinBox_hapticDampingForce->setValue(probeParams->hapticDampingForce);
	ui.doubleSpinBox_hapticSpringForce->setValue(probeParams->hapticSpringForce);
	ui.doubleSpinBox_hapticSurfaceSamplingHitThreshold->setValue(probeParams->hapticHitThreshold);
	ui.doubleSpinBox_paintScale->setValue(probeParams->paintScale);
	ui.doubleSpinBox_probeScale->setValue(probeParams->probeSize);
	ui.doubleSpinBox_silhouetteScale->setValue(probeParams->silhouetteSize);
	ui.doubleSpinBox_workspaceScale->setValue(probeParams->workspaceScale);
	ui.spinBox_selectedGroup->setValue(probeParams->selectedGroup);

	widgetList.blockSignals(false);
}

void SettingProbeWidget::initConnections() {
	connect(ui.checkBox_enableHaptics, SIGNAL(stateChanged(int)), this, SLOT(updateHapticsEnabled(int)));
	connect(ui.comboBox_hapticMode, SIGNAL(currentIndexChanged(int)), this, SLOT(updateHapticMode(int)));
	connect(ui.comboBox_paintMode, SIGNAL(currentIndexChanged(int)), this, SLOT(updatePaintMode(int)));
	connect(ui.doubleSpinBox_hapticDampingForce, SIGNAL(valueChanged(double)), this, SLOT(updateHapticDampingForce(double)));
	connect(ui.doubleSpinBox_hapticSpringForce, SIGNAL(valueChanged(double)), this, SLOT(updateHapticSpringForce(double)));
	connect(ui.doubleSpinBox_hapticSurfaceSamplingHitThreshold, SIGNAL(valueChanged(double)), this, SLOT(updateHapticHitThreshold(double)));
	connect(ui.doubleSpinBox_paintScale, SIGNAL(valueChanged(double)), this, SLOT(updatePaintScale(double)));
	connect(ui.doubleSpinBox_probeScale, SIGNAL(valueChanged(double)), this, SLOT(updatePaintSize(double)));
	connect(ui.doubleSpinBox_silhouetteScale, SIGNAL(valueChanged(double)), this, SLOT(updateSilhouetteSize(double)));
	connect(ui.doubleSpinBox_workspaceScale, SIGNAL(valueChanged(double)), this, SLOT(updateWorkspaceScale(double)));
	connect(ui.spinBox_selectedGroup, SIGNAL(valueChanged(int)), this, SLOT(updateSelectedGroup(int)));
}


void SettingProbeWidget::updateHapticsEnabled(int v) { probeParams->hapticsEnabled = v; }
void SettingProbeWidget::updateHapticMode(int v) { probeParams->hapticType = v; }
void SettingProbeWidget::updatePaintMode(int v) { probeParams->paintType = v; }
void SettingProbeWidget::updateHapticDampingForce(double v) { probeParams->hapticDampingForce = v; }
void SettingProbeWidget::updateHapticSpringForce(double v) { probeParams->hapticSpringForce = v; }
void SettingProbeWidget::updateHapticHitThreshold(double v) { probeParams->hapticHitThreshold = v; }
void SettingProbeWidget::updatePaintScale(double v) { probeParams->paintScale = v; }
void SettingProbeWidget::updatePaintSize(double v) { probeParams->probeSize = v; emit paramUpdated(); }
void SettingProbeWidget::updateSilhouetteSize(double v) { probeParams->silhouetteSize = v; emit paramUpdated();}
void SettingProbeWidget::updateWorkspaceScale(double v) { probeParams->workspaceScale = v; }
void SettingProbeWidget::updateSelectedGroup(int v) { probeParams->selectedGroup = v; }