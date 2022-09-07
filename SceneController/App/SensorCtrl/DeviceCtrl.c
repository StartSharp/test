/*
 * DeviceCtrl.c
 *
 *  Created on: Sep 1, 2022
 *      Author: tct
 */

#include  <stdio.h>
#include  <stdlib.h>
#include  <signal.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <unistd.h>

#include "DeviceCtrl.h"


struct DeviceCtrlType* g_deviceCtrlTab[MAX_OUTPUT_ELEMENT_COUNT];
UINT16 g_acuCnt = 0;

/**
 * @brief      获取系统时间ms
 * @details
 * @param
 * @return     long long 获取到的系统时间
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
long long GetSysTimeMS(void)
{
	long long timeMS = 0;
	struct timeval sysCurrentTime;

	gettimeofday(&sysCurrentTime, NULL);
	timeMS = ((long long)sysCurrentTime.tv_sec*1000000 + sysCurrentTime.tv_usec)/1000;

	return timeMS;
}

/**
 * @brief      	查找操作句柄
 * @details		根据操作ID找到对应的操作句柄
 * @param		UINT16 id	查找ID
 * @return     struct ElementCtrlType* 操作句柄
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
struct DeviceCtrlType* GetDeviceHandlerFromID(UINT16 id)
{
	UINT16 i = 0;
	struct DeviceCtrlType* ptr = NULL;

	for(i = 0; i < MAX_OUTPUT_ELEMENT_COUNT; i++)
	{
		if(id == g_deviceCtrlTab[i]->ID)
		{
			ptr = g_deviceCtrlTab[i];
			break;
		}
		else
		{

		}

	}
	return ptr;
}

/**
 * @brief      关闭Modbus设备
 * @details
 * @param
 * @return     int  函数执行结果
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
void CloseModbusDevice(uint16_t argc, void* argv)
{
	struct DeviceCtrlType* pHandler = (struct DeviceCtrlType*)argv;

	printf("Closing device %s\r\n", pHandler->name);

    if(NULL != pHandler->pdev)
    {
    	ModbusTCPSetBit(pHandler->pdev, 1 , 0);
    }
    else
    {
    	printf("Device handler is not initialed\r\n");
    }
}

/**
 * @brief      打开Modbus设备
 * @details
 * @param
 * @return     int  函数执行结果
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
STATUS_T OpenModbusDevice(struct DeviceCtrlType* pHandler)
{
	STATUS_T ret = RET_UNKNOWN_ERR;

    /*打开modbus设备*/
	printf("Opening device %s\r\n", pHandler->name);
    if(NULL != pHandler->pdev)
    {
    	int result = ModbusTCPSetBit(pHandler->pdev, 1 , 1);
    	if(-1 != result)
    		ret = RET_NO_ERR;
    }
    else
    {
    	printf("Device handler is not initialed\r\n");
    }

    return ret;
}

/**
 * @brief      设备层控制
 * @details
 * @param
 * @return     int  函数执行结果
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
STATUS_T DeviceCtrl(struct DeviceCtrlType* pdevice)
{
	STATUS_T ret = RET_UNKNOWN_ERR;

	if(NULL != pdevice)
	{
		if(0 != pdevice->holdtime)
		{
			/*启动定时器*/
			softTimer_Start(&pdevice->timer, SOFTTIMER_MODE_ONE_SHOT, pdevice->holdtime, pdevice->close, 1, pdevice);
		}
		ret = pdevice->open(pdevice);
	}
	else
	{
		ret = RET_PARAM_ERR;
	}

	return ret;
}

/**
 * @brief     设备层初始化服务
 * @details
 * @param
 * @return     int  函数执行结果
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
int DeviceCtrlIint(void)
{
	int ret = -1;

	/*读取配置文件*/
	puts("Read XML"); /* prints !!!Hello World!!! */

	Config_Init();

	/*与场景元素建立连接并初始化设备*/
	puts("Init device");
	struct ModbusTCPConfType* pDevConf = malloc(sizeof(struct ModbusTCPConfType)); /*配置后期由xml文件给出*/
	g_deviceCtrlTab[0] = malloc(sizeof(struct DeviceCtrlType));					   /*数据保存点 部分信息由xml文件给出*/

	/*模拟参数配置*/
	memcpy(pDevConf->ip, "192.168.2.232", sizeof("192.168.2.232"));
	pDevConf->port = 8080;
	pDevConf->slaveID = 1;

	strcpy(g_deviceCtrlTab[0]->name, "DO1");
	g_deviceCtrlTab[0]->ID = 0;
	g_deviceCtrlTab[0]->holdtime = 300;
	g_deviceCtrlTab[0]->open = OpenModbusDevice;
	g_deviceCtrlTab[0]->close = CloseModbusDevice;
	g_deviceCtrlTab[0]->pro = modbustcp;
	g_deviceCtrlTab[0]->proConf = pDevConf;
	g_acuCnt = 1;

	/*初始化协议*/
	ret = ModbusTCPMasterInit(pDevConf, &(g_deviceCtrlTab[0]->pdev));
	if(-1 == ret)
	{
		puts("Init error");
	}

	/*为每个设备定义一个软timer*/
	softTimer_Init(&g_deviceCtrlTab[0]->timer, 1);

	return ret;
}

/**
 * @brief     设备层后台服务
 * @details
 * @param
 * @return     int  函数执行结果
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
void* DeviceCtrlRunDamon(void* arg)
{
	while(1)
	{
		softTimer_Update(&g_deviceCtrlTab[0]->timer, 1);

		sleep(1);
	}

	return NULL;
}


