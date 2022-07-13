#pragma once

#include "ofMain.h"
#include "ofxLeapMotion2.h"
#include "ofxThreadedImageLoaderProgress.h"
#include "ofxTween.h"

//#define TILE_SIZE 4

#define TILE_ROWS 10
#define TILE_COLS 15
#define TILE_ROWS_TEST 10
#define TILE_COLS_TEST 15

#define NUM_IMAGES 1

class ofApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();	
    void keyPressed  (int key);
    void exit();
    void drawHands();
    void checkHardwareCapabilities();
    void loadImages();
    void leapGestureEvent(GestureEventArgs& args);
    
	ofxLeapMotion leap;
	vector <ofxLeapMotionSimpleHand> simpleHands;
    
    unsigned int handsFound;
    ofCamera cam;
    float xt,yt,zt;

    ofImage img[NUM_IMAGES][TILE_ROWS][TILE_COLS];
    ofxThreadedImageLoader loader;
    int currentImage;

    bool bDebug;

    ofxTween tween_x;
    ofxTween tween_y;
    ofxTween tween_z;
    ofxEasingExpo 	easingexpo;

    //Zoom min-max variables
    float xmax;
    float xmin;
    float ymax;
    float ymin;

    unsigned int move;
    float zoom;

    float currentTime;

    bool bLandscapeOrientation;
};
