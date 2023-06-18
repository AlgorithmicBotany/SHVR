#include "CustomWidgets.h"

//========================//
//==== CUSTOM WIDGETS ====//
//========================//


//Double dials
DoubleDial::DoubleDial(QWidget *parent) : QDial(parent) {
	connect(this, SIGNAL(valueChanged(int)),
		this, SLOT(notifyValueChanged(int)));

}

void DoubleDial::notifyValueChanged(int value) {
	double doubleValue = value * precision;
	emit doubleValueChanged(doubleValue);
}

void DoubleDial::setValueDouble(double value) {
	int intValue = value / precision;
	this->setValue(intValue);
}

//Double sliders
DoubleSlider::DoubleSlider(QWidget *parent) : QSlider(parent) {
	connect(this, SIGNAL(valueChanged(int)),
		this, SLOT(notifyValueChanged(int)));

}

void DoubleSlider::notifyValueChanged(int value) {
	double doubleValue = value * precision;
	emit doubleValueChanged(doubleValue);
}

void DoubleSlider::setValueDouble(double value) {
	int intValue = value / precision;
	this->setValue(intValue);
}


//========================//
//===== WIDGET GROUP =====//
//========================//

WidgetSequence::WidgetSequence() {	
	mapper = new QSignalMapper(this);
	connect(mapper, SIGNAL(mapped(QWidget*)), this, SLOT(notifyValueChanged(QWidget*)));
}

WidgetSequence::~WidgetSequence(){
	// Note: all connected widgets are deleted by ui
	delete mapper;
}

void WidgetSequence::resize(int size) {
	g.resize(size);
}

void WidgetSequence::mapWidget(QWidget* widget, int index) {
	mapper->setMapping(widget, widget);	//assign index mapping to widget
	g[index] = widget;
}

void WidgetSequence::addWidget(QSpinBox* sb, int index) {
	mapWidget(sb, index);
	connect(sb, SIGNAL(valueChanged(int)), mapper, SLOT(map()));
}

void WidgetSequence::addWidget(QDoubleSpinBox* sb, int index) {	
	mapWidget(sb, index);
	connect(sb, SIGNAL(valueChanged(double)), mapper, SLOT(map()));
}

void WidgetSequence::addWidget(QCheckBox* cb, int index) {	
	mapWidget(cb, index);
	connect(cb, SIGNAL(stateChanged(int)), mapper, SLOT(map()));
}

void WidgetSequence::addWidget(QPushButton* b, int index) {	
	mapWidget(b, index);
	connect(b, SIGNAL(clicked()), mapper, SLOT(map()));	
}

void WidgetSequence::addWidget(DoubleDial* d, int index) {	
	mapWidget(d, index);
	connect(d, SIGNAL(doubleValueChanged(double)), mapper, SLOT(map()));	
}

void WidgetSequence::addWidget(DoubleSlider* s, int index) {
	mapWidget(s, index);
	connect(s, SIGNAL(doubleValueChanged(double)), mapper, SLOT(map()));
}

void WidgetSequence::removeWidget(QWidget* w) {
	removeWidget(index(w));
}

void WidgetSequence::removeWidget(int index) {
	if (index >= 0) {
		QWidget* w = at(index);
		mapper->removeMappings(w);
		disconnect(w, SIGNAL(valueChanged(double)), mapper, SLOT(map()));
		g[index] = 0;
	}
}

QWidget* WidgetSequence::operator[](int index) {
	return g[index];
}

QWidget* WidgetSequence::at(int index) {
	return g[index];
}

int WidgetSequence::index(QWidget* w) {
	for (int i = 0; i < g.size(); i++) {
		if (g[i] == w) {
			return i;
		}
	}
	return -1;
}

int WidgetSequence::size() {
	return g.size();
}

void WidgetSequence::notifyValueChanged(QWidget* w) {
	valueChanged(index(w));
}

