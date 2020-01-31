#include "depthshader.h"
#include "../core/utils.h"

DepthShader::DepthShader(Vector3D color_, double maxDist_, Vector3D bgColor_) :
    Shader(bgColor_), color(color_), maxDist(maxDist_)
{}

Vector3D DepthShader::computeColor(const Ray &r, const std::vector<Shape *> &objList, const std::vector<PointLightSource> &lsList) const
{
    Intersection its;
	

    if (Utils::getClosestIntersection(r, objList, its))
    {
		Vector3D finalPoint = its.itsPoint;
		Vector3D startPoint = r.o;
		//double depth = sqrt((startPoint.x - finalPoint.x)*(startPoint.x - finalPoint.x) + (startPoint.y - finalPoint.y)*(startPoint.y - finalPoint.y));
		double depth = (finalPoint - startPoint).length();

		if (depth >= maxDist) 
		{
			return bgColor;
		}
		else 
		{
			return color*(1.0-depth/maxDist);
		}
    } 
	else
    {
        return bgColor;
    }
}
