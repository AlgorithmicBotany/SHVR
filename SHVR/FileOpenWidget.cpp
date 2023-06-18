#include "FileOpenWidget.h"

FileOpenWidget::FileOpenWidget(Assign2::InputConfiguration& cf, QWidget* parent)
	: QWidget(parent, Qt::Window), configFile(cf)
{
	ui.setupUi(this);

	updateWidgets();
	initWidgetConnections();
}

FileOpenWidget::~FileOpenWidget() {
}

void FileOpenWidget::updateWidgets() {
	ui.radioButton_File->setChecked(true);
	updateFileTypeUI();	
}

void FileOpenWidget::initWidgetConnections() {
	connect(ui.pushButton_File, SIGNAL(clicked()),
		this, SLOT(browseFileName_Volume()));
	connect(ui.lineEdit_File, SIGNAL(editingFinished()),
		this, SLOT(updateVolumeSettings()));

	connect(ui.pushButton_Directory, SIGNAL(clicked()),
		this, SLOT(browseFileDirectory()));
	connect(ui.pushButton_GroupFile, SIGNAL(clicked()),
		this, SLOT(browseFileName_Group()));
	connect(ui.pushButton_TFFile, SIGNAL(clicked()),
		this, SLOT(browseFileName_TF()));
	connect(ui.pushButton_LightFile, SIGNAL(clicked()),
		this, SLOT(browseFileName_Light()));
	connect(ui.pushButton_NNFile, SIGNAL(clicked()),
		this, SLOT(browseFileName_NN()));
	connect(ui.pushButton_MaskFile, SIGNAL(clicked()),
		this, SLOT(browseFileName_Mask()));
	connect(ui.pushButton_OffsetFile, SIGNAL(clicked()),
		this, SLOT(browseFileName_Offset()));
	connect(ui.pushButton_GeneralFile, SIGNAL(clicked()),
		this, SLOT(browseFileName_General()));

	connect(ui.pushButton_ConfigFile, SIGNAL(clicked()),
		this, SLOT(loadConfigFile()));
	connect(ui.pushButton_Open, SIGNAL(clicked()),
		this, SLOT(loadData()));

	connect(ui.radioButton_File, SIGNAL(clicked()),
		this, SLOT(updateFileTypeUI()));
	connect(ui.radioButton_ImageStack, SIGNAL(clicked()),
		this, SLOT(updateFileTypeUI()));
	connect(ui.radioButton_Directory, SIGNAL(clicked()),
		this, SLOT(updateFileTypeUI()));

}

void FileOpenWidget::updateVolumeSettingsTIF(std::string filename) {
	int bitsPerSample = FileIO::getTIFFBitsPerSample(filename);
	if (bitsPerSample == 8) { ui.comboBox_VoxelType->setCurrentIndex(1); }
	else if (bitsPerSample == 16) { ui.comboBox_VoxelType->setCurrentIndex(2); }
	else { ui.comboBox_VoxelType->setCurrentIndex(0); }
		
	std::vector<int> dim = FileIO::getTIFFDimensions(filename);
	ui.spinBox_resolutionWidth->setValue(dim[0]);
	ui.spinBox_resolutionHeight->setValue(dim[1]);
	ui.spinBox_resolutionDepth->setValue(dim[2]);

	//Note: Spacing information is not guaranteed in TIFF files 
	//(and I don't know how to get them if they did exist using this library)
	ui.doubleSpinBox_spacingWidth->setValue(1);
	ui.doubleSpinBox_spacingHeight->setValue(1);
	ui.doubleSpinBox_spacingDepth->setValue(1);
}

void FileOpenWidget::loadConfigWithWidget(Assign2::InputConfiguration& out_config) {
	//File names
	out_config.inputFile = ui.lineEdit_File->text().toStdString();
	out_config.inputDir = ui.lineEdit_Directory->text().toStdString();
	out_config.groupFile = ui.lineEdit_GroupFile->text().toStdString();
	out_config.transferFuncFile = ui.lineEdit_TFFile->text().toStdString();
	out_config.nodeFile = ui.lineEdit_NNFile->text().toStdString();
	out_config.maskFile = ui.lineEdit_MaskFile->text().toStdString();
	out_config.offsetFile = ui.lineEdit_OffsetFile->text().toStdString();
	out_config.lightFile = ui.lineEdit_LightFile->text().toStdString();
	out_config.screenFile = ui.lineEdit_GeneralFile->text().toStdString();

	// Voxel type
	int voxelTypeIdx = ui.comboBox_VoxelType->currentIndex();
	if (voxelTypeIdx == 0) { out_config.inputType = Assign2::DataInput::DATA_UNSET; }
	else if (voxelTypeIdx == 1) { out_config.inputType = Assign2::DataInput::UINT8; }
	else if (voxelTypeIdx == 2) { out_config.inputType = Assign2::DataInput::UINT16; }

	//Spacings
	out_config.spacings[0] = ui.doubleSpinBox_spacingWidth->value();
	out_config.spacings[1] = ui.doubleSpinBox_spacingHeight->value();
	out_config.spacings[2] = ui.doubleSpinBox_spacingDepth->value();

	//Dimensions
	out_config.dimensions[0] = ui.spinBox_resolutionWidth->value();
	out_config.dimensions[1] = ui.spinBox_resolutionHeight->value();
	out_config.dimensions[2] = ui.spinBox_resolutionDepth->value();

	//Configuration data settings
	out_config.convertBackgroundValue = ui.checkBox_WhiteBackground->isChecked();
	out_config.convertEndian = ui.checkBox_BigEndian->isChecked();
	out_config.volumeCompressionEnabled = ui.checkBox_CompressVolume->isChecked();
	out_config.maskEnabled = !ui.checkBox_DisableMask->isChecked();
	out_config.sceneScale = ui.doubleSpinBox_worldScale->value();


}

void FileOpenWidget::loadWidgetsWithConfig(const Assign2::InputConfiguration& in_config) {
	//File names
	ui.lineEdit_File->setText(QString::fromStdString(in_config.inputFile));
	ui.lineEdit_Directory->setText(QString::fromStdString(in_config.inputDir));
	ui.lineEdit_GroupFile->setText(QString::fromStdString(in_config.groupFile));
	ui.lineEdit_TFFile->setText(QString::fromStdString(in_config.transferFuncFile));
	ui.lineEdit_NNFile->setText(QString::fromStdString(in_config.nodeFile));
	ui.lineEdit_MaskFile->setText(QString::fromStdString(in_config.maskFile));
	ui.lineEdit_OffsetFile->setText(QString::fromStdString(in_config.offsetFile));
	ui.lineEdit_LightFile->setText(QString::fromStdString(in_config.lightFile));
	ui.lineEdit_GeneralFile->setText(QString::fromStdString(in_config.screenFile));

	// Voxel type
	int voxelTypeIdx = 0;
	if (in_config.inputType == Assign2::DataInput::UINT8) { voxelTypeIdx = 1; }
	if (in_config.inputType == Assign2::DataInput::UINT16) { voxelTypeIdx = 2; }
	ui.comboBox_VoxelType->setCurrentIndex(voxelTypeIdx);

	//Spacings
	ui.doubleSpinBox_spacingWidth->setValue(in_config.spacings[0]);
	ui.doubleSpinBox_spacingHeight->setValue(in_config.spacings[1]);
	ui.doubleSpinBox_spacingDepth->setValue(in_config.spacings[2]);

	//Dimensions
	ui.spinBox_resolutionWidth->setValue(in_config.dimensions[0]);
	ui.spinBox_resolutionHeight->setValue(in_config.dimensions[1]);
	ui.spinBox_resolutionDepth->setValue(in_config.dimensions[2]);

	//Configuration data settings
	ui.checkBox_WhiteBackground->setChecked(in_config.convertBackgroundValue);
	ui.checkBox_BigEndian->setChecked(in_config.convertEndian);
	ui.checkBox_CompressVolume->setChecked(in_config.volumeCompressionEnabled);
	ui.checkBox_DisableMask->setChecked(!in_config.maskEnabled);
	ui.doubleSpinBox_worldScale->setValue(in_config.sceneScale);

	// Overwrite parameters if it's a TIF file
	if (in_config.inputDir == "" && 
		FileIO::isFileExtensionPresent(in_config.inputFile, ".tif")) {
		updateVolumeSettingsTIF(in_config.inputFile);
	}
}

void FileOpenWidget::updateVolumeSettings() {
	QString filename = ui.lineEdit_File->text();
	//***Shouldn't I check if file exists?
	if (FileIO::isFileExtensionPresent(filename.toStdString(), ".tif")) {
		updateVolumeSettingsTIF(filename.toStdString());
	}
}

void FileOpenWidget::browseFileName_Volume() {
	QString filename = FileIO::getFileNameWithDialog_Loading("Volume Data File (*.raw *.tif)");	
	if (filename != "") {
		ui.lineEdit_Directory->setText("");	//Clear text
		ui.lineEdit_File->setText(filename);
		updateVolumeSettings();
	}
}

void FileOpenWidget::browseFileDirectory() {
	QString filename = FileIO::getFileDirectoryWithDialog();
	if (filename != "") {
		ui.lineEdit_File->setText("");	//Clear text
		ui.lineEdit_Directory->setText(filename);
	}
}

void FileOpenWidget::browseFileName_Group() {
	QString filename = FileIO::getFileNameWithDialog_Loading("Group File (*.group)");
	if (filename != "") ui.lineEdit_GroupFile->setText(filename);
}

void FileOpenWidget::browseFileName_TF() {
	QString filename = FileIO::getFileNameWithDialog_Loading("TF File (*.tf)");
	if (filename != "") ui.lineEdit_TFFile->setText(filename);
}

void FileOpenWidget::browseFileName_Light() {
	QString filename = FileIO::getFileNameWithDialog_Loading("Light File (*.light)");
	if (filename != "") ui.lineEdit_LightFile->setText(filename);
}

void FileOpenWidget::browseFileName_NN() {
	QString filename = FileIO::getFileNameWithDialog_Loading("Node Network File (*.nn)");
	if (filename != "") ui.lineEdit_NNFile->setText(filename);
}

void FileOpenWidget::browseFileName_Mask() {
	QString filename = FileIO::getFileNameWithDialog_Loading("Mask File (*.mask)");
	if (filename != "") ui.lineEdit_MaskFile->setText(filename);
}

void FileOpenWidget::browseFileName_Offset() {
	QString filename = FileIO::getFileNameWithDialog_Loading("Offset File (*.off)");
	if (filename != "") ui.lineEdit_OffsetFile->setText(filename);
}

void FileOpenWidget::browseFileName_General() {
	QString filename = FileIO::getFileNameWithDialog_Loading("Screen File (*.screen)");
	if (filename != "") ui.lineEdit_GeneralFile->setText(filename);
}

void FileOpenWidget::loadConfigFile() {	
	QString filename = FileIO::getFileNameWithDialog_Loading("Volume Data File (*.ini)");
	if (filename != "") {
		Assign2::InputConfiguration config;
		FileIO::loadConfig(filename.toStdString(), config);
		loadWidgetsWithConfig(config);
	}
}

void FileOpenWidget::loadData() {
	hide();
	configFile.clear();
	loadConfigWithWidget(configFile);
	emit configLoaded();	
}

void FileOpenWidget::updateFileTypeButtons(int input) {
	if (input == 0) {
		ui.radioButton_File->setChecked(true);
	}
	else if (input == 1) {
		ui.radioButton_Directory->setChecked(true);
	}
	else if (input == 2) {
		ui.radioButton_ImageStack->setChecked(true);
	}
}

void FileOpenWidget::updateFileTypeUI() {
	if (ui.radioButton_Directory->isChecked()) {
		ui.groupBox_Directory->show();
		ui.groupBox_File->hide();
		ui.groupBox_ImageStack->hide();
	}

	else if (ui.radioButton_File->isChecked()) {
		ui.groupBox_Directory->hide();
		ui.groupBox_File->show();
		ui.groupBox_ImageStack->hide();
	}

	else if (ui.radioButton_ImageStack->isChecked()) {
		ui.groupBox_Directory->hide();
		ui.groupBox_File->hide();
		ui.groupBox_ImageStack->show();
	}
}

void separateFileTypes(
	const QList<QUrl>& urls,
	std::vector<QFileInfo>& out_config,
	std::vector<QFileInfo>& out_setting,
	std::vector<QFileInfo>& out_images,
	std::vector<QFileInfo>& out_directories) {

	for (auto& url : urls) {
		QFileInfo f(url.toString());
		QString f_suf = f.suffix();

		// Load to image files
		if (f_suf == "raw" ||
			f_suf == "tif" ||
			f_suf == "tiff") {
			out_images.push_back(f);
		}

		// Assign to setting files
		else if (f_suf == "ini") {
			out_config.push_back(f);
		}
		else if(f_suf == "tf" ||
			f_suf == "screen" ||
			f_suf == "group" ||
			f_suf == "light" ||
			f_suf == "nn" ||
			f_suf == "off" ||
			f_suf == "mask") {
			out_setting.push_back(f);
		}

		else if (f_suf == "") {
			out_directories.push_back(f);
		}
	}
}

void FileOpenWidget::loadDraggedFiles(const QList<QUrl>& urls) {
	std::vector<QFileInfo> configFiles;
	std::vector<QFileInfo> settingFiles;
	std::vector<QFileInfo> imageFiles;
	std::vector<QFileInfo> fileDirs;

	separateFileTypes(urls,
		configFiles,
		settingFiles,
		imageFiles,
		fileDirs);

	// Initialize default configuration file
	if (configFiles.size() > 0) {
		// Overwrite configuration file with the following parameters
		Assign2::InputConfiguration config;
		FileIO::loadConfig(configFiles[0].filePath().mid(8).toStdString(), config);
		FileIO::updateConfigBasedOnInputFile(config);
		FileIO::loadConfigBasedOnSettingFiles(settingFiles, config);
		loadWidgetsWithConfig(config);
		show();
	}

	else if (fileDirs.size() > 0) {
		QString fileDir = fileDirs[0].filePath().mid(8);
		qDebug() << "FILEDIR: " << fileDir;

		Assign2::InputConfiguration config;
		config.inputDir = fileDir.toStdString();
		FileIO::updateConfigBasedOnInputFile(config);
		FileIO::loadConfigBasedOnSettingFiles(settingFiles, config);
		loadWidgetsWithConfig(config);
		updateFileTypeButtons(1);
		show();
	}

	// Load volume file
	else if (imageFiles.size() == 1) {
		QString volumeFile = imageFiles[0].filePath().mid(8);
		qDebug() << "VOLUMEFILE: " << volumeFile;

		Assign2::InputConfiguration config;
		config.inputFile = volumeFile.toStdString();
		config.sceneScale = 0.001;	//DEFAULT VALUE
		FileIO::updateConfigBasedOnInputFile(config);
		FileIO::loadConfigBasedOnSettingFiles(settingFiles, config);
		loadWidgetsWithConfig(config);
		updateFileTypeButtons(0);
		show();
	}

	// Load image stack
	else if (imageFiles.size() > 1) {
		//TODO
	}
}
