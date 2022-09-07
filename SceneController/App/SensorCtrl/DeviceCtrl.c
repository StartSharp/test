/*
 * DeviceCtrl.c
 *
 *  Created on: Sep 1, 2022
 *      Author: tct
 */

#include  <stdio.h>
#include  <stdlib.h>
#include  <signal.h>
#include  <time.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <unistd.h>

#include "DeviceCtrl.h"
#include "Platform.h"

enum ProtocolType{
	udp = 1,
	tcp,
	modbustcp,

};

struct EleCtrlType{
	UINT16 inputCmd;			/*平台输入参数*/
	UINT16 outputCmd;			/*板卡输出参数*/
};

struct DeviceCtrlType{
	UINT16 ID;					/*设备操作ID*/
	char name[20];				/*名称*/
	enum ProtocolType pro;		/*协议*/
	void* pdev;					/*操作句柄*/
	UINT16 holdtime;			/*持续时间 单位s*/
	void* proConf;				/*指向不同协议的配置参数*/
	UINT16 ctrlTabLen;			/*控制表长度*/
	void* ctrlTab;				/*指向控制表*/
	softTimer_st timer;

	timer_t  timerID;

	STATUS_T (*open)(struct DeviceCtrlType* pHandler);
	void (*close)(uint16_t argc, void* argv);
};

/*输入型场景元素采样参数配置*/
struct RawDataConfType{
	enum ProtocolType pro;

};

struct ElementReadType{
	UINT16 ID;					/*场景元素ID*/
	char name[20];				/*名称*/
	void* pdata;				/*数据所在的数据源地址*/
	UINT16 size;				/*数据长度*/
	UINT16 offset;				/*数据偏移*/
};

struct DeviceCtrlType* g_deviceCtrlTab[MAX_OUTPUT_ELEMENT_COUNT];

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
 * @brief      初始化场景元素
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
    	ModbusTCPSetBit(pHandler->pdev, 1 , 1);
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
		pdevice->open(pdevice);
	}
	else
	{
		ret = RET_PARAM_ERR;
	}

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

	/*初始化协议*/
	ret = ModbusTCPMasterInit(pDevConf, &(g_deviceCtrlTab[0]->pdev));
	if(-1 == ret)
	{
		puts("Init error");
	}

	/*为每个设备定义一个软timer*/
	softTimer_Init(&g_deviceCtrlTab[0]->timer, 1);

	while(1)
	{
		softTimer_Update(&g_deviceCtrlTab[0]->timer, 1);

		sleep(1);
	}

	return NULL;
}


