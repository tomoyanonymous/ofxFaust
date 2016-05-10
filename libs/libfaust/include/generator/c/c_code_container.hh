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

#ifndef _C_CODE_CONTAINER_H
#define _C_CODE_CONTAINER_H

#include "code_container.hh"
#include "c_instructions.hh"
#include "vec_code_container.hh"
#include "omp_code_container.hh"
#include "wss_code_container.hh"

using namespace std;

class CCodeContainer : public virtual CodeContainer {

    protected:

        CInstVisitor fCodeProducer;
        std::ostream* fOut;
        
        void produceInfoFunctions(int tabs, const string& classname, bool isvirtual);
        void produceMetadata(int tabs);

    public:

        CCodeContainer(const string& name, int numInputs, int numOutputs, std::ostream* out)
            : fCodeProducer(out, name), fOut(out)
        {
            initializeCodeContainer(numInputs, numOutputs);
            fKlassName = name;
        }
        virtual ~CCodeContainer()
        {}

        virtual void produceClass();
        virtual void generateCompute(int tab) = 0;
        void produceInternal();

        CodeContainer* createScalarContainer(const string& name, int sub_container_type);

        static CodeContainer* createContainer(const string& name, int numInputs, int numOutputs, ostream* dst);

};

class CScalarCodeContainer : public CCodeContainer {

    protected:

    public:

        CScalarCodeContainer(const string& name, int numInputs, int numOutputs, std::ostream* out, int sub_container_type);
        virtual ~CScalarCodeContainer();

        void generateCompute(int tab);

};

class CVectorCodeContainer : public VectorCodeContainer, public CCodeContainer {

    protected:

    public:

        CVectorCodeContainer(const string& name, int numInputs, int numOutputs, std::ostream* out);
        virtual ~CVectorCodeContainer();

        void generateCompute(int n);

};

class COpenMPCodeContainer : public OpenMPCodeContainer, public CCodeContainer {

    protected:

    public:

        COpenMPCodeContainer(const string& name,int numInputs, int numOutputs, std::ostream* out);
        virtual ~COpenMPCodeContainer();

        void generateCompute(int tab);

};

class CWorkStealingCodeContainer : public WSSCodeContainer, public CCodeContainer {

    protected:

    public:

        CWorkStealingCodeContainer(const string& name, int numInputs, int numOutputs, std::ostream* out);
        virtual ~CWorkStealingCodeContainer();

        void generateCompute(int tab);
};

#endif
