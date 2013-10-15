#include <iostream>
#include <string>
#include <sstream>

#include "ofMain.h"

#include "CreepyPortrait.h"
#include "Model.h"
#include "VideoSource.h"

using namespace std;


void printUsage() {
	cout << "Usage: creepyportrait pi | <video device ID> [model]" << endl << endl;
	cout << "Arguments:" << endl;
	cout << "- pi: Use the Raspberry Pi camera. Only works on Raspberry Pi devices." << endl;
	cout << "- <video device ID>: Device ID number for the desired camera source." << endl;
	cout << "- model: An optional string to select a different 3D model.  Possible values are 'skull', 'jackevil', 'jackhappy', or 'all' (without quotes).  By default the value is 'skull'.  On a Raspberry Pi model B all models can't be loaded in memory at once." << endl << endl;
	cout << "Detected video devices:" << endl << endl;
	cout << "Device ID\tDevice Name" << endl;
	cout << "---------\t-----------" << endl;
	ofVideoGrabber video;
	for (auto device : video.listDevices()) {
		cout << setw(9) << setiosflags(ios::right) << device.id 
			 << "\t" << device.deviceName << endl;
	}
	cout << endl;
	cout << "Examples:" << endl;
	cout << "creepyportrait 0 jackevil" << endl << " - Run with video device 0 and the evil jack-o-lantern model." << endl;
	cout << "creepyportrait pi" << endl << " - Run with the Raspberry Pi camera and the default skull model." << endl;
	cout << endl;
	exit(0);
}

#ifdef TARGET_RASPBERRY_PI

// Setup default config for Raspberry Pi.
void configureApp(int videoDeviceId, bool usePiCamera, const string& model, CreepyPortrait* app) {
	const int videoWidth = 160;
	const int videoHeight = 120;
	const int screenWidth = 1024;
	const int screenHeight = 768;

	ofSetupOpenGL(screenWidth, screenHeight, OF_FULLSCREEN);

	app->displayVideo = true;
	app->faceBufferSize = 1;
	app->faceUpdateDelay = 2.0;
	app->model = model;
	app->noFaceResetSeconds = 6.0;
	app->skullFragmentShader = "lighting_es.frag";
	app->skullVertexShader = "lighting_es.vert";
	Model::useNormalMapping = false;
	if (usePiCamera) {
		app->video = make_shared<PiCameraSource>(videoWidth, videoHeight);
	}
	else {
		app->video = make_shared<VideoGrabberSource>(videoDeviceId, videoWidth, videoHeight);
	}
	app->videoFOV = 60;
}

#else

// Setup default config for other platforms.
void configureApp(int videoDeviceId, bool usePiCamera, const string& model, CreepyPortrait* app) {
	const int videoWidth = 320;
	const int videoHeight = 240;
	const int screenWidth = 1024;
	const int screenHeight = 768;
	const bool useNormalMapping = true;

	ofSetupOpenGL(screenWidth, screenHeight, OF_WINDOW);

	if (usePiCamera) {
		cout << "ERROR: Must be running on a Raspberry Pi to use the pi camera option!" << endl << endl;
		printUsage();
	}

	app->displayVideo = false;
	app->faceBufferSize = 5;
	app->faceUpdateDelay = 0.01;
	app->model = model;
	app->noFaceResetSeconds = 3.0;
	if (useNormalMapping) {
		app->skullFragmentShader = "lighting_gl_bump.frag";
		app->skullVertexShader = "lighting_gl_bump.vert";
	}
	else {
		app->skullFragmentShader = "lighting_gl.frag";
		app->skullVertexShader = "lighting_gl.vert";
	}
	Model::useNormalMapping = useNormalMapping;
	app->video = make_shared<VideoGrabberSource>(videoDeviceId, videoWidth, videoHeight);
	app->videoFOV = 60; // 73.0 for MS HD life cam
}

#endif

int main(int argc, char* argv[]){
	cout << "Creepy Portrait v1.0" << endl;
	cout << "Copyright 2013 Tony DiCola (tony@tonydicola.com)" << endl;
	cout << "Homepage: https://github.com/tdicola/creepyportrait" << endl << endl;
	ofSetLogLevel(OF_LOG_WARNING);
	// Parse video device parameter
	if (argc < 2) {
		cout << "ERROR: Must specify either a video device ID or pi as a command line parameter!" << endl << endl;
		printUsage();
	}
	int videoDeviceId = 0;
	bool usePiCamera = false;
	string camera(argv[1]);
	// Check for pi parameter to use the pi camera.
	if (camera == "pi") {
		usePiCamera = true;
	}
	// Check for a number to use as the video device id.
	else {
		stringstream buffer(camera);
		if (!(buffer >> videoDeviceId)) {
			// Couldn't parse parameter as a number.
			printUsage();
		}
	}
	// Parse optional model parameter
	string model = "skull";
	if (argc == 3) {
		model = argv[2];
		if (!(model == "skull" || model == "jackevil" || model == "jackhappy" || model == "all")) {
			cout << "ERROR: Model parameter must be one of 'skull', 'jackevil', 'jackhappy', or 'all' (lowercase, without quotes)!" << endl << endl;
			printUsage();
		}
	}
	else if (argc > 3) {
		cout << "ERROR: Too many command line parameters!" << endl << endl;
		printUsage();
	}
	// Setup the renderer and application.
	ofSetCurrentRenderer(ofGLProgrammableRenderer::TYPE);
	CreepyPortrait* app = new CreepyPortrait();
	configureApp(videoDeviceId, usePiCamera, model, app);
	ofRunApp(app);
}
