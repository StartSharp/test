/*
 * DeviceCtrl.c
 *
 *  Created on: Sep 1, 2022
 *      Author: tct
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <unistd.h>

#include "Platform.h"
#include "App.h"

#define MAX_OUTPUT_ELEMENT_COUNT	20
#define MAX_INPUT_ELEMENT_COUNT	20

enum ProtocolType{
	udp = 1,
	tcp,
	modbustcp,

};

struct EleCtrlType{
	UINT16 inputCmd;			/*平台输入参数*/
	UINT16 outputCmd;			/*板卡输出参数*/
};

struct ElementCtrlType{
	UINT16 ID;					/*场景元素ID*/
	char name[20];				/*名称*/
	enum ProtocolType pro;		/*协议*/
	void* pdev;					/*操作句柄*/
	UINT16 holdtime;			/*持续时间*/
	void* proConf;				/*指向不同协议的配置参数*/
	UINT16 ctrlTabLen;			/*123123aeqqwe*/
	void* ctrlTab;				/*指向控制表*/
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

struct ElementCtrlType* ElementCtrlTab[MAX_OUTPUT_ELEMENT_COUNT];


/**
 * @brief      初始化场景元素
 * @details
 * @param
 * @return     int  函数执行结果
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
int DeviceCtrlInit(void)
{
	int ret = -1;
	/*读取配置文件*/
	puts("ScenenController software "); /* prints !!!Hello World!!! */
	Config_Init();

	/*与场景元素建立连接并初始化设备*/
	puts("Init device");
	struct ModbusTCPConfType* pDevConf = malloc(sizeof(struct ModbusTCPConfType));
	ElementCtrlTab[0] = malloc(sizeof(struct ElementCtrlType));
	memcpy(pDevConf->ip, "192.168.2.232", sizeof("192.168.2.232"));
	pDevConf->port = 8080;
	pDevConf->slaveID = 1;
	ret = ModbusTCPMasterInit(pDevConf, &(ElementCtrlTab[0]->pdev));
	if(-1 == ret)
	{
		puts("Init error");
		return ret;
	}
	/*等待指令*/

	while(1)
	{
		ModbusTCPSetBit(ElementCtrlTab[0]->pdev, 1 , 1);

		sleep(1);

		ModbusTCPSetBit(ElementCtrlTab[0]->pdev, 1 , 0);

		sleep(1);

	}


	return EXIT_SUCCESS;
}


