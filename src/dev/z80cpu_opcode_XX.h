/** Z80: portable Z80 emulator *******************************/
/**                                                         **/
/**                         CodesXX.h                       **/
/**                                                         **/
/** This file contains implementation for FD/DD tables of   **/
/** Z80 commands. It is included from Z80.c.                **/
/**                                                         **/
/** Copyright (C) Marat Fayzullin 1994-2002                 **/
/**     You are not allowed to distribute this software     **/
/**     commercially. Please, notify me, if you make any    **/
/**     changes to this file.                               **/
/*************************************************************/

case JR_NZ:   if(AF_L&Z_FLAG) PC_W++; else { T_STATES-=5;M_JR; } break;
case JR_NC:   if(AF_L&C_FLAG) PC_W++; else { T_STATES-=5;M_JR; } break;
case JR_Z:    if(AF_L&Z_FLAG) { T_STATES-=5;M_JR; } else PC_W++; break;
case JR_C:    if(AF_L&C_FLAG) { T_STATES-=5;M_JR; } else PC_W++; break;

case JP_NZ:   if(AF_L&Z_FLAG) PC_W+=2; else { M_JP; } break;
case JP_NC:   if(AF_L&C_FLAG) PC_W+=2; else { M_JP; } break;
case JP_PO:   if(AF_L&P_FLAG) PC_W+=2; else { M_JP; } break;
case JP_P:    if(AF_L&S_FLAG) PC_W+=2; else { M_JP; } break;
case JP_Z:    if(AF_L&Z_FLAG) { M_JP; } else PC_W+=2; break;
case JP_C:    if(AF_L&C_FLAG) { M_JP; } else PC_W+=2; break;
case JP_PE:   if(AF_L&P_FLAG) { M_JP; } else PC_W+=2; break;
case JP_M:    if(AF_L&S_FLAG) { M_JP; } else PC_W+=2; break;

case RET_NZ:  if(!(AF_L&Z_FLAG)) { T_STATES-=6;M_RET; } break;
case RET_NC:  if(!(AF_L&C_FLAG)) { T_STATES-=6;M_RET; } break;
case RET_PO:  if(!(AF_L&P_FLAG)) { T_STATES-=6;M_RET; } break;
case RET_P:   if(!(AF_L&S_FLAG)) { T_STATES-=6;M_RET; } break;
case RET_Z:   if(AF_L&Z_FLAG)    { T_STATES-=6;M_RET; } break;
case RET_C:   if(AF_L&C_FLAG)    { T_STATES-=6;M_RET; } break;
case RET_PE:  if(AF_L&P_FLAG)    { T_STATES-=6;M_RET; } break;
case RET_M:   if(AF_L&S_FLAG)    { T_STATES-=6;M_RET; } break;

case CALL_NZ: if(AF_L&Z_FLAG) PC_W+=2; else { T_STATES-=7;M_CALL; } break;
case CALL_NC: if(AF_L&C_FLAG) PC_W+=2; else { T_STATES-=7;M_CALL; } break;
case CALL_PO: if(AF_L&P_FLAG) PC_W+=2; else { T_STATES-=7;M_CALL; } break;
case CALL_P:  if(AF_L&S_FLAG) PC_W+=2; else { T_STATES-=7;M_CALL; } break;
case CALL_Z:  if(AF_L&Z_FLAG) { T_STATES-=7;M_CALL; } else PC_W+=2; break;
case CALL_C:  if(AF_L&C_FLAG) { T_STATES-=7;M_CALL; } else PC_W+=2; break;
case CALL_PE: if(AF_L&P_FLAG) { T_STATES-=7;M_CALL; } else PC_W+=2; break;
case CALL_M:  if(AF_L&S_FLAG) { T_STATES-=7;M_CALL; } else PC_W+=2; break;

case ADD_B:    M_ADD(BC_H);break;
case ADD_C:    M_ADD(BC_L);break;
case ADD_D:    M_ADD(DE_H);break;
case ADD_E:    M_ADD(DE_L);break;
case ADD_H:    M_ADD(XX_H);break;
case ADD_L:    M_ADD(XX_L);break;
case ADD_A:    M_ADD(AF_H);break;
case ADD_xHL:  I=RdZ80(XX_W+(gint8)RdZ80(PC_W++));
               M_ADD(I);break;
case ADD_BYTE: I=RdZ80(PC_W++);M_ADD(I);break;

case SUB_B:    M_SUB(BC_H);break;
case SUB_C:    M_SUB(BC_L);break;
case SUB_D:    M_SUB(DE_H);break;
case SUB_E:    M_SUB(DE_L);break;
case SUB_H:    M_SUB(XX_H);break;
case SUB_L:    M_SUB(XX_L);break;
case SUB_A:    AF_H=0;AF_L=N_FLAG|Z_FLAG;break;
case SUB_xHL:  I=RdZ80(XX_W+(gint8)RdZ80(PC_W++));
               M_SUB(I);break;
case SUB_BYTE: I=RdZ80(PC_W++);M_SUB(I);break;

case AND_B:    M_AND(BC_H);break;
case AND_C:    M_AND(BC_L);break;
case AND_D:    M_AND(DE_H);break;
case AND_E:    M_AND(DE_L);break;
case AND_H:    M_AND(XX_H);break;
case AND_L:    M_AND(XX_L);break;
case AND_A:    M_AND(AF_H);break;
case AND_xHL:  I=RdZ80(XX_W+(gint8)RdZ80(PC_W++));
               M_AND(I);break;
case AND_BYTE: I=RdZ80(PC_W++);M_AND(I);break;

case OR_B:     M_IOR(BC_H);break;
case OR_C:     M_IOR(BC_L);break;
case OR_D:     M_IOR(DE_H);break;
case OR_E:     M_IOR(DE_L);break;
case OR_H:     M_IOR(XX_H);break;
case OR_L:     M_IOR(XX_L);break;
case OR_A:     M_IOR(AF_H);break;
case OR_xHL:   I=RdZ80(XX_W+(gint8)RdZ80(PC_W++));
               M_IOR(I);break;
case OR_BYTE:  I=RdZ80(PC_W++);M_IOR(I);break;

case ADC_B:    M_ADC(BC_H);break;
case ADC_C:    M_ADC(BC_L);break;
case ADC_D:    M_ADC(DE_H);break;
case ADC_E:    M_ADC(DE_L);break;
case ADC_H:    M_ADC(XX_H);break;
case ADC_L:    M_ADC(XX_L);break;
case ADC_A:    M_ADC(AF_H);break;
case ADC_xHL:  I=RdZ80(XX_W+(gint8)RdZ80(PC_W++));
               M_ADC(I);break;
case ADC_BYTE: I=RdZ80(PC_W++);M_ADC(I);break;

case SBC_B:    M_SBC(BC_H);break;
case SBC_C:    M_SBC(BC_L);break;
case SBC_D:    M_SBC(DE_H);break;
case SBC_E:    M_SBC(DE_L);break;
case SBC_H:    M_SBC(XX_H);break;
case SBC_L:    M_SBC(XX_L);break;
case SBC_A:    M_SBC(AF_H);break;
case SBC_xHL:  I=RdZ80(XX_W+(gint8)RdZ80(PC_W++));
               M_SBC(I);break;
case SBC_BYTE: I=RdZ80(PC_W++);M_SBC(I);break;

case XOR_B:    M_XOR(BC_H);break;
case XOR_C:    M_XOR(BC_L);break;
case XOR_D:    M_XOR(DE_H);break;
case XOR_E:    M_XOR(DE_L);break;
case XOR_H:    M_XOR(XX_H);break;
case XOR_L:    M_XOR(XX_L);break;
case XOR_A:    AF_H=0;AF_L=P_FLAG|Z_FLAG;break;
case XOR_xHL:  I=RdZ80(XX_W+(gint8)RdZ80(PC_W++));
               M_XOR(I);break;
case XOR_BYTE: I=RdZ80(PC_W++);M_XOR(I);break;

case CP_B:     M_CMP(BC_H);break;
case CP_C:     M_CMP(BC_L);break;
case CP_D:     M_CMP(DE_H);break;
case CP_E:     M_CMP(DE_L);break;
case CP_H:     M_CMP(XX_H);break;
case CP_L:     M_CMP(XX_L);break;
case CP_A:     AF_L=N_FLAG|Z_FLAG;break;
case CP_xHL:   I=RdZ80(XX_W+(gint8)RdZ80(PC_W++));
               M_CMP(I);break;
case CP_BYTE:  I=RdZ80(PC_W++);M_CMP(I);break;
               
case LD_BC_WORD: M_LDWORD(BC);break;
case LD_DE_WORD: M_LDWORD(DE);break;
case LD_HL_WORD: M_LDWORD(XX);break;
case LD_SP_WORD: M_LDWORD(SP);break;

case LD_PC_HL: PC_W=XX_W;break;
case LD_SP_HL: SP_W=XX_W;break;
case LD_A_xBC: AF_H=RdZ80(BC_W);break;
case LD_A_xDE: AF_H=RdZ80(DE_W);break;

case ADD_HL_BC:  M_ADDW(XX,BC);break;
case ADD_HL_DE:  M_ADDW(XX,DE);break;
case ADD_HL_HL:  M_ADDW(XX,XX);break;
case ADD_HL_SP:  M_ADDW(XX,SP);break;

case DEC_BC:   BC_W--;break;
case DEC_DE:   DE_W--;break;
case DEC_HL:   XX_W--;break;
case DEC_SP:   SP_W--;break;

case INC_BC:   BC_W++;break;
case INC_DE:   DE_W++;break;
case INC_HL:   XX_W++;break;
case INC_SP:   SP_W++;break;

case DEC_B:    M_DEC(BC_H);break;
case DEC_C:    M_DEC(BC_L);break;
case DEC_D:    M_DEC(DE_H);break;
case DEC_E:    M_DEC(DE_L);break;
case DEC_H:    M_DEC(XX_H);break;
case DEC_L:    M_DEC(XX_L);break;
case DEC_A:    M_DEC(AF_H);break;
case DEC_xHL:  I=RdZ80(XX_W+(gint8)RdZ80(PC_W));M_DEC(I);
               WrZ80(XX_W+(gint8)RdZ80(PC_W++),I);
               break;

case INC_B:    M_INC(BC_H);break;
case INC_C:    M_INC(BC_L);break;
case INC_D:    M_INC(DE_H);break;
case INC_E:    M_INC(DE_L);break;
case INC_H:    M_INC(XX_H);break;
case INC_L:    M_INC(XX_L);break;
case INC_A:    M_INC(AF_H);break;
case INC_xHL:  I=RdZ80(XX_W+(gint8)RdZ80(PC_W));M_INC(I);
               WrZ80(XX_W+(gint8)RdZ80(PC_W++),I);
               break;

case RLCA:
  I=(AF_H&0x80? C_FLAG:0);
  AF_H=(AF_H<<1)|I;
  AF_L=(AF_L&~(C_FLAG|N_FLAG|H_FLAG))|I;
  break;
case RLA:
  I=(AF_H&0x80? C_FLAG:0);
  AF_H=(AF_H<<1)|(AF_L&C_FLAG);
  AF_L=(AF_L&~(C_FLAG|N_FLAG|H_FLAG))|I;
  break;
case RRCA:
  I=AF_H&0x01;
  AF_H=(AF_H>>1)|(I? 0x80:0);
  AF_L=(AF_L&~(C_FLAG|N_FLAG|H_FLAG))|I;
  break;
case RRA:
  I=AF_H&0x01;
  AF_H=(AF_H>>1)|(AF_L&C_FLAG? 0x80:0);
  AF_L=(AF_L&~(C_FLAG|N_FLAG|H_FLAG))|I;
  break;

case RST00:    M_RST(0x0000);break;
case RST08:    M_RST(0x0008);break;
case RST10:    M_RST(0x0010);break;
case RST18:    M_RST(0x0018);break;
case RST20:    M_RST(0x0020);break;
case RST28:    M_RST(0x0028);break;
case RST30:    M_RST(0x0030);break;
case RST38:    M_RST(0x0038);break;

case PUSH_BC:  M_PUSH(BC);break;
case PUSH_DE:  M_PUSH(DE);break;
case PUSH_HL:  M_PUSH(XX);break;
case PUSH_AF:  M_PUSH(AF);break;

case POP_BC:   M_POP(BC);break;
case POP_DE:   M_POP(DE);break;
case POP_HL:   M_POP(XX);break;
case POP_AF:   M_POP(AF);break;

case DJNZ: if(--BC_H) { T_STATES-=5;M_JR; } else PC_W++;break;
case JP:   M_JP;break;
case JR:   M_JR;break;
case CALL: M_CALL;break;
case RET:  M_RET;break;
case SCF:  S(C_FLAG);R(N_FLAG|H_FLAG);break;
case CPL:  AF_H=~AF_H;S(N_FLAG|H_FLAG);break;
case NOP:  break;
case OUTA: I=RdZ80(PC_W++);OutZ80((AF_W & 0xff00) | (I & 0x00ff), AF_H);break;
case INA:  I=RdZ80(PC_W++);AF_H=InZ80((AF_W & 0xff00) | (I & 0x00ff));break;

case HALT:
  PC_W--;
  IF_W|=IFF_HALT;
  break;

case DI:
  IF_W &= ~(IFF_1 | IFF_2 | IFF_3);
  break;

case EI:
  IF_W |=  (IFF_1 | IFF_2 | IFF_3);
  break;

case CCF:
  AF_L^=C_FLAG;R(N_FLAG|H_FLAG);
  AF_L|=AF_L&C_FLAG? 0:H_FLAG;
  break;

case EXX:
  WZ_W=BC_W;BC_W=BC_P;BC_P=WZ_W;
  WZ_W=DE_W;DE_W=DE_P;DE_P=WZ_W;
  WZ_W=HL_W;HL_W=HL_P;HL_P=WZ_W;
  break;

case EX_DE_HL: WZ_W=DE_W;DE_W=HL_W;HL_W=WZ_W;break;
case EX_AF_AF: WZ_W=AF_W;AF_W=AF_P;AF_P=WZ_W;break;  
  
case LD_B_B:   BC_H=BC_H;break;
case LD_C_B:   BC_L=BC_H;break;
case LD_D_B:   DE_H=BC_H;break;
case LD_E_B:   DE_L=BC_H;break;
case LD_H_B:   XX_H=BC_H;break;
case LD_L_B:   XX_L=BC_H;break;
case LD_A_B:   AF_H=BC_H;break;
case LD_xHL_B: WZ_W=XX_W+(gint8)RdZ80(PC_W++);
               WrZ80(WZ_W,BC_H);break;

case LD_B_C:   BC_H=BC_L;break;
case LD_C_C:   BC_L=BC_L;break;
case LD_D_C:   DE_H=BC_L;break;
case LD_E_C:   DE_L=BC_L;break;
case LD_H_C:   XX_H=BC_L;break;
case LD_L_C:   XX_L=BC_L;break;
case LD_A_C:   AF_H=BC_L;break;
case LD_xHL_C: WZ_W=XX_W+(gint8)RdZ80(PC_W++);
               WrZ80(WZ_W,BC_L);break;

case LD_B_D:   BC_H=DE_H;break;
case LD_C_D:   BC_L=DE_H;break;
case LD_D_D:   DE_H=DE_H;break;
case LD_E_D:   DE_L=DE_H;break;
case LD_H_D:   XX_H=DE_H;break;
case LD_L_D:   XX_L=DE_H;break;
case LD_A_D:   AF_H=DE_H;break;
case LD_xHL_D: WZ_W=XX_W+(gint8)RdZ80(PC_W++);
               WrZ80(WZ_W,DE_H);break;

case LD_B_E:   BC_H=DE_L;break;
case LD_C_E:   BC_L=DE_L;break;
case LD_D_E:   DE_H=DE_L;break;
case LD_E_E:   DE_L=DE_L;break;
case LD_H_E:   XX_H=DE_L;break;
case LD_L_E:   XX_L=DE_L;break;
case LD_A_E:   AF_H=DE_L;break;
case LD_xHL_E: WZ_W=XX_W+(gint8)RdZ80(PC_W++);
               WrZ80(WZ_W,DE_L);break;

case LD_B_H:   BC_H=XX_H;break;
case LD_C_H:   BC_L=XX_H;break;
case LD_D_H:   DE_H=XX_H;break;
case LD_E_H:   DE_L=XX_H;break;
case LD_H_H:   XX_H=XX_H;break;
case LD_L_H:   XX_L=XX_H;break;
case LD_A_H:   AF_H=XX_H;break;
case LD_xHL_H: WZ_W=XX_W+(gint8)RdZ80(PC_W++);
               WrZ80(WZ_W,HL_H);break;

case LD_B_L:   BC_H=XX_L;break;
case LD_C_L:   BC_L=XX_L;break;
case LD_D_L:   DE_H=XX_L;break;
case LD_E_L:   DE_L=XX_L;break;
case LD_H_L:   XX_H=XX_L;break;
case LD_L_L:   XX_L=XX_L;break;
case LD_A_L:   AF_H=XX_L;break;
case LD_xHL_L: WZ_W=XX_W+(gint8)RdZ80(PC_W++);
               WrZ80(WZ_W,HL_L);break;

case LD_B_A:   BC_H=AF_H;break;
case LD_C_A:   BC_L=AF_H;break;
case LD_D_A:   DE_H=AF_H;break;
case LD_E_A:   DE_L=AF_H;break;
case LD_H_A:   XX_H=AF_H;break;
case LD_L_A:   XX_L=AF_H;break;
case LD_A_A:   AF_H=AF_H;break;
case LD_xHL_A: WZ_W=XX_W+(gint8)RdZ80(PC_W++);
               WrZ80(WZ_W,AF_H);break;

case LD_xBC_A: WrZ80(BC_W,AF_H);break;
case LD_xDE_A: WrZ80(DE_W,AF_H);break;

case LD_B_xHL:    BC_H=RdZ80(XX_W+(gint8)RdZ80(PC_W++));break;
case LD_C_xHL:    BC_L=RdZ80(XX_W+(gint8)RdZ80(PC_W++));break;
case LD_D_xHL:    DE_H=RdZ80(XX_W+(gint8)RdZ80(PC_W++));break;
case LD_E_xHL:    DE_L=RdZ80(XX_W+(gint8)RdZ80(PC_W++));break;
case LD_H_xHL:    HL_H=RdZ80(XX_W+(gint8)RdZ80(PC_W++));break;
case LD_L_xHL:    HL_L=RdZ80(XX_W+(gint8)RdZ80(PC_W++));break;
case LD_A_xHL:    AF_H=RdZ80(XX_W+(gint8)RdZ80(PC_W++));break;

case LD_B_BYTE:   BC_H=RdZ80(PC_W++);break;
case LD_C_BYTE:   BC_L=RdZ80(PC_W++);break;
case LD_D_BYTE:   DE_H=RdZ80(PC_W++);break;
case LD_E_BYTE:   DE_L=RdZ80(PC_W++);break;
case LD_H_BYTE:   XX_H=RdZ80(PC_W++);break;
case LD_L_BYTE:   XX_L=RdZ80(PC_W++);break;
case LD_A_BYTE:   AF_H=RdZ80(PC_W++);break;
case LD_xHL_BYTE: WZ_W=XX_W+(gint8)RdZ80(PC_W++);
                  WrZ80(WZ_W,RdZ80(PC_W++));break;

case LD_xWORD_HL:
  WZ_L=RdZ80(PC_W++);
  WZ_H=RdZ80(PC_W++);
  WrZ80(WZ_W++,XX_L);
  WrZ80(WZ_W,XX_H);
  break;

case LD_HL_xWORD:
  WZ_L=RdZ80(PC_W++);
  WZ_H=RdZ80(PC_W++);
  XX_L=RdZ80(WZ_W++);
  XX_H=RdZ80(WZ_W);
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
  WZ_L=RdZ80(SP_W);WrZ80(SP_W++,XX_L);
  WZ_H=RdZ80(SP_W);WrZ80(SP_W--,XX_H);
  XX_W=WZ_W;
  break;

case DAA:
  WZ_W=AF_H;
  if(AF_L&C_FLAG) WZ_W|=256;
  if(AF_L&H_FLAG) WZ_W|=512;
  if(AF_L&N_FLAG) WZ_W|=1024;
  AF_W=DAATable[WZ_W];
  break;
