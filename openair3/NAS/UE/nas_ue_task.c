/*******************************************************************************
    OpenAirInterface
    Copyright(c) 1999 - 2014 Eurecom

    OpenAirInterface is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.


    OpenAirInterface is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with OpenAirInterface.The full GNU General Public License is
   included in this distribution in the file called "COPYING". If not,
   see <http://www.gnu.org/licenses/>.

  Contact Information
  OpenAirInterface Admin: openair_admin@eurecom.fr
  OpenAirInterface Tech : openair_tech@eurecom.fr
  OpenAirInterface Dev  : openair4g-devel@lists.eurecom.fr

  Address      : Eurecom, Compus SophiaTech 450, route des chappes, 06451 Biot, France.

 *******************************************************************************/
#include "utils.h"
#if defined(ENABLE_ITTI)
# include "assertions.h"
# include "intertask_interface.h"
# include "nas_ue_task.h"
# include "UTIL/LOG/log.h"

# include "user_defs.h"
# include "user_api.h"
# include "nas_parser.h"
# include "nas_proc.h"
# include "msc.h"
# include "memory.h"

#include "nas_user.h"

// FIXME make command line option for NAS_UE_AUTOSTART
# define NAS_UE_AUTOSTART 1

// FIXME review these externs
extern unsigned char NB_eNB_INST;
extern unsigned char NB_UE_INST;

static int nas_ue_process_events(nas_user_t *user, struct epoll_event *events, int nb_events)
{
  int event;
  int exit_loop = FALSE;

  LOG_I(NAS, "[UE] Received %d events\n", nb_events);

  for (event = 0; event < nb_events; event++) {
    if (events[event].events != 0) {
      /* If the event has not been yet been processed (not an itti message) */
      if (events[event].data.fd == user_api_get_fd(user->user_api_id)) {
        exit_loop = nas_user_receive_and_process(user, NULL);
      } else {
        LOG_E(NAS, "[UE] Received an event from an unknown fd %d!\n", events[event].data.fd);
      }
    }
  }

  return (exit_loop);
}

// Initialize user api id and port number
void nas_user_api_id_initialize(nas_user_t *user) {
  user_api_id_t *user_api_id = calloc_or_fail(sizeof(user_api_id_t));
  user->user_api_id = user_api_id;

  if (user_api_initialize (user_api_id, NAS_PARSER_DEFAULT_USER_HOSTNAME, NAS_PARSER_DEFAULT_USER_PORT_NUMBER, NULL,
              NULL) != RETURNok) {
      LOG_E(NAS, "[UE %d] user interface initialization failed!", user->ueid);
      exit (EXIT_FAILURE);
  }

  itti_subscribe_event_fd (TASK_NAS_UE, user_api_get_fd(user_api_id));
}

void *nas_ue_task(void *args_p)
{
  int                   nb_events;
  struct epoll_event   *events;
  MessageDef           *msg_p;
  const char           *msg_name;
  instance_t            instance;
  unsigned int          Mod_id;
  int                   result;
  nas_user_t user_value = {};
  nas_user_t *user = &user_value;

  user->ueid = 0;

  itti_mark_task_ready (TASK_NAS_UE);
  MSC_START_USE();
  /* Initialize UE NAS (EURECOM-NAS) */
  {
    /* Get USIM data application filename */
    user->usim_data_store = memory_get_path(USIM_API_NVRAM_DIRNAME, USIM_API_NVRAM_FILENAME);
    if ( user->usim_data_store == NULL ) {
      LOG_E(NAS, "[UE %d] - Failed to get USIM data application filename", user->ueid);
      exit(EXIT_FAILURE);
    }

    /* Get UE's data pathname */
    user->user_nvdata_store = memory_get_path(USER_NVRAM_DIRNAME, USER_NVRAM_FILENAME);
    if ( user->user_nvdata_store == NULL ) {
      LOG_E(NAS, "[UE %d] - Failed to get USIM nvdata filename", user->ueid);
      exit(EXIT_FAILURE);
    }

    /* Get EMM data pathname */
    user->emm_nvdata_store = memory_get_path(EMM_NVRAM_DIRNAME, EMM_NVRAM_FILENAME);
    if ( user->emm_nvdata_store == NULL ) {
      LOG_E(NAS, "[UE %d] - Failed to get EMM nvdata filename", user->ueid);
      exit(EXIT_FAILURE);
    }

    /* Initialize user interface (to exchange AT commands with user process) */
    nas_user_api_id_initialize(user);
    user->user_at_commands = calloc_or_fail(sizeof(user_at_commands_t));
    user->at_response = calloc_or_fail(sizeof(at_response_t));
    user->lowerlayer_data = calloc_or_fail(sizeof(lowerlayer_data_t));
    /* Initialize NAS user */
    nas_user_initialize (user, &user_api_emm_callback, &user_api_esm_callback, FIRMWARE_VERSION);
  }

  /* Set UE activation state */
  for (instance = NB_eNB_INST; instance < (NB_eNB_INST + NB_UE_INST); instance++) {
    MessageDef *message_p;

    message_p = itti_alloc_new_message(TASK_NAS_UE, DEACTIVATE_MESSAGE);
    itti_send_msg_to_task(TASK_L2L1, instance, message_p);
  }

  while(1) {
    // Wait for a message or an event
    itti_receive_msg (TASK_NAS_UE, &msg_p);

    if (msg_p != NULL) {
      msg_name = ITTI_MSG_NAME (msg_p);
      instance = ITTI_MSG_INSTANCE (msg_p);
      Mod_id = instance - NB_eNB_INST;

      switch (ITTI_MSG_ID(msg_p)) {
      case INITIALIZE_MESSAGE:
        LOG_I(NAS, "[UE %d] Received %s\n", Mod_id, msg_name);
#if (NAS_UE_AUTOSTART != 0)
        {
          /* Send an activate modem command to NAS like UserProcess should do it */
          char *user_data = "at+cfun=1\r";

          nas_user_receive_and_process (user, user_data);
        }
#endif
        break;

      case TERMINATE_MESSAGE:
        itti_exit_task ();
        break;

      case MESSAGE_TEST:
        LOG_I(NAS, "[UE %d] Received %s\n", Mod_id, msg_name);
        break;

      case NAS_CELL_SELECTION_CNF:
        LOG_I(NAS, "[UE %d] Received %s: errCode %u, cellID %u, tac %u\n", Mod_id, msg_name,
              NAS_CELL_SELECTION_CNF (msg_p).errCode, NAS_CELL_SELECTION_CNF (msg_p).cellID, NAS_CELL_SELECTION_CNF (msg_p).tac);

        {
          int cell_found = (NAS_CELL_SELECTION_CNF (msg_p).errCode == AS_SUCCESS);

          nas_proc_cell_info (user, cell_found, NAS_CELL_SELECTION_CNF (msg_p).tac,
                              NAS_CELL_SELECTION_CNF (msg_p).cellID, NAS_CELL_SELECTION_CNF (msg_p).rat,
                              NAS_CELL_SELECTION_CNF (msg_p).rsrq, NAS_CELL_SELECTION_CNF (msg_p).rsrp);
        }
        break;

      case NAS_CELL_SELECTION_IND:
        LOG_I(NAS, "[UE %d] Received %s: cellID %u, tac %u\n", Mod_id, msg_name,
              NAS_CELL_SELECTION_IND (msg_p).cellID, NAS_CELL_SELECTION_IND (msg_p).tac);

        /* TODO not processed by NAS currently */
        break;

      case NAS_PAGING_IND:
        LOG_I(NAS, "[UE %d] Received %s: cause %u\n", Mod_id, msg_name,
              NAS_PAGING_IND (msg_p).cause);

        /* TODO not processed by NAS currently */
        break;

      case NAS_CONN_ESTABLI_CNF:
        LOG_I(NAS, "[UE %d] Received %s: errCode %u, length %u\n", Mod_id, msg_name,
              NAS_CONN_ESTABLI_CNF (msg_p).errCode, NAS_CONN_ESTABLI_CNF (msg_p).nasMsg.length);

        if ((NAS_CONN_ESTABLI_CNF (msg_p).errCode == AS_SUCCESS)
            || (NAS_CONN_ESTABLI_CNF (msg_p).errCode == AS_TERMINATED_NAS)) {
          nas_proc_establish_cnf(user, NAS_CONN_ESTABLI_CNF (msg_p).nasMsg.data, NAS_CONN_ESTABLI_CNF (msg_p).nasMsg.length);

          /* TODO checks if NAS will free the nas message, better to do it there anyway! */
          // result = itti_free (ITTI_MSG_ORIGIN_ID(msg_p), NAS_CONN_ESTABLI_CNF(msg_p).nasMsg.data);
          // AssertFatal (result == EXIT_SUCCESS, "Failed to free memory (%d)!\n", result);
        }

        break;

      case NAS_CONN_RELEASE_IND:
        LOG_I(NAS, "[UE %d] Received %s: cause %u\n", Mod_id, msg_name,
              NAS_CONN_RELEASE_IND (msg_p).cause);

        nas_proc_release_ind (user, NAS_CONN_RELEASE_IND (msg_p).cause);
        break;

      case NAS_UPLINK_DATA_CNF:
        LOG_I(NAS, "[UE %d] Received %s: UEid %u, errCode %u\n", Mod_id, msg_name,
              NAS_UPLINK_DATA_CNF (msg_p).UEid, NAS_UPLINK_DATA_CNF (msg_p).errCode);

        if (NAS_UPLINK_DATA_CNF (msg_p).errCode == AS_SUCCESS) {
          nas_proc_ul_transfer_cnf (user);
        } else {
          nas_proc_ul_transfer_rej (user);
        }

        break;

      case NAS_DOWNLINK_DATA_IND:
        LOG_I(NAS, "[UE %d] Received %s: UEid %u, length %u\n", Mod_id, msg_name,
              NAS_DOWNLINK_DATA_IND (msg_p).UEid, NAS_DOWNLINK_DATA_IND (msg_p).nasMsg.length);

        nas_proc_dl_transfer_ind (user, NAS_DOWNLINK_DATA_IND(msg_p).nasMsg.data, NAS_DOWNLINK_DATA_IND(msg_p).nasMsg.length);

        if (0) {
          /* TODO checks if NAS will free the nas message, better to do it there anyway! */
          result = itti_free (ITTI_MSG_ORIGIN_ID(msg_p), NAS_DOWNLINK_DATA_IND(msg_p).nasMsg.data);
          AssertFatal (result == EXIT_SUCCESS, "Failed to free memory (%d)!\n", result);
        }

        break;

      default:
        LOG_E(NAS, "[UE %d] Received unexpected message %s\n", Mod_id, msg_name);
        break;
      }

      result = itti_free (ITTI_MSG_ORIGIN_ID(msg_p), msg_p);
      AssertFatal (result == EXIT_SUCCESS, "Failed to free memory (%d)!\n", result);
      msg_p = NULL;
    }

    nb_events = itti_get_events(TASK_NAS_UE, &events);

    if ((nb_events > 0) && (events != NULL)) {
      if (nas_ue_process_events(user, events, nb_events) == TRUE) {
        LOG_E(NAS, "[UE] Received exit loop\n");
      }
    }
  }
}
#endif
