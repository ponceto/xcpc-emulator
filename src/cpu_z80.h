/** Z80: portable Z80 emulator *******************************/
/**                                                         **/
/**                         cpu_z80.h                       **/
/**                                                         **/
/** This file contains declarations relevant to emulation   **/
/** of Z80 CPU.                                             **/
/**                                                         **/
/** Copyright (C) Marat Fayzullin 1994,1995,1996,1997       **/
/**     You are not allowed to distribute this software     **/
/**     commercially. Please, notify me, if you make any    **/   
/**     changes to this file.                               **/
/*************************************************************/
#ifndef __CPU_Z80_H__
#define __CPU_Z80_H__

#ifdef __cplusplus
extern "C" {
#endif

                               /* Compilation options:       */
/* #define LSB_FIRST */        /* Compile for low-endian CPU */
/* #define MSB_FIRST */        /* Compile for hi-endian CPU  */

                               /* LoopZ80() may return:      */
#define INT_IRQ     0x0038     /* Standard RST 38h interrupt */
#define INT_NMI     0x0066     /* Non-maskable interrupt     */
#define INT_NONE    0xFFFF     /* No interrupt required      */
#define INT_QUIT    0xFFFE     /* Exit the emulation         */

                               /* Bits in Z80 F register:    */
#define S_FLAG      0x80       /* 1: Result negative         */
#define Z_FLAG      0x40       /* 1: Result is zero          */
#define H_FLAG      0x10       /* 1: Halfcarry/Halfborrow    */
#define P_FLAG      0x04       /* 1: Result is even          */
#define V_FLAG      0x04       /* 1: Overflow occured        */
#define N_FLAG      0x02       /* 1: Subtraction occured     */
#define C_FLAG      0x01       /* 1: Carry/Borrow occured    */

/** Structured Datatypes *************************************/
/** NOTICE: #define LSB_FIRST for machines where least      **/
/**         signifcant byte goes first.                     **/
/*************************************************************/
typedef union
{
#ifdef LSB_FIRST
  struct { byte l,h; } B;
#else
  struct { byte h,l; } B;
#endif
  word W;
} pair;

typedef struct
{
  pair AF,BC,DE,HL,IX,IY,PC,SP;       /* Main registers      */
  pair AF1,BC1,DE1,HL1;               /* Shadow registers    */
  byte IFF,I;                         /* Interrupt registers */

  int IPeriod,ICount; /* Set IPeriod to number of CPU cycles */
                      /* between calls to LoopZ80()          */
  int IBackup;        /* Private, don't touch                */
  word IRequest;      /* Set to address of pending IRQ       */
  void *User;         /* Arbitrary user data (ID,RAM*,etc.)  */
  byte TrapBadOps;    /* Set to 1 to warn of illegal opcodes */
  unsigned long Trap; /* Set Trap to address to trace from   */
  byte Trace;         /* Set Trace=1 to start tracing        */
  byte Refresh;       /* UC Refresh register                 */
} Z80;

extern Z80 z80;

extern void z80_init(void);
extern void z80_reset(void);
extern void z80_exit(void);

/** z80_exec() ***********************************************/
/** This function will execute a single Z80 opcode. It will **/
/** then return next PC, and current register values in R.  **/
/*************************************************************/
extern word z80_exec(void);

/** z80_int() ************************************************/
/** This function will generate interrupt of given vector.  **/
/*************************************************************/
extern void z80_int(register word vector);

/** z80_run() ************************************************/
/** This function will run Z80 code until an LoopZ80() call **/
/** returns INT_QUIT. It will return the PC at which        **/
/** emulation stopped, and current register values in R.    **/
/*************************************************************/
extern word z80_run(void);

/** z80_read()/z80_write() ***********************************/
/** These functions are called when access to RAM occurs.   **/
/** They allow to control memory access.                    **/
/************************************ TO BE WRITTEN BY USER **/
extern byte z80_read(register word address);
extern void z80_write(register word address, register byte value);

/** z80_in()/z80_out() ***************************************/
/** Z80 emulation calls these functions to read/write from  **/
/** I/O ports. There can be 65536 I/O ports, but only first **/
/** 256 are usually used                                    **/
/************************************ TO BE WRITTEN BY USER **/
extern byte z80_in(register word port);
extern void z80_out(register word port, register byte value);

/** z80_periodic() *******************************************/
/** Z80 emulation calls this function periodically to check **/
/** if the system hardware requires any interrupts. This    **/
/** function must return an address of the interrupt vector **/
/** (0x0038, 0x0066, etc.) or INT_NONE for no interrupt.    **/
/** Return INT_QUIT to exit the emulation loop.             **/
/************************************ TO BE WRITTEN BY USER **/
extern word z80_periodic(void);

#ifdef __cplusplus
}
#endif

#endif
