#include "cpu-z80a-opcodes-op.inc"
    case 0x00: /* NOP               */
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x03: /* INC BC            */
      self->state.regs.BC.w.l++;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 6;
      self->state.ctrs.i_period -= 6;
      goto epilog;
    case 0x06: /* LD B,n            */
      self->state.regs.BC.b.h = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0x07: /* RLCA              */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (T1.b.l << 1) | (T1.b.l >> 7);
      self->state.regs.AF.b.l &= (SF | ZF | YF | XF | PF);
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= CF;
      }
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x0b: /* DEC BC            */
      self->state.regs.BC.w.l--;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 6;
      self->state.ctrs.i_period -= 6;
      goto epilog;
    case 0x0e: /* LD C,n            */
      self->state.regs.BC.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0x0f: /* RRCA              */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (T1.b.l >> 1) | (T1.b.l << 7);
      self->state.regs.AF.b.l &= (SF | ZF | YF | XF | PF);
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= CF;
      }
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x13: /* INC DE            */
      self->state.regs.DE.w.l++;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 6;
      self->state.ctrs.i_period -= 6;
      goto epilog;
    case 0x16: /* LD D,n            */
      self->state.regs.DE.b.h = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0x17: /* RLA               */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = T1.b.l << 1;
      if(self->state.regs.AF.b.l & CF) {
        T2.b.l |= 0x01;
      }
      self->state.regs.AF.b.l &= (SF | ZF | YF | XF | PF);
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= CF;
      }
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x1b: /* DEC DE            */
      self->state.regs.DE.w.l--;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 6;
      self->state.ctrs.i_period -= 6;
      goto epilog;
    case 0x1e: /* LD E,n            */
      self->state.regs.DE.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0x1f: /* RRA               */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = T1.b.l >> 1;
      if(self->state.regs.AF.b.l & CF) {
        T2.b.l |= 0x80;
      }
      self->state.regs.AF.b.l &= (SF | ZF | YF | XF | PF);
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= CF;
      }
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x23: /* INC HL            */
      self->state.regs.HL.w.l++;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 6;
      self->state.ctrs.i_period -= 6;
      goto epilog;
    case 0x26: /* LD H,n            */
      self->state.regs.HL.b.h = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0x27: /* DAA               */
      WZ.w.l = self->state.regs.AF.b.h;
      if(self->state.regs.AF.b.l & CF) WZ.w.l |= 0x100;
      if(self->state.regs.AF.b.l & HF) WZ.w.l |= 0x200;
      if(self->state.regs.AF.b.l & NF) WZ.w.l |= 0x400;
      self->state.regs.AF.w.l = DAATable[WZ.w.l];
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x2b: /* DEC HL            */
      self->state.regs.HL.w.l--;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 6;
      self->state.ctrs.i_period -= 6;
      goto epilog;
    case 0x2e: /* LD L,n            */
      self->state.regs.HL.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0x33: /* INC SP            */
      self->state.regs.SP.w.l++;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 6;
      self->state.ctrs.i_period -= 6;
      goto epilog;
    case 0x3b: /* DEC SP            */
      self->state.regs.SP.w.l--;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 6;
      self->state.ctrs.i_period -= 6;
      goto epilog;
    case 0x3e: /* LD A,n            */
      self->state.regs.AF.b.h = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0x40: /* LD B,B            */
      self->state.regs.BC.b.h = self->state.regs.BC.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x41: /* LD B,C            */
      self->state.regs.BC.b.h = self->state.regs.BC.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x42: /* LD B,D            */
      self->state.regs.BC.b.h = self->state.regs.DE.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x43: /* LD B,E            */
      self->state.regs.BC.b.h = self->state.regs.DE.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x44: /* LD B,H            */
      self->state.regs.BC.b.h = self->state.regs.HL.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x45: /* LD B,L            */
      self->state.regs.BC.b.h = self->state.regs.HL.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x46: /* LD B,(HL)         */
      self->state.regs.BC.b.h = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l, 0x00);
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0x47: /* LD B,A            */
      self->state.regs.BC.b.h = self->state.regs.AF.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x48: /* LD C,B            */
      self->state.regs.BC.b.l = self->state.regs.BC.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x49: /* LD C,C            */
      self->state.regs.BC.b.l = self->state.regs.BC.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x4a: /* LD C,D            */
      self->state.regs.BC.b.l = self->state.regs.DE.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x4b: /* LD C,E            */
      self->state.regs.BC.b.l = self->state.regs.DE.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x4c: /* LD C,H            */
      self->state.regs.BC.b.l = self->state.regs.HL.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x4d: /* LD C,L            */
      self->state.regs.BC.b.l = self->state.regs.HL.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x4e: /* LD C,(HL)         */
      self->state.regs.BC.b.l = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l, 0x00);
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0x4f: /* LD C,A            */
      self->state.regs.BC.b.l = self->state.regs.AF.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x50: /* LD D,B            */
      self->state.regs.DE.b.h = self->state.regs.BC.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x51: /* LD D,C            */
      self->state.regs.DE.b.h = self->state.regs.BC.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x52: /* LD D,D            */
      self->state.regs.DE.b.h = self->state.regs.DE.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x53: /* LD D,E            */
      self->state.regs.DE.b.h = self->state.regs.DE.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x54: /* LD D,H            */
      self->state.regs.DE.b.h = self->state.regs.HL.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x55: /* LD D,L            */
      self->state.regs.DE.b.h = self->state.regs.HL.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x56: /* LD D,(HL)         */
      self->state.regs.DE.b.h = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l, 0x00);
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0x57: /* LD D,A            */
      self->state.regs.DE.b.h = self->state.regs.AF.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x58: /* LD E,B            */
      self->state.regs.DE.b.l = self->state.regs.BC.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x59: /* LD E,C            */
      self->state.regs.DE.b.l = self->state.regs.BC.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x5a: /* LD E,D            */
      self->state.regs.DE.b.l = self->state.regs.DE.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x5b: /* LD E,E            */
      self->state.regs.DE.b.l = self->state.regs.DE.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x5c: /* LD E,H            */
      self->state.regs.DE.b.l = self->state.regs.HL.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x5d: /* LD E,L            */
      self->state.regs.DE.b.l = self->state.regs.HL.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x5e: /* LD E,(HL)         */
      self->state.regs.DE.b.l = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l, 0x00);
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0x5f: /* LD E,A            */
      self->state.regs.DE.b.l = self->state.regs.AF.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x60: /* LD H,B            */
      self->state.regs.HL.b.h = self->state.regs.BC.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x61: /* LD H,C            */
      self->state.regs.HL.b.h = self->state.regs.BC.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x62: /* LD H,D            */
      self->state.regs.HL.b.h = self->state.regs.DE.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x63: /* LD H,E            */
      self->state.regs.HL.b.h = self->state.regs.DE.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x64: /* LD H,H            */
      self->state.regs.HL.b.h = self->state.regs.HL.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x65: /* LD H,L            */
      self->state.regs.HL.b.h = self->state.regs.HL.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x66: /* LD H,(HL)         */
      self->state.regs.HL.b.h = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l, 0x00);
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0x67: /* LD H,A            */
      self->state.regs.HL.b.h = self->state.regs.AF.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x68: /* LD L,B            */
      self->state.regs.HL.b.l = self->state.regs.BC.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x69: /* LD L,C            */
      self->state.regs.HL.b.l = self->state.regs.BC.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x6a: /* LD L,D            */
      self->state.regs.HL.b.l = self->state.regs.DE.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x6b: /* LD L,E            */
      self->state.regs.HL.b.l = self->state.regs.DE.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x6c: /* LD L,H            */
      self->state.regs.HL.b.l = self->state.regs.HL.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x6d: /* LD L,L            */
      self->state.regs.HL.b.l = self->state.regs.HL.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x6e: /* LD L,(HL)         */
      self->state.regs.HL.b.l = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l, 0x00);
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0x6f: /* LD L,A            */
      self->state.regs.HL.b.l = self->state.regs.AF.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x70: /* LD (HL),B         */
      (*self->iface.mreq_wr)(self, self->state.regs.HL.w.l, self->state.regs.BC.b.h);
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0x71: /* LD (HL),C         */
      (*self->iface.mreq_wr)(self, self->state.regs.HL.w.l, self->state.regs.BC.b.l);
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0x72: /* LD (HL),D         */
      (*self->iface.mreq_wr)(self, self->state.regs.HL.w.l, self->state.regs.DE.b.h);
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0x73: /* LD (HL),E         */
      (*self->iface.mreq_wr)(self, self->state.regs.HL.w.l, self->state.regs.DE.b.l);
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0x74: /* LD (HL),H         */
      (*self->iface.mreq_wr)(self, self->state.regs.HL.w.l, self->state.regs.HL.b.h);
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0x75: /* LD (HL),L         */
      (*self->iface.mreq_wr)(self, self->state.regs.HL.w.l, self->state.regs.HL.b.l);
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0x76: /* HALT              */
      self->state.regs.ST.b.l |= ST_HLT;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x77: /* LD (HL),A         */
      (*self->iface.mreq_wr)(self, self->state.regs.HL.w.l, self->state.regs.AF.b.h);
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0x78: /* LD A,B            */
      self->state.regs.AF.b.h = self->state.regs.BC.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x79: /* LD A,C            */
      self->state.regs.AF.b.h = self->state.regs.BC.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x7a: /* LD A,D            */
      self->state.regs.AF.b.h = self->state.regs.DE.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x7b: /* LD A,E            */
      self->state.regs.AF.b.h = self->state.regs.DE.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x7c: /* LD A,H            */
      self->state.regs.AF.b.h = self->state.regs.HL.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x7d: /* LD A,L            */
      self->state.regs.AF.b.h = self->state.regs.HL.b.l;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x7e: /* LD A,(HL)         */
      self->state.regs.AF.b.h = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l, 0x00);
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0x7f: /* LD A,A            */
      self->state.regs.AF.b.h = self->state.regs.AF.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x80: /* ADD A,B           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.h;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = OP_ADD | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x81: /* ADD A,C           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.l;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = OP_ADD | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x82: /* ADD A,D           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.h;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = OP_ADD | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x83: /* ADD A,E           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.l;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = OP_ADD | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x84: /* ADD A,H           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.HL.b.h;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = OP_ADD | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x85: /* ADD A,L           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.HL.b.l;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = OP_ADD | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x86: /* ADD A,(HL)        */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l, 0x00);
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = OP_ADD | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0x87: /* ADD A,A           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.AF.b.h;
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = OP_ADD | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x88: /* ADC A,B           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.h;
      WZ.w.l = T1.b.l + T2.b.l + (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_ADC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x89: /* ADC A,C           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.l;
      WZ.w.l = T1.b.l + T2.b.l + (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_ADC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x8a: /* ADC A,D           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.h;
      WZ.w.l = T1.b.l + T2.b.l + (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_ADC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x8b: /* ADC A,E           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.l;
      WZ.w.l = T1.b.l + T2.b.l + (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_ADC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x8c: /* ADC A,H           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.HL.b.h;
      WZ.w.l = T1.b.l + T2.b.l + (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_ADC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x8d: /* ADC A,L           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.HL.b.l;
      WZ.w.l = T1.b.l + T2.b.l + (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_ADC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x8e: /* ADC A,(HL)        */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l, 0x00);
      WZ.w.l = T1.b.l + T2.b.l + (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_ADC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0x8f: /* ADC A,A           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.AF.b.h;
      WZ.w.l = T1.b.l + T2.b.l + (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_ADC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x90: /* SUB A,B           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_SUB | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x91: /* SUB A,C           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.l;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_SUB | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x92: /* SUB A,D           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_SUB | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x93: /* SUB A,E           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.l;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_SUB | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x94: /* SUB A,H           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.HL.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_SUB | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x95: /* SUB A,L           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.HL.b.l;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_SUB | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x96: /* SUB A,(HL)        */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l, 0x00);
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_SUB | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0x97: /* SUB A,A           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.AF.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_SUB | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x98: /* SBC A,B           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.h;
      WZ.w.l = T1.b.l - T2.b.l - (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_SBC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x99: /* SBC A,C           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.l;
      WZ.w.l = T1.b.l - T2.b.l - (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_SBC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x9a: /* SBC A,D           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.h;
      WZ.w.l = T1.b.l - T2.b.l - (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_SBC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x9b: /* SBC A,E           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.l;
      WZ.w.l = T1.b.l - T2.b.l - (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_SBC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x9c: /* SBC A,H           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.HL.b.h;
      WZ.w.l = T1.b.l - T2.b.l - (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_SBC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x9d: /* SBC A,L           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.HL.b.l;
      WZ.w.l = T1.b.l - T2.b.l - (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_SBC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0x9e: /* SBC A,(HL)        */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l, 0x00);
      WZ.w.l = T1.b.l - T2.b.l - (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_SBC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0x9f: /* SBC A,A           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.AF.b.h;
      WZ.w.l = T1.b.l - T2.b.l - (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_SBC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0xa0: /* AND A,B           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.h;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = OP_AND | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0xa1: /* AND A,C           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.l;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = OP_AND | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0xa2: /* AND A,D           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.h;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = OP_AND | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0xa3: /* AND A,E           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.l;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = OP_AND | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0xa4: /* AND A,H           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.HL.b.h;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = OP_AND | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0xa5: /* AND A,L           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.HL.b.l;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = OP_AND | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0xa6: /* AND A,(HL)        */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l, 0x00);
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = OP_AND | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0xa7: /* AND A,A           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.AF.b.h;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = OP_AND | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0xa8: /* XOR A,B           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.h;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = OP_XOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0xa9: /* XOR A,C           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.l;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = OP_XOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0xaa: /* XOR A,D           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.h;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = OP_XOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0xab: /* XOR A,E           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.l;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = OP_XOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0xac: /* XOR A,H           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.HL.b.h;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = OP_XOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0xad: /* XOR A,L           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.HL.b.l;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = OP_XOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0xae: /* XOR A,(HL)        */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l, 0x00);
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = OP_XOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0xaf: /* XOR A,A           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.AF.b.h;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = OP_XOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0xb0: /* OR A,B            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.h;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = OP_IOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0xb1: /* OR A,C            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.l;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = OP_IOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0xb2: /* OR A,D            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.h;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = OP_IOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0xb3: /* OR A,E            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.l;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = OP_IOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0xb4: /* OR A,H            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.HL.b.h;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = OP_IOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0xb5: /* OR A,L            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.HL.b.l;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = OP_IOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0xb6: /* OR A,(HL)         */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l, 0x00);
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = OP_IOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0xb7: /* OR A,A            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.AF.b.h;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = OP_IOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0xb8: /* CP A,B            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_CMP | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0xb9: /* CP A,C            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.l;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_CMP | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0xba: /* CP A,D            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_CMP | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0xbb: /* CP A,E            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.l;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_CMP | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0xbc: /* CP A,H            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.HL.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_CMP | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0xbd: /* CP A,L            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.HL.b.l;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_CMP | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0xbe: /* CP A,(HL)         */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.HL.w.l, 0x00);
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_CMP | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0xbf: /* CP A,A            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.AF.b.h;
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_CMP | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto epilog;
    case 0xc6: /* ADD A,n           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      WZ.w.l = T1.b.l + T2.b.l;
      WZ.b.h = OP_ADD | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0xcb:
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto fetch_cb_opcode;
    case 0xce: /* ADC A,n           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      WZ.w.l = T1.b.l + T2.b.l + (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_ADC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & T2.b.l & ~WZ.b.l) | (~T1.b.l & ~T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0xd6: /* SUB A,n           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_SUB | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0xdd:
      goto fetch_dd_opcode;
    case 0xde: /* SBC A,n           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      WZ.w.l = T1.b.l - T2.b.l - (self->state.regs.AF.b.l & CF);
      WZ.b.h = OP_SBC | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0xe6: /* AND A,n           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = OP_AND | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0xed:
      self->state.ctrs.m_cycles += 1;
      self->state.ctrs.t_states += 4;
      self->state.ctrs.i_period -= 4;
      goto fetch_ed_opcode;
    case 0xee: /* XOR A,n           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = OP_XOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0xf6: /* OR A,n            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = OP_IOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
    case 0xfd:
      goto fetch_fd_opcode;
    case 0xfe: /* CP A,n            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_CMP | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 7;
      self->state.ctrs.i_period -= 7;
      goto epilog;
