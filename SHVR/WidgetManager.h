#pragma once

#include "ParameterManager.h"

#include "GroupWidget.h"
#include "OffsetWidget.h"
#include "FileOpenWidget.h"
#include "TFWidget.h"
#include "LightWidget.h"
#include "SettingProbeWidget.h"
#include "SettingVisualWidget.h"
#include "SettingRenderWidget.h"
#include "SettingScreenshotWidget.h"
#include "HelpKeysWidget.h"
#include "HelpAboutWidget.h"

class WidgetManager
{
public:
	WidgetManager();	
	~WidgetManager();

	void init(ParameterManager* pMan);


	FileOpenWidget* fileOpenWidget;
	GroupWidget* groupWidget;
	OffsetWidget* offsetWidget;
	TFWidget* tfWidget;
	LightWidget* lightWidget;
	SettingProbeWidget* probeWidget;
	SettingRenderQualityWidget* renderWidget;
	SettingVisualWidget* visualWidget;
	SettingScreenshotWidget* ssWidget;
	HelpKeysWidget* helpKeysWidget;
	HelpAboutWidget* helpAboutWidget;
};
