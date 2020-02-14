#ifndef MIRROR
#define MIRROR

#include "../core/vector3d.h"
#include "../core/utils.h"
#include "material.h"

class Mirror : public Material
{
public:
    Mirror(Vector3D kd_, Vector3D ks_, float s_);

    Vector3D kd;
    Vector3D ks;
    float s;

    virtual Vector3D getReflectance(const Vector3D& n, const Vector3D& wo,
        const Vector3D& wi) const;
    virtual bool hasSpecular() const;
    virtual bool hasTransmission() const;
    virtual bool hasDiffuseOrGlossy() const;
    virtual double getIndexOfRefraction() const;
    virtual Vector3D getDiffuseCoefficient() const;
};


#endif // MIRROR
