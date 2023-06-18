#pragma once

#include <QWidget>
#include "ui_HelpKeysWidget.h"

class HelpKeysWidget : public QWidget
{
	Q_OBJECT

public:
	HelpKeysWidget(QWidget *parent = Q_NULLPTR);
	~HelpKeysWidget();

private:
	Ui::HelpKeysWidget ui;
};
