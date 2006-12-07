/** Z80: portable Z80 emulator *******************************/
/**                                                         **/
/**                         CodesCB.h                       **/
/**                                                         **/
/** This file contains implementation for the CB table of   **/
/** Z80 commands. It is included from Z80.c.                **/
/**                                                         **/
/** Copyright (C) Marat Fayzullin 1994-2002                 **/
/**     You are not allowed to distribute this software     **/
/**     commercially. Please, notify me, if you make any    **/
/**     changes to this file.                               **/
/*************************************************************/

case RLC_B: M_RLC(BC_H);break;  case RLC_C: M_RLC(BC_L);break;
case RLC_D: M_RLC(DE_H);break;  case RLC_E: M_RLC(DE_L);break;
case RLC_H: M_RLC(HL_H);break;  case RLC_L: M_RLC(HL_L);break;
case RLC_xHL: I=RdZ80(HL_W);M_RLC(I);WrZ80(HL_W,I);break;
case RLC_A: M_RLC(AF_H);break;

case RRC_B: M_RRC(BC_H);break;  case RRC_C: M_RRC(BC_L);break;
case RRC_D: M_RRC(DE_H);break;  case RRC_E: M_RRC(DE_L);break;
case RRC_H: M_RRC(HL_H);break;  case RRC_L: M_RRC(HL_L);break;
case RRC_xHL: I=RdZ80(HL_W);M_RRC(I);WrZ80(HL_W,I);break;
case RRC_A: M_RRC(AF_H);break;

case RL_B: M_RL(BC_H);break;  case RL_C: M_RL(BC_L);break;
case RL_D: M_RL(DE_H);break;  case RL_E: M_RL(DE_L);break;
case RL_H: M_RL(HL_H);break;  case RL_L: M_RL(HL_L);break;
case RL_xHL: I=RdZ80(HL_W);M_RL(I);WrZ80(HL_W,I);break;
case RL_A: M_RL(AF_H);break;

case RR_B: M_RR(BC_H);break;  case RR_C: M_RR(BC_L);break;
case RR_D: M_RR(DE_H);break;  case RR_E: M_RR(DE_L);break;
case RR_H: M_RR(HL_H);break;  case RR_L: M_RR(HL_L);break;
case RR_xHL: I=RdZ80(HL_W);M_RR(I);WrZ80(HL_W,I);break;
case RR_A: M_RR(AF_H);break;

case SLA_B: M_SLA(BC_H);break;  case SLA_C: M_SLA(BC_L);break;
case SLA_D: M_SLA(DE_H);break;  case SLA_E: M_SLA(DE_L);break;
case SLA_H: M_SLA(HL_H);break;  case SLA_L: M_SLA(HL_L);break;
case SLA_xHL: I=RdZ80(HL_W);M_SLA(I);WrZ80(HL_W,I);break;
case SLA_A: M_SLA(AF_H);break;

case SRA_B: M_SRA(BC_H);break;  case SRA_C: M_SRA(BC_L);break;
case SRA_D: M_SRA(DE_H);break;  case SRA_E: M_SRA(DE_L);break;
case SRA_H: M_SRA(HL_H);break;  case SRA_L: M_SRA(HL_L);break;
case SRA_xHL: I=RdZ80(HL_W);M_SRA(I);WrZ80(HL_W,I);break;
case SRA_A: M_SRA(AF_H);break;

case SLL_B: M_SLL(BC_H);break;  case SLL_C: M_SLL(BC_L);break;
case SLL_D: M_SLL(DE_H);break;  case SLL_E: M_SLL(DE_L);break;
case SLL_H: M_SLL(HL_H);break;  case SLL_L: M_SLL(HL_L);break;
case SLL_xHL: I=RdZ80(HL_W);M_SLL(I);WrZ80(HL_W,I);break;
case SLL_A: M_SLL(AF_H);break;

case SRL_B: M_SRL(BC_H);break;  case SRL_C: M_SRL(BC_L);break;
case SRL_D: M_SRL(DE_H);break;  case SRL_E: M_SRL(DE_L);break;
case SRL_H: M_SRL(HL_H);break;  case SRL_L: M_SRL(HL_L);break;
case SRL_xHL: I=RdZ80(HL_W);M_SRL(I);WrZ80(HL_W,I);break;
case SRL_A: M_SRL(AF_H);break;
    
case BIT0_B: M_BIT(0,BC_H);break;  case BIT0_C: M_BIT(0,BC_L);break;
case BIT0_D: M_BIT(0,DE_H);break;  case BIT0_E: M_BIT(0,DE_L);break;
case BIT0_H: M_BIT(0,HL_H);break;  case BIT0_L: M_BIT(0,HL_L);break;
case BIT0_xHL: I=RdZ80(HL_W);M_BIT(0,I);break;
case BIT0_A: M_BIT(0,AF_H);break;

case BIT1_B: M_BIT(1,BC_H);break;  case BIT1_C: M_BIT(1,BC_L);break;
case BIT1_D: M_BIT(1,DE_H);break;  case BIT1_E: M_BIT(1,DE_L);break;
case BIT1_H: M_BIT(1,HL_H);break;  case BIT1_L: M_BIT(1,HL_L);break;
case BIT1_xHL: I=RdZ80(HL_W);M_BIT(1,I);break;
case BIT1_A: M_BIT(1,AF_H);break;

case BIT2_B: M_BIT(2,BC_H);break;  case BIT2_C: M_BIT(2,BC_L);break;
case BIT2_D: M_BIT(2,DE_H);break;  case BIT2_E: M_BIT(2,DE_L);break;
case BIT2_H: M_BIT(2,HL_H);break;  case BIT2_L: M_BIT(2,HL_L);break;
case BIT2_xHL: I=RdZ80(HL_W);M_BIT(2,I);break;
case BIT2_A: M_BIT(2,AF_H);break;

case BIT3_B: M_BIT(3,BC_H);break;  case BIT3_C: M_BIT(3,BC_L);break;
case BIT3_D: M_BIT(3,DE_H);break;  case BIT3_E: M_BIT(3,DE_L);break;
case BIT3_H: M_BIT(3,HL_H);break;  case BIT3_L: M_BIT(3,HL_L);break;
case BIT3_xHL: I=RdZ80(HL_W);M_BIT(3,I);break;
case BIT3_A: M_BIT(3,AF_H);break;

case BIT4_B: M_BIT(4,BC_H);break;  case BIT4_C: M_BIT(4,BC_L);break;
case BIT4_D: M_BIT(4,DE_H);break;  case BIT4_E: M_BIT(4,DE_L);break;
case BIT4_H: M_BIT(4,HL_H);break;  case BIT4_L: M_BIT(4,HL_L);break;
case BIT4_xHL: I=RdZ80(HL_W);M_BIT(4,I);break;
case BIT4_A: M_BIT(4,AF_H);break;

case BIT5_B: M_BIT(5,BC_H);break;  case BIT5_C: M_BIT(5,BC_L);break;
case BIT5_D: M_BIT(5,DE_H);break;  case BIT5_E: M_BIT(5,DE_L);break;
case BIT5_H: M_BIT(5,HL_H);break;  case BIT5_L: M_BIT(5,HL_L);break;
case BIT5_xHL: I=RdZ80(HL_W);M_BIT(5,I);break;
case BIT5_A: M_BIT(5,AF_H);break;

case BIT6_B: M_BIT(6,BC_H);break;  case BIT6_C: M_BIT(6,BC_L);break;
case BIT6_D: M_BIT(6,DE_H);break;  case BIT6_E: M_BIT(6,DE_L);break;
case BIT6_H: M_BIT(6,HL_H);break;  case BIT6_L: M_BIT(6,HL_L);break;
case BIT6_xHL: I=RdZ80(HL_W);M_BIT(6,I);break;
case BIT6_A: M_BIT(6,AF_H);break;

case BIT7_B: M_BIT(7,BC_H);break;  case BIT7_C: M_BIT(7,BC_L);break;
case BIT7_D: M_BIT(7,DE_H);break;  case BIT7_E: M_BIT(7,DE_L);break;
case BIT7_H: M_BIT(7,HL_H);break;  case BIT7_L: M_BIT(7,HL_L);break;
case BIT7_xHL: I=RdZ80(HL_W);M_BIT(7,I);break;
case BIT7_A: M_BIT(7,AF_H);break;

case RES0_B: M_RES(0,BC_H);break;  case RES0_C: M_RES(0,BC_L);break;
case RES0_D: M_RES(0,DE_H);break;  case RES0_E: M_RES(0,DE_L);break;
case RES0_H: M_RES(0,HL_H);break;  case RES0_L: M_RES(0,HL_L);break;
case RES0_xHL: I=RdZ80(HL_W);M_RES(0,I);WrZ80(HL_W,I);break;
case RES0_A: M_RES(0,AF_H);break;

case RES1_B: M_RES(1,BC_H);break;  case RES1_C: M_RES(1,BC_L);break;
case RES1_D: M_RES(1,DE_H);break;  case RES1_E: M_RES(1,DE_L);break;
case RES1_H: M_RES(1,HL_H);break;  case RES1_L: M_RES(1,HL_L);break;
case RES1_xHL: I=RdZ80(HL_W);M_RES(1,I);WrZ80(HL_W,I);break;
case RES1_A: M_RES(1,AF_H);break;

case RES2_B: M_RES(2,BC_H);break;  case RES2_C: M_RES(2,BC_L);break;
case RES2_D: M_RES(2,DE_H);break;  case RES2_E: M_RES(2,DE_L);break;
case RES2_H: M_RES(2,HL_H);break;  case RES2_L: M_RES(2,HL_L);break;
case RES2_xHL: I=RdZ80(HL_W);M_RES(2,I);WrZ80(HL_W,I);break;
case RES2_A: M_RES(2,AF_H);break;

case RES3_B: M_RES(3,BC_H);break;  case RES3_C: M_RES(3,BC_L);break;
case RES3_D: M_RES(3,DE_H);break;  case RES3_E: M_RES(3,DE_L);break;
case RES3_H: M_RES(3,HL_H);break;  case RES3_L: M_RES(3,HL_L);break;
case RES3_xHL: I=RdZ80(HL_W);M_RES(3,I);WrZ80(HL_W,I);break;
case RES3_A: M_RES(3,AF_H);break;

case RES4_B: M_RES(4,BC_H);break;  case RES4_C: M_RES(4,BC_L);break;
case RES4_D: M_RES(4,DE_H);break;  case RES4_E: M_RES(4,DE_L);break;
case RES4_H: M_RES(4,HL_H);break;  case RES4_L: M_RES(4,HL_L);break;
case RES4_xHL: I=RdZ80(HL_W);M_RES(4,I);WrZ80(HL_W,I);break;
case RES4_A: M_RES(4,AF_H);break;

case RES5_B: M_RES(5,BC_H);break;  case RES5_C: M_RES(5,BC_L);break;
case RES5_D: M_RES(5,DE_H);break;  case RES5_E: M_RES(5,DE_L);break;
case RES5_H: M_RES(5,HL_H);break;  case RES5_L: M_RES(5,HL_L);break;
case RES5_xHL: I=RdZ80(HL_W);M_RES(5,I);WrZ80(HL_W,I);break;
case RES5_A: M_RES(5,AF_H);break;

case RES6_B: M_RES(6,BC_H);break;  case RES6_C: M_RES(6,BC_L);break;
case RES6_D: M_RES(6,DE_H);break;  case RES6_E: M_RES(6,DE_L);break;
case RES6_H: M_RES(6,HL_H);break;  case RES6_L: M_RES(6,HL_L);break;
case RES6_xHL: I=RdZ80(HL_W);M_RES(6,I);WrZ80(HL_W,I);break;
case RES6_A: M_RES(6,AF_H);break;

case RES7_B: M_RES(7,BC_H);break;  case RES7_C: M_RES(7,BC_L);break;
case RES7_D: M_RES(7,DE_H);break;  case RES7_E: M_RES(7,DE_L);break;
case RES7_H: M_RES(7,HL_H);break;  case RES7_L: M_RES(7,HL_L);break;
case RES7_xHL: I=RdZ80(HL_W);M_RES(7,I);WrZ80(HL_W,I);break;
case RES7_A: M_RES(7,AF_H);break;

case SET0_B: M_SET(0,BC_H);break;  case SET0_C: M_SET(0,BC_L);break;
case SET0_D: M_SET(0,DE_H);break;  case SET0_E: M_SET(0,DE_L);break;
case SET0_H: M_SET(0,HL_H);break;  case SET0_L: M_SET(0,HL_L);break;
case SET0_xHL: I=RdZ80(HL_W);M_SET(0,I);WrZ80(HL_W,I);break;
case SET0_A: M_SET(0,AF_H);break;

case SET1_B: M_SET(1,BC_H);break;  case SET1_C: M_SET(1,BC_L);break;
case SET1_D: M_SET(1,DE_H);break;  case SET1_E: M_SET(1,DE_L);break;
case SET1_H: M_SET(1,HL_H);break;  case SET1_L: M_SET(1,HL_L);break;
case SET1_xHL: I=RdZ80(HL_W);M_SET(1,I);WrZ80(HL_W,I);break;
case SET1_A: M_SET(1,AF_H);break;

case SET2_B: M_SET(2,BC_H);break;  case SET2_C: M_SET(2,BC_L);break;
case SET2_D: M_SET(2,DE_H);break;  case SET2_E: M_SET(2,DE_L);break;
case SET2_H: M_SET(2,HL_H);break;  case SET2_L: M_SET(2,HL_L);break;
case SET2_xHL: I=RdZ80(HL_W);M_SET(2,I);WrZ80(HL_W,I);break;
case SET2_A: M_SET(2,AF_H);break;

case SET3_B: M_SET(3,BC_H);break;  case SET3_C: M_SET(3,BC_L);break;
case SET3_D: M_SET(3,DE_H);break;  case SET3_E: M_SET(3,DE_L);break;
case SET3_H: M_SET(3,HL_H);break;  case SET3_L: M_SET(3,HL_L);break;
case SET3_xHL: I=RdZ80(HL_W);M_SET(3,I);WrZ80(HL_W,I);break;
case SET3_A: M_SET(3,AF_H);break;

case SET4_B: M_SET(4,BC_H);break;  case SET4_C: M_SET(4,BC_L);break;
case SET4_D: M_SET(4,DE_H);break;  case SET4_E: M_SET(4,DE_L);break;
case SET4_H: M_SET(4,HL_H);break;  case SET4_L: M_SET(4,HL_L);break;
case SET4_xHL: I=RdZ80(HL_W);M_SET(4,I);WrZ80(HL_W,I);break;
case SET4_A: M_SET(4,AF_H);break;

case SET5_B: M_SET(5,BC_H);break;  case SET5_C: M_SET(5,BC_L);break;
case SET5_D: M_SET(5,DE_H);break;  case SET5_E: M_SET(5,DE_L);break;
case SET5_H: M_SET(5,HL_H);break;  case SET5_L: M_SET(5,HL_L);break;
case SET5_xHL: I=RdZ80(HL_W);M_SET(5,I);WrZ80(HL_W,I);break;
case SET5_A: M_SET(5,AF_H);break;

case SET6_B: M_SET(6,BC_H);break;  case SET6_C: M_SET(6,BC_L);break;
case SET6_D: M_SET(6,DE_H);break;  case SET6_E: M_SET(6,DE_L);break;
case SET6_H: M_SET(6,HL_H);break;  case SET6_L: M_SET(6,HL_L);break;
case SET6_xHL: I=RdZ80(HL_W);M_SET(6,I);WrZ80(HL_W,I);break;
case SET6_A: M_SET(6,AF_H);break;

case SET7_B: M_SET(7,BC_H);break;  case SET7_C: M_SET(7,BC_L);break;
case SET7_D: M_SET(7,DE_H);break;  case SET7_E: M_SET(7,DE_L);break;
case SET7_H: M_SET(7,HL_H);break;  case SET7_L: M_SET(7,HL_L);break;
case SET7_xHL: I=RdZ80(HL_W);M_SET(7,I);WrZ80(HL_W,I);break;
case SET7_A: M_SET(7,AF_H);break;
