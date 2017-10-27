function [ y H mag1 magb1 ] = DlschChannelCompensation_TM56(y_fxp, H_fxp, pmi, simparms, codeword, sym)
  [ y H mag1 magb1 ] = mexOAI(7, y_fxp, H_fxp, pmi, simparms, codeword, sym);
end
