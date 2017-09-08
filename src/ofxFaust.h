/************************************************************************
 FAUST Architecture File
 Copyright (C) 2003-2011 GRAME, Centre National de Creation Musicale
 ---------------------------------------------------------------------
 This Architecture section is free software; you can redistribute it
 and/or modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 3 of
 the License, or (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; If not, see <http://www.gnu.org/licenses/>.
 
 EXCEPTION : As a special exception, you may create a larger work
 that contains this FAUST architecture section and distribute
 that work under terms of your choice, so long as this FAUST
 architecture section is not modified.
 
 
 ************************************************************************
 ************************************************************************/
using namespace std;


#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif
#include "faust/gui/meta.h"
#include "faust/gui/UI.h"
#include "faust/dsp/dsp.h"
#include "ofSoundBuffer.h"
#include "ofLog.h"
#include "ofxFaustUI.h"
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <assert.h>

class ofxFaust{
private:
    
    dsp* fDSP;
    
    ofSoundBuffer ofBuf;

protected:
    
    int samplerate;
    int bufsize;
    float** inbuf;
    float** outbuf;
    //process faust dsp code

    
//    virtual ofSoundBuffer* faustbuf2ofbuf(FAUSTFLOAT** inbuf,channels){
//        FAUSTFLOAT* outbuf;
//        for(int i=0;i<bufsize)
//            return
//            }
//    virtual FAUSTFLOAT** ofbuf2faustbuf(ofSoundBuffer* inbuf){
//        FAUSTFLOAT** outbuf;
//        for(int ch=0;ch< inbuf->getNumChannels();ch++){
//            for(int i=0;i < inbuf->getNumFrames();i++){
//                outbuf[ch][i] = inbuf->getSample(i,ch);
//            }
//        }
//        return outbuf;
//        
//    }
    virtual void checkInputs(ofSoundBuffer &ofinbuf){
        if(fDSP->getNumInputs()==0){
            ofLogVerbose() <<"Faust Code has No Input. Input Buffer will be ignored.";
        }
        if(ofinbuf.getNumChannels() > fDSP->getNumInputs()){
            ofLogVerbose() <<"Hardware Input Channels exceeded faust inputs. Input Channels will be truncated.";
        }else if(ofinbuf.getNumChannels() < fDSP->getNumInputs()){
        ofLogVerbose() <<"faust inputs exceeded Hardware Input Channels. Exceeded inputs will be filled with 0.";
        }else{
        }
        
            }
public:
    ofxFaust(dsp* mydsp){
        fDSP = mydsp;
        bufsize = 256;
        samplerate = 48000;
        inbuf = new float*[fDSP->getNumInputs()];
        outbuf = new float*[fDSP->getNumOutputs()];
        for(int i=0;i<fDSP->getNumInputs();i++){
            inbuf[i] = new float[bufsize];
        }
        for(int i=0;i<fDSP->getNumOutputs();i++){
            outbuf[i] = new float[bufsize];
        }
    }
    ofxFaust(int size,int samplerate){
        bufsize = size;
        samplerate = ofBuf.getSampleRate();
    }
    virtual ~ofxFaust(){
        delete inbuf,outbuf;
    }
    
    virtual void init(ofSoundBuffer &ofinbuf){
       checkInputs(ofinbuf);
        fDSP->init(samplerate);
    }
    virtual void buildUI(ofxFaustUI &UI){
        fDSP->buildUserInterface(&UI);
    }
    
    virtual void process(ofSoundBuffer &input,ofSoundBuffer &output){
        
        for(int ch=0;ch< fDSP->getNumInputs();ch++){
            if (ch<input.getNumChannels()){
                for(int i=0;i < input.getNumFrames();i++){
                    inbuf[ch][i] = input.getSample(i,ch);
                }
            }else{
                for(int i=0;i < input.getNumFrames();i++){
                    inbuf[ch][i] = 0;
                }
            }
            
            fDSP->compute(bufsize,inbuf,outbuf);
            for(int ch=0;ch< output.getNumChannels();ch++){
                if (ch<fDSP->getNumOutputs()){
                    ofBuf.copyFrom(outbuf[ch],bufsize,1,samplerate);
                }else{
                    for (int i=0; i<output.getNumFrames(); i++) {
                        ofBuf[i*output.getNumChannels()+ch] = 0;
                    }
                }
                
            }
        }

}
};


