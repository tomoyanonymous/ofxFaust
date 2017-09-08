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

<<includeIntrinsic>>

//DSP class

<<includeclass>>

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



