/** @Describe: X-CTRL专用协议框架
  * @Author: _VIFEXTech
  * @Finished: UNKNOWTIME - v1.0 协议框架基本完成
  * @Upgrade:  UNKNOWTIME - v1.1 修改部分结构体
  * @Upgrade:  2018.12.25 - v1.2 去除TimePoint_Tran, 添加握手协议与时钟基准
  * @Upgrade:  2018.12.26 - v1.3 握手协议调试成功
  * @Upgrade:  2019.2.13  - v1.4 使用单字节对齐，保证跨平台兼容性
  * @Upgrade:  2020.1.20  - v1.5 添加跳频功能
  * @Upgrade:  2020.1.30  - v1.6 协议层重构，新协议命名为RCX，兼容旧协议
  * @Upgrade:  2020.3.25  - v1.7 完善框架，回传协议与发送协议统一，去除CtrlObject枚举
  * @Upgrade:  2020.3.27  - v1.8 添加通道反转功能
  * @Upgrade:  2020.5.26  - v1.9 添加RCX_SLAVE_MAX到配置文件，规范Rx和Tx相关的函数名
  */
#ifndef __RCX_CONFIG_H
#define __RCX_CONFIG_H

/*自定义*/
#define RCX_IS_MASTER             0        /*0:从机  1:主机*/
#define RCX_NAME                  "GOODUSB"/*设备名称描述 (<10字符)*/
#define RCX_CONNECT_LOST_TIMEOUT  500      /*失控保护超时[ms]*/
#define RCX_SIGNAL_LOST_TIMEOUT   6000     /*掉线超时[ms]*/

#if ( RCX_IS_MASTER == 1 )
#  define RCX_SLAVE_MAX           5        /*可保存的从机的个数*/
#  define RCX_TYPE                RCX::TxGetObjectType() /*不要修改，主机控制类型需要和从机保持一致*/
#  define RCX_USE_CH_DEF          0        /*不使用通道宏定义头文件*/
#else
#  define RCX_TYPE                0x36     /*控制类型*/
#  define RCX_USE_CH_DEF          1        /*使用通道宏定义头文件*/
#endif

/*通道*/
#define RCX_CHANNEL_NUM         8          /*支持的通道个数*/
#define RCX_CHANNEL_DATA_MAX    1000       /*通道数据的最大值*/
#define RCX_CHANNEL_DATA_LIMIT  0          /*是否对通道数据进行限制保护*/

#define RCX_USE_CUSTOM_ID       0          /*0:使用随机ID  1~0xFF:自定义ID*/
#define RCX_USE_CRC             1          /*是否使用CRC进行数据包校验*/
#define RCX_USE_PASSBACK        1          /*是否使用回传功能*/
#define RCX_USE_FHSS            1          /*是否使用跳频功能*/

#endif
