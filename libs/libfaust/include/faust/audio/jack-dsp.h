/************************************************************************
	IMPORTANT NOTE : this file contains two clearly delimited sections :
	the ARCHITECTURE section (in two parts) and the USER section. Each section
	is governed by its own copyright and license. Please check individually
	each section for license and copyright information.
*************************************************************************/
/*******************BEGIN ARCHITECTURE SECTION (part 1/2)****************/

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
 
#ifndef __jack_dsp__
#define __jack_dsp__

#include <stdio.h>
#include <cstdlib>
#include <list>
#include <vector>
#include <string.h>
#include <jack/jack.h>
#include <jack/midiport.h>
#ifdef JACK_IOS
#include <jack/custom.h>
#endif
#include "faust/audio/audio.h"
#include "faust/dsp/dsp.h"
#include "faust/midi/midi.h"
#include "faust/gui/ring-buffer.h"

#if defined(_WIN32) && !defined(__MINGW32__)
#define snprintf _snprintf_s
#endif

/******************************************************************************
*******************************************************************************

							JACK AUDIO INTERFACE

*******************************************************************************
*******************************************************************************/

class jackaudio : public audio {
    
    protected:

        dsp*			fDSP;               // FAUST DSP
        jack_client_t*	fClient;            // JACK client
    
        std::vector<jack_port_t*>	fInputPorts;        // JACK input ports
        std::vector<jack_port_t*>	fOutputPorts;       // JACK output ports
        
        std::vector<char*> fPhysicalInputs;
        std::vector<char*> fPhysicalOutputs;
    
        shutdown_callback fShutdown;        // Shutdown callback to be called by libjack
        void*           fShutdownArg;       // Shutdown callback data
        void*           fIconData;          // iOS specific
        int             fIconSize;          // iOS specific
        bool            fAutoConnect;       // autoconnect with system in/out ports
        
        std::list<std::pair<std::string, std::string> > fConnections;		// Connections list
    
        static int _jack_srate(jack_nframes_t nframes, void* arg)
        {
            fprintf(stdout, "The sample rate is now %u/sec\n", nframes);
            return 0;
        }
        
        static void _jack_shutdown(void* arg) 
        {}
       
        static void _jack_info_shutdown(jack_status_t code, const char* reason, void* arg)
        {
            fprintf(stderr, "%s\n", reason);
            static_cast<jackaudio*>(arg)->shutdown(reason);
        }
        
        static int _jack_process(jack_nframes_t nframes, void* arg)
        {
            return static_cast<jackaudio*>(arg)->process(nframes);
        }
        
        static int _jack_buffersize(jack_nframes_t nframes, void* arg)
        {
            fprintf(stdout, "The buffer size is now %u/sec\n", nframes);
            return 0;
        }
     
        #ifdef _OPENMP
        static void* _jack_thread(void* arg)
        {
            jackaudio* audio = (jackaudio*)arg;
            audio->process_thread();
            return 0;
        }
        #endif
        
        void shutdown(const char* message)
        {
            fClient = NULL;
            
            if (fShutdown) {
                fShutdown(message, fShutdownArg);
            } else {
                exit(1); // By default
            }
        }
        
        // Save client connections
        virtual void save_connections()
        {
            fConnections.clear();
            
             for (int i = 0; i < fInputPorts.size(); i++) {
                const char** connected_port = jack_port_get_all_connections(fClient, fInputPorts[i]);
                if (connected_port != NULL) {
                    for (int port = 0; connected_port[port]; port++) {
                        fConnections.push_back(std::make_pair(connected_port[port], jack_port_name(fInputPorts[i])));
//                        printf("INPUT %s ==> %s\n", connected_port[port], jack_port_name(fInputPorts[i]));
                    }
                    jack_free(connected_port);
                }
            }
       
            for (int i = 0; i < fOutputPorts.size(); i++) {
                const char** connected_port = jack_port_get_all_connections(fClient, fOutputPorts[i]);
                if (connected_port != NULL) {
                    for (int port = 0; connected_port[port]; port++) {
                        fConnections.push_back(std::make_pair(jack_port_name(fOutputPorts[i]), connected_port[port]));
//                        printf("OUTPUT %s ==> %s\n", jack_port_name(fOutputPorts[i]), connected_port[port]);
                    }
                    jack_free(connected_port);
                }
            }
        }

        // Load previous client connections
        void load_connections()
        {
            std::list<std::pair<std::string, std::string> >::const_iterator it;
            for (it = fConnections.begin(); it != fConnections.end(); it++) {
                std::pair<std::string, std::string> connection = *it;
                jack_connect(fClient, connection.first.c_str(), connection.second.c_str());
            }
        }
     
    #ifdef _OPENMP
        void process_thread() 
        {
            jack_nframes_t nframes;
            while (1) {
                nframes = jack_cycle_wait(fClient);
                process(nframes);
                jack_cycle_signal(fClient, 0);
            }
        }
    #endif
    
        // JACK callbacks
        virtual int	process(jack_nframes_t nframes) 
        {
            AVOIDDENORMALS;
            
            // Retrieve JACK inputs/output audio buffers
            float** fInChannel = (float**)alloca(fInputPorts.size() * sizeof(float*));
            for (int i = 0; i < fInputPorts.size(); i++) {
                fInChannel[i] = (float*)jack_port_get_buffer(fInputPorts[i], nframes);
            }

            float** fOutChannel = (float**)alloca(fOutputPorts.size() * sizeof(float*));
            for (int i = 0; i < fOutputPorts.size(); i++) {
                fOutChannel[i] = (float*)jack_port_get_buffer(fOutputPorts[i], nframes);
            }

            fDSP->compute(nframes, fInChannel, fOutChannel);
            return 0;
        }

    public:
    
        jackaudio(const void* icon_data = 0, size_t icon_size = 0, bool auto_connect = true) 
            : fDSP(0), fClient(0), fShutdown(0), fShutdownArg(0), fAutoConnect(auto_connect)
        {
            if (icon_data) {
                fIconData = malloc(icon_size);
                fIconSize = icon_size;
                memcpy(fIconData, icon_data, icon_size);
            } else {
                fIconData = NULL;
                fIconSize = 0;
            }
        }
        
        virtual ~jackaudio() 
        { 
            if (fClient) {
                stop();
                
                for (int i = 0; i < fInputPorts.size(); i++) {
                    jack_port_unregister(fClient, fInputPorts[i]);
                }
                for (int i = 0; i < fOutputPorts.size(); i++) {
                    jack_port_unregister(fClient, fOutputPorts[i]);
                }
                jack_client_close(fClient);
                  
                if (fIconData) {
                    free(fIconData);
                }
            }
        }
        
        virtual bool init(const char* name, dsp* dsp) 
        {
            if (init(name)) {
                if (dsp) { set_dsp(dsp); }
                return true;
            } else {  
                return false;
            }
        }

        virtual bool init(const char* name) 
        {
            if ((fClient = jack_client_open(name, JackNullOption, NULL)) == 0) {
                fprintf(stderr, "JACK server not running ?\n");
                return false;
            }
        #ifdef JACK_IOS
            jack_custom_publish_data(fClient, "icon.png", fIconData, fIconSize);
        #endif
        
        #ifdef _OPENMP
            jack_set_process_thread(fClient, _jack_thread, this);
        #else
            jack_set_process_callback(fClient, _jack_process, this);
        #endif
        
            jack_set_sample_rate_callback(fClient, _jack_srate, this);
            jack_set_buffer_size_callback(fClient, _jack_buffersize, this);
            jack_on_info_shutdown(fClient, _jack_info_shutdown, this);
            
            // Get Physical inputs
            int inputs = 0;
            char** physicalInPorts = (char**)jack_get_ports(fClient, NULL, JACK_DEFAULT_AUDIO_TYPE, JackPortIsPhysical|JackPortIsOutput);
            if (physicalInPorts != NULL) {
                while (physicalInPorts[inputs]) { 
                    fPhysicalInputs.push_back(physicalInPorts[inputs]);  
                    printf("physical input %s\n", physicalInPorts[inputs]);
                    inputs++; 
                }
                jack_free(physicalInPorts);
            }
            
            // Get Physical outputs
            int outputs = 0;
            char** physicalOutPorts = (char**)jack_get_ports(fClient, NULL, JACK_DEFAULT_AUDIO_TYPE, JackPortIsPhysical|JackPortIsInput);
            if (physicalOutPorts != NULL) {
                while (physicalOutPorts[outputs]) { 
                    fPhysicalOutputs.push_back(physicalOutPorts[outputs]); 
                    printf("physical output %s\n", physicalOutPorts[outputs]);
                    outputs++; 
                }
                jack_free(physicalOutPorts);
            }
        
            return true;
        }    
        
        virtual bool start() 
        {
            if (jack_activate(fClient)) {
                fprintf(stderr, "Cannot activate client\n");
                return false;
            }
            
            if (fConnections.size() > 0) {
                load_connections();
            } else if (fAutoConnect) {
                default_connections();
            }
            
            return true;
        }

        virtual void stop() 
        {
            if (fClient) {
                save_connections();
                jack_deactivate(fClient);
            }
        }
    
        virtual void shutdown(shutdown_callback cb, void* arg)
        {
            fShutdown = cb;
            fShutdownArg = arg;
        }
        
        virtual int get_buffer_size() { return jack_get_buffer_size(fClient); }
        virtual int get_sample_rate() { return jack_get_sample_rate(fClient); }
                 
        virtual int get_num_inputs() 
        { 
            return fPhysicalInputs.size(); 
        }
        
        virtual int get_num_outputs() 
        { 
            return fPhysicalOutputs.size();
        }
  
        // Additional public API
        
        jack_client_t* get_client() { return fClient; }
   
        // Connect to physical inputs/outputs
        void default_connections()
        {
            // To avoid feedback at launch time, don't connect hardware inputs
            /*
            for (int i = 0; i < fInputPorts.size() && i < fPhysicalOutputs.size(); i++) {
                jack_connect(fClient, fPhysicalOutputs[i], jack_port_name(fInputPorts[i]));
            }
            */
            for (int i = 0; i < fOutputPorts.size() && i < fPhysicalInputs.size(); i++) {
                jack_connect(fClient, jack_port_name(fOutputPorts[i]), fPhysicalInputs[i]);
            }
        }
    
        virtual void set_dsp(dsp* dsp)
        {
            fDSP = dsp;
                       
            for (int i = 0; i < fDSP->getNumInputs(); i++) {
                char buf[256];
                snprintf(buf, 256, "in_%d", i);
                fInputPorts.push_back(jack_port_register(fClient, buf, JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0));
            }
            for (int i = 0; i < fDSP->getNumOutputs(); i++) {
                char buf[256];
                snprintf(buf, 256, "out_%d", i);
                fOutputPorts.push_back(jack_port_register(fClient, buf, JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0));
            }
            
            fDSP->init(jack_get_sample_rate(fClient));
        }
             
        void connect(jackaudio* driver, int src, int dst, bool reverse)
        {
            if (driver) {
                // Connection between drivers
                jack_port_t* src_port = get_output_port(src);
                jack_port_t* dst_port = driver->get_input_port(src);
                if (src_port && dst_port) {
                    jack_connect(fClient, jack_port_name(src_port), jack_port_name(dst_port));
                }
            } else if (reverse) {
                // Connection to physical input
                if (src > fPhysicalInputs.size()) return;
                jack_port_t* dst_port = get_input_port(dst);
                if (dst_port) {
                    jack_connect(fClient, fPhysicalInputs[src], jack_port_name(dst_port));
                }
            } else {
                // Connection to physical output
                if (dst > fPhysicalOutputs.size()) return;
                jack_port_t* src_port = get_output_port(src);
                if (src_port) {
                    jack_connect(fClient, jack_port_name(src_port), fPhysicalOutputs[dst]);
                }
            }
        }
        
        void disconnect(jackaudio* driver, int src, int dst, bool reverse)
        {
            if (driver) {
                // Connection between drivers
                jack_port_t* src_port = get_output_port(src);
                jack_port_t* dst_port = driver->get_input_port(src);
                if (src_port && dst_port) {
                    jack_disconnect(fClient, jack_port_name(src_port), jack_port_name(dst_port));
                }
            } else if (reverse) {
                // Connection to physical input
                if (src > fPhysicalInputs.size()) return;
                jack_port_t* dst_port = get_input_port(dst);
                if (dst_port) {
                    jack_disconnect(fClient, fPhysicalInputs[src], jack_port_name(dst_port));
                }
            } else {
                // Connection to physical output
                if (dst > fPhysicalOutputs.size()) return;
                jack_port_t* src_port = get_output_port(src);
                if (src_port) {
                    jack_disconnect(fClient, jack_port_name(src_port), fPhysicalOutputs[dst]);
                }
            }
        }
        
        bool is_connected(jackaudio* driver, int src, int dst, bool reverse)
        {
            if (driver) {
                // Connection between drivers
                jack_port_t* src_port = get_output_port(src);
                jack_port_t* dst_port = driver->get_input_port(src);
                if (src_port && dst_port) {
                    return jack_port_connected_to(src_port, jack_port_name(dst_port));
                } else {
                    return false;
                }
            } else if (reverse) {
                // Connection to physical input
                if (src > fPhysicalInputs.size()) return false;
                jack_port_t* dst_port = get_input_port(dst);
                if (dst_port) {
                    return jack_port_connected_to(dst_port, fPhysicalInputs[src]);
                } else {
                    return false;
                }
            } else {
                // Connection to physical output
                if (dst > fPhysicalOutputs.size()) return false;
                jack_port_t* src_port = get_output_port(src);
                if (src_port) {
                    return jack_port_connected_to(src_port, fPhysicalOutputs[dst]);
                } else {
                    return false;
                }
            }
        }
        
        jack_port_t* get_input_port(int port)  { return (port >= 0 && port < fInputPorts.size()) ? fInputPorts[port] : 0; }
        jack_port_t* get_output_port(int port) { return (port >= 0 && port < fOutputPorts.size()) ? fOutputPorts[port] : 0; }
     
};

// Add JACK MIDI

class jackaudio_midi : public jackaudio, public midi_handler {  

    protected:
    
        jack_port_t* fInputMidiPort;       // JACK input MIDI port
        jack_port_t* fOutputMidiPort;      // JACK output MIDI port
        
        ringbuffer_t* fOutBuffer;
        
        virtual void save_connections()
        {
            jackaudio::save_connections();
            
            if (fInputMidiPort) {
                const char** connected_port = jack_port_get_all_connections(fClient, fInputMidiPort);
                if (connected_port != NULL) {
                    for (int port = 0; connected_port[port]; port++) {
                        fConnections.push_back(std::make_pair(connected_port[port], jack_port_name(fInputMidiPort)));
                        // printf("INPUT %s ==> %s\n", connected_port[port], jack_port_name(fInputPorts[i]));
                    }
                    jack_free(connected_port);
                }
            }
        
            if (fOutputMidiPort) {
                const char** connected_port = jack_port_get_all_connections(fClient, fOutputMidiPort);
                if (connected_port != NULL) {
                    for (int port = 0; connected_port[port]; port++) {
                        fConnections.push_back(std::make_pair(jack_port_name(fOutputMidiPort), connected_port[port]));
                        // printf("OUTPUT %s ==> %s\n", jack_port_name(fOutputPorts[i]), connected_port[port]);
                    }
                    jack_free(connected_port);
                }
            }
        }
      
        virtual void processMidiIn(jack_nframes_t nframes) 
        {
            // MIDI input
            void* port_buf_in = jack_port_get_buffer(fInputMidiPort, nframes);
           
            for (int i = 0; i < jack_midi_get_event_count(port_buf_in); ++i) {
                jack_midi_event_t event;
                if (jack_midi_event_get(&event, port_buf_in, i) == 0) {
                
                    size_t nBytes = event.size;
                    int cmd = (int)event.buffer[0] & 0xf0;
                    int channel = (int)event.buffer[0] & 0x0f;
                    double time = event.time; // Timestamp in frames
                    
                    // MIDI sync
                    if (nBytes == 1) {
                    
                        int sync = (int)event.buffer[0];
                        
                        if (sync == MIDI_CLOCK) {
                            for (unsigned int i = 0; i < fMidiInputs.size(); i++) {
                                fMidiInputs[i]->clock(time);
                            }
                        } else if (sync == MIDI_START) {
                            for (unsigned int i = 0; i < fMidiInputs.size(); i++) {
                                fMidiInputs[i]->start(time);
                            }
                        } else if (sync == MIDI_STOP) {
                            for (unsigned int i = 0; i < fMidiInputs.size(); i++) {
                                fMidiInputs[i]->stop(time);
                            }
                        }
                        
                    } else if (nBytes == 2) {
                     
                        int data1 = (int)event.buffer[1];
                        
                        if (cmd == MIDI_PROGRAM_CHANGE) {
                            for (unsigned int i = 0; i < fMidiInputs.size(); i++) {
                                fMidiInputs[i]->progChange(time, channel, data1);
                            }
                        } else if (cmd == MIDI_AFTERTOUCH) {
                            for (unsigned int i = 0; i < fMidiInputs.size(); i++) {
                                fMidiInputs[i]->chanPress(time, channel, data1);
                            }
                        }
                    
                    } else if (nBytes == 3) {
                    
                        int data1 = (int)event.buffer[1];
                        int data2 = (int)event.buffer[2];
                        
                        if (cmd == MIDI_NOTE_OFF || ((cmd == MIDI_NOTE_ON) && (data2 == 0))) { 
                            for (unsigned int i = 0; i < fMidiInputs.size(); i++) {
                                fMidiInputs[i]->keyOff(time, channel, data1, data2);
                            }
                        } else if (cmd == MIDI_NOTE_ON) {
                            for (unsigned int i = 0; i < fMidiInputs.size(); i++) {
                                fMidiInputs[i]->keyOn(time, channel, data1, data2);
                            }
                        } else if (cmd == MIDI_CONTROL_CHANGE) {
                            for (unsigned int i = 0; i < fMidiInputs.size(); i++) {
                                fMidiInputs[i]->ctrlChange(time, channel, data1, data2);
                            }
                        } else if (cmd == MIDI_PITCH_BEND) {
                            for (unsigned int i = 0; i < fMidiInputs.size(); i++) {
                                fMidiInputs[i]->pitchWheel(time, channel, ((data2 * 128.0 + data1) - 8192) / 8192.0);
                            }
                        } else if (cmd == MIDI_POLY_AFTERTOUCH) {
                            for (unsigned int i = 0; i < fMidiInputs.size(); i++) {
                                fMidiInputs[i]->keyPress(time, channel, data1, data2);
                            }
                        }
                    } 
                }
            }
        }
        
        virtual void processAudio(jack_nframes_t nframes) 
        {
            // Audio
            AVOIDDENORMALS;
            
            // Retrieve JACK inputs/output audio buffers
            float** fInChannel = (float**)alloca(fInputPorts.size() * sizeof(float*));
            for (int i = 0; i < fInputPorts.size(); i++) {
                fInChannel[i] = (float*)jack_port_get_buffer(fInputPorts[i], nframes);
            }

            float** fOutChannel = (float**)alloca(fOutputPorts.size() * sizeof(float*));
            for (int i = 0; i < fOutputPorts.size(); i++) {
                fOutChannel[i] = (float*)jack_port_get_buffer(fOutputPorts[i], nframes);
            }
            
            // By convention timestamp of -1 means 'no timestamp conversion' : events already have a timestamp espressed in frames
            fDSP->compute(-1, nframes, fInChannel, fOutChannel); 
        }
        
        virtual void processMidiOut(jack_nframes_t nframes) 
        {
            // MIDI output 
            unsigned char* port_buf_out = (unsigned char*)jack_port_get_buffer(fOutputMidiPort, nframes);
            jack_midi_clear_buffer(port_buf_out);
            size_t res, message_size;
            
            // Write each message one by one
            while (ringbuffer_read(fOutBuffer, (char*)&message_size, sizeof(message_size)) == sizeof(message_size)) {
                // Reserve MIDI event with the correct size
                jack_midi_data_t* data = jack_midi_event_reserve(port_buf_out, 0, message_size);
                if (data) {
                    // Write its content
                    if ((res = ringbuffer_read(fOutBuffer, (char*)data, message_size)) != message_size) {
                        fprintf(stderr, "processMidiOut incorrect message : res =  %d\n", res);
                    }
                } else {
                    fprintf(stderr, "jack_midi_event_reserve error\n");
                }
            }
        }
     
        virtual int process(jack_nframes_t nframes) 
        {
            // MIDI in
            if (fInputMidiPort) {
                processMidiIn(nframes);
            }
            
            // Audio
            processAudio(nframes);
            
            // MIDI out
            if (fOutputMidiPort) {
                processMidiOut(nframes);
            }
            
            return 0;
        }
        
        void writeMessage(unsigned char* buffer, size_t size)
        {
            if (fOutBuffer) {
                size_t res;
                // Write size of message
                if ((res = ringbuffer_write(fOutBuffer, (const char*)&size, sizeof(size_t))) != sizeof(size_t)) {
                    fprintf(stderr, "writeMessage size : error size = %lu res = %lu\n", size, res);
                }
                // Write message content
                if ((res = ringbuffer_write(fOutBuffer, (const char*)buffer, size)) != size) {
                    fprintf(stderr, "writeMessage message : error size = %lu res = %lu\n", size, res);
                }
            }
        }
  
    public: 
    
        jackaudio_midi(const void* icon_data = 0, size_t icon_size = 0, bool auto_connect = true) 
            :jackaudio(icon_data, icon_size, auto_connect), midi_handler("JACKMidi"), 
            fInputMidiPort(0), fOutputMidiPort(0), fOutBuffer(0)
        {}
        
        virtual ~jackaudio_midi()
        {
            if (fClient) {
                if (fInputMidiPort) { jack_port_unregister(fClient, fInputMidiPort); }
                if (fOutputMidiPort) { jack_port_unregister(fClient, fOutputMidiPort); }
            }
            if (fOutBuffer) { 
                ringbuffer_free(fOutBuffer);
            }
        }
        
        virtual bool init(const char* name, dsp* dsp, bool midi = false) 
        {
            if (jackaudio::init(name)) {
                if (dsp) { set_dsp(dsp); }
                if (midi) {
                    fOutBuffer = ringbuffer_create(8192);
                    fInputMidiPort = jack_port_register(fClient, "midi_in_1", JACK_DEFAULT_MIDI_TYPE, JackPortIsInput, 0);
                    fOutputMidiPort = jack_port_register(fClient, "midi_out_1", JACK_DEFAULT_MIDI_TYPE, JackPortIsOutput, 0);
                }
                return true;
            } else {  
                return false;
            }
        }
           
        virtual bool start() 
        {
            return jackaudio::start();
        }
        
        virtual void stop() 
        {
            jackaudio::stop();
        }
        
        void keyOn(int channel, int pitch, int velocity) 
        {
            unsigned char buffer[3] = { MIDI_NOTE_ON + channel, pitch, velocity };
            writeMessage(buffer, 3);
        }
        
        void keyOff(int channel, int pitch, int velocity) 
        {
            unsigned char buffer[3] = { MIDI_NOTE_OFF + channel, pitch, velocity };
            writeMessage(buffer, 3);
        }
        
        void ctrlChange(int channel, int ctrl, int val) 
        {
            unsigned char buffer[3] = { MIDI_CONTROL_CHANGE + channel, ctrl, val };
            writeMessage(buffer, 3);
        }
        
        void chanPress(int channel, int press) 
        {
            unsigned char buffer[2] = { MIDI_AFTERTOUCH + channel, press };
            writeMessage(buffer, 2);
        }
        
        void progChange(int channel, int pgm) 
        {
            unsigned char buffer[2] = { MIDI_PROGRAM_CHANGE + channel, pgm };
            writeMessage(buffer, 2);
        }
          
        void keyPress(int channel, int pitch, int press) 
        {
            unsigned char buffer[3] = { MIDI_POLY_AFTERTOUCH + channel, pitch, press };
            writeMessage(buffer, 3);
        }
   
        void pitchWheel(int channel, int wheel) 
        {
            unsigned char buffer[3] = { MIDI_PITCH_BEND + channel, wheel & 0x7F, (wheel >> 7) & 0x7F };
            writeMessage(buffer, 3);
        }
        
        void ctrlChange14bits(int channel, int ctrl, int value) {}
         
        void start(double date) 
        {
            unsigned char buffer[1] = { MIDI_START };
            writeMessage(buffer, 1);
        }
       
        void stop(double date) 
        {
            unsigned char buffer[1] = { MIDI_STOP };
            writeMessage(buffer, 1);
        }
        
        void clock(double date) 
        {
            unsigned char buffer[1] = { MIDI_CLOCK };
            writeMessage(buffer, 1);
        }
 
};

#endif
