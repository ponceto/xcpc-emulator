#define XY   IY
#define XY_W IY_W
#define XY_L IY_L
#define XY_H IY_H
#include "cpu-z80a-opcodes-xy.inc"
#undef XY
#undef XY_W
#undef XY_L
#undef XY_H

    case 0x00: /* NOP               */
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x03: /* INC BC            */
      self->state.regs.BC.w.l++;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 10;
      self->state.ctrs.i_period -= 10;
      goto epilog;
    case 0x06: /* LD B,n            */
      self->state.regs.BC.b.h = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.i_period -= 11;
      goto epilog;
    case 0x07: /* RLCA              */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (T1.b.l << 1) | (T1.b.l >> 7);
      self->state.regs.AF.b.l &= (SF | ZF | YF | XF | PF);
      if(T1.b.l & 0x80) {
        self->state.regs.AF.b.l |= CF;
      }
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x0b: /* DEC BC            */
      self->state.regs.BC.w.l--;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 10;
      self->state.ctrs.i_period -= 10;
      goto epilog;
    case 0x0e: /* LD C,n            */
      self->state.regs.BC.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.i_period -= 11;
      goto epilog;
    case 0x0f: /* RRCA              */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (T1.b.l >> 1) | (T1.b.l << 7);
      self->state.regs.AF.b.l &= (SF | ZF | YF | XF | PF);
      if(T1.b.l & 0x01) {
        self->state.regs.AF.b.l |= CF;
      }
      self->state.regs.AF.b.h = T2.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x13: /* INC DE            */
      self->state.regs.DE.w.l++;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 10;
      self->state.ctrs.i_period -= 10;
      goto epilog;
    case 0x16: /* LD D,n            */
      self->state.regs.DE.b.h = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.i_period -= 11;
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
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x1b: /* DEC DE            */
      self->state.regs.DE.w.l--;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 10;
      self->state.ctrs.i_period -= 10;
      goto epilog;
    case 0x1e: /* LD E,n            */
      self->state.regs.DE.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.i_period -= 11;
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
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x23: /* INC IY            */
      self->state.regs.IY.w.l++;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 10;
      self->state.ctrs.i_period -= 10;
      goto epilog;
    case 0x26: /* LD IYh,n          */
      self->state.regs.IY.b.h = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.i_period -= 11;
      goto epilog;
    case 0x27: /* DAA               */
      WZ.w.l = self->state.regs.AF.b.h;
      if(self->state.regs.AF.b.l & CF) WZ.w.l |= 0x100;
      if(self->state.regs.AF.b.l & HF) WZ.w.l |= 0x200;
      if(self->state.regs.AF.b.l & NF) WZ.w.l |= 0x400;
      self->state.regs.AF.w.l = DAATable[WZ.w.l];
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x2b: /* DEC IY            */
      self->state.regs.IY.w.l--;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 10;
      self->state.ctrs.i_period -= 10;
      goto epilog;
    case 0x2e: /* LD IYl,n          */
      self->state.regs.IY.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.i_period -= 11;
      goto epilog;
    case 0x33: /* INC SP            */
      self->state.regs.SP.w.l++;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 10;
      self->state.ctrs.i_period -= 10;
      goto epilog;
    case 0x3b: /* DEC SP            */
      self->state.regs.SP.w.l--;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 10;
      self->state.ctrs.i_period -= 10;
      goto epilog;
    case 0x3e: /* LD A,n            */
      self->state.regs.AF.b.h = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.i_period -= 11;
      goto epilog;
    case 0x40: /* LD B,B            */
      self->state.regs.BC.b.h = self->state.regs.BC.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x41: /* LD B,C            */
      self->state.regs.BC.b.h = self->state.regs.BC.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x42: /* LD B,D            */
      self->state.regs.BC.b.h = self->state.regs.DE.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x43: /* LD B,E            */
      self->state.regs.BC.b.h = self->state.regs.DE.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x44: /* LD B,IYh          */
      self->state.regs.BC.b.h = self->state.regs.IY.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x45: /* LD B,IYl          */
      self->state.regs.BC.b.h = self->state.regs.IY.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x46: /* LD B,(IY+d)       */
      self->state.regs.BC.b.h = (*self->iface.mreq_rd)(self, (self->state.regs.IY.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00)), 0x00);
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.i_period -= 19;
      goto epilog;
    case 0x47: /* LD B,A            */
      self->state.regs.BC.b.h = self->state.regs.AF.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x48: /* LD C,B            */
      self->state.regs.BC.b.l = self->state.regs.BC.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x49: /* LD C,C            */
      self->state.regs.BC.b.l = self->state.regs.BC.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x4a: /* LD C,D            */
      self->state.regs.BC.b.l = self->state.regs.DE.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x4b: /* LD C,E            */
      self->state.regs.BC.b.l = self->state.regs.DE.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x4c: /* LD C,IYh          */
      self->state.regs.BC.b.l = self->state.regs.IY.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x4d: /* LD C,IYl          */
      self->state.regs.BC.b.l = self->state.regs.IY.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x4e: /* LD C,(IY+d)       */
      self->state.regs.BC.b.l = (*self->iface.mreq_rd)(self, (self->state.regs.IY.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00)), 0x00);
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.i_period -= 19;
      goto epilog;
    case 0x4f: /* LD C,A            */
      self->state.regs.BC.b.l = self->state.regs.AF.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x50: /* LD D,B            */
      self->state.regs.DE.b.h = self->state.regs.BC.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x51: /* LD D,C            */
      self->state.regs.DE.b.h = self->state.regs.BC.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x52: /* LD D,D            */
      self->state.regs.DE.b.h = self->state.regs.DE.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x53: /* LD D,E            */
      self->state.regs.DE.b.h = self->state.regs.DE.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x54: /* LD D,IYh          */
      self->state.regs.DE.b.h = self->state.regs.IY.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x55: /* LD D,IYl          */
      self->state.regs.DE.b.h = self->state.regs.IY.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x56: /* LD D,(IY+d)       */
      self->state.regs.DE.b.h = (*self->iface.mreq_rd)(self, (self->state.regs.IY.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00)), 0x00);
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.i_period -= 19;
      goto epilog;
    case 0x57: /* LD D,A            */
      self->state.regs.DE.b.h = self->state.regs.AF.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x58: /* LD E,B            */
      self->state.regs.DE.b.l = self->state.regs.BC.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x59: /* LD E,C            */
      self->state.regs.DE.b.l = self->state.regs.BC.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x5a: /* LD E,D            */
      self->state.regs.DE.b.l = self->state.regs.DE.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x5b: /* LD E,E            */
      self->state.regs.DE.b.l = self->state.regs.DE.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x5c: /* LD E,IYh          */
      self->state.regs.DE.b.l = self->state.regs.IY.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x5d: /* LD E,IYl          */
      self->state.regs.DE.b.l = self->state.regs.IY.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x5e: /* LD E,(IY+d)       */
      self->state.regs.DE.b.l = (*self->iface.mreq_rd)(self, (self->state.regs.IY.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00)), 0x00);
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.i_period -= 19;
      goto epilog;
    case 0x5f: /* LD E,A            */
      self->state.regs.DE.b.l = self->state.regs.AF.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x60: /* LD IYh,B          */
      self->state.regs.IY.b.h = self->state.regs.BC.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x61: /* LD IYh,C          */
      self->state.regs.IY.b.h = self->state.regs.BC.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x62: /* LD IYh,D          */
      self->state.regs.IY.b.h = self->state.regs.DE.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x63: /* LD IYh,E          */
      self->state.regs.IY.b.h = self->state.regs.DE.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x64: /* LD IYh,IYh        */
      self->state.regs.IY.b.h = self->state.regs.IY.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x65: /* LD IYh,IYl        */
      self->state.regs.IY.b.h = self->state.regs.IY.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x66: /* LD H,(IY+d)       */
      self->state.regs.HL.b.h = (*self->iface.mreq_rd)(self, (self->state.regs.IY.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00)), 0x00);
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.i_period -= 19;
      goto epilog;
    case 0x67: /* LD IYh,A          */
      self->state.regs.IY.b.h = self->state.regs.AF.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x68: /* LD IYl,B          */
      self->state.regs.IY.b.l = self->state.regs.BC.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x69: /* LD IYl,C          */
      self->state.regs.IY.b.l = self->state.regs.BC.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x6a: /* LD IYl,D          */
      self->state.regs.IY.b.l = self->state.regs.DE.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x6b: /* LD IYl,E          */
      self->state.regs.IY.b.l = self->state.regs.DE.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x6c: /* LD IYl,IYh        */
      self->state.regs.IY.b.l = self->state.regs.IY.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x6d: /* LD IYl,IYl        */
      self->state.regs.IY.b.l = self->state.regs.IY.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x6e: /* LD L,(IY+d)       */
      self->state.regs.HL.b.l = (*self->iface.mreq_rd)(self, (self->state.regs.IY.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00)), 0x00);
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.i_period -= 19;
      goto epilog;
    case 0x6f: /* LD IYl,A          */
      self->state.regs.IY.b.l = self->state.regs.AF.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x70: /* LD (IY+d),B       */
      (*self->iface.mreq_wr)(self, (self->state.regs.IY.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00)), self->state.regs.BC.b.h);
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.i_period -= 19;
      goto epilog;
    case 0x71: /* LD (IY+d),C       */
      (*self->iface.mreq_wr)(self, (self->state.regs.IY.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00)), self->state.regs.BC.b.l);
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.i_period -= 19;
      goto epilog;
    case 0x72: /* LD (IY+d),D       */
      (*self->iface.mreq_wr)(self, (self->state.regs.IY.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00)), self->state.regs.DE.b.h);
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.i_period -= 19;
      goto epilog;
    case 0x73: /* LD (IY+d),E       */
      (*self->iface.mreq_wr)(self, (self->state.regs.IY.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00)), self->state.regs.DE.b.l);
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.i_period -= 19;
      goto epilog;
    case 0x74: /* LD (IY+d),H       */
      (*self->iface.mreq_wr)(self, (self->state.regs.IY.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00)), self->state.regs.HL.b.h);
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.i_period -= 19;
      goto epilog;
    case 0x75: /* LD (IY+d),L       */
      (*self->iface.mreq_wr)(self, (self->state.regs.IY.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00)), self->state.regs.HL.b.l);
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.i_period -= 19;
      goto epilog;
    case 0x76: /* HALT              */
      self->state.regs.ST.b.l |= ST_HLT;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x77: /* LD (IY+d),A       */
      (*self->iface.mreq_wr)(self, (self->state.regs.IY.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00)), self->state.regs.AF.b.h);
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.i_period -= 19;
      goto epilog;
    case 0x78: /* LD A,B            */
      self->state.regs.AF.b.h = self->state.regs.BC.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x79: /* LD A,C            */
      self->state.regs.AF.b.h = self->state.regs.BC.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x7a: /* LD A,D            */
      self->state.regs.AF.b.h = self->state.regs.DE.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x7b: /* LD A,E            */
      self->state.regs.AF.b.h = self->state.regs.DE.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x7c: /* LD A,IYh          */
      self->state.regs.AF.b.h = self->state.regs.IY.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x7d: /* LD A,IYl          */
      self->state.regs.AF.b.h = self->state.regs.IY.b.l;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x7e: /* LD A,(IY+d)       */
      self->state.regs.AF.b.h = (*self->iface.mreq_rd)(self, (self->state.regs.IY.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00)), 0x00);
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.i_period -= 19;
      goto epilog;
    case 0x7f: /* LD A,A            */
      self->state.regs.AF.b.h = self->state.regs.AF.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
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
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
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
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
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
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
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
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x84: /* ADD A,IYh         */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IY.b.h;
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
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x85: /* ADD A,IYl         */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IY.b.l;
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
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x86: /* ADD A,(IY+d)      */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, (self->state.regs.IY.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00)), 0x00);
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
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.i_period -= 19;
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
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
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
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
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
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
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
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
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
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x8c: /* ADC A,IYh         */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IY.b.h;
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
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x8d: /* ADC A,IYl         */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IY.b.l;
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
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x8e: /* ADC A,(IY+d)      */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, (self->state.regs.IY.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00)), 0x00);
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
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.i_period -= 19;
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
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
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
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
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
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
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
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
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
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x94: /* SUB A,IYh         */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IY.b.h;
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
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x95: /* SUB A,IYl         */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IY.b.l;
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
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x96: /* SUB A,(IY+d)      */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, (self->state.regs.IY.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00)), 0x00);
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
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.i_period -= 19;
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
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
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
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
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
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
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
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
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
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x9c: /* SBC A,IYh         */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IY.b.h;
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
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x9d: /* SBC A,IYl         */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IY.b.l;
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
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0x9e: /* SBC A,(IY+d)      */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, (self->state.regs.IY.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00)), 0x00);
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
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.i_period -= 19;
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
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xa0: /* AND A,B           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.h;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = OP_AND | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xa1: /* AND A,C           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.l;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = OP_AND | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xa2: /* AND A,D           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.h;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = OP_AND | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xa3: /* AND A,E           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.l;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = OP_AND | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xa4: /* AND A,IYh         */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IY.b.h;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = OP_AND | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xa5: /* AND A,IYl         */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IY.b.l;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = OP_AND | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xa6: /* AND A,(IY+d)      */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, (self->state.regs.IY.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00)), 0x00);
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = OP_AND | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.i_period -= 19;
      goto epilog;
    case 0xa7: /* AND A,A           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.AF.b.h;
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = OP_AND | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xa8: /* XOR A,B           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.h;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = OP_XOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xa9: /* XOR A,C           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.l;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = OP_XOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xaa: /* XOR A,D           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.h;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = OP_XOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xab: /* XOR A,E           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.l;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = OP_XOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xac: /* XOR A,IYh         */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IY.b.h;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = OP_XOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xad: /* XOR A,IYl         */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IY.b.l;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = OP_XOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xae: /* XOR A,(IY+d)      */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, (self->state.regs.IY.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00)), 0x00);
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = OP_XOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.i_period -= 19;
      goto epilog;
    case 0xaf: /* XOR A,A           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.AF.b.h;
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = OP_XOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xb0: /* OR A,B            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.h;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = OP_IOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xb1: /* OR A,C            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.BC.b.l;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = OP_IOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xb2: /* OR A,D            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.h;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = OP_IOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xb3: /* OR A,E            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.DE.b.l;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = OP_IOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xb4: /* OR A,IYh          */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IY.b.h;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = OP_IOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xb5: /* OR A,IYl          */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IY.b.l;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = OP_IOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xb6: /* OR A,(IY+d)       */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, (self->state.regs.IY.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00)), 0x00);
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = OP_IOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.i_period -= 19;
      goto epilog;
    case 0xb7: /* OR A,A            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.AF.b.h;
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = OP_IOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
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
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
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
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
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
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
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
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xbc: /* CP A,IYh          */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IY.b.h;
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
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xbd: /* CP A,IYl          */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = self->state.regs.IY.b.l;
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
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
      goto epilog;
    case 0xbe: /* CP A,(IY+d)       */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, (self->state.regs.IY.w.l + (int8_t) (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00)), 0x00);
      WZ.w.l = T1.b.l - T2.b.l;
      WZ.b.h = OP_CMP | (WZ.b.l & (SF | YF | XF)) | ((WZ.b.l ^ T1.b.l ^ T2.b.l) & HF) | (WZ.b.h & CF);
      if(WZ.b.l == 0) {
        WZ.b.h |= ZF;
      }
      if(((T1.b.l & ~T2.b.l & ~WZ.b.l) | (~T1.b.l & T2.b.l & WZ.b.l)) & SF) {
        WZ.b.h |= VF;
      }
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 5;
      self->state.ctrs.t_states += 19;
      self->state.ctrs.i_period -= 19;
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
      self->state.ctrs.m_cycles += 2;
      self->state.ctrs.t_states += 8;
      self->state.ctrs.i_period -= 8;
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
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.i_period -= 11;
      goto epilog;
    case 0xcb:
      goto fetch_fdcb_opcode;
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
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.i_period -= 11;
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
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.i_period -= 11;
      goto epilog;
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
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.i_period -= 11;
      goto epilog;
    case 0xe6: /* AND A,n           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      WZ.w.l = T1.b.l & T2.b.l;
      WZ.b.h = OP_AND | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.i_period -= 11;
      goto epilog;
    case 0xee: /* XOR A,n           */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      WZ.w.l = T1.b.l ^ T2.b.l;
      WZ.b.h = OP_XOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.i_period -= 11;
      goto epilog;
    case 0xf6: /* OR A,n            */
      T1.b.l = self->state.regs.AF.b.h;
      T2.b.l = (*self->iface.mreq_rd)(self, self->state.regs.PC.w.l++, 0x00);
      WZ.w.l = T1.b.l | T2.b.l;
      WZ.b.h = OP_IOR | (WZ.b.l & (SF | YF | XF)) | PZSTable[WZ.b.l];
      self->state.regs.AF.b.h = WZ.b.l;
      self->state.regs.AF.b.l = WZ.b.h;
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.i_period -= 11;
      goto epilog;
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
      self->state.ctrs.m_cycles += 3;
      self->state.ctrs.t_states += 11;
      self->state.ctrs.i_period -= 11;
      goto epilog;
