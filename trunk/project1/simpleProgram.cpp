// SimpleProgram.cpp
//
// A simple 2D OpenGL program

#define GL3_PROTOTYPES

// COMMENT OR UNCOMMENT OUT THE LINE BELOW DEPENDING ON YOUR PLATFORM
#define USE_WINDOWS

// Include the vector and matrix utilities from the textbook, as well as some
// macro definitions.
#include "../include/Angel.h"

#ifdef __APPLE__
#  include <OpenGL/gl3.h>
#endif

#include <stdio.h>

// A global constant for the number of points that will be in our object.
const int NumPoints = 6;

using namespace std;

//----------------------------------------------------------------------------
// the HSV color model will be as follows
// h : [0 - 360]
// s : [0 - 1]
// v : [0 - 1]
// If you want it differently (in a 2 * pi scale, 256 instead of 1, etc,
// you'll have to change it yourself.
// rgb is returned in 0-1 scale (ready for color3f)
void HSVtoRGB(float hsv[3], float rgb[3]) {
	float tmp1 = hsv[2] * (1-hsv[1]);
	float tmp2 = hsv[2] * (1-hsv[1] * (hsv[0] / 60.0f - (int) (hsv[0]/60.0f) ));
	float tmp3 = hsv[2] * (1-hsv[1] * (1 - (hsv[0] / 60.0f - (int) (hsv[0]/60.0f) )));
	switch((int)(hsv[0] / 60)) {
		case 0:
			rgb[0] = hsv[2] ;
			rgb[1] = tmp3 ;
			rgb[2] = tmp1 ;
			break;
		case 1:
			rgb[0] = tmp2 ;
			rgb[1] = hsv[2] ;
			rgb[2] = tmp1 ;
			break;
		case 2:
			rgb[0] = tmp1 ;
			rgb[1] = hsv[2] ;
			rgb[2] = tmp3 ;
			break;
		case 3:
			rgb[0] = tmp1 ;
			rgb[1] = tmp2 ;
			rgb[2] = hsv[2] ;
			break;
		case 4:
			rgb[0] = tmp3 ;
			rgb[1] = tmp1 ;
			rgb[2] = hsv[2] ;
			break;
		case 5:
			rgb[0] = hsv[2] ;
			rgb[1] = tmp1 ;
			rgb[2] = tmp2 ;
			break;
		default:
			cout << "Inconceivable!\n";
	}
    
}


//----------------------------------------------------------------------------
void init(void)
{
    // Specifiy the vertices for a rectangle.  The first and last vertex are
    // duplicated to close the box.
    vec2 vertices[] = {
        vec2(-0.5, -0.5),
        vec2(0.5, -0.5),
        vec2(0.5, 0.5),
        vec2(-0.5, -0.5),
        vec2(0.5, 0.5),
        vec2(-0.5, 0.5)
    };
    
    // Specify some colors for the vertices
    vec3 colors[] = {
        vec3(0.0, 0.0, 1.0),
        vec3(0.0, 1.0, 0.0),
        vec3(1.0, 0.0, 0.0),
        vec3(0.0, 0.0, 1.0),
        vec3(1.0, 0.0, 0.0),
        vec3(1.0, 0.0, 1.0),
    };

    // Create a vertex array object---OpenGL needs this to manage the Vertex
    // Buffer Object
    GLuint vao[1];

    // Generate the vertex array and then bind it to make make it active.
    glGenVertexArrays(1, vao);
    glBindVertexArray(vao[0]);

    // Create and initialize a buffer object---that's the memory buffer that
    // will be on the card!
    GLuint buffer[2];

    // We only need one for this example.
    glGenBuffers(2, buffer);

    // Bind makes it the active VBO
    glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);

    // Here we copy the vertex data into our buffer on the card.  The parameters
    // tell it the type of buffer object, the size of the data in bytes, the
    // pointer for the data itself, and a hint for how we intend to use it.
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Load the shaders.  Note that this function is not offered by OpenGL
    // directly, but provided as a convenience.
#ifdef USE_WINDOWS
	GLuint program = InitShader("../SimpleProgram/src/vshader32.glsl", "../SimpleProgram/src/fshader32.glsl");
#else
	GLuint program = InitShader("vshader32.glsl", "fshader32.glsl");
#endif
	
    // Make that shader program active.
    glUseProgram(program);

    // Initialize the vertex position attribute from the vertex shader.  When
    // the shader and the program are linked, a table is created for the shader
    // variables.  Here, we get the index of the vPosition variable.
    GLuint loc = glGetAttribLocation(program, "vPosition");

    // We want to set this with an array!
    glEnableVertexAttribArray(loc);

    // We map it to this offset in our current buffer (VBO) So, our position
    // data is going into loc and contains 2 floats.  The parameters to this
    // function are the index for the shader variable, the number of components,
    // the type of the data, a boolean for whether or not this data is
    // normalized (0--1), stride (or byte offset between consective attributes),
    // and a pointer to the first component.  Note that BUFFER_OFFSET is a macro
    // defined in the Angel.h file.
    glVertexAttribPointer(loc, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    
    // Now repeat lots of that stuff for the colors
    glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
    
    GLuint colorLoc = glGetAttribLocation(program, "vColor");
    glEnableVertexAttribArray(colorLoc);
    glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    
    // Make the background white
    glClearColor(1.0, 1.0, 1.0, 1.0);
}

//----------------------------------------------------------------------------
void
display(void)
{
    // clear the window
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw the points.  The parameters to the function are: the mode, the first
    // index, and the count.
    glDrawArrays(GL_TRIANGLES, 0, NumPoints);
    glFlush();
    glutSwapBuffers();
}

//----------------------------------------------------------------------------
void
keyboard(unsigned char key, int x, int y)
{
    switch (key) {

    // Quit when ESC is pressed
    case 27:
        exit(EXIT_SUCCESS);
        break;
    }
}

//------------------------------------------------------------------------------
// This program draws a red rectangle on a white background, but it's still
// missing the machinery to move to 3D.
int main(int argc, char** argv)
{
    float rgb[] = { 0, 0, 0};
	float hsv[] = { 222, 81/100.0, 84/100.0 };
    
	HSVtoRGB(hsv,rgb);
	cout << "HSV: " << hsv[0] << " " << hsv[1] << " " << hsv[2] <<
    "  to  RGB: " << rgb[0] << " " << rgb[1] << " " << rgb[2] << endl;
    
    glutInit(&argc, argv);
#ifdef __APPLE__
    glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_RGBA | GLUT_DOUBLE);
#else
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
    glutInitContextVersion (3, 2);
    glutInitContextFlags (GLUT_FORWARD_COMPATIBLE);
#endif
    glutInitWindowSize(512, 512);
    glutInitWindowPosition(500, 300);
    glutCreateWindow("Simple Open GL Program");
    printf("%s\n%s\n", glGetString(GL_RENDERER), glGetString(GL_VERSION));

#ifndef __APPLE__
	glewExperimental = GL_TRUE;
	glewInit();
#endif

    init();

    //NOTE:  callbacks must go after window is created!!!
    glutKeyboardFunc(keyboard);
    glutDisplayFunc(display);
    glutMainLoop();

    return(0);
}
