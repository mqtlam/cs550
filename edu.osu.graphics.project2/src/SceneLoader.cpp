/*
 * Assignment2.cpp
 *
 *  Created on: Oct 23, 2013
 *      Author: behrooz
 */

#include "SceneLoader.h"
#include <iostream>
#include <fstream>
#include <string.h>
#include <stdlib.h>
using namespace std;

namespace graphics {

SceneLoader::SceneLoader() {
	// TODO Auto-generated constructor stub

}

SceneLoader::~SceneLoader() {
	// TODO Auto-generated destructor stub
}
void SceneLoader::loadSceneFile(string sceneFileName) {
	vector<string> objectFileNames;
	ifstream sceneFile(sceneFileName.c_str());
	if (!sceneFile.good()) {
		cout << "Can not Open Scene File!" << endl;
	}
	string stringLine;
	if (sceneFile.is_open()) {
		while (sceneFile.good()) {
			getline(sceneFile, stringLine);
			if (stringLine[0] == '#')
				continue;
			objectFiles.push_back(stringLine);
		}
	}
}

Object SceneLoader::loadObjectFile(string objectFileName) {
	ifstream objectFile(objectFileName.c_str());
	if (!objectFile.good()) {
		cout << "Can not Open Object File " << objectFileName << "!" << endl;
	}
	Object object;
	string stringLine;
	if (objectFile.is_open()) {
		while (objectFile.good()) {
			getline(objectFile, stringLine);
			if (stringLine[0] == '#')
				continue;
			char* pch = strtok((char*) stringLine.c_str(), " ");
			if (pch == NULL)
				break;
			if (strcmp(pch, "vn") == 0) {
				Vertex vertex;
				pch = strtok(NULL, " ");
				vertex.x = atof(pch);
				pch = strtok(NULL, " ");
				vertex.y = atof(pch);
				pch = strtok(NULL, " ");
				vertex.z = atof(pch);
				object.verticeNormals.push_back(vertex);
			} else if (strcmp(pch, "v") == 0) {
				Vertex vertex;
				pch = strtok(NULL, " ");
				vertex.x = atof(pch);
				pch = strtok(NULL, " ");
				vertex.y = atof(pch);
				pch = strtok(NULL, " ");
				vertex.z = atof(pch);
				object.vertices.push_back(vertex);
			} else if (strcmp(pch, "f") == 0) {
				Face face;
				vector<string> stringVector;
				pch = strtok(NULL, " ");
				stringVector.push_back(string(pch));
				pch = strtok(NULL, " ");
				stringVector.push_back(string(pch));
				pch = strtok(NULL, " ");
				stringVector.push_back(string(pch));
				for (unsigned int i = 0; i < stringVector.size(); i++) {
					pch = strtok((char*) stringVector[i].c_str(), "//");
					face.vertexId[i] = atoi(pch);
					pch = strtok(NULL, "//");
				}
				object.faces.push_back(face);
			}

		}
	}
	return object;
}

void SceneLoader::loadAndAddObjects() {
	for (unsigned int i = 0; i < objectFiles.size(); i++) {
		Object object = loadObjectFile(objectFiles[i]);
		objects.push_back(object);
	}
}

} /* namespace graphics */
