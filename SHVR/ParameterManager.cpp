#include "ParameterManager.h"

ParameterManager::ParameterManager(){	
	tf = new TransferFunction();
	ao = new AmbientOcclusion();
	cGroups = new ColorGroups();
	offsetVolume = new OffsetVolume();

	// Parameters
	lightParams = new LightParams();
	isoParams = new IsoParams();
	visualParams = new VisualParams();
	probeParams = new ProbeParams();
	renderParams = new RenderParams();
	ssParams = new ScreenshotParams();
}

ParameterManager::~ParameterManager()
{
	delete tf;
	delete ao;
	delete cGroups;

	// Parameters
	delete lightParams;
	delete isoParams;
	delete visualParams;
	delete probeParams;
	delete renderParams;
	delete ssParams;
}

void ParameterManager::initVolumeData(World* world) {
	// Data
	if (configFile.inputType == Assign2::DataInput::UINT8) {
		initVolumeBasedOnConfig_UINT8(world);
	}
	else if (configFile.inputType == Assign2::DataInput::UINT16) {
		initVolumeBasedOnConfig_UINT16(world);
	}
}

void ParameterManager::initMaskData(World* world) {
	if (FileIO::isFilePresent(configFile.maskFile)) {
		std::vector<unsigned char> tempMask;
		FileIO::loadRAW(configFile.maskFile, configFile.dimensions, tempMask);
		VolumeIO::loadVolumeMask(world->volume.vMask(), tempMask);
	}
}

void ParameterManager::initOffsetData(World* world) {
	if (FileIO::isFilePresent(configFile.offsetFile)) {		
		auto vOffset = world->volume.vState()->vOffset;
		world->volume.vState()->clearVolumeOffset();
		if (configFile.inputType == Assign2::DataInput::UINT8) {
			std::vector<short> tempOffsets;
			FileIO::loadRAW(configFile.offsetFile, configFile.dimensions, tempOffsets);
			VolumeIO::loadVolume(vOffset, tempOffsets);
		}
		else if (configFile.inputType == Assign2::DataInput::UINT16) {
			std::vector<int> tempOffsets;
			FileIO::loadRAW(configFile.offsetFile, configFile.dimensions, tempOffsets);
			VolumeIO::loadVolume(vOffset, tempOffsets);
		}

		//Reapply offset
		world->volume.vState()->applyVolumeOffset();
	}
}



void ParameterManager::init() {			
	importFromGeneralFile();
	importFromGroupFile();
	importFromTFFile();
	importFromLightFile();
}

void ParameterManager::importFromGeneralFile() {
	visualParams->importData(generalFile);
	probeParams->importData(generalFile);
	renderParams->importData(generalFile);
	ssParams->importData(generalFile);
}
void ParameterManager::importFromGroupFile() {
	cGroups->importData(groupFile);
}
void ParameterManager::importFromTFFile() {
	tf->importData(tfFile);			// Editor
	isoParams->importData(tfFile);	// Params
}
void ParameterManager::importFromLightFile() {
	ao->importData(lightFile);			//Editor
	lightParams->importData(lightFile);	//Param
}

void ParameterManager::clearSettingFiles() {
	groupFile.clear();
	nodeFile.clear();
	generalFile.clear();
	tfFile.clear();
}

Assign2::InputConfiguration ParameterManager::loadConfigFile(std::string configFilename) {
	Assign2::InputConfiguration config; 
	FileIO::loadConfig(configFilename, config);
	return config;
}

void ParameterManager::updateSettingsBasedOnConfigFile() {
	//Execute from configuration

	//Group Files
	if (FileIO::isFilePresent(configFile.groupFile)) {
		FileIO::loadGroups(configFile.groupFile, groupFile);
	}
	else {
		FileIO::loadGroups("./settings/default.group", groupFile);
	}

	// Transfer function
	if (FileIO::isFilePresent(configFile.transferFuncFile)) {
		FileIO::loadTransferFunction(configFile.transferFuncFile, tfFile);
	}
	else if (configFile.inputType == Assign2::DataInput::UINT8) {
		FileIO::loadTransferFunction("./settings/default_8b.tf", tfFile);
	}
	else if (configFile.inputType == Assign2::DataInput::UINT16) {
		FileIO::loadTransferFunction("./settings/default_16b.tf", tfFile);
	}

	// Light Files
	if (FileIO::isFilePresent(configFile.lightFile)) {
		FileIO::loadLight(configFile.lightFile, lightFile);
	}
	else {
		FileIO::loadLight("./settings/default.light", lightFile);
	}

	// Screen/General file
	if (FileIO::isFilePresent(configFile.screenFile)) {
		FileIO::loadGeneral(configFile.screenFile, generalFile);
	}
	else {
		FileIO::loadGeneral("./settings/default.screen", generalFile);
	}
}

//====

unsigned short convertEndian(unsigned short value) {
	/*
	eg. Value: 90 AB 12 CD (4 bytes)
	Big Endian : Address	|	Value
	1000	|	90
	1001	|	AB
	1002	|	12
	1003	|	CD
	= 90 AB 12 CD

	Little Endian : Address	|	Value
	1000	|	CD
	1001	|	12
	1002	|	AB
	1003	|	90
	= CD 12 AB 90
	*/
	//Note: this implementation of swapEndian assumes
	//all data is uint16 (ie.unsigned short = 2 bytes = 16 bits)
	//AB to BA

	unsigned short a = (value & 0xFF00);
	unsigned short b = (value & 0x00FF);
	a = a >> 8;
	b = b << 8;
	unsigned short ba = b | a;

	return ba;
}

template <typename T>
void convertEndian(std::vector<T>& RAWData) {
	//Note: this program reads little endian by default
	std::transform(RAWData.begin(),
		RAWData.end(),
		RAWData.begin(),
		[](T num) -> float {
			return convertEndian(num);
		});
}

template <typename T>
void convertBackgroundValue(std::vector<T>& RAWData, Assign2::DataInput input) {
	float maxValue = 0;
	if (input == Assign2::DataInput::UINT8) { maxValue = 255; }		//unsigned char: 2^8 - 1 = 255
	else if (input == Assign2::DataInput::UINT16) { maxValue = 65535; }	//unsigned short: 2^16 - 1 = 65535
	else { return; }

	transform(RAWData.begin(),
		RAWData.end(),
		RAWData.begin(),
		[&](T num) -> float {
			return maxValue - num;
		});
}

template <typename T>
void convertRAWDataTemplate(std::vector<T>& RAWData,
	const Assign2::InputConfiguration& configFile) {
	if (configFile.convertEndian) {
		convertEndian(RAWData);
	}

	if (configFile.convertBackgroundValue) {
		convertBackgroundValue(RAWData, configFile.inputType);
	}
}

void ParameterManager::convertRAWDataBasedOnConfig(std::vector<unsigned char>& RAWData) {
	convertRAWDataTemplate(RAWData, configFile);
}

void ParameterManager::convertRAWDataBasedOnConfig(std::vector<unsigned short>& RAWData) {
	convertRAWDataTemplate(RAWData, configFile);
}
//====

template <typename T>
void loadRAWDataTemplate(
		Assign2::InputConfiguration& configFile,
		std::vector<T>& out_RAWData) {

	if (configFile.inputFileSuffix == "raw") {
		FileIO::loadRAW(configFile.inputFile, configFile.dimensions, out_RAWData);
	}

	else if (configFile.inputFileSuffix == "tif" ||
		configFile.inputFileSuffix == "tiff") {

		// Image Slices
		if (configFile.inputSlices.size() > 0) {
			FileIO::loadTIFFSlices(configFile.inputSlices, configFile.dimensions, out_RAWData);
		}

		// Volume
		else {
			configFile.dimensions = FileIO::getTIFFDimensions(configFile.inputFile);
			FileIO::loadTIFFVolume(configFile.inputFile, configFile.dimensions, out_RAWData);
		}
	}
}



void ParameterManager::initVolumeBasedOnConfig_UINT8(World* world) {
	// Load Raw Data
	std::vector<unsigned char> RAWData;		
	loadRAWDataTemplate(configFile, RAWData);

	// Process Raw Data
	convertRAWDataBasedOnConfig(RAWData);	
	tf->updateTFRange(RAWData, tfFile);						
	tf->generateHistogram(RAWData);
	VolumeIO::loadVolumeData(world->volume.vData(), RAWData, 
		tf->xValueMin, tf->xValueMax, tf->dataTypeRange);
}

void ParameterManager::initVolumeBasedOnConfig_UINT16(World* world) {
	std::vector<unsigned short> RAWData;
	loadRAWDataTemplate(configFile, RAWData);

	// Process Raw Data
	convertRAWDataBasedOnConfig(RAWData);	
	tf->updateTFRange(RAWData, tfFile);
	tf->generateHistogram(RAWData);
	VolumeIO::loadVolumeData(world->volume.vData(), RAWData, 
		tf->xValueMin, tf->xValueMax, tf->dataTypeRange);
}

//====

