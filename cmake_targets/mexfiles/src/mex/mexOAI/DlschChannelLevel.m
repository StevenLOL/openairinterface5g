function [ avg ] = DlschChannelLevel(H_fxp, simparms)
  avg = mexOAI(12, H_fxp, simparms);
end
