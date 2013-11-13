// Simple 2D OpenGL Program

//Includes vec, mat, and other include files as well as macro defs
#define GL3_PROTOTYPES
#define PI 3.14159265358979323846
#define PI_DIV_180 0.017453292519943296

#define deg PI_DIV_180

// COMMENT OR UNCOMMENT LINE BELOW DEPENDING ON PLATFORM
//#define USE_WINDOWS

// Include the vector and matrix utilities from the textbook, as well as some
// macro definitions.
#ifdef USE_WINDOWS
#include "../include/Angel.h"
#include "../headers/SceneLoader.h"
#else
#include "Angel.h"
#include "SceneLoader.h"
#endif
#include <stdio.h>
#include <string.h>
#include <math.h>
#ifdef __APPLE__
#  include <OpenGL/gl3.h>
#endif
using namespace graphics;

typedef Angel::vec4 color4;
typedef Angel::vec4 point4;

//----------------------------------------------------------------------------

//GLuint model_view;  // model-view matrix uniform shader variable location
GLuint model; // model matrix uniform shader variable location
GLuint view;  // view matrix uniform shader variable location
GLuint projection; // projection matrix uniform shader variable location

//----------------------------------------------------------------------------

// Lighting
point4 light_position;

//---------------------------Camera Stuff------------------------------

GLdouble prot = 45;
GLdouble trot = 45;
double r = 3;

//---------------------------------------------------------
// define projection types
enum ProjectionType {
	perspective, orthonormal
};

ProjectionType tranformType = perspective;

//----------------------------------------------------------------------------

// define all the transformation modes
enum ObjectTransformMode {
	OBJECT_TRANSLATE, OBJECT_ROTATE, OBJECT_SCALE, OBJECT_NONE
};

enum SceneTransformMode {
	SCENE_ROTATE, SCENE_TRANSLATE, SCENE_DOLLY, SCENE_NONE
};

// global variable to store current transform modes
ObjectTransformMode currentModelTransformMode = OBJECT_NONE;
SceneTransformMode currentViewTransformMode = SCENE_NONE;

//----------------------------------------------------------------------------

// declarations and default camera/projection parameters
float fx = 2;
float fy = 2;
float fz = 2;
float ax = 0;
float ay = 0;
float az = 0;
float ux = 0;
float uy = 1;
float uz = 0;
float aspect = 1.0;
float fovy, zNear, zFar;
float left_, right_, bottom_, top_;

//----------------------------------------------------------------------------

// declarations for object selection functionality
// mostly borrowed from selection demo from class

// struct for holding an instance of an object
struct ObjectInstance {
	int vaoID;
	int numVertices;

	// selection stuff
	GLubyte selectionR;
	GLubyte selectionG;
	GLubyte selectionB;
	GLubyte selectionA;

	// points and normals
	point4* points;
	vec4* normals;

	// transforms
	mat4 translate;
	mat4 rotate;
	mat4 scale;
};

//An array of the objects that holds their vao ids
//The index itself will represent the selection ID
vector<ObjectInstance> myObjects;

// counter to increment for each loaded object
int idcount = 200; // initial color
int counter = 0; // initial counter

//selection state variables
GLuint selectionColorR, selectionColorG, selectionColorB, selectionColorA;
int picked = -1;
GLint flag = 0;
GLuint SelectFlagLoc;
GLuint SelectColorRLoc, SelectColorGLoc, SelectColorBLoc, SelectColorALoc;

//----------------------------------------------------------------------------
// other state information

// shader programs
GLuint mainShaderProgram;

// mouse state information
bool isMousePressed = false;
int prevMouseX, prevMouseY;

// keeps track of the currently selected axis
int selectedAxis = -1; // -1 = none, 0 = x-axis, 1 = y-axis, 2 = z-axis

//----------------------------------------------------------------------------

// defines world plane
class WorldPlane {
public:
	static const int DIVISIONS = 8;

	point4* points;
	vec4* normals;

	int vaoID;
	int numVertices;

	void init() {
		numVertices = DIVISIONS * DIVISIONS * 8; // grid * 8 vertices for a square
		points = new point4[numVertices];
		normals = new vec4[numVertices];
		makeGrid(points, normals);

		// Create a vertex array object
		GLuint vao;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		vaoID = vao;

		// Create and initialize a buffer object
		GLuint buffer;
		glGenBuffers(1, &buffer);
		glBindBuffer( GL_ARRAY_BUFFER, buffer);
		int pointsSize = numVertices * sizeof(point4);
		int normalzSize = numVertices * sizeof(vec4);
		glBufferData( GL_ARRAY_BUFFER, pointsSize + normalzSize,
		NULL, GL_STATIC_DRAW);
		glBufferSubData( GL_ARRAY_BUFFER, 0, pointsSize, points);
		glBufferSubData( GL_ARRAY_BUFFER, pointsSize, normalzSize, normals);

		// set up vertex arrays
		GLuint vPosition = glGetAttribLocation(mainShaderProgram, "vPosition");
		glEnableVertexAttribArray(vPosition);
		glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
				BUFFER_OFFSET(0));

		GLuint vNormal = glGetAttribLocation(mainShaderProgram, "vNormal");
		glEnableVertexAttribArray(vNormal);
		glVertexAttribPointer(vNormal, 4, GL_FLOAT, GL_FALSE, 0,
				BUFFER_OFFSET(pointsSize));
	}

	void makeGrid(point4* points, vec4* normals) {
		const float INCREMENT = 2.0 / DIVISIONS;
		int index = 0;
		for (float x = -1; x < 1; x += INCREMENT) {
			for (float z = -1; z < 1; z += INCREMENT) {
				points[index] = point4(x, 0, z, 1);
				normals[index] = vec4(0, 1, 0, 0);
				index++;
				points[index] = point4(x + INCREMENT, 0, z, 1);
				normals[index] = vec4(0, 1, 0, 0);
				index++;
				points[index] = point4(x, 0, z, 1);
				normals[index] = vec4(0, 1, 0, 0);
				index++;
				points[index] = point4(x, 0, z + INCREMENT, 1);
				normals[index] = vec4(0, 1, 0, 0);
				index++;
				points[index] = point4(x + INCREMENT, 0, z, 1);
				normals[index] = vec4(0, 1, 0, 0);
				index++;
				points[index] = point4(x + INCREMENT, 0, z + INCREMENT, 1);
				normals[index] = vec4(0, 1, 0, 0);
				index++;
				points[index] = point4(x, 0, z + INCREMENT, 1);
				normals[index] = vec4(0, 1, 0, 0);
				index++;
				points[index] = point4(x + INCREMENT, 0, z + INCREMENT, 1);
				normals[index] = vec4(0, 1, 0, 0);
				index++;
			}
		}
	}
};

// global variable to keep track of single world plane
WorldPlane worldPlane = WorldPlane();

//----------------------------------------------------------------------------

struct Manipulator {
	static point4 cube[8];

	// axes
	static point4 xAxis[2];
	static point4 yAxis[2];
	static point4 zAxis[2];

	// tips
	point4* xTip;
	point4* yTip;
	point4* zTip;

	int xVaoID;
	int xNumVertices;

	int yVaoID;
	int yNumVertices;

	int zVaoID;
	int zNumVertices;

	// selection stuff
	GLubyte xSelectionR;
	GLubyte xSelectionG;
	GLubyte xSelectionB;
	GLubyte xSelectionA;
	GLubyte ySelectionR;
	GLubyte ySelectionG;
	GLubyte ySelectionB;
	GLubyte ySelectionA;
	GLubyte zSelectionR;
	GLubyte zSelectionG;
	GLubyte zSelectionB;
	GLubyte zSelectionA;

	void init() {
		xSelectionR = 0;
		xSelectionG = 200;
		xSelectionB = 0;
		xSelectionA = 255;
		ySelectionR = 0;
		ySelectionG = 201;
		ySelectionB = 0;
		ySelectionA = 255;
		zSelectionR = 0;
		zSelectionG = 202;
		zSelectionB = 0;
		zSelectionA = 255;

		// --- X-AXIS
		xTip = new point4[36];
		colorcube(xTip, Translate(1, 0, 0));

		// Create a vertex array object
		GLuint xVao;
		glGenVertexArrays(1, &xVao);
		glBindVertexArray(xVao);

		xVaoID = xVao;
		xNumVertices = 2 + 36;

		// Create and initialize a buffer object
		GLuint xBuffer;
		glGenBuffers(1, &xBuffer);
		glBindBuffer( GL_ARRAY_BUFFER, xBuffer);
		glBufferData( GL_ARRAY_BUFFER, xNumVertices * sizeof(point4), NULL,
		GL_STATIC_DRAW);
		glBufferSubData( GL_ARRAY_BUFFER, 0, 2 * sizeof(point4), xAxis);
		glBufferSubData( GL_ARRAY_BUFFER, 2 * sizeof(point4),
				36 * sizeof(point4), xTip);

		// set up vertex arrays
		GLuint vPosition = glGetAttribLocation(mainShaderProgram, "vPosition");
		glEnableVertexAttribArray(vPosition);
		glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
				BUFFER_OFFSET(0));

		// --- Y-AXIS
		yTip = new point4[36];
		colorcube(yTip, Translate(0, 1, 0));

		// Create a vertex array object
		GLuint yVao;
		glGenVertexArrays(1, &yVao);
		glBindVertexArray(yVao);

		yVaoID = yVao;
		yNumVertices = 2 + 36;

		// Create and initialize a buffer object
		GLuint yBuffer;
		glGenBuffers(1, &yBuffer);
		glBindBuffer( GL_ARRAY_BUFFER, yBuffer);
		glBufferData( GL_ARRAY_BUFFER, yNumVertices * sizeof(point4), NULL,
		GL_STATIC_DRAW);
		glBufferSubData( GL_ARRAY_BUFFER, 0, 2 * sizeof(point4), yAxis);
		glBufferSubData( GL_ARRAY_BUFFER, 2 * sizeof(point4),
				36 * sizeof(point4), yTip);

		// set up vertex arrays
		vPosition = glGetAttribLocation(mainShaderProgram, "vPosition");
		glEnableVertexAttribArray(vPosition);
		glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
				BUFFER_OFFSET(0));

		// --- Z-AXIS
		zTip = new point4[36];
		colorcube(zTip, Translate(0, 0, 1));

		// Create a vertex array object
		GLuint zVao;
		glGenVertexArrays(1, &zVao);
		glBindVertexArray(zVao);

		zVaoID = zVao;
		zNumVertices = 2 + 36;

		// Create and initialize a buffer object
		GLuint zBuffer;
		glGenBuffers(1, &zBuffer);
		glBindBuffer( GL_ARRAY_BUFFER, zBuffer);
		glBufferData( GL_ARRAY_BUFFER, zNumVertices * sizeof(point4), NULL,
		GL_STATIC_DRAW);
		glBufferSubData( GL_ARRAY_BUFFER, 0, 2 * sizeof(point4), zAxis);
		glBufferSubData( GL_ARRAY_BUFFER, 2 * sizeof(point4),
				36 * sizeof(point4), zTip);

		// set up vertex arrays
		vPosition = glGetAttribLocation(mainShaderProgram, "vPosition");
		glEnableVertexAttribArray(vPosition);
		glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
				BUFFER_OFFSET(0));
	}

	void colorcube(point4* points, mat4 translate) {
		int index = 0;
		quad(points, index, translate, 4, 5, 6, 7);
		quad(points, index, translate, 5, 4, 0, 1);
		quad(points, index, translate, 1, 0, 3, 2);
		quad(points, index, translate, 2, 3, 7, 6);
		quad(points, index, translate, 3, 0, 4, 7);
		quad(points, index, translate, 6, 5, 1, 2);
	}

	void quad(point4* points, int& index, mat4 translate, int a, int b, int c,
			int d) {
		points[index] = translate * cube[a];
		index++;
		points[index] = translate * cube[b];
		index++;
		points[index] = translate * cube[c];
		index++;
		points[index] = translate * cube[a];
		index++;
		points[index] = translate * cube[c];
		index++;
		points[index] = translate * cube[d];
		index++;
	}
};

point4 Manipulator::xAxis[2] = { point4(0, 0, 0, 1), point4(1, 0, 0, 1) };
point4 Manipulator::yAxis[2] = { point4(0, 0, 0, 1), point4(0, 1, 0, 1) };
point4 Manipulator::zAxis[2] = { point4(0, 0, 0, 1), point4(0, 0, 1, 1) };

point4 Manipulator::cube[8] = { point4(-0.05, -0.05, 0.05, 1.0), point4(-0.05,
		0.05, 0.05, 1.0), point4(0.05, 0.05, 0.05, 1.0), point4(0.05, -0.05,
		0.05, 1.0), point4(-0.05, -0.05, -0.05, 1.0), point4(-0.05, 0.05, -0.05,
		1.0), point4(0.05, 0.05, -0.05, 1.0), point4(0.05, -0.05, -0.05, 1.0) };

// global variable to keep track of single world plane
Manipulator manipulator = Manipulator();

// location of flag for manipulator color
GLuint manipulatorFlagLoc;

//----------------------------------------------------------------------------

// helper function to load vertices for each face
int Index = 0;
void myFunc(Object object, Face face, struct ObjectInstance& curObject) {
	for (int i = 0; i < 3; i++) {
		Vertex vertex = object.vertices[face.vertexId[i]];
		Vertex vertexNormal = object.verticeNormals[face.vertexNormalId[i]];
		curObject.points[Index] = point4(vertex.x, vertex.y, vertex.z, 1);
		curObject.normals[Index] = vec4(vertexNormal.x, vertexNormal.y,
				vertexNormal.z, 0);
		Index++;
	}
}

//----------------------------------------------------------------------------

// loads an object into the scene from a file
void loadObjectFromFile(string filename) {
	// declare current object
	struct ObjectInstance curObject;

	// load object
	SceneLoader sceneLoader;
	int objId = sceneLoader.loadAndAddObject(filename);
	int NumVertices = sceneLoader.objects[objId].faces.size();
	NumVertices = NumVertices * 3;
	cout << NumVertices << endl;
	curObject.points = new point4[NumVertices];
	curObject.normals = new vec4[NumVertices];
	Index = 0;
	for (unsigned int j = 0; j < sceneLoader.objects[objId].faces.size(); j++) {
		myFunc(sceneLoader.objects[objId], sceneLoader.objects[objId].faces[j],
				curObject);
	}

	// Create a vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// fill in rest of details for object and add to list
	curObject.vaoID = vao;
	curObject.numVertices = NumVertices;
	curObject.selectionR = idcount; ///Really only using red component to store unique id!
	printf("Set red component to %d\n", curObject.selectionR);
	curObject.selectionG = 0;
	curObject.selectionB = 0;
	curObject.selectionA = 255;
	curObject.translate = Translate(0, 0, 0);
	curObject.rotate = RotateX(0);
	curObject.scale = Scale(1, 1, 1);
	myObjects.push_back(curObject);
	counter++;
	idcount++;

	// Create and initialize a buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer( GL_ARRAY_BUFFER, buffer);
	int pointsSize = NumVertices * sizeof(point4);
	int normalzSize = NumVertices * sizeof(vec4);
	glBufferData( GL_ARRAY_BUFFER, pointsSize + normalzSize,
	NULL, GL_STATIC_DRAW);
	glBufferSubData( GL_ARRAY_BUFFER, 0, pointsSize, curObject.points);
	glBufferSubData( GL_ARRAY_BUFFER, pointsSize, normalzSize,
			curObject.normals);

	// set up vertex arrays
	GLuint vPosition = glGetAttribLocation(mainShaderProgram, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
			BUFFER_OFFSET(0));

	GLuint vNormal = glGetAttribLocation(mainShaderProgram, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 4, GL_FLOAT, GL_FALSE, 0,
			BUFFER_OFFSET(pointsSize));
}

//----------------------------------------------------------------------------

// OpenGL initialization
void init() {
	// Load shaders and use the resulting shader program
	mainShaderProgram = InitShader("shaders/vshader.glsl",
			"shaders/fshader.glsl");
	glUseProgram(mainShaderProgram);

	// initialize world plane
	worldPlane.init();
	manipulator.init();

	// TODO for debugging convenience; final = remove below
	//loadObjectFromFile("data/bunnyS.obj");

	// Initialize shader lighting parameters
	// RAM: No need to change these...we'll learn about the details when we
	// cover Illumination and Shading
	//point4 light_position(1.5, 0.5, 2.0, 1.0);
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

	glUniform4fv( glGetUniformLocation(mainShaderProgram, "AmbientProduct"), 1,
			ambient_product);
	glUniform4fv( glGetUniformLocation(mainShaderProgram, "DiffuseProduct"), 1,
			diffuse_product);
	glUniform4fv( glGetUniformLocation(mainShaderProgram, "SpecularProduct"), 1,
			specular_product);

	glUniform4fv( glGetUniformLocation(mainShaderProgram, "LightPosition"), 1,
			light_position);

	glUniform1f( glGetUniformLocation(mainShaderProgram, "Shininess"),
			material_shininess);

	//Set up selection colors and a flag
	SelectColorRLoc = glGetUniformLocation(mainShaderProgram,
			"selectionColorR");
	SelectColorGLoc = glGetUniformLocation(mainShaderProgram,
			"selectionColorG");
	SelectColorBLoc = glGetUniformLocation(mainShaderProgram,
			"selectionColorB");
	SelectColorALoc = glGetUniformLocation(mainShaderProgram,
			"selectionColorA");
	glUniform1i(SelectColorRLoc, selectionColorR);
	glUniform1i(SelectColorGLoc, selectionColorG);
	glUniform1i(SelectColorBLoc, selectionColorB);
	glUniform1i(SelectColorALoc, selectionColorA);

	SelectFlagLoc = glGetUniformLocation(mainShaderProgram, "flag");
	glUniform1i(SelectFlagLoc, flag);

	manipulatorFlagLoc = glGetUniformLocation(mainShaderProgram,
			"manipulatorFlag");
	glUniform1i(manipulatorFlagLoc, 0);

	// Retrieve transformation uniform variable locations
	//model_view = glGetUniformLocation(program, "ModelView");
	model = glGetUniformLocation(mainShaderProgram, "Model");
	view = glGetUniformLocation(mainShaderProgram, "View");
	projection = glGetUniformLocation(mainShaderProgram, "Projection");

	//Set up model matrix
	mat4 m; // identity
	glUniformMatrix4fv(model, 1, GL_TRUE, m);

	//Set up the view matrix with LookAt
	point4 eye(fx, fy, fz, 1.0);
	point4 at(ax, ay, az, 1.0);
	vec4 up(ux, uy, uz, 0.0);

	mat4 v = LookAt(eye, at, up);
	glUniformMatrix4fv(view, 1, GL_TRUE, v);

	//Setup the view volume
	mat4 proj;
	if (tranformType == perspective) {
		proj = Perspective(fovy, aspect, zNear, zFar);
	} else {
		proj = Ortho(left_, right_, bottom_, top_, zNear, zFar);
	}

	glUniformMatrix4fv(projection, 1, GL_TRUE, proj);

	// z buffer stuff
	glEnable( GL_DEPTH_TEST);
	glClearColor(1.0, 1.0, 1.0, 1.0);
}

//----------------------------------------------------------------------------
void mouse(int button, int state, int x, int y) {
	// store location of latest mouse click
	if (button == GLUT_LEFT_BUTTON) {
		prevMouseX = x;
		prevMouseY = y;
	}

	//printf("Mouse button pressed at %d, %d\n", x, y);
	//0 is reserved for the background color so skip it.

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//render each object, setting the selection RGBA to the objects selection color (RGBA)
	for (int i = 0; i < counter; i++) {
		//should store numVerts with vao and possibly the index in the array of objects, instead of storing only ints as I currently am
		//which represent the vaos
		flag = 1;

		glBindVertexArray(myObjects[i].vaoID);

		selectionColorR = myObjects[i].selectionR;
		selectionColorG = myObjects[i].selectionG;
		selectionColorB = myObjects[i].selectionB;
		selectionColorA = myObjects[i].selectionA;

		//sync with shader
		glUniform1i(SelectColorRLoc, selectionColorR);
		glUniform1i(SelectColorGLoc, selectionColorG);
		glUniform1i(SelectColorBLoc, selectionColorB);
		glUniform1i(SelectColorALoc, selectionColorA);
		glUniform1i(SelectFlagLoc, flag);

		// properly transform object before drawing it
		mat4 m = myObjects[i].translate * myObjects[i].rotate
				* myObjects[i].scale;
		glUniformMatrix4fv(model, 1, GL_TRUE, m);

		//Draw the scene.  The flag will force shader to not use shading, but instead use a constant color
		glDrawArrays( GL_TRIANGLES, 0, myObjects[i].numVertices);
		glutPostRedisplay(); //MUST REMEMBER TO CALL POST REDISPLAY OR IT WONT RENDER!

	}

	// MANIPULATOR STUFF
	if (picked != -1) {
		for (int i = 0; i < 3; i++) {
			flag = 1;

			int numVertices;
			if (i == 0) // x-axis
					{
				glBindVertexArray(manipulator.xVaoID);
				selectionColorR = manipulator.xSelectionR;
				selectionColorG = manipulator.xSelectionG;
				selectionColorB = manipulator.xSelectionB;
				selectionColorA = manipulator.xSelectionA;
				numVertices = manipulator.xNumVertices;
			} else if (i == 1) // y-axis
					{
				glBindVertexArray(manipulator.yVaoID);
				selectionColorR = manipulator.ySelectionR;
				selectionColorG = manipulator.ySelectionG;
				selectionColorB = manipulator.ySelectionB;
				selectionColorA = manipulator.ySelectionA;
				numVertices = manipulator.yNumVertices;
			} else if (i == 2) // z-axis
					{
				glBindVertexArray(manipulator.zVaoID);
				selectionColorR = manipulator.zSelectionR;
				selectionColorG = manipulator.zSelectionG;
				selectionColorB = manipulator.zSelectionB;
				selectionColorA = manipulator.zSelectionA;
				numVertices = manipulator.zNumVertices;
			}

			//sync with shader
			glUniform1i(SelectColorRLoc, selectionColorR);
			glUniform1i(SelectColorGLoc, selectionColorG);
			glUniform1i(SelectColorBLoc, selectionColorB);
			glUniform1i(SelectColorALoc, selectionColorA);
			glUniform1i(SelectFlagLoc, flag);

			// properly transform object before drawing it
			mat4 m = myObjects[picked].translate;
			glUniformMatrix4fv(model, 1, GL_TRUE, m);

			//Draw the scene.  The flag will force shader to not use shading, but instead use a constant color
			glDrawArrays( GL_TRIANGLES, 0, numVertices);
			glutPostRedisplay(); //MUST REMEMBER TO CALL POST REDISPLAY OR IT WONT RENDER!
		}
	}

	// END MANIPULATOR STUFF

	//Now check the pixel location to see what color is found!
	GLubyte pixel[4];
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	//Read as unsigned byte.

	glReadPixels(x, viewport[3] - y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel);

	int prevSelectedAxis = selectedAxis;
	selectedAxis = -1;

	if (picked != -1) {
		if (manipulator.xSelectionR == ceil(pixel[0])
				&& manipulator.xSelectionG == pixel[1]
				&& manipulator.xSelectionB == pixel[2]
				&& manipulator.xSelectionA == pixel[3])
			selectedAxis = 0;
		else if (manipulator.ySelectionR == ceil(pixel[0])
				&& manipulator.ySelectionG == pixel[1]
				&& manipulator.ySelectionB == pixel[2]
				&& manipulator.ySelectionA == pixel[3])
			selectedAxis = 1;
		else if (manipulator.zSelectionR == ceil(pixel[0])
				&& manipulator.zSelectionG == pixel[1]
				&& manipulator.zSelectionB == pixel[2]
				&& manipulator.zSelectionA == pixel[3])
			selectedAxis = 2;
	}

	// if we just selected the axis or it was already selected, then keep the picked object
	if (selectedAxis == -1 && prevSelectedAxis == -1) {
		picked = -1;
		for (int i = 0; i < counter; i++) {
			if (myObjects[i].selectionR == ceil(pixel[0])
					&& myObjects[i].selectionG == pixel[1]
					&& myObjects[i].selectionB == pixel[2]
					&& myObjects[i].selectionA == pixel[3])
				picked = i;
		}
	}

	printf("Picked == %d, ", picked);
	printf("Selected Axis == %d\n", selectedAxis);
	//uncomment below to see the color render
	// Swap buffers makes the back buffer actually show...in this case, we don't want it to show so we comment out.  For debuggin, youi can uncomment it to see the render of the back buffer which will hold your 'fake color render'
	//glutSwapBuffers();
}

//----------------------------------------------------------------------------
// motion callback - calls when mouse is pressed and dragging in window

void display();

//TODO: fill below with appropriate transforms
//		also possibly need additional state information
void motion(int x, int y) {
	float adjust = 0.001;
	float xDiff = (x - prevMouseX) * adjust;
	float yDiff = -(y - prevMouseY) * adjust;
	float zDiff = -(x - prevMouseX) * adjust;

	float angle = 10;
	float scale = 1.05;
	if (selectedAxis == 0 && xDiff < 0) {
		angle = -10;
		scale = 0.9;
	} else if (selectedAxis == 1) {
		if (yDiff > 0) {
			scale = 1.1;
		} else if (yDiff < 0) {
			scale = 0.95;
		}
	} else if (selectedAxis == 2 && xDiff > 0) {
		scale = 0.9;
	}

	if (currentModelTransformMode == OBJECT_TRANSLATE) {
		switch (selectedAxis) {
		case -1:
			xDiff = 0;
			yDiff = 0;
			zDiff = 0;
			break;
		case 0:
			yDiff = 0;
			zDiff = 0;
			break;
		case 1:
			xDiff = 0;
			zDiff = 0;
			break;
		case 2:
			yDiff = 0;
			xDiff = 0;
			break;

		}
		myObjects[picked].translate = myObjects[picked].translate
				* Translate(xDiff, yDiff, zDiff);
		prevMouseX = x;
		prevMouseY = y;
	} else if (currentModelTransformMode == OBJECT_ROTATE) {
		switch (selectedAxis) {
		case -1:
			break;
		case 0:
			myObjects[picked].rotate = myObjects[picked].rotate
					* RotateX(angle);
			break;
		case 1:
			myObjects[picked].rotate = myObjects[picked].rotate
					* RotateY(angle);
			break;
		case 2:
			myObjects[picked].rotate = myObjects[picked].rotate
					* RotateZ(angle);
			break;
		}

	} else if (currentModelTransformMode == OBJECT_SCALE) {

		switch (selectedAxis) {
		case -1:
			break;
		case 0:
			myObjects[picked].scale = myObjects[picked].scale
					* Scale(scale, 1, 1);
			break;
		case 1:
			myObjects[picked].scale = myObjects[picked].scale
					* Scale(1, scale, 1);
			break;
		case 2:
			myObjects[picked].scale = myObjects[picked].scale
					* Scale(1, 1, scale);
			break;
		}

	} else if (currentViewTransformMode == SCENE_ROTATE) {
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
		fx += xDiff * 10;
		fy += yDiff * 10;
		ax += xDiff * 10;
		ay += yDiff * 10;
		prevMouseX = x;
		prevMouseY = y;
		glutPostRedisplay();
	} else if (currentViewTransformMode == SCENE_DOLLY) {
		float oldFz = fz;
		fz += zDiff;
		fx = (fz * fx) / oldFz;
		fy = (fz * fy) / oldFz;
		glutPostRedisplay();
	}

	display();
}

//----------------------------------------------------------------------------

void drawManipulator(mat4 m) {
	flag = 0;
	glUniform1i(SelectFlagLoc, flag);

	// apply model transformation (just translation from origin)
	glUniformMatrix4fv(model, 1, GL_TRUE, m);

	// draw x axis
	glBindVertexArray(manipulator.xVaoID);
	glUniform1i(manipulatorFlagLoc, 1);
	glDrawArrays(GL_LINES, 0, 2);
	glDrawArrays(GL_TRIANGLES, 2, 36);

	// draw y axis
	glBindVertexArray(manipulator.yVaoID);
	glUniform1i(manipulatorFlagLoc, 2);
	glDrawArrays(GL_LINES, 0, 2);
	glDrawArrays(GL_TRIANGLES, 2, 36);

	// draw z axis
	glBindVertexArray(manipulator.zVaoID);
	glUniform1i(manipulatorFlagLoc, 3);
	glDrawArrays(GL_LINES, 0, 2);
	glDrawArrays(GL_TRIANGLES, 2, 36);
}
bool initilized = false;
void display(void) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//  Set camera position and orientation.
	view = glGetUniformLocation(mainShaderProgram, "View");
	cout << "fx:" << fx << "fy:" << fy << "fz:" << fz << endl;
	//Set up the view matrix with LookAt
	point4 eye(fx, fy, fz, 1.0);
	point4 at(ax, ay, az, 1.0);
	vec4 up(ux, uy, uz, 0.0);

	mat4 v = LookAt(eye, at, up);
	glUniformMatrix4fv(view, 1, GL_TRUE, v);
	// draw ground plane
	glUniform1i(manipulatorFlagLoc, 0);
	flag = 0;
	glUniform1i(SelectFlagLoc, flag);
	mat4 m = mat4(); // identity
	glUniformMatrix4fv(model, 1, GL_TRUE, m);
	glBindVertexArray(worldPlane.vaoID);
	glDrawArrays( GL_LINES, 0, worldPlane.numVertices);

	// draw objects
	for (int i = 0; i < counter; i++) {
		if (i == picked) {
			// draw manipulator
			drawManipulator(myObjects[i].translate);

			// draw in wireframe mode
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glPolygonOffset(1.0, 2); //Try 1.0 and 2 for factor and units
		}

		//Render transformation
		mat4 m = myObjects[i].translate * myObjects[i].rotate
				* myObjects[i].scale;
		glUniformMatrix4fv(model, 1, GL_TRUE, m);

		// turn off manipulator flag
		glUniform1i(manipulatorFlagLoc, 0);

		//Render transformation
		glUniformMatrix4fv(model, 1, GL_TRUE, m);

		//Normal render so set selection Flag to 0
		flag = 0;
		glUniform1i(SelectFlagLoc, flag);
		glBindVertexArray(myObjects[i].vaoID);
		glDrawArrays( GL_TRIANGLES, 0, myObjects[i].numVertices);

		if (i == picked) {
			// revert back to fill mode
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}

	//comment this to see the color render
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
// menu stuff

void mainMenuCallback(int id) {
	if (id == 1) {
		// prompt user for input filename via command line
		string filename;
		while (filename.empty()) {
			cout << "Input file name: ";
			cin >> filename;
		}

		if (filename.find(".obj") == string::npos) {
			cout << "Invalid file name. Loading canceled." << endl << endl;
		} else {
			// load .obj file
			loadObjectFromFile(filename);
			cout << "Successfully loaded " << filename << "." << endl << endl;
		}
	} else if (id == 2) {
		for (int i = 0; i < counter; i++) {
			myObjects[i].translate = Translate(0, 0, 0);
			myObjects[i].rotate = RotateX(0);
			myObjects[i].scale = Scale(1, 1, 1);
		}

		picked = -1;
		selectedAxis = -1;
		display();

		cout << "All transformations reset." << endl << endl;
	} else if (id == 3) {
		exit(0);
	}
}

void objMenuCallback(int id) {
	if (id == 1) {
		cout << "Switched to OBJECT TRANSLATE mode." << endl;
		currentModelTransformMode = OBJECT_TRANSLATE;
		currentViewTransformMode = SCENE_NONE;
	} else if (id == 2) {
		cout << "Switched to OBJECT ROTATE mode." << endl;
		currentModelTransformMode = OBJECT_ROTATE;
		currentViewTransformMode = SCENE_NONE;
	} else if (id == 3) {
		cout << "Switched to OBJECT SCALE mode." << endl;
		currentModelTransformMode = OBJECT_SCALE;
		currentViewTransformMode = SCENE_NONE;
	}
}

void sceneMenuCallback(int id) {
	if (id == 1) {
		cout << "Switched to SCENE Rotate mode." << endl;
		currentViewTransformMode = SCENE_ROTATE;
		currentModelTransformMode = OBJECT_NONE;
	} else if (id == 2) {
		cout << "Switched to SCENE TRANSLATE mode." << endl;
		currentViewTransformMode = SCENE_TRANSLATE;
		currentModelTransformMode = OBJECT_NONE;
	} else if (id == 3) {
		cout << "Switched to SCENE DOLLY mode." << endl;
		currentViewTransformMode = SCENE_DOLLY;
		currentModelTransformMode = OBJECT_NONE;
	}

}

// This creates the menu/submenus
void makeMenu() {
	int objectTransformSubMenu = glutCreateMenu(objMenuCallback);
	glutAddMenuEntry("Translation", 1);
	glutAddMenuEntry("Rotation", 2);
	glutAddMenuEntry("Scale", 3);

	int sceneTransformSubMenu = glutCreateMenu(sceneMenuCallback);
	glutAddMenuEntry("Rotation", 1);
	glutAddMenuEntry("Translation", 2);
	glutAddMenuEntry("Dolly", 3);

	int menu = glutCreateMenu(mainMenuCallback);
	glutAddMenuEntry("Load .obj File", 1);
	glutAddSubMenu("Object Transformation", objectTransformSubMenu);
	glutAddSubMenu("Scene Transformation", sceneTransformSubMenu);
	glutAddMenuEntry("Reset Transformations", 2);
	glutAddMenuEntry("Exit", 3);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

//----------------------------------------------------------------------------

/*
 *  simple.c
 *  This program draws a red rectangle on a white background.
 *
 * Still missing the machinery to move to 3D
 */

/* glut.h includes gl.h and glu.h*/
int main(int argc, char** argv) {
	if (strcmp(argv[1], "P") == 0) {
		tranformType = perspective;
		fovy = atof(argv[2]);
		zNear = atof(argv[3]);
		zFar = atof(argv[4]);
		
		float light_x = atof(argv[5]);
		float light_y = atof(argv[6]);
		float light_z = atof(argv[7]);
		light_position = point4(light_x, light_y, light_z, 1.0);
	} else if (strcmp(argv[1], "O") == 0) {
		tranformType = orthonormal;
		left_ = atof(argv[2]);
		right_ = atof(argv[3]);
		bottom_ = atof(argv[4]);
		top_ = atof(argv[5]);
		zNear = atof(argv[6]);
		zFar = atof(argv[7]);

		float light_x = atof(argv[8]);
		float light_y = atof(argv[9]);
		float light_z = atof(argv[10]);
		light_position = point4(light_x, light_y, light_z, 1.0);
	} else {
		cerr
				<< "Usage: ./SimpleProgram (O left right bottom top near far)|(P fov near far)"
				<< endl;
		exit(1);
	}
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

	//NOTE:  callbacks must go after window is created!!!
	makeMenu();
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutDisplayFunc(display);
	glutMainLoop();

	return (0);
}
