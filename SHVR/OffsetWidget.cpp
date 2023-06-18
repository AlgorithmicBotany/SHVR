#include "OffsetWidget.h"

OffsetWidget::OffsetWidget(OffsetVolume* _offsetVolume, QWidget *parent)
	: QWidget(parent, Qt::Window), offsetVolume(_offsetVolume)
{
	ui.setupUi(this);
	ui.glWidget_graph->loadGraph(offsetVolume);

	//initWidgetList();
	//initConnections();
}

OffsetWidget::~OffsetWidget()
{
}
//
//void OffsetWidget::initWidgetList() {
//	// Graph 
//	widgetList.push_back(ui.checkBox_LockIndices);
//	widgetList.push_back(ui.pushButton_UpdateGraph);
//	widgetList.push_back(ui.glWidget_graph);
//
//	// Selected CP
//	widgetList.push_back(ui.spinBox_SelectedCPIdx);
//	widgetList.push_back(ui.doubleSpinBox_xValue);
//	widgetList.push_back(ui.doubleSpinBox_xValue);
//	widgetList.push_back(ui.doubleSpinBox_yValue);
//	widgetList.push_back(ui.doubleSpinBox_yValue);
//}
//
//void OffsetWidget::updateWidgets() {
//	widgetList.blockSignals(true);
//
//	// Initial values
//	ui.checkBox_AutoUpdateGraph->setChecked(false);
//	ui.checkBox_LockIndices->setChecked(false);
//	ui.label_updateStatus->setText("-Updated-");
//
//	// Selected CP
//	ui.spinBox_SelectedCPIdx->setMinimum(-1);
//	ui.doubleSpinBox_xValue->setMinimum(offsetVolume->xValueMin);
//	ui.doubleSpinBox_xValue->setMaximum(offsetVolume->xValueMax);
//	ui.doubleSpinBox_yValue->setMinimum(offsetVolume->yValueMin);
//	ui.doubleSpinBox_yValue->setMaximum(offsetVolume->yValueMax);
//	updateSelectedCP();
//
//	widgetList.blockSignals(false);
//}
//
//void OffsetWidget::initConnections() {
//	//Graph
//	connect(ui.checkBox_LockIndices, SIGNAL(stateChanged(int)), this, SLOT(updateLockIndices()));
//	connect(ui.pushButton_UpdateGraph, SIGNAL(clicked()), this, SLOT(manualUpdate()));
//	connect(ui.glWidget_graph, SIGNAL(updatedGraph()), this, SLOT(autoUpdateGraph()));
//	connect(ui.glWidget_graph, SIGNAL(updatedGraph()), this, SLOT(updateSelectedCP()));
//
//	//Selected CP
//	connect(ui.spinBox_SelectedCPIdx, SIGNAL(valueChanged(int)),
//		this, SLOT(updateSelectedCPIdx(int)));
//	connect(ui.doubleSpinBox_xValue, SIGNAL(valueChanged(double)),
//		this, SLOT(updateSelectedCP_xValue(double)));
//	connect(ui.doubleSpinBox_yValue, SIGNAL(valueChanged(double)),
//		this, SLOT(updateSelectedCP_yValue(double)));
//}
//
//
//void OffsetWidget::manualUpdate() {
//	emit paramUpdated();
//	emit graphUpdated();
//	ui.label_updateStatus->setText("-Updated-");
//}
//
//void OffsetWidget::autoUpdateAOParams() {
//	if (ui.checkBox_AutoUpdateGraph->isChecked()) {
//		emit paramUpdated();
//		ui.label_updateStatus->setText("-Updated-");
//	}
//	else {
//		ui.label_updateStatus->setText("-Needs Update-");
//	}
//}
//
//void OffsetWidget::autoUpdateGraph() {
//	if (ui.checkBox_AutoUpdateGraph->isChecked()) {
//		emit graphUpdated();
//		ui.label_updateStatus->setText("-Updated-");
//	}
//	else {
//		ui.label_updateStatus->setText("-Needs Update-");
//	}
//}
//
//void OffsetWidget::updateLockIndices() {
//	ui.glWidget_graph->lockTFIndices = ui.checkBox_LockIndices->isChecked();
//}
//
//void OffsetWidget::updateSelectedCP() {
//	ui.spinBox_SelectedCPIdx->setMaximum(offsetVolume->controlPoints.size() - 1);
//
//	int idx = ui.glWidget_graph->selectedCPIdx;
//	ui.spinBox_SelectedCPIdx->setValue(idx);
//	if (idx >= 0) {
//		ControlPoint& cp = offsetVolume->controlPoints[idx];
//		ui.doubleSpinBox_xValue->setValue(cp.values.x);
//		ui.doubleSpinBox_yValue->setValue(cp.values.y);
//	}
//	else {
//		ui.doubleSpinBox_xValue->setValue(0);
//		ui.doubleSpinBox_yValue->setValue(0);
//	}
//}
//
//void OffsetWidget::updateSelectedCPIdx(int idx) {
//	ui.glWidget_graph->selectedCPIdx = idx;
//	ui.glWidget_graph->update();
//	updateSelectedCP();
//}
//
//void OffsetWidget::updateSelectedCP_xValue(double xValue) {
//	int selectedCPIdx = ui.glWidget_graph->selectedCPIdx;
//	ControlPoint& selectedCP = offsetVolume->controlPoints[selectedCPIdx];
//	offsetVolume->controlPoints[selectedCPIdx].values.x = xValue;
//	if (ui.checkBox_LockIndices->isChecked()) {
//		offsetVolume->boundCPToIdx(selectedCPIdx);
//	}
//	else {
//		offsetVolume->boundCPToWindow(selectedCP);
//		ui.glWidget_graph->selectedCPIdx = offsetVolume->updateCPIdx(selectedCP);
//	}
//	ui.glWidget_graph->update();
//	updateSelectedCP();
//	autoUpdateGraph();
//}
//
//void OffsetWidget::updateSelectedCP_yValue(double yValue) {
//	int idx = ui.glWidget_graph->selectedCPIdx;
//	offsetVolume->controlPoints[idx].values.y = yValue;
//	ui.glWidget_graph->update();
//	updateSelectedCP();
//	autoUpdateGraph();
//}
//
