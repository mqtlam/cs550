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
#include "SceneLoader.h"
#endif
#include <stdio.h>
#include <string.h>
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

// define projection types
enum ProjectionType {
	perspective, orthonormal
};

ProjectionType tranformType = perspective;

//----------------------------------------------------------------------------

// define all the transformation modes
enum TransformMode {
	OBJECT_TRANSLATE, OBJECT_ROTATE, OBJECT_SCALE,
	SCENE_ROTATE_X, SCENE_ROTATE_Y, SCENE_ROTATE_Z, SCENE_TRANSLATE, SCENE_DOLLY
};

// global variable to store current mode
TransformMode currentTransformMode = OBJECT_TRANSLATE;

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
GLuint program;
GLuint selectionColorR, selectionColorG, selectionColorB, selectionColorA;
int picked = -1;
GLint flag = 0;
GLuint SelectFlagLoc;
GLuint SelectColorRLoc, SelectColorGLoc, SelectColorBLoc, SelectColorALoc;

//----------------------------------------------------------------------------
// other state information

// mouse state information
bool isMousePressed = false;
int pressedMouseX, pressedMouseY;

//----------------------------------------------------------------------------

// defines world plane
class WorldPlane
{
public:
	static point4 points[6];
	static vec4 normals[6];

	int vaoID;
	int numVertices;

	void init()
	{
		// Create a vertex array object
		GLuint vao;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		vaoID = vao;
		numVertices = 6;

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
		GLuint vPosition = glGetAttribLocation(program, "vPosition");
		glEnableVertexAttribArray(vPosition);
		glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
				BUFFER_OFFSET(0));

		GLuint vNormal = glGetAttribLocation(program, "vNormal");
		glEnableVertexAttribArray(vNormal);
		glVertexAttribPointer(vNormal, 4, GL_FLOAT, GL_FALSE, 0,
				BUFFER_OFFSET(pointsSize));
	}
};

point4 WorldPlane::points[6] = { point4(-1,0,-1,1), point4(1,0,1,1),
	point4(1,0,-1,1), point4(-1,0,1,1), point4(1,0,1,1), point4(-1,0,-1,1)};
vec4 WorldPlane::normals[6] = { vec4(0,1,0,0), vec4(0,1,0,0),
	vec4(0,1,0,0), vec4(0,1,0,0), vec4(0,1,0,0), vec4(0,1,0,0)};

// global variable to keep track of single world plane
WorldPlane worldPlane = WorldPlane();

//----------------------------------------------------------------------------

// helper function to load vertices for each face
int Index = 0;
void myFunc(Object object, Face face, struct ObjectInstance& curObject) {
	for (int i = 0; i < 3; i++) {
		Vertex vertex = object.vertices[face.vertexId[i]];
		Vertex vertexNormal = object.verticeNormals[face.vertexNormalId[i]];
		curObject.points[Index] = point4(vertex.x, vertex.y, vertex.z, 1);
		curObject.normals[Index] = vec4(vertexNormal.x, vertexNormal.y, vertexNormal.z,
				0);
		Index++;
	}
}

//----------------------------------------------------------------------------

// loads an object into the scene from a file
void loadObjectFromFile(string filename)
{
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
		myFunc(sceneLoader.objects[objId], sceneLoader.objects[objId].faces[j], curObject);
	}

	// Create a vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// fill in rest of details for object and add to list
	curObject.vaoID = vao;
	curObject.numVertices = NumVertices;
	curObject.selectionR = idcount;  ///Really only using red component to store unique id!
	printf("Set red component to %d\n", curObject.selectionR);
	curObject.selectionG=0;
	curObject.selectionB=0;
	curObject.selectionA=255;
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
	glBufferSubData( GL_ARRAY_BUFFER, pointsSize, normalzSize, curObject.normals);

	// set up vertex arrays
	GLuint vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0,
			BUFFER_OFFSET(0));

	GLuint vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 4, GL_FLOAT, GL_FALSE, 0,
			BUFFER_OFFSET(pointsSize));
}

//----------------------------------------------------------------------------

// OpenGL initialization
void init() {
	// Load shaders and use the resulting shader program
	program = InitShader("shaders/vshader.glsl", "shaders/fshader.glsl");
	glUseProgram(program);

	// initialize world plane
	worldPlane.init();

	// TODO for debugging convenience; final = remove below
	//loadObjectFromFile("data/bunnyS.obj");

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

	//Set up selection colors and a flag
	SelectColorRLoc = glGetUniformLocation(program,"selectionColorR");
	SelectColorGLoc = glGetUniformLocation(program,"selectionColorG");
	SelectColorBLoc = glGetUniformLocation(program,"selectionColorB");
	SelectColorALoc = glGetUniformLocation(program,"selectionColorA");
	glUniform1i(SelectColorRLoc, selectionColorR);
	glUniform1i(SelectColorGLoc, selectionColorG);
	glUniform1i(SelectColorBLoc, selectionColorB);
	glUniform1i(SelectColorALoc, selectionColorA);

	SelectFlagLoc = glGetUniformLocation(program, "flag");
	glUniform1i(SelectFlagLoc, flag);

	// Retrieve transformation uniform variable locations
	//model_view = glGetUniformLocation(program, "ModelView");
	model = glGetUniformLocation(program, "Model");
	view = glGetUniformLocation(program, "View");
	projection = glGetUniformLocation(program, "Projection");

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

	glEnable( GL_DEPTH_TEST);
	glClearColor(1.0, 1.0, 1.0, 1.0);
}

//----------------------------------------------------------------------------
void
mouse( int button, int state, int x, int y )
{
	// store location of latest mouse click
	if (button == GLUT_LEFT_BUTTON)
	{
		pressedMouseX = x;
		pressedMouseY = y;
	}

	//printf("Mouse button pressed at %d, %d\n", x, y);
	//0 is reserved for the background color so skip it.

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//render each object, setting the selection RGBA to the objects selection color (RGBA)
	for(int i=0; i < counter; i++)
	{
		//should store numVerts with vao and possibly the index in the array of objects, instead of storing only ints as I currently am
		//which represent the vaos
		flag = 1;

		glBindVertexArray(myObjects[i].vaoID);

		selectionColorR = myObjects[i].selectionR;
		selectionColorG = myObjects[i].selectionG;
		selectionColorB = myObjects[i].selectionB;
		selectionColorA = myObjects[i].selectionA;

		//sync with shader
		glUniform1i(SelectColorRLoc,selectionColorR);
		glUniform1i(SelectColorGLoc,selectionColorG);
		glUniform1i(SelectColorBLoc,selectionColorB);
		glUniform1i(SelectColorALoc,selectionColorA);
		glUniform1i(SelectFlagLoc, flag);

		//Draw the scene.  The flag will force shader to not use shading, but instead use a constant color
		glDrawArrays( GL_TRIANGLES, 0, myObjects[i].numVertices );
		glutPostRedisplay();  //MUST REMEMBER TO CALL POST REDISPLAY OR IT WONT RENDER!

	}

	//Now check the pixel location to see what color is found!
	GLubyte pixel[4];
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	//Read as unsigned byte.

	glReadPixels(x, viewport[3] - y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixel);
	picked = -1;
	for(int i=0; i < counter; i++)
	{

	//   printf("Red value is %d\n", pixel[0]);
		//printf("Green value is %d\n", pixel[1]);
		//printf("Blue value is %d\n", pixel[2]);
		//printf("Alpha value is %d\n", pixel[3]);
		if(myObjects[i].selectionR == ceil(pixel[0]) && myObjects[i].selectionG == pixel[1]
			&& myObjects[i].selectionB == pixel[2]&& myObjects[i].selectionA == pixel[3])
		picked = i;
	}

	printf("Picked == %d\n", picked);
	//uncomment below to see the color render
	// Swap buffers makes the back buffer actually show...in this case, we don't want it to show so we comment out.  For debuggin, youi can uncomment it to see the render of the back buffer which will hold your 'fake color render'
	//glutSwapBuffers();
}

//----------------------------------------------------------------------------
// motion callback - calls when mouse is pressed and dragging in window

//TODO: fill below with appropriate transforms
//		also possibly need additional state information
void motion(int x, int y)
{
	int xDiff = x - pressedMouseX;
	int yDiff = y - pressedMouseY;

	if (currentTransformMode == OBJECT_TRANSLATE)
	{

	}
	else if (currentTransformMode == OBJECT_ROTATE)
	{

	}
	else if (currentTransformMode == OBJECT_SCALE)
	{

	}
	else if (currentTransformMode == SCENE_ROTATE_X)
	{
		
	}
	else if (currentTransformMode == SCENE_ROTATE_Y)
	{

	}
	else if (currentTransformMode == SCENE_ROTATE_Z)
	{

	}
	else if (currentTransformMode == SCENE_TRANSLATE)
	{

	}
	else if (currentTransformMode == SCENE_DOLLY)
	{

	}
}

//----------------------------------------------------------------------------

void
display( void )
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// draw ground plane
	flag = 0;
	glUniform1i(SelectFlagLoc, flag);
	mat4 m = mat4(); // identity
	glUniformMatrix4fv(model, 1, GL_TRUE, m);
	glBindVertexArray(worldPlane.vaoID);
	glDrawArrays( GL_TRIANGLES, 0, worldPlane.numVertices );

	// draw objects
	for(int i=0; i < counter; i++)
	{
		if (i == picked)
		{
			// draw in wireframe mode
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); 
			glPolygonOffset(1.0, 2); //Try 1.0 and 2 for factor and units
		}

		//Render transformation
		mat4 m = myObjects[i].translate * myObjects[i].rotate * myObjects[i].scale;
		glUniformMatrix4fv(model, 1, GL_TRUE, m);

		//Normal render so set selection Flag to 0
		flag = 0;
		glUniform1i(SelectFlagLoc, flag);
		glBindVertexArray(myObjects[i].vaoID);
		glDrawArrays( GL_TRIANGLES, 0, myObjects[i].numVertices );

		if (i == picked)
		{
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

void mainMenuCallback(int id)
{
	if (id == 1)
	{
		// prompt user for input filename via command line
		string filename;
		while (filename.empty())
		{
			cout << "Input file name: ";
			cin >> filename;
		}

		if (filename.find(".obj") == string::npos)
		{
			cout << "Invalid file name. Loading canceled." << endl << endl;
		}
		else
		{
			// load .obj file
			loadObjectFromFile(filename);
			cout << "Successfully loaded " << filename << "." << endl << endl;
		}
	}
	else if (id == 2)
	{
		exit(0);
	}
}

void objMenuCallback(int id)
{
	if (id == 1)
	{
		cout << "Switched to OBJECT TRANSLATE mode." << endl;
		currentTransformMode = OBJECT_TRANSLATE;
	}
	else if (id == 2)
	{
		cout << "Switched to OBJECT ROTATE mode." << endl;
		currentTransformMode = OBJECT_ROTATE;
	}
	else if (id == 3)
	{
		cout << "Switched to OBJECT SCALE mode." << endl;
		currentTransformMode = OBJECT_SCALE;
	}
}

void sceneMenuCallback(int id)
{
	if (id == 1)
	{
		cout << "Switched to SCENE TRANSLATE mode." << endl;
		currentTransformMode = SCENE_TRANSLATE;
	}
	else if (id == 2)
	{
		cout << "Switched to SCENE DOLLY mode." << endl;
		currentTransformMode = SCENE_DOLLY;
	}
}

void sceneRotateMenuCallback(int id)
{
	if (id == 1)
	{
		cout << "Switched to SCENE ROTATE X mode." << endl;
		currentTransformMode = SCENE_ROTATE_X;
	}
	else if (id == 2)
	{
		cout << "Switched to SCENE ROTATE Y mode." << endl;
		currentTransformMode = SCENE_ROTATE_Y;
	}
	else if (id == 3)
	{
		cout << "Switched to SCENE ROTATE Z mode." << endl;
		currentTransformMode = SCENE_ROTATE_Z;
	}
}

// This creates the menu/submenus
void makeMenu()
{
	int objectTransformSubMenu = glutCreateMenu(objMenuCallback);
	glutAddMenuEntry("Translation", 1);
	glutAddMenuEntry("Rotation", 2);
	glutAddMenuEntry("Scale", 3);

	int sceneRotateSubMenu = glutCreateMenu(sceneRotateMenuCallback);
	glutAddMenuEntry("X", 1);
	glutAddMenuEntry("Y", 2);
	glutAddMenuEntry("Z", 3);

	int sceneTransformSubMenu = glutCreateMenu(sceneMenuCallback);
	glutAddSubMenu("Rotation", sceneRotateSubMenu);
	glutAddMenuEntry("Translation", 1);
	glutAddMenuEntry("Dolly", 2);

	int menu = glutCreateMenu(mainMenuCallback);
	glutAddMenuEntry("Load .obj File", 1);
	glutAddSubMenu("Object Transformation", objectTransformSubMenu);
	glutAddSubMenu("Scene Transformation", sceneTransformSubMenu);
	glutAddMenuEntry("Exit", 2);
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
	} else if (strcmp(argv[1], "O") == 0) {
		tranformType = orthonormal;
		left_ = atof(argv[2]);
		right_ = atof(argv[3]);
		bottom_ = atof(argv[4]);
		top_ = atof(argv[5]);
		zNear = atof(argv[6]);
		zFar = atof(argv[7]);
	} else {
		cerr << "Usage: ./SimpleProgram (O left right bottom top near far)|(P fov near far)" << endl;
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
