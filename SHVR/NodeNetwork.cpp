#include "NodeNetwork.h"
using namespace NN;


Node::Node(glm::vec3 position,
	int _groupNum,
	float _scale) //Default group num
{

	state = NState::DEFAULT;
	saveIndex = -1;			//not indexed yet 
	groupNum = _groupNum;
	flag_deleted = false;
	flag_disabled = false;

	pos = position;
	scale = _scale;
	updateM();
}

void Node::addLink(Link* l) {
	links.push_back(l);
}

glm::vec3 Node::position() {
	return pos;
}

void Node::updateM() {
	glm::mat4 translateM = MathLib::translateM(pos);
	glm::mat4 scaleM = glm::scale(scale, scale, scale);
	M = translateM * scaleM;
}
bool Node::hasLink(Node* n) {
	for (Link* l : links) {
		if (l->n0 == n || l->n1 == n) {
			return true;
		}
	}
	return false;
}

Link* Node::getLink(Node* n) {
	for (Link* l : links) {
		if (l->n0 == n || l->n1 == n) {
			return l;
		}
	}
	return 0;
}
//======
Link::Link(Node* node0, Node* node1,
	int _groupNum,
	float _scale)
{
	n0 = node0;
	n1 = node1;
	state = NState::DEFAULT;
	groupNum = _groupNum;
	scale = _scale;
	n0->addLink(this);
	n1->addLink(this);

	flag_deleted = false;
	flag_disabled = false;
	M = glm::mat4(1.0);
}


Node* Link::getOtherNode(Node* n) {
	if (n == n0) { return n1; }
	else { return n0; }
}

glm::vec3 Link::position() {
	//calculate the midpoint position of link
	glm::vec3 origin = n0->pos;
	glm::vec3 ray = n1->pos - n0->pos;
	return origin + glm::vec3(ray.x / 2, ray.y / 2, ray.z / 2);
}

void Link::updateM(float worldScale) {
	glm::vec3 start = glm::vec3(0, 0, 1);	// initial direction of link
	glm::vec3 dest = n1->pos - n0->pos;		// desired direction of link
	glm::quat q = MathLib::RotationBtwnVectors(start, dest);
	glm::mat4 linkRotateM = glm::toMat4(q);
	glm::mat4 linkTranslateM = MathLib::translateM(position());

	// Get scale factor
	float linkLength = glm::length(dest) / (worldScale * 2.f);	// link lengthens equally from both ends
	float linkRadius = scale;

	glm::mat4 linkScaleM = glm::mat4(
		linkRadius, 0, 0, 0,
		0, linkRadius, 0, 0,
		0, 0, linkLength, 0,
		0, 0, 0, 1);

	M = linkTranslateM * linkRotateM * linkScaleM;
}

//======
void NNState::clear() {
	for (Node* n : nodes) { delete n; }
	for (Link* l : links) { delete l; }
	nodes.clear();
	links.clear();

	sNode = 0; sLink = 0; nearby = 0;
	prev_sNode = 0;	prev_sLink = 0; prev_nearby = 0;
}

int NNState::getNodeIndex(Node* n) {
	if (n == 0) { return -1; }
	for (int i = 0; i < nodes.size(); i++) {
		if (n == nodes[i]) { return i; }
	}
	return -1;	//if no node exists
}

int NNState::getLinkIndex(Link* l) {
	if (l == 0) { return -1; }
	for (int i = 0; i < links.size(); i++) {
		if (l == links[i]) { return i; }
	}
	return -1;	//if no link exists
}

void NNState::selectNode(Node* n) {
	if (n == 0) {
		return;		//No node can be selected
	}

	// Set all nObjects to default state
	if (sNode != 0) {
		sNode->state = NN::NState::DEFAULT;
	}
	if (sLink != 0) {
		sLink->state = NN::NState::DEFAULT;
	}

	// Update the selected node
	sLink = 0;
	sNode = n;
	sNode->state = NN::NState::SELECTED;	
}

void NNState::selectLink(Link* l) {
	// Set all nObjects to default state
	if (sNode != 0) {
		sNode->state = NN::NState::DEFAULT;
	}
	if (sLink != 0) {
		sLink->state = NN::NState::DEFAULT;
	}

	// Update the selected link
	sNode = 0;
	sLink = l;	
	sLink->state = NN::NState::SELECTED;
}

void NNState::deselect() {
	if (sLink != 0) {
		sLink->state = NN::NState::DEFAULT;
	}
	if (sNode != 0) {
		sNode->state = NN::NState::DEFAULT;
	}
	sLink = 0;
	sNode = 0;
}

Link* NNState::getLinkBtwnNodes(Node* n0, Node* n1) {
	if (n0 == 0 || n1 == 0 || n0 == n1) { return 0; }

	//find link from one of the nodes
	for (Link* l : n0->links) {
		if (l->flag_deleted == false &&
			(l->n0 == n1 || l->n1 == n1)) {
			return l;	//Only one link btwn two nodes
		}
	}
	return 0;		//No link found			
}

template <typename T>
void disableNNGroup(int groupNum, std::vector<T>& list) {
	for (int i = 0; i < list.size(); i++) {
		T nObj = list[i];
		if (nObj->groupNum == groupNum) {
			nObj->flag_disabled == true;
		}
	}
}

template <typename T>
void enableNNGroup(int groupNum, std::vector<T>& list) {
	for (int i = 0; i < list.size(); i++) {
		T nObj = list[i];
		if (nObj->groupNum == groupNum && 
			nObj->flag_deleted == false) {

			nObj->flag_disabled == false;
		}
	}
}

//======

template <typename T>
int findNearestIndex(glm::vec3 pointVS, std::vector<T>& list) {
	double lowest_dist = 9999999;
	int lowest_dist_list_index = -1;
	//Note: This uses brute-force search. Might need to switch
	//algorithm if the number of elements exceed over 10000 elements
	for (int i = 0; i < list.size(); i++) {
		T nObj = list[i];
		if (nObj->flag_disabled == false) {
			double dist = glm::distance(pointVS, nObj->position());

			if (dist < lowest_dist) {
				lowest_dist = dist;
				lowest_dist_list_index = i;
			}
		}
	}

	return lowest_dist_list_index;
}

int NN::findNearestNodeIndex(glm::vec3 pointVS, std::vector<Node*>& list) {
	return findNearestIndex<Node*>(pointVS, list);
}

Node* NN::findNearestNode(glm::vec3 pointVS, std::vector<Node*>& list) {
	int index = findNearestNodeIndex(pointVS, list);
	if (index >= 0) { return list[index]; }
	else { return 0; }
}

int NN::findNearestLinkIndex(glm::vec3 pointVS, std::vector<Link*>& list) {
	return findNearestIndex<Link*>(pointVS, list);
}

Link* NN::findNearestLink(glm::vec3 pointVS, std::vector<Link*>& list) {
	int index = findNearestLinkIndex(pointVS, list);
	if (index >= 0) { return list[index]; }
	else { return 0; }
}

bool NN::isNodeNearest(glm::vec3 pointVS, Node* nearestNode, Link* nearestLink) {
	double dist_node = glm::distance(pointVS, nearestNode->position());
	double dist_link = glm::distance(pointVS, nearestLink->position());

	if (dist_node < dist_link) { return true; }
	else { return false; }
}
NObject* NN::findNearestNObject(glm::vec3 pointVS, NNState* nn) {
	//Note: Only considering links of selected node to reduce clutter
	Node* n = findNearestNode(pointVS, nn->nodes);
	Link* l = 0;

	// Select a link that is attached to selected node
	// (Most likely, a person would want to select nodes instead of links)
	if (nn->sNode != 0) {
		l = findNearestLink(pointVS, nn->sNode->links);
	}

	if (n == 0) { return 0; }		//Assume that links cannot exist without nodes
	else if (l == 0) { return n; }
	else {
		if (isNodeNearest(pointVS, n, l)) { return n; }
		else { return l; }
	}
}

//------------------------------
// Key Callback Functions
//------------------------------


void NN::loadNObjects(Assign2::InputNode* out_nodes, NNState* nn) {
	// clear all existing nodes and links
	nn->clear();

	// add to the list of nodes
	for (int i = 0; i < out_nodes->vertex_position.size(); i++) {
		glm::vec3 vertex = glm::vec3(out_nodes->vertex_position[i]);
		int node_group_num = out_nodes->vertex_group[i];
		float node_scale = out_nodes->vertex_scale[i];
		nn->nodes.push_back(new Node(vertex, node_group_num, node_scale));
	}

	//add to the list of links
	for (int i = 0; i < out_nodes->vertex_link.size(); i++) {
		int node_index0 = out_nodes->vertex_link[i][0];
		int node_index1 = out_nodes->vertex_link[i][1];
		int link_group_num = out_nodes->vertex_link[i][2];
		nn->links.push_back(new Link(nn->nodes[node_index0], nn->nodes[node_index1], link_group_num));
	}
}

void NN::saveNObjects(Assign2::InputNode* in_nodes, NNState* nn) {
	int counter_node = 0;
	in_nodes->vertex_position.clear();
	in_nodes->vertex_link.clear();
	in_nodes->vertex_group.clear();
	in_nodes->vertex_scale.clear();

	//write content of nodes
	for (int i = 0; i < nn->nodes.size(); i++) {
		Node* n = nn->nodes[i];
		if (n->flag_deleted == false) {
			n->saveIndex = counter_node; //update the index of all nodes (for writing links)

			in_nodes->vertex_position.push_back(n->pos);
			in_nodes->vertex_group.push_back(n->groupNum);
			in_nodes->vertex_scale.push_back(n->scale);

			counter_node++;
		}
	}

	//write contents of links
	for (int i = 0; i < nn->links.size(); i++) {
		Link* l = nn->links[i];
		if (l->flag_deleted == false) {
			int* link_index = new int[3];
			link_index[0] = l->n0->saveIndex;
			link_index[1] = l->n1->saveIndex;
			link_index[2] = l->groupNum;
			in_nodes->vertex_link.push_back(link_index);
		}
	}
}

void NN::selectNearestNObject(glm::vec3 pointVS, NNState* nn) {
	//Prioritize node if node is not selected
	if (nn->sNode == 0) {
		//Selects nothing if no nearest node is found (returns 0)
		nn->selectNode(findNearestNode(pointVS, nn->nodes));
	}

	else {
		//Note: Only considering links of selected node to reduce clutter
		Node* n = findNearestNode(pointVS, nn->nodes);
		Link* l = findNearestLink(pointVS, nn->sNode->links);

		if (n != 0 && l != 0) {
			if (isNodeNearest(pointVS, n, l)) { nn->selectNode(n); }
			else { nn->selectLink(l); }
		}
		else if (n != 0 && l == 0) { nn->selectNode(n); }
		else if (n == 0 && l != 0) { nn->selectLink(l); } //should never happen
		else {} //select nothing
	}
}


void NN::addNode(glm::vec3 pointVS, float scale, int groupNum, NNState* nn) {
	Node* n = new Node(pointVS, groupNum, scale);
	nn->nodes.push_back(n);
	nn->selectNode(n);
}

void NN::addLink(Node*& nStart, Node*& nEnd, float worldScale, float linkScale, NNState* nn) {
	int groupNum = nStart->groupNum;				// Take the groupNum of the starting node
	Link* l = new Link(nStart, nEnd, groupNum, linkScale);
	l->updateM(worldScale);		//Update matrix
	nn->links.push_back(l);
	nn->selectNode(nEnd);	
}

void NN::moveNode(Node* n, glm::vec3 pos) {	
	n->pos = pos;
	n->updateM();
}

void NN::deleteNode(Node* n) {
	n->flag_deleted = true;
	n->flag_disabled = true;
}

void NN::deleteLink(Link* l) {
	l->flag_deleted = true;
	l->flag_disabled = true;
}

void NN::undeleteNode(Node* n) {
	n->flag_deleted = false;
	n->flag_disabled = false;
}

void NN::undeleteLink(Link* l) {
	l->flag_deleted = false;
	l->flag_disabled = false;
}

void NN::scaleNObject(float scale, NObject* n) {
	n->scale = scale;
	n->updateM();
}

void NN::changeGroupNObject(int group, NObject* n) {
	n->groupNum = group;
}

void NN::updateNearestNObjectState(glm::vec3 pointWS, NNState* nn) {
	//Reset previous NObject's state to default
	if (nn->nearby != 0 && nn->nearby->state != NN::NState::SELECTED) { 
		nn->nearby->state = NN::NState::DEFAULT; 
	}

	//highlight nearby node or link	
	nn->nearby = findNearestNObject(pointWS, nn);
	if (nn->nearby != 0 && nn->nearby->state != NN::NState::SELECTED) {
		//Set object state to "nearby"
		nn->nearby->state = NState::NEARBY;			
	}
}

glm::vec3 NN::getTypeColor(glm::vec3 color, int state) {
	if (state == NState::DEFAULT) { return color; }		//default
	else if (state == NState::NEARBY) {
		float s = 0.2;
		glm::vec3 nearestColor = (1 - s) * glm::vec3(1, 0, 1) + s * color;
		return nearestColor;
	}
	else if (state == NState::SELECTED) {
		//float s = 0.10;
		//glm::vec3 selectedColor = (1 - s) * glm::vec3(1, 1, 0) + s * color;
		//return selectedColor;
		return color;
	}

	return glm::vec3(0, 0, 0);	//No state (Shouldn't happen)
}

////===============Widget Slots=================
//void NodeNetwork::nodeSave(std::string filename) {
//	saveNObjects();
//
//	qDebug() << "Writing contents of node network to: " << filename.data() << " ...";
//	//somewhat different implementation for generality purposes?
//	fManager->saveNNFile(filename);
//
//	qDebug() << "Finished writing to file: " << filename.data();
//}
//
//void NodeNetwork::nodeSave_SWC(std::string filename) {
//	saveNObjects();
//
//	qDebug() << "Writing contents of node network to: " << filename.data() << " ...";
//	//somewhat different implementation for generality purposes?
//	fManager->saveNN_SWCFile(filename);
//
//	qDebug() << "Finished writing to file: " << filename.data();
//}
//
//void NodeNetwork::nodeLoad(std::string filename) {
//	if (fManager->loadNNFile(filename)) {
//		loadNObjects();
//	}
//
//	qDebug() << "node load";
//}
//
//void NodeNetwork::nodeLoad_SWC(std::string filename) {
//	if (fManager->loadNN_SWCFile(filename)) {
//		loadNObjects();
//	}
//
//	qDebug() << "node load";
//}
