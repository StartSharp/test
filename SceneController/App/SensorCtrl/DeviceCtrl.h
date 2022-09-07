/*
 * DeviceCtrl.h
 *
 *  Created on: Sep 1, 2022
 *      Author: tct
 */

#ifndef APP_SENSORCTRL_DEVICECTRL_H_
#define APP_SENSORCTRL_DEVICECTRL_H_

#include "common.h"

#define MAX_OUTPUT_ELEMENT_COUNT	20
#define MAX_INPUT_ELEMENT_COUNT	20

extern struct DeviceCtrlType* g_deviceCtrlTab[MAX_OUTPUT_ELEMENT_COUNT];

/**
 * @brief      设备层控制
 * @details
 * @param
 * @return     int  函数执行结果
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
STATUS_T DeviceCtrl(struct DeviceCtrlType* pdevice);

/**
 * @brief      初始化场景元素
 * @details
 * @param
 * @return     int  函数执行结果
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
void* DeviceCtrlRunDamon(void* arg);

#endif /* APP_SENSORCTRL_DEVICECTRL_H_ */
