#include <iostream>
#include <stdlib.h> /* srand, rand */
#include <vector>
#include <algorithm>

#include "core/film.h"
#include "core/matrix4x4.h"
#include "core/ray.h"
#include "core/utils.h"
#include "shapes/sphere.h"
#include "cameras/ortographic.h"
#include "cameras/perspective.h"

void transformationsExercise()
{
    /* *************** */
    /* TRANSFORMATIONS */
    /* *************** */
    std::string separator     = "\n----------------------------------------------\n";
    std::string separatorStar = "\n**********************************************\n";

    std::cout << separatorStar << "Construct and visualize transforms" << separatorStar << std::endl;

    // Identity Matrix
    Matrix4x4 idMat; // By default, a matrix4x4 is initialized with the identity matrix
    std::cout << "The content of matrix idMat is: \n" << std::endl;
    std::cout << idMat << separator << std::endl;

    // Translation Matrix
    double deltaX, deltaY, deltaZ;
    deltaX = 1; deltaY = -2; deltaZ = 3;
    Vector3D delta(deltaX, deltaY, deltaZ);
    Matrix4x4 translationMat = Matrix4x4::translate(delta);
    std::cout << "The content of matrix translationMat is: \n" << std::endl;
    std::cout << translationMat << separator << std::endl;

    // Scale Matrix
    deltaX = 2; deltaY = 1; deltaZ = -1;
    delta.x = deltaX; delta.y = deltaY; delta.z = deltaZ;
    Matrix4x4 scaleMatrix = Matrix4x4::scale(delta);
    std::cout << "The content of matrix scaleMatrix is: \n" << std::endl;
    std::cout << scaleMatrix << separator << std::endl;

    // Rotate around X Matrix
    double angleInDegrees = 60;
    double thetaInRadians = Utils::degreesToRadians(angleInDegrees);
    Matrix4x4 rotateXMatrix = Matrix4x4::rotate(thetaInRadians, Vector3D(1, 0, 0));
    std::cout << "The content of matrix rotateXMatrix is: \n" << std::endl;
    std::cout << rotateXMatrix << separator << std::endl;

    // Rotate around an arbitrary axis Matrix
    double arbitraryAngle = 30;
    double arbitraryAngleRadians = Utils::degreesToRadians(arbitraryAngle);
    Matrix4x4 rotateArbitraryAxisMatrix = Matrix4x4::rotate(arbitraryAngleRadians, Vector3D(1, 1, 1));
    std::cout << "The content of matrix rotateArtitraryAxisMatrix is: \n" << std::endl;
    std::cout << rotateArbitraryAxisMatrix << separator << std::endl;

    // Transposed and Inversion
    std::cout << separatorStar << "Inverting and Transposing a Matrix" << separatorStar << std::endl;
    Matrix4x4 translationMatTransposed;
    translationMat.transpose(translationMatTransposed);
    std::cout << "The transpose of matrix \n\n" << translationMat << "\n is \n\n"
              << translationMatTransposed << separator << std::endl;

    Matrix4x4 inverseTranslationMat;
    translationMat.inverse(inverseTranslationMat);
    std::cout << "The inverse of matrix \n\n" << translationMat << "\n is \n\n" << inverseTranslationMat << std::endl;
    std::cout << "And their multiplication should thus give the identity matrix:\n\n";
    std::cout << inverseTranslationMat * translationMat << "\n";

    // Combine here some transforms, and visualize the result
    std::cout << separatorStar << "Combine transforms and visualize the result" << separatorStar << std::endl;

    Matrix4x4 scaleTranslate = scaleMatrix * translationMat;
    std::cout << "The content of matrix scaleTranslate is: \n" << std::endl;
    std::cout << scaleTranslate << separator << std::endl;

    Matrix4x4 translateScale = translationMat * scaleMatrix;
    std::cout << "The content of matrix translateScale is: \n" << std::endl;
    std::cout << translateScale << separator << std::endl;
}

void normalTransformExercise()
{
    std::string separator = "\n----------------------------------------------\n";

    // NOTE: points, vectors and normals are represented, in this framework, using
    //  the Vector3D class. It is up to the programmer to take care of whether the
    //  Vector3D is representing a point, a vector or a normal. For example, when
    //  applying a transform using a Matrix4x4, the programmer must choose which
    //  "applyTransform" method of the namespace Matrix4x4 should be used.

    Vector3D v(1, 1, 0);
    Vector3D n(-1.0/sqrt(2), 1.0/sqrt(2), 0);
    Matrix4x4 S = Matrix4x4::scale(Vector3D(2, 1, 1));

    std::cout << "Vector v = " << v << "\n" << std::endl;
    std::cout << "Normal n = " << n << "\n" << std::endl;
    std::cout << "Scale Matrix: \n" << std::endl;
    std::cout << S << std::endl;

    Vector3D vTransformed = S.transformVector(v);
    std::cout << "Vector v\' = " << vTransformed << "\n" << std::endl;

    Vector3D nTransformed = S.transformVector(n);
    std::cout << "Vector n\' = " << nTransformed << "\n" << std::endl;

    double dotValue = dot(nTransformed, vTransformed);
    std::cout << "dot(n\', v\') = " << dotValue << "\n" << std::endl;

    Matrix4x4 STransposed, Sinverse;
    S.transpose(STransposed);
    STransposed.inverse(Sinverse);
    Vector3D nNormal = Sinverse.transformVector(n);
    std::cout << "Vector n\' = " << nNormal << "\n" << std::endl;

    std::cout << "dot(n\', v\') = " << dot(nNormal, vTransformed) << "\n" << std::endl;

}

void paintingAnImageExercise()
{
    // Define the film (i.e., image) resolution
    size_t resX, resY;
    resX = 512;
    resY = 512;
    Film film(resX, resY);
    

    for(unsigned int col = 0; col < resX; col++)
        {
            for(unsigned int row = 0; row < resY; row++)
            {
                Vector3D color((col + 0.5) / resX, (row + 0.5) / resY, 0);
                film.setPixelValue(col, row, color);
            }
        }

    // Save the final result to file
    film.save();

}

void filteringAnImageExercise()
{
    // Create two instances of the film class with the same resolution
    int resX, resY;
    resX = 512;
    resY = 512;
    Film f1(resX, resY);
    Film f2(resX, resY);

    // Create the original image
    //  Draw a circle centered at centerX, centerY (in pixels, image space)
    //   and with ray r (also in pixels)
    int centerX = resX / 2;
    int centerY = resY / 2;
    int r = std::min(centerX, centerY)/2;
    for(int lin=0; lin<resX; lin++)
    {
        for(int col=0; col<resY; col++)
        {
            // Use the equation of the sphere to determine the pixel color
            if( (lin-centerX)*(lin-centerX) + (col-centerY)*(col-centerY) < r*r )
                f1.setPixelValue(col, lin, Vector3D(1, 1, 0));
        }
    }

    // Filter-related variables
    // Declare here your filter-related variables
    // (e.g., FILTER SIZE)
    //(...)
    // Implement here your image filtering algorithm
    //We will do the average of a 3x3 field like the one on the assignment
    
    int nFilter = 3;
    int offset = nFilter / 2;
    int multiple = nFilter * nFilter;
    Film* inputImage = &f1;
    Film* outputImage = &f2;
    Vector3D averageSum = Vector3D();

    for (int i = 0; i < 100; i++)
    {
        for (int lin = 0; lin < resX - 1; lin++)
        {
            for (int col = 0; col < resY - 1; col++)
            {
                if ((col - offset) > 0 && (col + offset) < resX && (lin - offset) > 0 && (lin + offset) < resY)
                {
                    for (int x = col - offset; x <= col + offset; x++)
                    {
                        for (int y = lin - offset; y <= lin + offset; y++)
                        {
                            averageSum += inputImage->getPixelValue(x, y);
                        }
                    }
                    averageSum = averageSum / multiple;
                    outputImage->setPixelValue(col, lin, averageSum);
                    averageSum = Vector3D(0, 0, 0);
                }
            }
        }
        inputImage = outputImage;
    }

    // DO NOT FORGET TO SAVE YOUR IMAGE!
    f2.save();
}

void completeSphereClassExercise()
{
    // Make your intersection tests here
    Vector3D p(0, 0, 3);
    Matrix4x4 objectToWorld = Matrix4x4::translate(p);
    Ray r1 = Ray(Vector3D(), Vector3D(0, 0, 1));
    Ray r2 = Ray(Vector3D(), Vector3D(0, 1, 0));
    Sphere* s = new Sphere(1, objectToWorld);

    std::cout << s << std::endl;
    std::cout << s->toString() << std::endl;

    std::cout << s->rayIntersectP(r1) << std::endl;
    std::cout << s->rayIntersectP(r2) << std::endl;
    
}

void eqSolverExercise()
{
    EqSolver solver;
    rootValues roots;

    double A, B, C;

    // (...)
    A = 5;
    B = 2;
    C = 1;

	bool hasRoots = true;
    hasRoots = solver.rootQuadEq(A, B, C, roots);

    if(!hasRoots)
    {
        std::cout << "Equation has no real roots!" << std::endl;
    }
    else
    {
        // SHOW THE SOLUTIONS OF THE EQUATION
        std::cout << "First value: " << roots.values[0] << std::endl;
        std::cout << "Second value: " << roots.values[1] << std::endl;
    }
}

void raytrace()
{
    // Define the film (i.e., image) resolution
    size_t resX, resY;
    resX = 512;
    resY = 512;
    Film film(resX, resY);

    //here we create the sphere with the appropiate translation
    Vector3D p(0, 0, 3);
    Matrix4x4 objectToWorld = Matrix4x4::translate(p);
    Sphere* s = new Sphere(1, objectToWorld);

    std::cout << s->toString() << std::endl;    //verify the sphere is appropiately placed

    /* ******************* */
    /* Orthographic Camera */
    /* ******************* */
    Matrix4x4 cameraToWorld; // By default, this gives an ID transform
                             // meaning that the camera space = world space
    
    OrtographicCamera camOrtho(cameraToWorld, film);

    for (double y = 0; y < resY; y++)
    {
        for (double x = 0; x < resX; x++)
        {
            Ray r = camOrtho.generateRay(x / resX, y / resY);
            if (s->rayIntersectP(r))
            {
                film.setPixelValue(x, y, Vector3D(1, 0, 0));
            }
            else
            {
                film.setPixelValue(x, y, Vector3D(0, 0, 0));
            }
        }
    }
    

    /* ******************* */
    /* Perspective Camera */
    /* ******************* */

    
    double fovRadians = Utils::degreesToRadians(60);
    PerspectiveCamera camPersp(cameraToWorld, fovRadians, film);

    for (double y = 0; y < resY; y++)
    {
        for (double x = 0; x < resX; x++)
        {
            Ray r = camPersp.generateRay(x / resX, y / resY);
            if (s->rayIntersectP(r))
            {
                film.setPixelValue(x, y, Vector3D(1, 0, 0));
            }
            else
            {
                film.setPixelValue(x, y, Vector3D(0, 0, 0));
            }
        }
    }
    

    // Save the final result to file
    film.save();
}

int main()
{
    std::string separator = "\n----------------------------------------------\n";

    std::cout << separator << "RTIS - Ray Tracer for \"Imatge Sintetica\"" << separator << std::endl;

    // ASSIGNMENT 1
    //transformationsExercise();
    //normalTransformExercise();
    //paintingAnImageExercise();
    //filteringAnImageExercise();

    // ASSIGNMENT 2
    //eqSolverExercise();
    //completeSphereClassExercise();
    raytrace();

    std::cout << getchar();
    std::cout << "\n\n" << std::endl;
    return 0;
}
