# 1 "mcc_generated_files/examples/i2c_master_example.c"
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 288 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 "C:/Program Files/Microchip/MPLABX/v6.00/packs/Microchip/PIC12-16F1xxx_DFP/1.3.90/xc8\\pic\\include\\language_support.h" 1 3
# 2 "<built-in>" 2
# 1 "mcc_generated_files/examples/i2c_master_example.c" 2
# 47 "mcc_generated_files/examples/i2c_master_example.c"
# 1 "mcc_generated_files/examples/i2c_master_example.h" 1
# 50 "mcc_generated_files/examples/i2c_master_example.h"
# 1 "C:\\Program Files\\Microchip\\xc8\\v2.36\\pic\\include\\c99\\stdint.h" 1 3



# 1 "C:\\Program Files\\Microchip\\xc8\\v2.36\\pic\\include\\c99\\musl_xc8.h" 1 3
# 4 "C:\\Program Files\\Microchip\\xc8\\v2.36\\pic\\include\\c99\\stdint.h" 2 3
# 22 "C:\\Program Files\\Microchip\\xc8\\v2.36\\pic\\include\\c99\\stdint.h" 3
# 1 "C:\\Program Files\\Microchip\\xc8\\v2.36\\pic\\include\\c99\\bits/alltypes.h" 1 3
# 127 "C:\\Program Files\\Microchip\\xc8\\v2.36\\pic\\include\\c99\\bits/alltypes.h" 3
typedef unsigned long uintptr_t;
# 142 "C:\\Program Files\\Microchip\\xc8\\v2.36\\pic\\include\\c99\\bits/alltypes.h" 3
typedef long intptr_t;
# 158 "C:\\Program Files\\Microchip\\xc8\\v2.36\\pic\\include\\c99\\bits/alltypes.h" 3
typedef signed char int8_t;




typedef short int16_t;




typedef __int24 int24_t;




typedef long int32_t;





typedef long long int64_t;
# 188 "C:\\Program Files\\Microchip\\xc8\\v2.36\\pic\\include\\c99\\bits/alltypes.h" 3
typedef long long intmax_t;





typedef unsigned char uint8_t;




typedef unsigned short uint16_t;




typedef __uint24 uint24_t;




typedef unsigned long uint32_t;





typedef unsigned long long uint64_t;
# 229 "C:\\Program Files\\Microchip\\xc8\\v2.36\\pic\\include\\c99\\bits/alltypes.h" 3
typedef unsigned long long uintmax_t;
# 22 "C:\\Program Files\\Microchip\\xc8\\v2.36\\pic\\include\\c99\\stdint.h" 2 3


typedef int8_t int_fast8_t;

typedef int64_t int_fast64_t;


typedef int8_t int_least8_t;
typedef int16_t int_least16_t;

typedef int24_t int_least24_t;
typedef int24_t int_fast24_t;

typedef int32_t int_least32_t;

typedef int64_t int_least64_t;


typedef uint8_t uint_fast8_t;

typedef uint64_t uint_fast64_t;


typedef uint8_t uint_least8_t;
typedef uint16_t uint_least16_t;

typedef uint24_t uint_least24_t;
typedef uint24_t uint_fast24_t;

typedef uint32_t uint_least32_t;

typedef uint64_t uint_least64_t;
# 144 "C:\\Program Files\\Microchip\\xc8\\v2.36\\pic\\include\\c99\\stdint.h" 3
# 1 "C:\\Program Files\\Microchip\\xc8\\v2.36\\pic\\include\\c99\\bits/stdint.h" 1 3
typedef int16_t int_fast16_t;
typedef int32_t int_fast32_t;
typedef uint16_t uint_fast16_t;
typedef uint32_t uint_fast32_t;
# 144 "C:\\Program Files\\Microchip\\xc8\\v2.36\\pic\\include\\c99\\stdint.h" 2 3
# 50 "mcc_generated_files/examples/i2c_master_example.h" 2

# 1 "C:\\Program Files\\Microchip\\xc8\\v2.36\\pic\\include\\c99\\stdio.h" 1 3
# 10 "C:\\Program Files\\Microchip\\xc8\\v2.36\\pic\\include\\c99\\stdio.h" 3
# 1 "C:\\Program Files\\Microchip\\xc8\\v2.36\\pic\\include\\c99\\features.h" 1 3
# 10 "C:\\Program Files\\Microchip\\xc8\\v2.36\\pic\\include\\c99\\stdio.h" 2 3
# 24 "C:\\Program Files\\Microchip\\xc8\\v2.36\\pic\\include\\c99\\stdio.h" 3
# 1 "C:\\Program Files\\Microchip\\xc8\\v2.36\\pic\\include\\c99\\bits/alltypes.h" 1 3





typedef void * va_list[1];




typedef void * __isoc_va_list[1];
# 122 "C:\\Program Files\\Microchip\\xc8\\v2.36\\pic\\include\\c99\\bits/alltypes.h" 3
typedef unsigned size_t;
# 137 "C:\\Program Files\\Microchip\\xc8\\v2.36\\pic\\include\\c99\\bits/alltypes.h" 3
typedef long ssize_t;
# 246 "C:\\Program Files\\Microchip\\xc8\\v2.36\\pic\\include\\c99\\bits/alltypes.h" 3
typedef long long off_t;
# 399 "C:\\Program Files\\Microchip\\xc8\\v2.36\\pic\\include\\c99\\bits/alltypes.h" 3
typedef struct _IO_FILE FILE;
# 24 "C:\\Program Files\\Microchip\\xc8\\v2.36\\pic\\include\\c99\\stdio.h" 2 3
# 52 "C:\\Program Files\\Microchip\\xc8\\v2.36\\pic\\include\\c99\\stdio.h" 3
typedef union _G_fpos64_t {
 char __opaque[16];
 double __align;
} fpos_t;

extern FILE *const stdin;
extern FILE *const stdout;
extern FILE *const stderr;





FILE *fopen(const char *restrict, const char *restrict);
FILE *freopen(const char *restrict, const char *restrict, FILE *restrict);
int fclose(FILE *);

int remove(const char *);
int rename(const char *, const char *);

int feof(FILE *);
int ferror(FILE *);
int fflush(FILE *);
void clearerr(FILE *);

int fseek(FILE *, long, int);
long ftell(FILE *);
void rewind(FILE *);

int fgetpos(FILE *restrict, fpos_t *restrict);
int fsetpos(FILE *, const fpos_t *);

size_t fread(void *restrict, size_t, size_t, FILE *restrict);
size_t fwrite(const void *restrict, size_t, size_t, FILE *restrict);

int fgetc(FILE *);
int getc(FILE *);
int getchar(void);
int ungetc(int, FILE *);

int fputc(int, FILE *);
int putc(int, FILE *);
int putchar(int);

char *fgets(char *restrict, int, FILE *restrict);

char *gets(char *);


int fputs(const char *restrict, FILE *restrict);
int puts(const char *);

__attribute__((__format__(__printf__, 1, 2)))
int printf(const char *restrict, ...);
__attribute__((__format__(__printf__, 2, 3)))
int fprintf(FILE *restrict, const char *restrict, ...);
__attribute__((__format__(__printf__, 2, 3)))
int sprintf(char *restrict, const char *restrict, ...);
__attribute__((__format__(__printf__, 3, 4)))
int snprintf(char *restrict, size_t, const char *restrict, ...);

__attribute__((__format__(__printf__, 1, 0)))
int vprintf(const char *restrict, __isoc_va_list);
int vfprintf(FILE *restrict, const char *restrict, __isoc_va_list);
__attribute__((__format__(__printf__, 2, 0)))
int vsprintf(char *restrict, const char *restrict, __isoc_va_list);
__attribute__((__format__(__printf__, 3, 0)))
int vsnprintf(char *restrict, size_t, const char *restrict, __isoc_va_list);

__attribute__((__format__(__scanf__, 1, 2)))
int scanf(const char *restrict, ...);
__attribute__((__format__(__scanf__, 2, 3)))
int fscanf(FILE *restrict, const char *restrict, ...);
__attribute__((__format__(__scanf__, 2, 3)))
int sscanf(const char *restrict, const char *restrict, ...);

__attribute__((__format__(__scanf__, 1, 0)))
int vscanf(const char *restrict, __isoc_va_list);
int vfscanf(FILE *restrict, const char *restrict, __isoc_va_list);
__attribute__((__format__(__scanf__, 2, 0)))
int vsscanf(const char *restrict, const char *restrict, __isoc_va_list);

void perror(const char *);

int setvbuf(FILE *restrict, char *restrict, int, size_t);
void setbuf(FILE *restrict, char *restrict);

char *tmpnam(char *);
FILE *tmpfile(void);




FILE *fmemopen(void *restrict, size_t, const char *restrict);
FILE *open_memstream(char **, size_t *);
FILE *fdopen(int, const char *);
FILE *popen(const char *, const char *);
int pclose(FILE *);
int fileno(FILE *);
int fseeko(FILE *, off_t, int);
off_t ftello(FILE *);
int dprintf(int, const char *restrict, ...);
int vdprintf(int, const char *restrict, __isoc_va_list);
void flockfile(FILE *);
int ftrylockfile(FILE *);
void funlockfile(FILE *);
int getc_unlocked(FILE *);
int getchar_unlocked(void);
int putc_unlocked(int, FILE *);
int putchar_unlocked(int);
ssize_t getdelim(char **restrict, size_t *restrict, int, FILE *restrict);
ssize_t getline(char **restrict, size_t *restrict, FILE *restrict);
int renameat(int, const char *, int, const char *);
char *ctermid(char *);







char *tempnam(const char *, const char *);
# 51 "mcc_generated_files/examples/i2c_master_example.h" 2

# 1 "mcc_generated_files/examples/../i2c_master.h" 1
# 56 "mcc_generated_files/examples/../i2c_master.h"
# 1 "C:\\Program Files\\Microchip\\xc8\\v2.36\\pic\\include\\c99\\stdbool.h" 1 3
# 56 "mcc_generated_files/examples/../i2c_master.h" 2


typedef enum {
    I2C_NOERR,
    I2C_BUSY,
    I2C_FAIL


} i2c_error_t;

typedef enum
{
    I2C_STOP=1,
    I2C_RESTART_READ,
    I2C_RESTART_WRITE,
    I2C_CONTINUE,
    I2C_RESET_LINK
} i2c_operations_t;

typedef uint8_t i2c_address_t;
typedef i2c_operations_t (*i2c_callback_t)(void *funPtr);


i2c_operations_t I2C_CallbackReturnStop(void *funPtr);
i2c_operations_t I2C_CallbackReturnReset(void *funPtr);
i2c_operations_t I2C_CallbackRestartWrite(void *funPtr);
i2c_operations_t I2C_CallbackRestartRead(void *funPtr);






void I2C_Initialize(void);
# 101 "mcc_generated_files/examples/../i2c_master.h"
i2c_error_t I2C_Open(i2c_address_t address);
# 111 "mcc_generated_files/examples/../i2c_master.h"
i2c_error_t I2C_Close(void);
# 123 "mcc_generated_files/examples/../i2c_master.h"
i2c_error_t I2C_MasterOperation(_Bool read);




i2c_error_t I2C_MasterWrite(void);




i2c_error_t I2C_MasterRead(void);
# 142 "mcc_generated_files/examples/../i2c_master.h"
void I2C_SetTimeout(uint8_t timeOut);
# 152 "mcc_generated_files/examples/../i2c_master.h"
void I2C_SetBuffer(void *buffer, size_t bufferSize);
# 164 "mcc_generated_files/examples/../i2c_master.h"
void I2C_SetDataCompleteCallback(i2c_callback_t cb, void *ptr);
# 174 "mcc_generated_files/examples/../i2c_master.h"
void I2C_SetWriteCollisionCallback(i2c_callback_t cb, void *ptr);
# 184 "mcc_generated_files/examples/../i2c_master.h"
void I2C_SetAddressNackCallback(i2c_callback_t cb, void *ptr);
# 194 "mcc_generated_files/examples/../i2c_master.h"
void I2C_SetDataNackCallback(i2c_callback_t cb, void *ptr);
# 204 "mcc_generated_files/examples/../i2c_master.h"
void I2C_SetTimeoutCallback(i2c_callback_t cb, void *ptr);
# 52 "mcc_generated_files/examples/i2c_master_example.h" 2


uint8_t I2C_Read1ByteRegister(i2c_address_t address, uint8_t reg);
uint16_t I2C_Read2ByteRegister(i2c_address_t address, uint8_t reg);
void I2C_Write1ByteRegister(i2c_address_t address, uint8_t reg, uint8_t data);
void I2C_Write2ByteRegister(i2c_address_t address, uint8_t reg, uint16_t data);
void I2C_WriteNBytes(i2c_address_t address, uint8_t *data, size_t len);
void I2C_ReadNBytes(i2c_address_t address, uint8_t *data, size_t len);
void I2C_ReadDataBlock(i2c_address_t address, uint8_t reg, uint8_t *data, size_t len);
# 47 "mcc_generated_files/examples/i2c_master_example.c" 2



typedef struct
{
    size_t len;
    uint8_t *data;
}i2c_buffer_t;

static i2c_operations_t rd1RegCompleteHandler(void *ptr);
static i2c_operations_t rd2RegCompleteHandler(void *ptr);
static i2c_operations_t wr1RegCompleteHandler(void *ptr);
static i2c_operations_t wr2RegCompleteHandler(void *ptr);
static i2c_operations_t rdBlkRegCompleteHandler(void *ptr);


uint8_t I2C_Read1ByteRegister(i2c_address_t address, uint8_t reg)
{
    uint8_t returnValue = 0x00;

    while(!I2C_Open(address));
    I2C_SetDataCompleteCallback(rd1RegCompleteHandler,&returnValue);
    I2C_SetBuffer(&reg,1);
    I2C_SetAddressNackCallback(((void*)0),((void*)0));
    I2C_MasterWrite();
    while(I2C_BUSY == I2C_Close());

    return returnValue;
}

uint16_t I2C_Read2ByteRegister(i2c_address_t address, uint8_t reg)
{
    uint16_t returnValue =0x00;

    while(!I2C_Open(address));
    I2C_SetDataCompleteCallback(rd2RegCompleteHandler,&returnValue);
    I2C_SetBuffer(&reg,1);
    I2C_SetAddressNackCallback(((void*)0),((void*)0));
    I2C_MasterWrite();
    while(I2C_BUSY == I2C_Close());

    return (returnValue << 8 | returnValue >> 8);
}

void I2C_Write1ByteRegister(i2c_address_t address, uint8_t reg, uint8_t data)
{
    while(!I2C_Open(address));
    I2C_SetDataCompleteCallback(wr1RegCompleteHandler,&data);
    I2C_SetBuffer(&reg,1);
    I2C_SetAddressNackCallback(((void*)0),((void*)0));
    I2C_MasterWrite();
    while(I2C_BUSY == I2C_Close());
}

void I2C_Write2ByteRegister(i2c_address_t address, uint8_t reg, uint16_t data)
{
    while(!I2C_Open(address));
    I2C_SetDataCompleteCallback(wr2RegCompleteHandler,&data);
    I2C_SetBuffer(&reg,1);
    I2C_SetAddressNackCallback(((void*)0),((void*)0));
    I2C_MasterWrite();
    while(I2C_BUSY == I2C_Close());
}

void I2C_WriteNBytes(i2c_address_t address, uint8_t* data, size_t len)
{
    while(!I2C_Open(address));
    I2C_SetBuffer(data,len);
    I2C_SetAddressNackCallback(((void*)0),((void*)0));
    I2C_MasterWrite();
    while(I2C_BUSY == I2C_Close());
}

void I2C_ReadNBytes(i2c_address_t address, uint8_t *data, size_t len)
{
    while(!I2C_Open(address));
    I2C_SetBuffer(data,len);
    I2C_MasterRead();
    while(I2C_BUSY == I2C_Close());
}

void I2C_ReadDataBlock(i2c_address_t address, uint8_t reg, uint8_t *data, size_t len)
{
    i2c_buffer_t bufferBlock;
    bufferBlock.data = data;
    bufferBlock.len = len;

    while(!I2C_Open(address));
    I2C_SetDataCompleteCallback(rdBlkRegCompleteHandler,&bufferBlock);
    I2C_SetBuffer(&reg,1);
    I2C_SetAddressNackCallback(((void*)0),((void*)0));
    I2C_MasterWrite();
    while(I2C_BUSY == I2C_Close());
}

static i2c_operations_t rd1RegCompleteHandler(void *ptr)
{
    I2C_SetBuffer(ptr,1);
    I2C_SetDataCompleteCallback(((void*)0),((void*)0));
    return I2C_RESTART_READ;
}

static i2c_operations_t rd2RegCompleteHandler(void *ptr)
{
    I2C_SetBuffer(ptr,2);
    I2C_SetDataCompleteCallback(((void*)0),((void*)0));
    return I2C_RESTART_READ;
}

static i2c_operations_t wr1RegCompleteHandler(void *ptr)
{
    I2C_SetBuffer(ptr,1);
    I2C_SetDataCompleteCallback(((void*)0),((void*)0));
    return I2C_CONTINUE;
}

static i2c_operations_t wr2RegCompleteHandler(void *ptr)
{
    I2C_SetBuffer(ptr,2);
    I2C_SetDataCompleteCallback(((void*)0),((void*)0));
    return I2C_CONTINUE;
}

static i2c_operations_t rdBlkRegCompleteHandler(void *ptr)
{
    I2C_SetBuffer(((i2c_buffer_t *)ptr)->data,((i2c_buffer_t*)ptr)->len);
    I2C_SetDataCompleteCallback(((void*)0),((void*)0));
    return I2C_RESTART_READ;
}
