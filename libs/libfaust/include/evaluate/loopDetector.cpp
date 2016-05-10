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

#include "loopDetector.hh"
#include "ppbox.hh"
#include "exception.hh"

bool loopDetector::detect(Tree t)
{
    fPhase++;
    int w = fPhase%fBuffersize;
    fBuffer[w] = t;
    if ((fPhase%fCheckperiod) == 0) {
        // time to check for a cycle
        for (int i=1; i<fBuffersize; i++) {
            int r = w-i; if (r < 0) { r += fBuffersize; }
            assert(r>=0);
            assert(r<fBuffersize);
            assert(r != w);
            if (fBuffer[r] == t) {
                stringstream error;
                error << "ERROR : after "
                    << fPhase
                    << " evaluation steps, the compiler has detected an endless evaluation cycle of "
                    << i
                    << " steps";
                throw faustexception(error.str());
            }
        }
    }
    return false;
}
