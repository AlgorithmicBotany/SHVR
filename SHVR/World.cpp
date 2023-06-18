#include "World.h"

Point::Point() {
	//Initialize values
	modelM = glm::mat4(1.0);
	scaleM = glm::mat4(1.0);
	silhouetteScaleM = glm::mat4(1.0);
}


void Point::updateModelM(glm::mat4& m) { modelM = m; }
void Point::updatePos(glm::vec3& v) { modelM[3][0] = v.x; modelM[3][1] = v.y; modelM[3][2] = v.z;}
void Point::updateScaleM(float s) { scaleM = glm::scale(s, s, s); }
void Point::updateSilhouetteScaleM(float s) { silhouetteScaleM = glm::scale(s, s, s);}

const glm::vec3 Point::pos() { return glm::vec3(modelM[3][0], modelM[3][1], modelM[3][2]); }
float Point::scale() { return scaleM[0][0]; }
float Point::silhouetteScale() { return silhouetteScaleM[0][0]; }
const glm::mat4 Point::M() { return modelM * scaleM; }
const glm::mat4 Point::M_silhouette() { return modelM * silhouetteScaleM; }
//=====




