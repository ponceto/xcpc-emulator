/** Z80: portable Z80 emulator *******************************/
/**                                                         **/
/**                          Codes.h                        **/
/**                                                         **/
/** This file contains implementation for the main table of **/
/** Z80 commands. It is included from Z80.c.                **/
/**                                                         **/
/** Copyright (C) Marat Fayzullin 1994-2002                 **/
/**     You are not allowed to distribute this software     **/
/**     commercially. Please, notify me, if you make any    **/
/**     changes to this file.                               **/
/*************************************************************/

case DJNZ: if(--BC_H) { I_PERIOD-=5;M_JR; } else PC_W++;break;
case SCF:  S(CF);R(NF|HF);break;
case CPL:  AF_H=~AF_H;S(NF|HF);break;
case OUTA: T1_L=RdZ80(PC_W++);OutZ80((AF_W & 0xff00) | (T1_L & 0x00ff), AF_H);break;
case INA:  T1_L=RdZ80(PC_W++);AF_H=InZ80((AF_W & 0xff00) | (T1_L & 0x00ff));break;

case CCF:
  AF_L^=CF;R(NF|HF);
  AF_L|=AF_L&CF? 0:HF;
  break;


case LD_xWORD_HL:
  WZ_L=RdZ80(PC_W++);
  WZ_H=RdZ80(PC_W++);
  WrZ80(WZ_W++,HL_L);
  WrZ80(WZ_W,HL_H);
  break;

case LD_HL_xWORD:
  WZ_L=RdZ80(PC_W++);
  WZ_H=RdZ80(PC_W++);
  HL_L=RdZ80(WZ_W++);
  HL_H=RdZ80(WZ_W);
  break;

case LD_A_xWORD:
  WZ_L=RdZ80(PC_W++);
  WZ_H=RdZ80(PC_W++); 
  AF_H=RdZ80(WZ_W);
  break;

case LD_xWORD_A:
  WZ_L=RdZ80(PC_W++);
  WZ_H=RdZ80(PC_W++);
  WrZ80(WZ_W,AF_H);
  break;
