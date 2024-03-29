/* system call prototype for PSP */

#ifndef _SYSCALL_H_INCLUDED
#define _SYSCALL_H_INCLUDED


typedef unsigned long u32;
typedef unsigned char u8;
// Salt deleted typedef unsigned long size_t;


/******************************************************************************/
/* sceDisplay */
void sceDisplayWaitVblankStart();
void sceDisplaySetMode(long,long,long);
void sceDisplaySetFrameBuf(char *topaddr,long linesize,long pixelsize,long);


/******************************************************************************/
/* sceCtrl */
/* Index for the two analog directions */ 
#define CTRL_ANALOG_X   0 
#define CTRL_ANALOG_Y   1 

/* Button bit masks */ 
#define CTRL_SQUARE      0x8000 
#define CTRL_TRIANGLE   0x1000 
#define CTRL_CIRCLE      0x2000 
#define CTRL_CROSS      0x4000 
#define CTRL_UP         0x0010 
#define CTRL_DOWN      0x0040 
#define CTRL_LEFT      0x0080 
#define CTRL_RIGHT      0x0020 
#define CTRL_START      0x0008 
#define CTRL_SELECT      0x0001 
#define CTRL_LTRIGGER   0x0100 
#define CTRL_RTRIGGER   0x0200 
#define		CTRL_HOLD	0x00020000

/* Returned control data */ 
typedef struct _ctrl_data 
{ 
   u32 frame; 
   u32 buttons; 
   u8  analog[4]; 
   u32 unused; 
} ctrl_data_t; 

/* Not 100% sure on this, init with 0 */ 
void sceCtrlInit(int unknown); 
/* Pass 1 to enable analogue mode */ 
void sceCtrlSetAnalogMode(int on); 
/* Read in the controller data. Unknown should be set to 1 */ 
void sceCtrlRead(ctrl_data_t* paddata, int unknown); 


/******************************************************************************/
/* IoFileMgrForUser */

#define O_RDONLY    0x0001 
#define O_WRONLY    0x0002 
#define O_RDWR      0x0003 
#define O_NBLOCK    0x0010 
#define O_APPEND    0x0100 
#define O_CREAT     0x0200 
#define O_TRUNC     0x0400 
#define O_NOWAIT    0x8000 

//int sceIoOpen(const char* file, int mode); 
int sceIoOpen(const char* file, int mode, int nPermission); 
void sceIoClose(int fd); 
int sceIoRead(int fd, void *data, int size); 
int sceIoWrite(int fd, void *data, int size); 
int sceIoLseek(int fd, long long offset, int whence); 
int sceIoRemove(const char *file); 
int sceIoMkdir(const char *dir, int mode); 
int sceIoRmdir(const char *dir); 
int sceIoRename(const char *oldname, const char *newname); 


enum { 
    TYPE_DIR=0x10, 
    TYPE_FILE=0x20 
}; 

struct dirent { 
    u32 unk0; 
    u32 type; 
    u32 size; 
    u32 unk[19]; 
    char name[0x108]; 
}; 

int sceIoDopen(const char *fn); 
int sceIoDread(int fd, struct dirent *de); 
void sceIoDclose(int fd); 


void sceAudio_0();//
void sceAudio_1();//
long sceAudio_2(long, long, long, void *);//
long sceAudio_3(long, long samplecount, long);//init buffer? returns handle, minus if error
void sceAudio_4(long handle);//free buffer?
void sceAudio_5();//
long sceAudio_6(long, long);//
void sceAudio_7();//
void sceAudio_8();//


void sceKernelExitGame();


typedef int (*pg_threadfunc_t)(int args, void *argp);


int sceKernelCreateThread(const char *name, pg_threadfunc_t func, unsigned long priority, unsigned long stacksize, unsigned long attrib, void *unk);
int sceKernelStartThread(int hthread, int arg0, void *arg1);
void sceKernelExitThread(int ret);
int sceKernelWaitThreadEnd(int hthread, void *unk);
int sceKernelDeleteThread(int hthread);



#endif // _SYSCALL_H_INCLUDED
