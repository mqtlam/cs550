// SimpleProgram.cpp
//
// A simple 2D OpenGL program

#define GL3_PROTOTYPES

// COMMENT OR UNCOMMENT OUT THE LINE BELOW DEPENDING ON YOUR PLATFORM
#define USE_WINDOWS

// Include the vector and matrix utilities from the textbook, as well as some
// macro definitions.
#ifdef USE_WINDOWS
#include "../include/Angel.h"
#else
#include "Angel.h"
#include <GL/glew.h>
#include <GL/glut.h>
#endif
#include <fstream>
#include <sstream>
#include <string>

#ifdef __APPLE__
#  include <OpenGL/gl3.h>
#endif

#include <stdio.h>

using namespace std;

//------------------------------------------------------------------------------
// global constants

// Number of buckets
const int NUM_BUCKETS = 20;

// number of pixels per data point (used for initial window size)
const int BLOCK_SIZE = 5;

// no bucket special case value
const int NO_DATA_BUCKET = -1;

//------------------------------------------------------------------------------
// global variables for convenience

// A global variable for the number of points that will be in our object.
int g_NumPoints = 6;

// number of rows and columns in our data
int g_nRows, g_nCols;

//------------------------------------------------------------------------------
// function to read in the data file and return the data
// also updates nRows and nCols variables
float** readFromFile(string filename, int& nRows, int& nCols) {
	// 2D matrix of floats to hold data
	float** data;

	// open file
	ifstream fh(filename.c_str());
	if (fh.is_open()) {
		string line;
		int lineIndex = 0;
		int row = 0;
		while (getline(fh, line)) {
			if (lineIndex == 0) {
				// parse the first line of the file
				stringstream ssLine(line);
				string sHash;
				string sCols;
				string sRows;
				getline(ssLine, sHash, ' ');
				getline(ssLine, sCols, ' ');
				getline(ssLine, sRows, ' ');

				// assignment
				nRows = atoi(sRows.c_str());
				nCols = atoi(sCols.c_str());

				// set up data structure
				data = new float*[nRows];
				for (int i = 0; i < nRows; i++) {
					data[i] = new float[nCols];
				}
			} else if (!line.empty()) {
				// parse line of data
				stringstream ssLine(line);
				for (int col = 0; col < nCols; col++) {
					// parse token
					string sValue;
					getline(ssLine, sValue, ' ');

					// assignment
					data[row][col] = atof(sValue.c_str());
				}
				row++;
			}
			lineIndex++;
		}
	} else {
		cerr << "Unable to open file!" << endl;
		exit(1);
	}

	return data;
}

//------------------------------------------------------------------------------
// function to discretize the data
int** discretizeData(float** data, int nRows, int nCols) {
	// initialize data structure
	int** discretized = new int*[nRows];
	for (int i = 0; i < nRows; i++) {
		discretized[i] = new int[nCols];
	}

	// pass 1: get smallest and largest temperature value
	float smallestTempValue, largestTempValue;
	bool initialized = false;
	for (int row = 0; row < nRows; row++) {
		for (int col = 0; col < nCols; col++) {
			float temp = data[row][col];
			if (temp == 0)
				continue;

			if (!initialized) {
				smallestTempValue = temp;
				largestTempValue = temp;
				initialized = true;
			} else {
				smallestTempValue = min(smallestTempValue, temp);
				largestTempValue = max(largestTempValue, temp);
			}
		}
	}

	// pass 2: discretize data
	for (int row = 0; row < nRows; row++) {
		for (int col = 0; col < nCols; col++) {
			float temp = data[row][col];
			if (temp == 0) {
				discretized[row][col] = NO_DATA_BUCKET;
			} else {
				discretized[row][col] = int(
						(temp - smallestTempValue)
								/ (largestTempValue - smallestTempValue)
								* (NUM_BUCKETS - 1) + 0.5);
			}
		}
	}

	return discretized;
}

//----------------------------------------------------------------------------
// the HSV color model will be as follows
// h : [0 - 360]
// s : [0 - 1]
// v : [0 - 1]
// If you want it differently (in a 2 * pi scale, 256 instead of 1, etc,
// you'll have to change it yourself.
// rgb is returned in 0-1 scale (ready for color3f)
void HSVtoRGB(float hsv[3], float rgb[3]) {
	float tmp1 = hsv[2] * (1 - hsv[1]);
	float tmp2 = hsv[2]
			* (1 - hsv[1] * (hsv[0] / 60.0f - (int) (hsv[0] / 60.0f)));
	float tmp3 = hsv[2]
			* (1 - hsv[1] * (1 - (hsv[0] / 60.0f - (int) (hsv[0] / 60.0f))));
	switch ((int) (hsv[0] / 60)) {
	case 0:
		rgb[0] = hsv[2];
		rgb[1] = tmp3;
		rgb[2] = tmp1;
		break;
	case 1:
		rgb[0] = tmp2;
		rgb[1] = hsv[2];
		rgb[2] = tmp1;
		break;
	case 2:
		rgb[0] = tmp1;
		rgb[1] = hsv[2];
		rgb[2] = tmp3;
		break;
	case 3:
		rgb[0] = tmp1;
		rgb[1] = tmp2;
		rgb[2] = hsv[2];
		break;
	case 4:
		rgb[0] = tmp3;
		rgb[1] = tmp1;
		rgb[2] = hsv[2];
		break;
	case 5:
		rgb[0] = hsv[2];
		rgb[1] = tmp1;
		rgb[2] = tmp2;
		break;
	default:
		cout << "Inconceivable!\n";
	}

}

//----------------------------------------------------------------------------
void init(int** discretizedData) {
	// Specify the discretized data in terms of vertices and colors for drawing
	vec2* vertices = new vec2[g_NumPoints];
	vec3* colors = new vec3[g_NumPoints];

	// camera sees x = [-1, 1] and y = [-1, 1]
	// so fit the number of rows and columns into this view by discretizing
	// this space into horizontal and vertical units
	// first discretize by assuming at origin,
	// then multiply by two since origin will be at (-1, -1)
	float horzUnit = 1.0 / g_nCols * 2;
	float vertUnit = 1.0 / g_nRows * 2;

	// calculate vertices and colors for each discretized data point
	for (int row = 0; row < g_nRows; row++) {
		for (int col = 0; col < g_nCols; col++) {
			int index = 6 * (row * g_nCols + col);

			// make a grid square:
			// draw two unit triangles based on our defined units: horzUnit, vertUnit
			// first draw relative to origin, then subtract 1 in both coordinates to put at (-1, -1)

			// first triangle
			vertices[index] = vec2(col * horzUnit - 1, row * vertUnit - 1);
			vertices[index + 1] = vec2((col + 1) * horzUnit - 1,
					row * vertUnit - 1);
			vertices[index + 2] = vec2((col + 1) * horzUnit - 1,
					(row + 1) * vertUnit - 1);
			// second triangle
			vertices[index + 3] = vec2(col * horzUnit - 1, row * vertUnit - 1);
			vertices[index + 4] = vec2(col * horzUnit - 1,
					(row + 1) * vertUnit - 1);
			vertices[index + 5] = vec2((col + 1) * horzUnit - 1,
					(row + 1) * vertUnit - 1);

			// assign appropriate color:
			// map bucket to HSV space then convert to RGB

			int bucket = discretizedData[row][col];

			// default color is white for no data bucket
			float rgb[3] = { 1, 1, 1 };
			if (bucket != NO_DATA_BUCKET) {
				// color with value
				float hsv[3] =
						{ 240 * (1.0 - 1.0 * bucket / NUM_BUCKETS), 1, 1 };
				HSVtoRGB(hsv, rgb);
			}

			for (int i = 0; i < 6; i++)
				colors[index + i] = vec3(rgb[0], rgb[1], rgb[2]);
		}
	}

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
	glBufferData(GL_ARRAY_BUFFER, g_NumPoints * sizeof(vec2), vertices,
	GL_STATIC_DRAW);

	// Load the shaders.  Note that this function is not offered by OpenGL
	// directly, but provided as a convenience.
#ifdef USE_WINDOWS
	GLuint program = InitShader("../SimpleProgram/src/vshader32.glsl",
			"../SimpleProgram/src/fshader32.glsl");
#else
	GLuint program = InitShader("shaders/vshader32.glsl", "shaders/fshader32.glsl");
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
	glBufferData(GL_ARRAY_BUFFER, g_NumPoints * sizeof(vec3), colors,
	GL_STATIC_DRAW);

	GLuint colorLoc = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(colorLoc);
	glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	// Make the background white
	glClearColor(1.0, 1.0, 1.0, 1.0);
}

//----------------------------------------------------------------------------
void display(void) {
	// clear the window
	glClear(GL_COLOR_BUFFER_BIT);

	// Draw the points.  The parameters to the function are: the mode, the first
	// index, and the count.
	glDrawArrays(GL_TRIANGLES, 0, g_NumPoints);
	glFlush();
	glutSwapBuffers();
}

//----------------------------------------------------------------------------
void keyboard(unsigned char key, int x, int y) {
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
int main(int argc, char** argv) {
	// check arguments
	// expects one argument: file path to data text file
	if (argc < 2) {
		cerr << "Usage: " << argv[0] << " datafile" << endl;
		exit(1);
	}

	// file path to data txt file
	string filename = argv[1];

	// load data and discretize
	g_nRows = 0;
	g_nCols = 0;
	float** data = readFromFile(filename, g_nRows, g_nCols);
	int** discretizedData = discretizeData(data, g_nRows, g_nCols);
	g_NumPoints = 6 * g_nRows * g_nCols;

	// misc
	float rgb[] = { 0, 0, 0 };
	float hsv[] = { 222, 81 / 100.0, 84 / 100.0 };

	HSVtoRGB(hsv, rgb);
	cout << "HSV: " << hsv[0] << " " << hsv[1] << " " << hsv[2] << "  to  RGB: "
			<< rgb[0] << " " << rgb[1] << " " << rgb[2] << endl;

	glutInit(&argc, argv);
#ifdef __APPLE__
	glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_RGBA | GLUT_DOUBLE);
#else
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitContextVersion(3, 2);
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
#endif
	//glutInitWindowSize(512, 512);
	glutInitWindowSize(BLOCK_SIZE * g_nCols, BLOCK_SIZE * g_nRows);
	glutInitWindowPosition(500, 300);
	glutCreateWindow("Simple Open GL Program");
	printf("%s\n%s\n", glGetString(GL_RENDERER), glGetString(GL_VERSION));

#ifndef __APPLE__
	glewExperimental = GL_TRUE;
	glewInit();
#endif

	init(discretizedData);

	//NOTE:  callbacks must go after window is created!!!
	glutKeyboardFunc(keyboard);
	glutDisplayFunc(display);
	glutMainLoop();

	return (0);
}
