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

case SCF:  S(CF);R(NF|HF);break;
case CPL:  AF_H=~AF_H;S(NF|HF);break;
case OUTA: T1_L=RdZ80(PC_W++);OutZ80((AF_W & 0xff00) | (T1_L & 0x00ff), AF_H);break;
case INA:  T1_L=RdZ80(PC_W++);AF_H=InZ80((AF_W & 0xff00) | (T1_L & 0x00ff));break;

case CCF:
  AF_L^=CF;R(NF|HF);
  AF_L|=AF_L&CF? 0:HF;
  break;

