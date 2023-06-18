#include "FileInputData.h"

namespace Assign2
{
	void InputConfiguration::clear() {
		configFile = "";
		inputFile = "";
		inputDir = "";
		inputSlices.clear();
		inputFileSuffix = "";
		pathRelativeTo = "";

		groupFile = "";
		transferFuncFile = "";
		screenFile = "";
		nodeFile = "";
		maskFile = "";
		offsetFile = "";

		inputType = DataInput::DATA_UNSET;
		sceneScale = -1;
		maskEnabled = true;
		volumeCompressionEnabled = true;
		convertEndian = false;
		convertBackgroundValue = false;

		spacings[0] = 1;
		spacings[1] = 1;
		spacings[2] = 1;

		dimensions[0] = 0;
		dimensions[1] = 0;
		dimensions[2] = 0;
	}

	bool InputConfiguration::checkInputs() const
	{
		bool ok = true;
		ok = ok && (inputFile != "");
		ok = ok && (transferFuncFile != "");
		ok = ok && (groupFile != "");
		ok = ok && (screenFile != "");
		ok = ok && (inputType != DATA_UNSET);
		//ok = ok && (useIntervals != UNSET);
		//ok = ok && (sceneScale > 0);	//Set to auto-set if either 0 or negative

		for (int i = 0; i < 3; ++i)
		{
			ok = ok && (spacings[i] > 0);
			ok = ok && (dimensions[i] > 0);
		}

		return ok;
	}


	std::string appendTokenStrings(std::vector<std::string>::iterator tokenIter, std::vector<std::string>::iterator end) {
		std::string str = "";
		tokenIter++;
		while (tokenIter != end) {
			str += *tokenIter + " ";
			tokenIter++;
		}
		str.pop_back();	//remove last character
		return str;
	}
	
	bool InputConfiguration::load(const std::vector<std::string >& strings)
	{
		auto lineIter = strings.begin();

		while (lineIter != strings.end())
		{
			std::string curr = (*lineIter);
			++lineIter;

			std::vector<std::string> tokens;
			std::istringstream iss(curr);
			std::copy(std::istream_iterator< std::string >(iss),
				std::istream_iterator< std::string >(),
				std::back_inserter< std::vector< std::string > >(tokens)
				);

			if (tokens.empty())
				continue;

			auto tokenIter = tokens.begin();

			curr = *tokenIter;

			if (curr[0] == '#' || curr[0] == ';')
				continue;

			if (curr == "-inputFile") {
				inputFile = appendTokenStrings(tokenIter, tokens.end());
			}
			else if (curr == "-inputDir") {
				inputDir = appendTokenStrings(tokenIter, tokens.end());
			}
			//else if (curr == "-pathRelativeTo") {
			//	pathRelativeTo = *(++tokenIter);
			//}

			else if (curr == "-colormap") {
				groupFile = appendTokenStrings(tokenIter, tokens.end());
			}

			else if (curr == "-transferFuncFile") {
				transferFuncFile = appendTokenStrings(tokenIter, tokens.end());
			}

			else if (curr == "-screenFile") {
				screenFile = appendTokenStrings(tokenIter, tokens.end());
			}

			else if (curr == "-maskFile") {
				maskFile = appendTokenStrings(tokenIter, tokens.end());
			}
			else if (curr == "-offsetFile") {
				offsetFile = appendTokenStrings(tokenIter, tokens.end());
			}

			else if (curr == "-nodeFile") {
				nodeFile = appendTokenStrings(tokenIter, tokens.end());
			}

			else if (curr == "-inputResolution") {
				std::istringstream parser(*(++tokenIter));
				std::string dimension;

				int i = 0;
				while (std::getline(parser, dimension, ','))
				{
					dimensions[i++] = std::stoi(dimension);
				}
			}
			else if (curr == "-inputSpacing") {
				std::istringstream parser(*(++tokenIter));
				std::string dimension;

				int i = 0;
				while (std::getline(parser, dimension, ','))
				{
					spacings[i++] = std::stof(dimension);
				}
			}
			else if (curr == "-inputType") {
				std::string type = *(++tokenIter);
				if (type == "uint8")
					inputType = DataInput::UINT8;
				else if (type == "uint16")
					inputType = DataInput::UINT16;
			}

			else if (curr == "-sceneScale") {
				sceneScale = std::stof(*(++tokenIter));
			}

			else if (curr == "-maskEnabled") {
				maskEnabled = std::stoi(*(++tokenIter));
			}

			else if (curr == "-volumeCompressionEnabled") {
				volumeCompressionEnabled = std::stoi(*(++tokenIter));
			}

			else if (curr == "-convertEndian") {
				convertEndian = std::stoi(*(++tokenIter));
			}

			else if (curr == "-swapEndian") {	//***To be deleted
				convertEndian = std::stoi(*(++tokenIter));
			}

			else if (curr == "-whiteisBackground") {
				convertBackgroundValue = std::stoi(*(++tokenIter));
			}

			else
			{
				qDebug() << curr.data() << " not recognized input from file.";
			}
		}

		return true;
	}

	bool InputConfiguration::save(std::string filename) {
		std::string str;
		std::ofstream myfile;
		myfile.open(filename);

		myfile << "-transferFuncFile " << transferFuncFile << "\n";
		myfile << "-colormap " << groupFile << "\n";
		myfile << "-screenFile " << screenFile << "\n";
		myfile << "-nodeNetworkFile " << nodeFile << "\n";
		myfile << "-maskFile " << maskFile << "\n";
		myfile << "-offsetFile " << offsetFile << "\n";

		myfile << "-inputFile " << inputFile << "\n";
		myfile << "-inputResolution " << dimensions[0] << "," << dimensions[1] << "," << dimensions[2] << "\n";
		if (inputType == 0) { str = "uint8"; }
		else if (inputType == 0) { str = "uint16"; }
		else { str = ""; }
		myfile << "-inputType " << str << "\n";
		myfile << "-inputSpacing " << spacings[0] << "," << spacings[1] << "," << spacings[2] << "\n";
		myfile << "\n";

		myfile << "-sceneScale " << sceneScale << "\n";
		myfile << "\n";

		myfile << "-volumeCompressionEnabled " << volumeCompressionEnabled << "\n";
		myfile << "-maskEnabled " << maskEnabled << "\n";
		myfile << "-convertEndian " << convertEndian << "\n";

		myfile.close();

		return true;
	}

	void InputLight::clear() {
		lockIDPositions = false;
		autoApplyUpdates = false;
		raycastStepSize = 0;
		raycastMaxSteps = 0;
		aoStepSize = 0;
		aoMaxSteps = 0;

		intensityAmbient = 0;
		intensityDiffuse = 0;
		intensitySpecular = 0;
		specularExponent = 0;
	}
	bool InputLight::load(const std::vector<std::string >& lines) {
		//Clear any existing tf points and settings
		linearIntensityTF.clear();
		auto lineIter = lines.begin();
		while (lineIter != lines.end())
		{
			std::string curr = (*lineIter);
			++lineIter;

			std::vector<std::string> tokens;
			std::istringstream iss(curr);
			std::copy(std::istream_iterator< std::string >(iss),
				std::istream_iterator< std::string >(),
				std::back_inserter< std::vector< std::string > >(tokens)
			);

			if (tokens.empty())
				continue;

			auto tokenIter = tokens.begin();

			curr = *tokenIter;

			if (curr[0] == '#' || curr[0] == ';')
				continue;
			else if (curr == "-autoApplyUpdates") {
				autoApplyUpdates = std::stoi(*(++tokenIter));
			}
			else if (curr == "-lockIDPositions") {
				lockIDPositions = std::stoi(*(++tokenIter));
			}
			else if (curr == "-raycastStepSize") {
				raycastStepSize = std::stof(*(++tokenIter));
			}
			else if (curr == "-raycastMaxSteps") {
				raycastMaxSteps = std::stoi(*(++tokenIter));
			}
			else if (curr == "-aoStepSize") {
				aoStepSize = std::stof(*(++tokenIter));
			}
			else if (curr == "-aoMaxSteps") {
				aoMaxSteps = std::stoi(*(++tokenIter));
			}
			else if (curr == "-intensityAmbient") {
				intensityAmbient = std::stof(*(++tokenIter));
			}
			else if (curr == "-intensityDiffuse") {
				intensityDiffuse = std::stof(*(++tokenIter));
			}
			else if (curr == "-intensitySpecular") {
				intensitySpecular = std::stof(*(++tokenIter));
			}
			else if (curr == "-specularExponent") {
				specularExponent = std::stof(*(++tokenIter));
			}
			else if (curr == "-linear_tf1D") {
				std::string value;

				Assign2::ColorPoint1D intensityPoint;

				intensityPoint.point = std::stof(*(++tokenIter));
				int i = 0;
				std::istringstream parser(*(++tokenIter));
				i = 0;
				while (std::getline(parser, value, ','))
				{
					intensityPoint.rgba[i++] = std::stoi(value);
				}
				if (i != 4)
				{
					qDebug() << curr.data() << " Incompatable TransferFunctions.";
					exit(1);
				}

				linearIntensityTF.push_back(intensityPoint);
			}
		}

		//sort transfer function points
		std::sort(linearIntensityTF.begin(),
			linearIntensityTF.end(),
			[](Assign2::ColorPoint1D const& a,
				Assign2::ColorPoint1D const& b)
			{ return a.point < b.point; }
		);

		return true;

	}
	bool InputLight::save(std::string filename) {
		std::ofstream myfile;
		myfile.open(filename);
		myfile << "-autoApplyUpdates " << autoApplyUpdates << "\n";
		myfile << "-lockIDPositions " << lockIDPositions << "\n";
		myfile << "-raycastStepSize " << raycastStepSize << "\n";
		myfile << "-raycastMaxSteps " << raycastMaxSteps << "\n";
		myfile << "-aoStepSize " << aoStepSize << "\n";
		myfile << "-aoMaxSteps " << aoMaxSteps << "\n";
		myfile << "-intensityAmbient " << intensityAmbient << "\n";
		myfile << "-intensityDiffuse " << intensityDiffuse << "\n";
		myfile << "-intensitySpecular " << intensitySpecular << "\n";
		myfile << "-specularExponent " << specularExponent << "\n";

		for (int i = 0; i < linearIntensityTF.size(); i++) {
			Assign2::ColorPoint1D p = linearIntensityTF[i];
			myfile << "-linear_tf1D " << p.point << " " << (float)p.rgba[0] << "," << (float)p.rgba[1] << "," << (float)p.rgba[2] << "," << (float)p.rgba[3] << "\n";
		}
		myfile.close();
		return true;
	}

	void InputTransferFunction::clear() {
		useIntervals = TransferFunc::UNSET;
		shiftTF = 0;
		minTF = -1;
		maxTF = -1;
		isoMinPaint = 0;
		isoMinHaptics = 0;
		autoAdjustIsoMin = false;
		lockIDPositions = false;
		autoApplyUpdates = false;

		// Set default control points
		linearIntensityTF.push_back(ColorPoint1D(100, 100, 100, 100,-999999));
		linearIntensityTF.push_back(ColorPoint1D(255, 255, 255, 255, 999999));
	}

	bool InputTransferFunction::load(const std::vector< std::string >& lines) {
		//Clear any existing tf points and settings
		linearIntensityTF.clear();
		intensityTF.clear();		
		useIntervals = Assign2::TransferFunc::UNSET;

		auto lineIter = lines.begin();
		while (lineIter != lines.end())
		{
			std::string curr = (*lineIter);
			++lineIter;

			std::vector<std::string> tokens;
			std::istringstream iss(curr);
			std::copy(std::istream_iterator< std::string >(iss),
				std::istream_iterator< std::string >(),
				std::back_inserter< std::vector< std::string > >(tokens)
				);

			if (tokens.empty())
				continue;

			auto tokenIter = tokens.begin();

			curr = *tokenIter;

			if (curr[0] == '#' || curr[0] == ';')
				continue;


			else if (curr == "-minTF") {
				minTF = std::stoi(*(++tokenIter));
			}
			else if (curr == "-maxTF") {
				maxTF = std::stoi(*(++tokenIter));
			}

			else if (curr == "-shiftTF") {
				shiftTF = std::stof(*(++tokenIter));
			}

			else if (curr == "-isoMinPaint") {
				isoMinPaint = std::stoi(*(++tokenIter));
			}
			else if (curr == "-isoMinHaptics") {
				isoMinHaptics = std::stoi(*(++tokenIter));
			}

			else if (curr == "-autoAdjustIsoMin") {
				autoAdjustIsoMin = std::stoi(*(++tokenIter));
			}

			else if (curr == "-lockIDPositions") {
				lockIDPositions = std::stoi(*(++tokenIter));
			}

			else if (curr == "-autoApplyUpdates") {
				autoApplyUpdates = std::stoi(*(++tokenIter));
			}


			else if (curr == "-linear_tf1D") {

				//Set transfer function to state "LINEAR_TF1D"
				if (useIntervals == Assign2::TransferFunc::UNSET)
					useIntervals = Assign2::TransferFunc::LINEAR_TF1D;

				//Reject TF if not the same state
				if (useIntervals != Assign2::TransferFunc::LINEAR_TF1D)
				{
					qDebug() << QString::fromStdString(curr) << " Incompatable TransferFunctions.";
					exit(1);
				}

				std::string value;

				Assign2::ColorPoint1D intensityPoint;

				intensityPoint.point = std::stof(*(++tokenIter));
				int i = 0;
				std::istringstream parser(*(++tokenIter));
				i = 0;
				while (std::getline(parser, value, ','))
				{
					intensityPoint.rgba[i++] = std::stoi(value);
				}
				if (i != 4)
				{
					qDebug() << curr.data() << " Incompatable TransferFunctions.";
					exit(1);
				}

				linearIntensityTF.push_back(intensityPoint);
			}

			else if (curr == "-tf1D") {
				//Set transfer function to state "TF1D"
				if (useIntervals == Assign2::TransferFunc::UNSET)
					useIntervals = Assign2::TransferFunc::TF1D;


				//Reject TF if not the same state
				if (useIntervals != Assign2::TransferFunc::TF1D)
				{
					qDebug() << QString::fromStdString(curr) << " Incompatable TransferFunctions.";
					exit(1);
				}

				std::istringstream parser(*(++tokenIter));
				std::string value;

				Assign2::ColorInterval1D intensityInverval;

				int i = 0;
				while (std::getline(parser, value, ','))
				{
					intensityInverval.endpoint[i++] = std::stof(value);
				}
				if (i != 2)
				{
					qDebug() << curr.data() << " Incompatable TransferFunctions.";
					exit(1);
				}

				parser.clear();
				parser.str(*(++tokenIter));
				i = 0;
				while (std::getline(parser, value, ','))
				{
					intensityInverval.rgba[i++] = std::stoi(value);
				}
				if (i != 4)
				{
					qDebug() << curr.data() << " Incompatable TransferFunctions.";
					exit(1);
				}

				intensityTF.push_back(intensityInverval);
			}
		}

		//sort transfer function points
		std::sort(linearIntensityTF.begin(),
			linearIntensityTF.end(),
			[](Assign2::ColorPoint1D const& a,
				Assign2::ColorPoint1D const& b)
			{ return a.point < b.point; }
		);

		return true;
	}

	bool InputTransferFunction::save(std::string filename) {
		std::ofstream myfile;
		myfile.open(filename);
		myfile << ";Volume Transfer Function\n";
		myfile << ";Created on \"SHVR\". \n\n";

		myfile << "-minTF " << minTF << "\n";

		myfile << "-maxTF " << maxTF << "\n";

		myfile << "-shiftTF " << shiftTF << "\n";

		myfile << "-isoMinPaint " << isoMinPaint << "\n";
		myfile << "-isoMinHaptics " << isoMinHaptics << "\n";
		myfile << "-autoAdjustIsoMin " << autoAdjustIsoMin << "\n";
		myfile << "-lockIDPositions " << lockIDPositions << "\n";
		myfile << "-autoApplyUpdates " << autoApplyUpdates << "\n";

		for (int i = 0; i < linearIntensityTF.size(); i++) {
			Assign2::ColorPoint1D p = linearIntensityTF[i];
			myfile << "-linear_tf1D " << p.point << " " << (float)p.rgba[0] << "," << (float)p.rgba[1] << "," << (float)p.rgba[2] << "," << (float)p.rgba[3] << "\n";
		}
		myfile.close();

		return true;
	}
	
	void InputGroup::clear() {
		size = 0;
		maskIntensity = 0;
		displayMask.clear();
		displayObject.clear();
		rgba.clear();
		linkScale.clear();
		densityOffset.clear();
		paintOver.clear();
	}

	bool InputGroup::load(const std::vector< std::string >& lines) {
		unsigned char r, g, b, a;
		size = 0;
		auto lineIter = lines.begin();
		while (lineIter != lines.end())
		{
			std::string curr = (*lineIter);
			++lineIter;

			std::vector<std::string> tokens;
			std::istringstream iss(curr);
			std::copy(std::istream_iterator< std::string >(iss),
				std::istream_iterator< std::string >(),
				std::back_inserter< std::vector< std::string > >(tokens)
				);

			if (tokens.empty())
				continue;

			auto tokenIter = tokens.begin();

			curr = *tokenIter;

			if (curr[0] == '#' || curr[0] == ';')
				continue;
						
			else if (curr == "-g") {
				std::istringstream parser(*(++tokenIter));
				std::string num;

				std::getline(parser, num, ',');
				displayMask.push_back(std::stoi(num));

				std::getline(parser, num, ',');
				displayObject.push_back(std::stoi(num));

				std::getline(parser, num, ',');				
				r = std::stoi(num);
				std::getline(parser, num, ',');
				g = std::stoi(num);
				std::getline(parser, num, ',');
				b = std::stoi(num);
				std::getline(parser, num, ',');
				a = std::stoi(num);
				rgba.push_back({ r,g,b,a });

				std::getline(parser, num, ',');
				linkScale.push_back(std::stof(num));

				std::getline(parser, num, ',');
				if (num != "") { paintOver.push_back(std::stoi(num)); }
				else { paintOver.push_back(true); }

				size++;
			}

			else if (curr == "-maskIntensity") {
				maskIntensity = std::stof(*(++tokenIter));
			}

		}
		return true;
	}

	bool InputGroup::save(std::string filename) {
		std::ofstream myfile;
		myfile.open(filename);
		myfile << ";===COLOR GROUP INFORMATION===\n";
		myfile << "-maskIntensity " << maskIntensity << "\n";

		myfile << ";-group maskEnabled, objEnabled, opacity, linkSize\n";
		for (int i = 0; i < size; i++) {
			myfile << "-g "
				<< (int)displayMask[i] << ","
				<< (int)displayObject[i] << ","
				<< (int)rgba[i][0] << ","
				<< (int)rgba[i][1] << ","
				<< (int)rgba[i][2] << ","
				<< (int)rgba[i][3] << ","
				<< linkScale[i] << ","
				<< paintOver[i] << "," << "\n";
				//TODO: Add densityOffset
		}

		myfile.close();

		return true;
	}

	void InputGeneral::clear() {
		// VolumeM -> identity matrix
		volumeM[0] = 1;
		volumeM[1] = 0;
		volumeM[2] = 0;
		volumeM[3] = 0;

		volumeM[4] = 0;
		volumeM[5] = 1;
		volumeM[6] = 0;
		volumeM[7] = 0;

		volumeM[8] = 0;
		volumeM[9] = 0;
		volumeM[10] = 1;
		volumeM[11] = 0;

		volumeM[12] = 0;
		volumeM[13] = 0;
		volumeM[14] = 0;
		volumeM[15] = 1;

		// Other settings
		cutMax = glm::vec3(1, 1, 1);
		cutMin = glm::vec3(0,0,0);
		fov = 30.f;
		nearPlane = 0.001f;
		farPlane = 10.f;
		clipPlaneNear = 0.02f;
		clipPlaneFar = -1.f;

		workspaceScale = 1.;
		compressionRatio = 0.5;
		fps = 30;

		cursorScale = 2.5f;
		silhouetteScale = 1.3f;
		paintScale = 2.0f;

		hapticSpringForce = 400.0;
		hapticDampingForce = 100.0;
		hapticHitThreshold = 0.4;

		hudEnabled = 0;
		arrowEnabled = 1;
		proxyEnabled = 0;
		volumeEnabled = 1;
		lineEnabled = 1;
		objectEnabled = 1;
		forceLineEnabled = 0;

		screenType = 0;
		paintType = 0;
		hapticType = 0;
		offsetType = 0;
		blendRate = 0.5;
		blendSize = 0.0;

		selectedGroup = 1;
		paintKernelSize = 1;

		ssKeepAspectRatio = 1;
		ssWidth = 4000;
		ssHeight = 0;
		maxNumberOfCommands = 10;

	}

	bool InputGeneral::load(const std::vector< std::string >& lines) {
		auto lineIter = lines.begin();

		while (lineIter != lines.end())
		{
			std::string curr = (*lineIter);
			++lineIter;

			std::vector<std::string> tokens;
			std::istringstream iss(curr);
			std::copy(std::istream_iterator< std::string >(iss),
				std::istream_iterator< std::string >(),
				std::back_inserter< std::vector< std::string > >(tokens)
				);

			if (tokens.empty())
				continue;

			auto tokenIter = tokens.begin();

			curr = *tokenIter;

			if (curr[0] == '#' || curr[0] == ';')
				continue;


			else if (curr == "-fov") {
				fov = std::stof(*(++tokenIter));
			}

			else if (curr == "-nearPlane") {
				nearPlane = std::stof(*(++tokenIter));
			}

			else if (curr == "-farPlane") {
				farPlane = std::stof(*(++tokenIter));
			}

			else if (curr == "-volumeM") {
				std::istringstream parser(*(++tokenIter));
				std::string num;
				int i = 0;

				while (std::getline(parser, num, ','))
				{
					volumeM[i++] = std::stof(num);
				}
			}

			else if (curr == "-workspaceScale") {
				workspaceScale = std::stof(*(++tokenIter));
			}

			else if (curr == "-cutPlane") {		//Cutting Plane
				std::istringstream parser(*(++tokenIter));
				std::string num;

				std::getline(parser, num, ',');
				cutMax[0] = std::stof(num);
				std::getline(parser, num, ',');
				cutMax[1] = std::stof(num);
				std::getline(parser, num, ',');
				cutMax[2] = std::stof(num);
				std::getline(parser, num, ',');
				cutMin[0] = std::stof(num);
				std::getline(parser, num, ',');
				cutMin[1] = std::stof(num);
				std::getline(parser, num, ',');
				cutMin[2] = std::stof(num);
			}

			else if (curr == "-clipPlaneNear") {
				clipPlaneNear = std::stof(*(++tokenIter));
			}

			else if (curr == "-clipPlaneFar") {
				clipPlaneFar = std::stof(*(++tokenIter));
			}

			else if (curr == "-cursorScale") {
				cursorScale = std::stof(*(++tokenIter));
			}

			else if (curr == "-silhouetteScale") {
				silhouetteScale = std::stof(*(++tokenIter));
			}

			else if (curr == "-paintScale") {
				paintScale = std::stof(*(++tokenIter));
			}

			else if (curr == "-arrowEnabled") {
				arrowEnabled = std::stoi(*(++tokenIter));
			}

			else if (curr == "-lineEnabled") {
				lineEnabled = std::stoi(*(++tokenIter));
			}

			else if (curr == "-volumeEnabled") {
				volumeEnabled = std::stoi(*(++tokenIter));
			}

			else if (curr == "-hudEnabled") {
				hudEnabled = std::stoi(*(++tokenIter));
			}

			else if (curr == "-proxyEnabled") {
				proxyEnabled = std::stoi(*(++tokenIter));
			}

			else if (curr == "-forceLineEnabled") {
				forceLineEnabled = std::stoi(*(++tokenIter));
			}			

			else if (curr == "-screenType") {
				screenType = std::stoi(*(++tokenIter));
			}			

			else if (curr == "-paintType") {
				paintType = std::stoi(*(++tokenIter));
			}

			else if (curr == "-hapticType") {
				hapticType = std::stoi(*(++tokenIter));
			}

			else if (curr == "-offsetType") {
				offsetType = std::stoi(*(++tokenIter));
			}
			else if (curr == "-blendRate") {
				blendRate = std::stof(*(++tokenIter));
			}
			else if (curr == "-blendSize") {
				blendSize = std::stof(*(++tokenIter));
			}
			
			else if (curr == "-groupNum") {
				selectedGroup = std::stoi(*(++tokenIter));
			}

			else if (curr == "-hapticConfidenceRate") {
				hapticHitThreshold = std::stof(*(++tokenIter));
			}

			else if (curr == "-objectEnabled") {
				objectEnabled = std::stoi(*(++tokenIter));
			}
			else if (curr == "-fps") {
				objectEnabled = std::stof(*(++tokenIter));
			}
			else if (curr == "-compressionRatio") {
				objectEnabled = std::stof(*(++tokenIter));
			}
			else if (curr == "-paintKernelSize") {
				paintKernelSize = std::stoi(*(++tokenIter));
			}
			else if (curr == "-ssKeepAspectRatio") {
				ssKeepAspectRatio = std::stoi(*(++tokenIter));
			}
			else if (curr == "-ssWidth") {
				ssWidth = std::stoi(*(++tokenIter));
			}
			else if (curr == "-ssHeight") {
				ssHeight = std::stoi(*(++tokenIter));
			}
			else if (curr == "-maxNumberOfCommands") {
				maxNumberOfCommands = std::stoi(*(++tokenIter));
			}
			
		}

		return true;
	}

	bool InputGeneral::save(std::string filename) {
		std::ofstream myfile;
		myfile.open(filename);
		myfile << ";===VOLUME SCREEN INFORMATION===\n";

		myfile << ";Projection Matrix\n"
			<< "-fov " << fov << "\n"
			<< "-nearPlane " << nearPlane << "\n"
			<< "-farPlane " << farPlane << "\n"
			<< "\n"
			<< ";Volume Matrix\n"
			<< "-volumeM " << volumeM[0] << "," << volumeM[1] << "," << volumeM[2] << "," << volumeM[3] << ","
			<< volumeM[4] << "," << volumeM[5] << "," << volumeM[6] << "," << volumeM[7] << ","
			<< volumeM[8] << "," << volumeM[9] << "," << volumeM[10] << "," << volumeM[11] << ","
			<< volumeM[12] << "," << volumeM[13] << "," << volumeM[14] << "," << volumeM[15] << "\n"
			<< "-workspaceScale " << workspaceScale << "\n"
			<< "\n"
			<< ";Clipping Plane\n"
			<< "-cutPlane " << cutMax[0] << "," << cutMax[1] << "," << cutMax[2] << ","
			<< cutMin[0] << "," << cutMin[1] << "," << cutMin[2] << "\n"
			<< "-clipPlaneNear " << clipPlaneNear << "\n"
			<< "-clipPlaneFar " << clipPlaneFar << "\n"
			<< "\n"
			<< ";Cursor Settings\n"
			<< "-cursorScale " << cursorScale << "\n"
			<< "-silhouetteScale " << silhouetteScale << "\n"
			<< "-paintScale " << paintScale << "\n"
			<< "\n"
			<< ";Haptic Settings\n"
			<< "-hapticType " << (int)hapticType << "\n"
			<< "-hapticSpringForce " << hapticSpringForce << "\n"
			<< "-hapticDampingForce " << hapticDampingForce << "\n"
			<< "-hapticConfidenceRate " << hapticHitThreshold << "\n"
			<< "\n"
			<< ";Rendering Quality\n"
			<< "-fps " << fps << "\n"
			<< "-compressionRatio " << compressionRatio << "\n"
			<< "\n"
			<< ";Other Visual Settings\n"
			<< "-hudEnabled " << hudEnabled << "\n"
			<< "-arrowEnabled " << arrowEnabled << "\n"
			<< "-lineEnabled " << lineEnabled << "\n"
			<< "-volumeEnabled " << volumeEnabled << "\n"
			<< "-objectEnabled " << objectEnabled << "\n"
			<< "-proxyEnabled " << proxyEnabled << "\n"
			<< "-forceLineEnabled " << forceLineEnabled << "\n"
			<< "-screenType " << (int)screenType << "\n"
			<< "-paintType " << (int)paintType << "\n"
			<< "-offsetType" << (int)offsetType << "\n"
			<< "-blendRate" << blendRate << "\n"
			<< "-blendSize" << blendSize << "\n"
			<< "-groupNum " << (int)selectedGroup << "\n"
			<< "-paintKernelSize " << paintKernelSize << "\n"
			<< "-ssKeepAspectRatio " << (int)ssKeepAspectRatio << "\n"
			<< "-ssWidth " << ssWidth << "\n"
			<< "-ssHeight " << ssHeight << "\n"
			<< "-maxNumberOfCommands " << maxNumberOfCommands << "\n";
			
			
			myfile.close();

		return true;
	}

	void InputNode::clear() {
		vertex_position.clear();
		vertex_link.clear();
		vertex_group.clear();
		vertex_scale.clear();
	}

	bool InputNode::load(const std::vector< std::string >& lines) {

		auto lineIter = lines.begin();

		while (lineIter != lines.end())
		{
			std::string curr = (*lineIter);
			++lineIter;

			std::vector<std::string> tokens;
			std::istringstream iss(curr);
			std::copy(std::istream_iterator< std::string >(iss),
				std::istream_iterator< std::string >(),
				std::back_inserter< std::vector< std::string > >(tokens)
				);

			if (tokens.empty())
				continue;

			auto tokenIter = tokens.begin();

			curr = *tokenIter;

			if (curr[0] == '#' || curr[0] == ';')
				continue;

			else if (curr[0] == 'v') {
				std::istringstream parser(*(++tokenIter));
				std::string num;

				std::getline(parser, num, ',');
				float x = std::stof(num);
				std::getline(parser, num, ',');
				float y = std::stof(num);
				std::getline(parser, num, ',');
				float z = std::stof(num);
				std::getline(parser, num, ',');
				int group = std::stoi(num);

				float scale;
				if (std::getline(parser, num, ',')) { scale = std::stof(num); }
				else { scale = 1.0; }


				glm::vec3 vertex(x, y, z);
				vertex_position.push_back(vertex);
				vertex_group.push_back(group);
				vertex_scale.push_back(scale);
			}

			else if (curr[0] == 'l') {
				std::istringstream parser(*(++tokenIter));
				std::string num;

				std::getline(parser, num, ',');
				int a = std::stoi(num);
				std::getline(parser, num, ',');
				int b = std::stoi(num);
				std::getline(parser, num, ',');
				int g = std::stoi(num);				//group

				int* link_index = new int[3];
				link_index[0] = a;
				link_index[1] = b;
				link_index[2] = g;
				vertex_link.push_back(link_index);
			}
		}

		return true;
	}

	bool InputNode::save(std::string filename) {
		std::ofstream myfile;
		myfile.open(filename);

		//write contents of vertices
		for (int i = 0; i < vertex_position.size(); i++) {
			myfile << "v " << vertex_position[i].x <<				//x position
				"," << vertex_position[i].y <<				//y position
				"," << vertex_position[i].z <<				//z position
				"," << vertex_group[i] <<				//group number
				"," << vertex_scale[i] << " \n";
		}

		myfile << "\n";

		//write contents of links
		for (int i = 0; i < vertex_link.size(); i++) {
			myfile << "l " << vertex_link[i][0] <<				//node 0
				"," << vertex_link[i][1] <<				//node 1
				"," << vertex_link[i][2] << " \n";		//group number
		}

		myfile.close();

		return true;
	}

	//bool loadNodesSWC(const std::vector< std::string >& lines, InputNode* _nodes, InputConfiguration* _config) {

	//	float halfwayX = ((_config->sceneScale * _config->boxDimensions[0]) * (double)_config->dimensions[0]) / 2.;
	//	float halfwayY = ((_config->sceneScale * _config->boxDimensions[1]) * (double)_config->dimensions[1]) / 2.;
	//	float halfwayZ = ((_config->sceneScale * _config->boxDimensions[2]) * (double)_config->dimensions[2]) / 2.;

	//	auto lineIter = lines.begin();

	//	while (lineIter != lines.end())
	//	{
	//		std::string curr = (*lineIter);
	//		++lineIter;

	//		std::vector<std::string> tokens;
	//		std::istringstream iss(curr);
	//		std::copy(std::istream_iterator< std::string >(iss),
	//			std::istream_iterator< std::string >(),
	//			std::back_inserter< std::vector< std::string > >(tokens)
	//			);

	//		if (tokens.empty())
	//			continue;

	//		auto tokenIter = tokens.begin();

	//		curr = *tokenIter;

	//		if (curr[0] == '#' || curr[0] == ';')
	//			continue;

	//		else {
	//			int n = std::stoi(tokens[0]);
	//			int group = std::stoi(tokens[1]);
	//			float vertex_indexX = std::stof(tokens[2]);
	//			float vertex_indexY = std::stof(tokens[3]);
	//			float vertex_indexZ = std::stof(tokens[4]);
	//			float scale = std::stof(tokens[5]);
	//			int p = std::stoi(tokens[6]);		//Assumes there is one parent per node

	//			float x = (vertex_indexX * _config->sceneScale * _config->boxDimensions[0]) - halfwayX;
	//			float y = (vertex_indexY * _config->sceneScale * _config->boxDimensions[1]) - halfwayY;
	//			float z = (vertex_indexZ * _config->sceneScale * _config->boxDimensions[2]) - halfwayZ;
	//			glm::vec3 vertex(x, y, z);
	//			_nodes->vertex_position.push_back(vertex);
	//			_nodes->vertex_group.push_back(group);
	//			_nodes->vertex_scale.push_back(scale);

	//			if (p >= 0) {
	//				int* link_index = new int[3];
	//				link_index[0] = p - 1;	//Indices start with 1 while arrays start from 0
	//				link_index[1] = n - 1;
	//				link_index[2] = group;
	//				_nodes->vertex_link.push_back(link_index);
	//			}
	//		}

	//	}

	//	return true;
	//}
	//
	//bool saveNodesSWC(std::string filename, InputNode* _nodes, InputConfiguration* _config) {
	//	std::ofstream myfile;
	//	myfile.open(filename);

	//	float halfwayX = ((_config->sceneScale * _config->boxDimensions[0]) * (double)_config->dimensions[0]) / 2.;
	//	float halfwayY = ((_config->sceneScale * _config->boxDimensions[1]) * (double)_config->dimensions[1]) / 2.;
	//	float halfwayZ = ((_config->sceneScale * _config->boxDimensions[2]) * (double)_config->dimensions[2]) / 2.;

	//	//Assumes one parent per node
	//	std::vector<int> parent_list(_nodes->vertex_position.size(), -1);	//Assumes there is one parent per node

	//	for (int i = 0; i < _nodes->vertex_link.size(); i++) {
	//		int nodeParent_index = _nodes->vertex_link[i][0];
	//		int nodeChild_index = _nodes->vertex_link[i][1];

	//		parent_list[nodeChild_index] = nodeParent_index + 1;	//indices start at 1 in swc files
	//	}

	//	for (int i = 0; i < _nodes->vertex_position.size(); i++) {
	//		float vertex_indexX = (_nodes->vertex_position[i].x + halfwayX) / (_config->sceneScale * _config->boxDimensions[0]);
	//		float vertex_indexY = (_nodes->vertex_position[i].y + halfwayY) / (_config->sceneScale * _config->boxDimensions[0]);
	//		float vertex_indexZ = (_nodes->vertex_position[i].z + halfwayZ) / (_config->sceneScale * _config->boxDimensions[0]);
	//		int unique_index = i + 1;	//indices start at 1 in swc files

	//		myfile << unique_index <<					// unique index
	//			" " << _nodes->vertex_group[i] <<	//group number
	//			" " << vertex_indexX <<	//x position
	//			" " << vertex_indexY <<	//y position
	//			" " << vertex_indexZ <<	//z position	
	//			" " << _nodes->vertex_scale[i] <<	//radius
	//			" " << parent_list[i] <<	//***Parent node??? (Probably will need to set this up better)
	//			"\n";

	//		//DEBUG
	//		//qDebug() << "Indices: " << vertex_indexX << " " << vertex_indexY << " " << vertex_indexZ;

	//	}


	//	myfile.close();

	//	return true;
	//}
}