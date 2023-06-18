#include "Commands.h"

Command_VolumePaint::Command_VolumePaint(VolumeState* _vState, glm::ivec3& _idxOrigin, Math::Array3D<char>* _affectedVoxels) :
	vState(_vState),
	affectedVoxels(_affectedVoxels) {
	idxOrigin = _idxOrigin;
}

Command_VolumePaint::~Command_VolumePaint() {
	delete affectedVoxels;
}

void Command_VolumePaint::undo() {
	VolumeMask* mask = vState->vMask;
	for (int k = 0; k < affectedVoxels->depth(); k++) {
		for (int j = 0; j < affectedVoxels->height(); j++) {
			for (int i = 0; i < affectedVoxels->width(); i++) {
				glm::ivec3 idx = idxOrigin + glm::ivec3(i, j, k);
				char valueCurr = mask->valueAtCell(idx);
				char valueDiff = affectedVoxels->at(i, j, k);
				char valueNew = valueCurr - valueDiff;

				mask->setValueAtCell(valueNew, idx);
			}
		}
	}
}

void Command_VolumePaint::redo() {
	VolumeMask* mask = vState->vMask;
	for (int k = 0; k < affectedVoxels->depth(); k++) {
		for (int j = 0; j < affectedVoxels->height(); j++) {
			for (int i = 0; i < affectedVoxels->width(); i++) {
				glm::ivec3 idx = idxOrigin + glm::ivec3(i, j, k);
				char valueCurr = mask->valueAtCell(idx);
				char valueDiff = affectedVoxels->at(i, j, k);
				char valueNew = valueCurr + valueDiff;

				mask->setValueAtCell(valueNew, idx);
			}
		}
	}
}

Command_VolumeOffset::Command_VolumeOffset(VolumeState* _vState, glm::ivec3& _idxOrigin, Math::Array3D<float>* _affectedVoxels) :
	vState(_vState),
	affectedVoxels(_affectedVoxels) {
	idxOrigin = _idxOrigin;				//currently applied group
}

Command_VolumeOffset::~Command_VolumeOffset() {
	delete affectedVoxels;
}

void Command_VolumeOffset::undo() {
	VolumeBase* data = vState->vData;
	VolumeBase* offset = vState->vOffset;
	for (int k = 0; k < affectedVoxels->depth(); k++) {
		for (int j = 0; j < affectedVoxels->height(); j++) {
			for (int i = 0; i < affectedVoxels->width(); i++) {
				glm::ivec3 idxVolume = idxOrigin + glm::ivec3(i, j, k);
				float valueVolume = offset->valueAtCell(idxVolume);
				float valueDiff = affectedVoxels->at(i, j, k);
				float valueNew = valueVolume - valueDiff;

				vState->removeVoxelOffset(idxVolume);
				vState->applyVoxelOffset(valueNew, idxVolume);
			}
		}
	}
}

void Command_VolumeOffset::redo() {
	VolumeBase* data = vState->vData;
	VolumeBase* offset = vState->vOffset;
	for (int k = 0; k < affectedVoxels->depth(); k++) {
		for (int j = 0; j < affectedVoxels->height(); j++) {
			for (int i = 0; i < affectedVoxels->width(); i++) {
				glm::ivec3 idxVolume = idxOrigin + glm::ivec3(i, j, k);
				float valueVolume = offset->valueAtCell(idxVolume);
				float valueDiff = affectedVoxels->at(i, j, k);
				float valueNew = valueVolume + valueDiff;

				vState->removeVoxelOffset(idxVolume);
				vState->applyVoxelOffset(valueNew, idxVolume);
			}
		}
	}
}

Command_VolumeGroup::Command_VolumeGroup(Command_VolumeOffset* _cOff, Command_VolumePaint* _cPaint) {
	cOff = _cOff;
	cPaint = _cPaint;
}

Command_VolumeGroup::~Command_VolumeGroup() {
	delete cOff;
	delete cPaint;
}

void Command_VolumeGroup::undo() {
	cOff->undo();
	cPaint->undo();
}

void Command_VolumeGroup::redo() {
	cOff->redo();
	cPaint->redo();
}


void CommandStack::commit(Command* c) {
	// Remove elements from back if exceeded
	if (undoStack.size() >= maxNumberOfCommands) {
		delete undoStack.front();
		undoStack.pop_front();
	}

	// Commit layers into the undo stack		
	undoStack.push_back(c);

	// Clear existing redo stack
	for (Command* c_redo : redoStack) {
		delete c_redo;
	}
	redoStack.clear();
}

void CommandStack::undo() {
	if (!undoStack.empty()) {
		Command* c = undoStack.back();
		c->undo();
		redoStack.push_back(c);
		undoStack.pop_back();
	}
}

void CommandStack::redo() {
	if (!redoStack.empty()) {
		Command* c = redoStack.back();
		c->redo();
		undoStack.push_back(c);
		redoStack.pop_back();
	}
}

void CommandStack::clearCommands() {
	for (Command* c_undo : undoStack) {
		delete c_undo;
	}
	undoStack.clear();

	for (Command* c_redo : redoStack) {
		delete c_redo;
	}
	redoStack.clear();
}

void CommandStack::updateMaxSize(unsigned int n) {
	maxNumberOfCommands = n;
	clearCommands();
}

Command_VolumeOffset* CommandStack::getCommand_VolumeOffset(VolumeState* vState, float paintMaxOffset, float minVisibleInValue) {
	glm::ivec3 originMin = vState->idxMin;
	glm::ivec3 dimensions = vState->idxMax - vState->idxMin;

	Math::Array3D<float>* affectedOffsetTotal = new Math::Array3D<float>();
	if (vState->affectedOffset.size() > 0) {
		affectedOffsetTotal->resize(dimensions.x, dimensions.y, dimensions.z, 0.f);

		//Merge changes to voxels from when the button was pressed, up to when the button is released
		for (int i = 0; i < vState->affectedOffset.size(); i++) {
			//Set offset to the newest offset
			glm::ivec3 idx = vState->affectedOffset[i] - originMin;
			affectedOffsetTotal->at(idx.x, idx.y, idx.z) += vState->affectedOffsetDiff[i];
		}

		vState->affectedOffset.clear();
		vState->affectedOffsetDiff.clear();

		//Find neighbouring voxels (ie. voxels beside voxels above invalue)
		vState->updateNeighbouringVoxelsOffsets(paintMaxOffset, minVisibleInValue, affectedOffsetTotal);

		//Merge changes to voxels from when the button was pressed, up to when the button is released
		for (int i = 0; i < vState->affectedOffset.size(); i++) {
			//Set offset to the newest offset
			glm::ivec3 idx = vState->affectedOffset[i] - originMin;
			affectedOffsetTotal->at(idx.x, idx.y, idx.z) += vState->affectedOffsetDiff[i];
		}

		vState->affectedOffset.clear();
		vState->affectedOffsetDiff.clear();
	}

	auto cOff = new Command_VolumeOffset(vState, originMin, affectedOffsetTotal);
	return cOff;
}

Command_VolumePaint* CommandStack::getCommand_VolumePaint(VolumeState* vState, int paintValue) {
	glm::ivec3 originMin = vState->idxMin;
	glm::ivec3 dimensions = vState->idxMax - vState->idxMin;

	Math::Array3D<char>* affectedTotal = new Math::Array3D<char>();
	if (vState->affectedPaintTotal.size() > 0) {
		affectedTotal->resize(dimensions.x, dimensions.y, dimensions.z, 0.f);

		//Merge changes to voxels from when the button was pressed, up to when the button is released
		for (int i = 0; i < vState->affectedPaintTotal.size(); i++) {
			//Get the difference between the two 
			glm::ivec3 idx = vState->affectedPaintTotal[i] - originMin;
			affectedTotal->at(idx.x, idx.y, idx.z) += vState->affectedPaintDiff[i];
		}

		vState->affectedPaintTotal.clear();
		vState->affectedPaintDiff.clear();

		//Find neighbouring voxels (ie. voxels beside voxels above invalue)
		vState->updateNeighbouringVoxelsPaint(paintValue, affectedTotal);

		//Merge changes to voxels from when the button was pressed, up to when the button is released
		for (int i = 0; i < vState->affectedPaintTotal.size(); i++) {
			//Set offset to the newest offset
			glm::ivec3 idx = vState->affectedPaintTotal[i] - originMin;
			affectedTotal->at(idx.x, idx.y, idx.z) += vState->affectedPaintDiff[i];
		}

		vState->affectedPaintTotal.clear();
		vState->affectedPaintDiff.clear();
	}

	auto cPaint = new Command_VolumePaint(vState, originMin, affectedTotal);
	return cPaint;
}