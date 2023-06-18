#include "WidgetManager.h"

WidgetManager::WidgetManager() {

}


WidgetManager::~WidgetManager()
{
}

void WidgetManager::init(ParameterManager* pMan)
{
	fileOpenWidget = new FileOpenWidget(pMan->configFile);

	tfWidget = new TFWidget(pMan->isoParams, pMan->tf);
	lightWidget = new LightWidget(pMan->lightParams, pMan->ao);
	groupWidget = new GroupWidget(pMan->cGroups);
	offsetWidget = new OffsetWidget(pMan->offsetVolume);

	probeWidget = new SettingProbeWidget(pMan->probeParams);
	visualWidget = new SettingVisualWidget(pMan->visualParams);
	renderWidget = new SettingRenderQualityWidget(pMan->renderParams);
	ssWidget = new SettingScreenshotWidget(pMan->ssParams);

	helpKeysWidget = new HelpKeysWidget();
	helpAboutWidget = new HelpAboutWidget();
}