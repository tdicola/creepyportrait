#pragma once

#include <algorithm>
#include <initializer_list>
#include <iterator>
#include <string>
#include <vector>

#include "ofMain.h"

class Model {

public:
	Model(std::initializer_list<std::string> meshNames,
		  const std::string& diffuseName,
		  const std::string& specularName,
		  const std::string& ambientName,
		  const std::string& normalName,
		  float uniformScale,
		  float translateY,
		  float rotateY);
	void draw(ofShader& shader);
	static bool useNormalMapping;
	
private:
	static std::vector<float> generateTangents(ofMesh& mesh);

	float uniformScale;
	float translateY;
	float rotateY;
	ofImage diffuse;
	ofImage specular;
	ofImage ambient;
	ofImage normal;
	std::vector<ofMesh> meshes;
	std::vector<std::vector<float>> tangents;

};