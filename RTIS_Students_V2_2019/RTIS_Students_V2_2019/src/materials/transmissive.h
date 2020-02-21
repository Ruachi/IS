#ifndef transmissive
#define transmissive

#include "../core/vector3d.h"
#include "../core/utils.h"
#include "material.h"

class Transmissive : public Material
{
public:
    Transmissive(Vector3D v,  float n_);

    Vector3D v;
    float n;

    virtual Vector3D getReflectance(const Vector3D& n, const Vector3D& wo,
        const Vector3D& wi) const;
    virtual bool hasSpecular() const;
    virtual bool hasTransmission() const;
    virtual bool hasDiffuseOrGlossy() const;
    virtual double getIndexOfRefraction() const;
};


#endif // transmissive
