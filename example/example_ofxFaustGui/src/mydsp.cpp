/* ------------------------------------------------------------
name: "sample"
Code generated with Faust 2.3.14 (http://faust.grame.fr)
Compilation options: -scal -ftz 0------------------------------------------------------------ */

#ifndef  __mydsp_H__
#define  __mydsp_H__

#include "faust/gui/meta.h"
#include "faust/gui/UI.h"
#include "faust/dsp/dsp.h"
#include "faust/misc.h"
#include "faust/audio/ofaudio-dsp.h"

#include "ofxGui.h"
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <assert.h>

//Intrinsic


//DSP class

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif  

#include <math.h>

float mydsp_faustpower2_f(float value) {
	return (value * value);
	
}

#ifndef FAUSTCLASS 
#define FAUSTCLASS mydsp
#endif

class mydsp : public dsp {
	
 private:
	
	FAUSTFLOAT fCheckbox0;
	FAUSTFLOAT fVslider0;
	FAUSTFLOAT fHslider0;
	int fSamplingFreq;
	float fConst0;
	FAUSTFLOAT fHslider1;
	FAUSTFLOAT fCheckbox1;
	float fRec0[3];
	float fRec1[3];
	
 public:
	
	void metadata(Meta* m) { 
		m->declare("basics.lib/name", "Faust Basic Element Library");
		m->declare("basics.lib/version", "0.0");
		m->declare("filters.lib/name", "Faust Filters Library");
		m->declare("filters.lib/version", "0.0");
		m->declare("maths.lib/author", "GRAME");
		m->declare("maths.lib/copyright", "GRAME");
		m->declare("maths.lib/license", "LGPL with exception");
		m->declare("maths.lib/name", "Faust Math Library");
		m->declare("maths.lib/version", "2.1");
		m->declare("name", "sample");
	}

	virtual int getNumInputs() {
		return 2;
		
	}
	virtual int getNumOutputs() {
		return 2;
		
	}
	virtual int getInputRate(int channel) {
		int rate;
		switch (channel) {
			case 0: {
				rate = 1;
				break;
			}
			case 1: {
				rate = 1;
				break;
			}
			default: {
				rate = -1;
				break;
			}
			
		}
		return rate;
		
	}
	virtual int getOutputRate(int channel) {
		int rate;
		switch (channel) {
			case 0: {
				rate = 1;
				break;
			}
			case 1: {
				rate = 1;
				break;
			}
			default: {
				rate = -1;
				break;
			}
			
		}
		return rate;
		
	}
	
	static void classInit(int samplingFreq) {
		
	}
	
	virtual void instanceConstants(int samplingFreq) {
		fSamplingFreq = samplingFreq;
		fConst0 = (3.14159274f / min(192000.0f, max(1.0f, float(fSamplingFreq))));
		
	}
	
	virtual void instanceResetUserInterface() {
		fCheckbox0 = FAUSTFLOAT(0.0f);
		fVslider0 = FAUSTFLOAT(-20.0f);
		fHslider0 = FAUSTFLOAT(2.0f);
		fHslider1 = FAUSTFLOAT(1000.0f);
		fCheckbox1 = FAUSTFLOAT(0.0f);
		
	}
	
	virtual void instanceClear() {
		for (int l0 = 0; (l0 < 3); l0 = (l0 + 1)) {
			fRec0[l0] = 0.0f;
			
		}
		for (int l1 = 0; (l1 < 3); l1 = (l1 + 1)) {
			fRec1[l1] = 0.0f;
			
		}
		
	}
	
	virtual void init(int samplingFreq) {
		classInit(samplingFreq);
		instanceInit(samplingFreq);
	}
	virtual void instanceInit(int samplingFreq) {
		instanceConstants(samplingFreq);
		instanceResetUserInterface();
		instanceClear();
	}
	
	virtual mydsp* clone() {
		return new mydsp();
	}
	virtual int getSampleRate() {
		return fSamplingFreq;
		
	}
	
	virtual void buildUserInterface(UI* ui_interface) {
		ui_interface->openVerticalBox("sample");
		ui_interface->openHorizontalBox("filter");
		ui_interface->addHorizontalSlider("cutoff_freq", &fHslider1, 1000.0f, 1.0f, 20000.0f, 0.00999999978f);
		ui_interface->addHorizontalSlider("q", &fHslider0, 2.0f, 0.100000001f, 6.0f, 0.00999999978f);
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("master");
		ui_interface->addCheckButton("mute", &fCheckbox0);
		ui_interface->addVerticalSlider("volume", &fVslider0, -20.0f, -70.0f, 6.0f, 0.00999999978f);
		ui_interface->closeBox();
		ui_interface->openHorizontalBox("space");
		ui_interface->addCheckButton("LRswap", &fCheckbox1);
		ui_interface->closeBox();
		ui_interface->closeBox();
		
	}
	
	virtual void compute(int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) {
		FAUSTFLOAT* input0 = inputs[0];
		FAUSTFLOAT* input1 = inputs[1];
		FAUSTFLOAT* output0 = outputs[0];
		FAUSTFLOAT* output1 = outputs[1];
		float fSlow0 = (1.0f / float(fHslider0));
		float fSlow1 = tanf((fConst0 * mydsp_faustpower2_f(float(fHslider1))));
		float fSlow2 = (1.0f / fSlow1);
		float fSlow3 = (((fSlow0 + fSlow2) / fSlow1) + 1.0f);
		float fSlow4 = ((float(fCheckbox0) * powf(10.0f, (0.0500000007f * float(fVslider0)))) / fSlow3);
		float fSlow5 = float(fCheckbox1);
		int iSlow6 = int(fSlow5);
		float fSlow7 = (1.0f / fSlow3);
		float fSlow8 = (((fSlow2 - fSlow0) / fSlow1) + 1.0f);
		float fSlow9 = (2.0f * (1.0f - (1.0f / mydsp_faustpower2_f(fSlow1))));
		float fSlow10 = (0.0f - fSlow2);
		int iSlow11 = (fSlow5 == 0.0f);
		for (int i = 0; (i < count); i = (i + 1)) {
			float fTemp0 = float(input0[i]);
			float fTemp1 = float(input1[i]);
			fRec0[0] = ((iSlow6?fTemp1:fTemp0) - (fSlow7 * ((fSlow8 * fRec0[2]) + (fSlow9 * fRec0[1]))));
			output0[i] = FAUSTFLOAT((fSlow4 * ((fSlow2 * fRec0[0]) + (fSlow10 * fRec0[2]))));
			fRec1[0] = ((iSlow11?fTemp1:fTemp0) - (fSlow7 * ((fSlow9 * fRec1[1]) + (fSlow8 * fRec1[2]))));
			output1[i] = FAUSTFLOAT((fSlow4 * ((fSlow2 * fRec1[0]) + (fSlow10 * fRec1[2]))));
			fRec0[2] = fRec0[1];
			fRec0[1] = fRec0[0];
			fRec1[2] = fRec1[1];
			fRec1[1] = fRec1[0];
			
		}
		
	}

	
};

class ofxFaustUIObject {

protected:

    std::string fLabel;
    FAUSTFLOAT* fZone;

    FAUSTFLOAT range(FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT val) {return (val < min) ? min : (val > max) ? max : val;}


public:

    ofxFaustUIObject(const std::string& label, FAUSTFLOAT* zone):fLabel(label),fZone(zone) {}
    virtual ~ofxFaustUIObject() {}

    virtual void setValue(FAUSTFLOAT f) { *fZone = range(0.0, 1.0, f); }
    virtual FAUSTFLOAT getValue() { return *fZone; }
    virtual void toString(char* buffer) {}
    virtual std::string getName() { return fLabel; }

};

/*--------------------------------------------------------------------------*/
class ofxFaustCheckButton : public ofxFaustUIObject {
private:

    virtual void valueChanged(bool &b){
        *fZone = (float)b;
    }
public:
    ofxToggle* toggle;

    ofxFaustCheckButton(const std::string& label, FAUSTFLOAT* zone):ofxFaustUIObject(label,zone) {
        toggle = new ofxToggle();
        toggle = toggle->setup(label,*fZone);
        toggle->addListener(this,&ofxFaustCheckButton::valueChanged);
    }

    virtual ~ofxFaustCheckButton() {
        delete toggle;
    }
        void toString(char* buffer)
    {
        snprintf(buffer, 256, "CheckButton(float): %s", fLabel.c_str());
    }
};

/*--------------------------------------------------------------------------*/
class ofxFaustButton : public ofxFaustUIObject {
private:

    virtual void valueChanged(){
        *fZone = (float)*button;
    }

public:
    ofxButton* button;

    ofxFaustButton(const std::string& label, FAUSTFLOAT* zone):ofxFaustUIObject(label, zone) {
        button = new ofxButton();
        button = button->setup(label);
        button->addListener(this,&ofxFaustButton::valueChanged);
    }
    virtual ~ofxFaustButton() {
        delete button;
    }

    void toString(char* buffer)
    {
        snprintf(buffer, 256, "Button(float): %s", fLabel.c_str());
    }
};

/*--------------------------------------------------------------------------*/
class ofxFaustSlider : public ofxFaustUIObject {

private:

    FAUSTFLOAT fInit;
    FAUSTFLOAT fMin;
    FAUSTFLOAT fMax;
    FAUSTFLOAT fStep;
    virtual void valueChanged(FAUSTFLOAT &f){
        *fZone = range(fMin, fMax, f);
    }


public:
    ofxSlider<FAUSTFLOAT>* slider;
    ofxFaustSlider(const std::string& label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step)
    :ofxFaustUIObject(label,zone),fInit(init),fMin(min),fMax(max),fStep(step) {
        slider = new ofxSlider<FAUSTFLOAT>();
        slider = slider->setup(label,fInit,fMin,fMax);
        slider->addListener(this, &ofxFaustSlider::valueChanged);
    }
    virtual ~ofxFaustSlider() {
        delete slider;
    }

    void toString(char* buffer)
    {
        stringstream s;
        s << "Slider(float): " << fLabel << " [init=" << fInit << ":min=" << fMin << ":max=" << fMax << ":step=" << fStep << ":cur=" << *fZone << "]";
        strcpy(buffer, s.str().c_str());
    }

    void setValue(FAUSTFLOAT f) {*fZone = range(fMin, fMax, f);}
};

/*--------------------------------------------------------------------------*/
class ofxFaustBargraph : public ofxFaustUIObject {

private:

    FAUSTFLOAT fMin;
    FAUSTFLOAT fMax;
    FAUSTFLOAT fCurrent;

public:

   ofxFaustBargraph(const std::string& label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max)
    :ofxFaustUIObject(label,zone),fMin(min),fMax(max),fCurrent(*zone) {}
    virtual ~ofxFaustBargraph() {}

    void toString(char* buffer)
    {
        stringstream s;
        s << "Bargraph(float): " << fLabel << " [min=" << fMin << ":max=" << fMax << ":cur=" << *fZone << "]";
        strcpy(buffer, s.str().c_str());
    }

    virtual FAUSTFLOAT getValue()
    {
        if (*fZone != fCurrent) {
            fCurrent = *fZone;
            return fCurrent;
        } else {
            return NAN;
        }
    }
};



class ofxFaustGui : public UI{
private:

public:


    ofxPanel* panel;
    ofParameterGroup parameters;
    std::map<std::string, ofxGuiGroup*> fUITable;
    std::vector<std::string> fUIHierarchy;
    ofxFaustGui(){
        panel = new ofxPanel();
        panel = panel->setup("GUI");
    }
    virtual ~ofxFaustGui(){
        delete panel;
    }
    void draw(){
        panel->draw();
    }

    // -- widget's layouts
    virtual void addMainPanel(const char* label){
        fUITable[std::string(label)] = panel;
        fUIHierarchy.push_back(label);
           }
    virtual void addToGroup(ofxBaseGui* element){
            std::string last = fUIHierarchy.back();
            fUITable[last]->add(element);

    }
    virtual void addGenericGroup(const char* label){

        if(fUIHierarchy.empty()){
            addMainPanel(label);

        }else{
            ofxGuiGroup* group = new ofxGuiGroup();
            group->setup(label);
            fUITable[std::string(label)] = group;
            addToGroup(group);
            fUIHierarchy.push_back(label);
        }
    }

    virtual void openTabBox(const char* label) {
        addGenericGroup(label);

    }
    virtual void openHorizontalBox(const char* label){
        addGenericGroup(label);
    }
    virtual void openVerticalBox(const char* label){
        addGenericGroup(label);

    }
    virtual void closeBox(){
        fUIHierarchy.pop_back();
    }

    // -- active widgets

    virtual void addButton(const char* label, FAUSTFLOAT* zone){
        ofxFaustButton* button = new ofxFaustButton(label,zone);
        addToGroup(button->button);

    }
    virtual void addCheckButton(const char* label, FAUSTFLOAT* zone) {
        ofxFaustCheckButton* button = new ofxFaustCheckButton(label,zone);
        addToGroup(button->toggle);
    }
    virtual void addSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step){
        ofxFaustSlider* slider = new ofxFaustSlider(label,zone,init,min,max,step);
        addToGroup(slider->slider);
    }
    virtual void addVerticalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step){
            addSlider(label,zone,init, min, max, step);
    }
    virtual void addHorizontalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step) {
        addSlider(label,zone,init, min, max, step);

    }

    virtual void addNumEntry(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step){
             addSlider(label,zone,init, min, max, step);
    }

    // -- passive widgets

    virtual void addHorizontalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max) {}
    virtual void addVerticalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max){}

    // -- metadata declarations

    virtual void declare(FAUSTFLOAT*, const char*, const char*){}

};




#endif
