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

class Assignment2 {
private:
	vector<string> objectFiles;
public:
	void loadSceneFile(string sceneFileName);
	Assignment2();
	virtual ~Assignment2();

};

} /* namespace graphics */

#endif /* ASSIGNMENT2_H_ */
