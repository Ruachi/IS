#include "directshader.h"
#include "../core/utils.h"
#include "../materials/phong.h"

DirectShader::DirectShader(Vector3D color_, double maxDist_, Vector3D bgColor_) :
    Shader(bgColor_), maxDist(maxDist_), color(color_)
{}

//perfect images
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

//chunksize images
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
            int chunckSize = 18;
            int halfSize = chunckSize / 2;
            int position;
            int depth = this->maxDist;
            int nL = lsList.size();
            std::vector<int> positions;

            for (size_t i = halfSize; i <= p.getNumberLightsWidth() - halfSize; i += chunckSize)
            {
                for (size_t j = halfSize; j <= p.getNumberLightsHeight() - halfSize; j += chunckSize)
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
                            for (int times = 0; times < chunckSize * chunckSize; times++)
                            {
								finalColor += Utils::multiplyPerCanal(lsList.at(position).getIntensity(its.itsPoint),
									its.shape->getMaterial().getReflectance(its.normal, wo, wi));
                            }
                        }
                        else if( chunckSize > 1)
                        {

                            //We compute the four corners inside chunckSize
                            int xsquareLeftTop = i - halfSize;
                            int ysquareLeftTop = j - halfSize;

                            int xsquareRightTop = i + halfSize;
                            int ysquareRightTop = j - halfSize;

                            int xsquareLeftBottom = i - halfSize;
                            int ysquareLeftBottom = j + halfSize;

                            int xsquareRightBottom = i + halfSize;
                            int ysquareRightBottom = j + halfSize;

                            std::vector<int> corners;
                            corners.push_back(xsquareLeftTop);
                            corners.push_back(ysquareLeftTop);
                            corners.push_back(xsquareRightTop);
                            corners.push_back(ysquareRightTop);
                            corners.push_back(xsquareLeftBottom);
                            corners.push_back(ysquareLeftBottom);
                            corners.push_back(xsquareRightBottom);
                            corners.push_back(ysquareRightBottom);

                            //We now throw a ray towards every corner. If it is visible, we will sum the contribution of all light points of chunkSize/4. Else, we throw a ray to every light.
                            int index = 0;
                            for (int corner = 0; corner < 4; corner++) 
                            {
                                position = corners[index] * p.getNumberLightsWidth() + corners[index + 1];
                                
                                Vector3D wi = lsList.at(position).getPosition() - its.itsPoint;
                                float d = wi.length();
                                wi = wi.normalized();
                                Ray visibilityRay = Ray(its.itsPoint, wi);
                                visibilityRay.maxT = d;

                                if (dot(wi, its.normal) > 0)
                                {
                                    if (!Utils::hasIntersection(visibilityRay, objList))
                                    {
                                        for (int times = 0; times < halfSize * halfSize; times++)
                                        {
                                            finalColor += Utils::multiplyPerCanal(lsList.at(position).getIntensity(its.itsPoint),
                                                its.shape->getMaterial().getReflectance(its.normal, wo, wi));
                                        }
                                    }
                                    else 
                                    {
                                        int xStart = corners[index];
                                        int yStart = corners[index + 1];

                                        //left top
                                        if (index == 0) 
                                        {
                                            for (int a = xStart; a < xStart + halfSize - 1; a++) 
                                            {
                                                for (int b = yStart; b <  yStart + halfSize - 1; b++) 
                                                {
                                                    position = a * p.getNumberLightsWidth() + b;
                                                    Vector3D wi = lsList.at(position).getPosition() - its.itsPoint;

                                                    float d = wi.length();
                                                    wi = wi.normalized();
                                                    Ray visibilityRay = Ray(its.itsPoint, wi);
                                                    visibilityRay.maxT = d;
                                                    if (dot(wi, its.normal) > 0)
                                                    {
                                                        if (!Utils::hasIntersection(visibilityRay, objList))
                                                        {
                                                            finalColor += Utils::multiplyPerCanal(lsList.at(position).getIntensity(its.itsPoint),
                                                                its.shape->getMaterial().getReflectance(its.normal, wo, wi));
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        //rigth top
                                        else if (index == 2) 
                                        {
                                            for (int a = xStart; a > xStart - halfSize; a--) 
                                            {
                                                for (int b = yStart; b > yStart - halfSize; b--) 
                                                {
                                                    position = a * p.getNumberLightsWidth() + b;
                                                    Vector3D wi = lsList.at(position).getPosition() - its.itsPoint;

                                                    float d = wi.length();
                                                    wi = wi.normalized();
                                                    Ray visibilityRay = Ray(its.itsPoint, wi);
                                                    visibilityRay.maxT = d;
                                                    if (dot(wi, its.normal) > 0)
                                                    {
                                                        if (!Utils::hasIntersection(visibilityRay, objList))
                                                        {
                                                            finalColor += Utils::multiplyPerCanal(lsList.at(position).getIntensity(its.itsPoint),
                                                                its.shape->getMaterial().getReflectance(its.normal, wo, wi));
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        //left bottom
                                        else if (index == 4) 
                                        {
                                            for (int a = xStart; a < xStart + halfSize - 1; a++)
                                            {
                                                for (int b = yStart; b < yStart + halfSize - 1; b++)
                                                {
                                                    position = a * p.getNumberLightsWidth() + b;
                                                    Vector3D wi = lsList.at(position).getPosition() - its.itsPoint;

                                                    float d = wi.length();
                                                    wi = wi.normalized();
                                                    Ray visibilityRay = Ray(its.itsPoint, wi);
                                                    visibilityRay.maxT = d;
                                                    if (dot(wi, its.normal) > 0)
                                                    {
                                                        if (!Utils::hasIntersection(visibilityRay, objList))
                                                        {
                                                            finalColor += Utils::multiplyPerCanal(lsList.at(position).getIntensity(its.itsPoint),
                                                                its.shape->getMaterial().getReflectance(its.normal, wo, wi));
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                        //right bottom
                                        else if (index == 6) 
                                        {
                                            for (int a = xStart; a > xStart - halfSize; a--)
                                            {
                                                for (int b = yStart; b > yStart - halfSize; b--)
                                                {
                                                    position = a * p.getNumberLightsWidth() + b;
                                                    Vector3D wi = lsList.at(position).getPosition() - its.itsPoint;

                                                    float d = wi.length();
                                                    wi = wi.normalized();
                                                    Ray visibilityRay = Ray(its.itsPoint, wi);
                                                    visibilityRay.maxT = d;
                                                    if (dot(wi, its.normal) > 0)
                                                    {
                                                        if (!Utils::hasIntersection(visibilityRay, objList))
                                                        {
                                                            finalColor += Utils::multiplyPerCanal(lsList.at(position).getIntensity(its.itsPoint),
                                                                its.shape->getMaterial().getReflectance(its.normal, wo, wi));
                                                        }
                                                    }
                                                }
                                            }
                                        }

                                    }
                                }
                                index++;
                            }
                            index = 0;
                        }
                    }
                }
            }
        }
		return finalColor;
    }
    return bgColor;
}