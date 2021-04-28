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



case LD_A_xBC: AF_H=RdZ80(BC_W);break;
case LD_A_xDE: AF_H=RdZ80(DE_W);break;

case ADD_HL_BC:  M_ADDW(HL,BC);break;
case ADD_HL_DE:  M_ADDW(HL,DE);break;
case ADD_HL_HL:  M_ADDW(HL,HL);break;
case ADD_HL_SP:  M_ADDW(HL,SP);break;

case DEC_xHL:  T1_L=RdZ80(HL_W);M_DEC(T1_L);WrZ80(HL_W,T1_L);break;

case INC_xHL:  T1_L=RdZ80(HL_W);M_INC(T1_L);WrZ80(HL_W,T1_L);break;

case DJNZ: if(--BC_H) { I_PERIOD-=5;M_JR; } else PC_W++;break;
case SCF:  S(CF);R(NF|HF);break;
case CPL:  AF_H=~AF_H;S(NF|HF);break;
case OUTA: T1_L=RdZ80(PC_W++);OutZ80((AF_W & 0xff00) | (T1_L & 0x00ff), AF_H);break;
case INA:  T1_L=RdZ80(PC_W++);AF_H=InZ80((AF_W & 0xff00) | (T1_L & 0x00ff));break;

case CCF:
  AF_L^=CF;R(NF|HF);
  AF_L|=AF_L&CF? 0:HF;
  break;


case EX_DE_HL: WZ_W=DE_W;DE_W=HL_W;HL_W=WZ_W;break;
case EX_AF_AF: WZ_W=AF_W;AF_W=AF_P;AF_P=WZ_W;break;  
  
case LD_xBC_A: WrZ80(BC_W,AF_H);break;
case LD_xDE_A: WrZ80(DE_W,AF_H);break;

case LD_xHL_BYTE: WrZ80(HL_W,RdZ80(PC_W++));break;

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

case EX_HL_xSP:
  WZ_L=RdZ80(SP_W);WrZ80(SP_W++,HL_L);
  WZ_H=RdZ80(SP_W);WrZ80(SP_W--,HL_H);
  HL_W=WZ_W;
  break;
