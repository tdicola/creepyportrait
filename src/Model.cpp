#include "Model.h"

using namespace std;

bool Model::useNormalMapping = false;

Model::Model(initializer_list<string> meshNames,
			 const string& diffuseName,
			 const string& specularName,
		 	 const string& ambientName,
		 	 const string& normalName,
		 	 float uniformScale,
			 float translateY,
			 float rotateY) {
	// Load meshes
	meshes.resize(meshNames.size());
	int i = 0;
	for (auto name : meshNames) {
		meshes[i].load(name);
		i += 1;
	}
	// Load textures
	diffuse.loadImage(diffuseName);
	specular.loadImage(specularName);
	if (useNormalMapping) {
		ambient.loadImage(ambientName);
		normal.loadImage(normalName);
		// Generate tangent values for normal mapping
		transform(begin(meshes), end(meshes), back_inserter(tangents), Model::generateTangents);
	}
	// Save scaling and translation values
	this->uniformScale = uniformScale;
	this->translateY = translateY;
	this->rotateY = rotateY;
}

void Model::draw(ofShader& shader) {
	// Scale and translate the model into position.
	ofRotateY(rotateY);
	ofScale(uniformScale, uniformScale, uniformScale);
	ofTranslate(0, translateY, 0); 
	// Set shader inputs and draw meshes.
	shader.setUniformTexture("diffuseTex", diffuse, 1);
	shader.setUniformTexture("specularTex", specular, 2);
	if (useNormalMapping) {
		shader.setUniformTexture("ambientTex", ambient, 3);
		shader.setUniformTexture("normalTex", normal, 4);
	}
	for (unsigned int i = 0; i < meshes.size(); ++i) {
		if (useNormalMapping) {
			shader.setAttribute4fv("tangent", tangents[i].data(), 4);
		}
		meshes[i].drawFaces();
	}
}

vector<float> Model::generateTangents(ofMesh& mesh) {
	// Tangent generation adapted from algorithm at http://www.terathon.com/code/tangent.html
	auto vertices = mesh.getVertices();
	auto normals = mesh.getNormals();
	auto texcoords = mesh.getTexCoords();
	auto triangles = mesh.getIndices();

	vector<float> tangents(vertices.size()*4);

	vector<ofVec3f> tan1(vertices.size(), ofVec3f(0,0,0));
	vector<ofVec3f> tan2(vertices.size(), ofVec3f(0,0,0));

	for (unsigned int i = 0; i < triangles.size(); i += 3) {
		long i1 = triangles[i];
		long i2 = triangles[i+1];
		long i3 = triangles[i+2];

		const ofVec3f& v1 = vertices[i1];
		const ofVec3f& v2 = vertices[i2];
		const ofVec3f& v3 = vertices[i3];

		const ofVec2f& w1 = texcoords[i1];
		const ofVec2f& w2 = texcoords[i2];
		const ofVec2f& w3 = texcoords[i3];

		float x1 = v2.x - v1.x;
		float x2 = v3.x - v1.x;
		float y1 = v2.y - v1.y;
		float y2 = v3.y - v1.y;
		float z1 = v2.z - v1.z;
		float z2 = v3.z - v1.z;

		float s1 = w2.x - w1.x;
		float s2 = w3.x - w1.x;
		float t1 = w2.y - w1.y;
		float t2 = w3.y - w1.y;

		float r = 1.0F / (s1 * t2 - s2 * t1);
		ofVec3f sdir((t2 * x1 - t1 * x2) * r, 
					 (t2 * y1 - t1 * y2) * r,
					 (t2 * z1 - t1 * z2) * r);
		ofVec3f tdir((s1 * x2 - s2 * x1) * r,
					 (s1 * y2 - s2 * y1) * r,
					 (s1 * z2 - s2 * z1) * r);

		tan1[i1] += sdir;
		tan1[i2] += sdir;
		tan1[i3] += sdir;

		tan2[i1] += tdir;
		tan2[i2] += tdir;
		tan2[i3] += tdir;
	}

	for (unsigned int i = 0; i < vertices.size(); ++i) {
		ofVec3f& n = normals[i];
		ofVec3f& t = tan1[i];

		// Gram-Schmidt orthogonalize
		auto tangent = (t - n * n.dot(t)).normalize();
		// Calculate handedness
		float w = (n.cross(t).dot(tan2[i]) < 0.0) ? -1.0 : 1.0;

		tangents[i*4] = tangent.x;
		tangents[i*4+1] = tangent.y;
		tangents[i*4+2] = tangent.z;
		tangents[i*4+3] = w;
	}

	return tangents;
}