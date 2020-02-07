#include "phong.h"
#include <iostream>

Phong::Phong(Vector3D ka_, Vector3D kd_, Vector3D ks_, float s_)
{
    ka = ka_;
    kd = kd_;   //diffuse
    ks = ks_;   //specular
    s = s_;
}

bool Phong::hasSpecular() const
{
    // This material does not have specular component
    return false;
}

bool Phong::hasTransmission() const
{
    return false;
}

bool Phong::hasDiffuseOrGlossy() const
{
    return true;
}

// Implement the function "getReflectance" here
//
// (...)
//

Vector3D Phong::getReflectance(const Vector3D& n, const Vector3D& wo,
    const Vector3D& wi) const
{
    Vector3D rd, rs, wr;

    rd = kd * dot(wi, n);
    wr = Utils::computeReflectionDirection(-wo, n);
    rs = ks * pow(std::fmax(0, dot(wi, wr)), s);
	return  rs+rd;
}

double Phong::getIndexOfRefraction() const
{
    std::cout << "Warning! Calling \"Material::getIndexOfRefraction()\" for a non-transmissive material"
              << std::endl;
    return -1;
}
