#include "EventFilter.h"


EventFilter::EventFilter(OpenGLWindow* openGLWindow, Haptics* _haptics, 
	ParameterManager* _pMan, WidgetManager* _wMan, MainWindowEvents* _mEvent) :
	glWindow(openGLWindow),
	world(glWindow->world),
	pMan(_pMan),
	wMan(_wMan),
	fileOpenWidget(_wMan->fileOpenWidget),
	mEvent(_mEvent){
	mouseButtonPressed = false;
	modifierPressed = false;
	pointNDC_prev = glm::vec3();
	modelM_prev = glm::mat4();
}

void EventFilter::startViewing(QEvent* event) {
	QMouseEvent* mEvent = (QMouseEvent*)event;
	pointNDC_prev = glWindow->getNDCfromSS(
		glm::vec2(mEvent->x(), mEvent->y()));
	modelM_prev = world->volume.M();
}

void EventFilter::panView(QEvent* event) {
	QMouseEvent* mEvent = (QMouseEvent*)event;
	if (mEvent->buttons() & Qt::LeftButton) {
		glm::vec3 pointNDC = glWindow->getNDCfromSS(
			glm::vec2(mEvent->x(), mEvent->y()));

		short camLocation = world->cams.getCameraLocation(pointNDC);
		if (camLocation >= 0) {
			CameraScreen& camScreen(world->cams[camLocation]);

			// Translate volume in world space
			glm::vec3 pointWS = camScreen.getWSfromNDC(pointNDC);
			glm::vec3 pointWS_prev = camScreen.getWSfromNDC(pointNDC_prev);
			glm::vec3 translateWS = (pointWS - pointWS_prev) * 2.f;
			world->volume.translate(-translateWS);
		}
		// Update previous point NDC
		pointNDC_prev = pointNDC;
	}
}
void EventFilter::zoomView(QEvent* event) {
	QWheelEvent* wEvent = (QWheelEvent*)event;
	QPoint numDegrees = wEvent->angleDelta() / 8;

	glm::vec3 pointNDC = glWindow->getNDCfromSS(
		glm::vec2(wEvent->x(), wEvent->y()));

	short camLocation = world->cams.getCameraLocation(pointNDC);
	if (camLocation >= 0) {
		CameraScreen& camScreen(world->cams[camLocation]);
		glm::vec3 dirWS = glm::normalize(
			camScreen.cam().getWSfromCS(
				glm::vec4(world->camDirCS, 0)));

		// Translate volume in world space
		float t = 0.0005 * (float)numDegrees.y();
		glm::vec3 translateWS = t * dirWS;
		world->volume.translate(translateWS);
	}
}

void EventFilter::rotateVolumeInView(QEvent* event) {
	QMouseEvent* mEvent = (QMouseEvent*)event;
	if (mEvent->buttons() & Qt::LeftButton) {
		glm::vec3 pointNDC = glWindow->getNDCfromSS(
			glm::vec2(mEvent->x(), mEvent->y()));

		short camLocation = world->cams.getCameraLocation(pointNDC);
		if (camLocation >= 0) {
			CameraScreen& camScreen(world->cams[camLocation]);

			// Rotational axis
			glm::vec3 pointVS = world->volume.getVSfromWS(camScreen.getWSfromNDC(pointNDC));
			glm::vec3 pointVS_prev = world->volume.getVSfromWS(camScreen.getWSfromNDC(pointNDC_prev));
			glm::vec3 mouseMoveDirVS = glm::normalize(pointVS - pointVS_prev);
			glm::vec3 camDirVS = world->volume.getVSfromWS(camScreen.cam().getWSfromCS(
				glm::vec4(world->camDirCS, 0)));
			glm::vec3 rotateAxis = glm::cross(mouseMoveDirVS, camDirVS);

			// Angle
			float moveMagnitude = glm::length(pointNDC - pointNDC_prev);
			const float TURN_ANGLE_DEGREES = 55;	//Any higher, then the rotation becomes unstable
			float angleDegrees = glm::min(TURN_ANGLE_DEGREES * moveMagnitude, TURN_ANGLE_DEGREES);
			
			// Update rotation of volume
			world->volume.updateM(modelM_prev);
			world->volume.rotate(rotateAxis, angleDegrees);
		}
	}
}

bool EventFilter::keyPressEvent(QKeyEvent* event) {
	bool shiftOn = event->modifiers().testFlag(Qt::ShiftModifier);
	bool altOn = event->modifiers().testFlag(Qt::AltModifier);
	bool controlOn = event->modifiers().testFlag(Qt::ControlModifier);

	//if (lockKeyPresses == true) {
	//	return;
	//}

	if (event->key() == Qt::Key_Space) {
		mEvent->toggleHaptics();
	}

	// Camera movement
	else if (event->key() == Qt::Key_W && altOn) {
		mEvent->rotateCamera(shiftOn, glm::vec3(-1, 0, 0));
	}
	else if (event->key() == Qt::Key_S && altOn) {
		mEvent->rotateCamera(shiftOn, glm::vec3(1, 0, 0));
	}
	else if (event->key() == Qt::Key_A && altOn) {
		mEvent->rotateCamera(shiftOn, glm::vec3(0, -1, 0));
	}
	else if (event->key() == Qt::Key_D && altOn) {
		mEvent->rotateCamera(shiftOn, glm::vec3(0, 1, 0));
	}
	else if (event->key() == Qt::Key_Q && altOn) {
		mEvent->rotateCamera(shiftOn, glm::vec3(0, 0, -1));
	}
	else if (event->key() == Qt::Key_E && altOn) {
		mEvent->rotateCamera(shiftOn, glm::vec3(0, 0, 1));
	}
	else if (event->key() == Qt::Key_W) {
		mEvent->moveCamera(shiftOn, glm::vec3(0, 0, 1));
	}
	else if (event->key() == Qt::Key_S) {
		mEvent->moveCamera(shiftOn, glm::vec3(0, 0, -1));
	}
	else if (event->key() == Qt::Key_A) {
		mEvent->moveCamera(shiftOn, glm::vec3(1, 0, 0));
	}
	else if (event->key() == Qt::Key_D) {
		mEvent->moveCamera(shiftOn, glm::vec3(-1, 0, 0));
	}
	else if (event->key() == Qt::Key_Q) {
		mEvent->moveCamera(shiftOn, glm::vec3(0, 1, 0));
	}
	else if (event->key() == Qt::Key_E) {
		mEvent->moveCamera(shiftOn, glm::vec3(0, -1, 0));
	}

	// Clipping plane
	else if (event->key() == Qt::Key_C) {
		float dist = 0.01;
		if (controlOn) { dist = -dist; }
		if (shiftOn) { dist /= 8.f; }

		VisualParams* visualParams = pMan->visualParams;
		float clipNear = visualParams->clipPlaneNear + dist;
		if (clipNear > 0.f) {
			visualParams->updateClipNear(clipNear);
			world->cams.updateClipNear(clipNear);
		}
		wMan->visualWidget->updateWidgets();

		QString str = "Clip Near: " + QString::number(clipNear);
		statusWriteTimedMessage(str);
	}
	else if (event->key() == Qt::Key_V) {
		float dist = -0.01;
		if (controlOn) { dist = -dist; }
		if (shiftOn) { dist /= 8.f; }

		VisualParams* visualParams = pMan->visualParams;
		float clipFar = visualParams->clipPlaneFar + dist;

		//Note: Epsilon prevents Z-fighting between clipping plane and the volume
		visualParams->updateClipFar(clipFar);
		world->cams.updateClipFar(clipFar);
		wMan->visualWidget->updateWidgets();

		QString str = "Clip Far: " + QString::number(clipFar);
		statusWriteTimedMessage(str);
	}

	// Cutting plane
	else if (event->key() == Qt::Key_Y) {
		float dist = 0.01;
		if (controlOn) { dist = -dist; }
		if (shiftOn) { dist /= 8.f; }

		VisualParams* visualParams = pMan->visualParams;
		visualParams->updateCutMin(visualParams->cutMin + glm::vec3(dist, 0, 0));
		mEvent->updateCutPlanes();
		wMan->visualWidget->updateWidgets();

		QString str = "Cut Plane X Min: " + QString::number(visualParams->cutMin.x);
		statusWriteTimedMessage(str);
	}
	else if (event->key() == Qt::Key_U) {
		float dist = 0.01;
		if (controlOn) { dist = -dist; }
		if (shiftOn) { dist /= 8.f; }

		VisualParams* visualParams = pMan->visualParams;
		visualParams->updateCutMin(visualParams->cutMin + glm::vec3(0, dist, 0));
		mEvent->updateCutPlanes();
		wMan->visualWidget->updateWidgets();

		QString str = "Cut Plane Y Min: " + QString::number(visualParams->cutMin.y);
		statusWriteTimedMessage(str);
	}

	else if (event->key() == Qt::Key_I) {
		float dist = 0.01;
		if (controlOn) { dist = -dist; }
		if (shiftOn) { dist /= 8.f; }

		VisualParams* visualParams = pMan->visualParams;
		visualParams->updateCutMin(visualParams->cutMin + glm::vec3(0, 0, dist));

		mEvent->updateCutPlanes();
		wMan->visualWidget->updateWidgets();

		QString str = "Cut Plane Z Min: " + QString::number(visualParams->cutMin.z);
		statusWriteTimedMessage(str);
	}

	else if (event->key() == Qt::Key_H) {
		float dist = -0.01;
		if (controlOn) { dist = -dist; }
		if (shiftOn) { dist /= 8.f; }

		VisualParams* visualParams = pMan->visualParams;
		visualParams->updateCutMax(visualParams->cutMax + glm::vec3(dist, 0, 0));
		mEvent->updateCutPlanes();
		wMan->visualWidget->updateWidgets();

		QString str = "Cut Plane X Max: " + QString::number(visualParams->cutMax.x);
		statusWriteTimedMessage(str);
	}
	else if (event->key() == Qt::Key_J) {
		float dist = -0.01;
		if (controlOn) { dist = -dist; }
		if (shiftOn) { dist /= 8.f; }

		VisualParams* visualParams = pMan->visualParams;
		visualParams->updateCutMax(visualParams->cutMax + glm::vec3(0, dist, 0));
		mEvent->updateCutPlanes();
		wMan->visualWidget->updateWidgets();

		QString str = "Cut Plane Y Max: " + QString::number(visualParams->cutMax.y);
		statusWriteTimedMessage(str);
	}

	else if (event->key() == Qt::Key_K) {
		float dist = -0.01;
		if (controlOn) { dist = -dist; }
		if (shiftOn) { dist /= 8.f; }

		VisualParams* visualParams = pMan->visualParams;
		visualParams->updateCutMax(visualParams->cutMax + glm::vec3(0, 0, dist));
		mEvent->updateCutPlanes();
		wMan->visualWidget->updateWidgets();

		QString str = "Cut Plane Z Max: " + QString::number(visualParams->cutMax.z);
		statusWriteTimedMessage(str);
	}

	else if (event->key() == Qt::Key_R) {
		float dist = 0.1;
		VisualParams* visualParams = pMan->visualParams;
		if (controlOn) { dist = -dist; }
		if (shiftOn) { dist /= 8.f; }
		if (controlOn && altOn) { dist = -visualParams->blendRate; }	//Reset to zero

		visualParams->blendRate = std::max(visualParams->blendRate + dist, 0.f);
		glWindow->blendRate = std::pow(visualParams->blendRate, 2);
		QString str = "Blend Rate: " + QString::number(glWindow->blendRate);
		statusWriteTimedMessage(str);
	}
	else if (event->key() == Qt::Key_T) {
		float dist = 0.1;
		VisualParams* visualParams = pMan->visualParams;
		if (controlOn) { dist = -dist; }
		if (shiftOn) { dist /= 8.f; }
		if (controlOn && altOn) { dist = -visualParams->blendSize; }	//Reset to zero

		visualParams->blendSize = std::max(visualParams->blendSize + dist, 0.f);
		//glWindow->blendSize = visualParams->blendSize;
		glWindow->blendSize = std::pow(visualParams->blendSize, 2);;

		QString str = "Blend Size: " + QString::number(glWindow->blendSize);
		statusWriteTimedMessage(str);
	}

	// Undo & redo
	else if (event->key() == Qt::Key_Z && controlOn && shiftOn) {
		if (!glWindow->queueUpdateTextureMask) {
			world->commandStack.redo();
			QString str = "Redo";
			statusWriteTimedMessage(str);
			glWindow->queueUpdateTextureVolume = true;
			glWindow->queueUpdateTextureMask = true;
		}
	}
	else if (event->key() == Qt::Key_Z && controlOn) {
		if (!glWindow->queueUpdateTextureMask) {
			world->commandStack.undo();
			QString str = "Undo";
			statusWriteTimedMessage(str);
			glWindow->queueUpdateTextureVolume = true;
			glWindow->queueUpdateTextureMask = true;
		}
	}

	// Modes of haptic and painting
	else if (event->key() == Qt::Key_Home) {
		ProbeParams* probeParams = pMan->probeParams;
		if (probeParams->hapticType == HAPTIC_SURFACE_SAMPLING) {
			probeParams->hapticType = HAPTIC_PROXIMITY_FOLLOWING;
			QString str = "Haptic Mode: PROXIMITY FOLLOWING";
			statusWriteTimedMessage(str);
		}
		else if (probeParams->hapticType == HAPTIC_PROXIMITY_FOLLOWING) {
			probeParams->hapticType = HAPTIC_OBJECT_FOLLOWING;
			QString str = "Haptic Mode: OBJECT FOLLOWING";
			statusWriteTimedMessage(str);
		}
		else if (probeParams->hapticType == HAPTIC_OBJECT_FOLLOWING) {
			probeParams->hapticType = HAPTIC_SURFACE_SAMPLING;
			QString str = "Haptic Mode: SURFACE SAMPLING";
			statusWriteTimedMessage(str);
		}
		wMan->probeWidget->updateWidgets();
	}
	else if (event->key() == Qt::Key_End) {
		ProbeParams* probeParams = pMan->probeParams;
		if (probeParams->paintType == FILL) {
			probeParams->paintType = FLOOD_FILL;
			QString str = "Paint Mode: FLOOD FILL";
			statusWriteTimedMessage(str);
		}
		else if (probeParams->paintType == FLOOD_FILL) {
			probeParams->paintType = SPECKLES;
			QString str = "Paint Mode: SPECKLE FILL";
			statusWriteTimedMessage(str);
		}
		else if (probeParams->paintType == SPECKLES) {
			probeParams->paintType = FILL;
			QString str = "Paint Mode: FILL";
			statusWriteTimedMessage(str);
		}

		wMan->probeWidget->updateWidgets();
	}

	// Probe size
	else if (event->key() == Qt::Key_PageUp) {
		ProbeParams* probeParams = pMan->probeParams;
		float radiusNew = pMan->probeParams->probeSize + 0.5;
		float silhouetteNew = pMan->probeParams->silhouetteSize + 0.5;
		if (radiusNew > 0) {
			probeParams->probeSize = radiusNew;
			world->probe.updateScaleM(probeParams->probeSize);

			probeParams->silhouetteSize = silhouetteNew;
			world->probe.updateSilhouetteScaleM(probeParams->silhouetteSize);
		}
		QString str = "Probe Radius: " + QString::number(pMan->probeParams->probeSize);
		statusWriteTimedMessage(str);
		wMan->probeWidget->updateWidgets();
	}
	else if (event->key() == Qt::Key_PageDown) {
		ProbeParams* probeParams = pMan->probeParams;
		float radiusNew = pMan->probeParams->probeSize - 0.5;
		float silhouetteNew = pMan->probeParams->silhouetteSize - 0.5;
		if (radiusNew > 0.f) {
			probeParams->probeSize = radiusNew;
			world->probe.updateScaleM(probeParams->probeSize);

			probeParams->silhouetteSize = silhouetteNew;
			world->probe.updateSilhouetteScaleM(probeParams->silhouetteSize);
		}
		QString str = "Probe Radius: " + QString::number(pMan->probeParams->probeSize);
		statusWriteTimedMessage(str);
		wMan->probeWidget->updateWidgets();
	}

	// Link Size
	else if (event->key() == Qt::Key_Minus) {
		if (world->nn.sNode != 0) {
			NN::Node* n = world->nn.sNode;
			float scale = n->scale - 0.1;
			if (scale > 0.f) {
				NN::scaleNObject(scale, n);
			}
		}
		else {
			int groupNum = pMan->probeParams->selectedGroup;
			float factor = 0.5;
			if (shiftOn) { factor /= 5.f; }
			if (controlOn) {
				// Update all groups
				std::vector<float> linkScaleGroups;
				for (int i = 0; i < pMan->cGroups->size(); i++) {
					float linkScale = std::max(pMan->cGroups->at(i).linkScale - factor, 0.f);
					linkScaleGroups.push_back(linkScale);
					pMan->cGroups->at(i).linkScale = linkScale;
				}

				world->nn.updateLinkGroupScale(world->worldScale, linkScaleGroups);
			}

			//Update all nodes
			else if (altOn) {
				for (int i = 0; i < world->nn.nodes.size(); i++) {
					NN::Node* n = world->nn.nodes[i];
					if (!n->flag_disabled && !n->flag_deleted && n->groupNum == groupNum) {
						float nodeScale = std::max(n->scale - 0.1f, 0.f);
						NN::scaleNObject(nodeScale, n);
					}
				}
			}

			else {
				// Update only current group of links
				float linkScale = std::max(pMan->cGroups->at(groupNum).linkScale - factor, 0.f);
				pMan->cGroups->at(groupNum).linkScale = linkScale;
				world->nn.updateLinkGroupScale(groupNum, world->worldScale, linkScale);
			}

		}
	}

	else if (event->key() == Qt::Key_Equal) {
		if (world->nn.sNode != 0) {
			NN::Node* n = world->nn.sNode;
			float scale = n->scale + 0.1;
			NN::scaleNObject(scale, n);
		}
		else {
			int groupNum = pMan->probeParams->selectedGroup;
			float factor = 0.5;
			if (shiftOn) { factor /= 5.f; }
			if (controlOn) {
				// Update all groups
				std::vector<float> linkScaleGroups;
				for (int i = 0; i < pMan->cGroups->size(); i++) {
					float linkScale = pMan->cGroups->at(i).linkScale + factor;
					linkScaleGroups.push_back(linkScale);
					pMan->cGroups->at(i).linkScale = linkScale;
				}

				world->nn.updateLinkGroupScale(world->worldScale, linkScaleGroups);
			}
			//Update all nodes in group
			else if (altOn) {
				for (int i = 0; i < world->nn.nodes.size(); i++) {
					NN::Node* n = world->nn.nodes[i];
					if (!n->flag_disabled && !n->flag_deleted && n->groupNum == groupNum) {
						float nodeScale = n->scale + 0.1;
						NN::scaleNObject(nodeScale, n);
					}
				}
			}

			else {
				// Update only current group
				float linkScale = pMan->cGroups->at(groupNum).linkScale + factor;
				pMan->cGroups->at(groupNum).linkScale = linkScale;
				world->nn.updateLinkGroupScale(groupNum, world->worldScale, linkScale);
			}

		}
	}

	// Node Operations
	else if (event->key() == Qt::Key_1 && controlOn) {
		char groupNew = 1;
		NN::NObject* n = world->nn.getSelectedNObject();
		if (n != 0 && n != world->nn.prev_sNode && n != world->nn.prev_sLink) {
			world->nn.updatePreviousNObjects();
			world->commandStack.commit(new Command_NNChangeGroup(n, n->groupNum, &world->nn));
		}
		NN::changeGroupNObject(groupNew, n);
	}
	else if (event->key() == Qt::Key_2 && controlOn) {
		char groupNew = 2;
		NN::NObject* n = world->nn.getSelectedNObject();
		if (n != 0 && n != world->nn.prev_sNode && n != world->nn.prev_sLink) {
			world->nn.updatePreviousNObjects();
			world->commandStack.commit(new Command_NNChangeGroup(n, n->groupNum, &world->nn));
		}
		NN::changeGroupNObject(groupNew, n);
	}
	else if (event->key() == Qt::Key_3 && controlOn) {
		char groupNew = 3;
		NN::NObject* n = world->nn.getSelectedNObject();
		if (n != 0 && n != world->nn.prev_sNode && n != world->nn.prev_sLink) {
			world->nn.updatePreviousNObjects();
			world->commandStack.commit(new Command_NNChangeGroup(n, n->groupNum, &world->nn));
		}
		NN::changeGroupNObject(groupNew, n);
	}
	else if (event->key() == Qt::Key_4 && controlOn) {
		char groupNew = 4;
		NN::NObject* n = world->nn.getSelectedNObject();
		if (n != 0 && n != world->nn.prev_sNode && n != world->nn.prev_sLink) {
			world->commandStack.commit(new Command_NNChangeGroup(n, n->groupNum, &world->nn));
			world->nn.updatePreviousNObjects();
			NN::changeGroupNObject(groupNew, n);
		}
	}
	else if (event->key() == Qt::Key_5 && controlOn) {
		char groupNew = 5;
		NN::NObject* n = world->nn.getSelectedNObject();
		if (n != 0 && n != world->nn.prev_sNode && n != world->nn.prev_sLink) {
			world->nn.updatePreviousNObjects();
			world->commandStack.commit(new Command_NNChangeGroup(n, n->groupNum, &world->nn));
		}
		NN::changeGroupNObject(groupNew, n);
	}
	else if (event->key() == Qt::Key_6 && controlOn) {
		char groupNew = 6;
		NN::NObject* n = world->nn.getSelectedNObject();
		if (n != 0 && n != world->nn.prev_sNode && n != world->nn.prev_sLink) {
			world->nn.updatePreviousNObjects();
			world->commandStack.commit(new Command_NNChangeGroup(n, n->groupNum, &world->nn));
		}
		NN::changeGroupNObject(groupNew, n);
	}
	else if (event->key() == Qt::Key_7 && controlOn) {
		char groupNew = 7;
		NN::NObject* n = world->nn.getSelectedNObject();
		if (n != 0 && n != world->nn.prev_sNode && n != world->nn.prev_sLink) {
			world->nn.updatePreviousNObjects();
			world->commandStack.commit(new Command_NNChangeGroup(n, n->groupNum, &world->nn));
		}
		NN::changeGroupNObject(groupNew, n);
	}
	else if (event->key() == Qt::Key_8 && controlOn) {
		char groupNew = 8;
		NN::NObject* n = world->nn.getSelectedNObject();
		if (n != 0 && n != world->nn.prev_sNode && n != world->nn.prev_sLink) {
			world->nn.updatePreviousNObjects();
			world->commandStack.commit(new Command_NNChangeGroup(n, n->groupNum, &world->nn));
		}
		NN::changeGroupNObject(groupNew, n);
	}
	else if (event->key() == Qt::Key_9 && controlOn) {
		char groupNew = 9;
		NN::NObject* n = world->nn.getSelectedNObject();
		if (n != 0 && n != world->nn.prev_sNode && n != world->nn.prev_sLink) {
			world->nn.updatePreviousNObjects();
			world->commandStack.commit(new Command_NNChangeGroup(n, n->groupNum, &world->nn));
		}
		NN::changeGroupNObject(groupNew, n);
	}
	else if (event->key() == Qt::Key_0 && controlOn) {
		char groupNew = 0;
		NN::NObject* n = world->nn.getSelectedNObject();
		if (n != 0 && n != world->nn.prev_sNode && n != world->nn.prev_sLink) {
			world->nn.updatePreviousNObjects();
			world->commandStack.commit(new Command_NNChangeGroup(n, n->groupNum, &world->nn));
		}
		NN::changeGroupNObject(groupNew, n);
	}

	else if (event->key() == Qt::Key_N) {
		// New node
		glm::vec3 posVS = glm::vec3(world->volume.invM() * glm::vec4(world->probe.pos(), 1));
		NN::Node* sNode = world->nn.sNode;
		if (sNode != 0) {
			// Connect new node with selected node
			int groupNum = pMan->probeParams->selectedGroup;
			float linkScale = pMan->cGroups->at(groupNum).linkScale;
			NN::addNode(posVS, world->probe.scale(), groupNum, &world->nn);
			NN::addLink(sNode, world->nn.nodes.back(), world->worldScale, linkScale, &world->nn);

			// Push to command stack
			world->nn.updatePreviousNObjects();
			world->commandStack.commit(new Command_NNAddConnectedNode(world->nn.nodes.back(), sNode, &world->nn));
		}
		else {
			// Create new node
			NN::addNode(posVS, world->probe.scale(), pMan->probeParams->selectedGroup, &world->nn);

			// Push to command stack
			world->nn.updatePreviousNObjects();
			world->commandStack.commit(new Command_NNAddNode(world->nn.nodes.back(), &world->nn));
		}
	}

	else if (event->key() == Qt::Key_M) {
		glm::vec3 posVS = glm::vec3(world->volume.invM() * glm::vec4(world->probe.pos(), 1));
		NN::Node* n = world->nn.sNode;

		if (n != 0) {
			// Push to command stack
			world->nn.updatePreviousNObjects();
			world->commandStack.commit(new Command_NNMoveNode(n, n->pos, &world->nn, world->worldScale));

			// Move selected node
			NN::moveNode(n, posVS);
			for (NN::Link* l : n->links) {
				l->updateM(world->worldScale);
			}
		}
	}

	else if (event->key() == Qt::Key_Comma) {
		glm::vec3 posVS = glm::vec3(world->volume.invM() * glm::vec4(world->probe.pos(), 1));
		NN::Node* sNode = world->nn.sNode;
		NN::Node* nNode = NN::findNearestNode(posVS, world->nn.nodes);
		if (sNode != 0 && nNode != 0 && sNode != nNode && !sNode->hasLink(nNode)) {
			// Connect selected node with nearby node
			int groupNum = pMan->probeParams->selectedGroup;
			float linkScale = pMan->cGroups->at(groupNum).linkScale;
			NN::addLink(sNode, nNode, world->worldScale, linkScale, &world->nn);

			// Push to command stack
			world->nn.updatePreviousNObjects();
			world->commandStack.commit(new Command_NNAddLink(world->nn.links.back(), &world->nn));
		}
		else if (sNode != 0 && nNode != 0) {
			// Update colour group of nearby node, and its link to the colour of the selected node
			std::vector<NN::NObject*> n;
			std::vector<char> groupNum;
			char groupNew = sNode->groupNum;

			n.push_back(sNode);
			n.push_back(sNode->getLink(nNode));
			n.push_back(nNode);

			groupNum.push_back(n[0]->groupNum);
			groupNum.push_back(n[1]->groupNum);
			groupNum.push_back(n[2]->groupNum);

			NN::changeGroupNObject(groupNew, n[1]);
			NN::changeGroupNObject(groupNew, n[2]);
			world->nn.selectNode(nNode);

			world->nn.updatePreviousNObjects();



			world->commandStack.commit(new Command_NNChangeGroup(n, groupNum, &world->nn));
		}
	}

	else if (event->key() == Qt::Key_Period) {
		// Select Node
		world->nn.updatePreviousNObjects();
		glm::vec3 posVS = glm::vec3(world->volume.invM() * glm::vec4(world->probe.pos(), 1));
		NN::selectNearestNObject(posVS, &world->nn);
	}

	else if (event->key() == Qt::Key_Slash) {
		// Deselect Node
		world->nn.updatePreviousNObjects();
		world->nn.deselect();
	}

	else if (event->key() == Qt::Key_Delete) {
		// Remove selected node or link
		NN::Node* sNode = world->nn.sNode;
		NN::Link* sLink = world->nn.sLink;
		if (sNode != 0) {
			NN::deleteNode(sNode);

			std::vector<NN::Link*> ls;
			for (NN::Link* l : sNode->links) {
				if (!l->flag_deleted) {
					ls.push_back(l);
					deleteLink(l);
				}
			}

			// Push to command stack
			world->nn.updatePreviousNObjects();
			world->commandStack.commit(new Command_NNDeleteNode(sNode, ls, &world->nn));
		}
		if (sLink != 0) {
			NN::deleteLink(sLink);

			// Push to command stack
			world->nn.updatePreviousNObjects();
			world->commandStack.commit(new Command_NNDeleteLink(sLink, &world->nn));
		}

		world->nn.deselect();
	}

	// ColorGroups
	else if (event->key() == Qt::Key_1) {
		pMan->probeParams->selectedGroup = 1;
		wMan->probeWidget->updateWidgets();
	}
	else if (event->key() == Qt::Key_2) {
		pMan->probeParams->selectedGroup = 2;
		wMan->probeWidget->updateWidgets();
	}
	else if (event->key() == Qt::Key_3) {
		pMan->probeParams->selectedGroup = 3;
		wMan->probeWidget->updateWidgets();
	}
	else if (event->key() == Qt::Key_4) {
		pMan->probeParams->selectedGroup = 4;
		wMan->probeWidget->updateWidgets();
	}
	else if (event->key() == Qt::Key_5) {
		pMan->probeParams->selectedGroup = 5;
		wMan->probeWidget->updateWidgets();
	}
	else if (event->key() == Qt::Key_6) {
		pMan->probeParams->selectedGroup = 6;
		wMan->probeWidget->updateWidgets();
	}
	else if (event->key() == Qt::Key_7) {
		pMan->probeParams->selectedGroup = 7;
		wMan->probeWidget->updateWidgets();
	}
	else if (event->key() == Qt::Key_8) {
		pMan->probeParams->selectedGroup = 8;
		wMan->probeWidget->updateWidgets();
	}
	else if (event->key() == Qt::Key_9) {
		pMan->probeParams->selectedGroup = 9;
		wMan->probeWidget->updateWidgets();
	}
	else if (event->key() == Qt::Key_0) {
		pMan->probeParams->selectedGroup = 0;
		wMan->probeWidget->updateWidgets();
	}

	// Additional Features
	else if (event->key() == Qt::Key_L) {
		//mask vs tf distribution
		float dist = -0.01;
		if (controlOn) { dist = -dist; }
		if (shiftOn) { dist /= 8.f; }

		float value = glWindow->maskIntensity + dist;
		glWindow->maskIntensity = glm::clamp(value, 0.f, 1.f);
		QString str = "Mask_T: " + QString::number(glWindow->maskIntensity);
		statusWriteTimedMessage(str);
	}

	else if (event->key() == Qt::Key_P && controlOn) {
		mEvent->takeSSOpenGL(2);
	}
	else if (event->key() == Qt::Key_P) {
		mEvent->takeSSOpenGL(1);
	}

	else if (event->key() == Qt::Key_Semicolon) {
		//mEvent->setFromFarClippingPlane();
		mEvent->resetClippingPlanes();
	}
	else if (event->key() == Qt::Key_Apostrophe) {
		mEvent->setFromNearClippingPlane();
	}

	else if (event->key() == Qt::Key_BracketLeft) {
		float value = pMan->probeParams->paintScale - 0.2f;
		if (value > 0.1f) {
			pMan->probeParams->paintScale = value;


			QString str = "Paint Scale: " + QString::number(value);
			statusWriteTimedMessage(str);
		}
	}
	else if (event->key() == Qt::Key_BracketRight) {
		float value = pMan->probeParams->paintScale + 0.2f;
		pMan->probeParams->paintScale = value;

		QString str = "Paint Scale: " + QString::number(value);
		statusWriteTimedMessage(str);
	}


	else if (event->key() == Qt::Key_F1) {
		world->cams.isArrowEnabled = !world->cams.isArrowEnabled;
		pMan->visualParams->arrowEnabled = world->cams.isArrowEnabled;
		wMan->visualWidget->updateWidgets();
	}
	else if (event->key() == Qt::Key_F2) {
		world->cams.setVolumeEnabled(!world->cams.getVolumeEnabled());
		pMan->visualParams->volumeEnabled = world->cams.getVolumeEnabled();
		wMan->visualWidget->updateWidgets();
	}
	else if (event->key() == Qt::Key_F3) {
		world->cams.setObjectEnabled(!world->cams.getObjectEnabled());
		pMan->visualParams->objectEnabled = world->cams.getObjectEnabled();
		wMan->visualWidget->updateWidgets();
	}

	else if (event->key() == Qt::Key_F4) {

		if (!controlOn && !shiftOn) {
			char visualMode = pMan->isoParams->paintVisualMode;
			if (visualMode == 0) {
				pMan->isoParams->paintVisualMode = 1;
				statusWriteTimedMessage("ISO: Painting region");
			}
			else if (visualMode == 1) {
				pMan->isoParams->paintVisualMode = 2;
				statusWriteTimedMessage("ISO: Entire region");
			}
			else {
				pMan->isoParams->paintVisualMode = 0;
				statusWriteTimedMessage("ISO: Off");
			}
		}

		else if (controlOn) {
			bool& b = world->volume.vState()->keepExistingOffset;

			if (!b) { b = true; }
			else { b = false; }
			QString str = "Keep Offset: " + QString::number(b);
			statusWriteTimedMessage(str);
		}

		else if (shiftOn) {
			auto& offsetType = pMan->probeParams->offsetType;
			if (offsetType == 0) {
				offsetType = 1;
				statusWriteTimedMessage("OFFSET: Uniform");


			}
			else if (offsetType == 1) {
				offsetType = 0;
				statusWriteTimedMessage("OFFSET: Dist2");
			}
			else {
				//Shouldn't happen
				offsetType = 0;
			}
		}
	}

	else if (event->key() == Qt::Key_F5 && altOn) {

		//openConfirmationMessage("Are you sure you want to close?")
		float paintMaxOffset = pMan->isoParams->offsetInValue;

		int w = world->volume.vData()->width();
		int h = world->volume.vData()->height();
		int d = world->volume.vData()->depth();
		int selectedGroup = pMan->probeParams->selectedGroup;

		for (int k = 0; k < d; k++) {
			for (int j = 0; j < h; j++) {
				for (int i = 0; i < w; i++) {
					glm::ivec3 idx(i, j, k);
					if (selectedGroup == world->volume.vMask()->valueAtCell(idx)) {
						world->volume.vState()->updateOffsetValue(
							paintMaxOffset, paintMaxOffset, idx);
					}
				}
			}
		}

		//Update in render
		world->volume.maskUpdateInfo->queueUpdate(
			glm::ivec3(0, 0, 0),
			glm::ivec3(w, h, d),
			world->volume.vMask(),
			world->volume.vData());
		world->volume.vState()->clearMinMax();

		//Clear buffer
		world->volume.vState()->affectedOffset.clear();
		world->volume.vState()->affectedOffsetDiff.clear();
	}


	else if (event->key() == Qt::Key_F5) {
		float paintIsoMin = pMan->isoParams->paintIsoMin;
		float hapticsIsoMin = pMan->isoParams->hapticsIsoMin;
		float isoUpdateRate = pMan->isoParams->IsoUpdateRate;

		if (controlOn) {
			wMan->tfWidget->updatePaintIsoMinWidget(paintIsoMin + isoUpdateRate);
			wMan->tfWidget->updateHapticIsoMinWidget(hapticsIsoMin + isoUpdateRate);
		}
		else {
			wMan->tfWidget->updatePaintIsoMinWidget(paintIsoMin - isoUpdateRate);
			wMan->tfWidget->updateHapticIsoMinWidget(hapticsIsoMin - isoUpdateRate);
		}
		QString str = "Paint Iso Min: " + QString::number(paintIsoMin) + "; Haptic Iso Min: " + QString::number(hapticsIsoMin);
		statusWriteTimedMessage(str);
	}


	else if (event->key() == Qt::Key_F6) {
		//Save Camera location
	}

	else if (event->key() == Qt::Key_F7) {
		ProbeParams* probeParams = pMan->probeParams;
		float factor = 0.1;
		if (shiftOn) { factor /= 8.f; }

		if (altOn && controlOn) {
			probeParams->workspaceScale = 1;	//Reset
		}
		else if (controlOn) {
			probeParams->workspaceScale = std::max(probeParams->workspaceScale - factor, 0.f);
		}
		else {
			probeParams->workspaceScale += factor;
		}

		float updatedWorkspaceScale = std::pow(probeParams->workspaceScale, 2);
		haptics->updateWorkspaceScale(updatedWorkspaceScale);
		QString str = "Workspace Scale: " + QString::number(updatedWorkspaceScale);
		statusWriteTimedMessage(str);

	}
	else if (event->key() == Qt::Key_F8) {
		glWindow->queueUpdateTextureVolumeAO = true;
	}
	else if (event->key() == Qt::Key_F9) {
		mEvent->updateCameraScreenType(Cameras::SCR_OVERVIEW);
	}
	else if (event->key() == Qt::Key_F10) {
		mEvent->updateCameraScreenType(Cameras::SCR_FULL);
	}
	else if (event->key() == Qt::Key_F11) {
		mEvent->updateCameraScreenType(Cameras::SCR_STEREO);
	}

	qDebug() << ("You Pressed Key " + event->text());
	return true;
}

//Note:Events from QWindow needs to be sent back to MainWindow for consistent keyboard inputs
bool EventFilter::eventFilter(QObject* obj, QEvent* event)
{
	// Mouse events
	if (event->type() == QEvent::MouseButtonPress) {
		startViewing(event);
		mouseButtonPressed = true;
		modifierPressed = QApplication::keyboardModifiers().testFlag(Qt::AltModifier);
		return true;
	}

	else if (event->type() == QEvent::MouseButtonRelease) {
		mouseButtonPressed = false;
		return true;
	}

	else if (event->type() == QEvent::MouseMove && mouseButtonPressed == true) {
		if (modifierPressed) {
			rotateVolumeInView(event);
		}
		else {
			panView(event);
		}
		return true;
	}

	else if (event->type() == QEvent::Wheel) {
		zoomView(event);
		return true;
	}

	// Keyboard events
	else if (event->type() == QEvent::KeyPress) {
		keyPressEvent(static_cast<QKeyEvent*>(event));
		return true;
	}

	else if (event->type() == QEvent::DragEnter) {
		static_cast<QDragEnterEvent*>(event)->acceptProposedAction();
	}

	else if (event->type() == QEvent::Drop) {
		//Note: Drop events to glWindow is automatically accepted
		qDebug() << "DROP FROM EVENTFILTER";
		QDropEvent* de = static_cast<QDropEvent*>(event);
		auto mime = de->mimeData();
		if (mime->hasUrls()) {
			fileOpenWidget->loadDraggedFiles(mime->urls());
		}
		event->accept();
		return true;
	}
	else if (event->type() == QEvent::Close) {
		obj->removeEventFilter(this);
		glWindow->hide();
		qDebug() << "CLOSE!";
		return true;
	}

	return QObject::eventFilter(obj, event);
}