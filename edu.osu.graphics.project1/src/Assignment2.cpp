/*
 * Assignment2.cpp
 *
 *  Created on: Oct 23, 2013
 *      Author: behrooz
 */

#include "Assignment2.h"
#include <iostream>
#include <fstream>
namespace graphics {

Assignment2::Assignment2() {
	// TODO Auto-generated constructor stub

}

Assignment2::~Assignment2() {
	// TODO Auto-generated destructor stub
}

void Assignment2::loadSceneFile(string sceneFileName) {
	ifstream sceneFile(sceneFileName.c_str());
	if (!sceneFile.good()) {
		cout << "Can not Open Scene File!" << endl;
	}
	string stringLine;
	if (sceneFile.is_open()) {
		while (sceneFile.good()) {
			getline(sceneFile, stringLine);
			cout << stringLine << endl;
		}
	}
}
int main(int argc, char **argv) {
	Assignment2 assignment;
	assignment.loadSceneFile(argv[1]);
}

} /* namespace graphics */
