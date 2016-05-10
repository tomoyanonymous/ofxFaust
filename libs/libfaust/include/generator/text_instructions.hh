/************************************************************************
 ************************************************************************
    FAUST compiler
	Copyright (C) 2003-2004 GRAME, Centre National de Creation Musicale
    ---------------------------------------------------------------------
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 ************************************************************************
 ************************************************************************/

#ifndef _TEXT_INSTRUCTIONS_H
#define _TEXT_INSTRUCTIONS_H

using namespace std;

#include "instructions.hh"
#include "type_manager.hh"
#include "Text.hh"

#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

class TextInstVisitor : public InstVisitor, public StringTypeManager {

    protected:

        int fTab;
        std::ostream* fOut;
        bool fFinishLine;
        string fObjectAccess;
    
    public:

        TextInstVisitor(std::ostream* out, const string& object_access, int tab = 0)
            :StringTypeManager(FLOATMACRO, "*"), fTab(tab), fOut(out), fFinishLine(true), fObjectAccess(object_access)
        {}
        
        TextInstVisitor(std::ostream* out, const string& object_access, string float_macro_name, string ptr_postfix, int tab = 0)
            :StringTypeManager(float_macro_name, ptr_postfix), fTab(tab), fOut(out), fFinishLine(true), fObjectAccess(object_access)
        {}

        virtual ~TextInstVisitor()
        {}

        void Tab(int n) { fTab = n; }

        void EndLine()
        {
            if (fFinishLine) {
                *fOut << ";";
                tab(fTab, *fOut);
            }
        }

        virtual void visit(AddMetaDeclareInst* inst) {}
        virtual void visit(OpenboxInst* inst) {}
        virtual void visit(CloseboxInst* inst) {}
        virtual void visit(AddButtonInst* inst) {}
        virtual void visit(AddSliderInst* inst) {}
        virtual void visit(AddBargraphInst* inst) {}
      
        virtual void visit(LabelInst* inst)
        {
            *fOut << inst->fLabel;
            tab(fTab, *fOut);
        }

        virtual void visit(DeclareVarInst* inst) { assert(false); }
        
        virtual void visit(RetInst* inst)
        {
            if (inst->fResult) {
                *fOut << "return ";
                inst->fResult->accept(this); 
            } else {
                *fOut << "return";
            }
            EndLine();
        }

        virtual void visit(DropInst* inst)
        {
            if (inst->fResult) {
                inst->fResult->accept(this); 
                EndLine();
            }
        }

        virtual void visit(DeclareFunInst* inst) { assert(false); }
           
        virtual void visit(NamedAddress* named)
        {
            *fOut << named->fName;
        }

        virtual void visit(IndexedAddress* indexed)
        {
            indexed->fAddress->accept(this);
            *fOut << "["; indexed->fIndex->accept(this); *fOut << "]";
        }
        
        virtual void visit(LoadVarInst* inst)
        {
            inst->fAddress->accept(this);
        }
     
        virtual void visit(LoadVarAddressInst* inst) { assert(false); }
       
        virtual void visit(StoreVarInst* inst)
        {
            inst->fAddress->accept(this);
            *fOut << " = ";
            inst->fValue->accept(this);
            EndLine();
        }

        virtual void visit(FloatNumInst* inst)
        {
            *fOut << checkFloat(inst->fNum);
        }
        
        virtual void visit(FloatArrayNumInst* inst)
        {
            char sep = '{';
            for (unsigned int i = 0; i < inst->fNumTable.size(); i++) {
                *fOut << sep << checkFloat(inst->fNumTable[i]);
                 sep = ',';
            }
            *fOut << '}';
        }

        virtual void visit(IntNumInst* inst)
        {
            *fOut << inst->fNum;
        }
        
        virtual void visit(IntArrayNumInst* inst)
        {
            char sep = '{';
            for (unsigned int i = 0; i < inst->fNumTable.size(); i++) {
                *fOut << sep << inst->fNumTable[i];
                 sep = ',';
            }
            *fOut << '}';
        }

        virtual void visit(BoolNumInst* inst)
        {
            *fOut << inst->fNum;
        }

        virtual void visit(DoubleNumInst* inst)
        {
            *fOut << checkDouble(inst->fNum);
        }
        
        virtual void visit(DoubleArrayNumInst* inst)
        {
            char sep = '{';
            for (unsigned int i = 0; i < inst->fNumTable.size(); i++) {
                *fOut << sep << checkDouble(inst->fNumTable[i]);
                 sep = ',';
            }
            *fOut << '}';
        }

        virtual void visit(BinopInst* inst)
        {
            *fOut << "(";
            inst->fInst1->accept(this);
            *fOut << " ";
            *fOut << gBinOpTable[inst->fOpcode]->fName;
            *fOut << " ";
            inst->fInst2->accept(this);
            *fOut << ")";
        }

        virtual void visit(CastNumInst* inst) { assert(false); }
        
        virtual string generateFunName(const string& name)
        {   
            // If function is actually a method (that is "xx::name"), then keep "xx::name" in gGlobalTable but print "name"
            size_t pos;
            if ((pos = name.find("::")) != string::npos) {
                return name.substr(pos + 2); // After the "::"
            } else {
                return name;
            }
        }
        
        virtual void generateFunCallArgs(list<ValueInst*>::const_iterator beg, list<ValueInst*>::const_iterator end, int size)
        {   
            list<ValueInst*>::const_iterator it = beg;
            int i = 0;
            for (it = beg; it != end; it++, i++) {
                // Compile argument
                (*it)->accept(this);
                if (i < size - 1) *fOut << ", ";
            }
        }
        
        virtual void generateFunDefArgs(DeclareFunInst* inst)
        {
            *fOut << "(";
            list<NamedTyped*>::const_iterator it;
            int size = inst->fType->fArgsTypes.size(), i = 0;
            for (it = inst->fType->fArgsTypes.begin(); it != inst->fType->fArgsTypes.end(); it++, i++) {
                *fOut << generateType((*it));
                if (i < size - 1) *fOut << ", ";
            }
        }
        
        virtual void generateFunDefBody(DeclareFunInst* inst)
        {
            if (inst->fCode->fCode.size() == 0) {
                *fOut << ");" << endl;  // Pure prototype
            } else {
                // Function body
                *fOut << ") {";
                    fTab++;
                    tab(fTab, *fOut);
                    inst->fCode->accept(this);
                    fTab--;
                    tab(fTab, *fOut);
                *fOut << "}";
                tab(fTab, *fOut);
            }
        }
        
        virtual void generateFunCall(FunCallInst* inst, const string& fun_name)
        {
            if (inst->fMethod) {
                list<ValueInst*>::const_iterator it = inst->fArgs.begin();
                // Compile object arg
                (*it)->accept(this);
                // Compile parameters
                *fOut << fObjectAccess << fun_name << "(";
                generateFunCallArgs(++it, inst->fArgs.end(), inst->fArgs.size() - 1);
            } else {
                *fOut << fun_name << "(";
                // Compile parameters
                generateFunCallArgs(inst->fArgs.begin(), inst->fArgs.end(), inst->fArgs.size());
            }
            *fOut << ")";
        }

        virtual void visit(FunCallInst* inst) { assert(false); }
           
        virtual void visit(Select2Inst* inst)
        {
            *fOut << "(";
            inst->fCond->accept(this);
            *fOut << "?";
            inst->fThen->accept(this);
            *fOut << ":";
            inst->fElse->accept(this);
            *fOut << ")";
        }

        virtual void visit(IfInst* inst)
        {
            *fOut << "if ";
            inst->fCond->accept(this);
            *fOut << " {";
                fTab++;
                tab(fTab, *fOut);
                inst->fThen->accept(this);
                fTab--;
                tab(fTab, *fOut);
            if (inst->fElse->fCode.size() > 0) {
                *fOut << "} else {";
                    fTab++;
                    tab(fTab, *fOut);
                    inst->fElse->accept(this);
                    fTab--;
                    tab(fTab, *fOut);
                *fOut << "}";
            } else {
                *fOut << "}";
            }
            tab(fTab, *fOut);
        }

        virtual void visit(ForLoopInst* inst)
        {
            // Don't generate empty loops...
            if (inst->fCode->size() == 0) return;
            
            *fOut << "for (";
                fFinishLine = false;
                inst->fInit->accept(this);
                *fOut << "; ";
                inst->fEnd->accept(this);
                *fOut << "; ";
                inst->fIncrement->accept(this);
                fFinishLine = true;
            *fOut << ") {";
                fTab++;
                tab(fTab, *fOut);
                inst->fCode->accept(this);
                fTab--;
                tab(fTab, *fOut);
            *fOut << "}";
            tab(fTab, *fOut);
        }

        virtual void visit(WhileLoopInst* inst)
        {
            *fOut << "while ("; inst->fCond->accept(this); *fOut << ") {";
                fTab++;
                tab(fTab, *fOut);
                inst->fCode->accept(this);
                fTab--;
                tab(fTab, *fOut);
             *fOut << "}";
             tab(fTab, *fOut);
        }

        virtual void visit(BlockInst* inst)
        {
            if (inst->fIndent) {
                *fOut << "{";
                fTab++;
                tab(fTab, *fOut);
            }
            list<StatementInst*>::const_iterator it;
            for (it = inst->fCode.begin(); it != inst->fCode.end(); it++) {
                (*it)->accept(this);
            }
            if (inst->fIndent) {
                fTab--;
                tab(fTab, *fOut);
                *fOut << "}";
                tab(fTab, *fOut);
            }
        }

        virtual void visit(::SwitchInst* inst)
        {
            *fOut << "switch ("; inst->fCond->accept(this); *fOut << ") {";
                fTab++;
                tab(fTab, *fOut);
                list<pair <int, BlockInst*> >::const_iterator it;
                for (it = inst->fCode.begin(); it != inst->fCode.end(); it++) {
                    if ((*it).first == -1) { // -1 used to code "default" case
                        *fOut << "default: {";
                    } else {
                         *fOut << "case " << (*it).first << ": {";
                    }
                        fTab++;
                        tab(fTab, *fOut);
                        ((*it).second)->accept(this);
                        if (!((*it).second)->hasReturn()) {
                            *fOut << "break;";
                        }
                        fTab--;
                        tab(fTab, *fOut);
                    *fOut << "}";
                    tab(fTab, *fOut);
                }
                fTab--;
                tab(fTab, *fOut);
            *fOut << "}";
            tab(fTab, *fOut);
        }

};

#endif
