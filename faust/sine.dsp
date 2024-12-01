import("stdfaust.lib");
f = hslider("freq",1000,50, 8000,0.01);
phasor(freq) = (+(freq/ma.SR) ~ ma.frac);
osc(freq) = sin(phasor(freq)*2*ma.PI);
process = osc(f);