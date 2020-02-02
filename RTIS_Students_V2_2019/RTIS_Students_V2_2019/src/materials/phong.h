#ifndef PHONG
#define PHONG

#include "../core/vector3d.h"
#include "material.h"

class Phong : public Material
{
public:
    Phong(Vector3D ka, Vector3D kd, Vector3D ks, float s);

    Vector3D ambient;
    Vector3D diffuse;
    Vector3D specular;
    float shininess;

    virtual Vector3D getReflectance(const Vector3D& n, const Vector3D& wo,
        const Vector3D& wi) const;
    virtual bool hasSpecular() const;
    virtual bool hasTransmission() const;
    virtual bool hasDiffuseOrGlossy() const;
    virtual double getIndexOfRefraction() const;
};


#endif // PHONG
