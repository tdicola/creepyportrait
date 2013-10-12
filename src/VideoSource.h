#pragma once

#include "ofMain.h"

#ifdef TARGET_RASPBERRY_PI
#include "ofxRPiCameraVideoGrabber.h"
#endif

class IVideoSource {

public:
	virtual ~IVideoSource() {}
	virtual void update() = 0;
	virtual bool isFrameNew() = 0;
	virtual ofPixels& getPixels() = 0;
	virtual int getWidth() = 0;
	virtual int getHeight() = 0;
	virtual void draw(int x, int y, int width, int height) = 0;

};

class VideoGrabberSource: public IVideoSource {

public:
	VideoGrabberSource(int deviceId, int desiredWidth, int desiredHeight);
	VideoGrabberSource(const VideoGrabberSource& copy);
	virtual ~VideoGrabberSource();
	virtual void update();
	virtual bool isFrameNew();
	virtual ofPixels& getPixels();
	virtual int getWidth();
	virtual int getHeight();
	virtual void draw(int x, int y, int width, int height);

private:
	ofVideoGrabber video;

};

#ifdef TARGET_RASPBERRY_PI

class PiCameraSource: public IVideoSource {

public:
	PiCameraSource(int desiredWidth, int desiredHeight);
	PiCameraSource(const PiCameraSource& copy);
	virtual ~PiCameraSource();
	virtual void update();
	virtual bool isFrameNew();
	virtual ofPixels& getPixels();
	virtual int getWidth();
	virtual int getHeight();
	virtual void draw(int x, int y, int width, int height);

private:
	ofxRPiCameraVideoGrabber video;
	ofFbo fbo;
	ofPixels frame;
	
};

#endif