#include <mex.h>
#include "PHY/defs.h"
#include "PHY/LTE_TRANSPORT/extern.h"
//#include "PHY/LTE_TRANSPORT/proto.h"

/*==============================================================================
* get_tbs.c
*
* Returns the transport block size (TBS) given the MCS and the number of RBs.
*
* example: tbs = get_tbs(16,25)
*
* Author: Sebastian Wagner
* Date: 23-07-2012
*
===============================================================================*/

//unsigned char get_I_TBS(unsigned char I_MCS);

#if 0
unsigned char get_I_TBS(unsigned char I_MCS)
{

  if (I_MCS < 10)
    return(I_MCS);
  else if (I_MCS == 10)
    return(9);
  else if (I_MCS < 17)
    return(I_MCS-1);
  else if (I_MCS == 17)
    return(15);
  else return(I_MCS-2);

}
#endif

void mexGetTbs( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[]
                )
{

    /* Declare */
  unsigned char mcs, nb_rb, Nl;
  unsigned int *tbs;
  mxArray *tmp;

  /* Check number of inputs */
  if(nrhs!=2) {
    mexErrMsgTxt("Two inputs required.");
  } else if(nlhs>1) {
    mexErrMsgTxt("Too many output arguments.");
  }



  tmp = mxGetField(prhs[0],0,"nb_rb");
  if (tmp == NULL) {
    mexErrMsgTxt("Non-existing field 'nb_rb' in input argument 2.");
  } else {
    nb_rb = (unsigned char) mxGetScalar(tmp);
  }

  tmp = mxGetField(prhs[1],0,"mcs");
  if (tmp == NULL) {
    mexErrMsgTxt("Non-existing field 'mcs' in input argument 2.");
  } else {
    mcs = (unsigned char) mxGetScalar(tmp);
  }

  tmp = mxGetField(prhs[0],0,"num_layers");
  if (tmp == NULL) {
    mexErrMsgTxt("Non-existing field 'num_layers' in input argument 2.");
  } else {
    Nl = (unsigned char) mxGetScalar(tmp);
  }

  /* Check input */
  if((mcs < 0) || (mcs > 28)) {
    mexErrMsgTxt("Wrong MCS index.");
  } else if((nb_rb<=0) || (nb_rb>25)) {
    mexErrMsgTxt("Unsupported number of RBs.");
  };

  /* Allocate Output */
  plhs[0] = mxCreateNumericMatrix(1, 1, mxUINT32_CLASS, mxREAL);
  tbs = (unsigned int*) mxGetPr(plhs[0]);

    /* Algo */
  *tbs = get_TBS_DL(mcs,Nl*nb_rb)<<3;

}
