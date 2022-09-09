/*
 * main.c
 *
 *  Created on: Sep 1, 2022
 *      Author: tct
 */
#include "App.h"
#include "Platform.h"
#include <pthread.h>

struct SoftVersionType{
	UINT32 s4	:	8;
	UINT32 s3   :	6;
	UINT32 s2	:	10;
	UINT32 s1	:	8;
};

struct SoftVersionUnionType{
	struct SoftVersionType bit;
	UINT32 all;
};

struct SoftVersionUnionType g_softversion = {{2,2,3,4}};

pthread_t deviceRun;
pthread_t cmdRun;
int main(void)
{
	printf("--------------ScenenController software version %d.%d.%d.%d------------\r\n", g_softversion.bit.s1, g_softversion.bit.s2, g_softversion.bit.s3, g_softversion.bit.s4); /* prints !!!Hello World!!! */

	/*初始化场景元素*/
	SceneElementInit();
	/*与平台建立连接*/

	/*启动线程 自检线程 场景元素报备 场景元素命令解析*/

	printf("Initial Scene Element\r\n");
	SceneElementReset(RESET_ALL, 0);

	pthread_create(&deviceRun, NULL, DeviceCtrlRunDamon, NULL);
	pthread_create(&cmdRun, NULL, TempCtrl, NULL);

	while(1)
	{
		sleep(100);
	}
}
