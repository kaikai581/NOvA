% qNC.mp -- do not edit, generated automatically by numubarccinclusive.tex
input feynmp
require_RCS_revision "1.30";
beginchar(1, 40*2.84526pt#, 25*2.84526pt#, 0);
"feynmf: 1";
LaTeX_unitlength:=2.84526pt;
subgraph (0, 0, w, h);
vinit;
pickup pencircle scaled thin;
pickup pencircle scaled thick;
vleft (__i1, __i2);
vright (__o1, __o2);
vtop (__t);
vconnect ("fermion,label=$q$", __i1, __v1);
vconnect ("fermion,label=$q$", __v1, __o1);
vconnect ("boson,label=$Z^0$", __v1, __t);
vfreeze;
vdraw;
endsubgraph;
endchar;
% the end.
end.
endinput;
