#ifndef __PAGEMANAGER_H
#define __PAGEMANAGER_H

#include "stdint.h"

typedef void(*PageCallbackFunction_t)(void);

typedef struct{
	PageCallbackFunction_t SetupFunction;
	PageCallbackFunction_t LoopFunction;
	PageCallbackFunction_t ExitFunction;
	PageCallbackFunction_t ButtonPressEventFunction;
	PageCallbackFunction_t ButtonLongPressEventFunction;
	PageCallbackFunction_t EncoderEventFunction;
}PageList_TypeDef;

class PageManager {
public:
    PageManager(uint8_t PageNum);
	uint8_t NowPage, LastPage, NextPage;
	bool PageRegister_Basic(uint8_t PageNum, 
		PageCallbackFunction_t SetupFunc,
		PageCallbackFunction_t LoopFunc,
		PageCallbackFunction_t ExitFunc);
	bool PageRegister_ButtonPressEvent(uint8_t PageNum, PageCallbackFunction_t Function);
	bool PageRegister_ButtonLongPressEvent(uint8_t PageNum, PageCallbackFunction_t Function);
	bool PageRegister_EncoderEvent(uint8_t PageNum, PageCallbackFunction_t Function);

	bool PageClear(uint8_t PageNum);
	void PageChangeTo(uint8_t PageNum);
	void PageButtonPressEvent();
	void PageButtonLongPressEvent();
	void PageEncoderEvent();

	void Running();
private:
	PageList_TypeDef* PageList;
    uint8_t MaxPage;
    bool IsPageBusy;
};

#endif
