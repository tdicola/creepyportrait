#include "CreepyPortrait.h"

using namespace std;

//--------------------------------------------------------------
void CreepyPortrait::setup(){
	// Make texture coordinates go from 0...1
	ofDisableArbTex();
	ofSetVerticalSync(true);

	// Setup the mesh and textures.
	// Mesh from http://www.videocopilot.net/blog/2012/10/free-halloween-3d-model-pack/
	skullMesh1.load("skull_mesh_1.ply");
	skullMesh2.load("skull_mesh_2.ply");
	skullDiffuse.loadImage("skull_diffuse_1024.jpg");
	skullSpecular.loadImage("skull_specular_1024.jpg");
	skullNormal.loadImage("skull_normal_1024.jpg");
	skullAmbient.loadImage("skull_ao_1024.jpg");

	// Set up background
	// Image from http://www.flickr.com/photos/57845051@N00/2884743046/in/photolist-5oV4B5-5y9Emo-5CwHZX-5FmVMm-5FHAVi-5FHBdg-5Pvqej-5QoGWr-5TQnrg-5U1kGK-5UQn9Y-5YtFGg-5YuhYR-63ARFf-6rv6wa-6KhmCY-6PBo3r-6RoBWu-7hAxcY-7isZdJ-7qjH53-7uLzi3-7uNx1m-bBHpKG-9f6MMX-ciGuDS-8idfF6-84MapV-auRVkb-8Uc9Un-9QJM29-aBJLDL-84P6M3-bkqTRy-dqCwjk-7z8TjC-9gcHkG-bCthyt-8VWSxe-9JFMuc-8RXXX3-aUPUkX-9svEM8-bjKDib-bKf5z2-9heXnG-8xG837-aeLZVX-9Pn5fQ-973wYP-7Smks9
	curtain.loadImage("redcurtain_1920.jpg");

	// Set up lighting shader
	shader.load(skullVertexShader, skullFragmentShader);

	// Generate tangents for normal mapping.
	if (useNormalMapping) {
		generateTangents(skullMesh1, skullMesh1Tangents);
		generateTangents(skullMesh2, skullMesh2Tangents);
	}

	// Calculate focal length of video camera (relative to pixel size)
	pixelFocalLength = sqrt(pow(video->getWidth()/2.0, 2) + pow(video->getHeight()/2.0, 2))/sin(ofDegToRad(videoFOV/2.0));

	// Set up model rotation state
	currentRotation = ofVec2f(0,0);

	// Set up camera
	camera.setDistance(cameraDistance);
	camera.setTarget(ofVec3f(0, 0, 0));
	camera.disableMouseInput();

	detector.setBufferSize(faceBufferSize);

	lastUpdate = ofGetElapsedTimef();
}

//--------------------------------------------------------------
void CreepyPortrait::update(){
	video->update();
}

//--------------------------------------------------------------
void CreepyPortrait::draw(){

	// Update current rotation in the draw routine because on the Raspberry Pi 
	// and other OpenGL ES devices we have to render the video frame texture
	// to a frame buffer object in order to read the pixels.  Unfortunately rendering
	// the video frame texture requires the default pixel shader to be setup, which
	// happens in OpenFrameworks before the draw event is fired.
	updateCurrentRotation();

	// Draw background
	curtain.draw(0,0,ofGetWidth(),ofGetHeight());

	// Draw skull
	ofEnableDepthTest();
	camera.begin();
	renderSkull(currentRotation);
	camera.end();
	ofDisableDepthTest();

	// Draw video and detected face rectangle if enabled
	if (displayVideo) {
		video->draw(videoOffset.x, videoOffset.y, video->getWidth(), video->getHeight());
		if (detector.isFaceDetected()) {
			ofPushStyle();
			ofSetColor(0, 255, 0);
			ofNoFill();
			auto currentFace = detector.getDetectedFace();
			ofRect(currentFace + ofPoint(videoOffset.x, videoOffset.y, 0));
			ofPopStyle();
		}
	}

}

void CreepyPortrait::updateCurrentRotation() {
	float time = ofGetElapsedTimef();
	float delta = time - lastUpdate;
	lastUpdate = time;

	if (rotateSkull) {
		// Rotate the skull around the Y axis.  Don't do any face detection.
		currentRotation.y += rotateSkullVelocity*delta;
		if (currentRotation.y > 360.0) {
			currentRotation.y -= 360.0;
		}
	}
	else {
		// Update face detection
		if (video->isFrameNew() && (time - faceLastUpdate >= faceUpdateDelay)) {
			// Update face detection with a new video frame.
			detector.updateFrame(video->getPixels());

			// Update target skull rotation
			if (detector.isFaceDetected()) {
				faceLastSeen = time;
				
				auto currentFace = detector.getDetectedFace();
				auto angle = videoAngleToHeadAngle(videoPointToAngle(ofVec2f(currentFace.getCenter().x, currentFace.getCenter().y)),
					currentFace.getArea());
				angle.x *= -1; // Flip x axis because video is mirrored

				oldRotation = targetRotation;
				targetRotation.x = angle.y; // Rotate head up-down around x axis
				targetRotation.y = angle.x; // Rotate head left-right around y axis
			}
			else if ((time - faceLastSeen) > noFaceResetSeconds) {
				// Lost the face, go back to center
				oldRotation = targetRotation;
				targetRotation.x = 0;
				targetRotation.y = 0;
			}
			// else don't move the head

			// Set last update to the current time.  Because face detection on the pi
			// can take a long time (500-800ms+) we should read the time again.
			faceLastUpdate = ofGetElapsedTimef();
		}
		// Animate moving from old to target skull rotation in the time it takes to wait for
		// the next face detection.
		else if (targetRotation != currentRotation) {
			float position = ofClamp((time - faceLastUpdate)/faceUpdateDelay, 0, 1);
			currentRotation = oldRotation.getInterpolated(targetRotation, position);
		}
	}
}

ofVec2f CreepyPortrait::videoPointToAngle(const ofVec2f& point) {
	// Make the center of the video 0,0
	int x = point.x - (video->getWidth()/2);
	int y = point.y - (video->getHeight()/2);
	// Find the angle to the point
	float xAngle = asin(x/pixelFocalLength);
	float yAngle = asin(y/pixelFocalLength);
	return ofVec2f(ofRadToDeg(xAngle), ofRadToDeg(yAngle));
}

ofVec2f CreepyPortrait::videoAngleToHeadAngle(const ofVec2f& angle, float area) {
	// Determine what angle the head should face to look at a point on the camera angle.
	// This angle is not the same as the camera angle because the skull is behind the camera.
	// TODO: Scale distance based on area to approximate depth
	float distance = faceDepth;
	float xWidth = sin(ofDegToRad(angle.x));
	float yWidth = sin(ofDegToRad(angle.y));
	return ofVec2f(ofRadToDeg(asin(xWidth/distance)), ofRadToDeg(asin(yWidth/distance)));
}

void CreepyPortrait::renderSkull(const ofVec2f& rotation) {
	ofPushMatrix();

	ofRotateX(rotation.x);
	ofRotateY(rotation.y);
	// Push the skull up a bit so it's better centered.
	ofTranslate(0,30,0); 

	shader.begin();
	shader.setUniformTexture("diffuseTex", skullDiffuse, 1);
	shader.setUniformTexture("specularTex", skullSpecular, 2);
	shader.setUniformTexture("ambientTex", skullAmbient, 3);
	if (useNormalMapping) {
		shader.setUniformTexture("normalTex", skullNormal, 4);
	}

	auto lightCameraPosition = camera.getModelViewMatrix()*lightPosition;
	shader.setUniform4f("lightCameraPosition", lightCameraPosition.x, lightCameraPosition.y, lightCameraPosition.z, 1.0);
	
	if (useNormalMapping) {
		shader.setAttribute4fv("tangent", skullMesh1Tangents.data(), 4);
	}
	skullMesh1.drawFaces();
	if (useNormalMapping) {
		shader.setAttribute4fv("tangent", skullMesh2Tangents.data(), 4);
	}
	skullMesh2.drawFaces();
	
	shader.end();

	ofPopMatrix();
}

void CreepyPortrait::generateTangents(ofMesh& mesh, vector<float>& tangents) {
	// Tangent generation adapted from algorithm at http://www.terathon.com/code/tangent.html

	auto vertices = mesh.getVertices();
	auto normals = mesh.getNormals();
	auto texcoords = mesh.getTexCoords();
	auto triangles = mesh.getIndices();

	tangents.resize(vertices.size()*4);

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
}

//--------------------------------------------------------------
void CreepyPortrait::keyPressed(int key){
	if (key == 'v') {
		displayVideo = !displayVideo;
	}
	else if (key == 'r') {
		rotateSkull = !rotateSkull;
	}
}

//--------------------------------------------------------------
void CreepyPortrait::keyReleased(int key){

}

//--------------------------------------------------------------
void CreepyPortrait::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void CreepyPortrait::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void CreepyPortrait::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void CreepyPortrait::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void CreepyPortrait::windowResized(int w, int h){

}

//--------------------------------------------------------------
void CreepyPortrait::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void CreepyPortrait::dragEvent(ofDragInfo dragInfo){ 

}
