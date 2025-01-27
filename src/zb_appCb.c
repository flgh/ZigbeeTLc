/********************************************************************************************************
 * @file    zb_appCb.c
 *
 * @brief   This is the source file for zb_appCb
 *
 * @author  Zigbee Group
 * @date    2021
 *
 * @par     Copyright (c) 2021, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *			All rights reserved.
 *
 *          Licensed under the Apache License, Version 2.0 (the "License");
 *          you may not use this file except in compliance with the License.
 *          You may obtain a copy of the License at
 *
 *              http://www.apache.org/licenses/LICENSE-2.0
 *
 *          Unless required by applicable law or agreed to in writing, software
 *          distributed under the License is distributed on an "AS IS" BASIS,
 *          WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *          See the License for the specific language governing permissions and
 *          limitations under the License.
 *
 *******************************************************************************************************/


/**********************************************************************
 * INCLUDES
 */
#include "tl_common.h"
#include "zb_api.h"
#include "zcl_include.h"
#include "bdb.h"
#include "ota.h"
#include "device.h"
#include "app_ui.h"
#include "lcd.h"

/**********************************************************************
 * LOCAL CONSTANTS
 */


/**********************************************************************
 * TYPEDEFS
 */


/**********************************************************************
 * LOCAL FUNCTIONS
 */
void zbdemo_bdbInitCb(u8 status, u8 joinedNetwork);
void zbdemo_bdbCommissioningCb(u8 status, void *arg);
void zbdemo_bdbIdentifyCb(u8 endpoint, u16 srcAddr, u16 identifyTime);
void zbdemo_bdbFindBindSuccessCb(findBindDst_t *pDstInfo);

/**********************************************************************
 * LOCAL VARIABLES
 */
bdb_appCb_t g_zbDemoBdbCb =
{
	zbdemo_bdbInitCb,
	zbdemo_bdbCommissioningCb,
	zbdemo_bdbIdentifyCb,
	zbdemo_bdbFindBindSuccessCb
};

#ifdef ZCL_OTA
ota_callBack_t sensorDevice_otaCb =
{
	sensorDevice_otaProcessMsgHandler,
};
#endif

ev_timer_event_t *steerTimerEvt = NULL;
#if REJOIN_FAILURE_TIMER
ev_timer_event_t *deviceRejoinBackoffTimerEvt = NULL;
#endif
/**********************************************************************
 * FUNCTIONS
 */
s32 sensorDevice_bdbNetworkSteerStart(void *arg){

	bdb_networkSteerStart();

	steerTimerEvt = NULL;
	return -1;
}
#if FIND_AND_BIND_SUPPORT
s32 sensorDevice_bdbFindAndBindStart(void *arg){
#ifdef ZCL_GROUP_SUPPORT
        BDB_ATTR_GROUP_ID_SET(0x1234);//only for initiator
#endif
        bdb_findAndBindStart(BDB_COMMISSIONING_ROLE_INITIATOR);

        g_sensorAppCtx.bdbFBTimerEvt = NULL;
        return -1;
}
#endif

#if REJOIN_FAILURE_TIMER

s32 sensorDevice_rejoinBackoff(void *arg){

	if(zb_isDeviceFactoryNew()){
		deviceRejoinBackoffTimerEvt = NULL;
		return -1;
	}

    zb_rejoinReqWithBackOff(zb_apsChannelMaskGet(), g_bdbAttrs.scanDuration);
    return 0;
}

#endif

/*********************************************************************
 * @fn      zbdemo_bdbInitCb
 *
 * @brief   application callback for bdb initiation
 *
 * @param   status - the status of bdb init BDB_INIT_STATUS_SUCCESS or BDB_INIT_STATUS_FAILURE
 *
 * @param   joinedNetwork  - 1: node is on a network, 0: node isn't on a network
 *
 * @return  None
 */
void zbdemo_bdbInitCb(u8 status, u8 joinedNetwork){
	if(status == BDB_INIT_STATUS_SUCCESS){
		/*
		 * for non-factory-new device:
		 * 		load zcl data from NV, start poll rate, start ota query, bdb_networkSteerStart
		 *
		 * for factory-new device:
		 * 		steer a network
		 *
		 */
		if(joinedNetwork){
			zb_setPollRate(DEFAULT_POLL_RATE);

#ifdef ZCL_OTA
			ota_queryStart(15 * 60);	// 15 m
#endif

#ifdef ZCL_POLL_CTRL
			sensorDevice_zclCheckInStart();
#endif
		}else{
			u16 jitter = 0;
			do{
				jitter = zb_random() % 0x0fff;
			}while(jitter == 0);

			if(steerTimerEvt){
				TL_ZB_TIMER_CANCEL(&steerTimerEvt);
			}
			///time_soff = 0;
			steerTimerEvt = TL_ZB_TIMER_SCHEDULE(sensorDevice_bdbNetworkSteerStart, NULL, jitter);
		}
	}
#if REJOIN_FAILURE_TIMER
	else
	{
		if(joinedNetwork){
			zb_rejoinReqWithBackOff(zb_apsChannelMaskGet(), g_bdbAttrs.scanDuration);
		}
	}
#endif
}

/*********************************************************************
 * @fn      zbdemo_bdbCommissioningCb
 *
 * @brief   application callback for bdb commissioning
 *
 * @param   status - the status of bdb commissioning
 *
 * @param   arg
 *
 * @return  None
 */
void zbdemo_bdbCommissioningCb(u8 status, void *arg){
	switch(status){
		case BDB_COMMISSION_STA_SUCCESS:
#if BOARD == BOARD_TS0201_TZ3000
			light_blink_start(7, 500, 500);
#else
			light_blink_start(5, 300, 300);
#endif
			zb_setPollRate(DEFAULT_POLL_RATE);

			if(steerTimerEvt){
				TL_ZB_TIMER_CANCEL(&steerTimerEvt);
			}
			if(deviceRejoinBackoffTimerEvt){
				TL_ZB_TIMER_CANCEL(&deviceRejoinBackoffTimerEvt);
			}
#if	USE_DISPLAY
			if(g_sensorAppCtx.timerTaskEvt) {
				TL_ZB_TIMER_CANCEL(&g_sensorAppCtx.timerTaskEvt);
				g_sensorAppCtx.timerTaskEvt = NULL;
			}
#endif
#ifdef ZCL_POLL_CTRL
		    sensorDevice_zclCheckInStart();
#endif
#ifdef ZCL_OTA
			ota_queryStart(OTA_PERIODIC_QUERY_INTERVAL);
#endif
#if FIND_AND_BIND_SUPPORT
                        //start Finding & Binding
                        if(!g_sensorAppCtx.bdbFBTimerEvt){
                                g_sensorAppCtx.bdbFBTimerEvt = TL_ZB_TIMER_SCHEDULE(sensorDevice_bdbFindAndBindStart, NULL, 50);
                        }
#endif
#if	USE_DISPLAY
#if BOARD == BOARD_MHO_C401N
			show_connected_symbol(true);
#else
			show_ble_symbol(false);
#endif
#endif
			break;
		case BDB_COMMISSION_STA_IN_PROGRESS:
			break;
		case BDB_COMMISSION_STA_NOT_AA_CAPABLE:
			break;
		case BDB_COMMISSION_STA_NO_NETWORK:
		case BDB_COMMISSION_STA_TCLK_EX_FAILURE:
		case BDB_COMMISSION_STA_TARGET_FAILURE:
			{
				u16 jitter = 0;
				do{
					jitter = zb_random() % 0x0fff;
				}while(jitter == 0);

				if(steerTimerEvt){
					TL_ZB_TIMER_CANCEL(&steerTimerEvt);
				}
#if REJOIN_FAILURE_TIMER
				steerTimerEvt = TL_ZB_TIMER_SCHEDULE(sensorDevice_bdbNetworkSteerStart, NULL, jitter + 60000);
#else
				steerTimerEvt = TL_ZB_TIMER_SCHEDULE(sensorDevice_bdbNetworkSteerStart, NULL, jitter);
#endif
#if	USE_DISPLAY
#if BOARD == BOARD_MHO_C401N
				show_connected_symbol(false);
#else
				show_ble_symbol(true);
#endif
				if(!g_sensorAppCtx.timerTaskEvt)
					g_sensorAppCtx.timerTaskEvt = TL_ZB_TIMER_SCHEDULE(sensors_task, NULL, READ_SENSOR_TIMER_MS);
#endif
			}
			break;
		case BDB_COMMISSION_STA_FORMATION_FAILURE:
			break;
		case BDB_COMMISSION_STA_NO_IDENTIFY_QUERY_RESPONSE:
			break;
		case BDB_COMMISSION_STA_BINDING_TABLE_FULL:
			break;
		case BDB_COMMISSION_STA_NOT_PERMITTED:
			break;
		case BDB_COMMISSION_STA_NO_SCAN_RESPONSE:
		case BDB_COMMISSION_STA_PARENT_LOST:
#if REJOIN_FAILURE_TIMER
			sensorDevice_rejoinBackoff(NULL);
#else
			zb_rejoinReqWithBackOff(zb_apsChannelMaskGet(), g_bdbAttrs.scanDuration);
#endif
#if	USE_DISPLAY
#if BOARD == BOARD_MHO_C401N
			show_connected_symbol(false);
#else
			show_ble_symbol(true);
#endif
			if(!g_sensorAppCtx.timerTaskEvt)
				g_sensorAppCtx.timerTaskEvt = TL_ZB_TIMER_SCHEDULE(sensors_task, NULL, READ_SENSOR_TIMER_MS);
#endif
			break;
		case BDB_COMMISSION_STA_REJOIN_FAILURE:
			if(!zb_isDeviceFactoryNew()){
#if REJOIN_FAILURE_TIMER
                // sleep for 3 minutes before reconnect if rejoin failed
                deviceRejoinBackoffTimerEvt = TL_ZB_TIMER_SCHEDULE(sensorDevice_rejoinBackoff, NULL, 360 * 1000);
#else
				zb_rejoinReqWithBackOff(zb_apsChannelMaskGet(), g_bdbAttrs.scanDuration);
#endif
			}
#if	USE_DISPLAY
#if BOARD == BOARD_MHO_C401N
			show_connected_symbol(false);
#else
			show_ble_symbol(true);
#endif
			if(!g_sensorAppCtx.timerTaskEvt)
				g_sensorAppCtx.timerTaskEvt = TL_ZB_TIMER_SCHEDULE(sensors_task, NULL, READ_SENSOR_TIMER_MS);
#endif
			break;
		default:
			break;
	}
}


extern void sensorDevice_zclIdentifyCmdHandler(u8 endpoint, u16 srcAddr, u16 identifyTime);
void zbdemo_bdbIdentifyCb(u8 endpoint, u16 srcAddr, u16 identifyTime){
	sensorDevice_zclIdentifyCmdHandler(endpoint, srcAddr, identifyTime);
}

/*********************************************************************
 * @fn      zbdemo_bdbFindBindSuccessCb
 *
 * @brief   application callback for finding & binding
 *
 * @param   pDstInfo
 *
 * @return  None
 */
void zbdemo_bdbFindBindSuccessCb(findBindDst_t *pDstInfo){
#if FIND_AND_BIND_SUPPORT
        epInfo_t dstEpInfo;
        TL_SETSTRUCTCONTENT(dstEpInfo, 0);

        dstEpInfo.dstAddrMode = APS_SHORT_DSTADDR_WITHEP;
        dstEpInfo.dstAddr.shortAddr = pDstInfo->addr;
        dstEpInfo.dstEp = pDstInfo->endpoint;
        dstEpInfo.profileId = HA_PROFILE_ID;

        zcl_identify_identifyCmd(SENSOR_DEVICE_ENDPOINT, &dstEpInfo, FALSE, 0, 0);
#endif
}

#ifdef ZCL_OTA
void sensorDevice_otaProcessMsgHandler(u8 evt, u8 status)
{
	if(evt == OTA_EVT_START){
		if(status == ZCL_STA_SUCCESS){
			zb_setPollRate(QUEUE_POLL_RATE);
		}else{

		}
	}else if(evt == OTA_EVT_COMPLETE){
		zb_setPollRate(DEFAULT_POLL_RATE);

		if(status == ZCL_STA_SUCCESS){
			ota_mcuReboot();
		}else{
			ota_queryStart(OTA_PERIODIC_QUERY_INTERVAL);
		}
	}else if(evt == OTA_EVT_IMAGE_DONE){
		zb_setPollRate(DEFAULT_POLL_RATE);
	}
}
#endif

/*********************************************************************
 * @fn      sensorDevice_leaveCnfHandler
 *
 * @brief   Handler for ZDO Leave Confirm message.
 *
 * @param   pRsp - parameter of leave confirm
 *
 * @return  None
 */
void sensorDevice_leaveCnfHandler(nlme_leave_cnf_t *pLeaveCnf)
{
    if(pLeaveCnf->status == SUCCESS){
		if(deviceRejoinBackoffTimerEvt){
			TL_ZB_TIMER_CANCEL(&deviceRejoinBackoffTimerEvt);
		}
    }
}

/*********************************************************************
 * @fn      sensorDevice_leaveIndHandler
 *
 * @brief   Handler for ZDO leave indication message.
 *
 * @param   pInd - parameter of leave indication
 *
 * @return  None
 */
void sensorDevice_leaveIndHandler(nlme_leave_ind_t *pLeaveInd)
{
    //printf("sensorDevice_leaveIndHandler, rejoin = %d\n", pLeaveInd->rejoin);
    //printfArray(pLeaveInd->device_address, 8);
}
