
#ifndef ofxFaustUI_h
#define ofxFaustUI_h

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif
#include "faust/gui/meta.h"
#include "faust/gui/UI.h"
#include "faust/gui/PathBuilder.h"
#include "faust/dsp/dsp.h"
#include "ofMain.h"
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <assert.h>

class ofxFaustUIObject {
    
protected:
    
    string fLabel;
    FAUSTFLOAT* fZone;
    
    FAUSTFLOAT range(FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT val) {return (val < min) ? min : (val > max) ? max : val;}
    
    
public:
    
    ofxFaustUIObject(const string& label, FAUSTFLOAT* zone):fLabel(label),fZone(zone) {}
    virtual ~ofxFaustUIObject() {}
    
    virtual void setValue(FAUSTFLOAT f) { *fZone = range(0.0, 1.0, f); }
    virtual FAUSTFLOAT getValue() { return *fZone; }
    virtual void toString(char* buffer) {}
    virtual string getName() { return fLabel; }
    
};

/*--------------------------------------------------------------------------*/
class ofxFaustCheckButton : public ofxFaustUIObject {
private:
    

public:
    ofParameter<bool>* toggle;
    virtual void valueChanged(bool &b){
        *fZone = (float)b;
    }
    ofxFaustCheckButton(const string& label, FAUSTFLOAT* zone):ofxFaustUIObject(label,zone) {
        toggle = new ofParameter<bool>();
        //toggle = toggle->setup(label,*fZone);
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
    
    virtual void valueChanged(bool &b){
        *fZone = (float)b;
    }
    
public:
    ofParameter<bool>* button;
    
    ofxFaustButton(const string& label, FAUSTFLOAT* zone):ofxFaustUIObject(label, zone) {
        button = new ofParameter<bool>();
        //button = button->setup(label);
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
    ofParameter<FAUSTFLOAT>* slider;
    ofxFaustSlider(const string& label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step)
    :ofxFaustUIObject(label,zone),fInit(init),fMin(min),fMax(max),fStep(step) {
        slider = new ofParameter<FAUSTFLOAT>();
        //slider = slider->setup(label,fInit,fMin,fMax);
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
    
    ofxFaustBargraph(const string& label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max)
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



class ofxFaustUI : public UI,PathBuilder{
private:
    std::map<string, ofxFaustUIObject*> fUITable;
    std::map<string, ofParameterGroup*> fUIGroupTable;
    std::vector<string> fUIHierarchy;
    string path;
protected:

    virtual void addToGroup(ofAbstractParameter & element,string path){
        fUIGroupTable[path]->add(element);
        
    }
    virtual void addMainPanel(const char* label){
        fControlsLevel.push_back(label);
        fUIGroupTable[buildPath(label)] = mainpanel;
        
    }
    virtual void addGenericGroup(const char* label){
        
        if(fUIHierarchy.empty()){
            addMainPanel(label);
            
        }else{
            ofParameterGroup* group = new ofParameterGroup();
            //group->setup(label);
            fControlsLevel.push_back(label);
            string path = buildPath(label);
            fUIGroupTable[path] = group;
            addToGroup(*group,path);
            
        }
    }
public:
    
    
    ofParameterGroup* mainpanel;

    ofxFaustUI(){

    }
    virtual ~ofxFaustUI(){
    };
    virtual void draw(){};
    void setValue(string label,FAUSTFLOAT val){
        if(fUITable.count(label)){
            fUITable[label]->setValue(val);
        }else{
            ofLogVerbose()<<"path "<< label <<" was not found";
        }
    }
    // -- widget's layouts

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
        fControlsLevel.pop_back();
    }
    
    // -- active widgets
    
    virtual void addButton(const char* label, FAUSTFLOAT* zone){
        ofxFaustButton* fbutton = new ofxFaustButton(label,zone);
        addToGroup(*(fbutton->button),fControlsLevel.back());
        fUITable[buildPath(label)] = fbutton;
        
    }
    virtual void addCheckButton(const char* label, FAUSTFLOAT* zone) {
        ofxFaustCheckButton* fbutton = new ofxFaustCheckButton(label,zone);
        addToGroup(*(fbutton->toggle),fControlsLevel.back());
        fUITable[buildPath(label)] = fbutton;
    }
    virtual void addSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step){
        ofxFaustSlider* fslider = new ofxFaustSlider(label,zone,init,min,max,step);
        addToGroup(*(fslider->slider),fControlsLevel.back());
        fUITable[buildPath(label)] = fslider;

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


#endif /* ofxFaustBaseGui_h */
