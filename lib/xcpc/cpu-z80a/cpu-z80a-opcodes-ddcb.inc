        case 0x00: /* LD B,RLC (IX+d)   */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = (T1.b.l << 1) | (T1.b.l >> 7);
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
            if(T1.b.l & 0x80) {
                self->state.regs.AF.b.l |= _CF;
            }
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.BC.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x01: /* LD C,RLC (IX+d)   */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = (T1.b.l << 1) | (T1.b.l >> 7);
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
            if(T1.b.l & 0x80) {
                self->state.regs.AF.b.l |= _CF;
            }
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.BC.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x02: /* LD D,RLC (IX+d)   */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = (T1.b.l << 1) | (T1.b.l >> 7);
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
            if(T1.b.l & 0x80) {
                self->state.regs.AF.b.l |= _CF;
            }
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.DE.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x03: /* LD E,RLC (IX+d)   */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = (T1.b.l << 1) | (T1.b.l >> 7);
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
            if(T1.b.l & 0x80) {
                self->state.regs.AF.b.l |= _CF;
            }
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.DE.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x04: /* LD H,RLC (IX+d)   */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = (T1.b.l << 1) | (T1.b.l >> 7);
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
            if(T1.b.l & 0x80) {
                self->state.regs.AF.b.l |= _CF;
            }
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.HL.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x05: /* LD L,RLC (IX+d)   */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = (T1.b.l << 1) | (T1.b.l >> 7);
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
            if(T1.b.l & 0x80) {
                self->state.regs.AF.b.l |= _CF;
            }
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.HL.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x06: /* RLC (IX+d)        */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = (T1.b.l << 1) | (T1.b.l >> 7);
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
            if(T1.b.l & 0x80) {
                self->state.regs.AF.b.l |= _CF;
            }
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x07: /* LD A,RLC (IX+d)   */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = (T1.b.l << 1) | (T1.b.l >> 7);
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
            if(T1.b.l & 0x80) {
                self->state.regs.AF.b.l |= _CF;
            }
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.AF.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x08: /* LD B,RRC (IX+d)   */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = (T1.b.l >> 1) | (T1.b.l << 7);
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
            if(T1.b.l & 0x01) {
                self->state.regs.AF.b.l |= _CF;
            }
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.BC.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x09: /* LD C,RRC (IX+d)   */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = (T1.b.l >> 1) | (T1.b.l << 7);
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
            if(T1.b.l & 0x01) {
                self->state.regs.AF.b.l |= _CF;
            }
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.BC.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x0a: /* LD D,RRC (IX+d)   */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = (T1.b.l >> 1) | (T1.b.l << 7);
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
            if(T1.b.l & 0x01) {
                self->state.regs.AF.b.l |= _CF;
            }
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.DE.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x0b: /* LD E,RRC (IX+d)   */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = (T1.b.l >> 1) | (T1.b.l << 7);
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
            if(T1.b.l & 0x01) {
                self->state.regs.AF.b.l |= _CF;
            }
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.DE.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x0c: /* LD H,RRC (IX+d)   */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = (T1.b.l >> 1) | (T1.b.l << 7);
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
            if(T1.b.l & 0x01) {
                self->state.regs.AF.b.l |= _CF;
            }
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.HL.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x0d: /* LD L,RRC (IX+d)   */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = (T1.b.l >> 1) | (T1.b.l << 7);
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
            if(T1.b.l & 0x01) {
                self->state.regs.AF.b.l |= _CF;
            }
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.HL.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x0e: /* RRC (IX+d)        */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = (T1.b.l >> 1) | (T1.b.l << 7);
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
            if(T1.b.l & 0x01) {
                self->state.regs.AF.b.l |= _CF;
            }
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x0f: /* LD A,RRC (IX+d)   */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = (T1.b.l >> 1) | (T1.b.l << 7);
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
            if(T1.b.l & 0x01) {
                self->state.regs.AF.b.l |= _CF;
            }
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.AF.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x10: /* LD B,RL  (IX+d)   */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l << 1;
            if(self->state.regs.AF.b.l & _CF) {
                T2.b.l |= 0x01;
            }
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
            if(T1.b.l & 0x80) {
                self->state.regs.AF.b.l |= _CF;
            }
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.BC.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x11: /* LD C,RL  (IX+d)   */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l << 1;
            if(self->state.regs.AF.b.l & _CF) {
                T2.b.l |= 0x01;
            }
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
            if(T1.b.l & 0x80) {
                self->state.regs.AF.b.l |= _CF;
            }
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.BC.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x12: /* LD D,RL  (IX+d)   */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l << 1;
            if(self->state.regs.AF.b.l & _CF) {
                T2.b.l |= 0x01;
            }
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
            if(T1.b.l & 0x80) {
                self->state.regs.AF.b.l |= _CF;
            }
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.DE.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x13: /* LD E,RL  (IX+d)   */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l << 1;
            if(self->state.regs.AF.b.l & _CF) {
                T2.b.l |= 0x01;
            }
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
            if(T1.b.l & 0x80) {
                self->state.regs.AF.b.l |= _CF;
            }
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.DE.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x14: /* LD H,RL  (IX+d)   */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l << 1;
            if(self->state.regs.AF.b.l & _CF) {
                T2.b.l |= 0x01;
            }
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
            if(T1.b.l & 0x80) {
                self->state.regs.AF.b.l |= _CF;
            }
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.HL.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x15: /* LD L,RL  (IX+d)   */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l << 1;
            if(self->state.regs.AF.b.l & _CF) {
                T2.b.l |= 0x01;
            }
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
            if(T1.b.l & 0x80) {
                self->state.regs.AF.b.l |= _CF;
            }
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.HL.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x16: /* RL  (IX+d)        */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l << 1;
            if(self->state.regs.AF.b.l & _CF) {
                T2.b.l |= 0x01;
            }
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
            if(T1.b.l & 0x80) {
                self->state.regs.AF.b.l |= _CF;
            }
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x17: /* LD A,RL  (IX+d)   */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l << 1;
            if(self->state.regs.AF.b.l & _CF) {
                T2.b.l |= 0x01;
            }
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
            if(T1.b.l & 0x80) {
                self->state.regs.AF.b.l |= _CF;
            }
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.AF.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x18: /* LD B,RR  (IX+d)   */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l >> 1;
            if(self->state.regs.AF.b.l & _CF) {
                T2.b.l |= 0x80;
            }
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
            if(T1.b.l & 0x01) {
                self->state.regs.AF.b.l |= _CF;
            }
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.BC.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x19: /* LD C,RR  (IX+d)   */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l >> 1;
            if(self->state.regs.AF.b.l & _CF) {
                T2.b.l |= 0x80;
            }
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
            if(T1.b.l & 0x01) {
                self->state.regs.AF.b.l |= _CF;
            }
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.BC.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x1a: /* LD D,RR  (IX+d)   */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l >> 1;
            if(self->state.regs.AF.b.l & _CF) {
                T2.b.l |= 0x80;
            }
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
            if(T1.b.l & 0x01) {
                self->state.regs.AF.b.l |= _CF;
            }
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.DE.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x1b: /* LD E,RR  (IX+d)   */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l >> 1;
            if(self->state.regs.AF.b.l & _CF) {
                T2.b.l |= 0x80;
            }
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
            if(T1.b.l & 0x01) {
                self->state.regs.AF.b.l |= _CF;
            }
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.DE.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x1c: /* LD H,RR  (IX+d)   */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l >> 1;
            if(self->state.regs.AF.b.l & _CF) {
                T2.b.l |= 0x80;
            }
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
            if(T1.b.l & 0x01) {
                self->state.regs.AF.b.l |= _CF;
            }
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.HL.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x1d: /* LD L,RR  (IX+d)   */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l >> 1;
            if(self->state.regs.AF.b.l & _CF) {
                T2.b.l |= 0x80;
            }
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
            if(T1.b.l & 0x01) {
                self->state.regs.AF.b.l |= _CF;
            }
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.HL.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x1e: /* RR  (IX+d)        */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l >> 1;
            if(self->state.regs.AF.b.l & _CF) {
                T2.b.l |= 0x80;
            }
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
            if(T1.b.l & 0x01) {
                self->state.regs.AF.b.l |= _CF;
            }
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x1f: /* LD A,RR  (IX+d)   */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l >> 1;
            if(self->state.regs.AF.b.l & _CF) {
                T2.b.l |= 0x80;
            }
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
            if(T1.b.l & 0x01) {
                self->state.regs.AF.b.l |= _CF;
            }
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.AF.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x20: /* LD B,SLA (IX+d)   */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = ((int8_t) T1.b.l) << 1;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
            if(T1.b.l & 0x80) {
                self->state.regs.AF.b.l |= _CF;
            }
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.BC.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x21: /* LD C,SLA (IX+d)   */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = ((int8_t) T1.b.l) << 1;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
            if(T1.b.l & 0x80) {
                self->state.regs.AF.b.l |= _CF;
            }
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.BC.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x22: /* LD D,SLA (IX+d)   */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = ((int8_t) T1.b.l) << 1;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
            if(T1.b.l & 0x80) {
                self->state.regs.AF.b.l |= _CF;
            }
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.DE.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x23: /* LD E,SLA (IX+d)   */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = ((int8_t) T1.b.l) << 1;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
            if(T1.b.l & 0x80) {
                self->state.regs.AF.b.l |= _CF;
            }
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.DE.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x24: /* LD H,SLA (IX+d)   */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = ((int8_t) T1.b.l) << 1;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
            if(T1.b.l & 0x80) {
                self->state.regs.AF.b.l |= _CF;
            }
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.HL.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x25: /* LD L,SLA (IX+d)   */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = ((int8_t) T1.b.l) << 1;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
            if(T1.b.l & 0x80) {
                self->state.regs.AF.b.l |= _CF;
            }
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.HL.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x26: /* SLA (IX+d)        */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = ((int8_t) T1.b.l) << 1;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
            if(T1.b.l & 0x80) {
                self->state.regs.AF.b.l |= _CF;
            }
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x27: /* LD A,SLA (IX+d)   */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = ((int8_t) T1.b.l) << 1;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
            if(T1.b.l & 0x80) {
                self->state.regs.AF.b.l |= _CF;
            }
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.AF.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x28: /* LD B,SRA (IX+d)   */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = ((int8_t) T1.b.l) >> 1;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
            if(T1.b.l & 0x01) {
                self->state.regs.AF.b.l |= _CF;
            }
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.BC.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x29: /* LD C,SRA (IX+d)   */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = ((int8_t) T1.b.l) >> 1;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
            if(T1.b.l & 0x01) {
                self->state.regs.AF.b.l |= _CF;
            }
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.BC.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x2a: /* LD D,SRA (IX+d)   */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = ((int8_t) T1.b.l) >> 1;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
            if(T1.b.l & 0x01) {
                self->state.regs.AF.b.l |= _CF;
            }
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.DE.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x2b: /* LD E,SRA (IX+d)   */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = ((int8_t) T1.b.l) >> 1;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
            if(T1.b.l & 0x01) {
                self->state.regs.AF.b.l |= _CF;
            }
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.DE.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x2c: /* LD H,SRA (IX+d)   */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = ((int8_t) T1.b.l) >> 1;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
            if(T1.b.l & 0x01) {
                self->state.regs.AF.b.l |= _CF;
            }
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.HL.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x2d: /* LD L,SRA (IX+d)   */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = ((int8_t) T1.b.l) >> 1;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
            if(T1.b.l & 0x01) {
                self->state.regs.AF.b.l |= _CF;
            }
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.HL.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x2e: /* SRA (IX+d)        */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = ((int8_t) T1.b.l) >> 1;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
            if(T1.b.l & 0x01) {
                self->state.regs.AF.b.l |= _CF;
            }
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x2f: /* LD A,SRA (IX+d)   */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = ((int8_t) T1.b.l) >> 1;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
            if(T1.b.l & 0x01) {
                self->state.regs.AF.b.l |= _CF;
            }
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.AF.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x30: /* LD B,SLL (IX+d)   */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = (T1.b.l << 1) | 0x01;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
            if(T1.b.l & 0x80) {
                self->state.regs.AF.b.l |= _CF;
            }
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.BC.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x31: /* LD C,SLL (IX+d)   */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = (T1.b.l << 1) | 0x01;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
            if(T1.b.l & 0x80) {
                self->state.regs.AF.b.l |= _CF;
            }
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.BC.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x32: /* LD D,SLL (IX+d)   */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = (T1.b.l << 1) | 0x01;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
            if(T1.b.l & 0x80) {
                self->state.regs.AF.b.l |= _CF;
            }
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.DE.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x33: /* LD E,SLL (IX+d)   */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = (T1.b.l << 1) | 0x01;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
            if(T1.b.l & 0x80) {
                self->state.regs.AF.b.l |= _CF;
            }
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.DE.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x34: /* LD H,SLL (IX+d)   */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = (T1.b.l << 1) | 0x01;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
            if(T1.b.l & 0x80) {
                self->state.regs.AF.b.l |= _CF;
            }
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.HL.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x35: /* LD L,SLL (IX+d)   */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = (T1.b.l << 1) | 0x01;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
            if(T1.b.l & 0x80) {
                self->state.regs.AF.b.l |= _CF;
            }
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.HL.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x36: /* SLL (IX+d)        */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = (T1.b.l << 1) | 0x01;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
            if(T1.b.l & 0x80) {
                self->state.regs.AF.b.l |= _CF;
            }
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x37: /* LD A,SLL (IX+d)   */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = (T1.b.l << 1) | 0x01;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
            if(T1.b.l & 0x80) {
                self->state.regs.AF.b.l |= _CF;
            }
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.AF.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x38: /* LD B,SRL (IX+d)   */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = (T1.b.l >> 1) & 0x7f;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
            if(T1.b.l & 0x01) {
                self->state.regs.AF.b.l |= _CF;
            }
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.BC.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x39: /* LD C,SRL (IX+d)   */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = (T1.b.l >> 1) & 0x7f;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
            if(T1.b.l & 0x01) {
                self->state.regs.AF.b.l |= _CF;
            }
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.BC.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x3a: /* LD D,SRL (IX+d)   */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = (T1.b.l >> 1) & 0x7f;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
            if(T1.b.l & 0x01) {
                self->state.regs.AF.b.l |= _CF;
            }
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.DE.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x3b: /* LD E,SRL (IX+d)   */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = (T1.b.l >> 1) & 0x7f;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
            if(T1.b.l & 0x01) {
                self->state.regs.AF.b.l |= _CF;
            }
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.DE.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x3c: /* LD H,SRL (IX+d)   */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = (T1.b.l >> 1) & 0x7f;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
            if(T1.b.l & 0x01) {
                self->state.regs.AF.b.l |= _CF;
            }
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.HL.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x3d: /* LD L,SRL (IX+d)   */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = (T1.b.l >> 1) & 0x7f;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
            if(T1.b.l & 0x01) {
                self->state.regs.AF.b.l |= _CF;
            }
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.HL.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x3e: /* SRL (IX+d)        */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = (T1.b.l >> 1) & 0x7f;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
            if(T1.b.l & 0x01) {
                self->state.regs.AF.b.l |= _CF;
            }
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x3f: /* LD A,SRL (IX+d)   */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = (T1.b.l >> 1) & 0x7f;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l];
            if(T1.b.l & 0x01) {
                self->state.regs.AF.b.l |= _CF;
            }
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.AF.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x40: /* BIT 0,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x01;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
            M_CYCLES += 5;
            T_STATES += 20;
            T_PERIOD -= 20;
            break;
        case 0x41: /* BIT 0,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x01;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
            M_CYCLES += 5;
            T_STATES += 20;
            T_PERIOD -= 20;
            break;
        case 0x42: /* BIT 0,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x01;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
            M_CYCLES += 5;
            T_STATES += 20;
            T_PERIOD -= 20;
            break;
        case 0x43: /* BIT 0,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x01;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
            M_CYCLES += 5;
            T_STATES += 20;
            T_PERIOD -= 20;
            break;
        case 0x44: /* BIT 0,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x01;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
            M_CYCLES += 5;
            T_STATES += 20;
            T_PERIOD -= 20;
            break;
        case 0x45: /* BIT 0,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x01;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
            M_CYCLES += 5;
            T_STATES += 20;
            T_PERIOD -= 20;
            break;
        case 0x46: /* BIT 0,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x01;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
            M_CYCLES += 5;
            T_STATES += 20;
            T_PERIOD -= 20;
            break;
        case 0x47: /* BIT 0,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x01;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
            M_CYCLES += 5;
            T_STATES += 20;
            T_PERIOD -= 20;
            break;
        case 0x48: /* BIT 1,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x02;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
            M_CYCLES += 5;
            T_STATES += 20;
            T_PERIOD -= 20;
            break;
        case 0x49: /* BIT 1,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x02;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
            M_CYCLES += 5;
            T_STATES += 20;
            T_PERIOD -= 20;
            break;
        case 0x4a: /* BIT 1,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x02;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
            M_CYCLES += 5;
            T_STATES += 20;
            T_PERIOD -= 20;
            break;
        case 0x4b: /* BIT 1,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x02;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
            M_CYCLES += 5;
            T_STATES += 20;
            T_PERIOD -= 20;
            break;
        case 0x4c: /* BIT 1,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x02;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
            M_CYCLES += 5;
            T_STATES += 20;
            T_PERIOD -= 20;
            break;
        case 0x4d: /* BIT 1,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x02;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
            M_CYCLES += 5;
            T_STATES += 20;
            T_PERIOD -= 20;
            break;
        case 0x4e: /* BIT 1,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x02;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
            M_CYCLES += 5;
            T_STATES += 20;
            T_PERIOD -= 20;
            break;
        case 0x4f: /* BIT 1,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x02;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
            M_CYCLES += 5;
            T_STATES += 20;
            T_PERIOD -= 20;
            break;
        case 0x50: /* BIT 2,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x04;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
            M_CYCLES += 5;
            T_STATES += 20;
            T_PERIOD -= 20;
            break;
        case 0x51: /* BIT 2,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x04;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
            M_CYCLES += 5;
            T_STATES += 20;
            T_PERIOD -= 20;
            break;
        case 0x52: /* BIT 2,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x04;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
            M_CYCLES += 5;
            T_STATES += 20;
            T_PERIOD -= 20;
            break;
        case 0x53: /* BIT 2,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x04;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
            M_CYCLES += 5;
            T_STATES += 20;
            T_PERIOD -= 20;
            break;
        case 0x54: /* BIT 2,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x04;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
            M_CYCLES += 5;
            T_STATES += 20;
            T_PERIOD -= 20;
            break;
        case 0x55: /* BIT 2,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x04;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
            M_CYCLES += 5;
            T_STATES += 20;
            T_PERIOD -= 20;
            break;
        case 0x56: /* BIT 2,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x04;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
            M_CYCLES += 5;
            T_STATES += 20;
            T_PERIOD -= 20;
            break;
        case 0x57: /* BIT 2,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x04;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
            M_CYCLES += 5;
            T_STATES += 20;
            T_PERIOD -= 20;
            break;
        case 0x58: /* BIT 3,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x08;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
            M_CYCLES += 5;
            T_STATES += 20;
            T_PERIOD -= 20;
            break;
        case 0x59: /* BIT 3,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x08;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
            M_CYCLES += 5;
            T_STATES += 20;
            T_PERIOD -= 20;
            break;
        case 0x5a: /* BIT 3,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x08;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
            M_CYCLES += 5;
            T_STATES += 20;
            T_PERIOD -= 20;
            break;
        case 0x5b: /* BIT 3,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x08;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
            M_CYCLES += 5;
            T_STATES += 20;
            T_PERIOD -= 20;
            break;
        case 0x5c: /* BIT 3,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x08;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
            M_CYCLES += 5;
            T_STATES += 20;
            T_PERIOD -= 20;
            break;
        case 0x5d: /* BIT 3,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x08;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
            M_CYCLES += 5;
            T_STATES += 20;
            T_PERIOD -= 20;
            break;
        case 0x5e: /* BIT 3,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x08;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
            M_CYCLES += 5;
            T_STATES += 20;
            T_PERIOD -= 20;
            break;
        case 0x5f: /* BIT 3,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x08;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
            M_CYCLES += 5;
            T_STATES += 20;
            T_PERIOD -= 20;
            break;
        case 0x60: /* BIT 4,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x10;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
            M_CYCLES += 5;
            T_STATES += 20;
            T_PERIOD -= 20;
            break;
        case 0x61: /* BIT 4,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x10;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
            M_CYCLES += 5;
            T_STATES += 20;
            T_PERIOD -= 20;
            break;
        case 0x62: /* BIT 4,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x10;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
            M_CYCLES += 5;
            T_STATES += 20;
            T_PERIOD -= 20;
            break;
        case 0x63: /* BIT 4,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x10;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
            M_CYCLES += 5;
            T_STATES += 20;
            T_PERIOD -= 20;
            break;
        case 0x64: /* BIT 4,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x10;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
            M_CYCLES += 5;
            T_STATES += 20;
            T_PERIOD -= 20;
            break;
        case 0x65: /* BIT 4,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x10;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
            M_CYCLES += 5;
            T_STATES += 20;
            T_PERIOD -= 20;
            break;
        case 0x66: /* BIT 4,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x10;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
            M_CYCLES += 5;
            T_STATES += 20;
            T_PERIOD -= 20;
            break;
        case 0x67: /* BIT 4,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x10;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
            M_CYCLES += 5;
            T_STATES += 20;
            T_PERIOD -= 20;
            break;
        case 0x68: /* BIT 5,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x20;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
            M_CYCLES += 5;
            T_STATES += 20;
            T_PERIOD -= 20;
            break;
        case 0x69: /* BIT 5,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x20;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
            M_CYCLES += 5;
            T_STATES += 20;
            T_PERIOD -= 20;
            break;
        case 0x6a: /* BIT 5,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x20;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
            M_CYCLES += 5;
            T_STATES += 20;
            T_PERIOD -= 20;
            break;
        case 0x6b: /* BIT 5,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x20;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
            M_CYCLES += 5;
            T_STATES += 20;
            T_PERIOD -= 20;
            break;
        case 0x6c: /* BIT 5,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x20;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
            M_CYCLES += 5;
            T_STATES += 20;
            T_PERIOD -= 20;
            break;
        case 0x6d: /* BIT 5,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x20;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
            M_CYCLES += 5;
            T_STATES += 20;
            T_PERIOD -= 20;
            break;
        case 0x6e: /* BIT 5,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x20;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
            M_CYCLES += 5;
            T_STATES += 20;
            T_PERIOD -= 20;
            break;
        case 0x6f: /* BIT 5,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x20;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
            M_CYCLES += 5;
            T_STATES += 20;
            T_PERIOD -= 20;
            break;
        case 0x70: /* BIT 6,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x40;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
            M_CYCLES += 5;
            T_STATES += 20;
            T_PERIOD -= 20;
            break;
        case 0x71: /* BIT 6,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x40;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
            M_CYCLES += 5;
            T_STATES += 20;
            T_PERIOD -= 20;
            break;
        case 0x72: /* BIT 6,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x40;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
            M_CYCLES += 5;
            T_STATES += 20;
            T_PERIOD -= 20;
            break;
        case 0x73: /* BIT 6,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x40;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
            M_CYCLES += 5;
            T_STATES += 20;
            T_PERIOD -= 20;
            break;
        case 0x74: /* BIT 6,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x40;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
            M_CYCLES += 5;
            T_STATES += 20;
            T_PERIOD -= 20;
            break;
        case 0x75: /* BIT 6,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x40;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
            M_CYCLES += 5;
            T_STATES += 20;
            T_PERIOD -= 20;
            break;
        case 0x76: /* BIT 6,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x40;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
            M_CYCLES += 5;
            T_STATES += 20;
            T_PERIOD -= 20;
            break;
        case 0x77: /* BIT 6,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x40;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
            M_CYCLES += 5;
            T_STATES += 20;
            T_PERIOD -= 20;
            break;
        case 0x78: /* BIT 7,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x80;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
            M_CYCLES += 5;
            T_STATES += 20;
            T_PERIOD -= 20;
            break;
        case 0x79: /* BIT 7,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x80;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
            M_CYCLES += 5;
            T_STATES += 20;
            T_PERIOD -= 20;
            break;
        case 0x7a: /* BIT 7,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x80;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
            M_CYCLES += 5;
            T_STATES += 20;
            T_PERIOD -= 20;
            break;
        case 0x7b: /* BIT 7,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x80;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
            M_CYCLES += 5;
            T_STATES += 20;
            T_PERIOD -= 20;
            break;
        case 0x7c: /* BIT 7,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x80;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
            M_CYCLES += 5;
            T_STATES += 20;
            T_PERIOD -= 20;
            break;
        case 0x7d: /* BIT 7,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x80;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
            M_CYCLES += 5;
            T_STATES += 20;
            T_PERIOD -= 20;
            break;
        case 0x7e: /* BIT 7,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x80;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
            M_CYCLES += 5;
            T_STATES += 20;
            T_PERIOD -= 20;
            break;
        case 0x7f: /* BIT 7,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x80;
            self->state.regs.AF.b.l = (T2.b.l & (_SF | _5F | _3F)) | PZSTable[T2.b.l] | (self->state.regs.AF.b.l & _CF) | _HF;
            M_CYCLES += 5;
            T_STATES += 20;
            T_PERIOD -= 20;
            break;
        case 0x80: /* LD B,RES 0,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0xfe;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.BC.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x81: /* LD C,RES 0,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0xfe;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.BC.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x82: /* LD D,RES 0,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0xfe;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.DE.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x83: /* LD E,RES 0,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0xfe;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.DE.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x84: /* LD H,RES 0,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0xfe;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.HL.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x85: /* LD L,RES 0,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0xfe;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.HL.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x86: /* RES 0,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0xfe;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x87: /* LD A,RES 0,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0xfe;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.AF.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x88: /* LD B,RES 1,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0xfd;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.BC.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x89: /* LD C,RES 1,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0xfd;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.BC.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x8a: /* LD D,RES 1,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0xfd;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.DE.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x8b: /* LD E,RES 1,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0xfd;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.DE.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x8c: /* LD H,RES 1,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0xfd;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.HL.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x8d: /* LD L,RES 1,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0xfd;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.HL.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x8e: /* RES 1,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0xfd;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x8f: /* LD A,RES 1,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0xfd;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.AF.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x90: /* LD B,RES 2,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0xfb;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.BC.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x91: /* LD C,RES 2,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0xfb;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.BC.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x92: /* LD D,RES 2,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0xfb;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.DE.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x93: /* LD E,RES 2,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0xfb;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.DE.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x94: /* LD H,RES 2,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0xfb;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.HL.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x95: /* LD L,RES 2,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0xfb;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.HL.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x96: /* RES 2,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0xfb;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x97: /* LD A,RES 2,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0xfb;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.AF.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x98: /* LD B,RES 3,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0xf7;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.BC.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x99: /* LD C,RES 3,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0xf7;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.BC.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x9a: /* LD D,RES 3,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0xf7;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.DE.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x9b: /* LD E,RES 3,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0xf7;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.DE.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x9c: /* LD H,RES 3,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0xf7;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.HL.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x9d: /* LD L,RES 3,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0xf7;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.HL.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x9e: /* RES 3,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0xf7;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0x9f: /* LD A,RES 3,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0xf7;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.AF.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xa0: /* LD B,RES 4,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0xef;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.BC.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xa1: /* LD C,RES 4,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0xef;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.BC.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xa2: /* LD D,RES 4,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0xef;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.DE.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xa3: /* LD E,RES 4,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0xef;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.DE.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xa4: /* LD H,RES 4,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0xef;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.HL.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xa5: /* LD L,RES 4,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0xef;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.HL.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xa6: /* RES 4,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0xef;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xa7: /* LD A,RES 4,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0xef;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.AF.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xa8: /* LD B,RES 5,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0xdf;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.BC.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xa9: /* LD C,RES 5,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0xdf;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.BC.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xaa: /* LD D,RES 5,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0xdf;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.DE.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xab: /* LD E,RES 5,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0xdf;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.DE.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xac: /* LD H,RES 5,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0xdf;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.HL.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xad: /* LD L,RES 5,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0xdf;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.HL.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xae: /* RES 5,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0xdf;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xaf: /* LD A,RES 5,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0xdf;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.AF.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xb0: /* LD B,RES 6,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0xbf;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.BC.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xb1: /* LD C,RES 6,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0xbf;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.BC.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xb2: /* LD D,RES 6,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0xbf;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.DE.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xb3: /* LD E,RES 6,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0xbf;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.DE.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xb4: /* LD H,RES 6,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0xbf;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.HL.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xb5: /* LD L,RES 6,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0xbf;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.HL.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xb6: /* RES 6,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0xbf;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xb7: /* LD A,RES 6,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0xbf;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.AF.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xb8: /* LD B,RES 7,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x7f;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.BC.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xb9: /* LD C,RES 7,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x7f;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.BC.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xba: /* LD D,RES 7,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x7f;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.DE.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xbb: /* LD E,RES 7,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x7f;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.DE.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xbc: /* LD H,RES 7,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x7f;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.HL.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xbd: /* LD L,RES 7,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x7f;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.HL.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xbe: /* RES 7,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x7f;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xbf: /* LD A,RES 7,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l & 0x7f;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.AF.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xc0: /* LD B,SET 0,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l | 0x01;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.BC.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xc1: /* LD C,SET 0,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l | 0x01;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.BC.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xc2: /* LD D,SET 0,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l | 0x01;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.DE.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xc3: /* LD E,SET 0,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l | 0x01;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.DE.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xc4: /* LD H,SET 0,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l | 0x01;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.HL.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xc5: /* LD L,SET 0,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l | 0x01;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.HL.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xc6: /* SET 0,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l | 0x01;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xc7: /* LD A,SET 0,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l | 0x01;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.AF.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xc8: /* LD B,SET 1,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l | 0x02;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.BC.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xc9: /* LD C,SET 1,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l | 0x02;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.BC.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xca: /* LD D,SET 1,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l | 0x02;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.DE.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xcb: /* LD E,SET 1,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l | 0x02;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.DE.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xcc: /* LD H,SET 1,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l | 0x02;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.HL.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xcd: /* LD L,SET 1,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l | 0x02;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.HL.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xce: /* SET 1,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l | 0x02;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xcf: /* LD A,SET 1,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l | 0x02;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.AF.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xd0: /* LD B,SET 2,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l | 0x04;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.BC.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xd1: /* LD C,SET 2,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l | 0x04;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.BC.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xd2: /* LD D,SET 2,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l | 0x04;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.DE.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xd3: /* LD E,SET 2,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l | 0x04;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.DE.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xd4: /* LD H,SET 2,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l | 0x04;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.HL.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xd5: /* LD L,SET 2,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l | 0x04;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.HL.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xd6: /* SET 2,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l | 0x04;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xd7: /* LD A,SET 2,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l | 0x04;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.AF.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xd8: /* LD B,SET 3,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l | 0x08;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.BC.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xd9: /* LD C,SET 3,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l | 0x08;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.BC.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xda: /* LD D,SET 3,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l | 0x08;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.DE.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xdb: /* LD E,SET 3,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l | 0x08;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.DE.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xdc: /* LD H,SET 3,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l | 0x08;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.HL.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xdd: /* LD L,SET 3,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l | 0x08;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.HL.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xde: /* SET 3,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l | 0x08;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xdf: /* LD A,SET 3,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l | 0x08;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.AF.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xe0: /* LD B,SET 4,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l | 0x10;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.BC.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xe1: /* LD C,SET 4,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l | 0x10;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.BC.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xe2: /* LD D,SET 4,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l | 0x10;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.DE.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xe3: /* LD E,SET 4,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l | 0x10;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.DE.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xe4: /* LD H,SET 4,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l | 0x10;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.HL.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xe5: /* LD L,SET 4,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l | 0x10;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.HL.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xe6: /* SET 4,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l | 0x10;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xe7: /* LD A,SET 4,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l | 0x10;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.AF.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xe8: /* LD B,SET 5,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l | 0x20;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.BC.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xe9: /* LD C,SET 5,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l | 0x20;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.BC.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xea: /* LD D,SET 5,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l | 0x20;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.DE.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xeb: /* LD E,SET 5,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l | 0x20;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.DE.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xec: /* LD H,SET 5,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l | 0x20;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.HL.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xed: /* LD L,SET 5,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l | 0x20;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.HL.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xee: /* SET 5,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l | 0x20;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xef: /* LD A,SET 5,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l | 0x20;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.AF.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xf0: /* LD B,SET 6,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l | 0x40;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.BC.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xf1: /* LD C,SET 6,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l | 0x40;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.BC.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xf2: /* LD D,SET 6,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l | 0x40;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.DE.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xf3: /* LD E,SET 6,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l | 0x40;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.DE.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xf4: /* LD H,SET 6,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l | 0x40;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.HL.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xf5: /* LD L,SET 6,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l | 0x40;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.HL.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xf6: /* SET 6,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l | 0x40;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xf7: /* LD A,SET 6,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l | 0x40;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.AF.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xf8: /* LD B,SET 7,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l | 0x80;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.BC.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xf9: /* LD C,SET 7,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l | 0x80;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.BC.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xfa: /* LD D,SET 7,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l | 0x80;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.DE.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xfb: /* LD E,SET 7,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l | 0x80;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.DE.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xfc: /* LD H,SET 7,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l | 0x80;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.HL.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xfd: /* LD L,SET 7,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l | 0x80;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.HL.b.l = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xfe: /* SET 7,(IX+d)      */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l | 0x80;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
        case 0xff: /* LD A,SET 7,(IX+d) */
            T1.b.l = (*self->iface.mreq_rd)(self, WZ.w.l);
            T2.b.l = T1.b.l | 0x80;
            (*self->iface.mreq_wr)(self, WZ.w.l, T2.b.l);
            self->state.regs.AF.b.h = T2.b.l;
            M_CYCLES += 6;
            T_STATES += 23;
            T_PERIOD -= 23;
            break;
