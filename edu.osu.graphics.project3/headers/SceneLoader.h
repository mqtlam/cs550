/*
 * Assignment2.h
 *
 *  Created on: Oct 23, 2013
 *      Author: behrooz
 */

#ifndef ASSIGNMENT2_H_
#define ASSIGNMENT2_H_
#include <vector>
#include <string>
using namespace std;

namespace graphics {
class Vertex {
public:
	float x;
	float y;
	float z;
};
class Face {
public:
	int vertexId[3];
	int textureId[3];
	int vertexNormalId[3];
};
class Object {
public:
	vector<Vertex> vertices;
	vector<Vertex> verticeNormals;
	vector<Face> faces;
};
class SceneLoader {
public:
	vector<string> objectFiles;
	vector<Object> objects;
	SceneLoader();
	virtual ~SceneLoader();
	void loadSceneFile(string sceneFileName);
	Object loadObjectFile(string objectFileName);
	void loadAndAddObjects();
	int loadAndAddObject(string filename);
};

} /* namespace graphics */

#endif /* ASSIGNMENT2_H_ */
