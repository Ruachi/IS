#include "transmissive.h"
#include <iostream>

Transmissive::Transmissive(Vector3D ka_, Vector3D kd_, Vector3D ks_, float s_, float n_)
{
    ka = ka_;
    kd = kd_;   //diffuse
    ks = ks_;   //specular
    s = s_;     //shininess
    n = n_;
}

bool Transmissive::hasSpecular() const
{
    // This material does not have specular component
    return false;
}

bool Transmissive::hasTransmission() const
{
    return true;
}

bool Transmissive::hasDiffuseOrGlossy() const
{
    return false;
}

// Implement the function "getReflectance" here

Vector3D Transmissive::getReflectance(const Vector3D& n, const Vector3D& wo,
    const Vector3D& wi) const
{
    Vector3D rd, rs, wr;

    rd = kd * dot(wi, n);
    wr = Utils::computeReflectionDirection(-wo, n);
    rs = ks * pow(std::fmax(0, dot(wi, wr)), s);
	return  rs+rd;
}

double Transmissive::getIndexOfRefraction() const
{
    //std::cout << "Warning! Calling \"Material::getIndexOfRefraction()\" for a non-transmissive material"
    //          << std::endl;
    return n;
}

Vector3D Transmissive::getDiffuseCoefficient() const
{
    std::cout << "Warning !"
        << "Calling \"Material::getDiffuseCoefficient()\""
        << "for a non-diffuse or non-glossy material"
        << std::endl;
        
    return Vector3D(-1);
}
