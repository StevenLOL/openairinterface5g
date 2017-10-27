#include "mex.h"

#include "PHY/vars.h"

/* list of defined function IDs */
#define FUN_ID_DlschDecodingInit                  0
#define FUN_ID_SetTausSeed                        1
#define FUN_ID_GetTbs                             2
#define FUN_ID_Taus                               3
#define FUN_ID_DlschEncoding                      4
#define FUN_ID_DlschChannelLevelPrec_TM56         5
#define FUN_ID_Log2Approx                         6
#define FUN_ID_DlschChannelCompensation_TM56      7
#define FUN_ID_DlschDualStreamCorrelation         8
#define FUN_ID_DlschDetectionMrc                  9
#define FUN_ID_DlschIALLR                        10
#define FUN_ID_DlschDecoding                     11
#define FUN_ID_DlschChannelLevel                 12
#define FUN_ID_DlschChannelCompensation          13
#define FUN_ID_DlschSISOLLR                      14

/* declarations of defined functions */
void mexDlschDecodingInit(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
void mexSetTausSeed(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
void mexGetTbs(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
void mexTaus(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
void mexDlschEncoding(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
void mexDlschChannelLevelPrec_TM56(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
void mexLog2Approx(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
void mexDlschChannelCompensation_TM56(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
void mexDlschDualStreamCorrelation(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
void mexDlschDetectionMrc(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
void mexDlschIALLR(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
void mexDlschDecoding(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
void mexDlschChannelLevel(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
void mexDlschChannelCompensation(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);
void mexDlschSISOLLR(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]);

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[]
                )
{
  if (nrhs < 1) mexErrMsgTxt("mexOAI: bad call: no argument");
  if (!mxIsDouble(prhs[0]) || mxIsComplex(prhs[0]) || mxGetNumberOfElements(prhs[0]) != 1)
    mexErrMsgTxt("mexOAI: bad call: first argument not a number");

  int fun_id = mxGetScalar(prhs[0]);

  switch (fun_id) {
  case FUN_ID_DlschDecodingInit:                 mexDlschDecodingInit(nlhs, plhs, nrhs-1, prhs+1);                 break;
  case FUN_ID_SetTausSeed:                       mexSetTausSeed(nlhs, plhs, nrhs-1, prhs+1);                       break;
  case FUN_ID_GetTbs:                            mexGetTbs(nlhs, plhs, nrhs-1, prhs+1);                            break;
  case FUN_ID_Taus:                              mexTaus(nlhs, plhs, nrhs-1, prhs+1);                              break;
  case FUN_ID_DlschEncoding:                     mexDlschEncoding(nlhs, plhs, nrhs-1, prhs+1);                     break;
  case FUN_ID_DlschChannelLevelPrec_TM56:        mexDlschChannelLevelPrec_TM56(nlhs, plhs, nrhs-1, prhs+1);        break;
  case FUN_ID_Log2Approx:                        mexLog2Approx(nlhs, plhs, nrhs-1, prhs+1);                        break;
  case FUN_ID_DlschChannelCompensation_TM56:     mexDlschChannelCompensation_TM56(nlhs, plhs, nrhs-1, prhs+1);     break;
  case FUN_ID_DlschDualStreamCorrelation:        mexDlschDualStreamCorrelation(nlhs, plhs, nrhs-1, prhs+1);        break;
  case FUN_ID_DlschDetectionMrc:                 mexDlschDetectionMrc(nlhs, plhs, nrhs-1, prhs+1);                 break;
  case FUN_ID_DlschIALLR:                        mexDlschIALLR(nlhs, plhs, nrhs-1, prhs+1);                        break;
  case FUN_ID_DlschDecoding:                     mexDlschDecoding(nlhs, plhs, nrhs-1, prhs+1);                     break;
  case FUN_ID_DlschChannelLevel:                 mexDlschChannelLevel(nlhs, plhs, nrhs-1, prhs+1);                 break;
  case FUN_ID_DlschChannelCompensation:          mexDlschChannelCompensation(nlhs, plhs, nrhs-1, prhs+1);          break;
  case FUN_ID_DlschSISOLLR:                      mexDlschSISOLLR(nlhs, plhs, nrhs-1, prhs+1);                      break;
  default:
    mexErrMsgTxt("mexOAI: unknown function ID");
  }
}
