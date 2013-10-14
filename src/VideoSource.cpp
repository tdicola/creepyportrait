#include "VideoSource.h"

#include <iostream>

using namespace std;

VideoGrabberSource::VideoGrabberSource(int deviceId, int desiredWidth, int desiredHeight) {
	// Set up and initialize the video grabber.
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
	// Set up pi camera.
	video.setup(desiredWidth, desiredHeight, 60);
	// Initialize the frame buffer object used in getting video frames from
	// the camera.  This is necessary because the pi camera writes frames
	// directly to textures in GPU memory, but OpenGL ES doesn't allow reading
	// from textures in GPU memory.  The captured camera frame has to be
	// rendered to an off screen framebuffer object and then it can be read
	// on the CPU.
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
	// Note: Currently this MUST be called inside the OpenFrameworks application draw() function!
	// Because the camera frames are rendered to framebuffer objects, a default
	// shader which can render the camera frame needs to be loaded.  There are various
	// ways to do this manually, but the easiest option is to just use the default
	// shader from OpenFrameworks in the draw() function.

	// Enable framebuffer object rendering.
	fbo.begin();
	// Draw the latest camera frame.
	video.draw();
	// Stop rendering to the framebuffer and copy out the pixels to CPU memory.
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
	// TODO: The pi camera draw doesn't yet support rendering to a specific location
	// and height like the video grabber.  Add this support when it's implemented.
	video.draw();
}

#endif