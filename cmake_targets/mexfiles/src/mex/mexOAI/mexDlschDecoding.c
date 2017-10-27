/*#include "mex.h"
#include "PHY/defs.h"
#include "PHY/vars.h"
#include "PHY/extern.h"
#include "PHY/LTE_TRANSPORT/defs.h"
#include <stdint.h>
#include <stdlib.h>
#include "PHY/CODING/defs.h"
#include "PHY/CODING/lte_interleaver_inline.h"
#include "SIMULATION/LTE_PHY/unitary_defs.h"
#include "PHY/CODING/lte_interleaver.h"*/
#include <stdint.h>
#include "PHY/CODING/defs.h"
//#include "PHY/CODING/vars.h"
#include <stdlib.h>
#include "PHY/defs.h"
//#include "PHY/vars.h"
#include "PHY/extern.h"
#include "mex.h"


//#include "PHY/CODING/vars.h"

/*==============================================================================
* dlsch_encoding.c
*
* Returns the decoded bit sequence.
*
* example: e = dlsch_encoding(llrs,struct1,struct2)
*
* Author: Sebastian Wagner
* Date: 23-07-2012
*
===============================================================================*/

 //#define DEBUG_DLSCH_DECODING

void mexDlschDecoding( int mlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[]
                )
{
  short *dlsch_llr;
  unsigned char mcs;
  unsigned int *ret;
  unsigned char mod_order;
  unsigned char num_pdcch_symbols;
  unsigned char harq_pid;
  unsigned char subframe;
  unsigned char Kmimo;
  unsigned char Mdlharq;
  unsigned char abstraction_flag;
  int Nsoft;
  LTE_UE_DLSCH_t* dlsch;
  PHY_VARS_UE *ue;
  PHY_VARS_UE *phy_vars_ue;

  ue = mxCalloc(1,sizeof(PHY_VARS_UE));

  /* Allocate input */
  dlsch_llr = (short*) mxGetData(prhs[0]);


  Kmimo = (unsigned char) mxGetScalar(mxGetField(prhs[2],0,"Kmimo"));
  Mdlharq = (unsigned char) mxGetScalar(mxGetField(prhs[2],0,"Mdlharq"));
  abstraction_flag = (unsigned char) mxGetScalar(mxGetField(prhs[1],0,"abstraction_flag"));
  mcs = (unsigned char) mxGetScalar(mxGetField(prhs[2],0,"mcs"));
  Nsoft = (int) mxGetScalar(mxGetField(prhs[1],0,"Nsoft"));


  ue->frame_parms.nb_antennas_tx = (unsigned char) mxGetScalar(mxGetField(prhs[1],0,"NB_ANTENNAS_TX"));
  ue->frame_parms.ofdm_symbol_size = (unsigned char) mxGetScalar(mxGetField(prhs[1],0,"ofdm_symbol_size"));

#ifdef DEBUG_DLSCH_DECODING
  mexPrintf("Kmimo %d\n",Kmimo);
  mexPrintf("Mdlharq %d\n",Mdlharq);
  mexPrintf("abstraction_flag %d\n",abstraction_flag);
  mexPrintf("mcs %d\n",mcs);
  mexPrintf("Nsoft %d\n",Nsoft);
#endif

/*Creating  a local dlsch*/
  dlsch = new_ue_dlsch(Kmimo,Mdlharq,Nsoft, 8, 25,abstraction_flag);

  crcTableInit();

  harq_pid = (unsigned char) mxGetScalar(mxGetField(prhs[2],0,"harq_pid"));

  dlsch->current_harq_pid = harq_pid;
  dlsch->harq_processes[harq_pid]->rvidx = (unsigned char) mxGetScalar(mxGetField(prhs[2],0,"rvidx"));
  dlsch->harq_processes[harq_pid]->Nl = (unsigned char) mxGetScalar(mxGetField(prhs[1],0,"num_layers"));
  dlsch->harq_processes[harq_pid]->mcs = mcs;
  dlsch->harq_processes[harq_pid]->rb_alloc_even[0] = (unsigned int) mxGetScalar(mxGetField(prhs[1],0,"rb_alloc"));
  dlsch->harq_processes[harq_pid]->nb_rb = (unsigned short) mxGetScalar(mxGetField(prhs[1],0,"nb_rb"));
  dlsch->harq_processes[harq_pid]->TBS = get_TBS_DL(dlsch->harq_processes[harq_pid]->mcs,dlsch->harq_processes[harq_pid]->Nl*(dlsch->harq_processes[harq_pid]->nb_rb))<<3;

  num_pdcch_symbols = (unsigned char) mxGetScalar(mxGetField(prhs[1],0,"num_pdcch_symbols"));
  subframe = (unsigned char) mxGetScalar(mxGetField(prhs[1],0,"subframe"));
  phy_vars_ue = mxCalloc(1,sizeof(PHY_VARS_UE));

  ue->frame_parms.N_RB_DL = (unsigned char) mxGetScalar(mxGetField(prhs[1],0,"nb_rb"));
  ue->frame_parms.frame_type = (unsigned char) mxGetScalar(mxGetField(prhs[1],0,"frame_type"));
  ue->frame_parms.mode1_flag = (unsigned char) mxGetScalar(mxGetField(prhs[1],0,"mode1_flag"));
  ue->frame_parms.Ncp = (unsigned char) mxGetScalar(mxGetField(prhs[1],0,"Ncp"));


  dlsch->harq_processes[harq_pid]->Qm = get_Qm(dlsch->harq_processes[harq_pid]->mcs);
  dlsch->harq_processes[harq_pid]->G = get_G(&ue->frame_parms,dlsch->harq_processes[harq_pid]->nb_rb,dlsch->harq_processes[harq_pid]->rb_alloc_even,dlsch->harq_processes[harq_pid]->Qm,dlsch->harq_processes[harq_pid]->Nl,num_pdcch_symbols,0,subframe,0);

  if (dlsch->harq_processes[harq_pid]->G != mxGetM(prhs[0])) {
    free_ue_dlsch(dlsch);
    mxFree(ue);
    free(phy_vars_ue);
    mexErrMsgTxt("Length of the LLR vector is incorrect.");
  }

  /* Allocate Output */
  plhs[0] = mxCreateNumericMatrix(1, 1, mxUINT32_CLASS, mxREAL);
  ret = (unsigned int*) mxGetPr(plhs[0]);

  *ret = dlsch_decoding(phy_vars_ue, dlsch_llr, &ue->frame_parms, dlsch, dlsch->harq_processes[harq_pid], 0, subframe, dlsch->current_harq_pid, 1,0);

  free_ue_dlsch(dlsch);
  mxFree(ue);
  mxFree(phy_vars_ue);
}
