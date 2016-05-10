#pragma once

#include "ofMain.h"
#include "ofxFaust.h"

class ofApp : public ofBaseApp{

	public:

		void setup();
		void update();
		void draw();

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
    
        void audioOut(float * input, int bufferSize, int nChannels);
    
        ofSoundStream soundStream;
    
        const char* argv[64];
        string error;
        llvm_dsp_factory* factory;
        llvm_dsp* dsp;
        float* laudio;
        float* raudio;
        float** audio;
        float** shard_buf ;
        float* indv_buf;
        float * point;
//        ofRect* rects;
        ofxFaustGui Gui;
//        ofxFaust faust;
};
