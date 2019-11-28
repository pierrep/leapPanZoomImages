#pragma once

#include "ofMain.h"
#include "ofxLeapMotion2.h"
#include "ofxThreadedImageLoaderProgress.h"
#include "ofxTween.h"

#define TILE_SIZE 4
#define NUM_IMAGES 2

class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();	
    void keyPressed  (int key);
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

    ofImage img[NUM_IMAGES][TILE_SIZE][TILE_SIZE];
    ofxThreadedImageLoader loader;
    int currentImage;

    bool bDebug;

    ofxTween tween_x;
    ofxTween tween_y;
    ofxTween tween_z;
    ofxEasingExpo 	easingexpo;

    float currentTime;
};
