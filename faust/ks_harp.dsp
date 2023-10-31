declare name "KarplusStrongPolyphony";
declare options "[midi:on][nvoices:12]";
import("stdfaust.lib");

ks_buttons = gate : pm.impulseExcitation*gain : pm.ks( (freq : pm.f2l), damping )
with{
    f = hslider("v:karplus/h:[0]params/[0]freq[style:knob]",440,50,1000,0.01);
    gain = hslider("v:karplus/h:[0]params/[2]gain[style:knob]",0.8,0,1,0.01);
    damping = hslider("v:karplus/h:[0]params/[1]damping[midi:ctrl 1][style:knob]" ,0.01,0,1,0.01) : si.smoo;
    t = button("v:karplus/[1]gate");
    gate = t : min(1);
    freq = f;
};

process = ks_buttons <: _,_;
