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

Vector3D DirectShader::calculateFourPoints(int i, int j, int halfSize, int nWidth, Vector3D wo,
    const std::vector<Shape*>& objList, const std::vector<PointLightSource>& lsList, 
    Intersection its, int depth, int &counter) const
{
    Vector3D auxiliarColor;
    for (int a = 0; a < 4; a++)
    {
        int aux;
        Vector3D wi;
        if (a == 0)
        {
            wi = lsList.at((i - halfSize) * nWidth + j - halfSize).getPosition() - its.itsPoint;
            aux = (i - halfSize) * nWidth + j - halfSize;
        }
        else if (a == 1)
        {
            wi = lsList.at((i - halfSize) * nWidth + j + halfSize).getPosition() - its.itsPoint;
            aux = (i - halfSize) * nWidth + j + halfSize;
        }
        else if (a == 2)
        {
            wi = lsList.at((i + halfSize) * nWidth + j - halfSize).getPosition() - its.itsPoint;
            aux = (i + halfSize) * nWidth + j - halfSize;
        }
        else if (a == 3)
        {
            wi = lsList.at((i + halfSize) * nWidth  + j + halfSize).getPosition() - its.itsPoint;
            aux = (i + halfSize) * nWidth + j + halfSize;
        }

        float d = wi.length();
        wi = wi.normalized();
        Ray visibilityRay = Ray(its.itsPoint, wi);
        visibilityRay.maxT = d;
        Vector3D areaNormal = (0, 0, 1);
        float theta_y = dot(areaNormal, wo);
        if (!Utils::hasIntersection(visibilityRay, objList))
        {
            counter++;
            auxiliarColor += Utils::multiplyPerCanal(lsList.at(aux).getIntensity(its.itsPoint) * 0.25,
                its.shape->getMaterial().getReflectance(its.normal, wo, wi));// *theta_y / sqrt(d);
        }
        else 
        {
            if (halfSize > 2 && depth > 0)
            {
                if (i > halfSize&& i < nWidth - halfSize && j > halfSize&& j < nWidth - halfSize)
                {
                    counter++;
                    depth--;
                    auxiliarColor += this->calculateFourPoints(i, j, halfSize, nWidth, wo,
                        objList, lsList, its, depth, counter);
                }
            }
        }
    }
    return auxiliarColor;
}

Vector3D DirectShader::computeColor(const Ray& r,
    const std::vector<Shape*>& objList,
    const std::vector<PointLightSource>& lsList,
    const Plafon& p) const
{
    Vector3D finalColor;
    Intersection its;
    Vector3D wo = -r.d;

    int counter = 0;    //to know number of lights used

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
            int chunckSize = 10;
            int halfSize = chunckSize / 2;
            int position;
            int depth = this->maxDist;
            int nL = lsList.size();
            std::vector<int> positions;
            Vector3D areaNormal = (0, 0, 1);

            for (size_t i = halfSize; i < p.getNumberLightsWidth() - chunckSize; i += chunckSize)
            {
                for (size_t j = halfSize; j < p.getNumberLightsHeight() - chunckSize; j += chunckSize)
                {
                    position = i * p.getNumberLightsWidth() + j;
                    Vector3D wi = lsList.at(position).getPosition() - its.itsPoint;
                    float d = wi.length();
                    wi = wi.normalized();
                    float theta_y = dot(areaNormal, wo);

                    if (dot(wi, its.normal) > 0)
                    {
                        Ray visibilityRay = Ray(its.itsPoint, wi);
                        visibilityRay.maxT = d;
                        if (!Utils::hasIntersection(visibilityRay, objList))
                        {
                            for (int times = 0; times < 4; times++)
                            {
                                counter++;
                                finalColor += Utils::multiplyPerCanal(lsList.at(position).getIntensity(its.itsPoint),
                                    its.shape->getMaterial().getReflectance(its.normal, wo, wi));// *theta_y / sqrt(d);
                            }
                        }
                        else 
                        {                         
                            finalColor += this->calculateFourPoints(i, j, halfSize, p.getNumberLightsWidth(), wo,
                                objList, lsList, its, depth, counter);
                        }
                    }
                }
            }
        }
        std::cout << counter << std::endl;
        return finalColor;// *36 / counter;
    }
    return bgColor;
}