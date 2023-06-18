#pragma once

#include <QObject>

#include <stdio.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <string>

#include <qdebug.h>
#include "FileInputData.h"

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
//using namespace glm;

#include "mathLib.h"


namespace NN {

	struct Link;
	struct Node;

	enum NState {
		DEFAULT,
		SELECTED,
		NEARBY
	};

	struct NObject {		
		char state;
		char groupNum;			// Should be between -1 to 9
		bool flag_deleted;		// Deleted from list
		bool flag_disabled;		// Temporarily disable rendering
		float scale;
		glm::mat4 M;

		virtual glm::vec3 position() { return glm::vec3(); }
		virtual void updateM() {}
	};

	struct Node : public NObject {
		std::vector<Link*> links;
		glm::vec3 pos;
		int saveIndex; //This is used for simplifying the writing of node network to file
		
		Node(glm::vec3 position,
			int _groupNum = -1,
			float _scale = 1.0);

		void addLink(Link* l);
		glm::vec3 position();
		void updateM();		
		bool hasLink(Node* n);
		Link* getLink(Node* n);

	};

	struct Link : public NObject {
		Node* n0;
		Node* n1;

		Link(Node* node0, Node* node1,
			int _groupNum = -1,
			float _scale = 1.0);

		Node* getOtherNode(Node* n);
		glm::vec3 position();
		void updateM(float worldScale);
	};

	struct NNState {
		Node* sNode;		//selected node
		Link* sLink;		//selected link
		NObject* nearby;	//nearby node/link
		std::vector<Node*> nodes;
		std::vector<Link*> links;

		Node* prev_sNode;
		Link* prev_sLink;
		NObject* prev_nearby;

		NNState() { clear(); }
		~NNState() { clear();}

		void clear();
	
		int getNodeIndex(Node* n);
		int getLinkIndex(Link* l);
		void selectNode(Node* n);
		void selectLink(Link* l);
		void deselect();
		Link* getLinkBtwnNodes(Node* n0, Node* n1);
		NObject* getSelectedNObject() {
			if (sNode != 0) {
				return sNode;
			}
			if (sLink != 0) {
				return sLink;
			}
			return 0;
		}
		bool isNode(NObject* n) {
			for (Node* node : nodes) {
				if (n == node) {
					return true;
				}
			}
			return false;
		}

		bool isLink(NObject* n) {
			for (Link* link: links) {
				if (n == link) {
					return true;
				}
			}
			return false;
		}
		void updatePreviousNObjects() {
			prev_nearby = nearby;
			prev_sNode = sNode;
			prev_sLink = sLink;
		}

		void updateLinkGroupScale(int groupNum, float worldScale, float linkScale) {
			// Update all links with same groupNum
			for (Link* l : links) {
				if (l->groupNum == groupNum) {
					l->scale = linkScale;
					l->updateM(worldScale);
				}
			}
		}

		void updateLinkGroupScale(float worldScale, std::vector<float>& linkScales) {
			// Update multiple group nums
			for (Link* l : links) {
				l->scale = linkScales[l->groupNum];
				l->updateM(worldScale);
			}
		}

	};

	// Functions on nodes
	int findNearestNodeIndex(glm::vec3 pointVS, std::vector<Node*>& list);
	Node* findNearestNode(glm::vec3 pointVS, std::vector<Node*>& list);
	int findNearestLinkIndex(glm::vec3 pointVS, std::vector<Link*>& list);
	Link* findNearestLink(glm::vec3 pointVS, std::vector<Link*>& list);
	bool isNodeNearest(glm::vec3 pointVS, Node* closestNode, Link* closestLink);
	NObject* findNearestNObject(glm::vec3 pointVS, NNState* nn);

	void loadNObjects(Assign2::InputNode* out_nodes, NNState* nn);
	void saveNObjects(Assign2::InputNode* in_nodes, NNState* nn);

	void selectNearestNObject(glm::vec3 pointVS, NNState* nn);
	void addNode(glm::vec3 pointVS, float scale, int groupNum, NNState* nn);
	void addLink(Node*& nStart, Node*& nEnd, float worldScale, float linkScale, NNState* nn);
	void moveNode(Node* n, glm::vec3 pos);
	void deleteNode(Node* n);
	void deleteLink(Link* l);
	void undeleteNode(Node* n);
	void undeleteLink(Link* l);
	void scaleNObject(float scale, NObject* n);
	void changeGroupNObject(int group, NObject* n);

	
	void updateNearestNObjectState(glm::vec3 pointWS, NNState* nn);
	glm::vec3 getTypeColor(glm::vec3 color, int state);
}

