#include "phong.h"

#include <iostream>

Phong::Phong(Vector3D ka, Vector3D kd, Vector3D ks, float s)
{
    ambient = ka;
    diffuse = kd;
    specular = ks;
    shininess = s;
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
    Vector3D reflectance;
    Vector3D wr = cross(cross(n, wi), n)*2 - wi;
    reflectance = dot(diffuse, dot(wi, n)) + dot(shininess, pow(dot(wo, wr), 50));
    return reflectance;
}

double Phong::getIndexOfRefraction() const
{
    std::cout << "Warning! Calling \"Material::getIndexOfRefraction()\" for a non-transmissive material"
              << std::endl;

    return -1;
}
