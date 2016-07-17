#pragma once

#include "ofMain.h"
#include "ofxAudioFeaturesChannel.h"
#include "sndfile.hh"
#include "ofxTSNE.h"
#include "ofxOsc.h"
#include "ofxPostProcessing.h"
#include "ofxGui.h"

#define PORT 12345

class ofApp : public ofBaseApp{

public:
    void setup();
    void update();
    void draw();
    
    //load/save funcs
    void loadSamplesAndRunTSNE();
    void loadSamplesAndBuildGUI();
    void loadSamples(string path, ofColor color);
    void loadAudioToData(string fileName, vector < float > & audioSamples);
    void saveTSNE();
    void loadTSNE();
//    void saveTSNE(string path);
//    void loadTSNE(string path);
    //update/event funcs
    void checkBounds();
    void calcBoundingBox();
    void playTSNE(float x, float y, int numNearPts);
    
    //draw funcs
    void beginzoom();
    void endzoom();
    void drawTSNE();
    void drawSelectedPoints();
    void drawPastPoints();
    
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void mouseEntered(int x, int y);
    void mouseExited(int x, int y);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

    vector<ofSoundPlayer> sampler;
    ofxAudioFeaturesChannel audioAnalyzer;

    struct TestPoint {
        int class_;
        ofColor color;
        vector<float> point;
        ofPoint tsnePoint;
        string path;
        ofSoundPlayer player;
    };
    
    ofxTSNE tsne;
    vector<TestPoint> testPoints;
    vector<vector<float> > data;
    vector<vector<double> > tsnePoints;
    bool runManually;
    bool bUseSpectrum;
    
    
    //osc
    ofxOscReceiver receiver;
    
    //point control
    struct pastPoint {
        ofPoint point;
        ofColor color;
    };
    
    vector<ofPoint> playPoints;
    vector<pastPoint> pastPlayPoints;
    vector<ofPoint> selectedPoints;
    vector<pastPoint> pastSelectedPoints;
    
    void addSelectedPoint(ofPoint point) { pastPoint pp; pp.color.set(255, 255); pp.point = point; pastSelectedPoints.push_back(pp); }
    
//    vector<ofPoint> pointsToBound;
    
    ofRectangle boundingBox, currentBox;
    bool zoomToPoints;
    
   //graphcis
    ofImage star;
    ofxPostProcessing post;
   //3d...
    ofEasyCam cam;
    ofSpherePrimitive sphere;
    
    
    //gui
    ofxPanel gui;
    ofxButton loadSamplesButton;
    ofxButton loadSettingsButton;
    ofxButton saveSettingsButton;
    bool showGui;
    
    
    
};