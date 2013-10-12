#pragma once

#include <list>
#include <memory>

#include "ofMain.h"
#include "ofxOpenCv.h"

// Class to detect a single face (the largest) in a stream of video frames.
class VideoFaceDetector {

public:
	VideoFaceDetector();
	void setBufferSize(int size);
	void updateFrame(ofPixels& frame);
	bool isFaceDetected();
	ofRectangle getDetectedFace();

private:
	ofxCvHaarFinder finder;
	std::list<std::shared_ptr<ofxCvBlob>> faceBuffer;
	std::shared_ptr<ofRectangle> currentFace;

	std::shared_ptr<ofRectangle> getCurrentFace();
};