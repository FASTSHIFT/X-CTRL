/** @Author: _VIFEXTech
  * @Describe: X-CTRL专用协议框架
  * @Finished: UNKNOWTIME - v1.0 协议框架基本完成
  * @Upgrade:  UNKNOWTIME - v1.1 修改部分结构体
  * @Upgrade:  2018.12.25 - v1.2 去除TimePoint_Tran, 添加握手协议与时钟基准
  * @Upgrade:  2018.12.26 - v1.3 握手协议调试成功
  * @Upgrade:  2019.2.13  - v1.4 使用单字节对齐，保证跨平台兼容性
  * @Upgrade:  2020.1.20  - v1.5 添加跳频功能
  * @Upgrade:  2020.1.30  - v1.6 协议层重构，新协议命名为RCX，兼容旧协议
  * @Upgrade:  2020.3.25  - v1.7 完善框架，回传协议与发送协议统一，去除CtrlObject枚举
  * @Upgrade:  2020.3.27  - v1.8 添加通道反转功能
  */
#ifndef __RCX_CONFIG_H
#define __RCX_CONFIG_H

/*自定义*/
#define RCX_IS_MASTER             0
#define RCX_NAME                  "RCX-mini"
#define RCX_TYPE                  0
#define RCX_CONNECT_LOST_TIMEOUT  500
#define RCX_SIGNAL_LOST_TIMEOUT   6000

/*通道*/
#define RCX_CHANNEL_NUM         8
#define RCX_CHANNEL_DATA_MAX    1000
#define RCX_CHANNEL_DATA_LIMIT  0

/*功能*/
#define RCX_USE_CUSTOM_ID       0
#define RCX_USE_CRC             1
#define RCX_USE_PASSBACK        1
#define RCX_USE_FHSS            1
#define RCX_USE_CH_DEF          1

#endif
