#pragma once

#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_0_Core>
#include <QMouseEvent>
#include <QWheelEvent>

#include "TransferFunction.h"
#include "ShaderProgram.h"	//shader types
#include "Cameras.h"

// Objects used for rendering
#include "VertexObject.h"
#include "TextureObject.h"
#include "RenderObject.h"

// OpenGL math libraries
#include <glm/glm.hpp>


class Graph2DOpenGL : public QOpenGLWidget, protected QOpenGLFunctions_4_0_Core
{
	Q_OBJECT
public:
	Graph2DOpenGL(QWidget* parent = Q_NULLPTR);
	~Graph2DOpenGL();

	void loadGraph(Graph2D* _graph);
	void generateHistogramVTX(std::vector<glm::vec3>& out_vtx);
	void generateCPQuadVTX_CLR(std::vector<glm::vec3>& out_vtx, std::vector<glm::vec3>& out_clr);
	void updateHistogramRO();
	void updateCPQuadRO();
	void initRO();
	void initShaders();
	void initTextures();

	int selectedCPIdx;
	bool lockTFIndices;
protected:
	void initializeGL();
	void resizeGL(int w, int h);
	void paintGL();
	
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	void wheelEvent(QWheelEvent* event) override;

private:
	enum RENDER {
		CIRCLE,
		SCREEN_QUAD,
		CP_QUAD,
		HISTOGRAM,
		COUNT_RENDER
	};

	enum SHADER {
		ObjColorUniform,
		ObjColorVertex,
		ObjGridUV,
		COUNT_SHADER
	};

	enum TEXTURE {
		COUNT_TEXTURE
	};

	int getClosestCPIdx(const glm::vec2& pointNDC);
	bool getClosestCP(const glm::vec2& pointNDC, ControlPoint& out_CP);
	glm::vec2 getNDCfromMouseEvent(QMouseEvent* event);

	void renderShaderVertexUniform(RenderObject* rObject, const glm::mat4& MVP, const glm::vec3& color);

	void renderBackground();
	void renderHistogram();
	void renderTFQuads();
	void renderTFPoints();

	std::vector<TextureObject*> textureObjects;
	std::vector<RenderObject*> renderObjects;
	std::vector<GLExt::ShaderProgram*> shaders;

	Graph2D* graph;
	CameraScreen camScreen;
	Camera camera;
	float pointRadius;
	glm::vec2 aspectRatio;

signals:
	void updatedGraph();
};
