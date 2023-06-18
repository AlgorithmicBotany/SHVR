#include "rendermanager.h"


//====

void printVec3(QString name, glm::vec3 v) {
	qDebug() << name <<": " << v.x << " " << v.y << " " << v.z;
}

void printMat4(QString name, glm::mat4 matrix) {
	qDebug() << name << ": ";
	qDebug() << matrix[0][0] << " " << matrix[0][1] << " " << matrix[0][2] << " " << matrix[0][3];
	qDebug() << matrix[1][0] << " " << matrix[1][1] << " " << matrix[1][2] << " " << matrix[1][3];
	qDebug() << matrix[2][0] << " " << matrix[2][1] << " " << matrix[2][2] << " " << matrix[2][3];
	qDebug() << matrix[3][0] << " " << matrix[3][1] << " " << matrix[3][2] << " " << matrix[3][3];

}



//==== CAMERA ====//




//==== RENDER MANAGER ====//

RenderManager::RenderManager()
{
}


RenderManager::~RenderManager()
{
}


void RenderManager::init() {
	qDebug() << "This is renderManager";
}