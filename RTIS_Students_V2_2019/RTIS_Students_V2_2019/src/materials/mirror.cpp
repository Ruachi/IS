#include "mirror.h"
#include <iostream>

Mirror::Mirror(Vector3D kd_, Vector3D ks_, float s_)
{
    kd = kd_;   //diffuse
    ks = ks_;   //specular
    s = s_;     //shininess
}

bool Mirror::hasSpecular() const
{
    // This material does not have specular component
    return true;
}

bool Mirror::hasTransmission() const
{
    return false;
}

bool Mirror::hasDiffuseOrGlossy() const
{
    return false;
}

// Implement the function "getReflectance" here

Vector3D Mirror::getReflectance(const Vector3D& n, const Vector3D& wo,
    const Vector3D& wi) const
{
    Vector3D rd, rs, wr;

    rd = kd * dot(wi, n);
    wr = Utils::computeReflectionDirection(-wo, n);
    rs = ks * pow(std::fmax(0, dot(wi, wr)), s);
	return  rs+rd;
}

double Mirror::getIndexOfRefraction() const
{
    std::cout << "Warning! Calling \"Material::getIndexOfRefraction()\" for a non-transmissive material"
              << std::endl;
    return -1;
}

Vector3D Mirror::getDiffuseCoefficient() const
{
    std::cout << "Warning !"
        << "Calling \"Material::getDiffuseCoefficient()\""
        << "for a non-diffuse or non-glossy material"
        << std::endl;

    return Vector3D(-1);
}