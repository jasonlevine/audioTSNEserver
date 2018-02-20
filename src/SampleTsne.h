//
//  SampleTsne.hpp
//  audioTSNE
//
//  Created by ArtCodeMusic on 2/20/18.
//

#pragma once
#include "ofMain.h"
#include "ofxAudioFeaturesChannel.h"
#include "sndfile.hh"
#include "ofxTSNE.h"

struct SamplePoint {
    //audio data
    string path;
    ofSoundPlayer player;
    //audio features
    vector<float> point;
    float length;
    int duration;
    float pitch;
    float pitchConfidence;
    float centroid;
    //points
    int class_;  //used?
    ofColor color;
    ofPoint tsnePoint2d;
    ofPoint tsnePoint3d;
    ofPoint unwrappedPoint;
    ofPoint screenPoint;
};

enum PointMode {
    TSNE2D,
    TSNE3D,
    UNWRAPPED
};

enum DrawMode {
    LIBRARY,
    CENTROID,
    PITCH,
    CONFIDENCE
};

class SampleTsne {
private:
    ofxAudioFeaturesChannel audioAnalyzer;
    ofxTSNE tsne;
    
    vector<vector<float>> data;
    vector<vector<double>> tsnePoints;
    vector<SamplePoint> samplePoints;

    float minCentroid, maxCentroid;
    float minPitch, maxPitch;
    float minConfidence, maxConfidence;
    
    vector<int> playablePitches;
    DrawMode drawMode = LIBRARY;
    
public:
    //load/save funcs
    void loadSamplesAndRunTSNE();
    void loadSamplesAndBuildGUI(); //used?
    void loadSamples(string path, ofColor color);
    void loadAudioToData(string fileName, vector < float > & audioSamples);
    void saveTSNE();
    void loadTSNE();
    void exportTSNE();
    int getPitchFromFilename(string filename);
    
    //play funcs
    void playTSNE(float x, float y, int numNearPts, float mainVol, int dur);
    bool isPlayable(int index);
    
    //draw funcs
    void drawTSNE(float x, float y, float w, float h, PointMode pm);
};
