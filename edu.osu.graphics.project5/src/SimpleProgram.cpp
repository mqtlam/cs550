// Simple 2D OpenGL Program

//Includes vec, mat, and other include files as well as macro defs
#define GL3_PROTOTYPES

// COMMENT OR UNCOMMENT LINE BELOW DEPENDING ON PLATFORM
//#define USE_WINDOWS

// Include the vector and matrix utilities from the textbook, as well as some
// macro definitions.
#ifdef USE_WINDOWS
#include "../include/Angel.h"
#include "../headers/SceneLoader.h"
#else
#include "Angel.h"
#endif
#include <stdio.h>
#include <string.h>
#ifdef __APPLE__
#  include <OpenGL/gl3.h>
#endif
#include <cv.h>
#include <highgui.h>
using namespace std;
using namespace cv;

typedef Angel::vec4 color4;
typedef Angel::vec4 point4;

int NumVertices = 0; //(6 faces)(2 triangles/face)(3 vertices/triangle)

GLuint model_view;  // model-view matrix uniform shader variable location
GLuint projection; // projection matrix uniform shader variable location

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
struct ImageChannel {
	int width;
	int height;
	int ** r;
	int ** g;
	int ** b;
	int ** gray;
};

ImageChannel imageChannel;
// OpenGL initialization
void init() {

	//
	imageChannel.width = -1;
	imageChannel.height = -1;

	// Create a vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Create and initialize a buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer( GL_ARRAY_BUFFER, buffer);
	//Must be modified
	//	glBufferData();

	// Load shaders and use the resulting shader program
	GLuint program = InitShader("shaders/vshader.glsl", "shaders/fshader.glsl");
	glUseProgram(program);

	// set up vertex arrays

	// Initialize shader lighting parameters
	// RAM: No need to change these...we'll learn about the details when we
	// cover Illumination and Shading
	point4 light_position(1.5, 0.5, 2.0, 1.0);
	color4 light_ambient(0.2, 0.2, 0.2, 1.0);
	color4 light_diffuse(1.0, 1.0, 1.0, 1.0);
	color4 light_specular(1.0, 1.0, 1.0, 1.0);

	color4 material_ambient(1.0, 0.0, 1.0, 1.0);
	color4 material_diffuse(1.0, 0.8, 0.0, 1.0);
	color4 material_specular(1.0, 0.8, 0.0, 1.0);
	float material_shininess = 100.0;

	color4 ambient_product = light_ambient * material_ambient;
	color4 diffuse_product = light_diffuse * material_diffuse;
	color4 specular_product = light_specular * material_specular;

	glUniform4fv( glGetUniformLocation(program, "AmbientProduct"), 1,
			ambient_product);
	glUniform4fv( glGetUniformLocation(program, "DiffuseProduct"), 1,
			diffuse_product);
	glUniform4fv( glGetUniformLocation(program, "SpecularProduct"), 1,
			specular_product);

	glUniform4fv( glGetUniformLocation(program, "LightPosition"), 1,
			light_position);

	glUniform1f( glGetUniformLocation(program, "Shininess"),
			material_shininess);

	model_view = glGetUniformLocation(program, "ModelView");
	projection = glGetUniformLocation(program, "Projection");
	mat4 p;

	///////// Should be modified
	point4 eye(0, 0, 0, 1.0);
	point4 at(0, 0, 0, 1.0);
	vec4 up(0, 0, 1, 0.0);

	mat4 mv = LookAt(eye, at, up);
	//vec4 v = vec4(0.0, 0.0, 1.0, 1.0);

	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);
	glUniformMatrix4fv(projection, 1, GL_TRUE, p);

	glEnable( GL_DEPTH_TEST);
	glClearColor(1.0, 1.0, 1.0, 1.0);
}

//----------------------------------------------------------------------------

void display(void) {
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDrawArrays( GL_TRIANGLES, 0, NumVertices);
	glutSwapBuffers();
}

//----------------------------------------------------------------------------

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 033:  // Escape key
	case 'q':
	case 'Q':
		exit( EXIT_SUCCESS);
		break;
	}
}

//----------------------------------------------------------------------------

/*
 *  simple.c
 *  This program draws a red rectangle on a white background.
 *
 * Still missing the machinery to move to 3D
 */

/* glut.h includes gl.h and glu.h*/

/**
 * Data Structure
 */

// This function will read the image file and generate the ImageChannel dataset
void loadImage(const char* fileName) {
	if (imageChannel.width !=-1 && imageChannel.height != -1) {
		delete imageChannel.r;
		delete imageChannel.g;
		delete imageChannel.b;
		delete imageChannel.gray;
	}
	cout << "Loading image " << fileName << "..." << endl;
	Mat image = imread(fileName);
	Mat grayImage;
	cvtColor(image, grayImage, CV_BGR2GRAY);
	imageChannel.width = image.cols;
	imageChannel.height = image.rows;
	imageChannel.r = new int*[imageChannel.height];
	imageChannel.g = new int*[imageChannel.height];
	imageChannel.b = new int*[imageChannel.height];
	imageChannel.gray = new int*[imageChannel.height];
	for (int y = 0; y < image.rows; y += 1) {
		imageChannel.r[y] = new int[imageChannel.width];
		imageChannel.g[y] = new int[imageChannel.width];
		imageChannel.b[y] = new int[imageChannel.width];
		imageChannel.gray[y] = new int[imageChannel.width];
		for (int x = 0; x < image.cols; x += 1) {
			const Vec3b pixelColor = image.at<Vec3b>(y, x);
			imageChannel.b[y][x] = (short) pixelColor[0];
			imageChannel.g[y][x] = (short) pixelColor[1];
			imageChannel.r[y][x] = (short) pixelColor[2];
			imageChannel.gray[y][x] = image.at<uchar>(y, x);
		}
	}
	cout << "Load successfully." << endl;
}
enum MENU_ITEMS {
	load, rChannel, gChannel, bChannel, grayChannel
};
// Assign a default value
// Menu handling function declaration
void menu(int menuItem) {
	string fileName;
	switch (menuItem) {
	case load:
		while (fileName.empty()) {
			cout << "Input image file name: ";
			cin >> fileName;
		}
		loadImage(fileName.c_str());
		break;
	case rChannel:

		break;
	case gChannel:

		break;
	case bChannel:

		break;
	case grayChannel:

		break;
	}
}

// Create main menu and submenus
void createMenues() {
// Create menue
	glutCreateMenu(menu);

// Add menu items
	glutAddMenuEntry("Load Image", load);
	glutAddMenuEntry("Switch RGB", rChannel);
	glutAddMenuEntry("Switch RGB", gChannel);
	glutAddMenuEntry("Switch RGB", bChannel);
	glutAddMenuEntry("Switch RGB", grayChannel);
// Associate a mouse button with menu
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
#ifdef __APPLE__
	glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_RGBA | GLUT_DEPTH);
#else
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitContextVersion(3, 2);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
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
	createMenues();
//NOTE:  callbacks must go after window is created!!!
	glutKeyboardFunc(keyboard);
	glutDisplayFunc(display);
	glutMainLoop();

	return (0);
}
