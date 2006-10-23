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
  J.B.l=RdZ80(z80cpu->PC.W++);
  J.B.h=RdZ80(z80cpu->PC.W++);
  WrZ80(J.W++,z80cpu->HL.B.l);
  WrZ80(J.W,z80cpu->HL.B.h);
  break;
case LD_xWORDe_DE:
  J.B.l=RdZ80(z80cpu->PC.W++);
  J.B.h=RdZ80(z80cpu->PC.W++);
  WrZ80(J.W++,z80cpu->DE.B.l);
  WrZ80(J.W,z80cpu->DE.B.h);
  break;
case LD_xWORDe_BC:
  J.B.l=RdZ80(z80cpu->PC.W++);
  J.B.h=RdZ80(z80cpu->PC.W++);
  WrZ80(J.W++,z80cpu->BC.B.l);
  WrZ80(J.W,z80cpu->BC.B.h);
  break;
case LD_xWORDe_SP:
  J.B.l=RdZ80(z80cpu->PC.W++);
  J.B.h=RdZ80(z80cpu->PC.W++);
  WrZ80(J.W++,z80cpu->SP.B.l);
  WrZ80(J.W,z80cpu->SP.B.h);
  break;

case LD_HL_xWORDe:
  J.B.l=RdZ80(z80cpu->PC.W++);
  J.B.h=RdZ80(z80cpu->PC.W++);
  z80cpu->HL.B.l=RdZ80(J.W++);
  z80cpu->HL.B.h=RdZ80(J.W);
  break;
case LD_DE_xWORDe:
  J.B.l=RdZ80(z80cpu->PC.W++);
  J.B.h=RdZ80(z80cpu->PC.W++);
  z80cpu->DE.B.l=RdZ80(J.W++);
  z80cpu->DE.B.h=RdZ80(J.W);
  break;
case LD_BC_xWORDe:
  J.B.l=RdZ80(z80cpu->PC.W++);
  J.B.h=RdZ80(z80cpu->PC.W++);
  z80cpu->BC.B.l=RdZ80(J.W++);
  z80cpu->BC.B.h=RdZ80(J.W);
  break;
case LD_SP_xWORDe:
  J.B.l=RdZ80(z80cpu->PC.W++);
  J.B.h=RdZ80(z80cpu->PC.W++);
  z80cpu->SP.B.l=RdZ80(J.W++);
  z80cpu->SP.B.h=RdZ80(J.W);
  break;

case RRD:
  I=RdZ80(z80cpu->HL.W);
  J.B.l=(I>>4)|(z80cpu->AF.B.h<<4);
  WrZ80(z80cpu->HL.W,J.B.l);
  z80cpu->AF.B.h=(I&0x0F)|(z80cpu->AF.B.h&0xF0);
  z80cpu->AF.B.l=PZSTable[z80cpu->AF.B.h]|(z80cpu->AF.B.l&C_FLAG);
  break;
case RLD:
  I=RdZ80(z80cpu->HL.W);
  J.B.l=(I<<4)|(z80cpu->AF.B.h&0x0F);
  WrZ80(z80cpu->HL.W,J.B.l);
  z80cpu->AF.B.h=(I>>4)|(z80cpu->AF.B.h&0xF0);
  z80cpu->AF.B.l=PZSTable[z80cpu->AF.B.h]|(z80cpu->AF.B.l&C_FLAG);
  break;

case LD_A_I:
  z80cpu->AF.B.h=z80cpu->IR.B.h;
  z80cpu->AF.B.l=(z80cpu->AF.B.l&C_FLAG)|(z80cpu->IFF&IFF_2? P_FLAG:0)|ZSTable[z80cpu->AF.B.h];
  break;

case LD_A_R:
  z80cpu->AF.B.h=z80cpu->IR.B.l;
  z80cpu->AF.B.l=(z80cpu->AF.B.l&C_FLAG)|(z80cpu->IFF&IFF_2? P_FLAG:0)|ZSTable[z80cpu->AF.B.h];
  break;

case LD_I_A:   z80cpu->IR.B.h=z80cpu->AF.B.h;break;
case LD_R_A:   break;

case IM_0:     z80cpu->IFF&=~(IFF_IM1|IFF_IM2);break;
case IM_1:     z80cpu->IFF=(z80cpu->IFF&~IFF_IM2)|IFF_IM1;break;
case IM_2:     z80cpu->IFF=(z80cpu->IFF&~IFF_IM1)|IFF_IM2;break;

case RETI:     M_RET;break;
case RETN:     if(z80cpu->IFF&IFF_2) z80cpu->IFF|=IFF_1; else z80cpu->IFF&=~IFF_1;
               M_RET;break;

case NEG:      I=z80cpu->AF.B.h;z80cpu->AF.B.h=0;M_SUB(I);break;

case IN_B_xC:  M_IN(z80cpu->BC.B.h);break;
case IN_C_xC:  M_IN(z80cpu->BC.B.l);break;
case IN_D_xC:  M_IN(z80cpu->DE.B.h);break;
case IN_E_xC:  M_IN(z80cpu->DE.B.l);break;
case IN_H_xC:  M_IN(z80cpu->HL.B.h);break;
case IN_L_xC:  M_IN(z80cpu->HL.B.l);break;
case IN_A_xC:  M_IN(z80cpu->AF.B.h);break;
case IN_F_xC:  M_IN(J.B.l);break;

case OUT_xC_B: OutZ80(z80cpu->BC.W,z80cpu->BC.B.h);break;
case OUT_xC_C: OutZ80(z80cpu->BC.W,z80cpu->BC.B.l);break;
case OUT_xC_D: OutZ80(z80cpu->BC.W,z80cpu->DE.B.h);break;
case OUT_xC_E: OutZ80(z80cpu->BC.W,z80cpu->DE.B.l);break;
case OUT_xC_H: OutZ80(z80cpu->BC.W,z80cpu->HL.B.h);break;
case OUT_xC_L: OutZ80(z80cpu->BC.W,z80cpu->HL.B.l);break;
case OUT_xC_A: OutZ80(z80cpu->BC.W,z80cpu->AF.B.h);break;

case INI:
  WrZ80(z80cpu->HL.W++,InZ80(z80cpu->BC.W));
  z80cpu->BC.B.h--;
  z80cpu->AF.B.l=N_FLAG|(z80cpu->BC.B.h? 0:Z_FLAG);
  break;

case INIR:
  do
  {
    WrZ80(z80cpu->HL.W++,InZ80(z80cpu->BC.W));
    z80cpu->BC.B.h--;z80cpu->TStates-=21;
  }
  while(z80cpu->BC.B.h&&(z80cpu->TStates>0));
  if(z80cpu->BC.B.h) { z80cpu->AF.B.l=N_FLAG;z80cpu->PC.W-=2; }
  else { z80cpu->AF.B.l=Z_FLAG|N_FLAG;z80cpu->TStates+=5; }
  break;

case IND:
  WrZ80(z80cpu->HL.W--,InZ80(z80cpu->BC.W));
  z80cpu->BC.B.h--;
  z80cpu->AF.B.l=N_FLAG|(z80cpu->BC.B.h? 0:Z_FLAG);
  break;

case INDR:
  do
  {
    WrZ80(z80cpu->HL.W--,InZ80(z80cpu->BC.W));
    z80cpu->BC.B.h--;z80cpu->TStates-=21;
  }
  while(z80cpu->BC.B.h&&(z80cpu->TStates>0));
  if(z80cpu->BC.B.h) { z80cpu->AF.B.l=N_FLAG;z80cpu->PC.W-=2; }
  else { z80cpu->AF.B.l=Z_FLAG|N_FLAG;z80cpu->TStates+=5; }
  break;

case OUTI:
  I=RdZ80(z80cpu->HL.W++);
  z80cpu->BC.B.h--;
  OutZ80(z80cpu->BC.W,I);
  z80cpu->AF.B.l=N_FLAG|(z80cpu->BC.B.h? 0:Z_FLAG)|(z80cpu->HL.B.l+I>255? (C_FLAG|H_FLAG):0);
  break;

case OTIR:
  do
  {
    I=RdZ80(z80cpu->HL.W++);
    z80cpu->BC.B.h--;
    OutZ80(z80cpu->BC.W,I);
    z80cpu->TStates-=21;
  }
  while(z80cpu->BC.B.h&&(z80cpu->TStates>0));
  if(z80cpu->BC.B.h)
  {
    z80cpu->AF.B.l=N_FLAG|(z80cpu->HL.B.l+I>255? (C_FLAG|H_FLAG):0);
    z80cpu->PC.W-=2;
  }
  else
  {
    z80cpu->AF.B.l=Z_FLAG|N_FLAG|(z80cpu->HL.B.l+I>255? (C_FLAG|H_FLAG):0);
    z80cpu->TStates+=5;
  }
  break;

case OUTD:
  I=RdZ80(z80cpu->HL.W--);
  z80cpu->BC.B.h--;
  OutZ80(z80cpu->BC.W,I);
  z80cpu->AF.B.l=N_FLAG|(z80cpu->BC.B.h? 0:Z_FLAG)|(z80cpu->HL.B.l+I>255? (C_FLAG|H_FLAG):0);
  break;

case OTDR:
  do
  {
    I=RdZ80(z80cpu->HL.W--);
    z80cpu->BC.B.h--;
    OutZ80(z80cpu->BC.W,I);
    z80cpu->TStates-=21;
  }
  while(z80cpu->BC.B.h&&(z80cpu->TStates>0));
  if(z80cpu->BC.B.h)
  {
    z80cpu->AF.B.l=N_FLAG|(z80cpu->HL.B.l+I>255? (C_FLAG|H_FLAG):0);
    z80cpu->PC.W-=2;
  }
  else
  {
    z80cpu->AF.B.l=Z_FLAG|N_FLAG|(z80cpu->HL.B.l+I>255? (C_FLAG|H_FLAG):0);
    z80cpu->TStates+=5;
  }
  break;

case LDI:
  WrZ80(z80cpu->DE.W++,RdZ80(z80cpu->HL.W++));
  z80cpu->BC.W--;
  z80cpu->AF.B.l=(z80cpu->AF.B.l&~(N_FLAG|H_FLAG|P_FLAG))|(z80cpu->BC.W? P_FLAG:0);
  break;

case LDIR:
  do
  {
    WrZ80(z80cpu->DE.W++,RdZ80(z80cpu->HL.W++));
    z80cpu->BC.W--;z80cpu->TStates-=21;
  }
  while(z80cpu->BC.W&&(z80cpu->TStates>0));
  z80cpu->AF.B.l&=~(N_FLAG|H_FLAG|P_FLAG);
  if(z80cpu->BC.W) { z80cpu->AF.B.l|=N_FLAG;z80cpu->PC.W-=2; }
  else z80cpu->TStates+=5;
  break;

case LDD:
  WrZ80(z80cpu->DE.W--,RdZ80(z80cpu->HL.W--));
  z80cpu->BC.W--;
  z80cpu->AF.B.l=(z80cpu->AF.B.l&~(N_FLAG|H_FLAG|P_FLAG))|(z80cpu->BC.W? P_FLAG:0);
  break;

case LDDR:
  do
  {
    WrZ80(z80cpu->DE.W--,RdZ80(z80cpu->HL.W--));
    z80cpu->BC.W--;z80cpu->TStates-=21;
  }
  while(z80cpu->BC.W&&(z80cpu->TStates>0));
  z80cpu->AF.B.l&=~(N_FLAG|H_FLAG|P_FLAG);
  if(z80cpu->BC.W) { z80cpu->AF.B.l|=N_FLAG;z80cpu->PC.W-=2; }
  else z80cpu->TStates+=5;
  break;

case CPI:
  I=RdZ80(z80cpu->HL.W++);
  J.B.l=z80cpu->AF.B.h-I;
  z80cpu->BC.W--;
  z80cpu->AF.B.l =
    N_FLAG|(z80cpu->AF.B.l&C_FLAG)|ZSTable[J.B.l]|
    ((z80cpu->AF.B.h^I^J.B.l)&H_FLAG)|(z80cpu->BC.W? P_FLAG:0);
  break;

case CPIR:
  do
  {
    I=RdZ80(z80cpu->HL.W++);
    J.B.l=z80cpu->AF.B.h-I;
    z80cpu->BC.W--;z80cpu->TStates-=21;
  }  
  while(z80cpu->BC.W&&J.B.l&&(z80cpu->TStates>0));
  z80cpu->AF.B.l =
    N_FLAG|(z80cpu->AF.B.l&C_FLAG)|ZSTable[J.B.l]|
    ((z80cpu->AF.B.h^I^J.B.l)&H_FLAG)|(z80cpu->BC.W? P_FLAG:0);
  if(z80cpu->BC.W&&J.B.l) z80cpu->PC.W-=2; else z80cpu->TStates+=5;
  break;  

case CPD:
  I=RdZ80(z80cpu->HL.W--);
  J.B.l=z80cpu->AF.B.h-I;
  z80cpu->BC.W--;
  z80cpu->AF.B.l =
    N_FLAG|(z80cpu->AF.B.l&C_FLAG)|ZSTable[J.B.l]|
    ((z80cpu->AF.B.h^I^J.B.l)&H_FLAG)|(z80cpu->BC.W? P_FLAG:0);
  break;

case CPDR:
  do
  {
    I=RdZ80(z80cpu->HL.W--);
    J.B.l=z80cpu->AF.B.h-I;
    z80cpu->BC.W--;z80cpu->TStates-=21;
  }
  while(z80cpu->BC.W&&J.B.l);
  z80cpu->AF.B.l =
    N_FLAG|(z80cpu->AF.B.l&C_FLAG)|ZSTable[J.B.l]|
    ((z80cpu->AF.B.h^I^J.B.l)&H_FLAG)|(z80cpu->BC.W? P_FLAG:0);
  if(z80cpu->BC.W&&J.B.l) z80cpu->PC.W-=2; else z80cpu->TStates+=5;
  break;
