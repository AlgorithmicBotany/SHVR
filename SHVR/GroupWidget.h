#pragma once

#include <QWidget>
#include <QColorDialog>	
#include "ParameterStructs.h"
#include "CustomWidgets.h"
#include "ui_GroupWidget.h"

class GroupWidget : public QWidget
{
	Q_OBJECT

public:
	GroupWidget(ColorGroups* _groups, QWidget *parent = Q_NULLPTR);
	~GroupWidget();

	void initWidgetSequences();
	void initConnections();
	void setWidgetColor(QWidget* widget, unsigned char r, unsigned char g, unsigned char b);
	void setWidgetColor(QWidget* widget, QColor& color);
	void initWidgetList();
	void updateWidgets();

	

public slots:
	void manualUpdate();
	void autoUpdate();
	void openColorDialog(int groupNum);
	void updateSelectedColor();
	void updateMaskOpacity(int groupNum);
	void updateMaskIntensity();

	void updateObjectEnabled(int groupNum);
	void updateMaskEnabled(int groupNum);

	void updatePaintOver(int groupNum);

signals:
	void maskColorsUpdated();
	void maskIntensityUpdated();
	void maskUpdated();
	void layersUpdated();

private:
	ObjectList widgetList;
	Ui::GroupWidget ui;	
	ColorGroups* groups;

	QColorDialog colorDialog;
	WidgetSequence wColor;
	WidgetSequence wLinkScale;
	WidgetSequence wMaskOpacity;
	WidgetSequence wMaskEnabled;
	WidgetSequence wObjEnabled;
	WidgetSequence wPaintOver;
	int selectedGroupNum;	

	bool isMaskUpdated;
	bool isLayersUpdated;
};
