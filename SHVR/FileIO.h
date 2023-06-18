#pragma once

#include <QObject>
#include <QFileDialog>
#include <QDebug>

#include <iostream>
#include <fstream>      // std::ifstream
#include <sstream>

#include <Array3D.h>
#include <qdebug.h>

#include "FileInputData.h"
#include "tiffio.h"		// TIFF support
#include "StatusBar.h"

#include <string>
//#include <filesystem>
//namespace fs = std::filesystem;


namespace FileIO 
{
	void iterateDirectory(QString dirPath, std::vector<std::string>& out_fileList);
	QString getFileDirectoryWithDialog();
	QString getFileNameWithDialog_Loading(QString nameFilters);
	QString getFileNameWithDialog_Saving(QString nameFilters);	

	std::vector<std::string> loadTextFileWithDialog(const QString& nameFilters);
	void saveTextFileWithDialog(const QString& nameFilters, const std::vector<std::string>& stringList);

	bool isFilePresent(const std::string& filename);
	bool isFileExtensionPresent(const std::string& filename, const std::string& fileExtension);

	
	// Volume & Mask Data
	std::vector<int> getTIFFDimensions(const std::string filename);
	int getTIFFBitsPerSample(const std::string filename);
	bool loadTIFFVolume(const std::string filename, std::vector<int>& dimensions, std::vector<unsigned char>& out_volume);
	bool loadTIFFVolume(const std::string filename, std::vector<int>& dimensions, std::vector<unsigned short>& out_volume);
		
	void loadTIFFSlices(const std::vector<std::string>& fileDir, std::vector<int>& dimensions, std::vector<unsigned char>& out_volume);
	void loadTIFFSlices(const std::vector<std::string>& fileDir, std::vector<int>& dimensions, std::vector<unsigned short>& out_volume);

	bool loadRAW(const std::string filename,
		const std::vector<int>& dimensions,
		std::vector<unsigned char>& out_Volume);
	bool loadRAW(const std::string filename, 
		const std::vector<int>& dimensions,
		std::vector<unsigned short>& out_Volume);
	bool loadRAW(const std::string filename,
		const std::vector<int>& dimensions,
		std::vector<short>& out_Volume);
	bool loadRAW(const std::string filename,
		const std::vector<int>& dimensions,
		std::vector<int>& out_Volume);
	bool saveRAW(std::string filename, std::vector<unsigned char>& in_volume);
	bool saveRAW(std::string filename, std::vector<unsigned short>& in_volume);
	bool saveRAW(std::string filename, std::vector<short>& in_volume);
	bool saveRAW(std::string filename, std::vector<int>& in_volume);
	bool saveRAW(std::string filename, unsigned char* in_volume, size_t size);
	bool saveRAW(std::string filename, unsigned short* in_volume, size_t size);

	bool saveLayerData(std::string filename, const Math::Array3D<std::vector<unsigned char>>& in_layers);
	bool loadLayerData(std::string filename, Math::Array3D<std::vector<unsigned char>>& out_layers);
	bool loadMaskFromLayerData(std::string filename, glm::ivec3 dimensions, std::vector<unsigned char>& out_mask);
	
	void updateConfigBasedOnInputFile(Assign2::InputConfiguration& configFile);

	bool loadConfigBasedOnSettingFiles(const std::vector<QFileInfo>& settingFiles, Assign2::InputConfiguration& config);

	//Settings
	bool loadConfig(std::string filename, Assign2::InputConfiguration& out_settingConfig);
	bool loadTransferFunction(std::string filename, Assign2::InputTransferFunction& out_settingTF);
	bool loadLight(std::string filename, Assign2::InputLight& out_settingLight);
	bool loadGroups(std::string filename, Assign2::InputGroup& out_settingGroups);
	bool loadNodeNetwork(std::string filename, Assign2::InputNode& out_settingNodes);
	bool loadGeneral(std::string filename, Assign2::InputGeneral& out_settingGeneral);

	void saveConfig(std::string filename, Assign2::InputConfiguration& out_settingConfig);
	void saveTransferFunction(std::string filename, Assign2::InputTransferFunction& out_settingTF);
	void saveLight(std::string filename, Assign2::InputLight& out_settingLight);
	void saveGroups(std::string filename, Assign2::InputGroup& out_settingGroups);
	void saveNodeNetwork(std::string filename, Assign2::InputNode& out_settingNodes);
	void saveGeneral(std::string filename, Assign2::InputGeneral& out_settingGeneral);
};
