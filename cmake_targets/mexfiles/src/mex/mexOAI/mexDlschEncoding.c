#include "mex.h"
#include "PHY/defs.h"
//#include "PHY/vars.h"
#include "PHY/extern.h"
#include "PHY/CODING/defs.h"
#include "PHY/CODING/extern.h"
//#include "PHY/CODING/lte_interleaver_inline.h"
#include "SIMULATION/LTE_PHY/unitary_defs.h"
//#include "PHY/CODING/lte_interleaver.h"
#include "PHY/LTE_TRANSPORT/defs.h"
#include "PHY/LTE_TRANSPORT/proto.h"
#include "SCHED/defs.h"
#include "defs.h"
#include "UTIL/LOG/vcd_signal_dumper.h"
#include "UTIL/LOG/log.h"
#include <syscall.h>

/*==============================================================================
* dlsch_encoding.c
*
* Returns the encoded bit sequence.
*
* example: e = dlsch_encoding(bits,struct1,struct2)
*
* Author: Sebastian Wagner
* Date: 23-07-2012
*
===============================================================================*/

 //#define DEBUG_DLSCH_ENCODING

void mexDlschEncoding( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[]
                )
{

    /* Declare */
  unsigned char *input_buffer;
  unsigned char *e; // encoded bits = output
  unsigned char mcs=0;
  unsigned char mod_order=2;
  unsigned char num_pdcch_symbols=1;
  unsigned char harq_pid=0;
  unsigned char subframe=0;
  unsigned char Kmimo=1;
  unsigned char n_tx=1;
  unsigned char Mdlharq=8;
  unsigned char abstraction_flag = 0;
  unsigned int ofdm_symbol_size=512;
  int input_buffer_length;
  unsigned int G;
  unsigned int Nsoft = 1827072;
  mxArray *tmp;
  LTE_eNB_DLSCH_t* dlsch;
  PHY_VARS_eNB *eNB;
  time_stats_t dlsch_rate_matching_stats, dlsch_turbo_encoding_stats, dlsch_interleaving_stats;

  /* Check proper input and output. */
  if(nrhs!=3)
    mexErrMsgTxt("Three inputs required.");
  else if(nlhs > 1)
    mexErrMsgTxt("Too many output arguments.");
  else if(!mxIsStruct(prhs[1]))
    mexErrMsgTxt("Second input must be a structure.");
  else if(!mxIsStruct(prhs[2]))
    mexErrMsgTxt("Third input must be a structure.");

  crcTableInit();

  /*create a local eNB structure to fill with frame parameters to pass to functions inside this mex function. At the end of the mex function these structures are freed.*/

  eNB = mxCalloc(1,sizeof(PHY_VARS_eNB));


  /* Extract input parameters from prhs to pass to new new_eNB_dlsch */
  tmp = mxGetField(prhs[2],0,"Kmimo");
  if (tmp == NULL) {
    mexErrMsgTxt("Non-existing field 'Kmimo' in input argument 3.");
  } else {
    Kmimo = (unsigned char) mxGetScalar(tmp);
  }

  tmp = mxGetField(prhs[2],0,"Mdlharq");
  if (tmp == NULL) {
    mexErrMsgTxt("Non-existing field 'Mdlharq' in input argument 3.");
  } else {
    Mdlharq = (unsigned char) mxGetScalar(tmp);
  }

  tmp = mxGetField(prhs[1],0,"abstraction_flag");
  if (tmp == NULL) {
    mexErrMsgTxt("Non-existing field 'abstraction_flag' in input argument 2.");
  } else {
  abstraction_flag = (unsigned char) mxGetScalar(tmp);
  }

  tmp = mxGetField(prhs[1],0,"Nsoft");
  if (tmp == NULL) {
    mexErrMsgTxt("Non-existing field 'Nsoft' in input argument 2.");
  } else {
  Nsoft = (int) mxGetScalar(tmp);
  }

  tmp = mxGetField(prhs[1],0,"NB_ANTENNAS_TX");
  if (tmp == NULL) {
    mexErrMsgTxt("Non-existing field 'NB_ANTENNAS_TX' in input argument 2.");
  } else {
    n_tx = (unsigned char) mxGetScalar(tmp);
  }

  eNB->frame_parms.nb_antennas_tx = n_tx;

  tmp = mxGetField(prhs[1],0,"ofdm_symbol_size");
  if (tmp == NULL) {
    mexErrMsgTxt("Non-existing field 'ofdm_symbol_size' in input argument 2.");
  } else {
    ofdm_symbol_size = (unsigned char) mxGetScalar(tmp);
   }

  eNB->frame_parms.ofdm_symbol_size = ofdm_symbol_size;

  /* Create new dlsch*/
  dlsch = new_eNB_dlsch(Kmimo,Mdlharq,Nsoft, 25,abstraction_flag, &eNB->frame_parms);

  /* Allocate input buffer*/
  input_buffer = (unsigned char*) mxGetPr(prhs[0]);

  (mxGetM(prhs[0]) > 1) ? (input_buffer_length = mxGetM(prhs[0])) : (input_buffer_length = mxGetN(prhs[0]));

  /* Extract parameters to fill dlsch->current_harq_pid */
  tmp = mxGetField(prhs[2],0,"mcs");
  if (tmp == NULL) {
    mexErrMsgTxt("Non-existing field 'mcs' in input argument 3.");
  } else {
    mcs = (unsigned char) mxGetScalar(tmp);
  }

  tmp = mxGetField(prhs[2],0,"harq_pid");
  if (tmp == NULL) {
    mexErrMsgTxt("Non-existing field 'harq_pid' in input argument 3.");
  } else {
    harq_pid = (unsigned char) mxGetScalar(tmp);
  }

  dlsch->current_harq_pid = harq_pid;
  dlsch->harq_processes[harq_pid]->mcs = mcs;
  dlsch->Mdlharq = Mdlharq;
  dlsch->Nsoft = Nsoft;
  dlsch->Kmimo = Kmimo;

  /* Extract parameters to fill dlsch->current_harq_pid */
  tmp = mxGetField(prhs[2],0,"rvidx");
  if (tmp == NULL) {
    mexErrMsgTxt("Non-existing field 'rvidx' in input argument 3.");
  } else {
    dlsch->harq_processes[harq_pid]->rvidx = (unsigned char) mxGetScalar(tmp);
  }
  tmp = mxGetField(prhs[1],0,"num_layers");
  if (tmp == NULL) {
    mexErrMsgTxt("Non-existing field 'num_layers' in input argument 2.");
  } else {
    dlsch->harq_processes[harq_pid]->Nl = (unsigned char) mxGetScalar(tmp);
  }
  tmp = mxGetField(prhs[2],0,"round");
  if (tmp == NULL) {
    mexErrMsgTxt("Non-existing field 'round' in input argument 3.");
  } else {
    dlsch->harq_processes[harq_pid]->round = (unsigned char) mxGetScalar(tmp);
  }
  tmp = mxGetField(prhs[1],0,"rb_alloc");
  if (tmp == NULL) {
    mexErrMsgTxt("Non-existing field 'rb_alloc' in input argument 2.");
  } else {
    dlsch->harq_processes[harq_pid]->rb_alloc[0] = (unsigned int) mxGetScalar(tmp);
  }

  tmp = mxGetField(prhs[1],0,"nb_rb");
  if (tmp == NULL) {
    mexErrMsgTxt("Non-existing field 'nb_rb' in input argument 2.");
  } else {
    dlsch->harq_processes[harq_pid]->nb_rb = (unsigned short) mxGetScalar(tmp);
  }

  dlsch->harq_processes[harq_pid]->TBS = get_TBS_DL(mcs,dlsch->harq_processes[harq_pid]->Nl*dlsch->harq_processes[harq_pid]->nb_rb)<<3;

  // Check if input buffer length is correct
  if ((input_buffer_length) != (dlsch->harq_processes[harq_pid]->TBS/8+4)) {
    free_eNB_dlsch(dlsch);
    mexErrMsgTxt("Length of the input buffer is incorrect.");
  }

  tmp = mxGetField(prhs[1],0,"num_pdcch_symbols");
  if (tmp == NULL) {
    mexErrMsgTxt("Non-existing field 'num_pdcch_symbols' in input argument 2.");
  } else {
    num_pdcch_symbols = (unsigned char) mxGetScalar(tmp);
  }
  tmp = mxGetField(prhs[1],0,"subframe");
  if (tmp == NULL) {
    mexErrMsgTxt("Non-existing field 'subframe' in input argument 2.");
  } else {
    subframe = (unsigned char) mxGetScalar(tmp);
  }

  // Create a LTE_DL_FRAME_PARMS structure and assign required params
  //frame_parms = mxCalloc(1,sizeof(LTE_DL_FRAME_PARMS));
  eNB->frame_parms.N_RB_DL = (unsigned char) dlsch->harq_processes[harq_pid]->nb_rb;

  tmp = mxGetField(prhs[1],0,"frame_type");
  if (tmp == NULL) {
    mexErrMsgTxt("Non-existing field 'frame_type' in input argument 2.");
  } else {
    eNB->frame_parms.frame_type = (unsigned char) mxGetScalar(tmp);
  }


  tmp = mxGetField(prhs[1],0,"mode1_flag");
  if (tmp == NULL) {
    mexErrMsgTxt("Non-existing field 'frame_type' in input argument 2.");
  } else {
    eNB->frame_parms.mode1_flag = (unsigned char) mxGetScalar(tmp);
  }

  tmp = mxGetField(prhs[1],0,"Ncp");
  if (tmp == NULL) {
    mexErrMsgTxt("Non-existing field 'frame_type' in input argument 2.");
  } else {
    eNB->frame_parms.Ncp = (unsigned char) mxGetScalar(tmp);
  }
  mod_order = get_Qm(dlsch->harq_processes[harq_pid]->mcs);

  G = get_G(&eNB->frame_parms,dlsch->harq_processes[harq_pid]->nb_rb,dlsch->harq_processes[harq_pid]->rb_alloc,mod_order,dlsch->harq_processes[harq_pid]->Nl,num_pdcch_symbols,0,subframe,0);
#ifdef DEBUG_DLSCH_ENCODING
  mexPrintf("I am in mexencoding!!! \n");
  mexPrintf("dlsch->current_harq_pid = %d\n", dlsch->current_harq_pid);
  mexPrintf("dlsch->harq_processes[harq_pid]->rvidx = %d\n", dlsch->harq_processes[harq_pid]->rvidx);
  mexPrintf("dlsch->harq_processes[harq_pid]->Nl = %d\n", dlsch->harq_processes[harq_pid]->Nl);
  mexPrintf("mod_order = %d\n",mod_order);
  mexPrintf("G = %d\n",G);
  mexPrintf("eNB->frame_parms.N_RB_DL = %d\n", eNB->frame_parms.N_RB_DL);
  mexPrintf("eNB->frame_parms.frame_type = %d\n",eNB->frame_parms.frame_type);
  mexPrintf("eNB->frame_parms.mode1_flag = %d\n", eNB->frame_parms.mode1_flag);
  mexPrintf("eNB->frame_parms.Ncp = %d\n", eNB->frame_parms.Ncp);
#endif
  /* Allocate Output */
  plhs[0] = mxCreateNumericMatrix(G, 1, mxUINT8_CLASS, mxREAL);
  e = (unsigned char*) mxGetPr(plhs[0]);
    /* Algo */


  dlsch_encoding(eNB, input_buffer, num_pdcch_symbols, dlsch, 0, subframe,&dlsch_rate_matching_stats, &dlsch_turbo_encoding_stats, &dlsch_interleaving_stats);
  /* Assign output */
  memcpy((void*) e, (void*) dlsch->harq_processes[harq_pid]->e,G);

  /* free */
  free_eNB_dlsch(dlsch);
  mxFree(eNB);
}
