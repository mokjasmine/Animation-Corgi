#ifndef _SHAPES_H_
#define _SHAPES_H_

#ifdef __APPLE__
#define glGenVertexArrays glGenVertexArraysAPPLE
#define glBindVertexArray glBindVertexArrayAPPLE
#endif

struct ShapeData
{
    GLuint vao;
    int numVertices;
};

void generateCube(GLuint program, ShapeData* cubeData);
void generateSphere(GLuint program, ShapeData* sphereData);
void generateCone(GLuint program, ShapeData* coneData);
void generateCylinder(GLuint program, ShapeData* cylData);
void generateTriangle(GLuint program, ShapeData* tridata);
void generateTriangle2(GLuint program, ShapeData* tridata2);
void generateBow(GLuint program, ShapeData* tridata, ShapeData* tridata2);

#endif