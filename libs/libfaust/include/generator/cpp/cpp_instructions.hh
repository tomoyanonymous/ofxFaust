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

#ifndef _CPP_INSTRUCTIONS_H
#define _CPP_INSTRUCTIONS_H

using namespace std;

#include "text_instructions.hh"
#include "type_manager.hh"

class CPPInstVisitor : public TextInstVisitor {
    
    private:
    
        /*
         Global functions names table as a static variable in the visitor
         so that each function prototype is generated as most once in the module.
         */
        static map <string, int> gFunctionSymbolTable;
 
    public:

        CPPInstVisitor(std::ostream* out, int tab = 0)
            :TextInstVisitor(out, "->", tab)
        {
            // Mark all math.h functions as generated...
            gFunctionSymbolTable["abs"] = 1;
            gFunctionSymbolTable["absf"] = 1;
            gFunctionSymbolTable["fabsf"] = 1;
            gFunctionSymbolTable["acosf"] = 1;
            gFunctionSymbolTable["asinf"] = 1;
            gFunctionSymbolTable["atanf"] = 1;
            gFunctionSymbolTable["atan2f"] = 1;
            gFunctionSymbolTable["ceilf"] = 1;
            gFunctionSymbolTable["cosf"] = 1;
            gFunctionSymbolTable["expf"] = 1;
            gFunctionSymbolTable["floorf"] = 1;
            gFunctionSymbolTable["fmodf"] = 1;
            gFunctionSymbolTable["logf"] = 1;
            gFunctionSymbolTable["log10f"] = 1;
            gFunctionSymbolTable["max"] = 1;
            gFunctionSymbolTable["min"] = 1;
            gFunctionSymbolTable["powf"] = 1;
            gFunctionSymbolTable["roundf"] = 1;
            gFunctionSymbolTable["sinf"] = 1;
            gFunctionSymbolTable["sqrtf"] = 1;
            gFunctionSymbolTable["tanf"] = 1;
        }

        virtual ~CPPInstVisitor()
        {}

        virtual void visit(AddMetaDeclareInst* inst)
        {
            // Special case
            if (inst->fZone == "0") {
                *fOut << "interface->declare(" << inst->fZone <<", " << quote(inst->fKey) << ", " << quote(inst->fValue) << ")";
            } else {
                *fOut << "interface->declare(" << "&" << inst->fZone <<", " << quote(inst->fKey) << ", " << quote(inst->fValue) << ")";
            }
            EndLine();
        }

        virtual void visit(OpenboxInst* inst)
        {
            string name;
            switch (inst->fOrient) {
                case 0:
                    name = "interface->openVerticalBox("; break;
                case 1:
                    name = "interface->openHorizontalBox("; break;
                case 2:
                    name = "interface->openTabBox("; break;
            }
            *fOut << name << quote(inst->fName) << ")";
            EndLine();
        }

        virtual void visit(CloseboxInst* inst)
        {
            *fOut << "interface->closeBox();"; tab(fTab, *fOut);
        }
        
        virtual void visit(AddButtonInst* inst)
        {
            if (inst->fType == AddButtonInst::kDefaultButton) {
                *fOut << "interface->addButton(" << quote(inst->fLabel) << "," << "&" << inst->fZone << ")"; 
            } else {
                *fOut << "interface->addCheckButton(" << quote(inst->fLabel) << "," << "&" << inst->fZone << ")"; 
            }
            EndLine();
        }

        virtual void visit(AddSliderInst* inst)
        {
            string name;
            switch (inst->fType) {
                case AddSliderInst::kHorizontal:
                    name = "interface->addHorizontalSlider"; break;
                case AddSliderInst::kVertical:
                    name = "interface->addVerticalSlider"; break;
                case AddSliderInst::kNumEntry:
                    name = "interface->addNumEntry"; break;
            }
            *fOut << name << "(" << quote(inst->fLabel) << ", " 
            << "&" << inst->fZone << ", " << checkReal(inst->fInit) 
            << ", " << checkReal(inst->fMin) << ", " 
            << checkReal(inst->fMax) << ", " << checkReal(inst->fStep) << ")";
            EndLine();
        }

        virtual void visit(AddBargraphInst* inst)
        {
            string name;
            switch (inst->fType) {
                case AddBargraphInst::kHorizontal:
                    name = "interface->addHorizontalBargraph"; break;
                case AddBargraphInst::kVertical:
                    name = "interface->addVerticalBargraph"; break;
            }
            *fOut << name << "(" << quote(inst->fLabel) << ", " << "&" << inst->fZone << ", "<< checkReal(inst->fMin) << ", " << checkReal(inst->fMax) << ")";
            EndLine();
        }

        virtual void visit(DeclareVarInst* inst)
        {
            if (inst->fAddress->getAccess() & Address::kStaticStruct) {
                 *fOut << "static ";
            }

            if (inst->fAddress->getAccess() & Address::kVolatile) {
                 *fOut << "volatile ";
            }

            *fOut << generateType(inst->fType, inst->fAddress->getName());
            if (inst->fValue) {
                *fOut << " = "; inst->fValue->accept(this); 
            }
            EndLine();
        }

        virtual void visit(DeclareFunInst* inst)
        {
            // Already generated
            if (gFunctionSymbolTable.find(inst->fName) != gFunctionSymbolTable.end()) {
                return;
            } else {
                gFunctionSymbolTable[inst->fName] = 1;
            }
            
            // Defined as macro in the architecture file...
            if (inst->fName == "min" || inst->fName == "max") {
                return;
            }
            
            // Prototype arguments
            if (inst->fType->fAttribute & FunTyped::kInline) {
                *fOut << "inline ";
            }
            
            if (inst->fType->fAttribute & FunTyped::kVirtual) {
                *fOut << "virtual ";
            }
                       
            // Prototype
            *fOut << generateType(inst->fType->fResult, generateFunName(inst->fName));
            generateFunDefArgs(inst);
            generateFunDefBody(inst);
        }
        
        virtual void visit(LoadVarAddressInst* inst)
        {
            *fOut << "&"; inst->fAddress->accept(this);
        }
     
        virtual void visit(CastNumInst* inst)
        {
            *fOut << generateType(inst->fType);
            *fOut << "("; inst->fInst->accept(this);  *fOut << ")";
        }
      
        virtual void visit(FunCallInst* inst)
        {
            generateFunCall(inst, inst->fName);
        }
        
        static void cleanup() { gFunctionSymbolTable.clear(); }

};

class CPPVecInstVisitor : public CPPInstVisitor {

    public:

        CPPVecInstVisitor(std::ostream* out, int tab = 0)
            :CPPInstVisitor(out, tab)
        {}

};

/**
 * Use the Apple Accelerate framework.
 *
 */

/*
class CPPVecAccelerateInstVisitor : public CPPVecInstVisitor {

    protected:

        std::string fCurValue;
        Typed::VarType fCurType;

        std::map<int, string> fVecBinOpTable;
        std::map<int, string> fScalarBinOpTable;
        std::map<string, string> fFunctionTable;
        int fVecCounter;

    public:

        CPPVecAccelerateInstVisitor(std::ostream* out, int tab = 0)
            :CPPVecInstVisitor(out, tab), fCurType(Typed::kNoType)
        {
            fVecBinOpTable[kAdd] = "vDSP_vadd";
            fVecBinOpTable[kSub] = "vDSP_vsub";
            fVecBinOpTable[kMul] = "vDSP_vmul";
            fVecBinOpTable[kDiv] = "vDSP_vdiv";

            fFunctionTable["sin"] = "vvsin";
            fFunctionTable["sinf"] = "vvsinf";
            fFunctionTable["cos"] = "vvcos";
            fFunctionTable["cosf"] = "vvcosf";
            fFunctionTable["tan"] = "vvatan";
            fFunctionTable["tanf"] = "vvatanf";
            fFunctionTable["log"] = "vvlog";
            fFunctionTable["logf"] = "vvlogf";
            fFunctionTable["log10"] = "vvlog10";
            fFunctionTable["log10f"] = "vvlog10f";

            //fFunctionTable["log2"] = "native_log2";
            //fFunctionTable["log2f"] = "native_log2";

            fFunctionTable["exp"] = "vvexp";
            fFunctionTable["expf"] = "vvexpf";
            fFunctionTable["pow"] = "vvpow";
            fFunctionTable["powf"] = "vvpowf";
            fFunctionTable["sqrt"] = "vvrsqrt";
            fFunctionTable["sqrtf"] = "vvrsqrtf";
            fFunctionTable["fabsf"] = "vvfabf";
            fFunctionTable["floor"] = "vvfloor";
            fFunctionTable["floorf"] = "vvfloorf";
            fFunctionTable["fmodf"] = "vvfmodf";

            fVecCounter = 0;
        }

        ~CPPVecAccelerateInstVisitor()
        {}

        inline string generateNameVec()
        {
            return subst("vec$0_", T(fVecCounter++));
        }

        virtual void visit(LoadVarInst* inst)
        {
             // Keep name as value
            fCurValue = inst->fAddress->getName();
            //cerr << "inst->fAddress->getName " << inst->fAddress->getName() << std::endl;
            // Keep type
            assert(gGlobal->gVarTypeTable.find(inst->fAddress->getName()) != gGlobal->gVarTypeTable.end());
            fCurType = gGlobal->gVarTypeTable[inst->fAddress->getName()]->getType();
        }

        virtual void visit(FloatNumInst* inst)
        {
            fCurValue = T(inst->fNum);
            fCurType = Typed::kFloat;
        }

        virtual void visit(IntNumInst* inst)
        {
            fCurValue = T(inst->fNum);
            fCurType = Typed::kInt;
        }

        virtual void visit(BoolNumInst* inst)
        {
            fCurValue = T(inst->fNum);
            fCurType = Typed::kBool;
        }

        virtual void visit(DoubleNumInst* inst)
        {
            fCurValue = T(inst->fNum);
            fCurType = Typed::kDouble;
        }

        virtual void visit(BinopInst* inst)
        {
            // Keep result of first arg compilation
            inst->fInst1->accept(this);
            std::string res1 = fCurValue;

            // Keep result of second arg compilation
            inst->fInst2->accept(this);
            std::string res2 = fCurValue;

            // Generate new result symbol, both arguments are equal, so fCurType is the one of last evaluated one
            fCurValue = generateNameVec();
            *fOut << fTypeDirectTable[fCurType] << " " << fCurValue << "[" << inst->fSize << "]"; 
            EndLine();

            // Generate stream
            if (inst->fInst1->fSize >= 1 && inst->fInst2->fSize >= 1) {
                // Full vector operation
                cerr << "inst->fOpcode " << inst->fOpcode  << endl;
                *fOut << fVecBinOpTable[inst->fOpcode] << "(" << res1 << ", 1, " << res2 << ", 1, " << fCurValue << ", 1, " << inst->fSize << ")";
            } else if (inst->fInst1->fSize > 1) {
                // Scalar-Vec operation
                // TODO
                // *fOut << fScalarBinOpTable[inst->fOpcode] << "(" << res1 << ", 1, " << res2 << fCurValue << ", 1, " << inst->fSize << ")";
            } else {
                // Scalar operation
                CPPVecInstVisitor::visit(inst);
            }

            EndLine();
        }

        virtual void visit(CastNumInst* inst)
        {
            // Compile value to cast, result in fCurValue
            inst->fInst->accept(this);

            BasicTyped* basic_typed = dynamic_cast<BasicTyped*>(inst->fType);

            switch (basic_typed->fType) {

                case Typed::kFloat: {
                    string res = generateNameVec();
                    *fOut << fTypeDirectTable[Typed::kFloat] << " " << res << "[" << inst->fSize << "]"; 
                    EndLine();
                    
                    switch (fCurType) {

                        case Typed::kInt:
                            *fOut << "vDSP_vflt32(" << fCurValue << " , 1 " << res << " , 1, " << inst->fSize << ")";
                            break;

                        case Typed::kFloat:
                            // No supposed to happen
                            assert(false);
                            break;

                         case Typed::kDouble:
                            *fOut << "vDSP_vspdp(" << fCurValue << " , 1 " << res << " , 1, " << inst->fSize << ")";
                            break;

                         default:
                            // No supposed to happen
                            cerr << "CurType " << fTypeDirectTable[fCurType] << endl;
                            assert(false);
                            break;
                    }
                    fCurType = Typed::kFloat;
                    fCurValue = res;
                    break;
                }

                case Typed::kInt: {
                    string res = generateNameVec();
                    // *fOut << fTypeDirectTable[Typed::kInt] << " " << res << "[" << inst->fSize << "]"; EndLine();
                    switch (fCurType) {

                        case Typed::kInt:
                            // No supposed to happen
                            assert(false);
                            break;

                        case Typed::kFloat:
                            *fOut << "vDSP_vfixr32(" << fCurValue << " , 1 " << res << " , 1, " << inst->fSize << ")";
                            break;

                         case Typed::kDouble:
                            *fOut << "vDSP_vfixr32D(" << fCurValue << " , 1 " << res << " , 1, " << inst->fSize << ")";
                            break;

                         default:
                            // No supposed to happen
                            assert(false);
                            break;
                    }
                    fCurType = Typed::kInt;
                    fCurValue = res;
                    break;
                }

                case Typed::kDouble: {
                    string res = generateNameVec();
                    *fOut << fTypeDirectTable[Typed::kDouble] << " " << res << "[" << inst->fSize << "]"; 
                    EndLine();
                    
                    switch (fCurType) {

                        case Typed::kInt:
                            *fOut << "vDSP_vflt32D(" << fCurValue << " , 1 " << res << " , 1, " << inst->fSize << ")";
                            break;

                        case Typed::kFloat:
                            *fOut << "vDSP_vdpsp(" << fCurValue << " , 1 " << res << " , 1, " << inst->fSize << ")";
                            break;

                         case Typed::kDouble:
                            // No supposed to happen
                            assert(false);
                            break;

                         default:
                            // No supposed to happen
                            assert(false);
                            break;
                    }
                    fCurType = Typed::kDouble;
                    fCurValue = res;
                    break;
                }

                case Typed::kQuad:
                    // No supposed to happen
                    assert(false);
                    break;

                default:
                    // No supposed to happen
                    assert(false);
                    break;

            }
            EndLine();
        }

        virtual void visit(FunCallInst* inst)
        {
            string res = generateNameVec();
            *fOut << fTypeDirectTable[fCurType] << " " << res << "[" << inst->fSize << "]"; 
            EndLine();

            if (inst->fMethod) {
                list<ValueInst*>::const_iterator it =  inst->fArgs.begin();
                // Compile object arg
                (*it)->accept(this);
                *fOut << "->" << ((fFunctionTable.find(inst->fName) != fFunctionTable.end()) ? fFunctionTable[inst->fName] : inst->fName) << "(";
                list<ValueInst*>::const_iterator it1;
                int i = 0;
                // Add result in parameter list
                *fOut << res;
                for (it1 = ++it; it1 != inst->fArgs.end(); it1++, i++) {
                    // Compile argument
                    (*it1)->accept(this);
                    *fOut << ", ";
                }
                // Add "count" in parameter list
                *fOut << inst->fSize;
                *fOut << ")";
            } else {
                *fOut << ((fFunctionTable.find(inst->fName) != fFunctionTable.end()) ? fFunctionTable[inst->fName] : inst->fName) << "(";
                list<ValueInst*>::const_iterator it;
                int i = 0;
                // Add result in parameter list
                *fOut << res;
                for (it = inst->fArgs.begin(); it != inst->fArgs.end(); it++, i++) {
                    // Compile argument
                    (*it)->accept(this);
                    *fOut << ", ";
                }
                // Add "count" in parameter list
                *fOut << inst->fSize;
                *fOut << ")";
            }

            fCurValue = res;
            // fCurType does not change
        }

        virtual void visit(Select2Inst* inst)
        {
            // Compile condition, result in fCurValue
            inst->fCond->accept(this);
            std::string cond_value = fCurValue;

            // Compile then branch, result in fCurValue
            inst->fThen->accept(this);
            std::string then_value = fCurValue;

            // Compile else branch, result in fCurValue
            inst->fElse->accept(this);
            std::string else_value = fCurValue;

            // TODO
        }

};

class MRCPPInstVisitor : public CPPInstVisitor {

    private:

        map <string, StructTyped*> gTypeTable;

    public:

        MRCPPInstVisitor(std::ostream* out, int tab = 0)
            :CPPInstVisitor(out, tab)
        {}

        virtual ~MRCPPInstVisitor()
        {}

        void visitStructAddress(StructTyped* struct_type, IndexedAddress* indexed)
        {
            ArrayTyped* array_type = dynamic_cast<ArrayTyped*>(struct_type->fType);
            assert(array_type);
            StructTyped* struct_type1 = dynamic_cast<StructTyped*>(array_type->fType);
            BasicTyped* basic_type1 = dynamic_cast<BasicTyped*>(array_type->fType);
            IndexedAddress* indexed1 = dynamic_cast<IndexedAddress*>(indexed->fAddress);

            *fOut << "[";
            indexed->fIndex->accept(this);

            if (indexed1) {
                *fOut << "].f";
            } else {
                *fOut << "]";
            }

            if (struct_type1 && indexed1) {
                visitStructAddress(struct_type1, indexed1);
            } else if (basic_type1 && indexed1) {
                visitAddress(indexed1);
            }
        }

        void visitAddress(IndexedAddress* indexed)
        {
            *fOut << "[";
            indexed->fIndex->accept(this);
            *fOut << "]";
        }

        virtual void visit(IndexedAddress* indexed)
        {
            // Struct type access
            if (gGlobal->gVarTypeTable.find(indexed->getName()) != gGlobal->gVarTypeTable.end()) {
                Typed* var_type = gGlobal->gVarTypeTable[indexed->getName()];
                ArrayTyped* array_type = dynamic_cast<ArrayTyped*>(var_type);
                assert(array_type);
                StructTyped* struct_type = dynamic_cast<StructTyped*>(array_type->fType);
                if (struct_type) {
                    visitStructAddress(struct_type, indexed);
                    return;
                }
            }

            // Default case
            visitAddress(indexed);
        }

        virtual void visit(DeclareTypeInst* inst)
        {
            StructTyped* struct_typed = dynamic_cast<StructTyped*>(inst->fType);

            // Check if type is already generated
            if (struct_typed && gTypeTable.find(struct_typed->fName) == gTypeTable.end()) {
                Typed* sub_type = struct_typed->fType;
                *fOut << "struct " << struct_typed->fName << " {" << endl;
                *fOut << "\t" << generateType(sub_type, "f"); 
                EndLine();
                *fOut << "}";
                EndLine();
                gTypeTable[struct_typed->fName] = struct_typed;
            }
        }

};
*/
#endif
