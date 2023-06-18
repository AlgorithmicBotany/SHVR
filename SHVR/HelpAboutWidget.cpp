#include "HelpAboutWidget.h"

HelpAboutWidget::HelpAboutWidget(QWidget* parent)
	: QWidget(parent, Qt::Window)
{
	ui.setupUi(this);
	QString str;
	str += "SHVR (Segmentation using Haptics and Volumetric Rendering)\n\n";
	str += "Created by: Philmo Gu and Andrew Owens\n";
	str += "Supervised by: Dr. Przemyslaw Prusinkiewicz\n";
	str += "From: Algorithmic Botany Lab, University of Calgary\n\n";
	str += "Contact Information : gup@ucalgary.ca\n";
	str += QString::fromWCharArray(L"\u00A9 2020 Philmo Gu\nAll rights reserved.");

	ui.label->setText(str);
	
}

HelpAboutWidget::~HelpAboutWidget()
{
}
