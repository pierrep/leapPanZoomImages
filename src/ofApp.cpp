#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    ofSetLogLevel(OF_LOG_NOTICE);

    leap.open();
    leap.setReceiveBackgroundFrames(true); // keep app receiving data from leap motion even when it's in the background
    leap.setupGestures();

    ofAddListener(leap.gestureEvent, this, &ofApp::leapGestureEvent);

    cam.enableOrtho();
    ofEnableDepthTest();

    checkHardwareCapabilities();

for(int rows =0; rows < NUM_IMAGES/4; rows++) {
    for(int cols =0; cols < NUM_IMAGES/4; cols++) {
        string filename = "tile_"+ofToString(rows+1)+"_"+ofToString(cols+1)+".jpg";
        loader.loadFromDisk(img[rows][cols], filename);
    }
}

    scaleFactor = 1.5;
    bDebug = true;
    currentTime = ofGetElapsedTimeMillis();
    xt = ofGetWidth() / 2;
    yt = ofGetHeight() / 2;
    zt = 0;
    tween_x.setParameters(4, easingexpo, ofxTween::easeOut, xt, xt, 0, 0);
    tween_y.setParameters(5, easingexpo, ofxTween::easeOut, yt, yt, 0, 0);
    tween_z.setParameters(6, easingexpo, ofxTween::easeOut, scaleFactor, scaleFactor, 0, 0);
}

//--------------------------------------------------------------
void ofApp::update()
{
    leap.updateGestures();

    vector<Hand> hands = leap.getLeapHands();
    if (leap.isFrameNew() && hands.size()) {
        handsFound = (unsigned int)hands.size();

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

    if (hands.size() == 2) {
        ofPoint hp1 = leap.getofPoint(hands[0].palmPosition());
        ofPoint hp2 = leap.getofPoint(hands[1].palmPosition());
        float distance = hp1.distance(hp2);

        if (distance < 120)
            scaleFactor += 0.01f;
        if (distance > 220)
            scaleFactor -= 0.01f;
        cout << "distance = " << distance << "  scale factor: " << scaleFactor << endl;

        if (scaleFactor > 1.5f)
            scaleFactor = 1.5f;
        if (scaleFactor < 0.1f)
            scaleFactor = 0.1f;
    }

    xt = tween_x.update();
    yt = tween_y.update();
    scaleFactor = tween_z.update();
    if (scaleFactor > 1.5f) {
        scaleFactor = 1.5f;
    }
    if (scaleFactor < 0.1f) {
        scaleFactor = 0.1f;
    }

    leap.markFrameAsOld();
}

//--------------------------------------------------------------
void ofApp::draw()
{
    ofBackgroundGradient(ofColor(90, 90, 90), ofColor(30, 30, 30), OF_GRADIENT_BAR);

    if (bDebug) {
        ofSetColor(200);
        ofDrawBitmapString("Leap Connected? " + ofToString(leap.isConnected()) + "\n Hands found? " + ofToString(handsFound), 20, 20);
        ofDrawBitmapString("", 20, 60);
    }

    if (loader.getProgress() < 1.0f) {
        ofDrawRectRounded(ofGetWidth() / 2 - ofGetWidth() / 4, ofGetHeight() / 2, 0, ofGetWidth() / 2, 20,5);
        ofSetColor(255, 0, 0);
        ofDrawRectRounded(ofGetWidth() / 2 - ofGetWidth() / 4 + 2, ofGetHeight() / 2 + 2, 1, loader.getProgress() * (ofGetWidth() / 2) - 4, 16,5);
    } else {

        ofPushMatrix();
        ofTranslate(xt, yt, zt);
        ofScale(scaleFactor, scaleFactor, scaleFactor);
        for(int rows = 0; rows < NUM_IMAGES/4;rows++){
            for(int cols = 0; cols < NUM_IMAGES/4;cols++){
                if (img[rows][cols].isAllocated()) {
                    //ofSetColor(255,0,0);
                    const float w = img[rows][cols].getWidth();
                    const float h = img[rows][cols].getHeight();
                    img[rows][cols].draw(-w*2 + rows*w, -h*2 + cols*h);
                }
            }
        }

        ofPopMatrix();
    }


    if (bDebug) {
        cam.begin();
        drawHands();
        cam.end();
    }
}

//--------------------------------------------------------------
void ofApp::leapGestureEvent(GestureEventArgs& args)
{
    unsigned int duration = 2000;
    unsigned int delay = 500;
    unsigned int move = 200;
    float zoom = 0.02f + (scaleFactor)*0.08f;

    if ((ofGetElapsedTimeMillis() - currentTime) < delay) {
        return;
    }

    if ((args.state == Leap::Gesture::STATE_START) || (args.state == Leap::Gesture::STATE_UPDATE)) {

        if (args.type == GestureType::SWIPE_RIGHT) {
            ofLogNotice() << "Swipe Right";
            tween_x.setParameters(1, easingexpo, ofxTween::easeOut, xt, xt - move, duration, 0);
            currentTime = ofGetElapsedTimeMillis();
        }
        if (args.type == GestureType::SWIPE_LEFT) {
            ofLogNotice() << "Swipe Left";
            tween_x.setParameters(2, easingexpo, ofxTween::easeOut, xt, xt + move, duration, 0);
            currentTime = ofGetElapsedTimeMillis();
        }
        if (args.type == GestureType::SWIPE_FORWARD) {
            ofLogNotice() << "Swipe Forward ";
            tween_y.setParameters(3, easingexpo, ofxTween::easeOut, yt, yt + move, duration, 0);
            currentTime = ofGetElapsedTimeMillis();
        }
        if (args.type == GestureType::SWIPE_BACK) {
            ofLogNotice() << "Swipe Back ";
            tween_y.setParameters(4, easingexpo, ofxTween::easeOut, yt, yt - move, duration, 0);
            currentTime = ofGetElapsedTimeMillis();
        }
        if (args.type == GestureType::CIRCLE_CLOCKWISE) {
            ofLogNotice() << "Circle Clockwise";
            tween_z.setParameters(5, easingexpo, ofxTween::easeOut, scaleFactor, scaleFactor + zoom, duration, 0);
            currentTime = ofGetElapsedTimeMillis();
        }
        if (args.type == GestureType::CIRCLE_ANTICLOCKWISE) {
            ofLogNotice() << "Circle Anti-clockwise";
            tween_z.setParameters(6, easingexpo, ofxTween::easeOut, scaleFactor, scaleFactor - zoom, duration, 0);
            currentTime = ofGetElapsedTimeMillis();
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
    if ((key == 'd') || (key == 'D')) {
        bDebug = !bDebug;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key)
{
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
    for (unsigned int i = 0; i < leapHands.size(); i++) {
        ofPoint handPos = leap.getMappedofPoint(leapHands[i].palmPosition());
        ofPoint handNormal = leap.getofPoint(leapHands[i].palmNormal());

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
    }
}

//--------------------------------------------------------------
void ofApp::checkHardwareCapabilities()
{
    int maxSize = 0;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxSize);
    ofLogNotice() << "Maximum texture size = " << maxSize;
}
