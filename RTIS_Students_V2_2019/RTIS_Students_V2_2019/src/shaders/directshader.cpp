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
        else if (its.shape->getMaterial().hasTransmission())
        {
			
            int nL = lsList.size();
            float eta1 = 1;
            float eta;

            for (size_t i = 0; i < nL; i++)
            {
                float eta2 = its.shape->getMaterial().getIndexOfRefraction();
                eta = eta2 / eta1;
                Vector3D wi = lsList.at(i).getPosition() - its.itsPoint;
                wi = wi.normalized();
                double thetaI = dot(its.normal, wi);
                double cosThetaT_out;

                if (!Utils::isTotalInternalReflection(eta, cos(thetaI), cosThetaT_out))
                {
                    finalColor += Vector3D(1, 0, 0);
                    //std::cout << "reflection" << std::endl;
                    Vector3D wt = Utils::computeTransmissionDirection(r, its.normal, eta, cos(thetaI), cosThetaT_out);
                    Ray refractionRay = Ray(its.itsPoint, wt.normalized(), r.depth + 1);
                    //finalColor += computeColor(refractionRay, objList, lsList);
                }
                else
                {
                    Vector3D wr = Utils::computeReflectionDirection(r.d, its.normal);
                    Ray reflectionRay = Ray(its.itsPoint, wr, r.depth + 1);
                    //finalColor += computeColor(reflectionRay, objList, lsList);
					finalColor += Vector3D(0, 1, 0);
                }
                
                /*
                Vector3D wi = lsList.at(i).getPosition() - its.itsPoint; //point to light direction
                wi = wi.normalized();
                double alpha = dot(its.normal, -wi);

                double nt = 1.5;
                double eta = its.shape->getMaterial().getIndexOfRefraction() / 1;
                double cosThetaOut;

                eta = nt;


                if (!Utils::isTotalInternalReflection(eta, cos(alpha), cosThetaOut))
                {
                    Vector3D wt = Utils::computeTransmissionDirection(r, its.normal, eta, cos(alpha), cosThetaOut);
                    Ray refractionRay = Ray(its.itsPoint, wt.normalized(), r.depth + 1);
                    finalColor = computeColor(refractionRay, objList, lsList);
                    //finalColor = Vector3D(0, 0, 1);
                }
                else
                {
                    Vector3D wr = Utils::computeReflectionDirection(r.d, its.normal);
                    Ray reflectionRay = Ray(its.itsPoint, wr, r.depth + 1);
                    finalColor = computeColor(reflectionRay, objList, lsList);
                    //finalColor = Vector3D(0, 1, 0);
                }
                */
            }
        }
        else//if Phong
        {
			int nL = lsList.size();
			for (size_t i = 0; i < nL; i++)
			{
				//direction light to intersection
				Vector3D wi = lsList.at(i).getPosition() - its.itsPoint;

				//direction intersection to view point
				Vector3D wo = -r.d;
				
				float d = wi.length();

				wi = wi.normalized();
				//Ray from the intersection point with direction to light source
				Ray shadowRay = Ray(its.itsPoint, wi);

				

				// Check if light bellow surface
				double angle = dot(wi, its.normal);

				if (angle > 0)
				{
					shadowRay.maxT = d;
					if (!Utils::hasIntersection(shadowRay, objList))
					{
						Vector3D aux = Utils::multiplyPerCanal(lsList.at(i).getIntensity(its.itsPoint),
							its.shape->getMaterial().getReflectance(its.normal, wo, wi));
						finalColor += (aux);
					}
				}
			}

	}
     return finalColor;
    }
    return bgColor;
}