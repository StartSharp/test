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
#include <pthread.h>

#include "DeviceCtrl.h"
#include "Platform.h"

struct DeviceCtrlType* g_deviceCtrlTab[MAX_OUTPUT_ELEMENT_COUNT];
UINT16 g_acuCtrlCnt = 0;

struct DeviceSampleType* g_deviceSampleTab[MAX_INPUT_ELEMENT_COUNT];
UINT16 g_acuSampleCnt = 0;

struct InputDataGroupType* g_inputDataGroup[MAX_INPUT_GROUP_COUNT];
UINT16 g_acuGroupCnt = 1;
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
    	ModbusTCPSetBit(pHandler->pdev, ((struct ModbusTCPConfType*)pHandler->proConf)->addr , 0);
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
STATUS_T OpenModbusDevice(void* ptr, UINT32 para)
{
	STATUS_T ret = RET_UNKNOWN_ERR;
	struct DeviceCtrlType* pHandler = ptr;
    /*打开modbus设备*/
	printf("Opening device %s\r\n", pHandler->name);
    if(NULL != pHandler->pdev)
    {
    	int result = ModbusTCPSetBit(pHandler->pdev, ((struct ModbusTCPConfType*)pHandler->proConf)->addr , 1);
    	if(-1 != result)
    		ret = RET_NO_ERR;
    	else
    		ret = RET_NOCONN_ERR;
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
STATUS_T DeviceCtrl(struct DeviceCtrlType* pdevice, UINT32 para)
{
	STATUS_T ret = RET_UNKNOWN_ERR;

	if(NULL != pdevice)
	{
		if(0 != pdevice->holdtime)
		{
			/*启动定时器*/
			softTimer_Start(&pdevice->timer, SOFTTIMER_MODE_ONE_SHOT, pdevice->holdtime, pdevice->close, 1, pdevice);
		}
		ret = pdevice->open(pdevice, para);
	}
	else
	{
		ret = RET_PARAM_ERR;
	}

	return ret;
}

/**
 * @brief     设备层控制初始化服务
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
	pDevConf->addr = 2;

	strcpy(g_deviceCtrlTab[0]->name, "DO3");
	g_deviceCtrlTab[0]->ID = 0;
	g_deviceCtrlTab[0]->holdtime = 500;
	g_deviceCtrlTab[0]->init = ModbusTCPMasterInit;
	g_deviceCtrlTab[0]->open = OpenModbusDevice;
	g_deviceCtrlTab[0]->close = CloseModbusDevice;
	g_deviceCtrlTab[0]->pro = modbustcp;
	g_deviceCtrlTab[0]->proConf = pDevConf;

	pDevConf = malloc(sizeof(struct ModbusTCPConfType)); /*配置后期由xml文件给出*/
	g_deviceCtrlTab[1] = malloc(sizeof(struct DeviceCtrlType));					   /*数据保存点 部分信息由xml文件给出*/
	memcpy(pDevConf->ip, "192.168.2.232", sizeof("192.168.2.232"));
	pDevConf->port = 8080;
	pDevConf->slaveID = 1;
	pDevConf->addr = 3;
	strcpy(g_deviceCtrlTab[1]->name, "DO4");
	g_deviceCtrlTab[1]->ID = 1;
	g_deviceCtrlTab[1]->holdtime = 500;
	g_deviceCtrlTab[1]->init = ModbusTCPMasterInit;
	g_deviceCtrlTab[1]->open = OpenModbusDevice;
	g_deviceCtrlTab[1]->close = CloseModbusDevice;
	g_deviceCtrlTab[1]->pro = modbustcp;
	g_deviceCtrlTab[1]->proConf = pDevConf;
	g_acuCtrlCnt = 2;

	/*初始化协议*/
	ret = g_deviceCtrlTab[0]->init((void*)pDevConf, &g_deviceCtrlTab[0]->pdev);
	if(-1 == ret)
	{
		puts("Init error");
	}
	g_deviceCtrlTab[1]->pdev = g_deviceCtrlTab[0]->pdev;

	/*为每个设备定义一个软timer*/
	softTimer_Init(&g_deviceCtrlTab[0]->timer, 1);
	/*为每个设备定义一个软timer*/
	softTimer_Init(&g_deviceCtrlTab[1]->timer, 1);

	sleep(5);
	return ret;
}

/**
 * @brief     获取采样组对应的操作句柄
 * @details
 * @param		UINT16 id	组别ID
 * @return     int  函数执行结果
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
struct InputDataGroupType* GetGroupHandler(UINT16 id)
{
	struct InputDataGroupType* pHandler;
	UINT16 i;

	for(i = 0; i < g_acuGroupCnt; i++)
	{
		if(id == g_inputDataGroup[i]->groupID)
		{
			pHandler = g_inputDataGroup[i];
		}
	}

	return pHandler;
}

/**
 * @brief     设备层采样服务初始化
 * @details
 * @param
 * @return     int  函数执行结果
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
STATUS_T DeviceCtrlSampleInit(void)
{
	STATUS_T ret = RET_UNKNOWN_ERR;
	/*从配置文件中读取配置*/
	g_inputDataGroup[0] = malloc(sizeof(struct InputDataGroupType));
	/*xml读取*/
	g_inputDataGroup[0]->groupID = 0;
	g_inputDataGroup[0]->datasize = 1024;
	strcpy(g_inputDataGroup[0]->name, "TIDS采样");
	g_inputDataGroup[0]->pro = udp;

	/*数据存放地址*/
	g_inputDataGroup[0]->pBuf = malloc(g_inputDataGroup[0]->datasize);
	switch(g_inputDataGroup[0]->pro)
	{
	case udp:
		g_inputDataGroup[0]->initConf = malloc(sizeof(struct InputDataGroupType));
		strcpy(((struct UDPSampleGroupType*)g_inputDataGroup[0]->initConf)->ip, "192.168.2.102");
		((struct UDPSampleGroupType*)g_inputDataGroup[0]->initConf)->port = 8800;
		g_inputDataGroup[0]->init = UDPInit;
		g_inputDataGroup[0]->resvDamon = UDPGroupSampleDamon;
		break;
	default:
		break;
	}

	/*UDP采样 Group建立连接*/
	int result = g_inputDataGroup[0]->init((void*)g_inputDataGroup[0]->initConf);
	if(0 == result)
	{
		ret = RET_NO_ERR;
	}

	return ret;
}

/**
 * @brief     设备层采样服务
 * @details
 * @param
 * @return     int  函数执行结果
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
void* DeviceCtrlSampleDamon(void* arg)
{
	/*数据采样到Group*/
	pthread_create(&g_inputDataGroup[0]->pthread, NULL, g_inputDataGroup[0]->resvDamon, &g_inputDataGroup[0]->initConf);

	return NULL;
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
		softTimer_Update(&g_deviceCtrlTab[1]->timer, 1);

		sleep(1);
	}

	return NULL;
}


