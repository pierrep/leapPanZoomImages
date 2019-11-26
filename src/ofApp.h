#pragma once

#include "ofMain.h"
#include "ofxLeapMotion2.h"
#include "ofxThreadedImageLoader.h"
#include "ofxTween.h"

class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
	
    void keyPressed  (int key);
    void keyReleased(int key);
    void exit();
    void drawHands();
    void checkHardwareCapabilities();
    void leapGestureEvent(GestureEventArgs& args);
    
	ofxLeapMotion leap;
	vector <ofxLeapMotionSimpleHand> simpleHands;
    
    unsigned int handsFound;
    ofCamera cam;
    float scaleFactor;
    float xt,yt,zt;

    ofImage img;
    ofImage img1;
    ofImage img2;
    ofImage img3;
    ofImage img4;
    ofxThreadedImageLoader loader;

    bool bDebug;

    ofxTween tween_x;
    ofxTween tween_y;
    ofxTween tween_z;
    ofxEasingExpo 	easingexpo;

    float currentTime;
};
