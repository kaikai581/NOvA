% qCC.mp -- do not edit, generated automatically by numubarccinclusive.tex
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
vbottom (__b);
vconnect ("fermion,label=$\nu _\ell /\bar {\nu }_\ell $", __i2, __v1);
vconnect ("fermion,label=$\nu _\ell /\bar {\nu }_\ell $", __v1, __o2);
vconnect ("boson,label=$Z^0$", __v1, __b);
vfreeze;
vdraw;
endsubgraph;
endchar;
% the end.
end.
endinput;
