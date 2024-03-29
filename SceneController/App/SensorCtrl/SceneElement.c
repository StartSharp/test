/*
 * SceneElement.c
 *
 *  Created on: Sep 6, 2022
 *      Author: tct
 */

#include "SceneElement.h"

#include "SensorCtrl/DeviceCtrl.h"
#include "Platform.h"
#include "EndianChange.h"
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#define MAX_SCENE_ELEMENT_CONTROLER_COUNT		20
#define MAX_SCENE_ELEMENT_SAMPLER_COUNT			20

#define WORK_STATE_FAULT	0
#define WORK_STATE_NORMAL	1

struct CtrlTabType{
	UINT16 cmd;									/*控制命令 xml文件读入*/
	UINT16 deviceID;							/*关联的设备操作ID xml文件读入*/
	struct DeviceCtrlType* pdevice;			/*关联的设备操作句柄 初始化时按ID查找写入*/
};

struct SceneElementCtrlType{
	UINT16 id;									/*场景元素ID XML读入*/
	char name[20];								/*场景元素名称 xml读入*/
	UINT16 ctrlTabLen;							/*控制表长度 xml读入*/
	struct CtrlTabType* ctrlTab;				/*指向控制表 XML读入*/
	UINT16 defaultCMD;							/*初始状态指令 xml读入*/
	UINT32 defaultPara;							/*初始指令参数 xml读入*/
	UINT16 currentCMD;							/*当前命令*/
	UINT32 currentCmdPara;						/*当前命令参数*/
	UINT16 currentSta;							/*当前状态*/
	UINT32 currentStaPara;						/*当前状态参数*/
	UINT8 linesta;								/*在线状态*/
	UINT8 worksta;								/*工作状态*/
	struct timeval cmdStamp;					/*命令时间戳*/
};

struct SceneElementSampleType{
	UINT16 id;									/*反馈场景元素ID	xml读取*/
	char name[20];								/*反馈场景元素名称 xml读入*/
	UINT16 bindGroup;							/*绑定的数据组		xml读取*/
	UINT16 length;								/*所在数据组的长度	xml读取*/
	UINT16 offset;								/*所在数据组的偏移	xml读取*/

	UINT8 linesta;								/*在线状态*/
	UINT8 worksta;								/*工作状态*/
	char* pdata;								/*数据地址	初始化时更新*/
	struct InputDataGroupType* pGroup;			/*关联组的操作句柄	初始化时更新*/
};

struct CmdFrameType{
	UINT16 id;
	UINT16 cmd;
	UINT32 para;
}ALIGN(1);

struct SCCtrlStateSendType{
	UINT16 id;
	UINT8 lineSta;
	UINT8 workSta;
	UINT16 cmd;
	UINT32 cmdPara;
	UINT16 sta;
	UINT32 staPara;
	struct timeval stamp;
}ALIGN(1);

struct SCSampleStateSendType{
	UINT16 id;
	UINT8 lineSta;
	UINT8 workSta;
	UINT32 sample;
	struct timeval stamp;
}ALIGN(1);

struct SceneElementCtrlType* g_sceneElementCtrl[MAX_SCENE_ELEMENT_CONTROLER_COUNT];
struct SceneElementSampleType* g_sceneElementSample[MAX_SCENE_ELEMENT_SAMPLER_COUNT];
UINT16 g_acuSCCtrlCnt = 1;
UINT16 g_acuSCSampleCnt = 1;

pthread_t g_deviceCtrlRun;
pthread_t g_sampleRun;
/**
 * @brief     通过场景元素ID和命令查找操作句柄
 * @details	  通过解析控制命令决定
 * @param     UINT16 ID 场景元素ID
 * 			  UINT16    命令
 * @return     struct ElementCtrlType*  操作句柄
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
static struct DeviceCtrlType* GetDeviceHandlerFromCmd(UINT16 id, UINT16 cmd, struct SceneElementCtrlType** ptrOut)
{
	struct DeviceCtrlType* ptr = NULL;
	UINT16 i, j;

	/*根据ID找场景元素控制句柄*/
	for(i = 0; i < g_acuSCCtrlCnt; i++)
	{
		if(id == g_sceneElementCtrl[i]->id)
		{
			*ptrOut = g_sceneElementCtrl[i];
			break;
		}
	}

	if(i != g_acuSCCtrlCnt)
	{
		/*根据命令在场景元素控制句柄中找设备操作句柄*/
		for(j = 0; j < g_sceneElementCtrl[i]->ctrlTabLen; j++)
		{
			if(cmd == g_sceneElementCtrl[i]->ctrlTab[j].cmd)
			{
				ptr = g_sceneElementCtrl[i]->ctrlTab[j].pdevice;
				break;
			}
		}
	}

	return ptr;
}

/**
 * @brief     通过场景元素ID和命令查找操作句柄
 * @details	  通过解析控制命令决定
 * @param     UINT16 ID 场景元素ID
 * 			  UINT16    命令
 * @return     struct ElementCtrlType*  操作句柄
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
static struct DeviceCtrlType* GetDeviceHandlerFromDefaultCmd(UINT16 id, struct SceneElementCtrlType** ptrOut)
{
	struct DeviceCtrlType* ptr = NULL;
	UINT16 i, j;

	/*根据ID找场景元素控制句柄*/
	for(i = 0; i < g_acuSCCtrlCnt; i++)
	{
		if(id == g_sceneElementCtrl[i]->id)
		{
			*ptrOut = g_sceneElementCtrl[i];
			break;
		}
	}

	if(i != g_acuSCCtrlCnt + 1)
	{
		/*根据命令在场景元素控制句柄中找设备操作句柄*/
		for(j = 0; j < g_sceneElementCtrl[i]->ctrlTabLen; j++)
		{
			if(g_sceneElementCtrl[i]->defaultCMD == g_sceneElementCtrl[i]->ctrlTab[j].cmd)
			{
				ptr = g_sceneElementCtrl[i]->ctrlTab[j].pdevice;
				break;
			}
		}
	}

	return ptr;
}
/**
 * @brief     场景元素控制
 * @details	  通过场景元素ID及命令控制绑定设备
 * @param     UINT16 id
 * 			  UINT16 cmd
 * 			  UINT32 para  命令参数
 * @return     STATUS_T ret执行结果
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
STATUS_T SceneElementCtrl(UINT16 id, UINT16 cmd, UINT32 para)
{
	struct DeviceCtrlType* pHandler;
	STATUS_T ret = RET_UNKNOWN_ERR;
	struct SceneElementCtrlType* pSC;

	/*使用场景元素ID和CMD找操作句柄*/
	pHandler = GetDeviceHandlerFromCmd(id, cmd, &pSC);
	if(NULL != pHandler)
	{
		pSC->currentCMD = cmd;
		pSC->currentCmdPara = para;
        gettimeofday(&pSC->cmdStamp, NULL);
		ret = DeviceCtrl(pHandler, para);
		if(ret == RET_NOCONN_ERR)
			pSC->worksta = WORK_STATE_FAULT;
		else
			pSC->worksta = WORK_STATE_NORMAL;
	}
	else
	{
		printf("No that id or cmd\r\n");
	}

	/*当前状态更新*/
	if(RET_NO_ERR == ret)
	{
		printf("Scene element ctrl completed!\r\n");
		pSC->currentSta = cmd;
		pSC->currentStaPara = para;
	}
	else
	{
		printf("Scene element ctrl failed!\r\n");
		pSC->worksta = WORK_STATE_FAULT;
	}

	return ret;
}

/**
 * @brief     场景元素控制 为命令模块提供的控制接口
 * @details	  通过场景元素ID及命令控制绑定设备
 * @param     UINT16 id
 * 			  UINT16 cmd
 * @return     STATUS_T ret执行结果
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
STATUS_T SceneElementCMDCtrl(void* pdata)
{
	STATUS_T ret = RET_UNKNOWN_ERR;

	struct CmdFrameType* frame = (struct CmdFrameType*)pdata;

#ifdef LITTLE_ENDIAN_SQUENCE
	frame->id = ChangeEndian16(frame->id);
	frame->cmd = ChangeEndian16(frame->cmd);
	frame->para = ChangeEndian32(frame->para);
#endif

	ret = SceneElementCtrl(frame->id, frame->cmd, frame->para);

	return ret;
}


/**
 * @brief     获取采集场景元素对应的采样值
 * @details
 * @param	  UINT16 id, 场景元素ID
 * 				char* pbuf, 场景元素当前采样
 * 				UINT16 len	场景元素采样指针所指区域大小
 * @return     int  函数执行结果
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
STATUS_T GetSCSampleFromHandler(struct SceneElementSampleType* pSCHandler, UINT8* pbuf, UINT16 len, struct timeval* timestampus)
{
	STATUS_T ret = RET_UNKNOWN_ERR;

	if(NULL != pSCHandler)
	{
		if(len >= pSCHandler->length)
		{
			ret = RET_NO_ERR;
			memcpy(pbuf, pSCHandler->pdata + pSCHandler->offset, pSCHandler->length);
			*timestampus = pSCHandler->pGroup->stampus;
		}
		else
		{
			ret = RET_MEM_ERR;
		}
	}
	else
	{
		ret = RET_ID_ERR;
	}

	return ret;
}

/**
 * @brief     场景元素导向安全操作
 * @details	  通过解析预设命令控制场景元素导向安全
 * @param     UINT16 id   场景元素ID
 * 			  enum RESET_MODE_TYPE mode   可选择全部初始化或某个初始化，若选择全部初始化id无意义
 * @return     int  函数执行结果
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
STATUS_T SceneElementReset(enum RESET_MODE_TYPE mode, UINT16 id)
{
	STATUS_T ret = RET_UNKNOWN_ERR;
	struct DeviceCtrlType* pHandler;
	struct SceneElementCtrlType* pSC;
	UINT16 i;

	if(RESET_ALL == mode)
	{
		for(i = 0; i < g_acuSCCtrlCnt; i++)
		{
			/*使用场景元素ID和CMD找操作句柄*/
			pHandler = GetDeviceHandlerFromDefaultCmd(g_sceneElementCtrl[i]->id, &pSC);
			if(NULL != pHandler)
			{
				pSC->currentCMD = pSC->defaultCMD;
				ret = DeviceCtrl(pHandler, pSC->defaultPara);
			}
			else
			{
				printf("No that id or cmd\r\n");
			}
		}
	}
	else if(RESET_ANYONE == mode)
	{
		/*使用场景元素ID和CMD找操作句柄*/
		pHandler = GetDeviceHandlerFromDefaultCmd(id, &pSC);
		if(NULL != pHandler)
		{
			pSC->currentCMD = pSC->defaultCMD;
			ret = DeviceCtrl(pHandler, pSC->defaultPara);
		}
		else
		{
			printf("No that id or cmd\r\n");
		}
	}


	/*当前状态更新*/
	if(RET_NO_ERR == ret)
	{
		printf("Scene element ctrl completed!\r\n");
		pSC->currentSta = pSC->defaultCMD;
	}
	else
	{
		printf("Scene element ctrl failed!\r\n");
	}

	return ret;


	return ret;
}

/**
 * @brief     获取采样类场景元素状态
 * @details	  获取场景元素状态 数据排列顺序为自定义顺序
 * @param     UINT8* pbuf, 		数据存放空间
 * 			  UINT16 areasize	空间大小
 * 			   UINT16* pSCcnt	场景元素个数
 * @return     占用空间大小
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
UINT16 GetSCSamplerSta(UINT8* pbuf, UINT16 areasize, UINT16* pSCcnt)
{
	STATUS_T ret = RET_UNKNOWN_ERR;
	struct SCSampleStateSendType* pdata = (struct SCSampleStateSendType*)pbuf;
	*pSCcnt = g_acuSCSampleCnt;
	UINT16 dataSize, i;
	struct timeval stamp;

	dataSize = g_acuSCSampleCnt * sizeof(struct SCSampleStateSendType);
	if(areasize >= dataSize)
	{
		for(i = 0; i < g_acuSCCtrlCnt; i++)
		{
			pdata[i].id = g_sceneElementSample[i]->id;
			pdata[i].lineSta = g_sceneElementSample[i]->linesta;
			pdata[i].workSta = g_sceneElementSample[i]->worksta;

			ret = GetSCSampleFromHandler(g_sceneElementSample[i], (UINT8*)&pdata[i].sample, (UINT16)sizeof(UINT32), &stamp);
			if(RET_NO_ERR == ret)
			{
				pdata[i].stamp.tv_sec = stamp.tv_sec;
				pdata[i].stamp.tv_usec = stamp.tv_usec;
			}
			else
			{
				pdata[i].stamp.tv_sec = 0xFFFFFFFF;
				pdata[i].stamp.tv_usec = 0xFFFFFFFF;
			}
#ifdef LITTLE_ENDIAN_SQUENCE
			pdata[i].id = ChangeEndian16(pdata[i].id);
			pdata[i].sample = ChangeEndian32(pdata[i].sample);
			pdata[i].stamp.tv_sec = ChangeEndian64(pdata[i].stamp.tv_sec);
			pdata[i].stamp.tv_usec = ChangeEndian64(pdata[i].stamp.tv_usec);
#endif
		}
	}
	else
	{
		dataSize = 0;
	}

	return dataSize;
}

/**
 * @brief     获取控制类场景元素状态
 * @details	  获取场景元素状态 数据排列顺序为自定义顺序
 * @param     UINT8* pbuf, 		数据存放空间
 * 			  UINT16 areasize	空间大小
 * 			   UINT16* pSCcnt	场景元素个数
 * @return     占用空间大小
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
UINT16 GetSCCtrllerSta(UINT8* pbuf, UINT16 areasize, UINT16* pSCcnt)
{
	UINT16 i;
	struct SCCtrlStateSendType* pdata = (struct SCCtrlStateSendType* )pbuf;

	*pSCcnt = g_acuSCCtrlCnt;
	UINT16	dataSize = g_acuSCCtrlCnt *sizeof(struct SCCtrlStateSendType);
	if(areasize >= dataSize)
	{

		for(i = 0; i < g_acuSCCtrlCnt; i++)
		{
			pdata[i].id = g_sceneElementCtrl[i]->id;
			pdata[i].lineSta = g_sceneElementCtrl[i]->linesta;
			pdata[i].workSta = g_sceneElementCtrl[i]->worksta;
			pdata[i].cmd = g_sceneElementCtrl[i]->currentCMD;
			pdata[i].cmdPara = g_sceneElementCtrl[i]->currentCmdPara;
			pdata[i].sta = g_sceneElementCtrl[i]->currentSta;
			pdata[i].staPara = g_sceneElementCtrl[i]->currentStaPara;
			pdata[i].stamp.tv_sec = g_sceneElementCtrl[i]->cmdStamp.tv_sec;
			pdata[i].stamp.tv_usec = g_sceneElementCtrl[i]->cmdStamp.tv_usec;
#ifdef LITTLE_ENDIAN_SQUENCE
			pdata[i].id = ChangeEndian16(pdata[i].id);
			pdata[i].cmd = ChangeEndian16(pdata[i].cmd);
			pdata[i].cmdPara = ChangeEndian32(pdata[i].cmdPara);
			pdata[i].sta = ChangeEndian16(pdata[i].sta);
			pdata[i].staPara = ChangeEndian32(pdata[i].staPara);
			pdata[i].stamp.tv_sec = ChangeEndian64(pdata[i].stamp.tv_sec);
			pdata[i].stamp.tv_usec = ChangeEndian64(pdata[i].stamp.tv_usec);
#endif

		}

	}
	else
	{
		dataSize = 0;
	}

	return dataSize;
}

/**
 * @brief     场景元素初始化
 * @details	  通过解析控制命令决定
 * @param     void* arg		操作句柄
 * @return     int  函数执行结果
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
STATUS_T SceneElementCtrlRun(void)
{
	STATUS_T ret = RET_UNKNOWN_ERR;
	UINT16 i;

	/*初始化所有设备*/
	DeviceCtrlIint();

	/*从XML文件中读取设备配置信息*/
	g_sceneElementCtrl[0] = malloc(sizeof(struct SceneElementCtrlType));
	memset(g_sceneElementCtrl[0], 0, sizeof(struct SceneElementCtrlType));

	g_sceneElementCtrl[0]->id = 0;
	strcpy(g_sceneElementCtrl[0]->name, "小障碍物");
	g_sceneElementCtrl[0]->ctrlTabLen = 2;
	g_sceneElementCtrl[0]->ctrlTab = malloc(g_sceneElementCtrl[0]->ctrlTabLen * sizeof(struct CtrlTabType));

	g_sceneElementCtrl[0]->ctrlTab[0].cmd = 0;
	g_sceneElementCtrl[0]->ctrlTab[0].deviceID = 0;
	g_sceneElementCtrl[0]->ctrlTab[1].cmd = 1;
	g_sceneElementCtrl[0]->ctrlTab[1].deviceID = 1;


	/*根据ID补全操作句柄*/
	for(i = 0; i < g_sceneElementCtrl[0]->ctrlTabLen; i++)
	{
		g_sceneElementCtrl[0]->ctrlTab[i].pdevice = GetDeviceHandlerFromID(g_sceneElementCtrl[0]->ctrlTab[i].deviceID);
		if(NULL == g_sceneElementCtrl[0]->ctrlTab[i].pdevice)
		{
			printf("Failed to get device handler[%d].\r\n", i);
		}
		else
		{
			printf("Success get deice handler\r\n");
		}
	}

	/*wait a moment*/
	int resTime = 10;
	while(resTime)
	{
		resTime = sleep(resTime);
	}

	pthread_create(&g_deviceCtrlRun, NULL, DeviceCtrlRunDamon, NULL);

	return ret;
}

/**
 * @brief     获取场景元素对应的操作句柄
 * @details
 * @param		UINT16 id	组别ID
 * @return     int  函数执行结果
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
struct SceneElementSampleType* GetSCSampleHandler(UINT16 id)
{
	struct SceneElementSampleType* pHandler = NULL;
	UINT16 i;

	for(i = 0; i < g_acuSCSampleCnt; i++)
	{
		if(id == g_sceneElementSample[i]->id)
		{
			pHandler = g_sceneElementSample[i];
			break;
		}
	}

	return pHandler;
}

/**
 * @brief     获取采集场景元素对应的采样值
 * @details
 * @param	  UINT16 id, 场景元素ID
 * 				char* pbuf, 场景元素当前采样
 * 				UINT16 len	场景元素采样指针所指区域大小
 * @return     int  函数执行结果
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
STATUS_T GetSCSample(UINT16 id, char* pbuf, UINT16 len, struct timeval* timestampus)
{
	STATUS_T ret = RET_UNKNOWN_ERR;
	struct SceneElementSampleType* pSCHandler = GetSCSampleHandler(id);

	if(NULL != pSCHandler)
	{
		if(len >= pSCHandler->length)
		{
			ret = RET_NO_ERR;
			memcpy(pbuf, pSCHandler->pdata + pSCHandler->offset, pSCHandler->length);
			*timestampus = pSCHandler->pGroup->stampus;
		}
		else
		{
			ret = RET_MEM_ERR;
		}
	}
	else
	{
		ret = RET_ID_ERR;
	}

	return ret;
}

/**
 * @brief     场景元素初始化
 * @details	  通过解析控制命令决定
 * @param
 * @return     int  函数执行结果
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
STATUS_T SceneElementSamplerInit(void)
{
	STATUS_T ret = RET_UNKNOWN_ERR;

	/*初始化采样组*/
	ret = DeviceCtrlSampleInit();
	if(RET_NO_ERR == ret)
	{
		/*初始化场景元素 读取配置文件*/
		g_sceneElementSample[0] = malloc(sizeof(struct SceneElementSampleType));
		g_sceneElementSample[0]->id = 128;
		strcpy(g_sceneElementSample[0]->name, "被测平台最近障碍物距离");
		g_sceneElementSample[0]->bindGroup = 0;
		g_sceneElementSample[0]->length = 2;
		g_sceneElementSample[0]->offset = 3;
		g_sceneElementSample[0]->pGroup = GetGroupHandler(g_sceneElementSample[0]->bindGroup);
		g_sceneElementSample[0]->pdata = g_sceneElementSample[0]->pGroup->pBuf;
	}
	else
	{

	}

	pthread_create(&g_sampleRun, NULL, DeviceCtrlSampleDamon, NULL);

	return ret;
}
time_t sec;
struct timeval us;
char data[1000] = {0};
/**
 * @brief     设备层暂时控制函数接口
 * @details	  通过解析控制命令决定
 * @param     void* arg		操作句柄
 * @return     int  函数执行结果
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
void* TempCtrl(void* arg)
{
	sleep(10);

	printf("sizeof = %ld, %ld\r\n",sizeof(us.tv_sec), sizeof(us.tv_usec));
	while(1)
	{
//		SceneElementCtrl(0, 0);
//		sleep(20);
//		SceneElementCtrl(0, 1);
//		sleep(20);
//		GetSCSample(128, data, 1000, &us);
//
//		struct tm* ptm = localtime(&us.tv_sec);
//		char szTmp[50] = {0};
//		strftime(szTmp, 50, "%H:%M:%S",ptm);
//		printf("%s \n", szTmp);
//		printf("%ld:%ld\r\n", us.tv_sec, us.tv_usec);


	}
	return NULL;
}
