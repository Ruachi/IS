#include "plafon.h"


Plafon::Plafon(Vector3D pos_, Vector3D intensity_, float numberLights_, float width_, float height_) :
	pos(pos_), intensity(intensity_), numberLights(numberLights_), width(width_), height(height_)
{ }

Vector3D Plafon::getPosition() const
{
	return pos;
}

Vector3D Plafon::getIntensity(const Vector3D &p) const
{
	double sqDistance = (p - pos).lengthSq();

	Vector3D result = (intensity*getNumberLightsIntersectP(p)) / sqDistance;
	return result;
}

float Plafon::getNumberLights() const
{
	return numberLights;
}

float Plafon::getNumberLightsIntersectP(const Vector3D &p) const
{
	float numberLights = 0;
	//for every light source point, if light.pos intersects with point p, numberoflights ++
	return numberLights;
}

