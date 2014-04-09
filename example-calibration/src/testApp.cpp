#include "testApp.h"

using namespace ofxCv;
using namespace cv;

const float diffThreshold = 2.5; // maximum amount of movement
const float timeThreshold = 1; // minimum time between snapshots
const int startCleaning = 10; // start cleaning outliers after this many samples

void testApp::setup() {
	ofSetVerticalSync(true);
    
#ifdef LIVE
	src.setup(1920, 1080, 30);
#endif
    
#ifdef MOVIE
    src.loadMovie("calibration.mov");
    src.play();
#endif
	
	FileStorage settings(ofToDataPath("settings.yml"), FileStorage::READ);
	if(settings.isOpened()) {
		int xCount = settings["xCount"], yCount = settings["yCount"];
		calibration.setPatternSize(xCount, yCount);
		float squareSize = settings["squareSize"];
		calibration.setSquareSize(squareSize);
		CalibrationPattern patternType;
		switch(settings["patternType"]) {
			case 0: patternType = CHESSBOARD; break;
			case 1: patternType = CIRCLES_GRID; break;
			case 2: patternType = ASYMMETRIC_CIRCLES_GRID; break;
		}
		calibration.setPatternType(patternType);
	}
	
    undistorted.allocate(1920, 1080, OF_IMAGE_GRAYSCALE);
	previous.allocate(1920, 1080, OF_IMAGE_GRAYSCALE);
	diff.allocate(1920, 1080, OF_IMAGE_GRAYSCALE);
	
	lastTime = 0;
	
	active = true;
}

void testApp::update() {
	if(src.update()) {
		Mat camMat = toCv(src.getGrayPixels());
		Mat prevMat = toCv(previous);
		Mat diffMat = toCv(diff);
		
		absdiff(prevMat, camMat, diffMat);	
		camMat.copyTo(prevMat);
		
		diffMean = mean(Mat(mean(diffMat)))[0];
		
		float curTime = ofGetElapsedTimef();
		if(active && curTime - lastTime > timeThreshold && diffMean < diffThreshold) {
			if(calibration.add(camMat)) {
				cout << "re-calibrating" << endl;
				calibration.calibrate();
				if(calibration.size() > startCleaning) {
					calibration.clean();
				}
				calibration.save("calibration.yml");
				lastTime = curTime;
			}
		}
		
		if(calibration.size() > 0) {
			calibration.undistort(toCv(src.getGrayPixels()), toCv(undistorted));
			undistorted.update();
		}
	}
}

void testApp::draw() {
	ofSetColor(255);
	src.getGrayTexture().draw(0, 0, 640, 480);
	undistorted.draw(640, 0, 640, 480);
	
	stringstream intrinsics;
	intrinsics << "fov: " << toOf(calibration.getDistortedIntrinsics().getFov()) << " distCoeffs: " << calibration.getDistCoeffs();
	drawHighlightString(intrinsics.str(), 10, 20, yellowPrint, ofColor(0));
	drawHighlightString("movement: " + ofToString(diffMean), 10, 40, cyanPrint);
	drawHighlightString("reproj error: " + ofToString(calibration.getReprojectionError()) + " from " + ofToString(calibration.size()), 10, 60, magentaPrint);
	for(int i = 0; i < calibration.size(); i++) {
		drawHighlightString(ofToString(i) + ": " + ofToString(calibration.getReprojectionError(i)), 10, 80 + 16 * i, magentaPrint);
	}
}

void testApp::keyPressed(int key) {
	if(key == ' ') {
		active = !active;
	}
}
