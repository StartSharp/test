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
#include <string.h>

#define MAX_SCENE_ELEMENT_COUNT		20

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
	UINT16 currentCMD;							/*当前状态	上点后设置为初始状态*/
};

struct SceneElementSampleType{

};

struct SceneElementCtrlType* g_sceneElementCtrl[MAX_SCENE_ELEMENT_COUNT];
UINT16 g_acuSCCnt = 1;

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
	for(i = 0; i < g_acuSCCnt; i++)
	{
		if(id == g_sceneElementCtrl[i]->id)
		{
			*ptrOut = g_sceneElementCtrl[i];
			break;
		}
	}

	if(i != g_acuSCCnt + 1)
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
 * @brief     场景元素控制
 * @details	  通过场景元素ID及命令控制绑定设备
 * @param     UINT16 id
 * 			  UINT16 cmd
 * @return     STATUS_T ret执行结果
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
STATUS_T SceneElementCtrl(UINT16 id, UINT16 cmd)
{
	struct DeviceCtrlType* pHandler;
	STATUS_T ret = RET_UNKNOWN_ERR;
	struct SceneElementCtrlType* pSC;

	/*使用场景元素ID和CMD找操作句柄*/
	pHandler = GetDeviceHandlerFromCmd(id, cmd, &pSC);
	if(NULL != pHandler)
	{
		ret = DeviceCtrl(pHandler);
	}
	else
	{
		printf("No that id or cmd\r\n");
	}

	if(RET_NO_ERR == ret)
	{
		printf("Scene element ctrl completed!\r\n");
		pSC->currentCMD = cmd;
	}
	else
	{
		printf("Scene element ctrl failed!\r\n");
	}

	return ret;
}

/**
 * @brief     场景元素初始化
 * @details	  通过解析控制命令决定
 * @param     void* arg		操作句柄
 * @return     int  函数执行结果
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
STATUS_T SceneElementInit(void)
{
	STATUS_T ret = RET_UNKNOWN_ERR;
	UINT16 i;

	/*初始化所有设备*/
	DeviceCtrlIint();

	/*从XML文件中读取设备配置信息*/
	g_sceneElementCtrl[0] = malloc(sizeof(struct SceneElementCtrlType));

	g_sceneElementCtrl[0]->id = 0;
	strcpy(g_sceneElementCtrl[0]->name, "小障碍物");
	g_sceneElementCtrl[0]->ctrlTabLen = 2;
	g_sceneElementCtrl[0]->ctrlTab = malloc(g_sceneElementCtrl[0]->ctrlTabLen * sizeof(struct CtrlTabType));

	g_sceneElementCtrl[0]->ctrlTab[0].cmd = 0;
	g_sceneElementCtrl[0]->ctrlTab[0].deviceID = 0;
	g_sceneElementCtrl[0]->ctrlTab[1].cmd = 1;
	g_sceneElementCtrl[0]->ctrlTab[1].deviceID = 1;

	g_sceneElementCtrl[0]->defaultCMD = 0;

	/*根据ID补全操作句柄*/
	g_sceneElementCtrl[0]->ctrlTab->pdevice = GetDeviceHandlerFromID(g_sceneElementCtrl[0]->ctrlTab->deviceID);
	if(NULL == g_sceneElementCtrl[0]->ctrlTab->pdevice)
	{
		printf("Failed to get device handler.\r\n");
	}

	/*wait a moment*/
	UINT32 timecnt = 2000000;
	while(timecnt--);

	/*预定义命令下发*/
	for(i = 0; i < g_acuSCCnt; i++)
	{
		/*控制预设命令下发*/
		ret = SceneElementCtrl(i, g_sceneElementCtrl[i]->defaultCMD);
		if(RET_NO_ERR == ret)
		{
			printf("Scene element init completed!\r\n");
		}
		else
		{
			printf("Scene element init failed!\r\n");
		}
	}

	return ret;
}

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
	UINT16 id = 0, cmd = 0;

	while(1)
	{

		SceneElementCtrl(id, cmd);
		sleep(100);
	}
	return NULL;
}
