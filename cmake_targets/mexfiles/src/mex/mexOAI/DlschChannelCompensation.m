function [ y H mag1 magb1 ] = DlschChannelCompensation(y_fxp, H_fxp, simparms, codeword, sym)
  [ y H mag1 magb1 ] = mexOAI(13, y_fxp, H_fxp, simparms, codeword, sym);
end
