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

#ifndef __FAUST_GLOBAL__
#define __FAUST_GLOBAL__

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <set>
#include <map>
#include <list>

#ifndef _WIN32
#include <unistd.h>
#endif

#include "sourcereader.hh"
#include "property.hh"
#include "sigtype.hh"
#include "loopDetector.hh"
#include "occurrences.hh"
#include "instructions.hh"

class CTree;
typedef CTree* Tree;

class Symbol;
typedef Symbol* Sym;

class xtended;
class AudioType;

class Garbageable;

struct LLVMResult;

class ASMJAVAScriptInstVisitor;

struct comp_str
{
    bool operator()(Tree s1, Tree s2) const
    {
        return (strcmp(tree2str(s1), tree2str(s2)) < 0);    
    }
};

typedef map<Tree, set<Tree>, comp_str> MetaDataSet;

struct global {

    Tree 			gResult;
    Tree 			gResult2;

    SourceReader	gReader;

    MetaDataSet     gMetaDataSet;
    string          gDocLang;
    tvec            gWaveForm; 

    //-- globals
    string          gFaustSuperSuperDirectory;
    string          gFaustSuperDirectory;
    string          gFaustDirectory;
    string          gMasterDocument;
    string          gMasterDirectory;
    string          gMasterName;
    string          gDocName;
    list<string>    gImportDirList;
    list<string>    gLibraryList;
    string          gOutputDir;
    Tree            gExpandedDefList;

    //-- command line arguments
    bool            gDetailsSwitch;
    bool            gDrawSignals;
    bool            gShadowBlur;        // note: svg2pdf doesn't like the blur filter
    bool            gScaledSVG;         // to draw scaled SVG files
    bool            gStripDocSwitch;	// Strip <mdoc> content from doc listings.
    int            	gFoldThreshold;
    int            	gMaxNameSize;
    bool			gSimpleNames;
    bool            gSimplifyDiagrams;
    bool			gLessTempSwitch;
    int				gMaxCopyDelay;
    string			gOutputFile;
  
    bool            gVectorSwitch;
    bool            gDeepFirstSwitch;
    int             gVecSize;
    int             gVectorLoopVariant;
    int             gVecLoopSize;

    bool            gOpenMPSwitch;
    bool            gOpenMPLoop;
    bool            gSchedulerSwitch;
    bool            gOpenCLSwitch;
    bool            gCUDASwitch;
    bool			gGroupTaskSwitch;
    bool			gFunTaskSwitch;

    bool            gUIMacroSwitch;
    bool            gDumpNorm;

    int             gFloatSize;

    bool			gPrintFileListSwitch;
    bool			gInlineArchSwitch;

    bool			gDSPStruct;

    string			gClassName;

    LLVMResult*     gLLVMResult;
    const char*     gInputString;
    
    ostream*        gStringResult;
    
    bool			gLstDependenciesSwitch;     ///< mdoc listing management.
    bool			gLstMdocTagsSwitch;         ///< mdoc listing management.
    bool			gLstDistributedSwitch;      ///< mdoc listing management.
    
    map<string, string>		gDocMetadatasStringMap;
    set<string>				gDocMetadatasKeySet;
    
    map<string, string>		gDocAutodocStringMap;
    set<string>				gDocAutodocKeySet;
    
    map<string, bool>       gDocNoticeFlagMap;
    
    map<string, string>		gDocMathStringMap;
    
    vector<Tree>            gDocVector;				///< Contains <mdoc> parsed trees: DOCTXT, DOCEQN, DOCDGM.
    
    map<string, string>     gDocNoticeStringMap;
    set<string>             gDocNoticeKeySet;
    
    set<string>				gDocMathKeySet;
    
    bool                    gLatexDocSwitch;		// Only LaTeX outformat is handled for the moment.
    
    int                     gErrorCount;
    
    string                  gErrorMsg;
  
    Tabber                  TABBER;
    
    list<string>            gInputFiles;
    
    int                     gFileNum;
    
    int                     gCountInferences;
    int                     gCountMaximal;
    int                     gDummyInput;
    
    int                     gBoxSlotNumber;     ///< counter for unique slot number
    
    Tree                    BOXTYPEPROP;
    Tree                    NUMERICPROPERTY;
    Tree                    DEFLINEPROP;
    Tree                    SIMPLIFIED;
    Tree                    DOCTABLES;
    Tree                    NULLENV;
    Tree                    COLORPROPERTY;
    Tree                    ORDERPROP;
    Tree                    RECURSIVNESS;
    Tree                    NULLTYPEENV;
    Tree                    RECDEF;
    Tree                    DEBRUIJN2SYM;
    Tree                    DEFNAMEPROPERTY;
    Tree                    NICKNAMEPROPERTY;
    Tree                    BCOMPLEXITY;        // Node used for memoization purposes
    
    Node                    PROPAGATEPROPERTY;
    
    xtended*                gAbsPrim;
    xtended*                gAcosPrim;
    xtended*                gTanPrim;
    xtended*                gSqrtPrim;
    xtended*                gSinPrim;
    xtended*                gRintPrim;
    xtended*                gRemainderPrim;
    xtended*                gPowPrim;
    xtended*                gMinPrim;
    xtended*                gMaxPrim;
    xtended*                gLogPrim;
    xtended*                gLog10Prim;
    xtended*                gFmodPrim;
    xtended*                gFloorPrim;
    xtended*                gExpPrim;
    xtended*                gCosPrim;
    xtended*                gCeilPrim;
    xtended*                gAtanPrim;
    xtended*                gAtan2Prim;
    xtended*                gAsinPrim;
    
    Sym BOXIDENT;
    Sym BOXCUT;
    Sym BOXWAVEFORM;
    Sym BOXWIRE;
    Sym BOXSLOT;
    Sym BOXSYMBOLIC;
    Sym BOXSEQ;
    Sym BOXPAR;
    Sym BOXREC;
    Sym BOXSPLIT;
    Sym BOXMERGE;
    Sym BOXIPAR;
    Sym BOXISEQ;
    Sym BOXISUM;
    Sym BOXIPROD;
    Sym BOXABSTR;
    Sym BOXAPPL;
    Sym CLOSURE;
    Sym BOXERROR;
    Sym BOXACCESS;
    Sym BOXWITHLOCALDEF;
    Sym BOXMODIFLOCALDEF;
    Sym BOXENVIRONMENT;
    Sym BOXCOMPONENT;
    Sym BOXLIBRARY;
    Sym IMPORTFILE;
    Sym BOXPRIM0;
    Sym BOXPRIM1;
    Sym BOXPRIM2;
    Sym BOXPRIM3;
    Sym BOXPRIM4;
    Sym BOXPRIM5;
    Sym BOXFFUN;
    Sym BOXFCONST;
    Sym BOXFVAR;
    Sym BOXBUTTON;
    Sym BOXCHECKBOX;
    Sym BOXHSLIDER;
    Sym BOXVSLIDER;
    Sym BOXNUMENTRY;
    Sym BOXHGROUP;
    Sym BOXVGROUP;
    Sym BOXTGROUP;
    Sym BOXHBARGRAPH;
    Sym BOXVBARGRAPH;
    Sym BOXCASE;
    Sym BOXPATMATCHER;
    Sym BOXPATVAR;
    Sym BOXINPUTS;
    Sym BOXOUTPUTS;
    Sym DOCEQN;
    Sym DOCDGM;
    Sym DOCNTC;
    Sym DOCLST;
    Sym DOCMTD;
    Sym DOCTXT;
    Sym BARRIER;
    
    property<bool>* gPureRoutingProperty;
    property<Tree>* gSymbolicBoxProperty;
    
    Node EVALPROPERTY;
    Node PMPROPERTYNODE;
    
    property<Tree>* gSimplifiedBoxProperty;
    
    Sym UIFOLDER;
    Sym UIWIDGET;
    
    Sym PATHROOT;
    Sym PATHPARENT;
    Sym PATHCURRENT;
    
    Sym FFUN;
    
    // the property used to memoize the results
    property<Tree>*  gSymListProp;
    
    Sym SIGINPUT;
    Sym SIGOUTPUT;
    Sym SIGDELAY1;
    Sym SIGFIXDELAY;
    Sym SIGPREFIX;
    Sym SIGIOTA;
    Sym SIGRDTBL;
    Sym SIGWRTBL;
    Sym SIGTABLE;
    Sym SIGGEN;
    Sym SIGDOCONSTANTTBL;
    Sym SIGDOCWRITETBL;
    Sym SIGDOCACCESSTBL;
    Sym SIGSELECT2;
    Sym SIGSELECT3;
    Sym SIGBINOP;
    Sym SIGFFUN;
    Sym SIGFCONST;
    Sym SIGFVAR;
    Sym SIGPROJ;
    Sym SIGINTCAST;
    Sym SIGFLOATCAST;
    Sym SIGBUTTON;
    Sym SIGCHECKBOX;
    Sym SIGWAVEFORM;
    Sym SIGHSLIDER;
    Sym SIGVSLIDER;
    Sym SIGNUMENTRY;
    Sym SIGHBARGRAPH;
    Sym SIGVBARGRAPH;
    Sym SIGATTACH;
    Sym SIGTUPLE;
    Sym SIGTUPLEACCESS;
    
    Sym SIMPLETYPE;
    Sym TABLETYPE;
    Sym TUPLETTYPE;
    
    // memoized type contruction
    property<AudioType*>* gMemoizedTypes;
    
    // Essential predefined types
    Type TINT;
    Type TREAL;

    Type TKONST;
    Type TBLOCK;
    Type TSAMP;

    Type TCOMP;
    Type TINIT;
    Type TEXEC;

    // More predefined types
    Type TINPUT;
    Type TGUI;
    Type TGUI01;
    Type INT_TGUI;
    
    // Trying to accelerate type convergence
    Type TREC; // kVect ou kScal ?
    
    Sym CONS;
    Sym NIL;
    Tree nil;
    
    Sym PROCESS;
    
    Sym DEBRUIJN;
    Sym DEBRUIJNREF;
    Sym SUBSTITUTE;

    Sym SYMREC;
    Sym SYMRECREF;
    Sym SYMLIFTN;
    
    loopDetector gLoopDetector;
    
    string gDrawPath;
    
    int gMachineFloatSize;
    int gMachineIntSize;
    int gMachineDoubleSize;
    int gMachineBoolSize;
    int gMachinePtrSize;
    
    int gMachineMaxStackSize;
    
    const char* 	gDocDevSuffix;			///< ".tex" (or .??? - used to choose output device).
    string 			gCurrentDir;			///< Room to save current directory name.
    string          gLatexheaderfilename;

    struct tm		gCompilationDate;
    
    map<string, int>    gIDCounters;

    string        gDocTextsDefaultFile;

    // internal state during drawing
    Occurrences* 	gOccurrences;
    bool			gFoldingFlag;		// true with complex block-diagrams
    stack<Tree>		gPendingExp;		// Expressions that need to be drawn
    set<Tree>		gDrawnExp;			// Expressions drawn or scheduled so far
    const char* 	gDevSuffix;			// .svg or .ps used to choose output device
    string			gSchemaFileName;	// name of schema file beeing generated
    map<Tree, string>    gBackLink;		// link to enclosing file for sub schema
    
    // FIR 
    map<Typed::VarType, BasicTyped*> gTypeTable;    
    map<string, Typed*> gVarTypeTable;          // Types of variables or functions
    map<Typed::VarType, int> gTypeSizeMap;      // Size in bytes on types
    
    // colorize
    map<Tree, int> gColorMap;
    int gNextFreeColor;
     
    // to keep track of already injected files
    set<string> gAlreadyIncluded;
    
    char* gCurrentLocal;
    
    int gAllocationCount;
    
    bool gInPlace;  // add cache to input for correct in-place computations
    
    string gOutputLang;
    
    ASMJAVAScriptInstVisitor* gASMJSVisitor;
    
    bool gHelpSwitch;
    bool gVersionSwitch;
    bool gGraphSwitch;
    bool gDrawPSSwitch;
    bool gDrawSVGSwitch;
    bool gPrintXMLSwitch;
    bool gPrintJSONSwitch;
    bool gPrintDocSwitch;
    int gBalancedSwitch;
    string gArchFile;
    bool gExportDSP;
    
    // source file injection
    bool gInjectFlag;
    string gInjectFile;

    int gTimeout;   // time out to abort compiler (in seconds)
    bool gLLVMOut;
    
    // Globals to transfer results in thread based evaluation
    Tree gProcessTree;
    Tree gLsignalsTree;
    int gNumInputs;
    int gNumOutputs;
    string gErrorMessage;
   
    // GC
    static list<Garbageable*> gObjectTable;
    static bool gHeapCleanup;

    global();
    ~global();
    
    void init();
    
    static void allocate();
    static void destroy();
    
    string getFreshID(const string& prefix);
};

// Unique shared global pointer
extern global* gGlobal;

#define FAUST_LIB_PATH "FAUST_LIB_PATH"
#define MAX_STACK_SIZE 50000

#endif
