/*
 * SceneElement.h
 *
 *  Created on: Sep 6, 2022
 *      Author: tct
 */

#ifndef APP_SENSORCTRL_SCENEELEMENT_H_
#define APP_SENSORCTRL_SCENEELEMENT_H_

#include "common.h"


/**
 * @brief     设备层后台服务
 * @details
 * @param     void* arg		操作句柄
 * @return     int  函数执行结果
 *     - RET_NO_ERR  成功
 *     - ohterSceneCmdElement
 */
void* TempCtrl(void* arg);

/**
 * @brief     场景元素初始化
 * @details	  通过解析控制命令决定
 * @param     void* arg		操作句柄
 * @return     int  函数执行结果
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
STATUS_T SceneElementInit(void);

#endif /* APP_SENSORCTRL_SCENEELEMENT_H_ */
