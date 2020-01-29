#include <time.h>
#include "delay.h"

time_t time(time_t * time)  
{  
    return millis();  
}  
  
void exit(int status)  
{
    while(1);
}

int system(const char * string)  
{  
    return 0;  
}  

int remove(const char *filename)
{
    return 0;
}

clock_t clock()
{
    return millis();
}
