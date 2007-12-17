/** Z80: portable Z80 emulator *******************************/
/**                                                         **/
/**                         CodesED.h                       **/
/**                                                         **/
/** This file contains implementation for the ED table of   **/
/** Z80 commands. It is included from Z80.c.                **/
/**                                                         **/
/** Copyright (C) Marat Fayzullin 1994-2002                 **/
/**     You are not allowed to distribute this software     **/
/**     commercially. Please, notify me, if you make any    **/
/**     changes to this file.                               **/
/*************************************************************/

case ADC_HL_BC: M_ADCW(BC);break;
case ADC_HL_DE: M_ADCW(DE);break;
case ADC_HL_HL: M_ADCW(HL);break;
case ADC_HL_SP: M_ADCW(SP);break;

case SBC_HL_BC: M_SBCW(BC);break;
case SBC_HL_DE: M_SBCW(DE);break;
case SBC_HL_HL: M_SBCW(HL);break;
case SBC_HL_SP: M_SBCW(SP);break;

case LD_xWORDe_HL:
  WZ_L=RdZ80(PC_W++);
  WZ_H=RdZ80(PC_W++);
  WrZ80(WZ_W++,HL_L);
  WrZ80(WZ_W,HL_H);
  break;
case LD_xWORDe_DE:
  WZ_L=RdZ80(PC_W++);
  WZ_H=RdZ80(PC_W++);
  WrZ80(WZ_W++,DE_L);
  WrZ80(WZ_W,DE_H);
  break;
case LD_xWORDe_BC:
  WZ_L=RdZ80(PC_W++);
  WZ_H=RdZ80(PC_W++);
  WrZ80(WZ_W++,BC_L);
  WrZ80(WZ_W,BC_H);
  break;
case LD_xWORDe_SP:
  WZ_L=RdZ80(PC_W++);
  WZ_H=RdZ80(PC_W++);
  WrZ80(WZ_W++,SP_L);
  WrZ80(WZ_W,SP_H);
  break;

case LD_HL_xWORDe:
  WZ_L=RdZ80(PC_W++);
  WZ_H=RdZ80(PC_W++);
  HL_L=RdZ80(WZ_W++);
  HL_H=RdZ80(WZ_W);
  break;
case LD_DE_xWORDe:
  WZ_L=RdZ80(PC_W++);
  WZ_H=RdZ80(PC_W++);
  DE_L=RdZ80(WZ_W++);
  DE_H=RdZ80(WZ_W);
  break;
case LD_BC_xWORDe:
  WZ_L=RdZ80(PC_W++);
  WZ_H=RdZ80(PC_W++);
  BC_L=RdZ80(WZ_W++);
  BC_H=RdZ80(WZ_W);
  break;
case LD_SP_xWORDe:
  WZ_L=RdZ80(PC_W++);
  WZ_H=RdZ80(PC_W++);
  SP_L=RdZ80(WZ_W++);
  SP_H=RdZ80(WZ_W);
  break;

case RRD:
  I=RdZ80(HL_W);
  WZ_L=(I>>4)|(AF_H<<4);
  WrZ80(HL_W,WZ_L);
  AF_H=(I&0x0F)|(AF_H&0xF0);
  AF_L=PZSTable[AF_H]|(AF_L&C_FLAG);
  break;
case RLD:
  I=RdZ80(HL_W);
  WZ_L=(I<<4)|(AF_H&0x0F);
  WrZ80(HL_W,WZ_L);
  AF_H=(I>>4)|(AF_H&0xF0);
  AF_L=PZSTable[AF_H]|(AF_L&C_FLAG);
  break;

case LD_A_I:
  AF_H=IR_H;
  AF_L=(AF_L&C_FLAG)|(IF_W&IFF_2? P_FLAG:0)|ZSTable[AF_H];
  break;

case LD_A_R:
  AF_H=IR_L;
  AF_L=(AF_L&C_FLAG)|(IF_W&IFF_2? P_FLAG:0)|ZSTable[AF_H];
  break;

case LD_I_A:   IR_H=AF_H;break;
case LD_R_A:   break;

case IM_0:     IF_W&=~(IFF_IM1|IFF_IM2);break;
case IM_1:     IF_W=(IF_W&~IFF_IM2)|IFF_IM1;break;
case IM_2:     IF_W=(IF_W&~IFF_IM1)|IFF_IM2;break;

case RETI:     M_RET;break;
case RETN:     if(IF_W&IFF_2) IF_W|=IFF_1; else IF_W&=~IFF_1;
               M_RET;break;

case NEG:      I=AF_H;AF_H=0;M_SUB(I);break;

case IN_B_xC:  M_IN(BC_H);break;
case IN_C_xC:  M_IN(BC_L);break;
case IN_D_xC:  M_IN(DE_H);break;
case IN_E_xC:  M_IN(DE_L);break;
case IN_H_xC:  M_IN(HL_H);break;
case IN_L_xC:  M_IN(HL_L);break;
case IN_A_xC:  M_IN(AF_H);break;
case IN_F_xC:  M_IN(WZ_L);break;

case OUT_xC_B: OutZ80(BC_W,BC_H);break;
case OUT_xC_C: OutZ80(BC_W,BC_L);break;
case OUT_xC_D: OutZ80(BC_W,DE_H);break;
case OUT_xC_E: OutZ80(BC_W,DE_L);break;
case OUT_xC_H: OutZ80(BC_W,HL_H);break;
case OUT_xC_L: OutZ80(BC_W,HL_L);break;
case OUT_xC_A: OutZ80(BC_W,AF_H);break;

case INI:
  WrZ80(HL_W++,InZ80(BC_W));
  BC_H--;
  AF_L=N_FLAG|(BC_H? 0:Z_FLAG);
  break;

case INIR:
  do
  {
    WrZ80(HL_W++,InZ80(BC_W));
    BC_H--;CCOUNTER-=21;
  }
  while(BC_H&&(CCOUNTER>0));
  if(BC_H) { AF_L=N_FLAG;PC_W-=2; }
  else { AF_L=Z_FLAG|N_FLAG;CCOUNTER+=5; }
  break;

case IND:
  WrZ80(HL_W--,InZ80(BC_W));
  BC_H--;
  AF_L=N_FLAG|(BC_H? 0:Z_FLAG);
  break;

case INDR:
  do
  {
    WrZ80(HL_W--,InZ80(BC_W));
    BC_H--;CCOUNTER-=21;
  }
  while(BC_H&&(CCOUNTER>0));
  if(BC_H) { AF_L=N_FLAG;PC_W-=2; }
  else { AF_L=Z_FLAG|N_FLAG;CCOUNTER+=5; }
  break;

case OUTI:
  I=RdZ80(HL_W++);
  BC_H--;
  OutZ80(BC_W,I);
  AF_L=N_FLAG|(BC_H? 0:Z_FLAG)|(HL_L+I>255? (C_FLAG|H_FLAG):0);
  break;

case OTIR:
  do
  {
    I=RdZ80(HL_W++);
    BC_H--;
    OutZ80(BC_W,I);
    CCOUNTER-=21;
  }
  while(BC_H&&(CCOUNTER>0));
  if(BC_H)
  {
    AF_L=N_FLAG|(HL_L+I>255? (C_FLAG|H_FLAG):0);
    PC_W-=2;
  }
  else
  {
    AF_L=Z_FLAG|N_FLAG|(HL_L+I>255? (C_FLAG|H_FLAG):0);
    CCOUNTER+=5;
  }
  break;

case OUTD:
  I=RdZ80(HL_W--);
  BC_H--;
  OutZ80(BC_W,I);
  AF_L=N_FLAG|(BC_H? 0:Z_FLAG)|(HL_L+I>255? (C_FLAG|H_FLAG):0);
  break;

case OTDR:
  do
  {
    I=RdZ80(HL_W--);
    BC_H--;
    OutZ80(BC_W,I);
    CCOUNTER-=21;
  }
  while(BC_H&&(CCOUNTER>0));
  if(BC_H)
  {
    AF_L=N_FLAG|(HL_L+I>255? (C_FLAG|H_FLAG):0);
    PC_W-=2;
  }
  else
  {
    AF_L=Z_FLAG|N_FLAG|(HL_L+I>255? (C_FLAG|H_FLAG):0);
    CCOUNTER+=5;
  }
  break;

case LDI:
  WrZ80(DE_W++,RdZ80(HL_W++));
  BC_W--;
  AF_L=(AF_L&~(N_FLAG|H_FLAG|P_FLAG))|(BC_W? P_FLAG:0);
  break;

case LDIR:
  do
  {
    WrZ80(DE_W++,RdZ80(HL_W++));
    BC_W--;CCOUNTER-=21;
  }
  while(BC_W&&(CCOUNTER>0));
  AF_L&=~(N_FLAG|H_FLAG|P_FLAG);
  if(BC_W) { AF_L|=N_FLAG;PC_W-=2; }
  else CCOUNTER+=5;
  break;

case LDD:
  WrZ80(DE_W--,RdZ80(HL_W--));
  BC_W--;
  AF_L=(AF_L&~(N_FLAG|H_FLAG|P_FLAG))|(BC_W? P_FLAG:0);
  break;

case LDDR:
  do
  {
    WrZ80(DE_W--,RdZ80(HL_W--));
    BC_W--;CCOUNTER-=21;
  }
  while(BC_W&&(CCOUNTER>0));
  AF_L&=~(N_FLAG|H_FLAG|P_FLAG);
  if(BC_W) { AF_L|=N_FLAG;PC_W-=2; }
  else CCOUNTER+=5;
  break;

case CPI:
  I=RdZ80(HL_W++);
  WZ_L=AF_H-I;
  BC_W--;
  AF_L =
    N_FLAG|(AF_L&C_FLAG)|ZSTable[WZ_L]|
    ((AF_H^I^WZ_L)&H_FLAG)|(BC_W? P_FLAG:0);
  break;

case CPIR:
  do
  {
    I=RdZ80(HL_W++);
    WZ_L=AF_H-I;
    BC_W--;CCOUNTER-=21;
  }  
  while(BC_W&&WZ_L&&(CCOUNTER>0));
  AF_L =
    N_FLAG|(AF_L&C_FLAG)|ZSTable[WZ_L]|
    ((AF_H^I^WZ_L)&H_FLAG)|(BC_W? P_FLAG:0);
  if(BC_W&&WZ_L) PC_W-=2; else CCOUNTER+=5;
  break;  

case CPD:
  I=RdZ80(HL_W--);
  WZ_L=AF_H-I;
  BC_W--;
  AF_L =
    N_FLAG|(AF_L&C_FLAG)|ZSTable[WZ_L]|
    ((AF_H^I^WZ_L)&H_FLAG)|(BC_W? P_FLAG:0);
  break;

case CPDR:
  do
  {
    I=RdZ80(HL_W--);
    WZ_L=AF_H-I;
    BC_W--;CCOUNTER-=21;
  }
  while(BC_W&&WZ_L);
  AF_L =
    N_FLAG|(AF_L&C_FLAG)|ZSTable[WZ_L]|
    ((AF_H^I^WZ_L)&H_FLAG)|(BC_W? P_FLAG:0);
  if(BC_W&&WZ_L) PC_W-=2; else CCOUNTER+=5;
  break;
