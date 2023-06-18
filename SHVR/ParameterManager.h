#pragma once

#include <QObject>
#include "FileIO.h"
#include "volumes.h"
#include "World.h"
#include "TransferFunction.h"
#include "AmbientOcclusion.h"
#include "ParameterStructs.h"
#include "OffsetVolume.h"

class ParameterManager
{
public:
	ParameterManager();
	~ParameterManager();

	void initVolumeData(World* world);
	void initMaskData(World* world);
	void initOffsetData(World* world);

	void init();
	void importFromGeneralFile();
	void importFromGroupFile();
	void importFromTFFile();
	void importFromLightFile();

	void clearSettingFiles();

	void convertRAWDataBasedOnConfig(std::vector<unsigned char>& RAWData);
	void convertRAWDataBasedOnConfig(std::vector<unsigned short>& RAWData);

	Assign2::InputConfiguration loadConfigFile(std::string configFilename);
	void updateSettingsBasedOnConfigFile();

	void initVolumeBasedOnConfig_UINT8(World* world);
	void initVolumeBasedOnConfig_UINT16(World* world);
			
	TransferFunction* tf;
	AmbientOcclusion* ao;
	ColorGroups* cGroups;
	OffsetVolume* offsetVolume;

	//Settings files
	Assign2::InputConfiguration configFile;
	Assign2::InputGroup groupFile;
	Assign2::InputNode nodeFile;
	Assign2::InputGeneral generalFile;
	Assign2::InputTransferFunction tfFile;
	Assign2::InputLight lightFile;

	// Parameters
	LightParams* lightParams;
	IsoParams* isoParams;
	VisualParams* visualParams;
	ProbeParams* probeParams;
	RenderParams* renderParams;
	ScreenshotParams* ssParams;


};

