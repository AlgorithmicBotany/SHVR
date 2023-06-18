#pragma once

#include <QObject>
#include <QDial>
#include <QDoubleSpinBox>
#include <QSlider>
#include <QCheckBox>
#include <QPushButton>
#include <QSignalMapper>	//Needed to connect each button press to their address


class DoubleDial : public QDial {
	Q_OBJECT

	private:
		double precision = 0.001;

	public:
		DoubleDial(QWidget *parent = 0);
	
	signals:
		void doubleValueChanged(double value);

	public slots:
		void notifyValueChanged(int value);
		void setValueDouble(double value);
};


class DoubleSlider: public QSlider {
	Q_OBJECT

	private:
		double precision = 0.01;

	public:
		DoubleSlider(QWidget *parent = 0);
		void setMinimumDouble(double min) {
			setMinimum(min / precision);
		}
		void setMaximumDouble(double max) {
			setMaximum(max / precision);
		}

	signals:
		void doubleValueChanged(double value);

	public slots:
		void notifyValueChanged(int value);
		void setValueDouble(double value);
};

struct ObjectList : std::vector<QObject*> {
	void blockSignals(bool b) {
		for (int i = 0; i < size(); i++) {
			at(i)->blockSignals(b);
		}
	}
};


class WidgetSequence : public QWidget {
	Q_OBJECT

	protected:
		std::vector<QWidget*> g;
		QSignalMapper* mapper;

	public:
		WidgetSequence();
		~WidgetSequence();
		void resize(int size);
		void mapWidget(QWidget* widget, int index);
		void addWidget(QSpinBox* sb, int index);
		void addWidget(QDoubleSpinBox* sb, int index);
		void addWidget(QCheckBox* cb, int index);
		void addWidget(QPushButton* b, int index);
		void addWidget(DoubleDial* d, int index);
		void addWidget(DoubleSlider* s, int index);
		void removeWidget(QWidget* w);
		void removeWidget(int index);
		QWidget* operator[](int index);
		QWidget* at(int index);
		int index(QWidget* w);
		int size();

	signals:
		void valueChanged(int index);

	public slots:
		void notifyValueChanged(QWidget* w);
};
