#pragma once

#include <QWidget>
#include "ui_FileOpenWidget.h"

#include "FileIO.h"

class FileOpenWidget : public QWidget
{
	Q_OBJECT

public:
	FileOpenWidget(Assign2::InputConfiguration& cf, QWidget *parent = Q_NULLPTR);
	~FileOpenWidget();

	void updateWidgets();
	void initWidgetConnections();

	void updateVolumeSettingsTIF(std::string filename);
	void loadConfigWithWidget(Assign2::InputConfiguration& out_config);
	void loadWidgetsWithConfig(const Assign2::InputConfiguration& in_config);
	void updateFileTypeButtons(int input);
	void loadDraggedFiles(const QList<QUrl>& urls);

signals:
	void configLoaded();

public slots:
	void updateVolumeSettings();

	void browseFileName_Volume();
	void browseFileDirectory();
	void browseFileName_Group();
	void browseFileName_TF();
	void browseFileName_Light();
	void browseFileName_NN();
	void browseFileName_Mask();
	void browseFileName_Offset();
	void browseFileName_General();
	void loadConfigFile();
	void loadData();

	void updateFileTypeUI();

private:
	Ui::FileOpenWidget ui;
	Assign2::InputConfiguration& configFile;
};
