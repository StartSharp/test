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
	UINT16 holdtime;			/*持续时间 单位s*/
	void* proConf;				/*指向不同协议的配置参数*/
	UINT16 ctrlTabLen;			/*控制表长度*/
	void* ctrlTab;				/*指向控制表*/

	timer_t  timerID;

	STATUS_T (*open)(struct ElementCtrlType* pdev);
	void (*close)(sigval_t signal);
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
static void CloseModbusDevice(sigval_t signal)
{
	ModbusTCPSetBit(ElementCtrlTab[0]->pdev, 1 , 1);
//	printf("%d\r\n", signal.sival_int);
}
void timer_handler(union sigval sig)

{

       printf("signal_handler\n");


}
/**
 * @brief      打开Modbus设备
 * @details
 * @param
 * @return     int  函数执行结果
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
static STATUS_T OpenModbusDevice(struct ElementCtrlType* pdevice)
{
	STATUS_T ret = RET_UNKNOWN_ERR;

    clockid_t clockid=CLOCK_REALTIME;
    struct sigevent sev;

    sev.sigev_notify = SIGEV_THREAD;
    sev.sigev_signo = SIGRTMAX;
    sev.sigev_value.sival_ptr=&pdevice->timerID ;
    sev.sigev_notify_function = pdevice->close;//设置定时器回调

    /*创建定时器*/
    timer_create(clockid, &sev, &pdevice->timerID);
    printf("timer ID = %ld\r\n", *(long int*)pdevice->timerID);

    int flags = 0;

    struct itimerspec new_value;
    new_value.it_value.tv_sec = pdevice->holdtime;
    new_value.it_value.tv_nsec = 0;
    new_value.it_interval.tv_sec = 0;
    new_value.it_interval.tv_nsec = 0;

    /*启动定时器*/
    timer_settime(pdevice->timerID, flags, &new_value, NULL);

    /*打开modbus设备*/
	printf("Open device %s\r\n", pdevice->name);
	puts("Open device ");
	ModbusTCPSetBit(ElementCtrlTab[0]->pdev, 1 , 1);

	sleep(1);
	puts("Close device ");
	ModbusTCPSetBit(ElementCtrlTab[0]->pdev, 1 , 0);

	sleep(1);

    return ret;
}


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
	puts("Read XML"); /* prints !!!Hello World!!! */

	Config_Init();

	/*与场景元素建立连接并初始化设备*/
	puts("Init device");
	struct ModbusTCPConfType* pDevConf = malloc(sizeof(struct ModbusTCPConfType)); /*配置后期由xml文件给出*/
	ElementCtrlTab[0] = malloc(sizeof(struct ElementCtrlType));					   /*数据保存点 部分信息由xml文件给出*/

	/*模拟参数配置*/
	memcpy(pDevConf->ip, "192.168.2.232", sizeof("192.168.2.232"));
	pDevConf->port = 8080;
	pDevConf->slaveID = 1;
	ElementCtrlTab[0]->holdtime = 10;
	ElementCtrlTab[0]->open = OpenModbusDevice;
	ElementCtrlTab[0]->close = CloseModbusDevice;

	/*初始化协议*/
	ret = ModbusTCPMasterInit(pDevConf, &(ElementCtrlTab[0]->pdev));
	if(-1 == ret)
	{
		puts("Init error");
		return ret;
	}

//	ElementCtrlTab[0]->open(ElementCtrlTab[0]);

	OpenModbusDevice(ElementCtrlTab[0]);
	/*初始化modbus设备为安全态*/
	while(1)
	{
//		puts("Open device ");
//		ModbusTCPSetBit(ElementCtrlTab[0]->pdev, 1 , 1);
//
//		sleep(1);
//		puts("Close device ");
//		ModbusTCPSetBit(ElementCtrlTab[0]->pdev, 1 , 0);
//
//		sleep(1);

	}


	return EXIT_SUCCESS;
}


