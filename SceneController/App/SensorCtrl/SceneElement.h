/*
 * SceneElement.h
 *
 *  Created on: Sep 6, 2022
 *      Author: tct
 */

#ifndef APP_SENSORCTRL_SCENEELEMENT_H_
#define APP_SENSORCTRL_SCENEELEMENT_H_

#include "common.h"

enum RESET_MODE_TYPE{
	RESET_ALL,
	RESET_ANYONE,
};
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

/**
 * @brief     场景元素导向安全操作
 * @details	  通过解析预设命令控制场景元素导向安全
 * @param     UINT16 id   场景元素ID
 * 			  enum RESET_MODE_TYPE mode   可选择全部初始化或某个初始化，若选择全部初始化id无意义
 * @return     int  函数执行结果
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
STATUS_T SceneElementReset(enum RESET_MODE_TYPE mode, UINT16 id);

/**
 * @brief     场景元素初始化
 * @details	  通过解析控制命令决定
 * @param     void* arg		操作句柄
 * @return     int  函数执行结果
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
STATUS_T SceneElementSamplerInit(void);

#endif /* APP_SENSORCTRL_SCENEELEMENT_H_ */
