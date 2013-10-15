#pragma once

#include <iostream>
#include <list>
#include <memory>
#include <string>
#include <vector>

#include "ofMain.h"

#include "Model.h"
#include "VideoSource.h"
#include "VideoFaceDetector.h"


class CreepyPortrait: public ofBaseApp {

public:
	CreepyPortrait() {}
	~CreepyPortrait() {}

	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	// Application state that can be modified is below:

	// Distance camera is back on the Z axis from the origin.  For the skull model
	// this should be about 650.
	float 		cameraDistance = 650.0;

	// True/false if video and detected faces should be displayed.
	bool 		displayVideo;

	// Number of face detection samples to buffer.  The buffered faces will be used
	// to calculate the position of the detected face.  A value of 1 is no buffering
	// which can be somewhat noisey (i.e. the detected face moves around a lot).
	// Using a higher number will smooth out the noise, but not easily catch fast moving
	// faces.
	int 		faceBufferSize;

	// How far away from the camera to assume detected faces lie.  Ideally this should
	// be scaled in some way, perhaps based on the area of the detected face.  For now
	// a fixed value of 10 works well.  Increase or decrease this if the skull looks too
	// far or not far enough to the left and right.
	float 		faceDepth = 10.0;

	// How often to delay between runs of the face detection logic, in seconds.
	// On the Raspberry Pi face detection of 160x120 video takes about 500-800ms, so use 
	// a large value like 2 or 3.
	// On a PC this can be quite low, around 0.01 or less (but you should bump up faceBufferSize
	// so the detected face isn't as noisey).
	float 		faceUpdateDelay;

	// Position of the point light source in world coordinates.
	ofVec4f 	lightPosition = ofVec4f(-600, 100, 600, 1);

	// The model(s) to load for rendering.  Can be one of 'skull', 'jackevil', 'jackhappy', or 'all'.
	std::string model;

	// How long to wait for no detected face before the skull rotates back to 0.
	float 		noFaceResetSeconds;

	// True/false to render the skull (true) or evil jacklantern (false).
	bool 		renderSkullMode = true;

	// True/false if the skull should just rotate around the Y axis instead of looking
	// in the direction of detected faces.  Good for testing the shader.
	bool 		rotateSkull = false;

	// Velocity of the rotating skull in degrees/second.
	float 		rotateSkullVelocity = 5.0;

	// Skull rendering fragment and vertex shader files names.
	std::string	skullFragmentShader;
	std::string	skullVertexShader;

	// Enable or disable normal/bump mapping in the shader.  Unfortunately bump
	// mapping doesn't currently work on the Raspberry Pi (but looks good on the PC).
	bool 		useNormalMapping;

	// Reference to a video source.
	std::shared_ptr<IVideoSource> video;

	// Diagonal field of view of the camera, in degrees.  
	// For the Raspberry Pi camera this should be about 60.
	// For most PC webcams a value of 60 is good too.  If you have a wider angle web cam
	// or know the specific diagonal FOV, set this higher.  A Microsoft Lifecam HD cinema
	// I test with uses a 73 degree diagonal FOV.
	float 		videoFOV;

	// Pixel offset to use when rendering video on the screen.
	ofVec2f 	videoOffset = ofVec2f(10,10);

private:
	void updateCurrentRotation();
	ofVec2f cameraPointToAngle(const ofVec2f& point);
	ofVec2f cameraAngleToModelAngle(const ofVec2f& angle, float area);

	// Internal application state that shouldn't be modified unless you know what you're doing:
	ofEasyCam camera;
	float pixelFocalLength;
	float faceLastSeen = 0.0;
	float faceLastUpdate = 0.0;
	float lastUpdate;
	ofImage curtain;
	ofShader shader;
	std::list<Model> models;
	std::list<Model>::iterator currentModel;
	VideoFaceDetector detector;
	ofVec2f currentRotation;
	ofVec2f targetRotation;
	ofVec2f oldRotation;
};
