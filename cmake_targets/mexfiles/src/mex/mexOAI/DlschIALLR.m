function llr = DlschIALLR(ymf0, ymf1, H0, H1, rho, simparms, sym)
  llr = mexOAI(10, ymf0, ymf1, H0, H1, rho, simparms, sym);
end
