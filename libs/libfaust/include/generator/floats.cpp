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

#include <iostream>
#include <sstream>
#include <stdlib.h>

#include "floats.hh"
#include "global.hh"

//-----------------------------------------------
// float size coding :
//-----------------------------------------------
//          0: external float (macro name)
//          1: single precision float
//          2: double precision float
//          3: long double precision float

const char* mathsuffix[] = {"", "f", "", "l"};                              // suffix for math functions
const char* numsuffix[]  = {"", "f", "", "L"};                              // suffix for numeric constants
const char* floatname[]  = {FLOATMACRO, "float", "double", "quad"};         // float types
const char* castname[]   = {FLOATCASTER, "(float)", "(double)", "(quad)"};  // float castings

const char* isuffix() { return mathsuffix[gGlobal->gFloatSize]; } ///< suffix for math functions
const char* inumix()  { return numsuffix [gGlobal->gFloatSize]; } ///< suffix for numeric constants

const Typed::VarType itfloat()
{
    switch (gGlobal->gFloatSize) {
        case 1:
            return Typed::kFloat;
        case 2:
            return Typed::kDouble;
        case 3:
            return Typed::kQuad;
        default:
            assert(false);
            break;
    }
}

const char* ifloat() { return floatname[gGlobal->gFloatSize]; }
const char* icast()  { return castname[gGlobal->gFloatSize]; }
const char* xfloat() { return floatname[0]; }
const char* xcast()  { return castname[0]; }

void printfloatdef(std::ostream& fout, bool quad)
{
    fout << "#ifndef " << FLOATMACRO << std::endl;
    fout << "#define " << FLOATMACRO << " " << "float" << std::endl;
    fout << "#endif  " << std::endl;
    fout << std::endl;
    if (quad) {
        fout << "typedef long double quad;" << std::endl;
    }
}
