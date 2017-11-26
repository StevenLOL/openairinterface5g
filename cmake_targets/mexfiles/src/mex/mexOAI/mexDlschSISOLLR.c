#include "mex.h"
#include "PHY/defs.h"
//#include "PHY/vars.h"

/*==============================================================================
* dlsch_mu_mimo_llr.c
*
* Returns the LLRs for interference aware receiver.
*
* example: llr = dlsch_mu_mimo_llr(ymf0,ymf1,Hmag0,Hmag1,rho10,simparms,slot);
*
* Author: Sebastian Wagner
* Date: 24-07-2012
*
===============================================================================*/

//#define DEBUG_SISO_LLR
#define LLR_GUARD 48

void mexDlschSISOLLR( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[]
                )
{
  /* Declare */
  short *ymf0, *hmag0, *hmagb, *llr;
  short *llr_16 __attribute__ ((aligned(16)));
  unsigned char mod_order=2;
  unsigned char symbol, symbol_mod;
  int nb_re_per_symbol,i;
  mxArray *tmp;
  PHY_VARS_UE *ue;
  int len, len2;
  int len_mod4;




  if(nrhs!=5)
    mexErrMsgTxt("5 inputs required.");
  else if(nlhs > 1)
    mexErrMsgTxt("Too many output arguments.");
  else if(!mxIsStruct(prhs[3]))
    mexErrMsgTxt("4 input must be a structure.");

  if(!mxIsInt16(prhs[0]))
    mexErrMsgTxt("First argument must belong to Int16 class.");

  if(!mxIsInt16(prhs[1]))
    mexErrMsgTxt("Second argument must belong to Int16 class.");

  if(!mxIsInt16(prhs[2]))
    mexErrMsgTxt("Third argument must belong to Int16 class.");

  ue = mxCalloc(1,sizeof(PHY_VARS_UE));

  /* Allocate input */
  ymf0 = (short*) mxGetData(prhs[0]);
  hmag0 = (short*) mxGetData(prhs[1]);
  hmagb = (short*) mxGetData(prhs[2]);
  symbol = (unsigned char) mxGetScalar(prhs[4]);

  tmp = mxGetField(prhs[3],0,"cw");
  if (tmp == NULL) {
    mexErrMsgTxt("Non-existing field 'cw' in input argument 4");
  } else {
    tmp = mxGetField(mxGetField(prhs[3],0,"cw"),0,"mod_order");
    if (tmp == NULL) {
      mexErrMsgTxt("Non-existing field 'mod_order' in input argument 4 'cw'.");
    } else {
      mod_order = (unsigned char) mxGetScalar(tmp);
    }
  }

  tmp = mxGetField(prhs[3],0,"nb_rb");
  if (tmp == NULL) {
    mexErrMsgTxt("Non-existing field 'nb_rb' in input argument 4");
  } else {
    ue->frame_parms.N_RB_DL = (unsigned char) mxGetScalar(tmp);
  }
  tmp = mxGetField(prhs[3],0,"nb_antennas_rx");
  if (tmp == NULL) {
    mexErrMsgTxt("Non-existing field 'nb_antennas_rx' in input argument 4.");
  } else {
    ue->frame_parms.nb_antennas_rx = (unsigned char) mxGetScalar(tmp);
  }
  tmp = mxGetField(prhs[3],0,"nb_antennas_tx");
  if (tmp == NULL) {
    mexErrMsgTxt("Non-existing field 'nb_antennas_tx' in input argument 4.");
  } else {
    ue->frame_parms.nb_antennas_tx = (unsigned char) mxGetScalar(tmp);
  }
  tmp = mxGetField(prhs[3],0,"Ncp");
  if (tmp == NULL) {
    mexErrMsgTxt("Non-existing field 'Ncp' in input argument 4.");
  } else {
    ue->frame_parms.Ncp = (unsigned char) mxGetScalar(tmp);
  }

  // Adapt the channel estimates and receive signal
  symbol_mod = (symbol>=(7-ue->frame_parms.Ncp)) ? symbol-(7-ue->frame_parms.Ncp) : symbol;

  if ((symbol_mod == 0) || (symbol_mod == (4-ue->frame_parms.Ncp)))
    nb_re_per_symbol = ue->frame_parms.N_RB_DL*8; // pilots
  else
    nb_re_per_symbol = ue->frame_parms.N_RB_DL*12;

  plhs[0] = mxCreateNumericMatrix(mod_order*nb_re_per_symbol,1, mxINT16_CLASS, mxREAL);
  llr = (short*) mxGetPr(plhs[0]);

#ifdef DEBUG_SISO_LLR
  mexPrintf("mod_order: %d\n", mod_order);
  mexPrintf("frame_parms->N_RB_DL %d\n", ue->frame_parms.N_RB_DL);
  mexPrintf("frame_parms->nb_antennas_rx %d\n", ue->frame_parms.nb_antennas_rx);
  mexPrintf("frame_parms->nb_antennas_tx %d\n", ue->frame_parms.nb_antennas_tx);
  mexPrintf("frame_parms->Ncp %d\n", ue->frame_parms.Ncp);
  mexPrintf("nb_re_per_symbol %d\n", nb_re_per_symbol);
  mexPrintf("ue->frame_parms %p\n", ue->frame_parms);

  for (i=0;i<(8*2);i++) {
    mexPrintf("i=%d\n",i);
    mexPrintf("ymf0 = %d\n",ymf0[i]);
    mexPrintf("hmag0 = %d\n",hmag0[i]);
    mexPrintf("hmagb = %d\n",hmagb[i]);
  }
#endif
  //16 bit aligned memory allocation with guard samples
  //llr_16 = (short *) calloc(1,(mod_order*nb_re_per_symbol+LLR_GUARD)*sizeof(short));
 llr_16 = (short *) malloc16((mod_order*nb_re_per_symbol+LLR_GUARD)*sizeof(short));
    /* Algo */
  switch (mod_order) {
    case 2 :
      qpsk_llr(ymf0, llr_16, nb_re_per_symbol);
      break;
    case 4 :
      len = nb_re_per_symbol;
      len_mod4 = len&3;
      //printf("len_mod4=%d\n", len_mod4);
      len>>=2;  // length in quad words (4 REs)
      //printf("len>>=2=%d\n", len);
      len+=(len_mod4==0 ? 0 : 1);

      qam16_llr(ymf0,
                hmag0,
                llr_16,
                len);
      break;
    case 6 :
      len = nb_re_per_symbol;
      len_mod4 =len&3;
      len2=len>>2;  // length in quad words (4 REs)
      len2+=((len_mod4==0)?0:1);

      qam64_llr(ymf0,
                hmag0,
                hmagb,
                llr_16,
                len2);
      break;
    default :
      mexErrMsgTxt("Unknown mod_order.");
      break;
  }

#ifdef DEBUG_SISO_LLR
  for (i=0;i<(mod_order*nb_re_per_symbol);i++) {
    mexPrintf("llr_16 [%d]=%d\n",i, llr_16[i]);
  }
  for (i=0;i<(mod_order*nb_re_per_symbol);i++) {
    mexPrintf("llr [%d]=%d\n",i, llr[i]);
  }
#endif
   // copy just valid LLRs to output
  memcpy((void*) llr, (void *) llr_16, mod_order*nb_re_per_symbol*sizeof(short));

#ifdef DEBUG_SISO_LLR
   for (i=0;i<(mod_order*nb_re_per_symbol);i++) {
    mexPrintf("llr [%d]=%d\n",i, llr[i]);
  }
#endif

  mxFree(ue);

  free(llr_16);

}
