#include "VideoSource.h"

#include <iostream>

using namespace std;

VideoGrabberSource::VideoGrabberSource(int deviceId, int desiredWidth, int desiredHeight) {
	video.setDeviceID(deviceId);
	video.initGrabber(desiredWidth, desiredHeight, true);
}

VideoGrabberSource::VideoGrabberSource(const VideoGrabberSource& copy) {
	video = copy.video;
}

VideoGrabberSource::~VideoGrabberSource() {
	video.close();
}

void VideoGrabberSource::update() {
	video.update();
}

bool VideoGrabberSource::isFrameNew() {
	return video.isFrameNew();
}
		
ofPixels& VideoGrabberSource::getPixels() {
	return video.getPixelsRef();
}

int VideoGrabberSource::getWidth() {
	return video.width;
}

int VideoGrabberSource::getHeight() {
	return video.height;
}

void VideoGrabberSource::draw(int x, int y, int width, int height) {
	video.draw(x, y, width, height);
}

#ifdef TARGET_RASPBERRY_PI

PiCameraSource::PiCameraSource(int desiredWidth, int desiredHeight) {
	video.setup(desiredWidth, desiredHeight, 60);
	fbo.allocate(video.getWidth(), video.getHeight(), GL_RGB);
	frame.allocate(video.getWidth(), video.getHeight(), OF_PIXELS_RGB);
}

PiCameraSource::PiCameraSource(const PiCameraSource& copy) {
	video = copy.video;
	fbo = copy.fbo;
	frame = copy.frame;
}

PiCameraSource::~PiCameraSource() {
}

void PiCameraSource::update() {
	// Do nothing, no update function for the pi camera.
}

bool PiCameraSource::isFrameNew() {
	return video.isReady();
}
		
ofPixels& PiCameraSource::getPixels() {
	// MUST BE CALLED IN THE DRAW FUNCTION!!
	fbo.begin();
	video.draw();
	fbo.end();
	fbo.readToPixels(frame);
	return frame;
}

int PiCameraSource::getWidth() {
	return video.getWidth();
}

int PiCameraSource::getHeight() {
	return video.getHeight();
}

void PiCameraSource::draw(int x, int y, int width, int height) {
	video.draw();
}

#endif