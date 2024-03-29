/*
 * MobusTCPOps.c
 *
 *  Created on: Sep 1, 2022
 *      Author: tct
 */
#include "ModbusTCPOps.h"
#include <unistd.h>
/**
 * @brief      Modbustcp从站连接
 * @details    初始化MODBUS tcp  设置从站地址
 * @param      struct ModbusTCPConfType* ptr 输入 操作结构体
 * 			   void* pHandler	输出操作句柄
 * @return     int  函数执行结果
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
int ModbusTCPMasterInit(void* ptr, void** pHandler)
{
	struct ModbusTCPConfType* pConf = ptr;
	modbus_t* pCtx;
	int ret = -1;
//	uint16_t tab_reg[100] = {0};
	if(NULL != ptr)
	{
		/*初始化modbusTCP配置*/
		pCtx = modbus_new_tcp(pConf->ip, pConf->port);
		/*设置从机地址*/
		ret = modbus_set_slave(pCtx, pConf->slaveID);
		if (-1 != ret)
		{
			/*连接modbus设备*/
			ret = modbus_connect(pCtx);
			if(-1 != ret)
			{
				printf("Connect to ip = %s port = %d device success!!!\r\n",pConf->ip, pConf->port);

				*pHandler = pCtx;
			}
			else
			{
				printf("Connect to ip = %s port = %d device failed!\r\n",pConf->ip, pConf->port);
				modbus_free(pCtx);
			}
		}
		else
		{
			fprintf(stderr, "Invalid slave ID\n");
			modbus_free(pCtx);
		}
	}
	else
	{
		fprintf(stderr, "Invalid input ID\n");
	}

	return ret;
}

/**
 * @brief      modbusTCP线圈写入
 * @details    写线圈指
 * @param      void* pHandler	输出操作句柄
 * 			   UINT16 addr 	线圈地址
 * 			   UINT16 state 控制开关
 * @return     int  函数执行结果
 *     - RET_NO_ERR  成功
 *     - ohter       失败
 */
int ModbusTCPSetBit(void* pHandler, UINT16 addr, UINT16 state)
{
	modbus_t* pCtx = pHandler;
	int ret = -1;

	if(NULL != pCtx)
	{
		/*控制线圈*/
		ret = modbus_write_bit(pCtx, addr, state?1:0);
		if(-1 != ret)
		{
			fprintf(stderr, "modbus operation ok\r\n");
		}
		else
		{
			fprintf(stderr, "modbus operation error\r\n");
		}

	}
	else
	{
		fprintf(stderr, "Invalid input handler\n");
	}

	return ret;
}
