#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

void ofApp::setup() {
    distorted.load("distorted.jpg");
    
    calibration.loadLcp("5dmkii-28-105mm-jpg.lcp", 105);

    imitate(undistorted, distorted);
    
    Mat distortedMat = toCv(distorted);
    Mat undistortedMat = toCv(undistorted);
    calibration.undistort(distortedMat, undistortedMat);
    undistorted.update();
}

void ofApp::update() {
}

void ofApp::draw() {
    float scale = ofGetHeight() / distorted.getHeight();
    ofScale(scale, scale);
    distorted.draw(0, 0);
    if(ofGetKeyPressed('u')) {
        undistorted.draw(0, 0);
    }
}
