#include "Graph2DOpenGL.h"

Graph2DOpenGL::Graph2DOpenGL(QWidget* parent)
	: QOpenGLWidget(parent),
	camScreen(camera) {

	// Create shader objects
	shaders.resize(COUNT_SHADER);
	for (int i = 0; i < shaders.size(); i++) {
		shaders[i] = new GLExt::ShaderProgram();
	}

	// Create texture objects	
	textureObjects.resize(COUNT_TEXTURE);
	for (int i = 0; i < textureObjects.size(); i++) {
		textureObjects[i] = new TextureObject();
	}

	// Create render objects
	renderObjects.resize(COUNT_RENDER);
	for (int i = 0; i < renderObjects.size(); i++) {
		renderObjects[i] = new RenderObject();
	}

	pointRadius = 0.1f;

	camScreen.isOrtho = true;
	camScreen.updatePlaneNear(-1);
	camScreen.updatePlaneFar(1);
	camScreen.updateClipNear(-1);
	camScreen.updateClipFar(1);

	selectedCPIdx = -1;
	lockTFIndices = 0;
	graph = 0;
}

Graph2DOpenGL::~Graph2DOpenGL()
{
	for (auto obj : textureObjects) { delete obj; }
	for (auto obj : renderObjects) { delete obj; }
	for (auto obj : shaders) { delete obj; }
}

void Graph2DOpenGL::loadGraph(Graph2D* _graph) {
	graph = _graph;
}

void Graph2DOpenGL::generateHistogramVTX(std::vector<glm::vec3>& out_vtx) {
	glm::vec3 a(0, 0, 0);
	glm::vec3 b(0, 0, 0);
	glm::vec3 c(0, 0, 0);
	glm::vec3 d(0, 0, 0);
	float xStep = (graph->xGraphMax - graph->xGraphMin) / graph->buckets.size();
	float yRange = graph->yGraphMax - graph->yGraphMin;
	float x = graph->xGraphMin;
	for (int i = 0; i < graph->buckets.size(); ++i)
	{
		auto n = graph->buckets[i];

		a.y = graph->yGraphMin;
		b.y = a.y;
		c.y = std::log((float)n) / std::log((float)graph->histMax) * yRange + a.y;
		d.y = c.y;

		a.x = x;
		d.x = a.x;
		b.x = x + xStep;
		c.x = b.x;

		out_vtx.push_back(a);
		out_vtx.push_back(b);
		out_vtx.push_back(d);

		out_vtx.push_back(b);
		out_vtx.push_back(c);
		out_vtx.push_back(d);

		x += xStep;
	}
}

void Graph2DOpenGL::generateCPQuadVTX_CLR(std::vector<glm::vec3>& out_vtx, std::vector<glm::vec3>& out_clr) {
	for (auto cp : graph->controlPoints)
	{
		glm::vec2 graphPos = graph->getGraphPosFromValue(cp.values);
		out_vtx.push_back(glm::vec3(graphPos, 0));
		out_vtx.push_back(glm::vec3(graphPos.x, graph->yGraphMin, 0));
		out_clr.push_back(cp.color);
		out_clr.push_back(cp.color);
	}
}

void Graph2DOpenGL::updateHistogramRO() {
	std::vector<glm::vec3> histogramVTX;
	generateHistogramVTX(histogramVTX);

	VertexObject vo_histogram;
	vo_histogram.vtx.swap(histogramVTX);

	RenderObject* rObject = renderObjects[HISTOGRAM];
	vo_histogram.initRO(*rObject);
	//vo_histogram.updateRO(*rObject);
}

void Graph2DOpenGL::updateCPQuadRO() {
	std::vector<glm::vec3> cpQuadVTX;
	std::vector<glm::vec3> cpQuadCLR;
	generateCPQuadVTX_CLR(cpQuadVTX, cpQuadCLR);

	VertexObject vo_cpQuad;
	vo_cpQuad.vtx.swap(cpQuadVTX);
	vo_cpQuad.clr.swap(cpQuadCLR);
	RenderObject* rObject = renderObjects[CP_QUAD];
	vo_cpQuad.initRO(*rObject);
	//vo_cpQuad.updateRO(*rObject);
}

void Graph2DOpenGL::initRO() {
	for (int i = 0; i < renderObjects.size(); i++) {
		renderObjects[i]->init();
	}

	VOQuad vo_quad;
	vo_quad.generate();
	vo_quad.initRO(*renderObjects[SCREEN_QUAD]);

	VOCircle vo_circle;
	vo_circle.generate();
	vo_circle.initRO(*renderObjects[CIRCLE]);

	updateHistogramRO();
}


void Graph2DOpenGL::initShaders() {
	for (int i = 0; i < shaders.size(); i++) {
		shaders[i]->createProgram();
	}

	shaders[ObjColorUniform]->addShader("shaders/colorUniform.vert", GL_VERTEX_SHADER);
	shaders[ObjColorUniform]->addShader("shaders/colorVertex.frag", GL_FRAGMENT_SHADER);

	shaders[ObjColorVertex]->addShader("shaders/colorVertex.vert", GL_VERTEX_SHADER);
	shaders[ObjColorVertex]->addShader("shaders/colorVertex.frag", GL_FRAGMENT_SHADER);

	shaders[ObjGridUV]->addShader("shaders/vertexBasic.vert", GL_VERTEX_SHADER);
	shaders[ObjGridUV]->addShader("shaders/vertexGrid.frag", GL_FRAGMENT_SHADER);
}

void Graph2DOpenGL::initTextures() {
	for (int i = 0; i < textureObjects.size(); i++) {
		textureObjects[i]->init();
	}
}

void Graph2DOpenGL::initializeGL() {
	initializeOpenGLFunctions();
	initRO();
	initShaders();
	//initTextures();	//***None at the moment
}
void Graph2DOpenGL::resizeGL(int w, int h) {
	camScreen.updateScreenRatio(w, h);
	camScreen.updateOrtho(1.f);

	aspectRatio = glm::vec2(camScreen.screenRatio(), 1.f);
}

void Graph2DOpenGL::renderShaderVertexUniform(RenderObject* rObject, const glm::mat4& MVP, const glm::vec3& color) {
	GLExt::ShaderProgram* shader = shaders[ObjColorUniform];
	glBindVertexArray(rObject->vao);
	shader->useProgram();
	shader->setMat4("MVP", MVP);
	shader->setVec3("color", color);
	shader->setFloat("alpha", 1);

	glDrawElements(
		GL_TRIANGLES,      	// mode
		rObject->size,	// count
		GL_UNSIGNED_SHORT,  // state
		(void*)0           	// element array buffer offset
		);
}

void Graph2DOpenGL::renderBackground() {
	glm::mat4 MVP = camScreen.P() * glm::scale(aspectRatio.x, aspectRatio.y, 0.f);

	GLExt::ShaderProgram* shader = shaders[ObjGridUV];
	RenderObject* rObject = renderObjects[SCREEN_QUAD];
	glBindVertexArray(rObject->vao);
	shader->useProgram();
	shader->setMat4("MVP", MVP);

	glDrawElements(
		GL_TRIANGLES,      	// mode
		rObject->size,	// count
		GL_UNSIGNED_SHORT,  // state
		(void*)0           	// element array buffer offset
		);
}

void Graph2DOpenGL::renderHistogram() {
	glm::mat4 MVP = glm::mat4(1.0);
	glm::vec3 color = glm::vec3(0.5, 0.5, 0.5);
	RenderObject* rObject = renderObjects[HISTOGRAM];
	GLExt::ShaderProgram* shader = shaders[ObjColorUniform];

	glBindVertexArray(rObject->vao);
	shader->useProgram();
	shader->setMat4("MVP", MVP);
	shader->setVec3("color", color);
	shader->setFloat("alpha", 0.9);

	glDrawArrays(
		GL_TRIANGLES,
		0,
		rObject->size);
}

void Graph2DOpenGL::renderTFQuads() {
	updateCPQuadRO();

	glm::mat4 MVP = glm::mat4(1.0);
	RenderObject* rObject = renderObjects[CP_QUAD];
	GLExt::ShaderProgram* shader = shaders[ObjColorVertex];

	glBindVertexArray(rObject->vao);
	shader->useProgram();
	shader->setMat4("MVP", MVP);
	shader->setFloat("alpha", 0.6);

	glDrawArrays(
		GL_TRIANGLE_STRIP,
		0,
		graph->controlPoints.size() * 2);
}

void Graph2DOpenGL::renderTFPoints() {
	glm::mat4 MVP;
	glm::mat4 scaleM_outline = glm::scale(pointRadius + 0.02f, pointRadius + 0.02f, 0.f);
	glm::mat4 scaleM_tfPoint = glm::scale(pointRadius, pointRadius, 0.f);
	for (int i = 0; i < graph->controlPoints.size(); i++) {
		glm::vec2 graphPos = graph->getGraphPosFromValue(graph->controlPoints[i].values) * aspectRatio;
		glm::mat4 M = glm::translate(graphPos.x, graphPos.y, 0.f);
		MVP = camScreen.P() * M * scaleM_outline;

		glm::vec3 outlineColor = glm::vec3(0.7, 0.7, 0.7);
		if (i == selectedCPIdx) { outlineColor = glm::vec3(0, 1, 0); }
		renderShaderVertexUniform(renderObjects[CIRCLE], MVP, outlineColor);

		MVP = camScreen.P() * M * scaleM_tfPoint;
		renderShaderVertexUniform(renderObjects[CIRCLE], MVP, graph->controlPoints[i].color);
	}
}

void Graph2DOpenGL::paintGL() {
	glBindFramebuffer(GL_FRAMEBUFFER, context()->currentContext()->defaultFramebufferObject());
	renderBackground();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	renderHistogram();
	renderTFQuads();
	glDisable(GL_BLEND);

	renderTFPoints();
}

//====
int Graph2DOpenGL::getClosestCPIdx(const glm::vec2& pointNDC) {
	float shortestDist = 9999999.f;
	int nearestIdx = -1;
	glm::vec2 aspectRatio(camScreen.screenRatio(), 1.f);
	for (int i = 0; i < graph->controlPoints.size(); i++) {
		glm::vec2 cpNDC = graph->getGraphPosFromValue(graph->controlPoints[i].values);
		float dist = glm::length((pointNDC - cpNDC) * aspectRatio);
		if (dist < shortestDist && dist < pointRadius) {
			shortestDist = dist;
			nearestIdx = i;
		}
	}
	return nearestIdx;
}
bool Graph2DOpenGL::getClosestCP(const glm::vec2& pointNDC, ControlPoint& out_CP) {
	int nearestIdx = getClosestCPIdx(pointNDC);
	if (nearestIdx >= 0) {
		out_CP = graph->controlPoints[nearestIdx];
		return true;
	}
	return false;
}

glm::vec2 Graph2DOpenGL::getNDCfromMouseEvent(QMouseEvent* event) {
	return glm::vec2(
		2 * ((float)event->x() / (float)width()) - 1,
		1 - 2 * ((float)event->y() / (float)height()));
}

void Graph2DOpenGL::mousePressEvent(QMouseEvent* event) {
	bool shiftOn = event->modifiers().testFlag(Qt::ShiftModifier);
	bool altOn = event->modifiers().testFlag(Qt::AltModifier);
	bool controlOn = event->modifiers().testFlag(Qt::ControlModifier);

	glm::vec2 mouseNDC = getNDCfromMouseEvent(event);
	int idx = getClosestCPIdx(mouseNDC);
	selectedCPIdx = idx;

	// Insert CP
	if (controlOn) {
		glm::vec2 mouseValues = graph->getValueFromGraphPos(mouseNDC);
		ControlPoint cp(1, 1, 1, mouseValues.x, mouseValues.y);
		graph->boundCPToWindow(cp);
		selectedCPIdx = graph->insertCP(cp);
		qDebug() << "selectedCPIdx: " << selectedCPIdx;

		emit updatedGraph();
		update();
	}

	// Remove CP
	else if (shiftOn && idx >= 0) {
		graph->removeCP(idx);
		selectedCPIdx = -1;
		emit updatedGraph();
		update();
	}
}


void Graph2DOpenGL::mouseMoveEvent(QMouseEvent* event) {
	if (selectedCPIdx >= 0) {
		// Move CP point to mouse position
		ControlPoint& selectedCP = graph->controlPoints[selectedCPIdx];
		glm::vec2 mouseNDC = getNDCfromMouseEvent(event);
		selectedCP.values = graph->getValueFromGraphPos(mouseNDC);
		if (lockTFIndices) {
			graph->boundCPToIdx(selectedCPIdx);
		}
		else {
			graph->boundCPToWindow(selectedCP);
			selectedCPIdx = graph->updateCPIdx(selectedCP);
		}

		update();
	}
}
void Graph2DOpenGL::mouseReleaseEvent(QMouseEvent* event) {
	if (selectedCPIdx >= 0) {
		emit updatedGraph();
	}
}

void Graph2DOpenGL::wheelEvent(QWheelEvent* event) {
	bool shiftOn = event->modifiers().testFlag(Qt::ShiftModifier);
	bool controlOn = event->modifiers().testFlag(Qt::ControlModifier);

	QWheelEvent* wEvent = (QWheelEvent*)event;
	QPoint numDegrees = wEvent->angleDelta() / 8;
	float scrollRatio = numDegrees.y() / 1440.f;
	float xValueRange = graph->xValueMax - graph->xValueMin;

	// Slow down scroll
	if (shiftOn) { scrollRatio /= 8.f; }		
	//Lock max or min value
	int savedIdx = -1;
	float savedValueX = 0;
	if (graph->controlPoints.size() > 0) {
		if (scrollRatio < 0) { 
			int idx = graph->controlPoints.size() - 1;		//last idx
			float value = graph->controlPoints[idx].values.x;
			if (value >= graph->xValueMax) {
				savedIdx = idx;
				savedValueX = value;
			}
		}
		else { 
			int idx = 0; //First CP
			float value = graph->controlPoints[idx].values.x;
			if (value <= graph->xValueMin) {
				savedIdx = idx;
				savedValueX = value;
			}
		}	
		savedValueX = graph->controlPoints[savedIdx].values.x;
	}
	
	graph->shiftPointsX(scrollRatio * xValueRange);
	if (savedIdx >= 0) { graph->controlPoints[savedIdx].values.x = savedValueX; }
	emit updatedGraph();
	update();
}