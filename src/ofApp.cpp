#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(5);
    
    audioAnalyzer.setup(4096, 2048, 44100);
    bUseSpectrum = true;
    
    
    //zooming
    boundingBox.set(0, 0, ofGetWidth(), ofGetHeight());
    currentBox.set(0, 0, ofGetWidth(), ofGetHeight());
    zoomToPoints = true;
    
    
//    loadSamplesAndBuildGUI();
//    loadSamplesAndRunTSNE();
//    loadTSNE("settings.xml");
    receiver.setup(PORT);
    
    
    star.load("dot.png");
    post.init(ofGetWidth(), ofGetHeight());
    post.createPass<BloomPass>()->setEnabled(true);
//    post.createPass<RimHighlightingPass>()->setEnabled(true);
    post.createPass<BloomPass>()->setEnabled(true);
    post.createPass<BloomPass>()->setEnabled(true);
    
    cam.setDistance(500);
    sphere.set(15, 20);
    
    ///gui
    loadSamplesButton.addListener(this, &ofApp::loadSamplesAndRunTSNE);
    loadSettingsButton.addListener(this, &ofApp::loadTSNE);
    saveSettingsButton.addListener(this, &ofApp::saveTSNE);
   
    gui.setup();
    gui.add(loadSamplesButton.setup("Load samples and run t-SNE"));
    gui.add(loadSettingsButton.setup("Load Settings"));
    gui.add(saveSettingsButton.setup("Save Settings"));
    showGui = true;
}


//--------------------------------------------------------------
void ofApp::update(){
    // if we are running our t-SNE manually, we need to run tsne.iterate() to
    // go through each iteration and collect the points where they currently are
    if (runManually) {
        tsnePoints = tsne.iterate();
        for (int i=0; i<testPoints.size(); i++) {
            testPoints[i].tsnePoint = ofPoint(tsnePoints[i][0], tsnePoints[i][1], tsnePoints[i][2]);
        }
    }
    
    
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
            playTSNE(x, y, n);
        }
    }
   
    //zooming
    checkBounds();
    
    for (int i = 0; i < pastSelectedPoints.size(); i++) {
        pastSelectedPoints[i].color.a-=2;
    }
    
    for (int i = pastSelectedPoints.size()-1; i >= 0; i--) {
        if (pastSelectedPoints[i].color.a < 5) {
            pastSelectedPoints.erase(pastSelectedPoints.begin());
        }
    }
    
//    for (int i = 0; i < testPoints.size(); i++){
//        if (testPoints[i].player.getPositionMS() > 100) {
//            testPoints[i].player.stop();
//        }
//    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    post.begin();
    beginzoom();
    
    ofSetRectMode(OF_RECTMODE_CENTER);
    drawTSNE();
    drawPastPoints();
    drawSelectedPoints();
    
    endzoom();
    post.end();
    
    if (showGui) gui.draw();
}

#pragma mark LOAD
//--------------------------------------------------------------
void ofApp::loadSamplesAndRunTSNE(){
    sampler.clear();
    tsnePoints.clear();
    data.clear();
    testPoints.clear();
    
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
    
    //setup TSNE vars
    int dims = 2;
    float perplexity = 40;
    float theta = 0.2;
    bool normalize = true;
    runManually = true;
    
    tsnePoints = tsne.run(data, dims, perplexity, theta, normalize, runManually);
    
    // if we didn't run manually, we can collect the points immediately
    if (!runManually) {
        for (int i=0; i<testPoints.size(); i++) {
            testPoints[i].tsnePoint = ofPoint(tsnePoints[i][0], tsnePoints[i][1]);
        }
    }

}


//--------------------------------------------------------------
//void ofApp::loadSamplesAndRunTSNE(){
//   //    loadSamples("OH", ofColor::red);
//    
//    vector<string> samplelibs;
//    
//    samplelibs.push_back("biolabsDubstepKit3");
//    samplelibs.push_back("medieval-perc");
//    samplelibs.push_back("Dirty");
//    samplelibs.push_back("analog-drum-kit");
//    samplelibs.push_back("nord-kit");
//    samplelibs.push_back("Djembe");
//    samplelibs.push_back("Djembe2");
//    samplelibs.push_back("Dun Dun & Conga");
//    samplelibs.push_back("Conga");
//    samplelibs.push_back("Darbuka");
//    samplelibs.push_back("Framedrum");
//    samplelibs.push_back("RIQ");
//    samplelibs.push_back("SteamKit");
//    samplelibs.push_back("Seaweed Glitch FX");
////    samplelibs.push_back("Micronesia");
//    samplelibs.push_back("MetalKit");
//    samplelibs.push_back("IndustrialKit");
//    samplelibs.push_back("HipHopKit");
//    samplelibs.push_back("Dubstep Drum Set");
//    samplelibs.push_back("cinematic hits");
//    samplelibs.push_back("CavedrumKit");
//    samplelibs.push_back("C-Bowl");
//    samplelibs.push_back("BreakbeatKit");
//    samplelibs.push_back("DirtyKicks");
//    samplelibs.push_back("LayeredSnares");
//    samplelibs.push_back("OneShotPercussionTools");
//    samplelibs.push_back("DirtyHats");
//    samplelibs.push_back("SuperFatSnares");
//    
//    
//    for (int i = 1; i <= 7; i++) {
//        samplelibs.push_back("dnb/Kit_" + ofToString(i));
//    }
//    
//    samplelibs.push_back("Marimba.cord.ff.stereo");
////    samplelibs.push_back("Marimba.yarn.ff.stereo");
////    samplelibs.push_back("Marimba.deadstroke.ff.stereo");
////    samplelibs.push_back("Marimba.roll.ff.stereo");
//    
//    samplelibs.push_back("Vibraphone.bow.stereo");
//    samplelibs.push_back("Vibraphone.dampen.ff.stereo");
//    samplelibs.push_back("Vibraphone.shortsustain.ff.stereo");
//    samplelibs.push_back("Vibraphone.sustain.ff.stereo");
//    
//    samplelibs.push_back("Xylophone.hardrubber.ff.stereo");
////    samplelibs.push_back("Xylophone.hardrubber.roll.ff.stereo");
//    samplelibs.push_back("Xylophone.rosewood.ff.stereo");
////    samplelibs.push_back("Xylophone.rosewood.roll.ff.stereo");
//    
////    samplelibs.push_back("DanElectro-Plectrum-4RR");
////    samplelibs.push_back("piano");
//    
//    float hueInc = 255. / samplelibs.size();
//    
//    for (int i = 0; i < samplelibs.size(); i++) {
//        loadSamples(samplelibs[i], ofColor::fromHsb(hueInc * i, 180, 255));
//    }
//    
//    //setup TSNE vars
//    int dims = 2;
//    float perplexity = 40;
//    float theta = 0.2;
//    bool normalize = true;
//    runManually = true;
//    
//    tsnePoints = tsne.run(data, dims, perplexity, theta, normalize, runManually);
//    
//    // if we didn't run manually, we can collect the points immediately
//    if (!runManually) {
//        for (int i=0; i<testPoints.size(); i++) {
//            testPoints[i].tsnePoint = ofPoint(tsnePoints[i][0], tsnePoints[i][1]);
//        }
//    }
// 
//}

//--------------------------------------------------------------
void ofApp::loadSamples(string path, ofColor color){
    ofDirectory samplesDir;
    samplesDir.listDir("samples/" + path);
    
    int leastSamples = INFINITY;
    int samplesLoaded = 0;
    
//    int zeroBuffers = 0;
//    int oneB
    for (int i = 0; i < samplesDir.size(); i++){
        string path = samplesDir.getPath(i);
        
        
        ofSoundPlayer sample;
        sample.setLoop(false);
        sample.load(path);
        
        vector<float> samples;
        loadAudioToData(path, samples);
        
        if (samples.size() < leastSamples) leastSamples = samples.size();
//        for (int j = 0; j < 8192; j++){
//            if (samples[j] != samples[j]) {
//                cout << "NAN! gotcha!!!" <<endl;
//            }
//        }
        if (samples.size() > 4096) {
        
            vector<float> point;
            
            for (int j = 0; j < 1; j++) {
            
                audioAnalyzer.inputBuffer.assign(samples.begin() + j*4096, samples.begin() + (j+1)*4096);
                audioAnalyzer.process(0);
            
                for (int k = 0; k < audioAnalyzer.spectrum.size(); k++) {
                    point.push_back(audioAnalyzer.spectrum[k]);
                    if (audioAnalyzer.spectrum[k] != audioAnalyzer.spectrum[k]) {
                        cout << "NAN! gotcha!!!" << endl;
                    }
                }
            }
            
            TestPoint testPoint;
            testPoint.path = path;
            testPoint.player = sample;
            testPoint.color = color;
            testPoint.point = point;
            testPoint.class_ = 0;
            
            testPoints.push_back(testPoint);
            data.push_back(point);
            samplesLoaded++;
        }
//        cout << point.size() << endl;
        
//        cout << "loaded and pushed back " + samplesDir.getPath(i) << endl;
    }
    
    cout << "Loaded " << samplesLoaded << "samples from "  << samplesDir.path() << "least samples " << leastSamples << endl;
    
}


//--------------------------------------------------------------
void ofApp::loadAudioToData(string fileName, vector < float > & audioSamples){
    
    SndfileHandle myf = SndfileHandle( ofToDataPath(fileName).c_str());
    
    //    printf ("Opened file '%s'\n", ofToDataPath(fileName).c_str()) ;
    //    printf ("  Sample rate : %d\n", myf.samplerate ()) ;
    //    printf ("  Channels  : %d\n", myf.channels ()) ;
    //    printf ("  Error   : %s\n", myf.strError());
    //    printf ("  Frames   : %d\n", int(myf.frames())); // frames is essentially samples
    //    puts("");
    
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
void ofApp::playTSNE(float x, float y, int numNearPts){
    if (testPoints.size() > 0){
        ofPoint pos(x / ofGetWidth(), y / ofGetHeight());
        vector<float> minDist;
        minDist.assign(numNearPts, INFINITY);
        vector<int> closest;
        closest.assign(numNearPts, -1);
        vector<float> vol;
        vol.assign(numNearPts, 0.0);
        
        selectedPoints.clear();
        for (int i = 0; i < testPoints.size(); i++) {
            float dist = pos.squareDistance(testPoints[i].tsnePoint);
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
                for (int k = closest.size()-1; k > flag; k--) {
                    //                    minDist[k] = minDist[k-1];
                    //                    closest[k] = closest[k-1];
                    iter_swap(minDist.begin() + k, minDist.begin() + k-1);
                    iter_swap(closest.begin() + k, closest.begin() + k-1);
                    //                    cout << "-------" << endl;
                }
                
                minDist[flag] = dist;
                closest[flag] = i;
            }
            
            
        }
        
        //calc sum of distances
        float sumDists = 0.;
        for (int i = 0; i < minDist.size(); i++) {
            sumDists += sqrt(minDist[i]);
        }
        
        //calc vol and play sound
        for (int i = 0; i < minDist.size(); i++) {
            float vol = sqrt(minDist[i]) / sumDists;
            testPoints[closest[i]].player.setVolume(vol * 0.5);
            testPoints[closest[i]].player.play();
        }
        
        
        //add points
        for (int i = 0; i < closest.size(); i++) {
            selectedPoints.push_back(testPoints[closest[i]].tsnePoint * ofPoint(ofGetWidth(), ofGetHeight()));
            
            playPoints.push_back(pos * ofPoint(ofGetWidth(), ofGetHeight()));
        }
        
        
        calcBoundingBox();
    }
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
            testPoint.addValue("x", testPoints[i].tsnePoint.x);
            testPoint.addValue("y", testPoints[i].tsnePoint.y);
            settings.addXml(testPoint);
        }
        
        string path = openFileResult.getName();
        settings.save("settings/" + path + ".xml");
        
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
        
        sampler.clear();
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
                testPoint.tsnePoint.x = settings.getValue<float>("x");
                testPoint.tsnePoint.y = settings.getValue<float>("y");
                testPoints.push_back(testPoint);
                cout << "loaded " << testPoint.path << endl;
            }
            
        }
        while( settings.setToSibling() );
    }
}


//--------------------------------------------------------------
void ofApp::checkBounds(){
    float src;
    float dst;
    float padding = 64;
    
    if (currentBox != boundingBox) {
        ofRectangle dangerzone = currentBox;
        dangerzone.x += padding;
        dangerzone.y += padding;
        dangerzone.width -= padding*2;
        dangerzone.height -= padding*2;
        
        bool outside = false;
        for (int i = 0; i < pastSelectedPoints.size(); i++) {
            if (!currentBox.inside(pastSelectedPoints[i].point)) outside = true;
        }
        
        if (outside){
            src = 0.97;
        }
        else {
            src = 0.99;
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
    
    for (int i = 0; i < selectedPoints.size(); i++) {
        addSelectedPoint(selectedPoints[i]);
    }
    
//    while (pastSelectedPoints.size() > selectedPoints.size() * 4) {
//        pastSelectedPoints.erase(pastSelectedPoints.begin());
//    }
    
    if (pastSelectedPoints.size() == 1) {
        w = 20./9.*16.;
        h = 20;
        x = pastSelectedPoints[0].point.x - w * 0.5;
        y = pastSelectedPoints[0].point.y - h * 0.5;
    }
    else {
        float minX = INFINITY;
        float minY = INFINITY;
        float maxX = 0;
        float maxY = 0;
        
        for (int i = 0; i < pastSelectedPoints.size(); i++) {
            if (pastSelectedPoints[i].point.x < minX) minX = pastSelectedPoints[i].point.x;
            if (pastSelectedPoints[i].point.y < minY) minY = pastSelectedPoints[i].point.y;
            if (pastSelectedPoints[i].point.x > maxX) maxX = pastSelectedPoints[i].point.x;
            if (pastSelectedPoints[i].point.y > maxY) maxY = pastSelectedPoints[i].point.y;
        }
        
        float xDiff = maxX - minX;
        float yDiff = maxY - minY;
        
        
        if (xDiff / 16. * 9. > yDiff) {
            w = xDiff;
            h = xDiff / 16. * 9.;
            x = minX;
            y = minY - (h - yDiff) * 0.5;
        }
        else {
            w = yDiff / 9. * 16.;
            h = yDiff;
            x = minX - (w - xDiff) * 0.5;
            y = minY;
        }
        
    }
    float padding = 64;
    
    x -= padding;
    y -= padding;
    w += padding*2;
    h += padding*2;
    boundingBox.set(x, y, w, h);
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

void ofApp::drawTSNE(){
    for (int i = 0; i < testPoints.size(); i++) {
        float x = ofGetWidth() * testPoints[i].tsnePoint.x;
        float y = ofGetHeight() * testPoints[i].tsnePoint.y;
        ofSetColor(testPoints[i].color, 100);
        star.draw(x, y, 15, 15);
//        ofDrawSphere(testPoints[i].tsnePoint * 1000, 5);
//        ofPushMatrix();
//        ofTranslate(testPoints[i].tsnePoint * 1000);
//        sphere.draw();
//        ofPopMatrix();
    }
}

void ofApp::drawSelectedPoints(){
    if (selectedPoints.size()) {
        ofSetColor(200);
        ofFill();
        for (int i = 0; i < selectedPoints.size(); i++){
            star.draw(selectedPoints[i], 30, 30);
//            ofDrawCircle(selectedPoints[i], 5);
        }
        ofNoFill();
        ofBeginShape();
        for (int i = 0; i < selectedPoints.size(); i++){
            ofVertex(selectedPoints[i]);
            
        }
        ofEndShape();
        selectedPoints.clear();
    }
}

void ofApp::drawPastPoints(){
    if (pastSelectedPoints.size() > 0) {
        for (int i = 0; i < pastSelectedPoints.size(); i++) {
            ofSetColor(pastSelectedPoints[i].color);
            star.draw(pastSelectedPoints[i].point, 20, 20);
            
        }
        ofNoFill();
        for (int i = 0; i < pastSelectedPoints.size()-1; i++){
            ofSetColor(pastSelectedPoints[i].color);
            ofDrawLine(pastSelectedPoints[i].point, pastSelectedPoints[i+1].point);
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == 's') saveTSNE();
    
    if (key == 'z') zoomToPoints ^= true;
    
    if (key == 'g') showGui ^= true;
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    playTSNE(x, y, 1);
    
//    ofPoint normPt;
//    normPt.x = (float)x / ofGetWidth();
//    normPt.y = (float)y / ofGetHeight();
//   
//    float shortestDist = 1.0;
//    int closest = -1;
//    float secondShortestDist = 1.0;
//    int secondClosest = -1;
//    
//    for (int i = 0; i < testPoints.size(); i++){
//        float dist =normPt.squareDistance(testPoints[i].tsnePoint);
//        if (dist < secondShortestDist) {
//            secondShortestDist = dist;
//            secondClosest = i;
//            
//            if (dist < shortestDist) {
//                secondShortestDist = shortestDist;
//                secondClosest = closest;
//                
//                shortestDist = dist;
//                closest = i;
//            }
//        }
//        
//        
//    }
//    
//    float vol1 = shortestDist / (shortestDist+secondShortestDist);
//    float vol2 = secondShortestDist / (shortestDist+secondShortestDist);
//    
//    testPoints[closest].player.setVolume(1.);
//    testPoints[closest].player.play();
////    testPoints[secondClosest].player.setVolume(vol2);
////    testPoints[secondClosest].player.play();
////    
//    
//    selectedPoints.push_back(testPoints[closest].tsnePoint * ofPoint(ofGetWidth(), ofGetHeight()));
//    selectedPoints.push_back(ofPoint(x,y));
////    selectedPoints.push_back(testPoints[secondClosest].tsnePoint * ofPoint(ofGetWidth(), ofGetHeight()));
////    
//    
//    
////    for (int i = 0; i < testPoints.size(); i++){
////        if (normPt.squareDistance(testPoints[i].tsnePoint) < 0.00004) {
////            testPoints[i].player.play();
////            break;
////        }
////    }
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    selectedPoints.clear();
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
//            point.push_back(audioAnalyzer.spectralFeatures["energy"]);
//            point.push_back(audioAnalyzer.spectralFeatures["hfc"]);
//            point.push_back(audioAnalyzer.spectralFeatures["complex"]);
//            point.push_back(audioAnalyzer.spectralFeatures["phase"]);
//            point.push_back(audioAnalyzer.spectralFeatures["specdiff"]);
//            point.push_back(audioAnalyzer.spectralFeatures["kl"]);
//            point.push_back(audioAnalyzer.spectralFeatures["mkl"]);
//            point.push_back(audioAnalyzer.spectralFeatures["specflux"]);
//            point.push_back(audioAnalyzer.spectralFeatures["centroid"]);
//            point.push_back(audioAnalyzer.spectralFeatures["spread"]);
//            point.push_back(audioAnalyzer.spectralFeatures["kurtosis"]);
//            point.push_back(audioAnalyzer.spectralFeatures["slope"]);
//            point.push_back(audioAnalyzer.spectralFeatures["decrease"]);
//            point.push_back(audioAnalyzer.spectralFeatures["rolloff"]);
