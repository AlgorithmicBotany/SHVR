#include "FileIO.h"

void FileIO::iterateDirectory(QString dirPath, std::vector<std::string>& out_fileList) {
	QDir dir(dirPath);
	dir.setFilter(QDir::Files);
	QFileInfoList entries = dir.entryInfoList();
	for (QFileInfoList::ConstIterator entry = entries.begin(); entry != entries.end(); ++entry)
	{
		qDebug() << *entry;
		out_fileList.push_back((*entry).filePath().toStdString());
	}
}


QString FileIO::getFileNameWithDialog_Loading(QString nameFilters) {	
	QFileDialog dialog;
	dialog.setFileMode(QFileDialog::AnyFile);

	QString fileName;
	QStringList fileNames;
	dialog.setNameFilter(nameFilters);	//filter for files with this extension
	dialog.setViewMode(QFileDialog::Detail);
	dialog.setDirectory("./");

	//Obtain filename using Qt load in OS
	if (dialog.exec())
		fileNames = dialog.selectedFiles();

	if (fileNames.size() > 0) {
		fileName = fileNames[0];
		qDebug() << "Loading: " << fileName;
	}

	return fileName;
}


QString FileIO::getFileDirectoryWithDialog() {
	QFileDialog dialog;
	dialog.setFileMode(QFileDialog::Directory);

	QString fileName;
	QStringList fileNames;
	dialog.setViewMode(QFileDialog::Detail);

	//Obtain filename using Qt load in OS
	if (dialog.exec())
		fileNames = dialog.selectedFiles();

	if (fileNames.size() > 0) {
		fileName = fileNames[0];
		qDebug() << "Loading: " << fileName;
	}

	return fileName;
}

QString FileIO::getFileNameWithDialog_Saving(QString nameFilters) {
	//Save using Qt save in OS 
	QString fileName =
		QFileDialog::getSaveFileName(Q_NULLPTR,
			"Save File",
			QString(),
			nameFilters);	//filter for files with this extension

	qDebug() << "Saving: " << fileName;
	return fileName;
}

class Line : public std::string
{
	friend std::istream& operator>>(std::istream& is, Line& line)
	{
		return std::getline(is, line);
	}
};

bool loadTextFile(const std::string& fileName, std::vector<std::string>& loaded) {
	std::ifstream ifs(fileName, std::ifstream::in);

	if (!ifs.good())
	{
		qDebug() << "exit(" << ifs.fail() << ") cannot open param file.";
		return false;
	}

	//Copy contents of file into a list
	std::copy(std::istream_iterator<Line>(ifs),
		std::istream_iterator<Line>(),
		std::back_inserter(loaded)
		);

	ifs.close();

	return true;
}

bool saveTextFile(const std::string& filename, const std::vector<std::string>& stringList) {
	std::string str;
	std::ofstream myfile;
	myfile.open(filename);

	//Save contents of list into a file
	for (std::string s : stringList) {
		myfile << s << "\n";
	}

	myfile.close();

	return true;
}

std::vector<std::string> FileIO::loadTextFileWithDialog(const QString& nameFilters) {
	std::vector<std::string> loaded;
	std::string filename = getFileNameWithDialog_Loading(nameFilters).toStdString();
	loadTextFile(filename, loaded);
	for (std::string s : loaded) {
		qDebug() << s.c_str();
	}
	return loaded;
}

void FileIO::saveTextFileWithDialog(const QString& nameFilters, const std::vector<std::string>& stringList) {
	std::vector<std::string> loaded;
	std::string filename = getFileNameWithDialog_Saving(nameFilters).toStdString();
	saveTextFile(filename, stringList);
}

bool FileIO::isFilePresent(const std::string& filename) {
	std::ifstream f(filename.c_str());
	return f.good();
}

bool FileIO::isFileExtensionPresent(const std::string& filename, const std::string& fileExtension) {
	//std::string extension = filename.substr(filename.find_last_of("."));

	std::string extension = "";
	int idx = filename.find_last_of(".");
	if (idx > 0) {
		extension = filename.substr(idx);
	}

	if (extension == fileExtension) { return true; }
	else { return false; }
}


int loadFileDirectoryContent(const std::string& fileDir, std::vector<std::string>& out_fileList) {
	QDir dir(QString::fromStdString(fileDir));
	dir.setFilter(QDir::Files);
	QStringList entries = dir.entryList();
	for (QStringList::ConstIterator entry = entries.begin(); entry != entries.end(); ++entry)
	{
		out_fileList.push_back((*entry).toStdString());
	}
	return entries.size();
}


//======================//
//== Load Volume Data ==//
//======================//

std::vector<int> FileIO::getTIFFDimensions(const std::string filename) {	
	// Note: The directory count doesn't reset until it's closed and opened again
	int w = 0; 
	int h = 0;
	int numberOfImages = 0;
	TIFF* tif = TIFFOpen(filename.c_str(), "r");
	if (tif) {		
		
		TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &w);
		TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);

		//***Read directory seems to be returning false when there are multiple images?
		do {
			numberOfImages++;
		} while (TIFFReadDirectory(tif));		//Read the next image in TIFF file
		TIFFClose(tif);
	}	

	std::vector<int> dimensions = {w, h, numberOfImages};
	return dimensions;
}

int FileIO::getTIFFBitsPerSample(const std::string filename) {
	int bits = 0;
	TIFF* tif = TIFFOpen(filename.c_str(), "r");
	if (tif) {
		TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bits);
		TIFFClose(tif);
	}	
	return bits;
}

template <typename T>
bool loadTIFFVolumeTemplate(
		const std::string filename,
		const std::vector<int>& dimensions,
		std::vector<T>& out_volume) {

	TIFF* tif = TIFFOpen(filename.c_str(), "r");
	if (tif) {
		uint32 w = dimensions[0];
		uint32 h = dimensions[1];
		int numberOfImages = dimensions[2];		
				
		// Allocate memory for buffer
		T* buffer = (T*)malloc(w * sizeof(T));		
		
		// Fill volume data with voxel values
		int count = 0;
		do {
			tsize_t count = 0;
			for (uint32 row = 0; row < h; row++)
			{
				TIFFReadScanline(tif, buffer, row);
				out_volume.insert(out_volume.end(), buffer, buffer + w);
				count += w;
			}
		} while (TIFFReadDirectory(tif));	// Move to next image

		// Clean up resources
		free(buffer);
		TIFFClose(tif);	


		//Pass on relevant information from TIFF
		//*** How do you obtain Endian byte order of image? There is no tag. (Assume little endian?)
		//*** I also don't know how to obtain spacing information, if it exists.				
		

		return true;
	}	

	return false;
}

bool FileIO::loadTIFFVolume(const std::string filename,
	std::vector<int>& dimensions,
	std::vector<unsigned char>& out_volume) {
	return loadTIFFVolumeTemplate(filename, dimensions, out_volume);
}

bool FileIO::loadTIFFVolume(const std::string filename,
	std::vector<int>& dimensions,
	std::vector<unsigned short>& out_volume) {
	return loadTIFFVolumeTemplate(filename, dimensions, out_volume);
}



template <typename T>
void loadTIFFSlicesTemplate(const std::vector<std::string> filePaths,
	std::vector<int>& dimensions,
	std::vector<T>& out_volume) {
	
	// Get dimension of one tiff slice
	uint32 w = dimensions[0];
	uint32 h = dimensions[1];

	// Allocate memory for buffer
	int bufSize = w * sizeof(T);
	T* buffer = (T*)malloc(w * sizeof(T));

	// Iterate through all image slices
	tsize_t count = 0;
	for (auto f : filePaths) {

		// Read data of opened TIF file
		TIFF* tif = TIFFOpen(f.c_str(), "r");
		if (tif) {			
			for (uint32 row = 0; row < h; row++)
			{
				TIFFReadScanline(tif, buffer, row);
				out_volume.insert(out_volume.end(), buffer, buffer + w);
				count += w;
			}
			
			TIFFClose(tif);
		}
	}

	// Deallocate memory from buffer
	free(buffer);
}

void FileIO::loadTIFFSlices(const std::vector<std::string>& filePaths,
	std::vector<int>& dimensions,
	std::vector<unsigned char>& out_volume) {	
	loadTIFFSlicesTemplate(filePaths, dimensions, out_volume);
}

void FileIO::loadTIFFSlices(const std::vector<std::string>& filePaths,
	std::vector<int>& dimensions,
	std::vector<unsigned short>& out_volume) {	
	loadTIFFSlicesTemplate(filePaths, dimensions, out_volume);
}



template <typename T>
bool loadRAWTemplate(const std::string filename, 
		const std::vector<int>& dimensions,
		std::vector<T>& out_volume) {

	int width = dimensions[0];
	int height = dimensions[1];
	int depth = dimensions[2];
	
	std::ifstream file(filename, std::ifstream::binary);
	if (!file.good())
	{
		qDebug() << "File Error: can't open " << filename.data();
		return false;
	}

	long long int size = (long long int)width * (long long int)height * (long long int)depth;
	out_volume.resize(size); // needs to be resize as file.read(...) dosn't increment size

	file.read(reinterpret_cast<char*>(out_volume.data()), size * sizeof(T));
	
	return true;
}

bool FileIO::loadRAW(const std::string filename,
	const std::vector<int>& dimensions,
	std::vector<unsigned char>& out_volume) {
	return loadRAWTemplate(filename, dimensions, out_volume);
}

bool FileIO::loadRAW(const std::string filename,
	const std::vector<int>& dimensions,
	std::vector<unsigned short>& out_volume) {
	return loadRAWTemplate(filename, dimensions, out_volume);
}

bool FileIO::loadRAW(const std::string filename,
	const std::vector<int>& dimensions,
	std::vector<short>& out_volume) {
	return loadRAWTemplate(filename, dimensions, out_volume);
}

bool FileIO::loadRAW(const std::string filename,
	const std::vector<int>& dimensions,
	std::vector<int>& out_volume) {
	return loadRAWTemplate(filename, dimensions, out_volume);
}


template <typename T>
bool saveRAWTemplate(std::string filename, std::vector<T>& in_volume) {	
	std::ofstream myfile;
	myfile.open(filename, std::ofstream::binary);

	myfile.write(reinterpret_cast<char*>(in_volume.data()), sizeof(T) * in_volume.size());

	myfile.close();

	return true;
}

bool FileIO::saveRAW(std::string filename, std::vector<unsigned char>& in_volume) {
	return saveRAWTemplate(filename, in_volume);
}

bool FileIO::saveRAW(std::string filename, std::vector<unsigned short>& in_volume) {
	return saveRAWTemplate(filename, in_volume);
}

bool FileIO::saveRAW(std::string filename, std::vector<short>& in_volume) {
	return saveRAWTemplate(filename, in_volume);
}

bool FileIO::saveRAW(std::string filename, std::vector<int>& in_volume) {
	return saveRAWTemplate(filename, in_volume);
}

bool FileIO::saveLayerData(std::string filename, const Math::Array3D<std::vector<unsigned char>>& in_layers) {
	//DEPRECATED!!
	int width = in_layers.width();
	int height = in_layers.height();
	int depth = in_layers.depth();

	/*
	Layer Information:
	0. Layer data size
	1. Size at each voxel layer
	2. Information inside voxels

	Note: These information are already given:
	1. The total size of g_layer is given by the
	width, height and depth of the volume

	2. The voxels with size zero means that there
	is nothing inside, so their information doesn't
	have to be stored.

	3. Any information that is repeated or is beyond
	a "delete" layer from the top doesn't have to be
	stored since they don't store any additional
	information about the layer
	*/

	//Store about each voxel layer
	std::vector<unsigned char> layerSize;
	std::vector<unsigned char> layerData;
	for (int k = 0; k < depth; k++) {
		qDebug() << "[SYSTEM] Saving... (" + QString::number(k) + " /" + QString::number(depth) + ")";
		for (int j = 0; j < height; j++) {
			for (int i = 0; i < width; i++) {
				int voxelSize = in_layers.at(i, j, k).size();

				std::vector<unsigned char > relevantGroup;
				//view of layers is from top to bottom
				for (int m = voxelSize - 1; m >= 0; m--) {
					unsigned char voxelLayer = in_layers.at(i, j, k)[m];

					//Only include c groups that aren't inside 
					bool isAlreadyIncluded = false;
					for (int n = 0; n < relevantGroup.size(); n++) {
						if (voxelLayer == relevantGroup[n]) {
							isAlreadyIncluded = true;
							break;
						}
					}

					if (isAlreadyIncluded == false) {
						relevantGroup.push_back(voxelLayer);
					}
				}

				//Load loads from bottom to top
				for (int m = relevantGroup.size() - 1; m >= 0; m--) {
					unsigned char voxelLayer = relevantGroup[m];
					layerData.push_back(voxelLayer);
				}

				//update size of each voxel layer
				voxelSize = relevantGroup.size();
				layerSize.push_back((unsigned char)voxelSize);

				relevantGroup.clear();
			}
		}
	}

	//divide up layer data size into 4 unsigned char
	//Note: Need to be divided long int to char to prevent exception?
	//Note: This has limit of 32 bits, or between [0, 4,294,967,295]
	unsigned long int layerDataSize_int = layerData.size();
	std::vector<unsigned char> layerDataSize;
	for (int i = 0; i < 4; i++) {
		unsigned char size_part = layerDataSize_int | 0;
		layerDataSize.push_back(size_part);

		//shift by 8 since unsigned char is 8 bits long
		layerDataSize_int = layerDataSize_int >> 8;
	}

	std::ofstream myfile;
	myfile.open(filename, std::ofstream::binary);

	myfile.write(reinterpret_cast<char*>(layerDataSize.data()), sizeof(unsigned char) * layerDataSize.size());
	myfile.write(reinterpret_cast<char*>(layerSize.data()), sizeof(unsigned char) * layerSize.size());
	myfile.write(reinterpret_cast<char*>(layerData.data()), sizeof(unsigned char) * layerData.size());
	myfile.close();

	return true;
}

bool FileIO::loadLayerData(std::string filename, Math::Array3D<std::vector<unsigned char>>& out_layers) {
	//DEPRECATED!!
	//Note: all layers data is lost after loading

	int width = out_layers.width();
	int height = out_layers.height();
	int depth = out_layers.depth();

	//load in size of data first
	std::ifstream file(filename, std::ifstream::binary);
	if (!file.good())
	{
		qDebug() << "File Error: can't open out_layers file";
		return false;
	}

	std::vector<unsigned char> layerSizeOfData;
	int sizeOfSizeInfo = 4;	//size of element containing number of layer data
	layerSizeOfData.resize(sizeOfSizeInfo); // needs to be resize as file.read dosn't increment size
	file.read(reinterpret_cast<char*>(layerSizeOfData.data()), sizeOfSizeInfo * sizeof(unsigned char));

	//extract size of layer data
	unsigned long int layerDataSize_int = 0;
	for (int i = 0; i < sizeOfSizeInfo; i++) {
		//shift by 8 since unsigned char is 8 bits long
		unsigned int shifted_num = layerSizeOfData[i] << (i * 8);

		//bitwise OR operation
		layerDataSize_int = layerDataSize_int | shifted_num;
	}

	//load in the rest of the data
	//Note: continuing to read data starts where it left off
	int volumeSize = width * height * depth;
	unsigned long int size = volumeSize + layerDataSize_int;

	std::vector<unsigned char> layerSize(volumeSize);
	file.read(reinterpret_cast<char*>(layerSize.data()), volumeSize * sizeof(unsigned char));

	std::vector<unsigned char> layerData(layerDataSize_int);
	file.read(reinterpret_cast<char*>(layerData.data()), layerDataSize_int * sizeof(unsigned char));

	file.close();

	//input all the elements to layers
	unsigned long int iterSize = 0;
	unsigned long int iterData = 0;
	for (int k = 0; k < depth; k++) {
		for (int j = 0; j < height; j++) {
			for (int i = 0; i < width; i++) {
				
				unsigned char voxelSize = layerSize[iterSize];

				out_layers.at(i, j, k).clear();
				if (voxelSize > 0) {
					for (int m = 0; m < voxelSize; m++) {
						out_layers.at(i, j, k).push_back(layerData[iterData]);
						iterData++;
					}
				}

				iterSize++;
			}
		}
		qDebug() << "[SYSTEM] Loading... (" + QString::number(k) + " /" + QString::number(depth) + ")";
	}

	//Note: Should clear any undo data after loading.
	//Currently, this is done in VolumeManager

	return true;
}

bool FileIO::loadMaskFromLayerData(std::string filename, glm::ivec3 dimensions, std::vector<unsigned char>& out_mask) {
	//Note: all layers data is lost after loading

	int width = dimensions.x;
	int height = dimensions.y;
	int depth = dimensions.z;

	//load in size of data first
	std::ifstream file(filename, std::ifstream::binary);
	if (!file.good())
	{
		qDebug() << "File Error: can't open out_layers file";
		return false;
	}

	std::vector<unsigned char> layerSizeOfData;
	int sizeOfSizeInfo = 4;	//size of element containing number of layer data
	layerSizeOfData.resize(sizeOfSizeInfo); // needs to be resize as file.read dosn't increment size
	file.read(reinterpret_cast<char*>(layerSizeOfData.data()), sizeOfSizeInfo * sizeof(unsigned char));

	//extract size of layer data
	unsigned long int layerDataSize_int = 0;
	for (int i = 0; i < sizeOfSizeInfo; i++) {
		//shift by 8 since unsigned char is 8 bits long
		unsigned int shifted_num = layerSizeOfData[i] << (i * 8);

		//bitwise OR operation
		layerDataSize_int = layerDataSize_int | shifted_num;
	}

	//load in the rest of the data
	//Note: continuing to read data starts where it left off
	int volumeSize = width * height * depth;
	unsigned long int size = volumeSize + layerDataSize_int;

	std::vector<unsigned char> layerSize(volumeSize);
	file.read(reinterpret_cast<char*>(layerSize.data()), volumeSize * sizeof(unsigned char));

	std::vector<unsigned char> layerData(layerDataSize_int);
	file.read(reinterpret_cast<char*>(layerData.data()), layerDataSize_int * sizeof(unsigned char));

	file.close();

	//input all the elements to layers
	long int iterSize = 0;		
	long int iterData = -1;		//align with index
	out_mask.resize(volumeSize, 0);
	for (int k = 0; k < depth; k++) {
		for (int j = 0; j < height; j++) {
			for (int i = 0; i < width; i++) {

				// Obtain the last element of each voxel list
				unsigned char voxelSize = layerSize[iterSize];
				if (voxelSize > 0) {
					iterData += voxelSize;
					out_mask.at(i + width * j + width * height * k) = layerData[iterData];					
				}

				iterSize++;
			}
		}
		qDebug() << "[SYSTEM] Loading... (" + QString::number(k) + " /" + QString::number(depth) + ")";
	}

	//Note: Should clear any undo data after loading.
	//Currently, this is done in VolumeManager

	return true;
}

//=======================//
//== Load Setting Data ==//
//=======================//

template <typename T>
bool loadSettingFileTemplate(std::string filename, T& out_settingFile) {
	T setting;

	//Process string through setting file
	std::vector< std::string > loadedStrings;
	if (loadTextFile(filename, loadedStrings)) {
		setting.load(loadedStrings);
		out_settingFile = setting;
		return true;
	}
	return false;
}

void FileIO::updateConfigBasedOnInputFile(Assign2::InputConfiguration& configFile) {
	//Load directory
	if (configFile.inputDir != "") {
		FileIO::iterateDirectory(
			QString::fromStdString(configFile.inputDir),
			configFile.inputSlices);
		configFile.inputFile = configFile.inputSlices[0];
	}

	QFileInfo f(QString::fromStdString(configFile.inputFile));
	configFile.inputFileSuffix = f.suffix().toStdString();

	if (configFile.inputFileSuffix == "tif" ||
		configFile.inputFileSuffix == "tiff") {
		std::string filePath = configFile.inputFile;

		if (configFile.dimensions[0] == 0 &&
			configFile.dimensions[1] == 0 &&
			configFile.dimensions[2] == 0) {

			std::vector<int> dim = FileIO::getTIFFDimensions(filePath);
			configFile.dimensions[0] = dim[0];
			configFile.dimensions[1] = dim[1];
			configFile.dimensions[2] = dim[2];

			if (configFile.inputSlices.size() > 0) {
				configFile.dimensions[2] = configFile.inputSlices.size();
			}
		}

		if (configFile.spacings[0] == 0 &&
			configFile.spacings[1] == 0 &&
			configFile.spacings[2] == 0) {
			//*** How do you get spacing from a tiff file?
			configFile.spacings[0] = 1;
			configFile.spacings[1] = 1;
			configFile.spacings[2] = 1;
		}

		if (configFile.inputType == Assign2::DataInput::DATA_UNSET) {
			int bitsPerSample = FileIO::getTIFFBitsPerSample(filePath);
			if (bitsPerSample == 8) { configFile.inputType = Assign2::DataInput::UINT8; }
			else if (bitsPerSample == 16) { configFile.inputType = Assign2::DataInput::UINT16; }
		}
	}
}
bool FileIO::loadConfigBasedOnSettingFiles(const std::vector<QFileInfo> &settingFiles, Assign2::InputConfiguration& config) {
	for (int i = 0; i < settingFiles.size(); i++) {
		if (settingFiles[i].completeSuffix() == "tf") {
			config.transferFuncFile = settingFiles[i].filePath().mid(8).toStdString();
		}
		else if (settingFiles[i].completeSuffix() == "screen") {
			config.screenFile = settingFiles[i].filePath().mid(8).toStdString();
		}
		else if (settingFiles[i].completeSuffix() == "group") {
			config.groupFile = settingFiles[i].filePath().mid(8).toStdString();
		}
		else if (settingFiles[i].completeSuffix() == "light") {
			config.lightFile = settingFiles[i].filePath().mid(8).toStdString();
		}
		else if (settingFiles[i].completeSuffix() == "nn") {
			config.nodeFile = settingFiles[i].filePath().mid(8).toStdString();
		}
		else if (settingFiles[i].completeSuffix() == "mask") {
			config.maskFile = settingFiles[i].filePath().mid(8).toStdString();
		}
		else if (settingFiles[i].completeSuffix() == "off") {
			config.offsetFile = settingFiles[i].filePath().mid(8).toStdString();
		}
	}
	return true;
}

bool FileIO::loadConfig(std::string filename, Assign2::InputConfiguration& out_settingConfig) {
	if (loadSettingFileTemplate(filename, out_settingConfig)) {
		out_settingConfig.configFile = filename;		//Update filename for saving
		return true;
	}
	return false;
}
bool FileIO::loadTransferFunction(std::string filename, Assign2::InputTransferFunction& out_settingTF) {
	return loadSettingFileTemplate(filename, out_settingTF);
}
bool FileIO::loadLight(std::string filename, Assign2::InputLight& out_settingLight) {
	return loadSettingFileTemplate(filename, out_settingLight);
}
bool FileIO::loadGroups(std::string filename, Assign2::InputGroup& out_settingGroups) {
	return loadSettingFileTemplate(filename, out_settingGroups);
}
bool FileIO::loadNodeNetwork(std::string filename, Assign2::InputNode& out_settingNodes) {
	return loadSettingFileTemplate(filename, out_settingNodes);
}
bool FileIO::loadGeneral(std::string filename, Assign2::InputGeneral& out_settingGeneral) {
	return loadSettingFileTemplate(filename, out_settingGeneral);
}

template <typename T>
void saveSettingFileTemplate(std::string filename, T& settingFile) {
	settingFile.save(filename);
}
void FileIO::saveConfig(std::string filename, Assign2::InputConfiguration& out_settingConfig) {
	saveSettingFileTemplate(filename, out_settingConfig);
}
void FileIO::saveTransferFunction(std::string filename, Assign2::InputTransferFunction& out_settingTF) {
	saveSettingFileTemplate(filename, out_settingTF);
}
void FileIO::saveLight(std::string filename, Assign2::InputLight& out_settingLight) {
	saveSettingFileTemplate(filename, out_settingLight);
}
void FileIO::saveGroups(std::string filename, Assign2::InputGroup& out_settingGroups) {
	saveSettingFileTemplate(filename, out_settingGroups);
}
void FileIO::saveNodeNetwork(std::string filename, Assign2::InputNode& out_settingNodes) {
	saveSettingFileTemplate(filename, out_settingNodes);
}
void FileIO::saveGeneral(std::string filename, Assign2::InputGeneral& out_settingGeneral) {
	saveSettingFileTemplate(filename, out_settingGeneral);
}


