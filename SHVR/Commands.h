#pragma once

#include "VolumeTemplate.h"
#include "volumes.h"
#include "NodeNetwork.h"

struct Command {
	virtual void undo() {};
	virtual void redo() {};
};

struct Command_VolumePaint : public Command {
	VolumeState* vState;
	glm::ivec3 idxOrigin;
	Math::Array3D<char>* affectedVoxels;

	Command_VolumePaint(VolumeState* _vState, glm::ivec3& _idxOrigin, Math::Array3D<char>* _affectedVoxels);
	~Command_VolumePaint();

	void undo();
	void redo();
};

struct Command_VolumeOffset : public Command {
	VolumeState* vState;
	glm::ivec3 idxOrigin;
	Math::Array3D<float>* affectedVoxels;

	Command_VolumeOffset(VolumeState* _vState, glm::ivec3& _idxOrigin, Math::Array3D<float>* _affectedVoxels);
	~Command_VolumeOffset();

	void undo();
	void redo();
};

struct Command_VolumeGroup : public Command {
	Command_VolumeOffset* cOff;
	Command_VolumePaint* cPaint;

	Command_VolumeGroup(Command_VolumeOffset* _cOff, Command_VolumePaint* _cPaint);
	~Command_VolumeGroup();
	void undo();
	void redo();
};

struct Command_NNAddConnectedNode : public Command {
	NN::Node* nOld;
	NN::Node* nNew;
	NN::NNState* nn;

	Command_NNAddConnectedNode(NN::Node* _nNew, NN::Node* _nOld, NN::NNState* _nn) :
		nNew(_nNew), nOld(_nOld), nn(_nn) {		
	}

	void undo() override {
		NN::deleteNode(nNew);
		NN::deleteLink(nNew->links[0]);
		nn->selectNode(nOld);
	}

	void redo() override {
		NN::undeleteNode(nNew);
		NN::undeleteLink(nNew->links[0]);
		nn->selectNode(nNew);
	}
};

struct Command_NNAddNode: public Command {
	NN::Node* n;
	std::vector<NN::Link*> ls;
	NN::NNState* nn;

	Command_NNAddNode(NN::Node* _n, NN::NNState* _nn): n(_n), nn(_nn) {
		for (NN::Link* l : n->links) {
			if (!l->flag_deleted) {
				ls.push_back(l);
			}
		}	
	}

	void undo() override {
		NN::deleteNode(n);
		nn->deselect();
	}

	void redo() override {
		NN::undeleteNode(n);
		for (NN::Link* l : ls) {
			NN::undeleteLink(l);
		}
		nn->selectNode(n);
	}
};

struct Command_NNAddLink : public Command {
	NN::Link* l;
	NN::NNState* nn;

	Command_NNAddLink(NN::Link* _l, NN::NNState* _nn) : l(_l), nn(_nn) {}

	void undo() override {
		NN::deleteLink(l);
		nn->deselect();
	}

	void redo() override {
		NN::undeleteLink(l);
		nn->selectLink(l);
	}
};

struct Command_NNDeleteLink: public Command{
	NN::Link* l;
	NN::NNState* nn;

	Command_NNDeleteLink(NN::Link* _l, NN::NNState* _nn): l(_l), nn(_nn) {}

	void undo() override {
		NN::undeleteLink(l);
		nn->selectLink(l);
	}

	void redo() override {
		NN::deleteLink(l);
		nn->deselect();
	}
};

struct Command_NNDeleteNode: public Command {
	NN::Node* n;
	NN::NNState* nn;
	std::vector<NN::Link*> ls;

	Command_NNDeleteNode(NN::Node* _n, std::vector<NN::Link*>& _ls, NN::NNState* _nn) : n(_n), ls(_ls), nn(_nn) {}

	void undo() override {
		NN::undeleteNode(n);
		nn->selectNode(n);

		//Undelete all connected links
		for (NN::Link* l : ls) {
			undeleteLink(l);
		}
	}

	void redo() override {
		NN::deleteNode(n);
		nn->deselect();


		//Delete all connected links
		for (NN::Link* l : n->links) {
			deleteLink(l);
		}
	}
};

struct Command_NNMoveNode : public Command {
	NN::Node* n;
	glm::vec3 pos;
	float worldScale;
	NN::NNState* nn;

	Command_NNMoveNode(NN::Node* _n, glm::vec3 posOld, NN::NNState* _nn, float _worldScale) : 
		n(_n), pos(posOld), nn(_nn), worldScale(_worldScale){}

	void undo() override {
		std::swap(n->pos, pos);
		n->updateM();
		nn->selectNode(n);
		for (auto* l : n->links) {
			l->updateM(worldScale);
		}

	}

	void redo() override {
		std::swap(n->pos, pos);
		n->updateM();
		nn->selectNode(n);
		for (auto* l : n->links) {
			l->updateM(worldScale);
		}
	}
};

struct Command_NNChangeGroup: public Command {
	//NN::NObject* n;
	//char groupNum;
	//NN::NNState* nn;
	std::vector<NN::NObject*> n;
	std::vector<char> groupNum;
	NN::NNState* nn;


	Command_NNChangeGroup(NN::NObject* _n, char groupNumOld, NN::NNState* _nn) : 
		nn(_nn) {
		n.push_back(_n);
		groupNum.push_back(groupNumOld);
	
	}

	Command_NNChangeGroup(std::vector<NN::NObject*> _n, std::vector<char> groupNumOld, NN::NNState* _nn) :
		n(_n), groupNum(groupNumOld), nn(_nn) {}

	void undo() override {
		for (int i = 0; i < n.size(); i++) {
			std::swap(n[i]->groupNum, groupNum[i]);
		}
		if (nn->isNode(n[0])) {
			nn->selectNode((NN::Node*)n[0]);
		}
		else {
			nn->selectLink((NN::Link*)n[0]);
		}
	}

	void redo() override {
		for (int i = 0; i < n.size(); i++) {
			std::swap(n[i]->groupNum, groupNum[i]);
		}
		if (nn->isNode(n.back())) {
			nn->selectNode((NN::Node*)n.back());
		}
		else {
			nn->selectLink((NN::Link*)n.back());
		}
	}
};

//TODO: Undo/redo node scale

struct CommandStack {
	std::deque<Command*> undoStack;		
	std::deque<Command*> redoStack;
	//std::vector<Command*> undoStack;
	//std::vector<Command*> redoStack;
	unsigned int maxNumberOfCommands = 10;

	void commit(Command* c);
	void undo();
	void redo();
	void clearCommands();
	void updateMaxSize(unsigned int n);
	//====
	Command_VolumeOffset* getCommand_VolumeOffset(VolumeState* vState, float paintMaxOffset, float minVisibleInValue);
	Command_VolumePaint* getCommand_VolumePaint(VolumeState* vState, int paintValue);	
};
