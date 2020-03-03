#include "plafon.h"


Plafon::Plafon(Vector3D pos_, Vector3D intensity_, float numberLightsPerHeight_, float numberLightsPerWidth_, float width_, float height_) :
	pos(pos_), intensity(intensity_), numberLightsPerHeight(numberLightsPerHeight_),  numberLightsPerWidth(numberLightsPerWidth_), width(width_), height(height_)
{ 
	lightSourceList = new std::vector<PointLightSource>;
}

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
	return numberLightsPerHeight + numberLightsPerWidth;
}

float Plafon::getNumberLightsIntersectP(const Vector3D &p) const
{
	float numberLights = 0;
	Vector3D wi;
	for (int i = 0; i < lightSourceList->size(); i++) 
	{
		wi = lightSourceList->at(i).getPosition();
		if (dot(wi, normal)<0) 
		{
			numberLights++;
		}
	}
	//for every light source point, if light.pos intersects with point p, numberoflights ++
	return numberLights;
}

void Plafon::distributeLights() const
{

	float x = this->pos.x - this->width * 0.5;
	float y = this->pos.y - this->height * 0.5;
	
	float space_x = this->width / this->numberLightsPerWidth;
	float space_y = this->height / this->numberLightsPerHeight;

	/*
	float aux;
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = space_x - space_y;
	float r = random * diff;
	r = aux + r;
	*/

	Vector3D start = Vector3D(x, y, this->pos.z);

	for (int i = 0; i < this->numberLightsPerHeight; i++)
	{
		for (int j = 0; j < this->numberLightsPerWidth; j++) 
		{
			PointLightSource light = PointLightSource(start, intensity);
			lightSourceList->push_back(light);
			start.x = start.x + space_x;
		}
		start.y = start.y + space_y;
	}
}

std::vector<PointLightSource>* Plafon::getList() const
{
	return lightSourceList;
}