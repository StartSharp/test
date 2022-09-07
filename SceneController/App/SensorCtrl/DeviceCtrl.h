/*
 * DeviceCtrl.h
 *
 *  Created on: Sep 1, 2022
 *      Author: tct
 */

#ifndef APP_SENSORCTRL_DEVICECTRL_H_
#define APP_SENSORCTRL_DEVICECTRL_H_

#include "common.h"
#include  <time.h>
#include "Platform.h"

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

struct DeviceCtrlType{
	UINT16 ID;					/*设备操作ID*/
	char name[20];				/*名称*/
	enum ProtocolType pro;		/*协议*/
	void* pdev;					/*操作句柄*/
	UINT16 holdtime;			/*持续时间 单位s*/
	void* proConf;				/*指向不同协议的配置参数*/
	softTimer_st timer;

	STATUS_T (*open)(struct DeviceCtrlType* pHandler);
	void (*close)(UINT16 argc, void* argv);
};

/*输入型场景元素采样参数配置*/
struct RawDataConfType{
	enum ProtocolType pro;

};

struct InputDataGroupType{
	UINT16 groupID;				/*场景元素ID  xml文件读取*/
	char name[20];				/*名称 xml文件读取*/
	enum ProtocolType pro;		/*通信协议 xml文件读取*/
	void* initConf;				/*初始化配置 xml文件读取*/
	void* handler;				/*操作句柄   自动生成*/
	UINT32 datasize;			/*数据长度 xml文件读取*/
};

extern struct DeviceCtrlType* g_deviceCtrlTab[MAX_OUTPUT_ELEMENT_COUNT];

/**
 * @brief      	查找操作句柄
 * @details		根据操作ID找到对应的操作句柄
 * @param		UINT16 id	查找ID
 * @return     struct ElementCtrlType* 操作句柄
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
struct DeviceCtrlType* GetDeviceHandlerFromID(UINT16 id);

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

/**
 * @brief     设备层初始化服务
 * @details
 * @param
 * @return     int  函数执行结果
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
int DeviceCtrlIint(void);

#endif /* APP_SENSORCTRL_DEVICECTRL_H_ */
