//#include "../../../defs.h"
#include <stdint.h>
#include "PHY/CODING/defs.h"
//#include "PHY/CODING/vars.h"
#include <stdlib.h>
#include "PHY/defs.h"
#include "PHY/extern.h"
#include "mex.h"

/*==============================================================================
* dlsch_encoding_init.c
*
* Returns the decoded bit sequence.
*
* example: dlsch_decoding_init(struct1)
*
* Author: Sebastian Wagner
* Date: 23-07-2012
*
===============================================================================*/

//#define DEBUG_DLSCH_DECODING_INIT

void mexDlschDecodingInit(
    int mlhs, mxArray *plhs[],
    int nrhs, const mxArray *prhs[])
{
    extern int *pi2tab16[188],*pi5tab16[188],*pi4tab16[188],*pi6tab16[188];
    int i;

    init_td16();

    for (i=0;i<188;i++) {
        mexMakeMemoryPersistent((void *) pi2tab16[i]);
        mexMakeMemoryPersistent((void *) pi5tab16[i]);
        mexMakeMemoryPersistent((void *) pi4tab16[i]);
        mexMakeMemoryPersistent((void *) pi6tab16[i]);
    }

#ifdef DEBUG_DLSCH_DECODING_INIT
    mexPrintf("pi2tab %p\n",&pi2tab16[0]);
    mexPrintf("pi4tab %p\n",&pi4tab16[0]);
    mexPrintf("pi5tab %p\n",&pi5tab16[0]);
    mexPrintf("pi6tab %p\n",&pi6tab16[0]);
#endif

}
