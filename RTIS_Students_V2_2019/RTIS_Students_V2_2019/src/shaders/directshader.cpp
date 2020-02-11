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
        if (its.shape->getMaterial().hasSpecular())
        {
            Vector3D wr = Utils::computeReflectionDirection(r.d, its.normal);
            Ray reflectionRay = Ray(its.itsPoint, wr, r.depth + 1);
            finalColor = computeColor(reflectionRay, objList, lsList);
        }
        else 
        {
            for (size_t i = 0; i < nL; i++)
            {
                Vector3D wi = lsList.at(i).getPosition() - its.itsPoint; //point to light direction

                Vector3D wo = -r.d;    //point to cam direction
                Ray shadowRay = Ray(its.itsPoint, wi);


                float d = wi.length();
                wi = wi.normalized();

                // CHECK IF LIGHT BELOW SURFACE
                double angle = dot(wi, its.normal);

                if (angle > 0)
                {
                    //Ray shadowRay = Ray(its.itsPoint, wi);
                    shadowRay.maxT = d;

                    if (!Utils::hasIntersection(shadowRay, objList))
                    {
                        Vector3D aux = Utils::multiplyPerCanal(lsList.at(i).getIntensity(its.itsPoint), its.shape->getMaterial().getReflectance(its.normal, wo, wi));
                        finalColor += (aux);
                    }
                }
            }
        }
        return finalColor;
    }
    return bgColor;
}