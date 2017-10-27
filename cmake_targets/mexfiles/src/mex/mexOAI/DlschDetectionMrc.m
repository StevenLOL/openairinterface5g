function [ y0 y1 H0 Hb0 H1 Hb1 rho ] = DlschDetectionMrc(ymf0,ymf1,Hmag0,Hmag1,Hmagb0,Hmagb1,rho10,simparms,sym)
  [ y0 y1 H0 Hb0 H1 Hb1 rho ] = mexOAI(9, ymf0,ymf1,Hmag0,Hmag1,Hmagb0,Hmagb1,rho10,simparms,sym);
end
