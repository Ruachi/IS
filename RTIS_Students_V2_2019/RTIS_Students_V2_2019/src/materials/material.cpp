#include "material.h"

#include <iostream>

Material::Material()
{ }

double Material::getIndexOfRefraction() const
{
    std::cout << "Warning! Calling \"Material::getIndexOfRefraction()\" for a non-transmissive material"
              << std::endl;

    return -1;
}

Vector3D Material::getDiffuseCoefficient() const
{
    std::cout << "Calling a material with no diffuse coefficietn " << std::endl;
    return Vector3D(-1);
}
