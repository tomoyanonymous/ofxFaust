import("stdfaust.lib");

freq = hslider("/h:filter/cutoff_freq",1000,1,20000,0.01);
q = hslider("/h:filter/q",2,0.1,6,0.01);
volume = vslider("/h:master/volume",-20,-70,6,0.01):ba.db2linear;

switch = checkbox("/h:space/LRswap");

swap = _,_<:select2(switch),select2(switch==0):>_,_;

mute = checkbox("/h:master/mute")==0;

osc = os.osc(1000);
beep = checkbox("/h:/beep");

mono_module = (_:fi.resonbp(freq,q,1),osc*beep ):> *(volume):*(mute);

process = _,_:swap:mono_module,mono_module;
