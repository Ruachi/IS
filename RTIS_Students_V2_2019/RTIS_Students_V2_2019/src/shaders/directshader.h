#ifndef DIRECTSHADER_H
#define DIRECTSHADER_H

#include "shader.h"
#include "../lightsources/plafon.h"

class DirectShader : public Shader
{
public:
    DirectShader() = delete;
    DirectShader(Vector3D color_, double maxDist_, Vector3D bgColor_);

    virtual Vector3D computeColor(const Ray& r,
        const std::vector<Shape*>& objList,
        const std::vector<PointLightSource>& lsList) const;

    Vector3D computeColor(const Ray &r,
                             const std::vector<Shape*> &objList,
                             const std::vector<PointLightSource> &lsList,
                             const Plafon &p) const;
    bool hasSpecular();
    bool hasDiffuseOrGlossy();

private:
    double maxDist; // Used to store the value of the maximum distance which can be visualized
    Vector3D color; // Used to store the visualization color
    Vector3D calculateFourPoints(int i, int j, int halfSize, int nWidth, Vector3D wo,
        const std::vector<Shape*>& objList, const std::vector<PointLightSource>& lsList, Intersection its) const;    //to calculate more points in the soft shadow
};

#endif // DIRECTSHADER_H