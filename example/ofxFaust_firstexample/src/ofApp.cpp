#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    string src = "process = _,vgroup(\"voo\",hgroup(\"foo\",hslider(\"gain\",0.8,0,1,0.01))): *;";
    
    factory = createDSPFactoryFromFile(ofToDataPath("UITester.dsp"),0,argv,"",error,0);
    dsp = createDSPInstance(factory);
    dsp->init(48000);
    dsp->buildUserInterface(&Gui);
    ofBackground(20,20,20);
    soundStream.printDeviceList();
    ofSetRectMode(OF_RECTMODE_CORNER);
    
    cout << soundStream.setup(this,2, 2, 44100, 256, 4)<<endl;
//    ofSoundStreamStart();
    ofSetFrameRate(30);
    laudio = new float[soundStream.getBufferSize()]();
//    for(int i ; i<soundStream.getBufferSize();i++){
//        laudio[i]=i;
//    }
    cout <<"buffersize"<< soundStream.getBufferSize() <<endl;
    raudio = new float[soundStream.getBufferSize()]();
    audio = new float*[32]();
//    Rects = new ofRect[soundStream.getBufferSize()];
    shard_buf = new float*[32];
    for(int i=0;i<32;i++){
        shard_buf[i] = new float[256];
        audio[i] = new float[256];
    }
    
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    Gui.draw();
    ofSetColor(255,20,20);
    ofFill();
    for(int i = 0; i<soundStream.getBufferSize();i++ ){
        ofDrawRectangle(i*3,ofGetHeight()/3-laudio[i]*60,2,2+laudio[i]*60);
    }
    ofSetColor(20,255,20);
    ofFill();
    for(int i = 0; i<soundStream.getBufferSize();i++ ){
        ofDrawRectangle(i*3,ofGetHeight()*2/3-raudio[i]*60,2,2+raudio[i]*60);
    }
    ofDrawRectangle(mouseX,mouseY,10,10);
}
//-----------------
void ofApp::audioOut(float * output, int bufferSize, int nChannels){
    
    audio[0] = laudio;
    audio[1] = raudio;
    dsp->compute(bufferSize,shard_buf,audio);
    for(int i=0 ; i<bufferSize ;i++){
        output[i*2]   = laudio[i];
        output[i*2+1] = raudio[i];
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

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

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

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



