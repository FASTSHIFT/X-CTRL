#ifndef __COMMONMACRO_H
#define __COMMONMACRO_H

/*********************************/
/*          通用宏定义库         */
/*  提升代码的精简程度以及可读性 */
/*    Designed by _VIFEXTech     */
/*********************************/

//Finish 2019.3.21 v1.0 添加注释

/**
  * @brief  变量监视器，当变量改变时触发一个事件
  * @param  now:被监控的变量(整形)
  * @param  func:事件触发回调函数(可以是赋值等其他语句)
  * @retval 无
  */
#define __ValueMonitor(now,func) \
do{\
    static int last=now;\
    if(last!=now)func,last=now;\
}while(0)
#define __EventMonitor(now,func) __ValueMonitor(now,func)//兼容旧代码

/**
  * @brief  非阻塞式间隔指定时间执行一个函数
  * @param  func:被执行函数(可以是赋值等其他语句)
  * @param  time:设定间隔时间(ms)
  * @retval 无
  */
#define __IntervalExecute(func,time) \
do{\
    static uint32_t lasttime=0;\
    if(millis()-lasttime>=time)func,lasttime=millis();\
}while(0)

/**
  * @brief  将一个函数重复调用指定次数
  * @param  func:被调用函数(可以是赋值等其他语句)
  * @param  n:重复调用次数
  * @retval 无
  */
#define __LoopExecute(func,n) for(int i=0;i<n;i++)func

/**
  * @brief  将一个值限制在一个范围内
  * @param  x:被限制的值(任意类型)
  * @param  min:最小值(任意类型)
  * @param  max:最大值(任意类型)
  * @retval 无
  */
#define __LimitValue(x,min,max) (x=constrain(x,min,max))

/**
  * @brief  将一个值的变化区间线性映射到另一个区间
  * @param  x:被映射的值(任意类型)
  * @param  in_min:被映射的值的最小值
  * @param  in_min:被映射的值的最大值
  * @param  out_min:被映射的值的最小值
  * @param  out_min:被映射的值的最大值
  * @retval 映射值输出
  */
#define __Map(x,in_min,in_max,out_min,out_max) \
    ((x-in_min)*(out_max-out_min)/(in_max-in_min)+out_min)

/**
  * @brief  获取一个数组的元素个数
  * @param  arr:数组名(任意类型)
  * @retval 这个数组的元素个数
  */
#define __Sizeof(arr) (sizeof(arr)/sizeof(arr[0]))

/**
  * @brief  将一个值强制按指定类型解释，常用于结构体拷贝
  * @param  type:类型名(任意类型)
  * @param  data:被解释的数据(任意类型)
  * @retval 解释输出
  */
#define __TypeExplain(type,data) (*(type*)&data)

#endif
