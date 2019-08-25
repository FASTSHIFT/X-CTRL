#ifndef __STRPRINTER_H__
#define __STRPRINTER_H__

#include <stdint.h>

extern const char Fonts_Bold11px[];
extern const char Fonts_MicrosoftYahei11px[];
extern uint32_t SP_Buffer_Width;
extern uint32_t SP_Buffer_Height;
extern uint32_t SP_Buffer_Length;
extern uint16_t SP_Buffer[];
extern uint8_t  SP_Brush_Opacity;

void SP_GetRect(const char *map, char *str, uint8_t xoffset, uint32_t *width, uint32_t *height);
void SP_PrepareRect(const char *map, char *str, uint8_t xoffset);
void SP_FillBuffer_SolidBrush(uint16_t color);
void SP_CoverString(const char *map, char *str, uint8_t xoffset, uint16_t foreColor);

#endif

