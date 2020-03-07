#ifndef PLAFON_H
#define PLAFON_H

#include "../core/vector3d.h"
#include "../core/utils.h"
#include "pointlightsource.h"

// To start, let this be the interface of a point light source
// Then, make this an abstract class from which we can derive:
//   - omnidirectional uniform point light sources
//   - area light sources
class Plafon
{
public:
	Plafon() = delete;
	Plafon(Vector3D pos_, Vector3D intensity_, float numberLightsPerHeight_, float numberLightsPerWidth_, float width_, float height__);

	// Getters
	Vector3D getPosition() const;
	float getNumberLights() const;
	float getNumberLightsWidth() const;
	float getNumberLightsHeight() const;
	void distributeLights() const;
	std::vector<PointLightSource>* getList() const;


private:
	Vector3D pos; //center mass position on the scene
	Vector3D intensity; // (unity: watts/sr)
	float numberLightsPerWidth;
	float numberLightsPerHeight;
	float totalLights;
	float width; //width of the square
	float height; //heigth of the square
	std::vector<PointLightSource>* lightSourceList;

	float generateRandomPosition( float size ) const;
};

#endif // PLAFON_H

