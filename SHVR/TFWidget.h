#pragma once
#include "Graph2DOpenGL.h"
class TFOpenGLWidget : public Graph2DOpenGL {
public:
	TFOpenGLWidget(QWidget* parent) : Graph2DOpenGL(parent)
	{}
};

#include <QWidget>
#include <qcolordialog.h>
#include "CustomWidgets.h"
#include "ui_TFWidget.h"	//Requires "Graph2DOpenGL" included above

class TFWidget : public QWidget
{
	Q_OBJECT

public:	
	TFWidget(IsoParams* _isoParams, TransferFunction* _tf, QWidget* parent = Q_NULLPTR);
	~TFWidget();

	void initWidgetList();
	void updateWidgets();
	void initConnections();

	void updatePaintIsoMinWidget(double isoValue);

	void updateHapticIsoMinWidget(double isoValue);

private:
	ObjectList widgetList;
	Ui::TFWidget ui;	
	TransferFunction* tf;
	IsoParams* isoParams;
	QColorDialog* colorDialog;
signals:
	void graphUpdated();

public slots:	
	void manualUpdateGraph();
	void autoUpdateGraph();
	void updateLockIndices();
	void updateSelectedCP();
	void updateSelectedCPIdx(int idx);
	void updateSelectedCP_xValue(double xValue);
	void updateSelectedCP_yValue(double yValue);
	void openColorDialog();
	void updateSelectedCP_color();
	void updateHapticIsoMin(double isoValue);
	void autoUpdateHapticIsoMin();
	
	void updatePaintIsoMin(double isoValue);
	void autoUpdatePaintIsoMin();
	void updateOffsetValue(double offsetValue);
	void autoUpdateOffsetValue();
	void updateOffsetRate(double offsetRate);
};
