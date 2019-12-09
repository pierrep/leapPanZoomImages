#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    ofSetLogLevel(OF_LOG_NOTICE);
    ofEnableAntiAliasing();
    ofHideCursor();

    leap.open();
    leap.setReceiveBackgroundFrames(true); // keep app receiving data from leap motion even when it's in the background
    leap.setupGestures();

    ofAddListener(leap.gestureEvent, this, &ofApp::leapGestureEvent);

    //cam.enableOrtho();
    cam.setNearClip(0.1f);
    cam.setFarClip(35000.0f);
    cam.lookAt(glm::vec3(0,0,0));

    ofLogNotice() << "Far clip: " << cam.getFarClip() << "  Near clip: " << cam.getNearClip();
    //ofEnableDepthTest();

    checkHardwareCapabilities();

    for (int i = 0; i < NUM_IMAGES; i++) {
        for (int rows = 0; rows < TILE_SIZE; rows++) {
            for (int cols = 0; cols < TILE_SIZE; cols++) {
                string filename = "image" + ofToString(i + 1) + "/tile_" + ofToString(rows + 1) + "_" + ofToString(cols + 1) + ".jpg";
                loader.loadFromDisk(img[i][rows][cols], filename);
            }
        }
    }
    currentImage = 0;
    scaleFactor = 1;
    bDebug = true;
    currentTime = ofGetElapsedTimeMillis();
    xt = ofGetWidth() / 2;
    yt = -10000;
    zt = -10000;
    tween_x.setParameters(4, easingexpo, ofxTween::easeOut, xt, xt, 0, 0);
    tween_y.setParameters(5, easingexpo, ofxTween::easeOut, yt, yt, 0, 0);
    tween_z.setParameters(6, easingexpo, ofxTween::easeOut, zt, zt, 0, 0);
}

//--------------------------------------------------------------
void ofApp::update()
{
    leap.updateGestures();

    vector<Hand> hands = leap.getLeapHands();
    if (leap.isFrameNew() && hands.size()) {
        handsFound = static_cast<unsigned int>(hands.size());

        leap.setMappingX(-230, 230, -ofGetWidth() / 2, ofGetWidth() / 2);
        leap.setMappingY(90, 490, -ofGetHeight() / 2, ofGetHeight() / 2);
        leap.setMappingZ(-150, 150, -200, 200);

        for (unsigned i = 0; i < hands.size(); i++) {
            ofPoint handPos;
            handPos = leap.getofPoint(hands[i].palmPosition());

            for (int j = 0; j < 5; j++) {
                ofPoint pt;
                const Finger& finger = hands[i].fingers()[ofxLeapMotion::fingerTypes[j]];

                pt = leap.getMappedofPoint(finger.tipPosition());
                pt = leap.getMappedofPoint(finger.jointPosition(finger.JOINT_DIP));
            }
        }
    } else {
        handsFound = 0;
    }

    if (loader.getProgress() >= 1.0f) {
        xt = tween_x.update();
        yt = tween_y.update();
        zt = tween_z.update();
        if (zt > -700.0) {
            zt = -700.0f;
        }
        if (zt < -28000) {
            zt = -28000;
        }

        float xmax = ofMap(zt,-28000,-700,500,29000);
        float xmin = ofMap(zt,-700,-28000,-29000,-2000);
        float ymax = ofMap(zt,-28000,-700,-5000, 9500);
        float ymin = ofMap(zt,-700,-28000,-30000,-15000);

        if(xt > xmax) xt = xmax;
        if(xt < xmin) xt = xmin;
        if(yt > ymax) yt = ymax;
        if(yt < ymin) yt = ymin;

        ofLogNotice() << "xmin: " << xmin << " xmax: " << xmax << " ymin:" << ymin << " ymax: " << ymax;
    }

    leap.markFrameAsOld();
}

//--------------------------------------------------------------
void ofApp::draw()
{
    ofSetWindowTitle(ofToString(ofGetFrameRate()));
    ofBackgroundGradient(ofColor(90, 90, 90), ofColor(30, 30, 30), OF_GRADIENT_BAR);

    if (loader.getProgress() < 1.0f) {
        loader.draw();
        if (loader.getProgress() >= 0.95f) {
            bDebug = false;
        }
    } else {        
        cam.begin();
        ofPushMatrix();
        ofTranslate(xt, yt, zt);
        ofScale(scaleFactor, scaleFactor, scaleFactor);
        ofSetColor(255);
        for (int rows = 0; rows < TILE_SIZE; rows++) {

            for (int cols = 0; cols < TILE_SIZE; cols++) {
                if (img[currentImage][rows][cols].isAllocated()) {                    
                    //if(rows % 2 == 0) ofSetColor(0,0,255);
                    //else ofSetColor(255,0,0);
                    const float w = img[currentImage][cols][rows].getWidth();
                    const float h = img[currentImage][cols][rows].getHeight();
                    img[currentImage][cols][rows].draw(-w * 2 + cols * w, h * 2 - rows * h);
                }
            }
        }

        ofPopMatrix();
        cam.end();
    }

    if (bDebug) {
        cam.begin();
        drawHands();
        cam.end();
        ofSetColor(0,255,0);
        string s = "xt: " + ofToString(xt) + " yt: " + ofToString(yt) + " zt:" + ofToString(zt);
        ofDrawBitmapString(s,20,ofGetHeight()-20);
        ofDrawBitmapString(ofToString(ofGetFrameRate()),ofGetWidth()-50,20);
        ofSetColor(200);
        ofDrawBitmapString("Leap Connected? " + ofToString(leap.isConnected()) + "\n Hands found? " + ofToString(handsFound), 20, 20);
        ofDrawBitmapString("", 20, 60);
        ofSetColor(255);
    }
}

//--------------------------------------------------------------
void ofApp::leapGestureEvent(GestureEventArgs& args)
{
    unsigned int duration = 2000;
    unsigned int delay = 500;
    unsigned int move = 400 + fabs(zt*0.1f);
    float zoom = 400 + fabs(zt*0.1f);//0.02f + (scaleFactor)*0.08f;

    if ((ofGetElapsedTimeMillis() - currentTime) < delay) {
        return;
    }

    if ((args.state == Leap::Gesture::STATE_START) || (args.state == Leap::Gesture::STATE_UPDATE)) {

        if (args.type == GestureType::SWIPE_RIGHT) {
            ofLogNotice() << "Swipe Right";
            tween_x.setParameters(1, easingexpo, ofxTween::easeOut, xt, xt + move, duration, 0);
            currentTime = ofGetElapsedTimeMillis();
        }
        if (args.type == GestureType::SWIPE_LEFT) {
            ofLogNotice() << "Swipe Left";
            tween_x.setParameters(2, easingexpo, ofxTween::easeOut, xt, xt - move, duration, 0);
            currentTime = ofGetElapsedTimeMillis();
        }
        if (args.type == GestureType::SWIPE_FORWARD) {
            ofLogNotice() << "Swipe Forward ";
            tween_y.setParameters(3, easingexpo, ofxTween::easeOut, yt, yt - move, duration, 0);
            currentTime = ofGetElapsedTimeMillis();
        }
        if (args.type == GestureType::SWIPE_BACK) {
            ofLogNotice() << "Swipe Back ";
            tween_y.setParameters(4, easingexpo, ofxTween::easeOut, yt, yt + move, duration, 0);
            currentTime = ofGetElapsedTimeMillis();
        }
        if (args.type == GestureType::CIRCLE_CLOCKWISE) {
            ofLogNotice() << "Circle Clockwise zt = " << zt;
            tween_z.setParameters(5, easingexpo, ofxTween::easeOut, zt, zt + zoom, duration, 0);
            currentTime = ofGetElapsedTimeMillis();
        }
        if (args.type == GestureType::CIRCLE_ANTICLOCKWISE) {
            ofLogNotice() << "Circle Anti-clockwise zt = " << zt;
            tween_z.setParameters(6, easingexpo, ofxTween::easeOut, zt, zt - zoom, duration, 0);
            currentTime = ofGetElapsedTimeMillis();
        }

        ofLogNotice() << "xt: "<< xt << " yt: " << yt << " zt: " << zt;
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
    if ((key == 'd') || (key == 'D')) {
        bDebug = !bDebug;
    }
    if (key == ' ') {
        currentImage = !currentImage;
    }
}

//--------------------------------------------------------------
void ofApp::exit()
{
    leap.close();
}

//--------------------------------------------------------------
void ofApp::drawHands()
{
    vector<Hand> leapHands = leap.getLeapHands();
    ofPushStyle();
    for (unsigned int i = 0; i < leapHands.size(); i++) {
        ofPoint handPos = leap.getMappedofPoint(leapHands[i].palmPosition());
        ofPoint handNormal = leap.getofPoint(leapHands[i].palmNormal());

        ofPushStyle();
        ofSetColor(0, 0, 255);
        ofDrawSphere(handPos.x, handPos.y, handPos.z, 20);
        ofSetColor(255, 255, 0);
        ofDrawArrow(handPos, handPos + 100 * handNormal);

        for (int f = 0; f < 5; f++) {
            const Finger& finger = leapHands[i].fingers()[ofxLeapMotion::fingerTypes[f]];
            ofPoint dip = leap.getMappedofPoint(finger.jointPosition(finger.JOINT_DIP));
            ofPoint mcp = leap.getMappedofPoint(finger.jointPosition(finger.JOINT_MCP));
            ofPoint pip = leap.getMappedofPoint(finger.jointPosition(finger.JOINT_PIP));
            ofPoint tip = leap.getMappedofPoint(finger.jointPosition(finger.JOINT_TIP));

            ofSetColor(0, 255, 0);
            ofDrawSphere(mcp.x, mcp.y, mcp.z, 12);
            ofDrawSphere(pip.x, pip.y, pip.z, 12);
            ofDrawSphere(dip.x, dip.y, dip.z, 12);
            ofDrawSphere(tip.x, tip.y, tip.z, 12);

            ofSetColor(255, 0, 0);
            ofSetLineWidth(20);
            ofDrawLine(mcp.x, mcp.y, mcp.z, pip.x, pip.y, pip.z);
            ofDrawLine(pip.x, pip.y, pip.z, dip.x, dip.y, dip.z);
            ofDrawLine(dip.x, dip.y, dip.z, tip.x, tip.y, tip.z);
        }
        ofPopStyle();
    }
    ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::checkHardwareCapabilities()
{
    int maxSize = 0;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxSize);
    ofLogNotice() << "Maximum texture size = " << maxSize;
}
