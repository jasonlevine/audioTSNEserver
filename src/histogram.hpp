//
//  histogram.hpp
//  audioTSNE
//
//  Created by ArtCodeMusic on 2/20/18.
//

#pragma once
#include "ofMain.h"

class histogram {
private:
    vector<float> values;
    vector<int> indices;
    vector<vector<int>> bins;
    float min = INFINITY;
    float max = 0;
    int tallest = 0;
    
    
public:
//    void setup(int numBins, float min, float max) {
//        bins.clear();
//        bins.assign(numBins, 0);
//        this->min = min;
//        this->max = max;
//    }
    vector<testPoint>& testPoints;
    
    void addValue(int index){
        indices.push_back(index);
        float val = testPoints[index].pitch;
        values.push_back(val);
        if (val < min) min = floor(val);
        if (val > max) max = ceil(val);
    }
    
    void calculate() {
        bins.assign(max - min, 0);
        
        for (int i = 0; i < values.size(); i++){
            int index = floor(values[i]) - min;
            bins[index].push_back(indices[i]);
            if (bins[index].size() > tallest) tallest = bins[index].size();
        }
    }
    
    void draw(float x, float y, float w, float h){
        float binWidth = w / bins.size();
        for (int i = 0; i < bins.size(); i++){
            for (int j = 0; j < bins[i].size(); j++){
                float binHeight = (float)tallest / h;
                float binX = x + i * binWidth;
                float binY = h - j * binHeight;
                ofSetColor(testPoints[bins[i][j]])
                ofFill();
                ofDrawRectangle(binX, binY, binWidth, binHeight);
                ofSetColor(255);
                ofNoFill();
                ofDrawRectangle(binX, binY, binWidth, binHeight);
            }
        }
    }
};
