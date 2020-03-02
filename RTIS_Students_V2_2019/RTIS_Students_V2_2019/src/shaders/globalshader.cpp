#include "globalshader.h"
#include "../core/utils.h"
#include "../materials/phong.h"
#include "../core/hemisphericalsampler.h"
#include <math.h>

GlobalShader::GlobalShader(Vector3D color_, double maxDist_, Vector3D bgColor_) :
    Shader(bgColor_), maxDist(maxDist_), color(color_)
{
	ambient = (0.1, 0.1, 0.1);
	maxDepth = 5;
}

Vector3D GlobalShader::computeColor(const Ray& r,
    const std::vector<Shape*>& objList,
    const std::vector<PointLightSource>& lsList) const
{
    Vector3D finalColor;
    Intersection its;
	bool ambiental = false;
	bool twoBounces = true;
	//Number of rays when bouncing
	int n = 1000;
	Vector3D wo = -r.d;
	
    if (Utils::getClosestIntersection(r, objList, its))
    {
        if (its.shape->getMaterial().hasSpecular())
        {
            Vector3D wr = Utils::computeReflectionDirection(r.d, its.normal);
            Ray reflectionRay = Ray(its.itsPoint, wr, r.depth);
            finalColor = computeColor(reflectionRay, objList, lsList);
        }
        else if (its.shape->getMaterial().hasTransmission())
        {
			Vector3D normalAuxiliar = its.normal.normalized();
			double eta = its.shape->getMaterial().getIndexOfRefraction();
			double cosThetaT_out;
			double thetaI = dot(its.normal.normalized(), wo);

			if (thetaI < 0)
			{
				thetaI = dot(-its.normal.normalized(), wo);
				normalAuxiliar = -normalAuxiliar;
				eta = 1 / eta;
			}

			if (!Utils::isTotalInternalReflection(eta, thetaI, cosThetaT_out))
			{
				Vector3D wt = Utils::computeTransmissionDirection(r, normalAuxiliar, eta, thetaI, cosThetaT_out);
				Ray refractionRay = Ray(its.itsPoint, wt, r.depth);
				return computeColor(refractionRay, objList, lsList);
			}
			else
			{
				Vector3D wr = Utils::computeReflectionDirection(r.d, normalAuxiliar);
				Ray reflectionRay = Ray(its.itsPoint, wr.normalized(), r.depth);
				return computeColor(reflectionRay, objList, lsList);
			}
        }
        else//if Phong
        {
			//DIRECT ILLUMINATION
			if (r.depth > 0) {
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
			Vector3D indirectLight;
			Vector3D wo = -r.d;
			//INDIRECT ILLUMINATION
			//for the first case, we only take into account the ambient light
			if (r.depth == maxDepth)
			{
				indirectLight += Utils::multiplyPerCanal( its.shape->getMaterial().getDiffuseCoefficient(),ambient);
			}
			else if (r.depth == 0 && twoBounces)
			{
				
				double aux = 1 / (2 * (n * M_PI));
				HemisphericalSampler sample = HemisphericalSampler();
				for (int i = 0; i < n; i++)
				{
					Vector3D dir = sample.getSample(its.normal.normalized());
					Ray secondaryRay = Ray(its.itsPoint, dir.normalized(), r.depth + 1);
					indirectLight += Utils::multiplyPerCanal(computeColor(secondaryRay, objList, lsList),
						its.shape->getMaterial().getReflectance(its.normal, wo.normalized(), dir.normalized()));
				}
				indirectLight = Utils::multiplyPerCanal(aux, indirectLight);
			}
			else if(r.depth > 0)
			{
				Vector3D dir = its.normal.normalized();
				Ray normalRay = Ray(its.itsPoint, dir, r.depth + 1);
				
				Vector3D wr = Utils::computeReflectionDirection(wo, its.normal.normalized());
				Ray secondaryRay = Ray(its.itsPoint, wr.normalized(), r.depth + 1);
				normalRay.maxT = wr.length();
				
				double aux = 1 / (4 * M_PI);
				indirectLight += (computeColor(normalRay, objList, lsList) + computeColor(secondaryRay, objList, lsList))*aux;

			}
			finalColor += indirectLight;
        }
        return finalColor;
    }
    return bgColor;
}