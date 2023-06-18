#include "GroupWidget.h"

GroupWidget::GroupWidget(ColorGroups* _groups, QWidget *parent)
	: QWidget(parent, Qt::Window), groups(_groups)
{
	ui.setupUi(this);	
	initWidgetSequences();
	initWidgetList();
	initConnections();
	isMaskUpdated = false;
	isLayersUpdated = false;
}

GroupWidget::~GroupWidget() {
}

void GroupWidget::initWidgetSequences() {
	const int MAX_GROUP_SIZE = groups->size();

	wColor.resize(MAX_GROUP_SIZE);
	wColor.addWidget(ui.pushButton_colorBackground, 0);
	wColor.addWidget(ui.pushButton_color1, 1);
	wColor.addWidget(ui.pushButton_color2, 2);
	wColor.addWidget(ui.pushButton_color3, 3);
	wColor.addWidget(ui.pushButton_color4, 4);
	wColor.addWidget(ui.pushButton_color5, 5);
	wColor.addWidget(ui.pushButton_color6, 6);
	wColor.addWidget(ui.pushButton_color7, 7);
	wColor.addWidget(ui.pushButton_color8, 8);
	wColor.addWidget(ui.pushButton_color9, 9);

	wMaskOpacity.resize(MAX_GROUP_SIZE);
	wMaskOpacity.addWidget(ui.spinBox_volumeOpacity, 0);
	wMaskOpacity.addWidget(ui.spinBox_maskOpacity1, 1);
	wMaskOpacity.addWidget(ui.spinBox_maskOpacity2, 2);
	wMaskOpacity.addWidget(ui.spinBox_maskOpacity3, 3);
	wMaskOpacity.addWidget(ui.spinBox_maskOpacity4, 4);
	wMaskOpacity.addWidget(ui.spinBox_maskOpacity5, 5);
	wMaskOpacity.addWidget(ui.spinBox_maskOpacity6, 6);
	wMaskOpacity.addWidget(ui.spinBox_maskOpacity7, 7);
	wMaskOpacity.addWidget(ui.spinBox_maskOpacity8, 8);
	wMaskOpacity.addWidget(ui.spinBox_maskOpacity9, 9);

	wLinkScale.resize(MAX_GROUP_SIZE);
	wLinkScale.addWidget(ui.doubleSpinBox_linkScale1, 1);
	wLinkScale.addWidget(ui.doubleSpinBox_linkScale2, 2);
	wLinkScale.addWidget(ui.doubleSpinBox_linkScale3, 3);
	wLinkScale.addWidget(ui.doubleSpinBox_linkScale4, 4);
	wLinkScale.addWidget(ui.doubleSpinBox_linkScale5, 5);
	wLinkScale.addWidget(ui.doubleSpinBox_linkScale6, 6);
	wLinkScale.addWidget(ui.doubleSpinBox_linkScale7, 7);
	wLinkScale.addWidget(ui.doubleSpinBox_linkScale8, 8);
	wLinkScale.addWidget(ui.doubleSpinBox_linkScale9, 9);

	wMaskEnabled.resize(MAX_GROUP_SIZE);
	wMaskEnabled.addWidget(ui.checkBox_mask1, 1);
	wMaskEnabled.addWidget(ui.checkBox_mask2, 2);
	wMaskEnabled.addWidget(ui.checkBox_mask3, 3);
	wMaskEnabled.addWidget(ui.checkBox_mask4, 4);
	wMaskEnabled.addWidget(ui.checkBox_mask5, 5);
	wMaskEnabled.addWidget(ui.checkBox_mask6, 6);
	wMaskEnabled.addWidget(ui.checkBox_mask7, 7);
	wMaskEnabled.addWidget(ui.checkBox_mask8, 8);
	wMaskEnabled.addWidget(ui.checkBox_mask9, 9);

	wObjEnabled.resize(MAX_GROUP_SIZE);
	wObjEnabled.addWidget(ui.checkBox_object1, 1);
	wObjEnabled.addWidget(ui.checkBox_object2, 2);
	wObjEnabled.addWidget(ui.checkBox_object3, 3);
	wObjEnabled.addWidget(ui.checkBox_object4, 4);
	wObjEnabled.addWidget(ui.checkBox_object5, 5);
	wObjEnabled.addWidget(ui.checkBox_object6, 6);
	wObjEnabled.addWidget(ui.checkBox_object7, 7);
	wObjEnabled.addWidget(ui.checkBox_object8, 8);
	wObjEnabled.addWidget(ui.checkBox_object9, 9);

	wPaintOver.resize(MAX_GROUP_SIZE);
	wPaintOver.addWidget(ui.checkBox_paintOver0, 0);
	wPaintOver.addWidget(ui.checkBox_paintOver1, 1);
	wPaintOver.addWidget(ui.checkBox_paintOver2, 2);
	wPaintOver.addWidget(ui.checkBox_paintOver3, 3);
	wPaintOver.addWidget(ui.checkBox_paintOver4, 4);
	wPaintOver.addWidget(ui.checkBox_paintOver5, 5);
	wPaintOver.addWidget(ui.checkBox_paintOver6, 6);
	wPaintOver.addWidget(ui.checkBox_paintOver7, 7);
	wPaintOver.addWidget(ui.checkBox_paintOver8, 8);
	wPaintOver.addWidget(ui.checkBox_paintOver9, 9);
}

void GroupWidget::initConnections() {
	connect(ui.pushButton_updateMaskLayers, SIGNAL(clicked()), this, SLOT(manualUpdate()));

	//Connect each button to edit its own color
	//Note: We need to separate opening of color dialog and editing widget color because
	//If we use "m_colorDialog->getColor(...)", canceling the operation would return black
	connect(&wColor, SIGNAL(valueChanged(int)), this, SLOT(openColorDialog(int)));
	connect(&colorDialog, SIGNAL(accepted()), this, SLOT(updateSelectedColor()));	

	//Connect values such that they show in visualizer
	connect(&wMaskOpacity, SIGNAL(valueChanged(int)), this, SLOT(updateMaskOpacity(int)));
	connect(ui.doubleSpinBox_maskIntensity, SIGNAL(valueChanged(double)), this, SLOT(updateMaskIntensity()));
	connect(&wMaskEnabled, SIGNAL(valueChanged(int)), this, SLOT(updateMaskEnabled(int)));
	connect(&wObjEnabled, SIGNAL(valueChanged(int)), this, SLOT(updateObjectEnabled(int)));
	connect(&wPaintOver, SIGNAL(valueChanged(int)), this, SLOT(updatePaintOver(int)));
	//connect(&wLinkScale, SIGNAL(valueChanged(int)), this, SLOT(openColorDialog(int)));
}

void GroupWidget::setWidgetColor(QWidget* widget, unsigned char r, unsigned char g, unsigned char b) {
	QString str = "background-color: rgb(" + QString::number(r) + "," + QString::number(g) + "," + QString::number(b) + ")";
	widget->setStyleSheet(str);
}

void GroupWidget::setWidgetColor(QWidget* widget, QColor& color) {
	setWidgetColor(widget, color.red(), color.green(), color.blue());
}

void GroupWidget::initWidgetList() {
	widgetList.push_back(ui.doubleSpinBox_maskIntensity);
	for (int i = 0; i < groups->size(); i++) {
		widgetList.push_back(wColor[i]);
		widgetList.push_back(wMaskOpacity[i]);
		widgetList.push_back(wPaintOver[i]);
	}

	for (int i = 1; i < groups->size(); i++) {
		widgetList.push_back(wLinkScale[i]);
		widgetList.push_back(wMaskEnabled[i]);
		widgetList.push_back(wObjEnabled[i]);
	}
}

void GroupWidget::updateWidgets() {
	widgetList.blockSignals(true);
	ui.doubleSpinBox_maskIntensity->setValue(groups->maskIntensity);

	for (int i = 0; i < groups->size(); i++) {
		ColorGroup& g = groups->at(i);
		setWidgetColor(wColor[i], g.r, g.g, g.b);
		((QSpinBox*)wMaskOpacity[i])->setValue(g.a);
		((QCheckBox*)wPaintOver[i])->setChecked(g.paintOver);
	}

	for (int i = 1; i < groups->size(); i++) {
		ColorGroup& g = groups->at(i);
		((QDoubleSpinBox*)wLinkScale[i])->setValue(g.linkScale);
		((QCheckBox*)wMaskEnabled[i])->setChecked(g.displayMask);	
		((QCheckBox*)wObjEnabled[i])->setChecked(g.displayObject);
	}
	widgetList.blockSignals(false);
	ui.label_updateStatus->setText("-Updated-");
}

void GroupWidget::manualUpdate() {
	ui.label_updateStatus->setText("-Updated-");
	if (isMaskUpdated) { emit maskUpdated(); }	
	isMaskUpdated = false;
}

void GroupWidget::autoUpdate() {
	if (ui.checkBox_autoUpdate->isChecked()) {
		manualUpdate();
	}
	else {
		ui.label_updateStatus->setText("-Needs Update-");
	}
}

void GroupWidget::openColorDialog(int groupNum) {
	selectedGroupNum = groupNum;

	QWidget* widget = wColor[selectedGroupNum];
	QColor c_init = widget->palette().color(QPalette::Button);
	colorDialog.setCurrentColor(c_init);
	colorDialog.show();
}

void GroupWidget::updateSelectedColor() {
	QWidget* widget = wColor[selectedGroupNum];
	QColor color = colorDialog.currentColor();
	setWidgetColor(widget, color);

	ColorGroup& g = groups->at(selectedGroupNum);	
	g.r = color.red();
	g.g = color.green();
	g.b = color.blue();
	emit maskColorsUpdated();
}

void GroupWidget::updateMaskOpacity(int groupNum) {	
	ColorGroup& g = groups->at(groupNum);
	g.a = ((QSpinBox*)wMaskOpacity[groupNum])->value();
	emit maskColorsUpdated();
}

void GroupWidget::updateMaskIntensity() {
	groups->maskIntensity = ui.doubleSpinBox_maskIntensity->value();
	emit maskIntensityUpdated();
}

void GroupWidget::updateObjectEnabled(int groupNum) {
	ColorGroup& g = groups->at(groupNum);
	g.displayObject = ((QCheckBox*)wObjEnabled[groupNum])->isChecked();
}
void GroupWidget::updateMaskEnabled(int groupNum) {
	isMaskUpdated = true;
	ColorGroup& g = groups->at(groupNum);
	g.displayMask = ((QCheckBox*)wMaskEnabled[groupNum])->isChecked();
	autoUpdate();
}

void GroupWidget::updatePaintOver(int groupNum) {
	ColorGroup& g = groups->at(groupNum);
	g.paintOver = ((QCheckBox*)wPaintOver[groupNum])->isChecked();
}

//void GroupWidget::updateLinkScale(int groupNum) {}
//void GroupWidget::updateMaskIntensity() {}
//void GroupWidget::updateVolumeOpacity() {}
//void GroupWidget::toggleAllObjects() {}
//void GroupWidget::toggleAllMasks() {}
//void GroupWidget::resetMaskOpacity() {}
//void GroupWidget::resetLinkScale() {}