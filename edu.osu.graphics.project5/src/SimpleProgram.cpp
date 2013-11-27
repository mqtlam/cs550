// Simple 2D OpenGL Program

//Includes vec, mat, and other include files as well as macro defs
#define GL3_PROTOTYPES

// COMMENT OR UNCOMMENT LINE BELOW DEPENDING ON PLATFORM
//#define USE_WINDOWS

// Include the vector and matrix utilities from the textbook, as well as some
// macro definitions.
#ifdef USE_WINDOWS
#include "../include/Angel.h"
//#include "../headers/SceneLoader.h"
#include "opencv2/opencv.hpp"
//#include <highgui.h>
#else
#include "Angel.h"
#include <cv.h>
#include <highgui.h>
#endif
#include <stdio.h>
#include <string.h>
#ifdef __APPLE__
#  include <OpenGL/gl3.h>
#endif
using namespace std;
using namespace cv;

typedef Angel::vec4 color4;
typedef Angel::vec4 point4;

//Includes vec, mat, and other include files as well as macro defs
#define GL3_PROTOTYPES
#define PI 3.14159265358979323846
#define PI_DIV_180 0.017453292519943296

#define deg PI_DIV_180

// Lighting
point4 light_position;

//---------------------------Camera Stuff------------------------------

GLdouble prot = 45;
GLdouble trot = 45;
double r = 3;

// define all the transformation modes
enum SceneTransformMode {
	SCENE_ROTATE, SCENE_TRANSLATE, SCENE_DOLLY, SCENE_NONE
};

SceneTransformMode currentViewTransformMode = SCENE_NONE;

// shader programs
GLuint mainShaderProgram;

// mouse state information
bool isMousePressed = false;
int prevMouseX, prevMouseY;

// default parameters
const point4 DEFAULT_EYE = point4(5, 5, 5, 1.0);
const point4 DEFAULT_AT = point4(0, 0, 0, 1);
const vec4 DEFAULT_UP = vec4(0, 1, 0, 0);

// declarations and default camera/projection parameters
float fx = DEFAULT_EYE.x;
float fy = DEFAULT_EYE.y;
float fz = DEFAULT_EYE.z;
float ax = DEFAULT_AT.x;
float ay = DEFAULT_AT.y;
float az = DEFAULT_AT.z;
float ux = DEFAULT_UP.x;
float uy = DEFAULT_UP.y;
float uz = DEFAULT_UP.z;
float aspect = 1.0;
float fovy, zNear, zFar;
float left_, right_, bottom_, top_;
float lightPos[3];

int NumVertices = 0; //(6 faces)(2 triangles/face)(3 vertices/triangle)

GLuint model; // model matrix uniform shader variable location
GLuint view;  // view matrix uniform shader variable location
GLuint projection; // projection matrix uniform shader variable location

GLuint depthFlagLoc;

//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
struct ImageMesh {
	int numTrianglePoints;
	vec4* vertices;
	vec3* colors;
};

// stores all image meshes
ImageMesh imageMesh;

struct ImageChannel {
	int width;
	int height;
	int ** r;
	int ** g;
	int ** b;
	int ** gray;
};

// Create a vertex array object---OpenGL needs this to manage the Vertex
// Buffer Object
GLuint vao[1];
ImageChannel imageChannel;

int depthMap = 0;
// OpenGL initialization
void init() {

	// Load shaders and use the resulting shader program
	mainShaderProgram = InitShader("shaders/vshader.glsl",
			"shaders/fshader.glsl");
	glUseProgram(mainShaderProgram);

	imageMesh.numTrianglePoints = 0;

	//
	imageChannel.width = -1;
	imageChannel.height = -1;
	view = glGetUniformLocation(mainShaderProgram, "View");
	//Set up the view matrix with LookAt
	point4 eye(fx, fy, fz, 1.0);
	point4 at(ax, ay, az, 1.0);
	vec4 up(ux, uy, uz, 0.0);

	mat4 v = LookAt(eye, at, up);
	glUniformMatrix4fv(view, 1, GL_TRUE, v);
	glEnable( GL_DEPTH_TEST);
	glClearColor(1.0, 1.0, 1.0, 1.0);
}

//----------------------------------------------------------------------------

void display(void) {
	cout << "display()" << endl;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Load the shaders.  Note that this function is not offered by OpenGL
	// directly, but provided as a convenience.
	GLuint program = InitShader("shaders/vshader32.glsl",
			"shaders/fshader32.glsl");
	glUseProgram(program);

	// Initialize the vertex position attribute from the vertex shader.  When
	// the shader and the program are linked, a table is created for the shader
	// variables.  Here, we get the index of the vPosition variable.
	glBindVertexArray(vao[0]);

	// Retrieve transformation uniform variable locations
	//model_view = glGetUniformLocation(program, "ModelView");
	model = glGetUniformLocation(program, "Model");
	view = glGetUniformLocation(program, "View");
	projection = glGetUniformLocation(program, "Projection");

	//Set up model matrix
	mat4 m; // identity
	glUniformMatrix4fv(model, 1, GL_TRUE, m);
	glUniform1i(depthFlagLoc, depthMap);
	//Set up the view matrix with LookAt
	//the image is centered at the origin and aligned with the x, y axes
	//the z values range from 0 to 1.

	cout << "fx:" << fx << "fy:" << fy << "fz:" << fz << endl;
	//Set up the view matrix with LookAt
	point4 eye(fx, fy, fz, 1.0);
	point4 at(ax, ay, az, 1.0);
	vec4 up(ux, uy, uz, 0.0);
	mat4 v = LookAt(eye, at, up);
	glUniformMatrix4fv(view, 1, GL_TRUE, v);

	//Setup the view volume
	//mat4 proj = Perspective(45, 1, 0.1, 20);
	mat4 proj = Ortho(-1, 1, -1, 1, 0.1, 20);

	glUniformMatrix4fv(projection, 1, GL_TRUE, proj);

	//Set the light
	light_position = point4(lightPos[0], lightPos[1], lightPos[2], 1.0);
	glUniform4fv( glGetUniformLocation(mainShaderProgram, "LightPosition"), 1,
			light_position);

	// z buffer stuff
	glEnable( GL_DEPTH_TEST);
	glClearColor(1.0, 1.0, 1.0, 1.0);

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDrawArrays(GL_TRIANGLES, 0, imageMesh.numTrianglePoints);
	glFlush();
	glutSwapBuffers();
	cout << "display return()" << endl;
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

void drawImage() {
	cout << "drawImage()" << endl;
	const int nCols = imageChannel.width;
	const int nRows = imageChannel.height;

	// determine the horizontal and vertical measure of each unit square to draw
	float horzUnit = 2.0 / (nCols);
	float vertUnit = 2.0 / (nRows);

	imageMesh.numTrianglePoints = 6 * nRows * nCols;
	imageMesh.vertices = new vec4[imageMesh.numTrianglePoints];
	imageMesh.colors = new vec3[imageMesh.numTrianglePoints];

	for (int row = 0; row < nRows; row++) {
		for (int col = 0; col < nCols; col++) {
			int index = 6 * (row * nCols + col);

			// make a grid square:
			// draیقw two unit triangles based on our defined units: horzUnit, vertUnit
			// first draw relative to origin, then subtract 1 in both coordinates to put at (-1, -1)

			// NOTE: depth or z-value of point cloud is determined in shader
			float depth = 0;			//1.0*imageChannel.gray[row][col]/255;

			// first triangle
			imageMesh.vertices[index] = vec4(col * horzUnit - 1,
					-row * vertUnit + 1, depth, 1);
			imageMesh.vertices[index + 1] = vec4(col * horzUnit - 1,
					-(row + 1) * vertUnit + 1, depth, 1);
			imageMesh.vertices[index + 2] = vec4((col + 1) * horzUnit - 1,
					-row * vertUnit + 1, depth, 1);
			// second triangle
			imageMesh.vertices[index + 3] = vec4((col + 1) * horzUnit - 1,
					-row * vertUnit + 1, depth, 1);
			imageMesh.vertices[index + 4] = vec4((col + 1) * horzUnit - 1,
					-(row + 1) * vertUnit + 1, depth, 1);
			imageMesh.vertices[index + 5] = vec4(col * horzUnit - 1,
					-(row + 1) * vertUnit + 1, depth, 1);

			// assign appropriate color:
			float rgb[3] = { 1.0 * imageChannel.r[row][col] / 255, 1.0
					* imageChannel.g[row][col] / 255, 1.0
					* imageChannel.b[row][col] / 255 };
			for (int i = 0; i < 6; i++)
				imageMesh.colors[index + i] = vec3(rgb[0], rgb[1], rgb[2]);
		}
	}

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
	glBufferData(GL_ARRAY_BUFFER, (imageMesh.numTrianglePoints) * sizeof(vec4),
			imageMesh.vertices,
			GL_STATIC_DRAW);

	// Load the shaders.  Note that this function is not offered by OpenGL
	// directly, but provided as a convenience.
	GLuint program = InitShader("shaders/vshader32.glsl",
			"shaders/fshader32.glsl");
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
	glVertexAttribPointer(loc, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	// Now repeat lots of that stuff for the colors
	glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
	glBufferData(GL_ARRAY_BUFFER, (imageMesh.numTrianglePoints) * sizeof(vec3),
			imageMesh.colors,
			GL_STATIC_DRAW);

	GLuint colorLoc = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(colorLoc);
	glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	depthFlagLoc = glGetUniformLocation(program, "depthFlag");
	glUniform1i(depthFlagLoc, 0);

	// Retrieve transformation uniform variable locations
	//model_view = glGetUniformLocation(program, "ModelView");
	model = glGetUniformLocation(program, "Model");
	view = glGetUniformLocation(program, "View");
	projection = glGetUniformLocation(program, "Projection");

	//Set up model matrix
	mat4 m; // identity
	glUniformMatrix4fv(model, 1, GL_TRUE, m);

	//Set up the view matrix with LookAt
	//the image is centered at the origin and aligned with the x, y axes
	//the z values range from 0 to 1.

	mat4 v = LookAt(DEFAULT_EYE, DEFAULT_AT, DEFAULT_UP);
	glUniformMatrix4fv(view, 1, GL_TRUE, v);

	//Setup the view volume
	//mat4 proj = Perspective(45, 1, 0.1, 20);
	mat4 proj = Ortho(-1, 1, -1, 1, 0.1, 20);

	glUniformMatrix4fv(projection, 1, GL_TRUE, proj);

	// z buffer stuff
	glEnable( GL_DEPTH_TEST);
	glClearColor(1.0, 1.0, 1.0, 1.0);

	cout << "Image Initialized." << endl;
}

/**
 * Data Structure
 */

// This function will read the image file and generate the ImageChannel dataset
void loadImage(const char* fileName) {
	if (imageChannel.width != -1 && imageChannel.height != -1) {
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

	drawImage();
}
enum MENU_ITEMS {
	load, rChannel, gChannel, bChannel, grayChannel, resetScene
};
// Assign a default value
// Menu handling function declaration
void menuCallBack(int menuItem) {
	cout << "menuCallBack" << endl;
	string fileName;
	switch (menuItem) {
	case load:
		while (fileName.empty()) {
			cout << "Input image file name: ";
			cin >> fileName;
		}
		loadImage(fileName.c_str());
		glutPostRedisplay();
		break;
	case rChannel:
		glUniform1i(depthFlagLoc, 1);
		depthMap = 1;
		cout << "Switched to red channel." << endl;
		glutPostRedisplay();
		break;
	case gChannel:
		glUniform1i(depthFlagLoc, 2);
		depthMap = 2;
		cout << "Switched to green channel." << endl;
		glutPostRedisplay();
		break;
	case bChannel:
		glUniform1i(depthFlagLoc, 3);
		depthMap = 3;
		cout << "Switched to blue channel." << endl;
		glutPostRedisplay();
		break;
	case grayChannel:
		glUniform1i(depthFlagLoc, 0);
		depthMap = 0;
		cout << "Switched to grayscale." << endl;
		glutPostRedisplay();
		break;
	case resetScene:
		fx = DEFAULT_EYE.x;
		fy = DEFAULT_EYE.y;
		fz = DEFAULT_EYE.z;
		ax = DEFAULT_AT.x;
		ay = DEFAULT_AT.y;
		az = DEFAULT_AT.z;
		ux = DEFAULT_UP.x;
		uy = DEFAULT_UP.y;
		uz = DEFAULT_UP.z;

		display();

		cout << "All view transformations reset." << endl << endl;
		glutPostRedisplay();

		break;
	}
}
void mouse(int button, int state, int x, int y) {
	cout << "mouse" << endl;
	// store location of latest mouse click
	if (button == GLUT_LEFT_BUTTON) {
		prevMouseX = x;
		prevMouseY = y;
	}
}

void motion(int x, int y) {
	cout << "mouse" << endl;
	float adjust = 0.001;
	float xDiff = +(x - prevMouseX) * adjust;
	float yDiff = -(y - prevMouseY) * adjust;
	float zDiff = -(x - prevMouseX) * adjust;

	if (currentViewTransformMode == SCENE_ROTATE) {
		trot += (x - prevMouseX) / 2;
		if (trot > 360)
			trot -= 360;
		if (trot < 0)
			trot += 360;
		prevMouseX = x;
		prot += (y - prevMouseY) / 2;
		if (prot < -90)
			prot = -90;
		if (prot > 90)
			prot = 90;
		prevMouseY = y;

		fx = r * cos(prot * deg) * cos(trot * deg);
		fy = r * sin(prot * deg);
		fz = r * cos(prot * deg) * sin(trot * deg);
		glutPostRedisplay();

	} else if (currentViewTransformMode == SCENE_TRANSLATE) {
		fx += xDiff;
		fy += yDiff;
		ax += xDiff;
		ay += yDiff;
		prevMouseX = x;
		prevMouseY = y;
		glutPostRedisplay();
	} else if (currentViewTransformMode == SCENE_DOLLY) {
		float oldFz = fz;
		fz += yDiff * 0.1;
		fx = (fz * fx) / oldFz;
		fy = (fz * fy) / oldFz;
		glutPostRedisplay();
	}

//	display();
}
void sceneMenuCallback(int id) {
	if (id == 1) {
		cout << "Switched to SCENE Rotate mode." << endl;
		currentViewTransformMode = SCENE_ROTATE;
		glutPostRedisplay();
	} else if (id == 2) {
		cout << "Switched to SCENE TRANSLATE mode." << endl;
		currentViewTransformMode = SCENE_TRANSLATE;
		glutPostRedisplay();
	} else if (id == 3) {
		cout << "Switched to SCENE DOLLY mode." << endl;
		currentViewTransformMode = SCENE_DOLLY;
		glutPostRedisplay();
	}

}
// Create main menu and submenus
void createMenues() {
//	Submenues
	int sceneTransformSubMenu = glutCreateMenu(sceneMenuCallback);
	glutAddMenuEntry("Rotation", 1);
	glutAddMenuEntry("Translation", 2);
	glutAddMenuEntry("Dolly", 3);

// Create menue
	glutCreateMenu(menuCallBack);
// Add menu items
	glutAddMenuEntry("Load Image", load);
	glutAddMenuEntry("Switch to Red Channel", rChannel);
	glutAddMenuEntry("Switch to Green Channel", gChannel);
	glutAddMenuEntry("Switch to Blue Channel", bChannel);
	glutAddMenuEntry("Switch to Grayscale", grayChannel);

//
	glutAddSubMenu("Scene Transformation", sceneTransformSubMenu);
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

	createMenues();
	init();
//NOTE:  callbacks must go after window is created!!!
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutDisplayFunc(display);
	glutMainLoop();

	return (0);
}
