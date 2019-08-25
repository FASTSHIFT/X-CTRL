#ifndef __COMMONMACRO_H
#define __COMMONMACRO_H

#define __Sizeof(arr) (sizeof(arr)/sizeof(arr[0]))
#define __LimitValue(x,min,max) (x=constrain(x,min,max))
#define __EventMonitor(now,func) \
do{\
    static int last=now;\
    if(last!=now)last=now,func;\
}while(0)
#define __LoopExecute(func,n) for(int i=0;i<n;i++)func
#define __IntervalExecute(func,time) \
do{\
    static uint32_t lasttime=0;\
    if(millis()-lasttime>=time)func,lasttime=millis();\
}while(0)

#define __Map(x,in_min,in_max,out_min,out_max) \
    ((x-in_min)*(out_max-out_min)/(in_max-in_min)+out_min)


#endif
