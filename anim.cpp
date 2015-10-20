////////////////////////////////////////////////////
// anim.cpp version 4.1
// Template code for drawing an articulated figure.
// CS 174A 
////////////////////////////////////////////////////

#ifdef WIN32
#include <windows.h>
#include "GL/glew.h"
#include <GL/gl.h>
#include <GL/glu.h>
#else
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#ifdef WIN32
#include "GL/freeglut.h"
#else
#include <GLUT/glut.h>
#endif

#include "Ball.h"
#include "FrameSaver.h"
#include "Timer.h"
#include "Shapes.h"
#include "tga.h"

#include "Angel/Angel.h"
#include <stack>

#ifdef __APPLE__
#define glutInitContextVersion(a,b)
#define glutInitContextProfile(a)
#define glewExperimental int glewExperimentalAPPLE
#define glewInit()
#endif

FrameSaver FrSaver ;
Timer TM ;

BallData *Arcball = NULL ;
int Width = 800;
int Height = 800 ;
int Button = -1 ;
float Zoom = 1 ;
int PrevY = 0 ;

int Animate = 1 ;
int Recording = 0 ;

void resetArcball() ;
void save_image();
void instructions();
void set_colour(float r, float g, float b) ;

const int STRLEN = 100;
typedef char STR[STRLEN];

#define PI 3.1415926535897
#define X 0
#define Y 1
#define Z 2

//----------------------------------

#define NOTHING 0
#define FLYFORWARD 1
#define HEADTURN 2
#define WALK 1

float flyBeginTime=0;
float timeToFly=37; //100
float flySceneTime;
float crashBeginTime=37;
float timeToCrash=2;
float crashSceneTime;

// corgi hits tree at 37 seconds

//----------------------------------

//texture

GLuint texture_cube;
GLuint texture_earth;
GLuint texture_leaf;
GLuint texture_exp;
GLuint texture_bush;

// Structs that hold the Vertex Array Object index and number of vertices of each shape.
ShapeData cubeData;
ShapeData sphereData;
ShapeData coneData;
ShapeData cylData;
ShapeData tridata2;
ShapeData tridata;

// Matrix stack that can be used to push and pop the modelview matrix.
class MatrixStack {
    int    _index;
    int    _size;
    mat4*  _matrices;

   public:
    MatrixStack( int numMatrices = 32 ):_index(0), _size(numMatrices)
        { _matrices = new mat4[numMatrices]; }

    ~MatrixStack()
	{ delete[]_matrices; }

    void push( const mat4& m ) {
        assert( _index + 1 < _size );
        _matrices[_index++] = m;
    }

    mat4& pop( void ) {
        assert( _index - 1 >= 0 );
        _index--;
        return _matrices[_index];
    }
};

MatrixStack  mvstack;
mat4         model_view;
GLint        uModelView, uProjection, uView;
GLint        uAmbient, uDiffuse, uSpecular, uLightPos, uShininess;
GLint        uTex, uEnableTex;

// The eye point and look-at point.
// Currently unused. Use to control a camera with LookAt().
Angel::vec4 eye{0.0, 3.0, 20.0, 1.0};
Angel::vec4 ref{0.0, 3.0, 0.0, 1.0};
Angel::vec4 up{0.0, 1.0, 0.0, 0.0};
Angel::vec4 flyingdog{0, 3, 20, 1.0};
Angel::vec4 walkingdog{-2, 1.5, 13};

Angel::vec4 baseRef=ref;
Angel::vec4 baseUp=up;

Angel::vec4 EYE;
Angel::vec4 REF;

double TIME = 0.0 ;

/////////////////////////////////////////////////////
//    PROC: drawCylinder()
//    DOES: this function 
//          render a solid cylinder  oriented along the Z axis. Both bases are of radius 1. 
//          The bases of the cylinder are placed at Z = 0, and at Z = 1.
//
//          
// Don't change.
//////////////////////////////////////////////////////
void drawCylinder(void)
{
    glUniformMatrix4fv( uModelView, 1, GL_TRUE, model_view );
    glBindVertexArray( cylData.vao );
    glDrawArrays( GL_TRIANGLES, 0, cylData.numVertices );
}

//////////////////////////////////////////////////////
//    PROC: drawCone()
//    DOES: this function 
//          render a solid cone oriented along the Z axis with base radius 1. 
//          The base of the cone is placed at Z = 0, and the top at Z = 1. 
//         
// Don't change.
//////////////////////////////////////////////////////
void drawCone(void)
{
    glUniformMatrix4fv( uModelView, 1, GL_TRUE, model_view );
    glBindVertexArray( coneData.vao );
    glDrawArrays( GL_TRIANGLES, 0, coneData.numVertices );
}


//////////////////////////////////////////////////////
//    PROC: drawCube()
//    DOES: this function draws a cube with dimensions 1,1,1
//          centered around the origin.
// 
// Don't change.
//////////////////////////////////////////////////////

void drawCubeText(void)
{
    glBindTexture( GL_TEXTURE_2D, texture_cube );
    glUniform1i( uEnableTex, 1 );
    glUniformMatrix4fv( uModelView, 1, GL_TRUE, model_view );
    glBindVertexArray( cubeData.vao );
    glDrawArrays( GL_TRIANGLES, 0, cubeData.numVertices );
    glUniform1i( uEnableTex, 0 );
}

void drawCubeGrass(void)
{
    glBindTexture( GL_TEXTURE_2D, texture_cube );
    glUniform1i( uEnableTex, 1 );
    glUniformMatrix4fv( uModelView, 1, GL_TRUE, model_view );
    glBindVertexArray( cubeData.vao );
    glDrawArrays( GL_TRIANGLES, 0, cubeData.numVertices );
    glUniform1i( uEnableTex, 0 );
}

void drawCubeExplosion(void)
{
    glBindTexture( GL_TEXTURE_2D, texture_exp );
    glUniform1i( uEnableTex, 1 );
    glUniformMatrix4fv( uModelView, 1, GL_TRUE, model_view );
    glBindVertexArray( cubeData.vao );
    glDrawArrays( GL_TRIANGLES, 0, cubeData.numVertices );
    glUniform1i( uEnableTex, 0 );
}

void drawCube(void)
{
    glBindTexture( GL_TEXTURE_2D, texture_cube );
    glUniformMatrix4fv( uModelView, 1, GL_TRUE, model_view );
    glBindVertexArray( cubeData.vao );
    glDrawArrays( GL_TRIANGLES, 0, cubeData.numVertices );
}


//////////////////////////////////////////////////////
//    PROC: drawSphere()
//    DOES: this function draws a sphere with radius 1
//          centered around the origin.
// 
// Don't change.
//////////////////////////////////////////////////////

void drawSphereText(void)
{
    glBindTexture( GL_TEXTURE_2D, texture_earth);
    glUniform1i( uEnableTex, 1);
    glUniformMatrix4fv( uModelView, 1, GL_TRUE, model_view );
    glBindVertexArray( sphereData.vao );
    glDrawArrays( GL_TRIANGLES, 0, sphereData.numVertices );
    glUniform1i( uEnableTex, 0 );
}

void drawSphereLeaf(void)
{
    glBindTexture( GL_TEXTURE_2D, texture_leaf);
    glUniform1i( uEnableTex, 1);
    glUniformMatrix4fv( uModelView, 1, GL_TRUE, model_view );
    glBindVertexArray( sphereData.vao );
    glDrawArrays( GL_TRIANGLES, 0, sphereData.numVertices );
    glUniform1i( uEnableTex, 0 );
}

void drawSphereBush(void)
{
    glBindTexture( GL_TEXTURE_2D, texture_bush);
    glUniform1i( uEnableTex, 1);
    glUniformMatrix4fv( uModelView, 1, GL_TRUE, model_view );
    glBindVertexArray( sphereData.vao );
    glDrawArrays( GL_TRIANGLES, 0, sphereData.numVertices );
    glUniform1i( uEnableTex, 0 );
}

void drawSphere(void)
{
    glBindTexture( GL_TEXTURE_2D, texture_earth);
    glUniformMatrix4fv( uModelView, 1, GL_TRUE, model_view );
    glBindVertexArray( sphereData.vao );
    glDrawArrays( GL_TRIANGLES, 0, sphereData.numVertices );
}


void resetArcball()
{
    Ball_Init(Arcball);
    Ball_Place(Arcball,qOne,0.75);
}

//////////////////////////////////////////////////////
// My drawTriangle() function
//////////////////////////////////////////////////////

void drawBow(void)
{
    glUniformMatrix4fv(uModelView, 1, GL_TRUE, model_view);
    glBindVertexArray(tridata.vao);
    glDrawArrays(GL_TRIANGLES, 0, tridata.numVertices);
    
    glBindVertexArray(tridata2.vao);
    glDrawArrays(GL_TRIANGLES, 0, tridata2.numVertices);
}


//////////////////////////////////////////////////////
//    PROC: myKey()
//    DOES: this function gets caled for any keypresses
// 
//////////////////////////////////////////////////////

void myKey(unsigned char key, int x, int y)
{
    float time ;
    switch (key) {
        case 'q':
        case 27:
            exit(0); 
        case 's':
            FrSaver.DumpPPM(Width,Height) ;
            break;
        case 'r':
            resetArcball() ;
            glutPostRedisplay() ;
            break ;
        case 'a': // togle animation
            Animate = 1 - Animate ;
            // reset the timer to point to the current time		
            time = TM.GetElapsedTime() ;
            TM.Reset() ;
            // printf("Elapsed time %f\n", time) ;
            break ;
        case '0':
            //reset your object
            break ;
        case 'm':
            if( Recording == 1 )
            {
                printf("Frame recording disabled.\n") ;
                Recording = 0 ;
            }
            else
            {
                printf("Frame recording enabled.\n") ;
                Recording = 1  ;
            }
            FrSaver.Toggle(Width);
            break ;
        case 'h':
        case '?':
            instructions();
            break;
    }
    glutPostRedisplay() ;

}

/*********************************************************
    PROC: myinit()
    DOES: performs most of the OpenGL intialization
     -- change these with care, if you must.

**********************************************************/

void myinit(void)
{
    // Load shaders and use the resulting shader program
    GLuint program = InitShader( "vshader.glsl", "fshader.glsl" );
    glUseProgram(program);

    // Generate vertex arrays for geometric shapes
    generateCube(program, &cubeData);
    generateSphere(program, &sphereData);
    generateCone(program, &coneData);
    generateCylinder(program, &cylData);
    generateBow(program, &tridata, &tridata2);

    uModelView  = glGetUniformLocation( program, "ModelView"  );
    uProjection = glGetUniformLocation( program, "Projection" );
    uView       = glGetUniformLocation( program, "View"       );

    glClearColor(0.8, 1, 1, 1.0);
    //glClearColor( 0.1, 0.1, 0.2, 1.0 ); // dark blue background

    uAmbient   = glGetUniformLocation( program, "AmbientProduct"  );
    uDiffuse   = glGetUniformLocation( program, "DiffuseProduct"  );
    uSpecular  = glGetUniformLocation( program, "SpecularProduct" );
    uLightPos  = glGetUniformLocation( program, "LightPosition"   );
    uShininess = glGetUniformLocation( program, "Shininess"       );
    uTex       = glGetUniformLocation( program, "Tex"             );
    uEnableTex = glGetUniformLocation( program, "EnableTex"       );

    glUniform4f(uAmbient,    0.2f,  0.2f,  0.2f, 1.0f);
    glUniform4f(uDiffuse,    0.6f,  0.6f,  0.6f, 1.0f);
    glUniform4f(uSpecular,   0.2f,  0.2f,  0.2f, 1.0f);
    glUniform4f(uLightPos,  15.0f, 15.0f, 30.0f, 0.0f);
    glUniform1f(uShininess, 100.0f);

    glEnable(GL_DEPTH_TEST);
    
    TgaImage coolImage;
    if (!coolImage.loadTGA("challenge.tga"))
    {
        printf("Error loading image challenge file\n");
        exit(1);
    }
    
    TgaImage earthImage;
    if (!earthImage.loadTGA("earth.tga"))
    {
        printf("Error loading image earth file\n");
        exit(1);
    }
    
    TgaImage grassImage;
    if (!grassImage.loadTGA("grass.tga"))
    {
        printf("Error loading grass image file\n");
        exit(1);
    }
    
    TgaImage leafImage;
    if (!leafImage.loadTGA("leaf.tga"))
    {
        printf("Error loading leaf image file\n");
        exit(1);
    }
    
    TgaImage explosionImage;
    if (!explosionImage.loadTGA("explosion.tga"))
    {
        printf("Error loading explosion image file\n");
        exit(1);
    }
    
    TgaImage bushImage;
    if (!bushImage.loadTGA("bush.tga"))
    {
        printf("Error loading explosion image file\n");
        exit(1);
    }
     
    glGenTextures( 1, &texture_cube );
    glBindTexture( GL_TEXTURE_2D, texture_cube );
    glTexImage2D(GL_TEXTURE_2D, 0, 4, grassImage.width, grassImage.height, 0,
                 (grassImage.byteCount == 3) ? GL_BGR : GL_BGRA,
                 GL_UNSIGNED_BYTE, grassImage.data );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    
    glGenTextures( 1, &texture_exp );
    glBindTexture( GL_TEXTURE_2D, texture_exp );
    glTexImage2D(GL_TEXTURE_2D, 0, 4, explosionImage.width, explosionImage.height, 0,
                 (explosionImage.byteCount == 3) ? GL_BGR : GL_BGRA,
                 GL_UNSIGNED_BYTE, explosionImage.data );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    
    /*
    glGenTextures( 1, &texture_cube );
    glBindTexture( GL_TEXTURE_2D, texture_cube );
    glTexImage2D(GL_TEXTURE_2D, 0, 4, coolImage.width, coolImage.height, 0,
                 (coolImage.byteCount == 3) ? GL_BGR : GL_BGRA,
                 GL_UNSIGNED_BYTE, coolImage.data );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    */
    /*
    glGenTextures( 1, &texture_earth );
    glBindTexture( GL_TEXTURE_2D, texture_earth );
    glTexImage2D(GL_TEXTURE_2D, 0, 4, earthImage.width, earthImage.height, 0,
                 (earthImage.byteCount == 3) ? GL_BGR : GL_BGRA,
                 GL_UNSIGNED_BYTE, earthImage.data );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );*/
    
    glGenTextures( 1, &texture_leaf );
    glBindTexture( GL_TEXTURE_2D, texture_leaf );
    glTexImage2D(GL_TEXTURE_2D, 0, 4, leafImage.width, leafImage.height, 0,
                 (leafImage.byteCount == 3) ? GL_BGR : GL_BGRA,
                 GL_UNSIGNED_BYTE, leafImage.data );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    
    glGenTextures( 1, &texture_bush );
    glBindTexture( GL_TEXTURE_2D, texture_bush );
    glTexImage2D(GL_TEXTURE_2D, 0, 4, bushImage.width, bushImage.height, 0,
                 (bushImage.byteCount == 3) ? GL_BGR : GL_BGRA,
                 GL_UNSIGNED_BYTE, bushImage.data );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    
    // Set texture sampler variable to texture unit 0
    
    glUniform1i( uTex, 0);
    
    Arcball = new BallData;
    Ball_Init(Arcball);
    Ball_Place(Arcball,qOne,0.75);
}

/*********************************************************
    PROC: set_colour();
    DOES: sets all material properties to the given colour
    -- don't change
**********************************************************/

void set_colour(float r, float g, float b)
{
    float ambient  = 0.2f;
    float diffuse  = 0.6f;
    float specular = 0.2f;
    glUniform4f(uAmbient,  ambient*r,  ambient*g,  ambient*b,  1.0f);
    glUniform4f(uDiffuse,  diffuse*r,  diffuse*g,  diffuse*b,  1.0f);
    glUniform4f(uSpecular, specular*r, specular*g, specular*b, 1.0f);
}

/*********************************************************
**********************************************************
**********************************************************

    PROC: display()
    DOES: this gets called by the event handler to draw
          the scene, so this is where you need to build
          your ROBOT --  
      
        MAKE YOUR CHANGES AND ADDITIONS HERE

    Add other procedures if you like.

**********************************************************
**********************************************************
**********************************************************/
std::stack<mat4> s;

void drawExplosion()
{
    //float relTime = crashSceneTime;
    //set_colour(0.5, 0.5, 0.5);
    set_colour(0.88f, 0.88f, 0.88f);
    model_view *= Scale(20, 20, 1);
    model_view *= Translate(0, 0, 1*sin(20*TIME));
    drawCubeExplosion();
}

void drawTree()
{
    set_colour(0.6f, 0.3f, 0);
    model_view *= RotateX(90);
    model_view *= Translate(0, 0, -7);
    model_view *= Scale(2, 2, 10);
    drawCylinder();
    set_colour(0.8f, 1, 0.6f);
    model_view *= RotateX(-90);
    model_view *= Translate(0, 1.7, 0);
    model_view *= Scale(4, 0.8, 4);
    drawSphereLeaf();
}

void drawBush()
{
    model_view *= Scale(2, 1.8, 1);
    drawSphereBush();
}

void drawGround()
{
    set_colour(0.93f, 0.98f, 0.92f);
    //model_view *= Translate(-80, -3, -20);
    model_view *= Translate(0, -10, 0); // x=0;
    s.push(model_view);
    model_view *= Scale(1000, 0.01, 1000);
    drawCubeGrass();
    
    // draw trees
    model_view = s.top();
    model_view *= Translate(-220, 0, 0);
    drawTree();
    model_view =s.top();
    model_view *= Translate(50, 0, -5);
    drawTree();
    model_view = s.top();
    model_view *= Translate(30, 0, -20);
    model_view *= Scale(0.8, 0.8, 0.8);
    drawTree();
    model_view = s.top();
    model_view *= Translate(10, 0, -70);
    model_view *= Scale(0.6, 0.6, 0.6);
    drawTree();
    model_view = s.top();
    model_view *= Translate(-20, 0, -50);
    model_view *= Scale(0.8, 0.8, 0.8);
    drawTree();
    model_view = s.top();
    model_view *= Translate(-50, 0, -70);
    drawTree();
    model_view = s.top();
    model_view *= Translate(-70, 0, -40);
    model_view *= Scale(0.8, 0.8, 0.8);
    drawTree();
    model_view = s.top();
    model_view *= Translate(-90, 0, -60);
    model_view *= Scale(0.6, 0.6, 0.6);
    drawTree();
    model_view = s.top();
    model_view *= Translate(-120, 0, -45);
    model_view *= Scale(0.8, 0.8, 0.8);
    drawTree();
    model_view = s.top();
    model_view *= Translate(-130, 0, -90);
    drawTree();
    model_view = s.top();
    model_view *= Translate(-150, 0, -60);
    drawTree();
    model_view = s.top();
    model_view *= Translate(-170, 0, -40);
    model_view *= Scale(0.6, 0.6, 0.6);
    drawTree();
    model_view = s.top();
    model_view *= Translate(-200, 0, -70);
    drawTree();
    model_view = s.top();
    model_view *= Translate(-210, 0, -50);
    model_view *= Scale(0.8, 0.8, 0.8);
    drawTree();
    model_view = s.top();
    model_view *= Translate(-270, 0, 25);
    drawTree();
    model_view = s.top();
    model_view *= Translate(-300, 0, 5);
    model_view *= Scale(0.8, 0.8, 0.8);
    drawTree();
    
    model_view =s.top();
    model_view *= Translate(50, 0, 25);
    drawTree();
    model_view = s.top();
    model_view *= Translate(30, 0, 40);
    model_view *= Scale(0.8, 0.8, 0.8);
    drawTree();
    model_view = s.top();
    model_view *= Translate(10, 0, 90);
    model_view *= Scale(0.6, 0.6, 0.6);
    drawTree();
    model_view = s.top();
    model_view *= Translate(-20, 0, 70);
    model_view *= Scale(0.8, 0.8, 0.8);
    drawTree();
    model_view = s.top();
    model_view *= Translate(-50, 0, 90);
    drawTree();
    model_view = s.top();
    model_view *= Translate(-70, 0, 60);
    model_view *= Scale(0.8, 0.8, 0.8);
    drawTree();
    model_view = s.top();
    model_view *= Translate(-90, 0, 80);
    model_view *= Scale(0.6, 0.6, 0.6);
    drawTree();
    model_view = s.top();
    model_view *= Translate(-120, 0, 85);
    model_view *= Scale(0.8, 0.8, 0.8);
    drawTree();
    model_view = s.top();
    model_view *= Translate(-130, 0, 110);
    drawTree();
    model_view = s.top();
    model_view *= Translate(-150, 0, 80);
    drawTree();
    model_view = s.top();
    model_view *= Translate(-170, 0, 60);
    model_view *= Scale(0.6, 0.6, 0.6);
    drawTree();
    model_view = s.top();
    model_view *= Translate(-200, 0, 90);
    drawTree();
    model_view = s.top();
    model_view *= Translate(-210, 0, 70);
    model_view *= Scale(0.8, 0.8, 0.8);
    drawTree();
    model_view = s.top();
    model_view *= Translate(-270, 0, 60);
    drawTree();
    
    // draw bushes
    model_view = s.top();
    model_view *= Translate(75, 0, 20);
    model_view *= RotateY(90);
    model_view *= Scale(2,2,2);
    drawBush();
}

void balloon()
{
    //bottom layer
    //s.pop();
    model_view = s.top();
    model_view *= Translate(-0.8, 8, 0.8);
    set_colour(0.6f, 1, 1); // light blue
    model_view *= Scale (0.7, 0.8, 0.7);
    drawSphere();
    model_view *= Translate(0, -1, 0);
    model_view *= RotateX(90);
    model_view *= Scale (0.1, 0.1, 0.1);
    drawCone();
    set_colour(0.01f, 0.01f, 0.01f);
    model_view = s.top();
    model_view *= Translate(-0.4, 5, 0.4);
    model_view *= RotateY(45);
    model_view *= RotateZ(13);
    model_view *= Scale(0.03, 5, 0.03);
    drawCube();
    
    model_view = s.top();
    set_colour(0.8f, 0.8f, 1); // lavender
    model_view *= Translate(0.8, 8, 0.8);
    model_view *= Scale (0.7, 0.8, 0.7);
    drawSphere();
    model_view *= Translate(0, -1, 0);
    model_view *= RotateX(90);
    model_view *= Scale (0.1, 0.1, 0.1);
    drawCone();
    set_colour(0.01f, 0.01f, 0.01f);
    model_view = s.top();
    model_view *= Translate(0.4, 5, 0.4);
    model_view *= RotateY(-45);
    model_view *= RotateZ(-13);
    model_view *= Scale(0.03, 5, 0.03);
    drawCube();
    
    model_view = s.top();
    model_view *= Translate(-0.8, 8, -0.8);
    set_colour(1, 0.6f, 0.8); // light pink
    model_view *= Scale (0.7, 0.8, 0.7);
    drawSphere();
    model_view *= Translate(0, -1, 0);
    model_view *= RotateX(90);
    model_view *= Scale (0.1, 0.1, 0.1);
    drawCone();
    set_colour(0.01f, 0.01f, 0.01f);
    model_view = s.top();
    model_view *= Translate(-0.4, 5, -0.4);
    model_view *= RotateY(-45);
    model_view *= RotateZ(13);
    model_view *= Scale(0.03, 5, 0.03);
    drawCube();
    
    model_view = s.top();
    model_view *= Translate(0.8, 8, -0.8);
    set_colour(1, 0.5f, 0); // orange
    model_view *= Scale (0.7, 0.8, 0.7);
    drawSphere();
    model_view *= Translate(0, -1, 0);
    model_view *= RotateX(90);
    model_view *= Scale (0.1, 0.1, 0.1);
    drawCone();
    set_colour(0.01f, 0.01f, 0.01f);
    model_view = s.top();
    model_view *= Translate(0.4, 5, -0.4);
    model_view *= RotateY(45);
    model_view *= RotateZ(-13);
    model_view *= Scale(0.03, 5, 0.03);
    drawCube();
    
    //second layer
    model_view = s.top();
    set_colour(0.5f, 1, 0); // light green
    model_view *= Translate(0, 9, 2);
    model_view *= RotateX(15);
    model_view *= Scale(0.7, 0.8, 0.7);
    drawSphere();
    model_view *= Translate(0, -1, 0);
    model_view *= RotateX(90);
    model_view *= Scale (0.1, 0.1, 0.1);
    drawCone();
    set_colour(0.01f, 0.01f, 0.01f);
    model_view = s.top();
    model_view *= Translate(0, 5.5, 0.9);
    model_view *= RotateX(17);
    model_view *= Scale(0.03, 6, 0.03);
    drawCube();
    
    model_view = s.top();
    set_colour(0, 0.8f, 0.8f); // blue
    model_view *= Translate(0, 9, -2);
    model_view *= RotateX(-15);
    model_view *= Scale(0.7, 0.8, 0.7);
    drawSphere();
    model_view *= Translate(0, -1, 0);
    model_view *= RotateX(90);
    model_view *= Scale (0.1, 0.1, 0.1);
    drawCone();
    set_colour(0.01f, 0.01f, 0.01f);
    model_view = s.top();
    model_view *= Translate(0, 5.5, -0.9);
    model_view *= RotateX(-17);
    model_view *= Scale(0.03, 6, 0.03);
    drawCube();
    
    model_view = s.top();
    set_colour(1, 1, 0); // yellow
    model_view *= Translate(2, 9, 0);
    model_view *= RotateZ(-15);
    model_view *= Scale(0.7, 0.8, 0.7);
    drawSphere();
    model_view *= Translate(0, -1, 0);
    model_view *= RotateX(90);
    model_view *= Scale (0.1, 0.1, 0.1);
    drawCone();
    set_colour(0.01f, 0.01f, 0.01f);
    model_view = s.top();
    model_view *= Translate(0.9, 5.5, 0);
    model_view *= RotateZ(-17);
    model_view *= Scale(0.03, 6, 0.03);
    drawCube();
    
    model_view = s.top();
    set_colour(0.3f, 0, 0.6f); // purple
    model_view *= Translate(-2, 9, 0);
    model_view *= RotateZ(15);
    model_view *= Scale(0.7, 0.8, 0.7);
    drawSphere();
    model_view *= Translate(0, -1, 0);
    model_view *= RotateX(90);
    model_view *= Scale (0.1, 0.1, 0.1);
    drawCone();
    set_colour(0.01f, 0.01f, 0.01f);
    model_view = s.top();
    model_view *= Translate(-0.9, 5.5, 0);
    model_view *= RotateZ(17);
    model_view *= Scale(0.03, 6, 0.03);
    drawCube();
    
    //top layer
    model_view = s.top();
    model_view *= Translate(-0.8, 10, 0.8);
    set_colour(0, 0.5f, 1); // dark blue
    model_view *= Scale (0.7, 0.8, 0.7);
    drawSphere();
    model_view *= Translate(0, -1, 0);
    model_view *= RotateX(90);
    model_view *= Scale (0.1, 0.1, 0.1);
    drawCone();
    
    model_view = s.top();
    set_colour(0.8f, 0, 0.4f); // magenta
    model_view *= Translate(0.8, 10, 0.8);
    model_view *= Scale (0.7, 0.8, 0.7);
    drawSphere();
    model_view *= Translate(0, -1, 0);
    model_view *= RotateX(90);
    model_view *= Scale (0.1, 0.1, 0.1);
    drawCone();
    
    model_view = s.top();
    set_colour(1, 0, 0); // red
    model_view *= Translate(-0.8, 10, -0.8);
    model_view *= Scale (0.7, 0.8, 0.7);
    drawSphere();
    model_view *= Translate(0, -1, 0);
    model_view *= RotateX(90);
    model_view *= Scale (0.1, 0.1, 0.1);
    drawCone();
    
    model_view = s.top();
    set_colour(0.6f, 1, 0.8f); // mint
    model_view *= Translate(0.8, 10, -0.8);
    model_view *= Scale (0.7, 0.8, 0.7);
    drawSphere();
    model_view *= Translate(0, -1, 0);
    model_view *= RotateX(90);
    model_view *= Scale (0.1, 0.1, 0.1);
    drawCone();
}

void drawCorgi(float action)
{
    //body
    model_view = s.top();
    if (action >= WALK)
    {
        model_view *= Translate(-8*TIME, 0, 0);
    }
    model_view *= Translate(0, 0.07*sin(30*TIME), 0);
    model_view *= Translate(80, 1.3, 20);
    s.push(model_view);
    set_colour(1.0f, 0.6f, 0.2f);
    model_view *= Scale(1.2, 0.7, 0.7);
    model_view *= RotateY(90);
    drawCylinder();
    
    //butt
    model_view = s.top();
    model_view *= Translate(1, 0, 0);
    model_view *= Scale(0.7, 0.7, 0.7);
    drawSphere();
    
    //tail
    model_view *= Translate(0.7, 0.7, 0);
    model_view *= Translate(0, 0, 0.1*sin(20*TIME));
    model_view *= Scale(0.2, 0.2, 0.2);
    drawSphere();
    
    //chest
    model_view = s.top();
    model_view *= Translate(-1, 0, 0);
    model_view *= Scale(0.7, 0.7, 0.7);
    drawSphere();
    
    //neck
    model_view = s.top();
    model_view *= Translate(-1.2, 0.3, 0);
    s.push(model_view);
    model_view *= Scale(0.4, 0.4, 0.4);
    model_view *= RotateY(90);
    model_view *= RotateX(45);
    drawCylinder();
    
    //bow
    model_view = s.top();
    model_view *= Translate(-0.2, 0.14, 0.5);
    //set_colour(0.59f,1,1);
    set_colour(0.8f, 0, 0.4f);
    model_view *= RotateY(180);
    model_view *= RotateZ(-35);
    model_view *= Scale(0.5, 0.5, 0.5);
    drawBow();

    //head
    model_view = s.top();
    set_colour(1.0f, 0.6f, 0.2f);
    model_view *= Translate(-0.5, 0.5, 0);
    s.push(model_view);
    model_view *= Scale(0.55, 0.55, 0.55);
    drawSphere();
    
    //eyes
    model_view = s.top();
    set_colour(0.01f, 0.01f, 0.01f);
    model_view *= Translate(-0.45, 0.2, 0.25);
    model_view *= RotateZ(-20);
    model_view *= RotateY(25);
    model_view *= Scale(0.01, 0.09, 0.09);
    drawSphere();
    model_view = s.top();
    //set_colour(1, 1, 0);
    model_view *= Translate(-0.4, 0.23, 0.33);
    model_view *= RotateZ(-50);
    model_view *= RotateX(50);
    model_view *= Scale(0.01, 0.07, 0.01);
    drawCube();
    model_view = s.top();
    model_view *= Translate(-0.4, 0.25, 0.31);
    model_view *= RotateZ(-50);
    model_view *= RotateX(40);
    model_view *= Scale(0.01, 0.07, 0.01);
    drawCube();
    model_view = s.top();
    model_view *= Translate(-0.4, 0.27, 0.29);
    model_view *= RotateZ(-50);
    model_view *= RotateX(30);
    model_view *= Scale(0.01, 0.07, 0.01);
    drawCube();
    
    model_view = s.top();
    set_colour(0.01f, 0.01f, 0.01f);
    model_view *= Translate(-0.45, 0.2, -0.25);
    model_view *= RotateZ(-20);
    model_view *= RotateY(-25);
    model_view *= Scale(0.01, 0.09, 0.09);
    drawSphere();
    model_view = s.top();
    model_view *= Translate(-0.4, 0.23, -0.33);
    model_view *= RotateZ(-50);
    model_view *= RotateX(-50);
    model_view *= Scale(0.01, 0.07, 0.01);
    drawCube();
    model_view = s.top();
    model_view *= Translate(-0.4, 0.25, -0.31);
    model_view *= RotateZ(-50);
    model_view *= RotateX(-40);
    model_view *= Scale(0.01, 0.07, 0.01);
    drawCube();
    model_view = s.top();
    model_view *= Translate(-0.4, 0.27, -0.29);
    model_view *= RotateZ(-50);
    model_view *= RotateX(-30);
    model_view *= Scale(0.01, 0.07, 0.01);
    drawCube();
    
    //snout
    model_view = s.top();
    set_colour(1.0f, 0.6f, 0.2f);
    model_view *= Translate(-0.35, -0.2, 0);
    model_view *= Scale(0.25, 0.25, 0.25);
    model_view *= RotateY(90);
    drawCylinder();
    s.push(model_view);
    
    model_view = s.top();
    model_view *= Translate(0, 0, -1);
    model_view *= Scale(1, 1, 1);
    drawSphere();
    
    //nose
    model_view = s.top();
    set_colour(0.01f, 0.01f, 0.01f);
    model_view *= Translate(0, 0.3, -1.8);
    model_view *= Scale(0.4, 0.4, 0.4);
    drawSphere();
    
    //ear
    s.pop();
    model_view = s.top();
    set_colour(1.0f, 0.6f, 0.2f);
    model_view *= Translate(0, 0.45, 0.35);
    model_view *= RotateX(120);
    model_view *= Scale(0.01, 0.17, 0.35);
    drawSphere();
    
    model_view = s.top();
    model_view *= Translate(0, 0.45, -0.35);
    model_view *= RotateX(60);
    model_view *= Scale(0.01, 0.17, 0.35);
    drawSphere();
    
    //legs
    s.pop();
    s.pop();
    model_view = s.top();
    model_view *= Translate(-1, -0.4, 0.4);
    model_view *= RotateX(90);
    model_view *= RotateY(20*sin(TIME*15));
    model_view *= Scale(0.15, 0.15, 0.8); //x, z, y
    drawCylinder();
    //paw
    model_view *= Translate(-0.15, 0, 0.95);
    model_view *= Scale(1.2, 1, 0.15);
    drawSphere();

    model_view = s.top();
    model_view *= Translate(-1, -0.4, -0.4);
    model_view *= RotateX(90);
    model_view *= RotateY(-20*sin(TIME*15));
    model_view *= Scale(0.15, 0.15, 0.8); //x, z, y
    drawCylinder();
    //paw
    model_view *= Translate(-0.15, 0, 0.95);
    model_view *= Scale(1.2, 1, 0.15);
    drawSphere();
    
    model_view = s.top();
    model_view *= Translate(1, -0.4, 0.4);
    model_view *= RotateX(90);
    model_view *= RotateY(-20*sin(TIME*15));
    model_view *= Scale(0.15, 0.15, 0.8); //x, z, y
    drawCylinder();
    //paw
    model_view *= Translate(-0.15, 0, 0.95);
    model_view *= Scale(1.2, 1, 0.15);
    drawSphere();
    
    model_view = s.top();
    model_view *= Translate(1, -0.4, -0.4);
    model_view *= RotateX(90);
    model_view *= RotateY(20*sin(TIME*15));
    model_view *= Scale(0.15, 0.15, 0.8); //x, z, y
    drawCylinder();
    //paw
    model_view *= Translate(-0.15, 0, 0.95);
    model_view *= Scale(1.2, 1, 0.15);
    drawSphere();
}

void flyingCorgi(float action)
{
    bool headturnt = false;
    s.pop();
    model_view = s.top();
    if (action >= FLYFORWARD)
    {
        float relTime = flySceneTime - 10;//10
        model_view *= Translate(-8*relTime, 0, 0);
    }
    if(action >= NOTHING)
    {
        model_view *= Translate(0, 1*sin(2*TIME), 0);
    }
    model_view *= Translate(0, 10, 0); // x=0
    s.push(model_view);
    set_colour(1.0, 0.45f, 0.0f);
    model_view *= Scale(1.2, 0.7, 0.7);
    model_view *= RotateY(90);
    drawCylinder();
    
    //harness
    set_colour(0.01f, 0.01f, 0.01f);
    model_view *= Translate(0, 0, -0.7); //z, y, x
    model_view *= Scale(1.1, 1.1, 0.1); //z, y, x
    drawCylinder();
    
    model_view *= Translate(0, 0, 10); //z, y, x
    drawCylinder();
    
    model_view = s.top();
    model_view *= Translate(-0.3, 0, 0);
    model_view *= Scale(1.1, 0.2, 1.5);
    drawCube();
    
    model_view = s.top();
    model_view *= Translate(-0.3, 0, 0);
    model_view *= RotateX(90);
    model_view *= Scale(1.1, 0.2, 1.5);
    drawCube();

    //butt
    model_view = s.top();
    set_colour(1.0, 0.45f, 0.0f);
    model_view *= Translate(1, 0, 0);
    model_view *= Scale(0.7, 0.7, 0.7);
    drawSphere();
    //tail
    model_view *= Translate(0.7, 0.7, 0);
    //model_view *= Translate(0, 0, 0.1*sin(20*TIME));
    model_view *= Scale(0.2, 0.2, 0.2);
    drawSphere();
    
    //chest
    model_view = s.top();
    model_view *= Translate(-1, 0, 0);
    model_view *= Scale(0.7, 0.7, 0.7);
    drawSphere();
    
    //neck
    model_view = s.top();
    model_view *= Translate(-1.2, 0.3, 0);
    s.push(model_view);
    model_view *= Scale(0.4, 0.4, 0.4);
    model_view *= RotateY(90);
    model_view *= RotateX(45);
    drawCylinder();
    
    //head
    model_view = s.top();
    
    model_view *= Translate(-0.5, 0.5, 0);
    if (action == HEADTURN)
    {
        float relTime = flySceneTime - 15; //15
        if(relTime >= 0 && relTime < 1)
        {
            model_view *= RotateY(70*relTime);
            model_view *= RotateZ(20*relTime);
        }
        else if(relTime >=1)
        {
            model_view *= RotateY(70);
            model_view *= RotateZ(20);
        }
    }
    s.push(model_view);
    
    model_view *= Scale(0.55, 0.55, 0.55);
    
    drawSphere();
    
    //eyes
   
    
    float relTime = flySceneTime - 17; //17
    //float relTime = 13;
        if (relTime >=0)
        {
            model_view = s.top();
            set_colour(1, 0, 0);
            model_view *= Translate(-0.47, 0.2, 0.21);
            model_view *= RotateZ(-20);
            model_view *= RotateY(25);
            model_view *= RotateX(45);
            model_view *= Scale(0.001, 0.1, 0.15);
            if (relTime >= 5)
                model_view *= Scale(1, 1.5, 1.5);
            if (relTime >= 9)
                model_view *= Scale(1, 1.5, 1.5);
            if (relTime >= 13)
            {
                model_view *= Translate(0, 0.3, 0.2);
                model_view *= Scale (1, 1.5, 1.5);
            }
            drawCube();
            model_view = s.top();
            model_view *= Translate(-0.47, 0.25, 0.165); // closer
            model_view *= RotateY(-60);
            model_view *= RotateX(-15);
            model_view *= Scale(0.05, 0.05, 0.001);
            if (relTime >= 5)
            {
                model_view *= Translate(-0.3, 0.5, 0);
                model_view *= Scale(1.5, 1.5, 1);
            }
            if (relTime >= 9)
            {
                model_view *= Translate(-0.3, 0.5, 0); //(z, y, x)
                model_view *= Scale(1.5, 1.5, 1);
            }
            if (relTime >= 13)
            {
                model_view *= Translate(0.8, 0, 0);
                model_view *= Translate(-0.4, 0.5, 0);
                model_view *= Scale(1.5, 1.5, 1);
            }
            drawCone();
    
            model_view = s.top();
            model_view *= Translate(-0.45, 0.2, 0.25);
            model_view *= RotateZ(-20);
            model_view *= RotateY(25);
            model_view *= RotateX(-45);
            model_view *= Scale(0.001, 0.1, 0.15);
            if (relTime >= 5)
            {
                model_view *= Translate(0, -0.05, 0.05); //(x, -z, y)
                model_view *= Scale(1, 1.5, 1.5);
            }
            if (relTime >= 9)
            {
                model_view *= Translate(0, -0.1, 0.05);
                model_view *= Scale(1, 1.5, 1.5);
            }
            if (relTime >= 13)
            {
                model_view *= Translate(0, -0.3, 0.3);
                model_view *= Translate(0, -0.1, 0.05);
                model_view *= Scale (1, 1.5, 1.5);
            }
            drawCube();
            model_view = s.top();
            model_view *= Translate(-0.4, 0.24, 0.295); // farther
            model_view *= RotateY(-60);
            model_view *= RotateX(-15);
            model_view *= Scale(0.05, 0.05, 0.001);
            if (relTime >= 5)
            {
                model_view *= Translate(0.7, 0.4, 0); //(z, y, x)
                model_view *= Scale(1.5, 1.5, 1);
            }
            if (relTime >= 9)
            {
                model_view *= Translate(0.75, 0.4, 0);
                model_view *= Scale(1.5, 1.5, 1);
            }
            if (relTime >= 13)
            {
                model_view *= Translate(1, 0, 0);
                model_view *= Translate(0.85, 0.3, 0);
                model_view *= Scale(1.5, 1.5, 1);
            }
            drawCone();
    
            ///////////////////////////////////////////
            ///////////////////////////////////////////
                                        
            model_view = s.top();
            set_colour(1, 0, 0);
            model_view *= Translate(-0.47, 0.2, -0.21);
            model_view *= RotateZ(-20);
            model_view *= RotateY(-25);
            model_view *= RotateX(-45);
            model_view *= Scale(0.001, 0.1, 0.15);
            if (relTime >= 5)
                model_view *= Scale(1, 1.5, 1.5);
            if (relTime >= 9)
                model_view *= Scale(1, 1.5, 1.5);
            if (relTime >= 13)
            {
                model_view *= Translate(0, 0.3, -0.3);
                model_view *= Scale (1, 1.5, 1.5);
            }
            drawCube();
            model_view = s.top();
            model_view *= Translate(-0.47, 0.25, -0.165); // closer
            model_view *= RotateY(-120);
            model_view *= RotateX(-15);
            model_view *= Scale(0.05, 0.05, 0.001);
            if (relTime >= 5)
            {
                model_view *= Translate(0.3, 0.5, 0);
                model_view *= Scale(1.5, 1.5, 1);
            }
            if (relTime >= 9)
            {
                model_view *= Translate(0.3, 0.5, 0); //(z, y, x)
                model_view *= Scale(1.5, 1.5, 1);
            }
            if (relTime >= 13)
            {
                model_view *= Translate(-0.8, 0, 0);
                model_view *= Translate(0.4, 0.5, 0);
                model_view *= Scale(1.5, 1.5, 1);
            }
            drawCone();
    
            model_view = s.top();
            model_view *= Translate(-0.45, 0.2, -0.25);
            model_view *= RotateZ(-20);
            model_view *= RotateY(-25);
            model_view *= RotateX(45);
            model_view *= Scale(0.001, 0.1, 0.15);
            if (relTime >= 5)
            {
                model_view *= Translate(0, -0.05, 0.05); //(x, -z, y)
                model_view *= Scale(1, 1.5, 1.5);
            }
            if (relTime >= 9)
            {
                model_view *= Translate(0, -0.2, -0.1); //(x, -z, -y)
                model_view *= Scale(1, 1.5, 1.5);
            }
            if (relTime >= 13)
            {
                model_view *= Translate(0, -0.3, -0.3);
                model_view *= Translate(0, -0.2, -0.1);
                model_view *= Scale (1, 1.5, 1.5);
            }
            drawCube();
            model_view = s.top();
            model_view *= Translate(-0.4, 0.24, -0.295); // farther
            model_view *= RotateY(-120);
            model_view *= RotateX(-15);
            model_view *= Scale(0.05, 0.05, 0.001);
            if (relTime >= 5)
            {
                model_view *= Translate(-0.6, 0.3, 0);
                model_view *= Scale(1.5, 1.5, 1);
            }
            if (relTime >= 9)
            {
                model_view *= Translate(-1, 0.3, 0); //(z, y, x)
                model_view *= Scale(1.5, 1.5, 1);
            }
            if (relTime >= 13)
            {
                model_view *= Translate(-1, 0, 0);
                model_view *= Translate(-1, 0.3, 0);
                model_view *= Scale(1.5, 1.5, 1);
            }
            drawCone();
        }
        else
        {
             model_view = s.top();
             set_colour(0.01f, 0.01f, 0.01f);
             model_view *= Translate(-0.45, 0.2, 0.25);
             model_view *= RotateZ(-20);
             model_view *= RotateY(25);
             model_view *= Scale(0.01, 0.09, 0.09);
             drawSphere();
            
             model_view = s.top();
             set_colour(0.01f, 0.01f, 0.01f);
             model_view *= Translate(-0.45, 0.2, -0.25);
             model_view *= RotateZ(-20);
             model_view *= RotateY(-25);
             model_view *= Scale(0.01, 0.09, 0.09);
             drawSphere();
             
        }
    
    
    //snout
    model_view = s.top();
    set_colour(1.0, 0.45f, 0.0f);
    model_view *= Translate(-0.35, -0.2, 0);
    model_view *= Scale(0.25, 0.25, 0.25);
    model_view *= RotateY(90);
    drawCylinder();
    s.push(model_view);
    
    model_view = s.top();
    model_view *= Translate(0, 0, -1);
    model_view *= Scale(1, 1, 1);
    drawSphere();
    
    //nose
    model_view = s.top();
    set_colour(0.01f, 0.01f, 0.01f);
    model_view *= Translate(0, 0.3, -1.8);
    model_view *= Scale(0.4, 0.4, 0.4);
    drawSphere();
    
    //ear
    s.pop();
    model_view = s.top();
    set_colour(1.0, 0.45f, 0.0f);
    model_view *= Translate(0, 0.45, 0.35);
    model_view *= RotateX(120);
    model_view *= Scale(0.01, 0.17, 0.35);
    drawSphere();
    
    model_view = s.top();
    model_view *= Translate(0, 0.45, -0.35);
    model_view *= RotateX(60);
    model_view *= Scale(0.01, 0.17, 0.35);
    drawSphere();
    
    //legs
    s.pop();
    s.pop();
    model_view = s.top();
    model_view *= Translate(-1, -0.4, 0.3);
    model_view *= RotateX(90);
    model_view *= RotateY(-25);
    model_view *= Scale(0.15, 0.15, 0.4); //x, z, y
    drawCylinder();
    model_view = s.top();
    model_view *= Translate(-1, -0.8, 0.3);
    model_view *= RotateY(90);
    model_view *= Scale(0.13, 0.13, 0.1);
    drawCylinder();
    model_view = s.top();
    model_view *= Translate(-1.17, -0.78, 0.3);
    model_view *= Scale (0.15, 0.15, 0.15);
    drawSphere();
    model_view = s.top();
    model_view *= Translate (-0.9, -0.79, 0.3);
    model_view *= Scale (0.13, 0.13, 0.13);
    drawSphere();
    
    model_view = s.top();
    model_view *= Translate(-1, -0.4, -0.3);
    model_view *= RotateX(90);
    model_view *= RotateY(-25);
    model_view *= Scale(0.15, 0.15, 0.4); //x, z, y
    drawCylinder();
    model_view = s.top();
    model_view *= Translate(-1, -0.8, -0.3);
    model_view *= RotateY(90);
    model_view *= Scale(0.13, 0.13, 0.1);
    drawCylinder();
    model_view = s.top();
    model_view *= Translate(-1.17, -0.78, -0.3);
    model_view *= Scale (0.15, 0.15, 0.15);
    drawSphere();
    model_view = s.top();
    model_view *= Translate (-0.9, -0.79, -0.3);
    model_view *= Scale (0.13, 0.13, 0.13);
    drawSphere();
    
    model_view = s.top();
    model_view *= Translate(1, -0.4, 0.3);
    model_view *= RotateX(90);
    model_view *= RotateY(-25);
    model_view *= Scale(0.15, 0.15, 0.4); //x, z, y
    drawCylinder();
    model_view = s.top();
    model_view *= Translate(1, -0.8, 0.3);
    model_view *= RotateY(90);
    model_view *= Scale(0.13, 0.13, 0.1);
    drawCylinder();
    model_view = s.top();
    model_view *= Translate(0.85, -0.78, 0.3);
    model_view *= Scale (0.15, 0.15, 0.15);
    drawSphere();
    model_view = s.top();
    model_view *= Translate (1.1, -0.79, 0.3);
    model_view *= Scale (0.13, 0.13, 0.13);
    drawSphere();
    
    model_view = s.top();
    model_view *= Translate(1, -0.4, -0.3);
    model_view *= RotateX(90);
    model_view *= RotateY(-25);
    model_view *= Scale(0.15, 0.15, 0.4); //x, z, y
    drawCylinder();
    model_view = s.top();
    model_view *= Translate(1, -0.8, -0.3);
    model_view *= RotateY(90);
    model_view *= Scale(0.13, 0.13, 0.1);
    drawCylinder();
    model_view = s.top();
    model_view *= Translate(0.85, -0.78, -0.3);
    model_view *= Scale (0.15, 0.15, 0.15);
    drawSphere();
    model_view = s.top();
    model_view *= Translate (1.1, -0.79, -0.3);
    model_view *= Scale (0.13, 0.13, 0.13);
    drawSphere();
    
    //balloons
    s.pop();
    model_view = s.top();
    if (action >= FLYFORWARD)
    {
        float relTime = flySceneTime -10;
        model_view *= Translate(-8*relTime, 0, 0);
    }
    model_view *= Translate(0, 1*sin(2*TIME), 0);
    model_view *= Translate(-0.3, 8, 0);
    s.push(model_view);
    balloon();
}

/*
 set_colour(0.01f, 0.01f, 0.01f);
 model_view *= Scale(0.1, 10, 1);
 drawCube();
 */

void display(void)
{
    // Clear the screen with the background colour (set in myinit)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    model_view = mat4(1.0f);
    
    model_view *= Translate(0.0f, 0.0f, -15.0f);
    HMatrix r;
    Ball_Value(Arcball,r);
    
    mat4 mat_arcball_rot(
                         r[0][0], r[0][1], r[0][2], r[0][3],
                         r[1][0], r[1][1], r[1][2], r[1][3],
                         r[2][0], r[2][1], r[2][2], r[2][3],
                         r[3][0], r[3][1], r[3][2], r[3][3]);
    model_view *= mat_arcball_rot;
    model_view *= Scale(Zoom);
    glUniformMatrix4fv( uView, 1, GL_TRUE, model_view );
    
    //drawExplosion();
    /*
    drawGround();
    drawCorgi(NOTHING);
    flyingCorgi(HEADTURN);
    */
    
    flySceneTime = TIME - flyBeginTime;
   
    if(flySceneTime > 0 && flySceneTime < timeToFly)
    {
        if (flySceneTime < (timeToFly-32)) // zoom in on flying corgi //95
        {
            //eye = Translate(0, 0, 150) * flyingdog;
            eye = Translate(0, 0, -1.5*TIME) * flyingdog;
            model_view = Angel::LookAt(eye, ref, up);//just the view matrix;
            drawGround();
            drawCorgi(NOTHING);
            flyingCorgi(NOTHING);
            
        }
        else if(flySceneTime >= (timeToFly-32) && flySceneTime < (timeToFly-27))  // 360 lookaround at corgi //95 - 90
        {
            eye = Translate(0, 0, -7.5) * flyingdog;
            eye = RotateY(-360 / 5 * (flySceneTime-5)) * eye;
            model_view = Angel::LookAt(eye, ref, up);
            drawGround();
            drawCorgi(NOTHING);
            flyingCorgi(NOTHING);
        }
        else if(flySceneTime >= (timeToFly-27) && flySceneTime < timeToFly)  // corgi flies for rest of time  //90
        {
            // relTime values = seconds after start of animation that something will happen
            float relTime = flySceneTime -10;
            float relTime2 = flySceneTime - 17;
            float relTime3 = flySceneTime - 20;
            float rel_Time3 = flySceneTime - 22;
            float relTime4 = flySceneTime - 24;
            float relTime5 = flySceneTime - 28;
            float relTime6 = flySceneTime - 33;
            float relTime7 = flySceneTime - 35;
            
            if (flySceneTime >= (timeToFly-27) && flySceneTime < (timeToFly-25))  // eye moves slightly forward  //90-88
            {
                eye = Translate(-12*relTime, 0, -7.5) * flyingdog;
                ref= Translate(-8*relTime,0,0)*baseRef;
            }
            else if(flySceneTime >= (timeToFly-25) && timeToFly)  // flying the whole time //88
            {
                eye = Translate(-8, 0, 0) * flyingdog;
                eye = Translate(-8*relTime, 0, -7.5) * eye;
                ref= Translate(-8*relTime,0,0)*baseRef;
                if (flySceneTime >= (timeToFly-20) && flySceneTime < (timeToFly-17)) // corgi turns head and camera spans out  //83-80
                {
                    eye = Translate(2.5*relTime2, -3*relTime2, 5*relTime2) * eye;
                    ref= Translate(-8*relTime,-6*relTime2,0)*baseRef;
                }
                else if (flySceneTime >= (timeToFly-17) && flySceneTime < (timeToFly-15)) // close up of female corgi's face //80-78
                {
                    eye = Angel::vec4(-85+relTime3*-8, -7, 20, 1.0);
                    ref = Angel::vec4(-75+relTime3*-8, -10, 20, 1.0);
                }
                else if(flySceneTime >= (timeToFly-15) && flySceneTime < (timeToFly-13)) // zoom in on flying corgi  //78-76
                {
                    eye = Angel::vec4(-99+rel_Time3*-8, 0.5, 7, 1.0);
                    ref = Angel::vec4(-97.7+rel_Time3*-8, 0.5, 0, 1.0);
                }
                else if(flySceneTime >= (timeToFly-13) && flySceneTime < (timeToFly - 11)) // close up on female corgi's body  //76-74
                {
                    eye = Angel::vec4(-114+relTime4*-7, -8.5, 24, 1.0);
                    ref = Angel::vec4(-114+relTime4*-7, -8.5, 20, 1.0);
                }
                else if (flySceneTime >= (timeToFly-11) && flySceneTime < (timeToFly - 9))  // zoom in on flying corgi  //74-72
                {
                    eye = Angel::vec4(-99+rel_Time3*-8, 0.5, 4, 1.0);
                    ref = Angel::vec4(-97.7+rel_Time3*-8, 0.5, 0, 1.0);
                }
                else if(flySceneTime >= (timeToFly - 9) && flySceneTime < (timeToFly - 7))  // close up on female corgi's butt  //72-70
                {
                    eye = Angel::vec4(-140+relTime5*-8, -8, 20, 1.0);
                    ref = Angel::vec4(-155+relTime5*-8, -11, 20, 1.0);
                }
                else if (flySceneTime >= (timeToFly - 7) && flySceneTime < (timeToFly - 5))  // zoom in on flying corgi  //70-68
                {
                    eye = Angel::vec4(-98+rel_Time3*-8, 0.5, 2, 1.0);
                    ref = Angel::vec4(-97.7+rel_Time3*-8, 0.5, 0, 1.0);
                }
                else if (flySceneTime >= (timeToFly - 5) && flySceneTime < (timeToFly-2))  // span out to show both corgis  //68-65
                {
                    eye = Angel::vec4(-98+rel_Time3*-8, -1*relTime6, 35, 1.0);
                    ref = Angel::vec4(-98+rel_Time3*-8, -5, 10, 1.0);
                }
                else if (flySceneTime >= (timeToFly-2) && flySceneTime) // zoom in on flying corgi  // 65
                {
                    eye = Angel::vec4(-97+rel_Time3*-8, 2, 20, 1.0);
                    ref = Angel::vec4(-97+rel_Time3*-8, 2, 0, 1.0);
                }
            }
            model_view = Angel::LookAt(eye, ref, up);
            drawGround();
            drawCorgi(WALK);
            flyingCorgi(HEADTURN);  // head turns and eyes become hearts
            
        }
    }
    
    crashSceneTime = TIME - crashBeginTime;
    if(crashSceneTime >= 0 && crashSceneTime < timeToCrash)
    {
        drawExplosion();
        eye = Angel::vec4(0, 0, -10, 1.0);
        ref = Angel::vec4(0, 0, 0, 1.0);
    }
    
    double PREV_TIME = 0;
    int frameNumber = 0;
    double average = 0;
    if ( TIME != PREV_TIME) {                   // ensure you don't divide by 0
        
        double newMeasurement = 1.0 / (TIME - PREV_TIME);       // frames per second
        
        // average = (average * frameNumber + newMeasurement) / (frameNumber + 1);
        
        const float decay_period = 10;
        const float a = 2. / (decay_period - 1.);
        average = a * newMeasurement + (1 - a) * average;
        
        if (frameNumber++ % 10 == 0)
            std::cout << average << std::endl;
        
        PREV_TIME = TIME;
    }

    glutSwapBuffers();
    if(Recording == 1)
        FrSaver.DumpPPM(Width, Height) ;
}

/**********************************************
    PROC: myReshape()
    DOES: handles the window being resized 
    
      -- don't change
**********************************************************/

void myReshape(int w, int h)
{
    Width = w;
    Height = h;

    glViewport(0, 0, w, h);

    mat4 projection = Perspective(50.0f, (float)w/(float)h, 1.0f, 1000.0f);
    glUniformMatrix4fv( uProjection, 1, GL_TRUE, projection );
}

void instructions() 
{
    printf("Press:\n");
    printf("  s to save the image\n");
    printf("  r to restore the original view.\n") ;
    printf("  0 to set it to the zero state.\n") ;
    printf("  a to toggle the animation.\n") ;
    printf("  m to toggle frame dumping.\n") ;
    printf("  q to quit.\n");
}

// start or end interaction
void myMouseCB(int button, int state, int x, int y)
{
    Button = button ;
    if( Button == GLUT_LEFT_BUTTON && state == GLUT_DOWN )
    {
        HVect arcball_coords;
        arcball_coords.x = 2.0*(float)x/(float)Width-1.0;
        arcball_coords.y = -2.0*(float)y/(float)Height+1.0;
        Ball_Mouse(Arcball, arcball_coords) ;
        Ball_Update(Arcball);
        Ball_BeginDrag(Arcball);

    }
    if( Button == GLUT_LEFT_BUTTON && state == GLUT_UP )
    {
        Ball_EndDrag(Arcball);
        Button = -1 ;
    }
    if( Button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN )
    {
        PrevY = y ;
    }


    // Tell the system to redraw the window
    glutPostRedisplay() ;
}

// interaction (mouse motion)
void myMotionCB(int x, int y)
{
    if( Button == GLUT_LEFT_BUTTON )
    {
        HVect arcball_coords;
        arcball_coords.x = 2.0*(float)x/(float)Width - 1.0 ;
        arcball_coords.y = -2.0*(float)y/(float)Height + 1.0 ;
        Ball_Mouse(Arcball,arcball_coords);
        Ball_Update(Arcball);
        glutPostRedisplay() ;
    }
    else if( Button == GLUT_RIGHT_BUTTON )
    {
        if( y - PrevY > 0 )
            Zoom  = Zoom * 1.03 ;
        else 
            Zoom  = Zoom * 0.97 ;
        PrevY = y ;
        glutPostRedisplay() ;
    }
}


void idleCB(void)
{
    if( Animate == 1 )
    {
        // TM.Reset() ; // commenting out this will make the time run from 0
        // leaving 'Time' counts the time interval between successive calls to idleCB
        if( Recording == 0 )
            TIME = TM.GetElapsedTime() ;
        else
            TIME += 0.033 ; // save at 30 frames per second.
        
        //eye.x = 20*sin(TIME);
        //eye.z = 20*cos(TIME);
        
        printf("TIME %f\n", TIME) ;
        glutPostRedisplay() ; 
    }
}
/*********************************************************
     PROC: main()
     DOES: calls initialization, then hands over control
           to the event handler, which calls 
           display() whenever the screen needs to be redrawn
**********************************************************/

int main(int argc, char** argv) 
{
    glutInit(&argc, argv);
    // If your code fails to run, uncommenting these lines may help.
    //glutInitContextVersion(3, 2);
    //glutInitContextProfile(GLUT_CORE_PROFILE);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowPosition (0, 0);
    glutInitWindowSize(Width,Height);
    glutCreateWindow(argv[0]);
    printf("GL version %s\n", glGetString(GL_VERSION));
    glewExperimental = GL_TRUE;
    glewInit();
    
    myinit();

    glutIdleFunc(idleCB) ;
    glutReshapeFunc (myReshape);
    glutKeyboardFunc( myKey );
    glutMouseFunc(myMouseCB) ;
    glutMotionFunc(myMotionCB) ;
    instructions();

    glutDisplayFunc(display);
    glutMainLoop();

    TM.Reset() ;
    return 0;         // never reached
}




