#include "ofApp.h"


//TODO:
// Unwarp tsne√ -> try assignment
// multi color modes with legends : sample lib, pitch, centroid, pitch confidence√
// tsne 3d with same random seed√
// put all samples in one folder
// create interface for selecting samples to be tsned (and excluding individual amples)
// polynomial regression to find curve
// export tsne as extempore sampler load commands!!!√
// remove osx dependencies
// monster code cleanup
// push to git!

#define BUFFER 4096
//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(25);
    ofSetCircleResolution(80);
    audioAnalyzer.setup(BUFFER, BUFFER / 2, 44100);
    bUseSpectrum = true;

    cam.setDistance(100);
    
    //zooming
    boundingBox.set(0, 0, ofGetWidth(), ofGetHeight());
    currentBox.set(0, 0, ofGetWidth(), ofGetHeight());
    zoomToPoints = true;
    
    receiver.setup(PORT);
    
    star.load("dot.png");
    
    ///gui
    loadSamplesButton.addListener(this, &ofApp::loadSamplesAndRunTSNE);
    loadSettingsButton.addListener(this, &ofApp::loadTSNE);
    saveSettingsButton.addListener(this, &ofApp::saveTSNE);
   
    gui.setup();
    gui.add(loadSamplesButton.setup("Load samples and run t-SNE"));
    gui.add(loadSettingsButton.setup("Load Settings"));
    gui.add(saveSettingsButton.setup("Save Settings"));
    gui.add(pitchToDraw.setup("Pitch to draw", 0, 0, 11));
//    gui.add(minConfidence.setup("min conf", 0.0, 0.0, 0.9));
//    gui.add(maxConfidence.setup("max conf", 1.0, 0.1, 1.0));
    showGui = false;
    
    for (int i = 0; i < 12; i++) playablePitches.push_back(i);
//    minConfidence = 0.0;
//    maxConfidence = 1.0;
}


//--------------------------------------------------------------
void ofApp::update(){
    stepTSNE();
    calcBoundingBox();
    checkBounds();
    updatePastPoints();
    updatePlayers();
    processOSC();
}

//--------------------------------------------------------------
void ofApp::draw(){
     ofBackground(27, 100, 105);
    ofSetLineWidth(2);
    
    pitchHistogram.draw(0,0,ofGetWidth(),ofGetHeight());
   
//    beginzoom();
//    ofSetRectMode(OF_RECTMODE_CENTER);
//    if (addPointsToCurve){
    
//    drawTSNE(padding,padding,ofGetWidth()/2-padding*2,ofGetHeight()-padding*2, TSNE2D);
//    drawTSNE(ofGetWidth()/2,0,ofGetWidth()/2,ofGetHeight(), UNWRAPPED);
//    ofSetColor(255);
//    ofNoFill();
//    ofDrawRectangle(0, 0, ofGetWidth()/2, ofGetHeight());
//    ofDrawRectangle(ofGetWidth()/2, 0, ofGetWidth()/2, ofGetHeight());
//    ofDrawLine(ofGetWidth()/2, ofGetHeight()/2, ofGetWidth(), ofGetHeight()/2);
//    }
//    else {
//        drawTSNE(0,0,ofGetWidth()/2,ofGetHeight(), TSNE2D);
//    }
//    drawPastPoints();
//    endzoom();
    ofSetColor(255);
  
    if (bDebug){
        ofNoFill();
        bezierCurve.draw();
        for (auto pt : pts) ofDrawCircle(pt, 8);
    }
    
//    if (testPoints.size() > 0) {
//        TestPoint t = testPoints[selectedPoint];
//        string data =
//        "path - " + ofToString(t.path) + "\n" +
//        "pitch - " + ofToString(t.pitch) + "\n" +
//        "confidence - " + ofToString(t.pitchConfidence) + "\n"
//        "centroid - " + ofToString(t.centroid) + "\n";
//
//        ofSetColor(255);
//        ofDrawBitmapString(data, ofGetWidth() - 500, 20);
//
//        string position = "(" + ofToString((float)mouseX / ofGetWidth() * 1000) + ", " + ofToString((float)mouseY / ofGetHeight() * 1000) + ")";
//        ofDrawBitmapString(position, mouseX, mouseY);
//    }
}

#pragma mark LOAD
//--------------------------------------------------------------
void ofApp::loadSamplesAndRunTSNE(){
   
    tsnePoints.clear();
    data.clear();
    testPoints.clear();
    
    avgFft.assign(2048, 0.0);
    minFft.assign(2048, 0.0);
    maxFft.assign(2048, 0.0);
    
    minCentroid = minPitch = minConfidence = INFINITY;
    maxCentroid = maxPitch = maxConfidence = 0;
    
    ofDirectory samplesDir;
    samplesDir.listDir("samples");
   
    vector<string> samplelibs;
    for (int i = 0; i < samplesDir.size(); i++) {
        samplelibs.push_back(samplesDir.getName(i));
    }
    
    float hueInc = 255. / samplelibs.size();
    
    for (int i = 0; i < samplelibs.size(); i++) {
        loadSamples(samplelibs[i], ofColor::fromHsb(hueInc * i, 180, 255));
    }
    
    for (int i = 0; i < testPoints.size(); i++){
        pitchHistogram.addValue(testPoints[i].pitch);
    }
    
    pitchHistogram.calculate();
    
    
    //normalize
//    for (int i = 0; i < data.size(); i++){
//        for (int j = 0; j < data[i].size(); j++){
//            data[i][j] = (data[i][j] - minFft[i]) / maxFft[i];
//            testPoints[i].point = data[i];
//        }
//    }
    
    //setup TSNE vars
//    int dims = 2;
//    float perplexity = 100;
//    float theta = 0.2;
//    bool normalize = true;
//    runManually = true;
//
//
//    float seed = ofGetElapsedTimef();
//    ofSeedRandom(seed);
//    tsnePoints = tsne.run(data, 2, perplexity, theta, normalize, false);
//    for (int i=0; i<testPoints.size(); i++) {
//        testPoints[i].tsnePoint2d = ofPoint(tsnePoints[i][0], tsnePoints[i][1]);
//        testPoints[i].screenPoint = testPoints[i].tsnePoint2d;
//    }
//    ofSeedRandom(seed);
//    tsnePoints = tsne.run(data, 3, perplexity, theta, normalize, false);
//    for (int i=0; i<testPoints.size(); i++) {
//        testPoints[i].tsnePoint3d = ofPoint(tsnePoints[i][0], tsnePoints[i][1], tsnePoints[i][2]);
//    }
//
}

//--------------------------------------------------------------
void ofApp::sortByPeaks(){
//    float minX = INFINITY;
//    float maxX = -INFINITY;
//    float minY = INFINITY;
//    float maxY = -INFINITY;
//    float minZ = INFINITY;
//    float maxZ = -INFINITY;
//    
//    for (auto &point : testPoints){
//        int highestPeak = -1;
//        int secondHighestPeak = -1;
//        float maxAmp = 0;
//        
//        for (int bin = 0; bin < point.point.size(); bin++){
//            float val = point.point[bin];
//            if (val > maxAmp) {
//                maxAmp = val;
//                secondHighestPeak = highestPeak;
//                highestPeak = bin;
//            }
//        }
//        
//        point.tsnePoint.x = pow((float)highestPeak / point.point.size(), 0.1);
//        point.tsnePoint.y = point.point[highestPeak];
//        point.player.setPosition(0.5f);
//        point.tsnePoint.z = 2 * point.player.getPositionMS();
//        point.player.setPosition(0);
////        point.tsnePoint.y = 1.0 - sqrt((float)secondHighestPeak / point.point.size());
//        
//        if (point.tsnePoint.x < minX) minX = point.tsnePoint.x;
//        if (point.tsnePoint.x > maxX) maxX = point.tsnePoint.x;
//        if (point.tsnePoint.y < minY) minY = point.tsnePoint.y;
//        if (point.tsnePoint.y > maxY) maxY = point.tsnePoint.y;
//        if (point.tsnePoint.z < minZ) minZ = point.tsnePoint.z;
//        if (point.tsnePoint.z > maxZ) maxZ = point.tsnePoint.z;
//    }
//    
//    //normalize
//    for (auto &point : testPoints){
//        point.tsnePoint.x = (point.tsnePoint.x - minX) / (maxX - minX);
//        point.tsnePoint.y = (point.tsnePoint.y - minY) / (maxY - minY);
//        point.tsnePoint.z = (point.tsnePoint.z - minZ) / (maxZ - minZ);
//    }
//    
//    
}

//--------------------------------------------------------------
void ofApp::sortByCentroid(){
//    float minCentroid = INFINITY;
//    float maxCentroid = -INFINITY;
//    float minConfidence = INFINITY;
//    float maxConfidence = -INFINITY;
//  //normalize
//    for (auto &point : testPoints){
//        if (point.centroid < minCentroid) minCentroid = point.centroid;
//        if (point.centroid > maxCentroid) maxCentroid = point.centroid;
//        if (point.pitchConfidence < minConfidence) minConfidence = point.pitchConfidence;
//        if (point.pitchConfidence > maxConfidence) maxConfidence = point.pitchConfidence;
//    }
//    
//    for (auto &point : testPoints){
//        point.tsnePoint.x = (point.centroid - minCentroid) / (maxCentroid - minCentroid);
//        point.tsnePoint.y = (point.pitchConfidence - minConfidence) / (maxConfidence - minConfidence);
//    }
}

////--------------------------------------------------------------
void ofApp::sortByPitch(){
//    float minPitch = INFINITY;
//    float maxPitch = -INFINITY;
//    float minConfidence = INFINITY;
//    float maxConfidence = -INFINITY;
////    float minLength = INFINITY;
////    float maxLength = -INFINITY;
//    
//    //normalize
//    for (auto &point : testPoints){
//        if (point.pitch < minPitch) minPitch = point.pitch;
//        if (point.pitch > maxPitch) maxPitch = point.pitch;
////        if (point.centroid < minCentroid) minCentroid = point.centroid;
////        if (point.centroid > maxCentroid) maxCentroid = point.centroid;
//        if (point.pitchConfidence < minConfidence) minConfidence = point.pitchConfidence;
//        if (point.pitchConfidence > maxConfidence) maxConfidence = point.pitchConfidence;
////        if (point.length < minLength) minLength = point.length;
////        if (point.length > maxLength) maxLength = point.length;
//    }
//    
//    for (auto &point : testPoints){
//        point.tsnePoint.x = (point.pitch - minPitch) / (maxPitch - minPitch);
////        point.tsnePoint.x = (point.centroid - minCentroid) / maxCentroid;
//        point.tsnePoint.y = (point.pitchConfidence - minConfidence) / (maxConfidence - minConfidence);
////        point.tsnePoint.z = (point.length - minLength) / maxLength;
////        point.tsnePoint -= 0.5;
//    }
}


//--------------------------------------------------------------
void ofApp::loadSamples(string path, ofColor color){
    ofDirectory samplesDir;
    samplesDir.listDir("samples/" + path);
    
    int leastSamples = INFINITY;
    int samplesLoaded = 0;
    int samplesNotLoaded = 0;
    
    for (int i = 0; i < samplesDir.size(); i++){
        string path = samplesDir.getPath(i);
        
        
        ofSoundPlayer sample;
        sample.setLoop(false);
        sample.load(path);
        
        vector<float> samples;
        loadAudioToData(path, samples);
        
        if (samples.size() < leastSamples) leastSamples = samples.size();

        if (samples.size() >= BUFFER) {
        
            vector<float> point;
            
            for (int j = 0; j < 1; j++) {
            
                audioAnalyzer.inputBuffer.assign(samples.begin() + j*BUFFER, samples.begin() + (j+1)*BUFFER);
                audioAnalyzer.process(0);
            
                for (int k = 0; k < audioAnalyzer.spectrum.size(); k++) {
                    float bin = audioAnalyzer.spectrum[k];
                    point.push_back(bin);
                    avgFft[k] += bin;
                    if (bin < minFft[k]) minFft[k] = bin;
                    if (bin > maxFft[k]) maxFft[k] = bin;
                    
                    
                    if (audioAnalyzer.spectrum[k] != audioAnalyzer.spectrum[k]) {
                        cout << "NAN! gotcha!!!" << endl;
                    }
                }
            }
            
            TestPoint testPoint;
            testPoint.path = path;
            testPoint.player = sample;
//            testPoint.length = testPoint.player.getLength() / 44100.0;
//            if (testPoint.length > 1) cout << path << " - " << testPoint.length << endl;

            testPoint.color = color;
            testPoint.point = point;
            testPoint.class_ = 0;
            testPoint.pitch = audioAnalyzer.pitch;
//            testPoint.pitch = (int)(audioAnalyzer.pitch - 0.5)%12;//getPitchFromFilename(samplesDir.getName(i));//
            testPoint.pitchConfidence = audioAnalyzer.pitchConfidence;
            testPoint.centroid = log(audioAnalyzer.spectralFeatures["centroid"]/440.0)/log(2) * 12 + 69;
            if (audioAnalyzer.pitch > pitchToDraw.getMax()) pitchToDraw.setMax(audioAnalyzer.pitch);
            
            if (testPoint.centroid > maxCentroid) maxCentroid = testPoint.centroid;
            if (testPoint.centroid < minCentroid) minCentroid = testPoint.centroid;
            if (testPoint.pitch > maxPitch) maxPitch = testPoint.pitch;
            if (testPoint.pitch < minPitch) minPitch = testPoint.pitch;
            if (testPoint.pitchConfidence > maxConfidence) maxConfidence = testPoint.pitchConfidence;
            if (testPoint.pitchConfidence < minConfidence) minConfidence = testPoint.pitchConfidence;
            
            
            if (testPoint.pitch > 0){
                testPoints.push_back(testPoint);
                data.push_back(point);
                samplesLoaded++;
            }
            else {
                samplesNotLoaded++;
                cout << path << "not loaded because pitch = 0" << endl;
            }
        }
        else {
            samplesNotLoaded++;
        }
    }
    
    cout << "Loaded " << samplesLoaded << "samples from "  << samplesDir.path() << "least samples " << leastSamples << endl;
    cout << samplesNotLoaded << " samples were not loaded because they were less than " << BUFFER << " samples or a pitch was not found" << endl;
    
}

//--------------------------------------------------------------
int ofApp::getPitchFromFilename(string filename) {
    vector<string> substr = split(filename, '.');
    string noteName = substr[3];
    
    for (int i = 0; i < 10; i++){
        int number = noteName.find(ofToString(i));
        if (number != -1) {
            noteName.erase(number);
        }
    }
    
    if (noteName == "C") return 0;
    if (noteName == "Db") return 1;
    if (noteName == "D") return 2;
    if (noteName == "Eb") return 3;
    if (noteName == "E") return 4;
    if (noteName == "F") return 5;
    if (noteName == "Gb") return 6;
    if (noteName == "G") return 7;
    if (noteName == "Ab") return 8;
    if (noteName == "A") return 9;
    if (noteName == "Bb") return 10;
    if (noteName == "B") return 11;
    
}

//--------------------------------------------------------------
void ofApp::loadAudioToData(string fileName, vector < float > & audioSamples){
    
    SndfileHandle myf = SndfileHandle( ofToDataPath(fileName).c_str());
    
    int nChannels = myf.channels();
    int nFrames = (int) myf.frames();
    
    if (nChannels != 1){
        vector < float > audioSamplesMultiChannel;
        audioSamples.resize(myf.frames());
        audioSamplesMultiChannel.resize(myf.frames() * myf.channels());
        myf.read(&audioSamplesMultiChannel[0], myf.frames() * myf.channels());
        for (int i = 0; i < myf.frames(); i++){
            audioSamples[i] = audioSamplesMultiChannel[i * nChannels];
        }
    } else {
        audioSamples.resize(myf.frames());
        myf.read(&audioSamples[0], myf.frames());
    }
}


//TODO: bering in scalable point selction code. fix algo

 //--------------------------------------------------------------
void ofApp::stepTSNE(){
//    if (runManually) {
//        tsnePoints = tsne.iterate();
//        for (int i=0; i<testPoints.size(); i++) {
//            ofPoint tsnePoint = ofPoint(tsnePoints[i][0], tsnePoints[i][1]);
//            testPoints[i].tsnePoint = tsnePoint;
//            testPoints[i].screenPoint = tsnePoint;
//        }
//    }
}


 //--------------------------------------------------------------
void ofApp::processOSC(){
     //get messages from extempore
    while(receiver.hasWaitingMessages()){
        // get the next message
        ofxOscMessage m;
        receiver.getNextMessage(m);
        
        // check for mouse moved message
        if(m.getAddress() == "/point"){
            float x = m.getArgAsFloat(0);
            float y = m.getArgAsFloat(1);
            int n = m.getArgAsInt(2);
            float vol = m.getArgAsFloat(3);
            int dur = m.getArgAsFloat(4);
           
            ofPoint pos = (bakedCurve[x] + curveNormals[x] * y) / ofPoint(ofGetWidth(), ofGetHeight());

            if (vol > 0.0 && n > 0 && dur > 0) playTSNE(pos.x, pos.y, n, vol, dur);
//            if (vol > 0.0 && n > 0 && dur > 0) playTSNE(x/1000.0, y/1000.0, n, vol, dur);
        }
        else if (m.getAddress() == "/chord"){
            
            playablePitches.clear();
            
            for (int i = 0; i < m.getNumArgs(); i++){
                playablePitches.push_back(m.getArgAsInt(i) % 12);
            }
            
            for (auto tsnePoint : testPoints) {
                bool playable;
                for (auto playablePitch : playablePitches) {
                    if ((int)(tsnePoint.pitch-0.5) % 12 == playablePitch) {
                        playable = true;
                        break;
                    }
                }
                if (!playable) tsnePoint.duration = 1; //this makes it fade out
            }
        }
        else if (m.getAddress() == "/confidence"){
            minConfidence = m.getArgAsFloat(0);
            maxConfidence = m.getArgAsFloat(1);
        }
        else if (m.getAddress() == "/length"){
            minLength = m.getArgAsFloat(0);
            maxLength = m.getArgAsFloat(1);
        }
    }
}


//--------------------------------------------------------------
void ofApp::updatePastPoints(){
    for (int i = 0; i < pastSelectedPoints.size(); i++) {
        pastSelectedPoints[i].color.a-=3;
    }
    
    for (int i = pastSelectedPoints.size()-1; i >= 0; i--) {
        if (pastSelectedPoints[i].color.a < 5) {
            pastSelectedPoints.erase(pastSelectedPoints.begin());
        }
    }
}

//--------------------------------------------------------------
void ofApp::updatePlayers(){
    for (int i = 0; i < testPoints.size(); i++){
        if (testPoints[i].duration > -1 &&
            testPoints[i].player.getPositionMS() > testPoints[i].duration) {
            float vol = testPoints[i].player.getVolume() * 0.75;
            if (vol < 0.01) {
                testPoints[i].player.stop();
            }
            else {
                testPoints[i].player.setVolume(testPoints[i].player.getVolume() * 0.75);
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::playTSNE(float x, float y, int numNearPts, float mainVol, int dur){
    if (testPoints.size() > 0 && playablePitches.size() > 0){
        float t = ofGetElapsedTimeMillis();
        ofPoint pos(x, y);
        vector<float> minDist;
        minDist.assign(numNearPts, INFINITY);
        vector<int> closest;
        closest.assign(numNearPts, -1);
        vector<float> vol;
        vol.assign(numNearPts, 0.0);
        vector<float> pan;
        pan.assign(numNearPts, 0.0);
        
        for (int i = 0; i < testPoints.size(); i++) {
            if (isPlayable(i))
            {
                float dist = pos.squareDistance(testPoints[i].screenPoint);
                int flag = -1;
                //for every potential closest point
                for (int j = closest.size()-1; j >= 0; j--) {
                    
                    // check if the distance is less than the current minimum. flag the point.
                    if (dist < minDist[j]) {
                        flag = j;
                    }
                }
                
                if (flag != -1){
                    //if so "trickle down" the current values from furthest point to current point
                    cout << "-----------"  << endl << "flag = " << flag << endl << "indices" << endl;
                    for (int k = closest.size()-1; k > flag; k--) {
                        cout << k << endl;
                        iter_swap(minDist.begin() + k, minDist.begin() + k-1);
                        iter_swap(closest.begin() + k, closest.begin() + k-1);
                    }
                   
                    cout << "reverse logiv" << endl;
                    int min = flag + 1;
                    int max = closest.size() - min - 1;
                    for (int k = 0; k < max; k++) {
                        int l = max - k + min;
                        cout << l << endl;
                    }
                    
                    minDist[flag] = dist;
                    closest[flag] = i;
                    pan[flag] = pos.x - testPoints[i].tsnePoint2d.x;
                }
            }
            
        }
        
        //calc sum of distances
        float sumDists = 0.;
        for (int i = 0; i < minDist.size(); i++) {
            sumDists += sqrt(minDist[i]);
        }
       
        //find max distance
        float maxXdist = 0;
        for (int i = 0; i < pan.size(); i++){
            if (abs(pan[i]) > maxXdist) maxXdist = abs(pan[i]);
        }
        
        
        
        //calc vol and play sound
        for (int i = 0; i < minDist.size(); i++) {
            float vol = minDist.size() > 1 ? (sqrt(minDist[minDist.size()-i-1]) / sumDists) : 1.0;
            vol *= ofMap(minDist.size(), 1, 20, 1, 10);
            testPoints[closest[i]].duration = dur;
            testPoints[closest[i]].player.setVolume(vol * mainVol);
            float panAmt = (testPoints[closest[i]].pitch - 16.) / 112.;
            testPoints[closest[i]].player.setPan(pan[i] / maxXdist * panAmt);
            testPoints[closest[i]].player.play();
        }
        cout << ofGetElapsedTimeMillis() - t << "ms to play " << numNearPts << "points out of " << testPoints.size() << " tsne points" << endl;
        
        selectedPoints.clear();
        selectedIndices.clear();
        //add points
        for (int i = 0; i < closest.size(); i++) {
            selectedIndices.push_back(closest[i]);
            selectedPoints.push_back(testPoints[closest[i]].screenPoint * ofPoint(ofGetWidth(), ofGetHeight()));
            ofPoint p = testPoints[closest[i]].screenPoint;
            ofColor c = testPoints[closest[i]].color;
            float r = 1;
        }
        
        addSelectedPoint(pos, selectedPoints, mainVol);
        playPointSnapShot = pos;
        
        if (numNearPts == 1) selectedPoint = closest[0];
    }
}

//--------------------------------------------------------------
bool ofApp::isPlayable(int index) {
    //false if out of confidence range
//    float confidence = testPoints[index].pitchConfidence;
//    if (confidence < minConfidence || confidence > maxConfidence) return false;
//    if (testPoints[index].length < minLength || testPoints[index].length > maxLength) {
//     return false;
//    }
    int pitch = int(testPoints[index].pitch - 0.5) % 12;
    //true if pitchcenter matches one of the playablepitches
    for (auto playable : playablePitches) {
//        int pitchCenter = testPoints[index].pitch;// int(testPoints[index].pitch - 0.5);
        if (pitch == playable) {
            return true;
        }
    }
    
    //false if not one of the playable pitches
    return false;
}

//--------------------------------------------------------------
void ofApp::saveTSNE(){
    //Open the Open File Dialog
    ofFileDialogResult openFileResult= ofSystemSaveDialog("settings", "Save your t-SNE!");
    
    //Check if the user opened a file
    if (openFileResult.bSuccess){
        
        ofLogVerbose("User selected a file");
        
        ofXml settings;
        settings.addChild("TSNE");
        settings.setTo("TSNE");
        
        for (int i = 0; i < testPoints.size(); i++) {
            ofXml testPoint;
            testPoint.addChild("testPoint");
            testPoint.setTo("testPoint");
            testPoint.addValue("path", testPoints[i].path);
            testPoint.addValue("color", testPoints[i].color);
            testPoint.addValue("x", testPoints[i].tsnePoint2d.x);
            testPoint.addValue("y", testPoints[i].tsnePoint2d.y);
            testPoint.addValue("pitch", testPoints[i].pitch);
            testPoint.addValue("pitchConfidence", testPoints[i].pitchConfidence);
            testPoint.addValue("length", testPoints[i].length);
            testPoint.addValue("centroid", testPoints[i].centroid);
            settings.addXml(testPoint);
        }
        
        string path = openFileResult.getName();
        settings.save(path + ".xml");
        
    }else {
        ofLogVerbose("User hit cancel");
    }
}

//--------------------------------------------------------------
void ofApp::loadTSNE(){
    //Open the Open File Dialog
    ofFileDialogResult openFileResult= ofSystemLoadDialog("Load a t-SNE!");
    
    //Check if the user opened a file
    if (openFileResult.bSuccess){

        tsnePoints.clear();
        data.clear();
        testPoints.clear();
    
        ofXml settings;
        settings.load(openFileResult.getPath());
        
        settings.setTo("TSNE");
        settings.setTo("testPoint[0]");
        
        
        do {
            if(settings.getName() == "testPoint")
            {
                TestPoint testPoint;
                testPoint.path = settings.getValue<string>("path");
                testPoint.player.load(testPoint.path);
                testPoint.color = settings.getValue<ofColor>("color");
                testPoint.tsnePoint2d.x = settings.getValue<float>("x");
                testPoint.tsnePoint2d.y = settings.getValue<float>("y");
                testPoint.screenPoint = testPoint.tsnePoint2d;
                testPoint.pitch = settings.getValue<float>("pitch");
                testPoint.pitchConfidence = settings.getValue<float>("pitchConfidence");
                testPoint.length = settings.getValue<float>("length");
                testPoint.centroid = settings.getValue<float>("centroid");
                testPoints.push_back(testPoint);
                cout << "loaded " << testPoint.path << " color: " << testPoint.color << endl;
            }
            
        }
        while( settings.setToSibling() );
    }
}

//--------------------------------------------------------------
void ofApp::exportTSNE(){
    ofFile tsnextm;
    tsnextm.open("/Users/artcodemusic/Code/extempore/tSNExtm/tsneSampler.xtm", ofFile::WriteOnly);
//    tsnextm << "(sys:load \"libs/external/instruments_ext.xtm\")";
    int index = 0;
    int bank = 0;
    
    for (int i = 0; i < testPoints.size(); i++){
//        tsnextm << "(set-sampler-index sampler " << "\"tSNExtm/" << testPoints[i].path << "\" " << index << " 0 0 " << bank << " 1)" << endl;
        
        tsnextm << "(set_tSNE_point " << i
            << " \"tSNExtm/" << testPoints[i].path << "\" "
            << testPoints[i].unwrappedPoint.x << " "
            << testPoints[i].unwrappedPoint.y << " "
            << testPoints[i].class_ << " "
            << testPoints[i].pitch << " "
            << ")" << endl;
        
        index++;
        if (index > 127){
            index = 0;
            bank++;
        }
    }
}

//--------------------------------------------------------------
void ofApp::checkBounds(){
    float src;
    float dst;
    float padding = 64;
    
    if (pastSelectedPoints.size() > 0 && currentBox != boundingBox) {
        
        ofRectangle dangerzone = currentBox;
        dangerzone.x += padding;
        dangerzone.y += padding;
        dangerzone.width -= padding*2;
        dangerzone.height -= padding*2;
        
        bool outside = false;
        for (int i = 0; i < pastSelectedPoints.size(); i++) {
            if (!currentBox.inside(pastSelectedPoints[i].playPoint)) outside = true;
        }
        
        if (outside){
            src = 0.95;
        }
        else {
            src = 0.996;
        }
        
        dst = 1. - src;
        
        currentBox.x = currentBox.x * src + boundingBox.x * dst;
        currentBox.y = currentBox.y * src + boundingBox.y * dst;
        currentBox.width = currentBox.width * src + boundingBox.width * dst;
        currentBox.height = currentBox.height * src + boundingBox.height * dst;
    }
}

//TODO: move to class. feed only center points leave bigger boundaries
//--------------------------------------------------------------
void ofApp::calcBoundingBox(){
    float x, y, w, h;
    
    if (selectedPoints.size() == 0) {
        boundingBox.set(0, 0, ofGetWidth(), ofGetHeight());
    }
    else {
        float minX = INFINITY;
        float minY = INFINITY;
        float maxX = 0;
        float maxY = 0;
       
        //Zoom to pastselected points
        for (int i = 0; i < pastSelectedPoints.size(); i++) {
            if (pastSelectedPoints[i].playPoint.x < minX) minX = pastSelectedPoints[i].playPoint.x;
            if (pastSelectedPoints[i].playPoint.y < minY) minY = pastSelectedPoints[i].playPoint.y;
            if (pastSelectedPoints[i].playPoint.x > maxX) maxX = pastSelectedPoints[i].playPoint.x;
            if (pastSelectedPoints[i].playPoint.y > maxY) maxY = pastSelectedPoints[i].playPoint.y;
        }
        
        float xDiff = maxX - minX;
        float yDiff = maxY - minY;
        
        if (xDiff / ofGetWidth() * ofGetHeight() > yDiff) {
            w = xDiff;
            h = xDiff / ofGetWidth() * ofGetHeight();
            x = minX;
            y = minY - (h - yDiff) * 0.5;
        }
        else {
            w = yDiff / ofGetHeight() * ofGetWidth();
            h = yDiff;
            x = minX - (w - xDiff) * 0.5;
            y = minY;
        }
        
        float padding = 192;
        
        x -= padding;
        y -= padding;
        w += padding*2;
        h += padding*2;
        boundingBox.set(x, y, w, h);
    }

}


void ofApp::bakeCurve(){
//    float startX = pts[0].x;
//    float endX = pts.back().x;
//    
//    for (int x = startX; x < endX; x++){
//        bakedCurve.addVertex(x, interpolate(x));
//    }
    
    bakedCurve = bezierCurve.getResampledByCount(251);
    
    curveNormals.clear();
    for (int i = 0; i < 1000; i++) {
        ofPoint normal = bakedCurve[i+1] - bakedCurve[i];
        normal.normalize();
        normal.rotate(90, ofPoint(0,0,1));
        curveNormals.push_back(normal);
    }
    
    
  //  baked = true;
}

void ofApp::unwrapTSNE() {
//    pointsPerXHistogram.assign(1000, 0);
    
    for (int i = 0; i < testPoints.size(); i++){
        float minDist = INFINITY;
        float closest = -1;
        bool leftOfCurve;
        
        ofPoint tsnePoint2d = testPoints[i].tsnePoint2d * ofPoint(ofGetWidth()/2-padding*2, ofGetHeight()-padding*2) + ofPoint(padding, padding);
        for (int j = 0; j < bakedCurve.size()-1; j++){
            float dist = bakedCurve[j].squareDistance(tsnePoint2d);
//             float dist = testPoints[i].tsnePoint2d.squareDistance((bakedCurve[j]) / ofPoint(ofGetWidth()/2-padding*2, ofGetHeight()-padding*2));// - ofPoint(padding, padding)
            if (dist < minDist){
                minDist = dist;
                closest = j;
                
            }
        }
        

        minDist = sqrt(minDist)/ofGetHeight();
        testPoints[i].unwrappedPoint.x = closest / 250.;
        testPoints[i].unwrappedPoint.y = 0.5;
        
        leftOfCurve = isLeft(bakedCurve[closest], bakedCurve[closest+1], tsnePoint2d);
        if (leftOfCurve) {
            testPoints[i].unwrappedPoint.y += minDist;
        }
        else{
            testPoints[i].unwrappedPoint.y -= minDist;
        }
        
        testPoints[i].screenPoint = testPoints[i].unwrappedPoint;
    }
    
    
   // unwrapped = true;
    
    
}

#pragma mark DRAWROUTINES

void ofApp::beginzoom(){
    if (zoomToPoints){
        ofPushMatrix();
        ofTranslate(ofGetWidth()*0.5, ofGetHeight()*0.5);
        float scale = ofGetWidth() / currentBox.width;
        ofScale(scale, scale);
        float xTranslate = -(currentBox.x + currentBox.width * 0.5);
        float yTranslate = -(currentBox.y + currentBox.height * 0.5);
        
        ofTranslate(xTranslate, yTranslate);
    }
}

void ofApp::endzoom(){
    if (zoomToPoints) {
        ofPopMatrix();
    }
}

void ofApp::drawTSNE(float x, float y, float w, float h, PointMode pm){
    if (pm == TSNE3D) cam.begin();
    for (int i = 0; i < testPoints.size(); i++) {
        ofPoint p;
        if (pm == TSNE3D){
            p = testPoints[i].tsnePoint3d;
        }
        else {
            if (pm == TSNE2D) p = testPoints[i].tsnePoint2d;
            if (pm == UNWRAPPED) p = testPoints[i].unwrappedPoint;
            p.x = p.x * w + x;
            p.y = p.y * h + y;
        }
        
        ofColor col;
        float brightness = 0;
        switch (drawMode) {
            case LIBRARY:
                col = testPoints[i].color;
                break;
                
            case CENTROID:
                brightness = (testPoints[i].centroid - minCentroid) / (maxCentroid - minCentroid) * 255;
                col.setHsb(0.0, 127, brightness);
            
                break;
                
            case PITCH:
                brightness = (testPoints[i].pitch - minPitch) / (maxPitch - minPitch) * 255;
                col.setHsb(50, 50, brightness);
                break;
                
            case CONFIDENCE:
                col = (testPoints[i].pitchConfidence - minConfidence) / (maxConfidence - minConfidence) * 255;
                break;
            default:
                break;
        }

        ofSetColor(col, isPlayable(i) ? 255 : 50);
        if (pm == TSNE3D) ofDrawSphere(p, 3);
        else {
            ofFill();
            ofDrawCircle(p, 4);
        }
    }
    if (pm == TSNE3D) cam.end();
}

void ofApp::drawSelectedPoints(){
    if (selectedIndices.size()) {
        ofSetLineWidth(volSnapShot * 8);
        
        for (int i = 0; i < selectedIndices.size(); i++){
            TestPoint tsnePoint = testPoints[selectedIndices[i]];
            ofPoint pt = tsnePoint.tsnePoint2d * ofPoint(ofGetWidth(), ofGetHeight());
            ofDrawLine(playPointSnapShot, pt);
            star.draw(pt, 100 * volSnapShot, 100 * volSnapShot);
        }
    }
}


void ofApp::drawPastPoints(){
    if (pastSelectedPoints.size() > 0) {
    ofSetLineWidth(3);
        for (auto psp : pastSelectedPoints){
            ofSetColor(psp.color);
            
            for (auto sp : psp.selectedPoints){
                ofDrawLine(psp.playPoint, sp);
            }
            
            for (auto sp : psp.selectedPoints){
                star.draw(sp, 100 * psp.vol, 100 * psp.vol);
            }
        }

    ofSetLineWidth(2);
        for (int i = 0; i < pastSelectedPoints.size()-1; i++){
            ofSetColor(pastSelectedPoints[i].color);
            ofDrawLine(pastSelectedPoints[i].playPoint, pastSelectedPoints[i+1].playPoint);
        }
    }
}

#pragma mark EVENTS

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == 's') saveTSNE();
    if (key == 'l') loadTSNE();
    
    if (key == 'z') zoomToPoints ^= true;
    
    if (key == 'g') showGui ^= true;
    
    if (key == 'f') ofToggleFullscreen();
    
    if (key >= 49 && key <= 57) {
        playTSNE((float)mouseX/ofGetWidth(), (float)mouseY/ofGetHeight(), key - 48, 1.0, -1);
    }
    
    if (key == 't') loadSamplesAndRunTSNE();
    
    if (key == 'r'){
        runManually = false;
        for (int i = 0; i < testPoints.size(); i++){
            ofPoint p = testPoints[i].tsnePoint2d;
            p -= ofPoint(0.5,0.5);
            p.rotate(1, ofPoint(0,0,1));
            p += ofPoint(0.5,0.5);
            testPoints[i].tsnePoint2d = p;
            testPoints[i].screenPoint = p;
        }
        tsneAngle++;
        cout << tsneAngle << endl;
      //  rotating = true;
    }
    if (key == 'R'){
        runManually = false;
        for (int i = 0; i < testPoints.size(); i++){
            ofPoint p = testPoints[i].tsnePoint2d;
            p -= ofPoint(0.5,0.5);
            p.rotate(-1, ofPoint(0,0,1));
            p += ofPoint(0.5,0.5);
            testPoints[i].tsnePoint2d = p;
            testPoints[i].screenPoint = p;
        }
        tsneAngle++;
        cout << tsneAngle << endl;
        //  rotating = true;
    }
    
   
    
    if (key == 'p'){
        addPointsToCurve ^= true;
    }
    
    if (key == 'L') drawMode = LIBRARY;
    if (key == 'C') drawMode = CENTROID;
    if (key == 'P') drawMode = PITCH;
    if (key == 'O') drawMode = CONFIDENCE;
        

    if (key == 'b'){
        bakeCurve();
    }
    
    if (key == 'u'){
        if (baked) unwrapTSNE();
    }
    
    if (key == 'e'){
        exportTSNE();
    }
    
    if (key == 'a'){
        vector<ofVec2f> tsneVec2f;
        for (int i = 0; i < testPoints.size(); i++){
            tsneVec2f.push_back(testPoints[i].unwrappedPoint);
        }
        
        vector<ofVec2f> grid;
        for (int y = 0; y < 8; y++) {
            for (int x = 0; x < 103; x++) {
                grid.push_back(ofPoint(x,y));
            }
        }
        
        vector<ofVec2f> solvedGrid;
        solvedGrid = solver.match(tsneVec2f, grid, false);
    }
    
    if (key == '.') normalDist += 10;
    if (key == ',') normalDist -= 10;
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    if (selectedPt != -1){
        pts[selectedPt].x = x;
        pts[selectedPt].y = y;
        bezierCurve.clear();
        bakedCurve.clear();
        for (auto pt : pts){
            bezierCurve.curveTo(pt);
        }
        if (pts.size() > 3){
            bakeCurve();
            unwrapTSNE();
        }
    }
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    if (addPointsToCurve){
        if (button == 0){
            for (int i = 0; i < pts.size(); i++) {
                if (pts[i].distance(ofPoint(x,y)) < 5){
                    selectedPt = i;
                }
            }
            if (selectedPt == -1){
                pts.push_back(ofPoint(x,y));
                bezierCurve.curveTo(x, y);
                
                if (pts.size() > 3){
                    bakeCurve();
                    unwrapTSNE();
                }
            }
        }
        else {
            bezierCurve.clear();
            bakedCurve.clear();
            pts.clear();
        }
    }
    else {
        playTSNE((float)x/ofGetWidth(), (float)y/ofGetHeight(), 1, 1.0, -1);
    }
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    selectedPt = -1;
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}


