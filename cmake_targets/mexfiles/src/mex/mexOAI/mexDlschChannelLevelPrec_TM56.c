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

void mexDlschChannelLevelPrec_TM56
                ( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[]
                )
{

    /* Declare */
    int *H,*dl_ch_estimates_ext[4];
    unsigned char *pmi_ext;
    int symbol=5;
    int *avg;
    unsigned int nb_re_per_frame=12;
    mxArray *tmp;
    PHY_VARS_eNB *eNB;


    /* Check proper input and output. */
    if(nrhs!=3)
        mexErrMsgTxt("Three inputs required.");
    else if(nlhs > 1)
        mexErrMsgTxt("Too many output arguments.");
    else if(!mxIsStruct(prhs[2]))
        mexErrMsgTxt("Third input must be a structure.");

    if(!mxIsInt16(prhs[0]))
        mexErrMsgTxt("First argument must belong to Int16 class.");
    if(!mxIsUint8(prhs[1]))
        mexErrMsgTxt("Second argument must belong to Uint8 class.");


    /*create a local eNB structure to fill with frame parameters to pass to functions inside this mex function. At the end of the mex function these structures are freed.*/

    eNB = mxCalloc(1,sizeof(PHY_VARS_eNB));

    /* Allocate input */
    H = (int*) mxGetData(prhs[0]);
    pmi_ext = (unsigned char*) mxGetData(prhs[1]);

    // Create a LTE_DL_FRAME_PARMS structure and assign required params

    tmp = mxGetField(prhs[2],0,"nb_rb");
    if (tmp == NULL) {
        mexErrMsgTxt("Non-existing field 'nb_rb' in input argument 3.");
    } else {
        eNB->frame_parms.N_RB_DL = (unsigned char) mxGetScalar(tmp);
    }
    tmp = mxGetField(prhs[2],0,"nb_antennas_rx");
    if (tmp == NULL) {
        mexErrMsgTxt("Non-existing field 'nb_antennas_rx' in input argument 3.");
    } else {
        eNB->frame_parms.nb_antennas_rx = (unsigned char) mxGetScalar(tmp);
    }
    tmp = mxGetField(prhs[2],0,"mode1_flag");
    if (tmp == NULL) {
        mexErrMsgTxt("Non-existing field 'mode1_flag' in input argument 3.");
    } else {
        eNB->frame_parms.mode1_flag = (unsigned char) mxGetScalar(tmp);
    }
    tmp = mxGetField(prhs[2],0,"Ncp");
    if (tmp == NULL) {
        mexErrMsgTxt("Non-existing field 'Ncp' in input argument 3.");
    } else {
        eNB->frame_parms.Ncp = (unsigned char) mxGetScalar(tmp);
    }

    // Adapt the channel estimates
    tmp = mxGetField(prhs[2],0,"nb_re_per_frame");
    if (tmp == NULL) {
        mexErrMsgTxt("Non-existing field 'nb_re_per_frame' in input argument 3.");
    } else {
        nb_re_per_frame = (unsigned int) mxGetScalar(tmp);
    }
    dl_ch_estimates_ext[0] = (int*) H;
    dl_ch_estimates_ext[1] = (int*) &H[nb_re_per_frame];
    dl_ch_estimates_ext[2] = (int*) &H[2*nb_re_per_frame];
    dl_ch_estimates_ext[3] = (int*) &H[3*nb_re_per_frame];


    /* Allocate Output */
    plhs[0] = mxCreateNumericMatrix(1, 1, mxINT32_CLASS, mxREAL);
    avg = (int*) mxGetPr(plhs[0]);

    /* Algo */
    //dlsch_channel_level_prec(dl_ch_estimates_ext, frame_parms, pmi_ext, avg, symbol_mod, frame_parms->N_RB_DL);
    dlsch_channel_level_TM56(dl_ch_estimates_ext,
                             &eNB->frame_parms,
                             pmi_ext,
                             avg,
                             symbol,
                             eNB->frame_parms.N_RB_DL);

    /* free */
    mxFree(eNB);
}
