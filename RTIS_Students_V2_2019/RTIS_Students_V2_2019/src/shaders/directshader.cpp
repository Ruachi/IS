#include "directshader.h"
#include "../core/utils.h"
#include "../materials/phong.h"

DirectShader::DirectShader( Vector3D color_, double maxDist_, Vector3D bgColor_ ) :
	Shader( bgColor_ ), maxDist( maxDist_ ), color( color_ )
{}

Vector3D DirectShader::computeColor(const Ray& r,
    const std::vector<Shape*>& objList,
    const std::vector<PointLightSource>& lsList) const
{
    int nL = lsList.size();
    Vector3D finalColor;

    Intersection its;
    if (Utils::getClosestIntersection(r, objList, its))
    {        

        for (size_t i = 0; i < nL; i++)
        {
            Vector3D wi = lsList.at(i).getPosition() - its.itsPoint; //point to light direction

            Vector3D wo = -r.d;    //point to cam direction

            Vector3D d = lsList.at(i).getPosition() - its.itsPoint;

            Ray vi = Ray(its.itsPoint, d);
            int visibility;

            if (Utils::hasIntersection(vi, objList))
            {
                visibility = 0;
            }
            else 
            {
                visibility = 1;
            }

            Vector3D aux = Utils::multiplyPerCanal(lsList.at(i).getIntensity(its.itsPoint), its.shape->getMaterial().getReflectance(its.normal, wo, wi) * visibility);
            
            finalColor += aux;
        }
        return color + finalColor;
    }
    return bgColor;
}
