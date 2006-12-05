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

/*
 * XXX
 */

case JR_NZ:   if(z80cpu->AF.B.l&Z_FLAG) z80cpu->PC.W++; else { z80cpu->t_states-=5;M_JR; } break;
case JR_NC:   if(z80cpu->AF.B.l&C_FLAG) z80cpu->PC.W++; else { z80cpu->t_states-=5;M_JR; } break;
case JR_Z:    if(z80cpu->AF.B.l&Z_FLAG) { z80cpu->t_states-=5;M_JR; } else z80cpu->PC.W++; break;
case JR_C:    if(z80cpu->AF.B.l&C_FLAG) { z80cpu->t_states-=5;M_JR; } else z80cpu->PC.W++; break;

case JP_NZ:   if(z80cpu->AF.B.l&Z_FLAG) z80cpu->PC.W+=2; else { M_JP; } break;
case JP_NC:   if(z80cpu->AF.B.l&C_FLAG) z80cpu->PC.W+=2; else { M_JP; } break;
case JP_PO:   if(z80cpu->AF.B.l&P_FLAG) z80cpu->PC.W+=2; else { M_JP; } break;
case JP_P:    if(z80cpu->AF.B.l&S_FLAG) z80cpu->PC.W+=2; else { M_JP; } break;
case JP_Z:    if(z80cpu->AF.B.l&Z_FLAG) { M_JP; } else z80cpu->PC.W+=2; break;
case JP_C:    if(z80cpu->AF.B.l&C_FLAG) { M_JP; } else z80cpu->PC.W+=2; break;
case JP_PE:   if(z80cpu->AF.B.l&P_FLAG) { M_JP; } else z80cpu->PC.W+=2; break;
case JP_M:    if(z80cpu->AF.B.l&S_FLAG) { M_JP; } else z80cpu->PC.W+=2; break;

case RET_NZ:  if(!(z80cpu->AF.B.l&Z_FLAG)) { z80cpu->t_states-=6;M_RET; } break;
case RET_NC:  if(!(z80cpu->AF.B.l&C_FLAG)) { z80cpu->t_states-=6;M_RET; } break;
case RET_PO:  if(!(z80cpu->AF.B.l&P_FLAG)) { z80cpu->t_states-=6;M_RET; } break;
case RET_P:   if(!(z80cpu->AF.B.l&S_FLAG)) { z80cpu->t_states-=6;M_RET; } break;
case RET_Z:   if(z80cpu->AF.B.l&Z_FLAG)    { z80cpu->t_states-=6;M_RET; } break;
case RET_C:   if(z80cpu->AF.B.l&C_FLAG)    { z80cpu->t_states-=6;M_RET; } break;
case RET_PE:  if(z80cpu->AF.B.l&P_FLAG)    { z80cpu->t_states-=6;M_RET; } break;
case RET_M:   if(z80cpu->AF.B.l&S_FLAG)    { z80cpu->t_states-=6;M_RET; } break;

case CALL_NZ: if(z80cpu->AF.B.l&Z_FLAG) z80cpu->PC.W+=2; else { z80cpu->t_states-=7;M_CALL; } break;
case CALL_NC: if(z80cpu->AF.B.l&C_FLAG) z80cpu->PC.W+=2; else { z80cpu->t_states-=7;M_CALL; } break;
case CALL_PO: if(z80cpu->AF.B.l&P_FLAG) z80cpu->PC.W+=2; else { z80cpu->t_states-=7;M_CALL; } break;
case CALL_P:  if(z80cpu->AF.B.l&S_FLAG) z80cpu->PC.W+=2; else { z80cpu->t_states-=7;M_CALL; } break;
case CALL_Z:  if(z80cpu->AF.B.l&Z_FLAG) { z80cpu->t_states-=7;M_CALL; } else z80cpu->PC.W+=2; break;
case CALL_C:  if(z80cpu->AF.B.l&C_FLAG) { z80cpu->t_states-=7;M_CALL; } else z80cpu->PC.W+=2; break;
case CALL_PE: if(z80cpu->AF.B.l&P_FLAG) { z80cpu->t_states-=7;M_CALL; } else z80cpu->PC.W+=2; break;
case CALL_M:  if(z80cpu->AF.B.l&S_FLAG) { z80cpu->t_states-=7;M_CALL; } else z80cpu->PC.W+=2; break;

case ADD_BYTE: I=RdZ80(z80cpu->PC.W++);M_ADD(I);break;
case SUB_BYTE: I=RdZ80(z80cpu->PC.W++);M_SUB(I);break;
case AND_BYTE: I=RdZ80(z80cpu->PC.W++);M_AND(I);break;
case OR_BYTE:  I=RdZ80(z80cpu->PC.W++);M_IOR(I);break;
case ADC_BYTE: I=RdZ80(z80cpu->PC.W++);M_ADC(I);break;
case SBC_BYTE: I=RdZ80(z80cpu->PC.W++);M_SBC(I);break;
case XOR_BYTE: I=RdZ80(z80cpu->PC.W++);M_XOR(I);break;
case CP_BYTE:  I=RdZ80(z80cpu->PC.W++);M_CMP(I);break;

case LD_BC_WORD: M_LDWORD(BC);break;
case LD_DE_WORD: M_LDWORD(DE);break;
case LD_HL_WORD: M_LDWORD(HL);break;
case LD_SP_WORD: M_LDWORD(SP);break;

case LD_PC_HL: z80cpu->PC.W=z80cpu->HL.W;break;
case LD_SP_HL: z80cpu->SP.W=z80cpu->HL.W;break;
case LD_A_xBC: z80cpu->AF.B.h=RdZ80(z80cpu->BC.W);break;
case LD_A_xDE: z80cpu->AF.B.h=RdZ80(z80cpu->DE.W);break;

case ADD_HL_BC:  M_ADDW(HL,BC);break;
case ADD_HL_DE:  M_ADDW(HL,DE);break;
case ADD_HL_HL:  M_ADDW(HL,HL);break;
case ADD_HL_SP:  M_ADDW(HL,SP);break;

case DEC_BC:   z80cpu->BC.W--;break;
case DEC_DE:   z80cpu->DE.W--;break;
case DEC_HL:   z80cpu->HL.W--;break;
case DEC_SP:   z80cpu->SP.W--;break;

case INC_BC:   z80cpu->BC.W++;break;
case INC_DE:   z80cpu->DE.W++;break;
case INC_HL:   z80cpu->HL.W++;break;
case INC_SP:   z80cpu->SP.W++;break;

case DEC_B:    M_DEC(z80cpu->BC.B.h);break;
case DEC_C:    M_DEC(z80cpu->BC.B.l);break;
case DEC_D:    M_DEC(z80cpu->DE.B.h);break;
case DEC_E:    M_DEC(z80cpu->DE.B.l);break;
case DEC_H:    M_DEC(z80cpu->HL.B.h);break;
case DEC_L:    M_DEC(z80cpu->HL.B.l);break;
case DEC_A:    M_DEC(z80cpu->AF.B.h);break;
case DEC_xHL:  I=RdZ80(z80cpu->HL.W);M_DEC(I);WrZ80(z80cpu->HL.W,I);break;

case INC_B:    M_INC(z80cpu->BC.B.h);break;
case INC_C:    M_INC(z80cpu->BC.B.l);break;
case INC_D:    M_INC(z80cpu->DE.B.h);break;
case INC_E:    M_INC(z80cpu->DE.B.l);break;
case INC_H:    M_INC(z80cpu->HL.B.h);break;
case INC_L:    M_INC(z80cpu->HL.B.l);break;
case INC_A:    M_INC(z80cpu->AF.B.h);break;
case INC_xHL:  I=RdZ80(z80cpu->HL.W);M_INC(I);WrZ80(z80cpu->HL.W,I);break;

case RLCA:
  I=z80cpu->AF.B.h&0x80? C_FLAG:0;
  z80cpu->AF.B.h=(z80cpu->AF.B.h<<1)|I;
  z80cpu->AF.B.l=(z80cpu->AF.B.l&~(C_FLAG|N_FLAG|H_FLAG))|I;
  break;
case RLA:
  I=z80cpu->AF.B.h&0x80? C_FLAG:0;
  z80cpu->AF.B.h=(z80cpu->AF.B.h<<1)|(z80cpu->AF.B.l&C_FLAG);
  z80cpu->AF.B.l=(z80cpu->AF.B.l&~(C_FLAG|N_FLAG|H_FLAG))|I;
  break;
case RRCA:
  I=z80cpu->AF.B.h&0x01;
  z80cpu->AF.B.h=(z80cpu->AF.B.h>>1)|(I? 0x80:0);
  z80cpu->AF.B.l=(z80cpu->AF.B.l&~(C_FLAG|N_FLAG|H_FLAG))|I; 
  break;
case RRA:
  I=z80cpu->AF.B.h&0x01;
  z80cpu->AF.B.h=(z80cpu->AF.B.h>>1)|(z80cpu->AF.B.l&C_FLAG? 0x80:0);
  z80cpu->AF.B.l=(z80cpu->AF.B.l&~(C_FLAG|N_FLAG|H_FLAG))|I;
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
case PUSH_HL:  M_PUSH(HL);break;
case PUSH_AF:  M_PUSH(AF);break;

case POP_BC:   M_POP(BC);break;
case POP_DE:   M_POP(DE);break;
case POP_HL:   M_POP(HL);break;
case POP_AF:   M_POP(AF);break;

case DJNZ: if(--z80cpu->BC.B.h) { z80cpu->t_states-=5;M_JR; } else z80cpu->PC.W++;break;
case JP:   M_JP;break;
case JR:   M_JR;break;
case CALL: M_CALL;break;
case RET:  M_RET;break;
case SCF:  S(C_FLAG);R(N_FLAG|H_FLAG);break;
case CPL:  z80cpu->AF.B.h=~z80cpu->AF.B.h;S(N_FLAG|H_FLAG);break;
case OUTA: I=RdZ80(z80cpu->PC.W++);OutZ80((z80cpu->AF.W & 0xff00) | (I & 0x00ff), z80cpu->AF.B.h);break;
case INA:  I=RdZ80(z80cpu->PC.W++);z80cpu->AF.B.h=InZ80((z80cpu->AF.W & 0xff00) | (I & 0x00ff));break;

case DI:
  z80cpu->IFF &= ~(IFF_1 | IFF_2 | IFF_3);
  break;

case EI:
  z80cpu->IFF |=  (IFF_1 | IFF_2 | IFF_3);
  break;

case CCF:
  z80cpu->AF.B.l^=C_FLAG;R(N_FLAG|H_FLAG);
  z80cpu->AF.B.l|=z80cpu->AF.B.l&C_FLAG? 0:H_FLAG;
  break;

case EXX:
  J.W=z80cpu->BC.W;z80cpu->BC.W=z80cpu->BC1.W;z80cpu->BC1.W=J.W;
  J.W=z80cpu->DE.W;z80cpu->DE.W=z80cpu->DE1.W;z80cpu->DE1.W=J.W;
  J.W=z80cpu->HL.W;z80cpu->HL.W=z80cpu->HL1.W;z80cpu->HL1.W=J.W;
  break;

case EX_DE_HL: J.W=z80cpu->DE.W;z80cpu->DE.W=z80cpu->HL.W;z80cpu->HL.W=J.W;break;
case EX_AF_AF: J.W=z80cpu->AF.W;z80cpu->AF.W=z80cpu->AF1.W;z80cpu->AF1.W=J.W;break;  
  
case LD_xBC_A: WrZ80(z80cpu->BC.W,z80cpu->AF.B.h);break;
case LD_xDE_A: WrZ80(z80cpu->DE.W,z80cpu->AF.B.h);break;

case LD_B_BYTE:   z80cpu->BC.B.h=RdZ80(z80cpu->PC.W++);break;
case LD_C_BYTE:   z80cpu->BC.B.l=RdZ80(z80cpu->PC.W++);break;
case LD_D_BYTE:   z80cpu->DE.B.h=RdZ80(z80cpu->PC.W++);break;
case LD_E_BYTE:   z80cpu->DE.B.l=RdZ80(z80cpu->PC.W++);break;
case LD_H_BYTE:   z80cpu->HL.B.h=RdZ80(z80cpu->PC.W++);break;
case LD_L_BYTE:   z80cpu->HL.B.l=RdZ80(z80cpu->PC.W++);break;
case LD_A_BYTE:   z80cpu->AF.B.h=RdZ80(z80cpu->PC.W++);break;
case LD_xHL_BYTE: WrZ80(z80cpu->HL.W,RdZ80(z80cpu->PC.W++));break;

case LD_xWORD_HL:
  J.B.l=RdZ80(z80cpu->PC.W++);
  J.B.h=RdZ80(z80cpu->PC.W++);
  WrZ80(J.W++,z80cpu->HL.B.l);
  WrZ80(J.W,z80cpu->HL.B.h);
  break;

case LD_HL_xWORD:
  J.B.l=RdZ80(z80cpu->PC.W++);
  J.B.h=RdZ80(z80cpu->PC.W++);
  z80cpu->HL.B.l=RdZ80(J.W++);
  z80cpu->HL.B.h=RdZ80(J.W);
  break;

case LD_A_xWORD:
  J.B.l=RdZ80(z80cpu->PC.W++);
  J.B.h=RdZ80(z80cpu->PC.W++); 
  z80cpu->AF.B.h=RdZ80(J.W);
  break;

case LD_xWORD_A:
  J.B.l=RdZ80(z80cpu->PC.W++);
  J.B.h=RdZ80(z80cpu->PC.W++);
  WrZ80(J.W,z80cpu->AF.B.h);
  break;

case EX_HL_xSP:
  J.B.l=RdZ80(z80cpu->SP.W);WrZ80(z80cpu->SP.W++,z80cpu->HL.B.l);
  J.B.h=RdZ80(z80cpu->SP.W);WrZ80(z80cpu->SP.W--,z80cpu->HL.B.h);
  z80cpu->HL.W=J.W;
  break;

case DAA:
  J.W=z80cpu->AF.B.h;
  if(z80cpu->AF.B.l&C_FLAG) J.W|=256;
  if(z80cpu->AF.B.l&H_FLAG) J.W|=512;
  if(z80cpu->AF.B.l&N_FLAG) J.W|=1024;
  z80cpu->AF.W=DAATable[J.W];
  break;
