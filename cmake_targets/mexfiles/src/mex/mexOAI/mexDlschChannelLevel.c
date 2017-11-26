#include "mex.h"
#include "PHY/defs.h"
//#include "PHY/vars.h"
#include "PHY/extern.h"
/*==============================================================================
* dlsch_channel_level_prec.c
*
* Returns average squared channel magnitude of effective channel.
*
* example: avg = dlsch_channel_level_prec(H,pmi,simparms);
*
* Author: Sebastian Wagner
* Date: 23-07-2012
*
===============================================================================*/

//#define DEBUG_CHANNEL_LEVEL

void mexDlschChannelLevel
                ( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[]
                )
{

  int *H,*dl_ch_estimates_ext[16];
  int symbol=5;
  int *avg;
  unsigned int nb_re_per_frame=12, ii;
  mxArray *tmp;
  PHY_VARS_eNB *eNB;

  if(nrhs!=2)
    mexErrMsgTxt("Three inputs required.");
  else if(nlhs > 1)
    mexErrMsgTxt("Too many output arguments.");
  else if(!mxIsStruct(prhs[1]))
    mexErrMsgTxt("Second input must be a structure.");

  if(!mxIsInt16(prhs[0]))
    mexErrMsgTxt("First argument must belong to Int16 class.");


  /*create a local eNB structure to fill with frame parameters to pass to functions inside this mex function. At the end of the mex function these structures are freed.*/
  eNB = mxCalloc(1,sizeof(PHY_VARS_eNB));

  /* Allocate input */
  H = (int*)mxGetData(prhs[0]);

  // Create a LTE_DL_FRAME_PARMS structure and assign required params
  tmp = mxGetField(prhs[1],0,"nb_rb");
  if (tmp == NULL) {
    mexErrMsgTxt("Non-existing field 'nb_rb' in input argument 2.");
  } else {
    eNB->frame_parms.N_RB_DL = (unsigned char) mxGetScalar(tmp);
  }

  tmp = mxGetField(prhs[1],0,"nb_antennas_rx");
  if (tmp == NULL) {
    mexErrMsgTxt("Non-existing field 'nb_antennas_rx' in input argument 2.");
  } else {
    eNB->frame_parms.nb_antennas_rx = (unsigned char) mxGetScalar(tmp);
  }

  tmp = mxGetField(prhs[1],0,"nb_antennas_tx");
  if (tmp == NULL) {
    mexErrMsgTxt("Non-existing field 'nb_antennas_tx' in input argument 2.");
  } else {
    eNB->frame_parms.nb_antenna_ports_eNB = (unsigned char) mxGetScalar(tmp);
  }

  tmp = mxGetField(prhs[1],0,"mode1_flag");
  if (tmp == NULL) {
    mexErrMsgTxt("Non-existing field 'mode1_flag' in input argument 2.");
  } else {
    eNB->frame_parms.mode1_flag = (unsigned char) mxGetScalar(tmp);
  }

  tmp = mxGetField(prhs[1],0,"Ncp");
  if (tmp == NULL) {
    mexErrMsgTxt("Non-existing field 'Ncp' in input argument 2.");
  } else {
    eNB->frame_parms.Ncp = (unsigned char) mxGetScalar(tmp);
  }

  tmp = mxGetField(prhs[1],0,"nb_re_per_frame");
  if (tmp == NULL) {
    mexErrMsgTxt("Non-existing field 'nb_re_per_frame' in input argument 2.");
  } else {
    nb_re_per_frame = (unsigned int) mxGetScalar(tmp);
  }

  dl_ch_estimates_ext[0] = (int*) H;

  for (ii=1; ii<eNB->frame_parms.nb_antennas_rx*eNB->frame_parms.nb_antenna_ports_eNB; ++ii){
    dl_ch_estimates_ext[ii] = (int*) &H[ii*nb_re_per_frame];
  }


  /* Allocate Output */
  plhs[0] = mxCreateNumericMatrix(4,4, mxINT32_CLASS, mxREAL);
  avg = (int*) mxGetPr(plhs[0]);

    /* Algo */
  dlsch_channel_level(dl_ch_estimates_ext,
                      &eNB->frame_parms,
                      avg,
                      symbol,
                      eNB->frame_parms.N_RB_DL);

  #ifdef DEBUG_CHANNEL_LEVEL
    /* Algo */
  mexPrintf("Rx = %d\n", eNB->frame_parms.nb_antennas_rx);
  mexPrintf("Tx = %d\n", eNB->frame_parms.nb_antenna_ports_eNB);
  for (ii=0; ii<eNB->frame_parms.nb_antennas_rx*eNB->frame_parms.nb_antenna_ports_eNB; ++ii)
  {
    mexPrintf("Avg %d = %d\n", ii, avg[ii]);
    mexPrintf("Ch_Ests %d = %d\n", ii, *dl_ch_estimates_ext[ii]);
  }
  mexPrintf("Number of REs = %d\n", nb_re_per_frame);
  mexPrintf("Sym_Mod = %d\n", symbol);
  mexPrintf("No_RB = %d\n", frame_parms->N_RB_DL);
  #endif

  mxFree(eNB);
}
