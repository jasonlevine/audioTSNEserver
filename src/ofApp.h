#pragma once

#include "ofMain.h"
#include "ofxAudioFeaturesChannel.h"
#include "sndfile.hh"
#include "ofxTSNE.h"
#include "ofxOsc.h"
#include "ofxGui.h"
#include "ofxAssignment.h"
#include "histogram.hpp"

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
    void exportTSNE();
    
    int getPitchFromFilename(string filename);
    
    void sortByPeaks();
    void sortByCentroid();
    void sortByPitch();
    
    void bakeCurve();
    ofPolyline bakedCurve;
    vector<ofPoint> curveNormals;
    bool baked = false;
    void unwrapTSNE();
    bool unwrapped = false;
    int padding = 40;

    //update/event funcs
    void stepTSNE();
    void processOSC();
    void checkBounds();
    void calcBoundingBox();
    void updatePastPoints();
    void updatePlayers();
    
    
    //play funcs
    void playTSNE(float x, float y, int numNearPts, float mainVol, int dur);
    bool isPlayable(int index);
    
     enum PointMode {
        TSNE2D,
        TSNE3D,
        UNWRAPPED
    };
    
    //draw funcs
    void beginzoom();
    void endzoom();
    void drawTSNE(float x, float y, float w, float h, PointMode pm);
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

    ofxAudioFeaturesChannel audioAnalyzer;

    struct TestPoint {
        int class_;
        ofColor color;
        vector<float> point;
        ofPoint tsnePoint2d;
        ofPoint tsnePoint3d;
        ofPoint unwrappedPoint;
        ofPoint screenPoint;
        string path;
        ofSoundPlayer player;
        float length;
        int duration;
        float pitch;
        float pitchConfidence;
        float centroid;
    };
    
    ofxTSNE tsne;
    vector<TestPoint> testPoints;

    
    vector<vector<float> > data;
    vector<vector<double> > tsnePoints;
    bool runManually;
    bool bUseSpectrum;
    
   
    enum DrawMode {
        LIBRARY,
        CENTROID,
        PITCH,
        CONFIDENCE
    };

    DrawMode drawMode = LIBRARY;
    
    histogram pitchHistogram;
    
    float minCentroid, maxCentroid;
    float minPitch, maxPitch;
    float minConfidence, maxConfidence;
 
    //osc
    ofxOscReceiver receiver;
    
    //point control
    struct pastPoint {
        ofPoint playPoint;
        vector<ofPoint> selectedPoints;
        float vol;
        ofColor color;
    };
    
    
    vector<ofPoint> selectedPoints;
    vector<int> selectedIndices;
    vector<pastPoint> pastSelectedPoints;
    
    
    void addSelectedPoint(ofPoint &playPt, vector<ofPoint> &selectedPts, float vol) {
         pastPoint pp;
         pp.playPoint = playPt * ofPoint(ofGetWidth(), ofGetHeight());
         pp.selectedPoints = selectedPts;
         pp.color.set(255);
         pp.vol = vol;
         pastSelectedPoints.push_back(pp);
//        cout << "vol = " << vol << endl;
     }
    
    
    ofRectangle boundingBox, currentBox;
    bool zoomToPoints;
    
   //graphcis
    ofImage star;
   //3d...
    ofEasyCam cam;
    ofSpherePrimitive sphere;
    
    //gui
    ofxPanel gui;
    ofxButton loadSamplesButton;
    ofxButton loadSettingsButton;
    ofxButton saveSettingsButton;
    ofxIntSlider pitchToDraw;
//    ofxFloatSlider minConfidence, maxConfidence;
    bool showGui;
    
  //pitch
    vector<int> playablePitches;
    vector<float> avgFft;
    vector<float> minFft;
    vector<float> maxFft;
    bool bDraAvgFft = false;
    float minLength = 0;
    float maxLength = 20;
   
    float tsneAngle;
    float volSnapShot;
    ofPoint playPointSnapShot;
    
    
    //string splitting code
    template<typename Out>
    void split(const std::string &s, char delim, Out result) {
        std::stringstream ss;
        ss.str(s);
        std::string item;
        while (std::getline(ss, item, delim)) {
            *(result++) = item;
        }
    }
    
    
    std::vector<std::string> split(const std::string &s, char delim) {
        std::vector<std::string> elems;
        split(s, delim, std::back_inserter(elems));
        return elems;
    }

    bool addPointsToCurve = false;
    ofPolyline bezierCurve;
    bool drawCurve = false;
    int selectedPt = -1;
    ofxAssignment solver;
    
    bool drawPointData = true;
    int selectedPoint = 0;
    
    bool bDebug = true;
    
    vector<int> pointsPerXHistogram;
    
    vector<ofPoint> pts;
    float normalDist = 100;
    
    bool isLeft(ofPoint a, ofPoint b, ofPoint c){
        return ((b.x - a.x)*(c.y - a.y) - (b.y - a.y)*(c.x - a.x)) > 0;
        //return sign((b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x));
    }

};
