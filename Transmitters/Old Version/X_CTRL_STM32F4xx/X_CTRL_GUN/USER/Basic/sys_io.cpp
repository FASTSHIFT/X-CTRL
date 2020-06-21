#include <ctype.h>
#include <rt_sys.h>
#include <stdint.h>
#include <stdlib.h>
#include "SdFat.h"
typedef SdFile file_t;

#pragma import(__use_no_semihosting) // 禁用半主机模式

enum {
    STDIN,
    STDOUT,
    STDERR,
    STDMAX
};
#define IS_STD(fh) ((fh) >= 0 && (fh) < STDMAX)

/*
 * These names are used during library initialization as the
 * file names opened for stdin, stdout, and stderr.
 * As we define _sys_open() to always return the same file handle,
 * these can be left as their default values.
 */
const char __stdin_name[] =  "__stdin";
const char __stdout_name[] =  "__stdout";
const char __stderr_name[] =  "__stderr";

FILEHANDLE _sys_open(const char *name, int openmode)
{
    if (name == __stdin_name)
        return STDIN;
    else if (name == __stdout_name)
        return STDOUT;
    else if (name == __stderr_name)
        return STDERR;
    
    uint8_t flags = O_RDONLY;
    if(openmode & OPEN_R) flags |= O_RDONLY;
    if(openmode & OPEN_W) flags |= O_WRONLY;
//    if(openmode & OPEN_A);
//    if(openmode & OPEN_B);
    if(openmode & OPEN_PLUS) flags |= O_ACCMODE;

    file_t *file_p = new file_t;
    if(file_p->open(name, flags))
    {
        file_p->seekSet(0);
        return (int)file_p;
    }
    else
    {
        delete file_p;
    }

    return -1;
}

int _sys_close(FILEHANDLE fh)
{
    if(IS_STD(fh))
        return 0;

    file_t *file_p = ((file_t*)fh);
    if(file_p->close())
    {
        delete file_p;
        return 0;
    }
    
    return -1;
}

int _sys_write(FILEHANDLE fh, const unsigned char *buf,
               unsigned len, int mode)
{
    if(IS_STD(fh))
        return 0;
    
    file_t *file_p = ((file_t*)fh); 
    int bw = file_p->write(buf, len);
    return (bw == -1) ? -1 : (int)len - bw;
}

int _sys_read(FILEHANDLE fh, unsigned char *buf,
              unsigned len, int mode)
{
    if(IS_STD(fh))
        return 0;
    
    file_t *file_p = ((file_t*)fh);
    int br = file_p->read(buf, len);
    return (br == -1) ? -1 : (int)len - br;
}

void _ttywrch(int ch)
{
}

int _sys_istty(FILEHANDLE fh)
{
    return IS_STD(fh);
}

int _sys_seek(FILEHANDLE fh, long pos)
{
    file_t *file_p = ((file_t*)fh);
    return file_p->seekSet(pos) == true ? 0 : -1;
}

long _sys_flen(FILEHANDLE fh)
{
    file_t *file_p = ((file_t*)fh);
    return file_p->fileSize();;
}

void _sys_exit(int status)
{
    /*main return*/
    while(1);
}

int _sys_tmpnam(char* name, int sig, unsigned maxlen)
{
    return 0;
}
