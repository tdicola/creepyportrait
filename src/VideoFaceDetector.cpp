#include "VideoFaceDetector.h"

using namespace std;

VideoFaceDetector::VideoFaceDetector() {
	finder.setup("haarcascade_frontalface_default.xml");	
}

void VideoFaceDetector::setBufferSize(int size) {
	faceBuffer.resize(size);
}

void VideoFaceDetector::updateFrame(ofPixels& frame) {
	// Run Haar detection algorithm.
	finder.findHaarObjects(frame);
	// Find the face with the largest area
	auto largestBlob = max_element(begin(finder.blobs), end(finder.blobs), [](ofxCvBlob& b1, ofxCvBlob& b2) {
		return b1.area < b2.area;
	});
	// Drop the oldest item in the buffer of recent face detections.
	faceBuffer.pop_back();
	// Add the most recent detection to the buffer
	if (largestBlob != end(finder.blobs)) {
		// Found a face
		faceBuffer.push_front(make_shared<ofxCvBlob>(*largestBlob));
	}
	else
	{
		// Did not find a face
		faceBuffer.push_front(nullptr);
	}
	// Update detected face
	currentFace = getCurrentFace();
}

bool VideoFaceDetector::isFaceDetected() {
	return currentFace != nullptr;
}

ofRectangle VideoFaceDetector::getDetectedFace() {
	return *currentFace;
}

shared_ptr<ofRectangle> VideoFaceDetector::getCurrentFace() {
	// Remove noise from recently detected faces to find the average face center.
	// Return nullptr if no face center found.

	// Copy out detected (non-null) faces.
	vector<shared_ptr<ofxCvBlob> > faces;
	copy_if(begin(faceBuffer), end(faceBuffer), back_inserter(faces), [](shared_ptr<ofxCvBlob> blob) { 
		return blob != nullptr;
	});
	// Fail to detect a face if more than half the measurements were empty.
	if (faces.size() < faceBuffer.size()/2) {
		return nullptr;
	}
	// If only one measurement, return it.
	else if (faces.size() == 1) {
		return make_shared<ofRectangle>(faces[0]->boundingRect);
	}
	// Find the cluster center (average face center).
	ofVec2f meanPoint = accumulate(begin(faces), end(faces), ofVec2f(0,0), [](ofVec2f sum, shared_ptr<ofxCvBlob> blob) {
		return sum + ofVec2f(blob->centroid.x, blob->centroid.y);
	}) / float(faces.size());
	// Calculate distances from each face center to the cluster center.
	vector<float> distances;
	transform(begin(faces), end(faces), back_inserter(distances), [meanPoint](shared_ptr<ofxCvBlob> blob) {
		return meanPoint.distance(ofVec2f(blob->centroid.x, blob->centroid.y));
	});
	// Calculate average and std. deviation of distances.
	float avgDistance = accumulate(begin(distances), end(distances), 0.0) / float(distances.size());
	float stddevDistance = sqrt(accumulate(begin(distances), end(distances), 0.0, [avgDistance](float sum, float distance) {
		return sum + pow(distance - avgDistance, 2.0);
	}));
	// Calculate average face rect for faces that are within 3 standard deviations distance to the cluster center.
	ofRectangle faceRect(0,0,0,0);
	float faceCount = 0;
	for (unsigned int i = 0; i < faces.size(); ++i) {
		if (distances[i] < 3.0*stddevDistance) {
			faceCount += 1;
			faceRect.x += faces[i]->boundingRect.x;
			faceRect.y += faces[i]->boundingRect.y;
			faceRect.width += faces[i]->boundingRect.width;
			faceRect.height += faces[i]->boundingRect.height;
		}
	}
	// Fail to detect a face if there's too much noise
	if (faceCount == 0) {
		return nullptr;
	}
	// Return the average rectangle for non-noisy points
	faceRect.x /= faceCount;
	faceRect.y /= faceCount;
	faceRect.width /= faceCount;
	faceRect.height /= faceCount;
	return make_shared<ofRectangle>(faceRect);
}