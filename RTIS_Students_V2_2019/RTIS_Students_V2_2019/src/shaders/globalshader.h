#ifndef GLOBALSHADER_H
#define GLOBALSHADER_H

#include <vector>

#include "shader.h"
#include "../core/utils.h"

class GlobalShader : public Shader
{
public:
    GlobalShader() = delete;
    GlobalShader(Vector3D color_, double maxDist_, Vector3D bgColor_);

    virtual Vector3D computeColor(const Ray &r,
                             const std::vector<Shape*> &objList,
                             const std::vector<PointLightSource> &lsList) const;

    bool hasSpecular();
    bool hasDiffuseOrGlossy();

private:
    double maxDist; // Used to store the value of the maximum distance which can be visualized
    Vector3D color; // Used to store the visualization color
	Vector3D ambient;
};

#endif // GLOBALSHADER_H
