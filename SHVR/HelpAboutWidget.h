#pragma once

#include <QWidget>
#include "ui_HelpAboutWidget.h"

class HelpAboutWidget : public QWidget
{
	Q_OBJECT

public:
	HelpAboutWidget(QWidget *parent = Q_NULLPTR);
	~HelpAboutWidget();

private:
	Ui::HelpAboutWidget ui;
};
