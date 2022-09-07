/*
 * SceneElement.c
 *
 *  Created on: Sep 6, 2022
 *      Author: tct
 */

#include "SceneElement.h"

#include "SensorCtrl/DeviceCtrl.h"
#include "Platform.h"

#include <unistd.h>

/**
 * @brief     设备层后台服务
 * @details
 * @param
 * @return     int  函数执行结果
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
void* DeviceCmd(void* arg)
{
	sleep(5);
	while(1)
	{
		DeviceCtrl(g_deviceCtrlTab[0]);

		sleep(100);

	}
	return NULL;
}
