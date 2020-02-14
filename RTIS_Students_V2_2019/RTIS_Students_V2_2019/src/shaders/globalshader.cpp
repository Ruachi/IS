#include "globalshader.h"
#include "../core/utils.h"
#include "../materials/phong.h"
#include "../core/hemisphericalsampler.h"

GlobalShader::GlobalShader(Vector3D color_, double maxDist_, Vector3D bgColor_) :
    Shader(bgColor_), maxDist(maxDist_), color(color_)
{}

Vector3D GlobalShader::computeColor(const Ray& r,
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

            }
        }
        else
        {
            Vector3D indirectLight;
            int nL = lsList.size();
            for (size_t i = 0; i < nL; i++)
            {
                Vector3D wi = lsList.at(i).getPosition() - its.itsPoint; //point to light direction

                Vector3D wo = -r.d;    //point to cam direction
                wi = wi.normalized();
                Ray shadowRay = Ray(its.itsPoint, wi);

                float d = wi.length();

                // CHECK IF LIGHT BELOW SURFACE
                double angle = dot(wi, its.normal);

                if (angle > 0)
                {
                    //Ray shadowRay = Ray(its.itsPoint, wi);
                    shadowRay.maxT = d;

                    if (!Utils::hasIntersection(shadowRay, objList))
                    {
                        Vector3D aux = Utils::multiplyPerCanal(lsList.at(i).getIntensity(its.itsPoint), 
                            its.shape->getMaterial().getReflectance(its.normal, wo, wi));
                        finalColor += (aux);
                    }
                }
                //calculate indirect
                int n = 5;
                Vector3D at = Vector3D(0.1, 0.1, 0.1);

                if (r.depth == 0)
                {
                    double aux = 1 / (2 * n * 3.14159);
                    HemisphericalSampler sample = HemisphericalSampler();
                    for (int i = 0; i < n; i++)
                    {
                        Vector3D dir = sample.getSample(its.normal);
                        Ray secondaryRay = Ray(its.itsPoint, dir, r.depth + 1);
                        indirectLight += computeColor(secondaryRay, objList, lsList);
                    }
                    indirectLight = Utils::multiplyPerCanal(aux, indirectLight);
                }
                else
                {
                    indirectLight += Utils::multiplyPerCanal(at, its.shape->getMaterial().getDiffuseCoefficient());
                }

            }
            finalColor = indirectLight + finalColor;
        }
        return finalColor;
    }
    return bgColor;
}