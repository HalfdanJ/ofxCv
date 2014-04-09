#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxBlackMagic.h"

#define LIVE
//#define MOVIE

class testApp : public ofBaseApp {
public:
	void setup();
	void update();
	void draw();
	void keyPressed(int key);
	
#ifdef LIVE
	//ofVideoGrabber src;
    ofxBlackMagic src;
#endif
    
#ifdef MOVIE
    ofVideoPlayer src;
#endif
    
	ofImage undistorted;
	ofPixels previous;
	ofPixels diff;
	float diffMean;
	
	float lastTime;
	bool active;
	
	ofxCv::Calibration calibration;
};
