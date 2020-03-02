#include "plafon.h"


Plafon::Plafon(Vector3D pos_, Vector3D intensity_, float numberLights_, float width_, float height_) :
	pos(pos_), intensity(intensity_), numberLights(numberLights_), width(width_), height(height_)
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
	return numberLights;
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
	float xpoints = sqrt(width/height* numberLights +pow((width-height),2.0)/4*pow(height,2.0))-(width-height)/(height*height);
	float ypoints = numberLights / xpoints;

	float spacing = width / (xpoints-1);

	float x = this->pos.x - this->width * 0.5;
	float y = this->pos.y - this->height * 0.5;
	
	Vector3D start = Vector3D(x, y, this->pos.z);

	//xpoints = 5;
	//ypoints = 5;

	for (int i = 0; i < xpoints; i++)
	{
		for (int j = 0; j < ypoints; j++) 
		{
			PointLightSource light = PointLightSource(start, intensity);
			lightSourceList->push_back(light);
			start.x = start.x + spacing;
		}
		start.y = start.y + spacing;
	}

}

std::vector<PointLightSource>* Plafon::getList() const
{
	return lightSourceList;
}