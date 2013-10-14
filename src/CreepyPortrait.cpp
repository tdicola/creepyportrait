#include "CreepyPortrait.h"

using namespace std;

// #include "ofxAssimpModelLoader.h"
// #include <sstream>
// void CreepyPortrait::save_model(ofxAssimpModelLoader& model, const string& prefix) {
// 	cout << "bongos " << model.getMeshCount() << endl;
// 	for (unsigned int i = 0; i < model.getMeshCount(); ++i) {
// 		stringstream filename;
// 		filename << prefix << i << ".ply";
// 		model.getMesh(i).save(filename.str(), false);
// 	}
// }

//--------------------------------------------------------------
void CreepyPortrait::setup(){
	ofSetVerticalSync(true);
	// Make texture coordinates go from 0...1
	ofDisableArbTex();
	// Calculate focal length of video camera (relative to video pixel size).
	pixelFocalLength = sqrt(pow(video->getWidth()/2.0, 2) + pow(video->getHeight()/2.0, 2))/sin(ofDegToRad(videoFOV/2.0));
	// Set up model rotation state
	currentRotation = ofVec2f(0,0);
	// Set up camera
	camera.setDistance(cameraDistance);
	camera.setTarget(ofVec3f(0, 0, 0));
	camera.disableMouseInput();
	// Set up video face detection buffer.
	detector.setBufferSize(faceBufferSize);
	// Load background
	// Image provided by: http://www.flickr.com/photos/57845051@N00/2884743046/in/photolist-5oV4B5-5y9Emo-5CwHZX-5FmVMm-5FHAVi-5FHBdg-5Pvqej-5QoGWr-5TQnrg-5U1kGK-5UQn9Y-5YtFGg-5YuhYR-63ARFf-6rv6wa-6KhmCY-6PBo3r-6RoBWu-7hAxcY-7isZdJ-7qjH53-7uLzi3-7uNx1m-bBHpKG-9f6MMX-ciGuDS-8idfF6-84MapV-auRVkb-8Uc9Un-9QJM29-aBJLDL-84P6M3-bkqTRy-dqCwjk-7z8TjC-9gcHkG-bCthyt-8VWSxe-9JFMuc-8RXXX3-aUPUkX-9svEM8-bjKDib-bKf5z2-9heXnG-8xG837-aeLZVX-9Pn5fQ-973wYP-7Smks9
	curtain.loadImage("redcurtain_1024.jpg");
	// Load lighting shader
	shader.load(skullVertexShader, skullFragmentShader);
	// Load models
	// Compile-time conditionals are here to allow making a build
	// which does not include certain models so the memory requirements
	// are not as high.  Unfortunately the Raspberry Pi model B can only
	// use ~2 models at a time.
	#ifdef LOAD_SKULL
	models.push_back(Model({"models/skull_mesh_1.ply", "models/skull_mesh_2.ply"},
							"models/skull_diffuse_1024.jpg",
							"models/skull_specular_1024.jpg",
							"models/skull_ao_1024.jpg",
							"models/skull_normal_1024.jpg",
							1.0,	// Don't scale the skull.
							30.0,	// Move the skull up to center.
							0.0));	// Don't rotate the skull.
	#endif
	#ifdef LOAD_JACK_EVIL
	models.push_back(Model({"models/jack_evil_mesh_1.ply", "models/jack_evil_mesh_2.ply"},
							"models/jack_evil_diffuse_1024.jpg",
							"models/jack_evil_specular_1024.jpg",
							"models/jack_evil_ilumination_1024.jpg",
							"models/jack_evil_normal_1024.jpg",
							6.5,		// Scale the pumpkin up in size.
							-40.0,		// Move the pumpkin down to center.
							0.0)); 		// Don't rotate the evil pumpkin.
	#endif
	#ifdef LOAD_JACK_HAPPY
	models.push_back(Model({"models/jack_happy_mesh_1.ply", "models/jack_happy_mesh_2.ply"},
							"models/jack_happy_diffuse_1024.jpg",
							"models/jack_happy_specular_1024.jpg",
							"models/jack_happy_ilumination_1024.jpg",
							"models/jack_happy_normal_1024.jpg",
							6.5,		// Scale the pumpkin up in size.
							-40.0,		// Move the pumpkin down to center.
							-4.0));		// Rotate the happy pumpkin to better face center.
	#endif
	currentModel = begin(models);
	// Prime the time delta for the first update loop run.
	lastUpdate = ofGetElapsedTimef();
}

//--------------------------------------------------------------
void CreepyPortrait::update(){
	// Update the video source (no-op internally for Raspberry Pi camera).
	video->update();
	// NOTE: Don't try to read the video source here if using the pi camera!
	// See note inside VideoSource.cpp's getPixels() function for the explanation.
}

//--------------------------------------------------------------
void CreepyPortrait::draw(){
	// Update current model rotation in the draw routine, NOT the update routine.
	// See note inside VideoSource.cpp's getPixels() function for the explanation.
	updateCurrentRotation();
	// Draw background
	curtain.draw(0,0,ofGetWidth(),ofGetHeight());
	// Enable depth buffer
	ofEnableDepthTest();
	// Set up the camera
	camera.begin();
	// Save transformation state
	ofPushMatrix();
	// Rotate model
	ofRotateX(currentRotation.x);
	ofRotateY(currentRotation.y);
	// Set up the shader for rendering.
	shader.begin();
	// Transform light from world to camera position.
	auto lightCameraPosition = camera.getModelViewMatrix()*lightPosition;
	shader.setUniform4f("lightCameraPosition", lightCameraPosition.x, lightCameraPosition.y, lightCameraPosition.z, 1.0);
	// Draw the current model.
	currentModel->draw(shader);
	// Reset all the modified rendering state.
	shader.end();
	ofPopMatrix();
	camera.end();
	ofDisableDepthTest();
	// Draw video and detected face rectangle if enabled.
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
		// Good for testing the shaders.
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
				auto angle = cameraAngleToModelAngle(cameraPointToAngle(ofVec2f(currentFace.getCenter().x, currentFace.getCenter().y)),
					currentFace.getArea());
				// Flip x axis because video is mirrored
				angle.x *= -1;
				oldRotation = targetRotation;
				// Rotate head up-down around x axis
				targetRotation.x = angle.y;
				// Rotate head left-right around y axis
				targetRotation.y = angle.x;
			}
			else if ((time - faceLastSeen) > noFaceResetSeconds) {
				// Lost the face, go back to center.
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

ofVec2f CreepyPortrait::cameraPointToAngle(const ofVec2f& point) {
	// Make the center of the video 0, 0.
	int x = point.x - (video->getWidth()/2);
	int y = point.y - (video->getHeight()/2);
	// Find the angle to the point.
	float xAngle = asin(x/pixelFocalLength);
	float yAngle = asin(y/pixelFocalLength);
	return ofVec2f(ofRadToDeg(xAngle), ofRadToDeg(yAngle));
}

ofVec2f CreepyPortrait::cameraAngleToModelAngle(const ofVec2f& angle, float area) {
	// Determine what angle the model should face to look at a point along the specified camera angle.
	// This angle is not the same as the camera angle because the model is 'behind' the camera.
	// TODO: Scale distance based on detected face distance to better approximate face distance.
	float distance = faceDepth;
	float xWidth = sin(ofDegToRad(angle.x));
	float yWidth = sin(ofDegToRad(angle.y));
	return ofVec2f(ofRadToDeg(asin(xWidth/distance)), ofRadToDeg(asin(yWidth/distance)));
}

//--------------------------------------------------------------
void CreepyPortrait::keyPressed(int key){
	if (key == 'v') {
		displayVideo = !displayVideo;
	}
	else if (key == 'r') {
		rotateSkull = !rotateSkull;
	}
	else if (key == 'm') {
		currentModel++;
		if (currentModel == end(models)) {
			currentModel = begin(models);
		}
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
