#include "transmissive.h"
#include <iostream>

Transmissive::Transmissive(Vector3D v_, float n_)
{
    v = v_;
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
    return n * 2 * dot(wo, n) - wo;
}

double Transmissive::getIndexOfRefraction() const
{
    //std::cout << "Warning! Calling \"Material::getIndexOfRefraction()\" for a non-transmissive material"
    //          << std::endl;
    return n;
}
