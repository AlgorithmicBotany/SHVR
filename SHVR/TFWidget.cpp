#include "TFWidget.h"

TFWidget::TFWidget(IsoParams* _isoParams, TransferFunction* _tf, QWidget* parent)
	: QWidget(parent, Qt::Window), isoParams(_isoParams), tf(_tf), colorDialog(0)
{	
	ui.setupUi(this);
	ui.glWidget_graph->loadGraph(_tf);
	colorDialog = new QColorDialog();

	initWidgetList();
	initConnections();
}

TFWidget::~TFWidget()
{
	if (colorDialog != 0) { delete colorDialog; }
}

void TFWidget::initWidgetList() {
	// Graph 
	widgetList.push_back(ui.checkBox_LockIndices);
	widgetList.push_back(ui.pushButton_UpdateGraph);
	widgetList.push_back(ui.glWidget_graph);

	// Haptics
	widgetList.push_back(ui.doubleSpinBox_HapticIsoMin);
	widgetList.push_back(ui.horizontalSlider_HapticIsoMin);
	widgetList.push_back(ui.checkBox_AutoUpdateHapticIsoMin);

	// Painting
	widgetList.push_back(ui.doubleSpinBox_PaintIsoMin);
	widgetList.push_back(ui.horizontalSlider_PaintIsoMin);
	widgetList.push_back(ui.checkBox_AutoUpdatePaintIsoMin);

	// Offsets
	widgetList.push_back(ui.doubleSpinBox_OffsetValue);
	widgetList.push_back(ui.checkBox_AutoUpdateOffsetValue);

	// Selected CP
	widgetList.push_back(ui.spinBox_SelectedCPIdx);
	widgetList.push_back(ui.doubleSpinBox_xValue);
	widgetList.push_back(ui.doubleSpinBox_xValue);
	widgetList.push_back(ui.doubleSpinBox_yValue);
	widgetList.push_back(ui.doubleSpinBox_yValue);
}

void TFWidget::updateWidgets() {
	widgetList.blockSignals(true);

	// Initial values
	ui.checkBox_AutoUpdateGraph->setChecked(true);
	ui.checkBox_LockIndices->setChecked(false);
	ui.label_updateStatus->setText("-Updated-");
		
	// Haptics Iso
	ui.horizontalSlider_HapticIsoMin->setMinimumDouble(tf->xValueMin);
	ui.horizontalSlider_HapticIsoMin->setMaximumDouble(tf->xValueMax);
	ui.horizontalSlider_HapticIsoMin->setValueDouble(isoParams->hapticsIsoMin);

	ui.doubleSpinBox_HapticIsoMin->setMinimum(tf->xValueMin);
	ui.doubleSpinBox_HapticIsoMin->setMaximum(tf->xValueMax);
	ui.doubleSpinBox_HapticIsoMin->setValue(isoParams->hapticsIsoMin);

	ui.checkBox_AutoUpdateHapticIsoMin->setChecked(true);	
	
	// Painting Iso
	ui.horizontalSlider_PaintIsoMin->setMinimumDouble(tf->xValueMin);
	ui.horizontalSlider_PaintIsoMin->setMaximumDouble(tf->xValueMax);
	ui.horizontalSlider_PaintIsoMin->setValueDouble(isoParams->paintIsoMin);

	ui.doubleSpinBox_PaintIsoMin->setMinimum(tf->xValueMin);
	ui.doubleSpinBox_PaintIsoMin->setMaximum(tf->xValueMax);
	ui.doubleSpinBox_PaintIsoMin->setValue(isoParams->paintIsoMin);
	
	ui.checkBox_AutoUpdatePaintIsoMin->setChecked(true);	

	// Offset Value
	ui.doubleSpinBox_OffsetValue->setMinimum(-tf->xValueMax);
	ui.doubleSpinBox_OffsetValue->setMaximum(tf->xValueMax);
	ui.doubleSpinBox_OffsetValue->setValue(isoParams->offsetIsoValue);

	ui.checkBox_AutoUpdateOffsetValue->setChecked(true);

	ui.doubleSpinBox_IsoUpdateRate->setMinimum(0);
	ui.doubleSpinBox_IsoUpdateRate->setMaximum(10000);
	ui.doubleSpinBox_IsoUpdateRate->setValue(isoParams->IsoUpdateRate);

	//Selected CP
	ui.spinBox_SelectedCPIdx->setMinimum(-1);
	ui.doubleSpinBox_xValue->setMinimum(tf->xValueMin);
	ui.doubleSpinBox_xValue->setMaximum(tf->xValueMax);
	ui.doubleSpinBox_yValue->setMinimum(tf->yValueMin);
	ui.doubleSpinBox_yValue->setMaximum(tf->yValueMax);
	updateSelectedCP();

	widgetList.blockSignals(false);
}

void TFWidget::initConnections() {
	// Graph
	connect(ui.checkBox_LockIndices, SIGNAL(stateChanged(int)), this, SLOT(updateLockIndices()));
	connect(ui.pushButton_UpdateGraph, SIGNAL(clicked()), this, SLOT(manualUpdateGraph()));
	connect(ui.glWidget_graph, SIGNAL(updatedGraph()), this, SLOT(autoUpdateGraph()));
	connect(ui.glWidget_graph, SIGNAL(updatedGraph()), this, SLOT(updateSelectedCP()));

	// Color Dialog	
	connect(ui.pushButton_EditColorCP, SIGNAL(clicked()), this, SLOT(openColorDialog()));
	connect(colorDialog, SIGNAL(accepted()), this, SLOT(updateSelectedCP_color()));

	// Haptics
	connect(ui.doubleSpinBox_HapticIsoMin, SIGNAL(valueChanged(double)),
		this, SLOT(updateHapticIsoMin(double)));
	connect(ui.checkBox_AutoUpdateHapticIsoMin, SIGNAL(stateChanged(int)),
		this, SLOT(autoUpdateHapticIsoMin()));
	connect(ui.doubleSpinBox_HapticIsoMin, SIGNAL(valueChanged(double)),
		ui.horizontalSlider_HapticIsoMin, SLOT(setValueDouble(double)));
	connect(ui.horizontalSlider_HapticIsoMin, SIGNAL(doubleValueChanged(double)),
		ui.doubleSpinBox_HapticIsoMin, SLOT(setValue(double)));

	// Painting
	connect(ui.doubleSpinBox_PaintIsoMin, SIGNAL(valueChanged(double)),
		this, SLOT(updatePaintIsoMin(double)));
	connect(ui.checkBox_AutoUpdatePaintIsoMin, SIGNAL(stateChanged(int)),
		this, SLOT(autoUpdatePaintIsoMin()));
	connect(ui.doubleSpinBox_PaintIsoMin, SIGNAL(valueChanged(double)),
		ui.horizontalSlider_PaintIsoMin, SLOT(setValueDouble(double)));
	connect(ui.horizontalSlider_PaintIsoMin, SIGNAL(doubleValueChanged(double)),
		ui.doubleSpinBox_PaintIsoMin, SLOT(setValue(double)));

	// Offset
	connect(ui.doubleSpinBox_OffsetValue, SIGNAL(valueChanged(double)),
		this, SLOT(updateOffsetValue(double)));
	connect(ui.checkBox_AutoUpdateOffsetValue, SIGNAL(stateChanged(int)),
		this, SLOT(autoUpdateOffsetValue()));

	connect(ui.doubleSpinBox_IsoUpdateRate, SIGNAL(valueChanged(double)),
		this, SLOT(updateOffsetRate(double)));
	
	//Selected CP
	connect(ui.spinBox_SelectedCPIdx, SIGNAL(valueChanged(int)),
		this, SLOT(updateSelectedCPIdx(int)));
	connect(ui.doubleSpinBox_xValue, SIGNAL(valueChanged(double)),
		this, SLOT(updateSelectedCP_xValue(double)));
	connect(ui.doubleSpinBox_yValue, SIGNAL(valueChanged(double)),
		this, SLOT(updateSelectedCP_yValue(double)));
}

void TFWidget::manualUpdateGraph() {
	emit graphUpdated();
	ui.label_updateStatus->setText("-Updated-");
}

void TFWidget::autoUpdateGraph() {
	if (ui.checkBox_AutoUpdateGraph->isChecked()) {
		emit graphUpdated();
		ui.label_updateStatus->setText("-Updated-");
	}
	else {
		ui.label_updateStatus->setText("-Needs update-");
	}
	autoUpdateHapticIsoMin();
	autoUpdatePaintIsoMin();

}

void TFWidget::updateLockIndices() {
	ui.glWidget_graph->lockTFIndices = ui.checkBox_LockIndices->isChecked();
}

void TFWidget::updateSelectedCP() {	
	ui.spinBox_SelectedCPIdx->setMaximum(tf->controlPoints.size() - 1);

	int idx = ui.glWidget_graph->selectedCPIdx;
	ui.spinBox_SelectedCPIdx->setValue(idx);
	if (idx >= 0) {
		ControlPoint& cp = tf->controlPoints[idx];
		ui.doubleSpinBox_xValue->setValue(cp.values.x);
		ui.doubleSpinBox_yValue->setValue(cp.values.y);
	}
	else { 
		ui.doubleSpinBox_xValue->setValue(0);
		ui.doubleSpinBox_yValue->setValue(0);
	}
}

void TFWidget::updateSelectedCPIdx(int idx) {
	ui.glWidget_graph->selectedCPIdx = idx;
	ui.glWidget_graph->update();
	updateSelectedCP();
}

void TFWidget::updateSelectedCP_xValue(double xValue) {
	int selectedCPIdx = ui.glWidget_graph->selectedCPIdx;
	if (selectedCPIdx >= 0) {
		ControlPoint& selectedCP = tf->controlPoints[selectedCPIdx];
		tf->controlPoints[selectedCPIdx].values.x = xValue;
		if (ui.checkBox_LockIndices->isChecked()) {
			tf->boundCPToIdx(selectedCPIdx);
		}
		else {
			tf->boundCPToWindow(selectedCP);
			ui.glWidget_graph->selectedCPIdx = tf->updateCPIdx(selectedCP);
		}
		ui.glWidget_graph->update();
		updateSelectedCP();
		autoUpdateGraph();
	}
}

void TFWidget::updateSelectedCP_yValue(double yValue) {
	int idx = ui.glWidget_graph->selectedCPIdx;
	tf->controlPoints[idx].values.y = yValue;
	ui.glWidget_graph->update();
	updateSelectedCP();
	autoUpdateGraph();
}


void TFWidget::openColorDialog() {
	//colorDialog
	int idx = ui.glWidget_graph->selectedCPIdx;
	if (idx >= 0) {
		ControlPoint& cp = tf->controlPoints[idx];
		glm::vec3 c = glm::vec3(cp.color.r, cp.color.g, cp.color.b) * 255.f;
		QColor color_dialog(c.r, c.g, c.b);
		colorDialog->setCurrentColor(color_dialog);
		colorDialog->show();
	}
}

void TFWidget::updateSelectedCP_color() {
	int idx = ui.glWidget_graph->selectedCPIdx;
	if (idx >= 0) {
		ControlPoint& cp = tf->controlPoints[idx];
		QColor c = colorDialog->currentColor();
		glm::vec3 color_cp = glm::vec3(c.red(), c.green(), c.blue()) / 255.f;
		cp.color = color_cp;

		ui.glWidget_graph->update();
		autoUpdateGraph();
	}
}

void TFWidget::updateHapticIsoMin(double isoValue) {
	isoParams->hapticsIsoMin = isoValue;
	isoParams->hapticsInValue = tf->getVolumeInValue(isoValue);
}

void TFWidget::autoUpdateHapticIsoMin() {
	if (ui.checkBox_AutoUpdateHapticIsoMin->isChecked()) {
		ui.doubleSpinBox_HapticIsoMin->setValue(tf->getMinimumVisibleValue());		
	}
}

void TFWidget::updatePaintIsoMin(double isoValue) {
	isoParams->paintIsoMin = isoValue;	
	isoParams->paintInValue = tf->getVolumeInValue(isoValue);
	isoParams->isoSurfaceThresh = isoParams->paintInValue / tf->dataTypeRange;	//within in value range [0,1]

	autoUpdateOffsetValue();
}

void TFWidget::autoUpdatePaintIsoMin() {
	if (ui.checkBox_AutoUpdatePaintIsoMin->isChecked()) {
		ui.doubleSpinBox_PaintIsoMin->setValue(tf->getMinimumVisibleValue());
	}
}

void TFWidget::updateOffsetValue(double offsetValue) {
	isoParams->offsetIsoValue = offsetValue;
	isoParams->offsetInValue = tf->getOffsetInValue(offsetValue);
}

void TFWidget::autoUpdateOffsetValue() {
	bool isOffsetSameAsPaint = true;
	if (ui.checkBox_AutoUpdateOffsetValue->isChecked()) {		
		float minVisibleIsoValue = tf->getMinimumVisibleValue();
		float offsetValue = minVisibleIsoValue - isoParams->paintIsoMin;;

		//Update if difference between minimum visible TF value and
		// the paint Iso Min value is above 0
		if (offsetValue <= 0) {
			offsetValue = 0;
		}

		ui.doubleSpinBox_OffsetValue->setValue(offsetValue);
	}
	else {
		updateOffsetValue(ui.doubleSpinBox_OffsetValue->value());
	}
}

void TFWidget::updateOffsetRate(double offsetRate) {
	ui.doubleSpinBox_HapticIsoMin->setSingleStep(offsetRate);
	ui.doubleSpinBox_PaintIsoMin->setSingleStep(offsetRate);
	ui.doubleSpinBox_OffsetValue->setSingleStep(offsetRate);
	isoParams->IsoUpdateRate = offsetRate;
}

void TFWidget::updatePaintIsoMinWidget(double isoValue) {
	ui.doubleSpinBox_PaintIsoMin->setValue(isoValue);
	// parameters are automatically updated through signals and slots
}
void TFWidget::updateHapticIsoMinWidget(double isoValue) {
	ui.doubleSpinBox_HapticIsoMin->setValue(isoValue);
	// parameters are automatically updated through signals and slots
}