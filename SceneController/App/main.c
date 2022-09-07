/*
 * main.c
 *
 *  Created on: Sep 1, 2022
 *      Author: tct
 */
#include "App.h"
#include "Platform.h"
#include <pthread.h>

pthread_t deviceRun;
pthread_t cmdRun;
int main(void)
{
	puts("--------------ScenenController software--------------"); /* prints !!!Hello World!!! */
	/*初始化场景元素*/

	/*与平台建立连接*/

	/*启动线程 自检线程 场景元素报备 场景元素命令解析*/

	pthread_create(&deviceRun, NULL, DeviceCtrlRunDamon, NULL);
	pthread_create(&cmdRun, NULL, DeviceCmd, NULL);

	while(1)
	{
		sleep(100);
	}
}
