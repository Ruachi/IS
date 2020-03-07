#include "plafon.h"


Plafon::Plafon(Vector3D pos_, Vector3D intensity_, float numberLightsPerHeight_, float numberLightsPerWidth_, float width_, float height_) :
	pos(pos_), intensity(intensity_), numberLightsPerHeight(numberLightsPerHeight_),  numberLightsPerWidth(numberLightsPerWidth_), width(width_), height(height_)
{ 
	lightSourceList = new std::vector<PointLightSource>;
	this->totalLights = this->numberLightsPerHeight * this->numberLightsPerWidth;
}

Vector3D Plafon::getPosition() const
{
	return pos;
}

float Plafon::getNumberLights() const
{
	return numberLightsPerHeight + numberLightsPerWidth;
}

float Plafon::getNumberLightsHeight() const
{
	return this->numberLightsPerHeight;
}

float Plafon::getNumberLightsWidth() const
{
	return this->numberLightsPerWidth;
}

void Plafon::distributeLights() const
{

	float x = this->pos.x - this->width * 0.5;
	float y = this->pos.z - this->height * 0.5;
	
	float xSquareSize = this->width / this->numberLightsPerWidth;
	float ySquareSize = this->height / this->numberLightsPerHeight;

	Vector3D start = Vector3D(x, this->pos.y, y);

	for (int i = 0; i < this->numberLightsPerHeight; i++)
	{
		for (float j = 0; j < this->numberLightsPerWidth; j++) 
		{
			float xOffset = generateRandomPosition(xSquareSize);
			float yOffset = generateRandomPosition(ySquareSize);

			xOffset = start.x + xOffset;
			yOffset = start.z + yOffset;

			PointLightSource light = PointLightSource(start, intensity);
			lightSourceList->push_back(light);
			start.x += xSquareSize;
		}
		start.z += ySquareSize;
	}
}

std::vector<PointLightSource>* Plafon::getList() const
{
	return lightSourceList;
}

float Plafon::generateRandomPosition(float size) const
{
	float random = ((float)rand()) / RAND_MAX;
	return random * size;
}