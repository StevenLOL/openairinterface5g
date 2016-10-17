#ifndef FF_MAC_H
#define FF_MAC_H

/** @defgroup _fapi  FAPI
 * @ingroup _mac
 * @{
 */

/* this file contains OAI related FAPI definitions */

/* this is the public view of the FAPI's OAI interface */
typedef struct {
  void *sched;     /* this is the pointer returned by SchedInit */
                   /* to be used when calling FAPI functions */
} fapi_interface_t;

/* this function initializes OAI's FAPI interfacing
 * it returns the opaque pointer given by SchedInit
 */
fapi_interface_t *init_fapi(void);

/* the following functions are called by OAI
 * they wait for the corresponding callback
 * to be called by the FAPI scheduler
 */

#include "ff-mac-sched-sap.h"
#include "ff-mac-csched-sap.h"

/* from SCHED */
void SchedDlConfigInd(fapi_interface_t *, struct SchedDlConfigIndParameters* params);
void SchedUlConfigInd(fapi_interface_t *, struct SchedUlConfigIndParameters* params);

/* from CSCHED */
void CschedCellConfigCnf(fapi_interface_t *, struct CschedCellConfigCnfParameters *params);
void CschedUeConfigCnf(fapi_interface_t *, struct CschedUeConfigCnfParameters *params);
void CschedLcConfigCnf(fapi_interface_t *, struct CschedLcConfigCnfParameters *params);
void CschedLcReleaseCnf(fapi_interface_t *, struct CschedLcReleaseCnfParameters *params);
void CschedUeReleaseCnf(fapi_interface_t *, struct CschedUeReleaseCnfParameters *params);
void CschedUeConfigUpdateInd(fapi_interface_t *, struct CschedUeConfigUpdateIndParameters *params);
void CschedCellConfigUpdateInd(fapi_interface_t *, struct CschedCellConfigUpdateIndParameters *params);

/* those functions are called by the PHY layer to inform FAPI of events */

/* signal uplink ACKs/NACKs */
void fapi_ul_ack_nack(int frame, int subframe, int harq_pid, int rnti, int ack);

/* signal uplink LC data length received */
void fapi_ul_lc_length(int frame, int subframe, int lcid, int length, int rnti);

/* signal downlink ACKs/NACKs */
void fapi_dl_ack_nack(int CC_id, int rnti, int harq_pid, int transport_block, int ack);

/* signal DL CQI (mode 3-0 only, see 36.213 7.2.1) */
void fapi_dl_cqi_report(int module_id, int rnti, int frame, int subframe, int cqi_wideband, int *cqi_subband, int rank_indication);

/*@}*/

#endif /* FF_MAC_H */
