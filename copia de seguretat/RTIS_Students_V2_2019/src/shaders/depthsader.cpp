#include "depthshader.h"
#include "../core/utils.h"

DepthShader::DepthShader( Vector3D color_, double maxDist_, Vector3D bgColor_ ) :
	Shader( bgColor_ ), maxDist( maxDist_ ), color( color_ )
{}

Vector3D DepthShader::computeColor(const Ray& r,
    const std::vector<Shape*>& objList,
    const std::vector<PointLightSource>& lsList) const
{
    Intersection its;
    if (Utils::getClosestIntersection(r, objList, its))
    {
        double depth = (its.itsPoint - r.o).length();
        if (depth > maxDist)
        {
            return bgColor;
        }
        else
        {
            return color * (1.0 - depth/maxDist);
        }
    }
    return bgColor;
}
