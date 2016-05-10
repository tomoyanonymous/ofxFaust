import("music.lib");
import("filter.lib");

gain= hslider("gain",0.1,0,1,0.01):smooth(tau2pole(0.01));
freq= hslider("freq",200,1,4000,0.01):smooth(tau2pole(0.01));

// local variables 'with'
mySine = osc(_)*_;

process = (freq,gain:mySine),_;
