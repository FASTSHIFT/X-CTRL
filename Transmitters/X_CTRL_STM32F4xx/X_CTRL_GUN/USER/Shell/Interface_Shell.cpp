#include "ShellPrivate.h"
#include "GUI/DisplayPrivate.h"

/**
 * Function to read data from serial port
 * Functions to read from physical media should use this prototype:
 * int my_reader_function(char * data)
 */
static int Shell_InterfaceReader(char * data)
{
    // Wrapper for Serial.read() method
    if (Serial.available())
    {
        *data = Serial.read();
        return 1;
    }
    return 0;
}

static String Shell_OutputText;

/**
 * Function to write data to serial port
 * Functions to write to physical media should use this prototype
 * void my_writer_function(char data)
 */
static void Shell_InterfaceWriter(char data)
{
    // Wrapper for Serial.write() method
    Serial.write(data);
    
    if(data == '\t') data = ' ';
    else if(data == '\r') return;
    Shell_OutputText += data;
}

static void Shell_DisplayUpdate()
{
    terminal_add(Shell_OutputText.c_str());
    Shell_OutputText = "";
}

static int Shell_Exit(int argc, char** argv)
{
    page.PagePop();
    return SHELL_RET_SUCCESS;
}

void Shell_Init()
{
    shell_set_prompt_msg(FS_GetPromptMsg());
    shell_set_prompt_finish_hook(Shell_DisplayUpdate);
    shell_init(Shell_InterfaceReader, Shell_InterfaceWriter, NULL);
    
    shell_register(Shell_Exit, "exit");
    
    ShellReg_FS();
    ShellReg_HW();
}
