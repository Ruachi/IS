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
    Vector3D wo = -r.d;

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
			int nL = lsList.size();
			for (size_t i = 0; i < nL; i++)
			{
				//direction light to intersection
				Vector3D wi = lsList.at(i).getPosition() - its.itsPoint;
				float d = wi.length();
				wi = wi.normalized();

				if (dot(wi, its.normal) > 0)
				{
                    //Ray from the intersection point with direction to light source
                    Ray shadowRay = Ray(its.itsPoint, wi);
					shadowRay.maxT = d;
					if (!Utils::hasIntersection(shadowRay, objList))
					{
						finalColor += Utils::multiplyPerCanal(lsList.at(i).getIntensity(its.itsPoint),
							its.shape->getMaterial().getReflectance(its.normal, wo, wi));
					}
				}
			}
	    }
     return finalColor;
    }
    return bgColor;
}

Vector3D DirectShader::computeColor(const Ray& r,
    const std::vector<Shape*>& objList,
    const std::vector<PointLightSource>& lsList,
    const Plafon& p) const
{
    Vector3D finalColor;
    Intersection its;
    Vector3D wo = -r.d;

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
            /*
            int nL = lsList.size();
            for (size_t i = 0; i < nL; i++)
            {
                //direction light to intersection
                Vector3D wi = lsList.at(i).getPosition() - its.itsPoint;
                float d = wi.length();
                wi = wi.normalized();

                if (dot(wi, its.normal) > 0)
                {
                    //Ray from the intersection point with direction to light source
                    Ray shadowRay = Ray(its.itsPoint, wi);
                    shadowRay.maxT = d;
                    if (!Utils::hasIntersection(shadowRay, objList))
                    {
                        finalColor += Utils::multiplyPerCanal(lsList.at(i).getIntensity(its.itsPoint),
                            its.shape->getMaterial().getReflectance(its.normal, wo, wi));
                    }
                }
            }
            */
            int chunckSize = 2;
            int halfSize = chunckSize / 2;
            int position;
            int nL = lsList.size();
            std::vector<int> positions;

            for (size_t i = halfSize; i < p.getNumberLightsWidth() - halfSize; i += chunckSize)
            {
                for (size_t j = halfSize; j < p.getNumberLightsHeight() - halfSize; j += chunckSize)
                {
                    position = i * p.getNumberLightsWidth() + j;
                    Vector3D wi = lsList.at(position).getPosition() - its.itsPoint;
                    float d = wi.length();
                    wi = wi.normalized();

                    if (dot(wi, its.normal) > 0)
                    {
                        Ray visibilityRay = Ray(its.itsPoint, wi);
                        visibilityRay.maxT = d;
                        if (!Utils::hasIntersection(visibilityRay, objList))
                        {
                            
                            for (int times = 0; times < 4; times++)
                            {
                                finalColor += Utils::multiplyPerCanal(lsList.at(position).getIntensity(its.itsPoint), // * (1 + chunckSize/10),
                                    its.shape->getMaterial().getReflectance(its.normal, wo, wi));
                            }
                            /*
                            finalColor += Utils::multiplyPerCanal(lsList.at(position).getIntensity(its.itsPoint), // * (1 + chunckSize/10),
                                its.shape->getMaterial().getReflectance(its.normal, wo, wi));
                                */
                        }
                        
                        else 
                        {
                            
                            position = (position - 2 * p.getNumberLightsWidth()) - halfSize / 2;
                            positions.push_back(position);
                            position = (position - 2 * p.getNumberLightsWidth()) + halfSize / 2;
                            positions.push_back(position);
                            position = (position + 2 * p.getNumberLightsWidth()) - halfSize / 2;
                            positions.push_back(position);
                            position = (position + 2 * p.getNumberLightsWidth()) + halfSize / 2;
                            positions.push_back(position);
                            
                            for (int a = 0; a < 4; a++)
                            {
                                int aux;
                                if (a == 0)
                                { 
                                    Vector3D wi = lsList.at((i - halfSize) * p.getNumberLightsWidth() + j - halfSize).getPosition() - its.itsPoint; 
                                    aux = (i - halfSize) * p.getNumberLightsWidth() + j - halfSize;
                                }
                                else if (a == 1)
                                {
                                    Vector3D wi = lsList.at((i - halfSize) * p.getNumberLightsWidth() + j + halfSize).getPosition() - its.itsPoint;
                                    aux = (i - halfSize) * p.getNumberLightsWidth() + j + halfSize;
                                }
                                else if (a == 2)
                                {
                                    Vector3D wi = lsList.at((i + halfSize) * p.getNumberLightsWidth() + j - halfSize).getPosition() - its.itsPoint;
                                    aux = (i + halfSize) * p.getNumberLightsWidth() + j - halfSize;
                                }
                                else if (a == 3) 
                                {
                                    Vector3D wi = lsList.at((i + halfSize) * p.getNumberLightsWidth() + j + halfSize).getPosition() - its.itsPoint;
                                    aux = (i + halfSize) * p.getNumberLightsWidth() + j + halfSize;
                                }
                                
                                float d = wi.length();
                                wi = wi.normalized();
                                Ray visibilityRay = Ray(its.itsPoint, wi);
                                visibilityRay.maxT = d;
                                if (!Utils::hasIntersection(visibilityRay, objList))
                                {
                                    finalColor += Utils::multiplyPerCanal(lsList.at(aux).getIntensity(its.itsPoint) * 0.25,
                                        its.shape->getMaterial().getReflectance(its.normal, wo, wi));
                                }
                            }
                            
                        }
                    }
                }
            }
        }
        return finalColor;
    }
    return bgColor;
}