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

#ifndef ofxFDATGUI_H
#define ofxFDATGUI_H

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

#include "faust/gui/UI.h"

#include "faust/gui/meta.h"
#include "ofxDatGui.h"

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <assert.h>

class ofxFdatUIObject {
    
protected:
    
    string fLabel;
    FAUSTFLOAT* fZone;
    
    FAUSTFLOAT range(FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT val) {return (val < min) ? min : (val > max) ? max : val;}
    
    
public:
    
    ofxFdatUIObject(const string& label, FAUSTFLOAT* zone):fLabel(label),fZone(zone) {}
    virtual ~ofxFdatUIObject() {}
    
    virtual void setValue(FAUSTFLOAT f) { *fZone = range(0.0, 1.0, f); }
    virtual FAUSTFLOAT getValue() { return *fZone; }
    virtual void toString(char* buffer) {}
    virtual string getName() { return fLabel; }
    
};

/*--------------------------------------------------------------------------*/
class ofxFdatCheckButton : public ofxFdatUIObject {
private:
    
    virtual void valueChanged(ofxDatGuiToggleEvent e){
        bool b = e.target->getChecked();
        *fZone = (float)b;
    }
public:
    ofxDatGuiToggle* toggle;

    ofxFdatCheckButton(const string& label, FAUSTFLOAT* zone):ofxFdatUIObject(label,zone) {
        toggle = new ofxDatGuiToggle(label);
        toggle->onToggleEvent(this,&ofxFdatCheckButton::valueChanged);
    }
    
    virtual ~ofxFdatCheckButton() {
        delete toggle;
    }

};

/*--------------------------------------------------------------------------*/
class ofxFdatButton : public ofxFdatUIObject {
private:
    
    virtual void valueChanged(ofxDatGuiButtonEvent e){
        *fZone = 1;
    }
    
public:
    ofxDatGuiButton* button;

    ofxFdatButton(const string& label, FAUSTFLOAT* zone):ofxFdatUIObject(label, zone) {
        button = new ofxDatGuiButton(label);
        button->onButtonEvent(this,&ofxFdatButton::valueChanged);
    }
    void reset(){
        *fZone = 0;
    }
    virtual ~ofxFdatButton() {
        delete button;
    }
    
};

/*--------------------------------------------------------------------------*/
class ofxFdatSlider : public ofxFdatUIObject {
    
private:
    
    FAUSTFLOAT fInit;
    FAUSTFLOAT fMin;
    FAUSTFLOAT fMax;
    FAUSTFLOAT fStep;
    int precision = floor((fMax-fMin)/fStep);
    virtual void valueChanged(ofxDatGuiSliderEvent e){
        *fZone = range(fMin, fMax, e.value);
    }
    
    
public:
    ofxDatGuiSlider* slider;
    ofxFdatSlider(const string& label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step)
    :ofxFdatUIObject(label,zone),fInit(init),fMin(min),fMax(max),fStep(step) {
        slider = new ofxDatGuiSlider(label,fMin,fMax,fInit);
        slider->setPrecision(precision);
        slider->onSliderEvent(this, &ofxFdatSlider::valueChanged);
    }
    virtual ~ofxFdatSlider() {
        delete slider;
    }
    

    
    void setValue(FAUSTFLOAT f) {*fZone = range(fMin, fMax, f);}
};
/*--------------------------------------------------------------------------*/
class ofxFdatNuminput : public ofxFdatUIObject {
    
private:
    
    FAUSTFLOAT fInit;
    FAUSTFLOAT fMin;
    FAUSTFLOAT fMax;
    FAUSTFLOAT fStep;
    virtual void valueChanged(ofxDatGuiTextInputEvent e){
        FAUSTFLOAT v =  range(fMin, fMax, stof(e.text));
        textinput->setText(to_string(v));
        *fZone = v;
    }
    
    
public:
    ofxDatGuiTextInput* textinput;
    ofxFdatNuminput(const string& label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step)
    :ofxFdatUIObject(label,zone),fInit(init),fMin(min),fMax(max),fStep(step) {
        textinput = new ofxDatGuiTextInput(label,to_string(fInit));
        textinput->setInputType(ofxDatGuiInputType::NUMERIC);
        textinput->onTextInputEvent(this, &ofxFdatNuminput::valueChanged);
    }
    virtual ~ofxFdatNuminput() {
        delete textinput;
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
class ofxFdatBargraph : public ofxFdatUIObject {
    
private:
    
    FAUSTFLOAT fMin;
    FAUSTFLOAT fMax;
    FAUSTFLOAT fCurrent;
    
public:
    ofxDatGuiValuePlotter* plotter;
    
   ofxFdatBargraph(const string& label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max)
    :ofxFdatUIObject(label,zone),fMin(min),fMax(max),fCurrent(*zone) {
        plotter = new ofxDatGuiValuePlotter(label,min,max);
    }
    virtual ~ofxFdatBargraph() {}

    
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



class ofxFdatGui : public UI{
private:

public:
    
    
    ofxDatGui* gui;
    ofParameterGroup parameters;
    std::map<string, ofxDatGuiFolder*> fUITable;
    std::vector<ofxFdatButton*> buttons;
    std::vector<string> fUIHierarchy;
    string last = fUIHierarchy.back();
    ofxFdatGui(){
           }
    virtual ~ofxFdatGui(){
        delete gui;
    }
    void draw(){
        gui->draw();
        resetbutton();
    }

    void resetbutton(){
        for(int i = 0;i<buttons.size();i++){
            buttons[i]->reset();
        }
    }
    
    // -- widget's layouts
    virtual void addMainPanel(const char* label){
        gui = new ofxDatGui(ofxDatGuiAnchor::TOP_LEFT);
        gui->setAutoDraw(false);
        gui->addHeader("ofxFaust-DatGui");

        fUIHierarchy.push_back(label);
           }

    virtual void addGenericGroup(const char* label){
        
        if(fUIHierarchy.empty()){
            addMainPanel(label);

        }else{
        ofxDatGuiFolder* group = new ofxDatGuiFolder(label);
            if(fUIHierarchy.size()==1){
                gui->addFolder(group);
            }else{
                fUITable[last]->addFolder(group);
            }
        fUITable[string(label)] = group;
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
    void addComponent(ofxDatGuiComponent* component){
        if(isFolder){
            fUITable[last]->attachItem(component);
        }else{
            gui->attachItem(component);
        }
    }
    
    // -- active widgets
    bool isFolder = !fUIHierarchy.empty();
    virtual void addButton(const char* label, FAUSTFLOAT* zone){
        ofxFdatButton* button = new ofxFdatButton(label,zone);
        buttons.push_back(button);
        addComponent(button->button);
    }
    virtual void addCheckButton(const char* label, FAUSTFLOAT* zone) {
        ofxFdatCheckButton* button = new ofxFdatCheckButton(label,zone);
        addComponent(button->toggle);

    }
    virtual void addSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step){
        ofxFdatSlider* slider = new ofxFdatSlider(label,zone,init,min,max,step);
        addComponent(slider->slider);
    }
    virtual void addVerticalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step){
            addSlider(label,zone,init, min, max, step);
    }
    virtual void addHorizontalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step) {
        addSlider(label,zone,init, min, max, step);

    }

    virtual void addNumEntry(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step){
        ofxFdatNuminput* input = new ofxFdatNuminput(label,zone,init,min,max,step);
        addComponent(input->input);
    }
    
    // -- passive widgets
    
    virtual void addHorizontalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max) {
        ofxFdatBargraph* bargraph = new ofxFdatBargraph(label,zone,min,max);
        addComponent(plotter->plotter);
    }
    virtual void addVerticalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max){
        ofxFdatBargraph* bargraph = new ofxFdatBargraph(label,zone,min,max);
        addComponent(plotter->plotter);
    }
    
    // -- metadata declarations
    
    virtual void declare(FAUSTFLOAT*, const char*, const char*){}

};




#endif