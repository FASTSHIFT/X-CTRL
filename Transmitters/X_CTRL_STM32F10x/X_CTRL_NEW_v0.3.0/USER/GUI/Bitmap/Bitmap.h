#ifndef __BITMAP_H 
#define __BITMAP_H

#ifdef __cplusplus
extern "C" {
#endif
	
#include "Bitmap_Ctrl.c"
#include "Bitmap_Option.c"
#include "Bitmap_Common.c"
#include "Bitmap_RC.c"
#include "Bitmap_DS.c"

#include "Bitmap_Logo1.c"
#include "Bitmap_Logo2.c"
#include "Bitmap_Logo3.c"
#include "Bitmap_Logo4.c"
#include "Bitmap_Logo5.c"
#include "Bitmap_Logo6.c"
#include "Bitmap_Logo7.c"
#include "Bitmap_Logo8.c"
#include "Bitmap_Logo9.c"
#include "Bitmap_Logo10.c"
#include "Bitmap_Logo11.c"
#include "Bitmap_Logo12.c"
	
const unsigned char* gImage_Bitmap_Logox[] = {
    gImage_Bitmap_Logo1,
    gImage_Bitmap_Logo2,
    gImage_Bitmap_Logo3,
    gImage_Bitmap_Logo4,
    gImage_Bitmap_Logo5,
    gImage_Bitmap_Logo6,
    gImage_Bitmap_Logo7,
    gImage_Bitmap_Logo8,
    gImage_Bitmap_Logo9,
    gImage_Bitmap_Logo10,
    gImage_Bitmap_Logo11,
    gImage_Bitmap_Logo12,
};
	
#ifdef __cplusplus
}
#endif

#endif
