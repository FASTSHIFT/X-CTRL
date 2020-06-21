#ifndef __SHELLPRIVATE_H
#define __SHELLPRIVATE_H

#include "Basic/FileGroup.h"
#include "Shell/Shell.h"

/*Shell*/
void Shell_Init();
void terminal_add(const char * txt_in);

void ShellReg_HW();

/*FileSystem*/
const char* FS_GetPromptMsg();
void ShellReg_FS();

#endif
