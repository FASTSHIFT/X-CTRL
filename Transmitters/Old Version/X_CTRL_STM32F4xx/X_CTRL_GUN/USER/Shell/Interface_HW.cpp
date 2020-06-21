#include "ShellPrivate.h"
#include "BSP/BSP.h"

static int HW_GPIO_Ctrl(int argc, char** argv)
{
    enum ioctrl_switches
    {
        E_PIN_MODE,
        E_DIGITAL_WRITE,
        E_DIGITAL_READ,
    };

    int i;
    int pin = 0xFF, mode = 0xFF, val = 0xFF;
    int op = 0;

    // If only the program name is given show message to the user about program usage
    if(argc == 1)
    {
        shell_println("SHELL IO CONTROL UTILITY");
        shell_println("This tool can manipulate the IO pins on the arduino boards using text commands");
        shell_println("Type ioctrl -help for more information.");
        shell_println("");
        return SHELL_RET_SUCCESS;
    }

    // Parse command arguments
    for(i = 0; i < argc; i++)
    {
        if(i == 0)
            continue;
        if(!strcmp(argv[i], "-help"))
        {
            shell_println("This tool can manipulate the IO pins on the arduino boards using text commands");
            shell_println("Arguments in square brackets [ ] are mandatory; arguments in curly brackets { } are optional");
            shell_println("");
            shell_println("AVAILABLE SWITCHES:");
            shell_println("  -p [PIN_NUMBER]     -> \"Sets\" the pin (PIN_NUMBER) to perform other operations on the pin");
            shell_println("  -m [INPUT|OUTPUT]   -> \"Configures\" PIN mode as INPUT or OUTPUT");
            shell_println("  -w [LOW|HIGH]       -> \"Writes\" a digital output value LOW or HIGH");
            shell_println("  -r                  -> \"Reads\" the state of a digital input or output");
            shell_println("  -help               -> Shows this help message");
            shell_println("");
            return SHELL_RET_SUCCESS;
        }
        // Asign pin to read / write or configure
        else if(!strcmp(argv[i], (const char *) "-p"))
        {
            if(i + 1 > argc)
                goto argerror;
            pin = strtol(argv[i + 1], 0, 0);
            shell_printf("#ioctrl-selpin:%d\r\n", pin);
        }
        // Configure the pin mode as input or output
        else if(!strcmp(argv[i], (const char *) "-m"))
        {
            if(i + 1 > argc)
                goto argerror;
            op = E_PIN_MODE;
            if(!strcmp(argv[i + 1], (const char *) "INPUT"))
                mode = INPUT;
            else if(!strcmp(argv[i + 1], (const char *) "OUTPUT"))
                mode = OUTPUT;
        }
        // Writes a digital value to IO pin
        else if(!strcmp(argv[i], (const char *) "-w"))
        {
            if(i + 1 > argc)
                goto argerror;
            op = E_DIGITAL_WRITE;
            if(!strcmp(argv[i + 1], (const char *) "LOW"))
                val = LOW;
            else if(!strcmp(argv[i + 1], (const char *) "HIGH"))
                val = HIGH;
        }
        else if(!strcmp(argv[i], (const char *) "-r"))
        {
            op = E_DIGITAL_READ;
        }
    }
    // Check valid pin number
    if(pin == 0xFF)
    {
        shell_print_error(E_SHELL_ERR_VALUE, 0);
        return SHELL_RET_FAILURE;
    }

    //  Perform operations on the IO pins with the provided information
    switch(op)
    {
    case E_PIN_MODE:
        shell_print("#ioctrl-mode:");
        if(mode == INPUT)
            shell_println("INTPUT");
        else if( mode == OUTPUT )
            shell_println("OUTPUT");
        else
        {
            shell_print_error(E_SHELL_ERR_VALUE, "Mode");
            return SHELL_RET_FAILURE;
        }
        pinMode(pin, (pinMode_TypeDef)mode);
        break;

    case E_DIGITAL_WRITE:
        shell_print("#ioctrl-write:");
        if(val == HIGH)
            shell_println("HIGH");
        else if(val == LOW)
            shell_println("LOW");
        else
        {
            shell_print_error(E_SHELL_ERR_VALUE, "Value");
            return SHELL_RET_FAILURE;
        }
        digitalWrite(pin, val);
        break;

    case E_DIGITAL_READ:
        shell_print("#ioctrl-read:");
        if(digitalRead(pin) == HIGH)
            shell_println("HIGH");
        else
            shell_println("LOW");
        break;
    }

    return SHELL_RET_SUCCESS;
    // Exit point for missing arguments in any part of the function
argerror:
    shell_print_error(E_SHELL_ERR_ARGCOUNT, 0);
    shell_println("");
    return SHELL_RET_FAILURE;

}

static int HW_SysReboot(int argc, char** argv)
{
    NVIC_SystemReset();
    return SHELL_RET_SUCCESS;
}

static int HW_SysShutdown(int argc, char** argv)
{
    Power_Shutdown();
    return SHELL_RET_SUCCESS;
}

static int HW_SetBaudrate(int argc, char** argv)
{
    static const uint32_t baudrateGrp[] = {
        1200, 2400, 4800, 9600, 14400,
        19200, 38400, 43000, 57600, 76800,
        115200, 128000, 230400, 256000, 460800,
        921600, 1382400
    };
    
    if(argc != 2)
    {
        shell_print_error(E_SHELL_ERR_ARGCOUNT, "example: baud 115200");
        return SHELL_RET_FAILURE;
    }
    
    uint32_t baud = String(argv[1]).toInt();
    
    for(int i = 0; i < __Sizeof(baudrateGrp); i++)
    {
        if(baud == baudrateGrp[i])
        {
            Serial.begin(baud);
            return SHELL_RET_SUCCESS;
        }
    }
    
    shell_print_error(E_SHELL_ERR_VALUE, "baudrate only support:");
    for(int i = 0; i < __Sizeof(baudrateGrp); i++)
    {
        shell_printf("%d\r\n", baudrateGrp[i]);
    }
    return SHELL_RET_FAILURE;
}

void ShellReg_HW()
{
    shell_register(HW_GPIO_Ctrl, "ioctrl");
    shell_register(HW_SysReboot, "reboot");
    shell_register(HW_SysShutdown, "shutdown");
    shell_register(HW_SetBaudrate, "baud");
}
