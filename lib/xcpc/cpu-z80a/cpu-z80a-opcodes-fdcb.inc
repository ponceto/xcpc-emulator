        case 0x00: /* LD B,RLC (IY+d)   */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = (T1_L << 1) | (T1_L >> 7);
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L];
            if(T1_L & 0x80) {
                AF_L |= CF;
            }
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            BC_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x01: /* LD C,RLC (IY+d)   */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = (T1_L << 1) | (T1_L >> 7);
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L];
            if(T1_L & 0x80) {
                AF_L |= CF;
            }
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            BC_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x02: /* LD D,RLC (IY+d)   */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = (T1_L << 1) | (T1_L >> 7);
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L];
            if(T1_L & 0x80) {
                AF_L |= CF;
            }
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            DE_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x03: /* LD E,RLC (IY+d)   */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = (T1_L << 1) | (T1_L >> 7);
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L];
            if(T1_L & 0x80) {
                AF_L |= CF;
            }
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            DE_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x04: /* LD H,RLC (IY+d)   */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = (T1_L << 1) | (T1_L >> 7);
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L];
            if(T1_L & 0x80) {
                AF_L |= CF;
            }
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            HL_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x05: /* LD L,RLC (IY+d)   */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = (T1_L << 1) | (T1_L >> 7);
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L];
            if(T1_L & 0x80) {
                AF_L |= CF;
            }
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            HL_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x06: /* RLC (IY+d)        */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = (T1_L << 1) | (T1_L >> 7);
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L];
            if(T1_L & 0x80) {
                AF_L |= CF;
            }
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x07: /* LD A,RLC (IY+d)   */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = (T1_L << 1) | (T1_L >> 7);
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L];
            if(T1_L & 0x80) {
                AF_L |= CF;
            }
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            AF_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x08: /* LD B,RRC (IY+d)   */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = (T1_L >> 1) | (T1_L << 7);
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L];
            if(T1_L & 0x01) {
                AF_L |= CF;
            }
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            BC_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x09: /* LD C,RRC (IY+d)   */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = (T1_L >> 1) | (T1_L << 7);
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L];
            if(T1_L & 0x01) {
                AF_L |= CF;
            }
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            BC_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x0a: /* LD D,RRC (IY+d)   */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = (T1_L >> 1) | (T1_L << 7);
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L];
            if(T1_L & 0x01) {
                AF_L |= CF;
            }
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            DE_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x0b: /* LD E,RRC (IY+d)   */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = (T1_L >> 1) | (T1_L << 7);
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L];
            if(T1_L & 0x01) {
                AF_L |= CF;
            }
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            DE_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x0c: /* LD H,RRC (IY+d)   */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = (T1_L >> 1) | (T1_L << 7);
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L];
            if(T1_L & 0x01) {
                AF_L |= CF;
            }
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            HL_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x0d: /* LD L,RRC (IY+d)   */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = (T1_L >> 1) | (T1_L << 7);
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L];
            if(T1_L & 0x01) {
                AF_L |= CF;
            }
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            HL_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x0e: /* RRC (IY+d)        */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = (T1_L >> 1) | (T1_L << 7);
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L];
            if(T1_L & 0x01) {
                AF_L |= CF;
            }
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x0f: /* LD A,RRC (IY+d)   */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = (T1_L >> 1) | (T1_L << 7);
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L];
            if(T1_L & 0x01) {
                AF_L |= CF;
            }
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            AF_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x10: /* LD B,RL  (IY+d)   */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L << 1;
            if(AF_L & CF) {
                T2_L |= 0x01;
            }
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L];
            if(T1_L & 0x80) {
                AF_L |= CF;
            }
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            BC_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x11: /* LD C,RL  (IY+d)   */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L << 1;
            if(AF_L & CF) {
                T2_L |= 0x01;
            }
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L];
            if(T1_L & 0x80) {
                AF_L |= CF;
            }
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            BC_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x12: /* LD D,RL  (IY+d)   */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L << 1;
            if(AF_L & CF) {
                T2_L |= 0x01;
            }
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L];
            if(T1_L & 0x80) {
                AF_L |= CF;
            }
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            DE_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x13: /* LD E,RL  (IY+d)   */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L << 1;
            if(AF_L & CF) {
                T2_L |= 0x01;
            }
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L];
            if(T1_L & 0x80) {
                AF_L |= CF;
            }
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            DE_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x14: /* LD H,RL  (IY+d)   */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L << 1;
            if(AF_L & CF) {
                T2_L |= 0x01;
            }
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L];
            if(T1_L & 0x80) {
                AF_L |= CF;
            }
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            HL_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x15: /* LD L,RL  (IY+d)   */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L << 1;
            if(AF_L & CF) {
                T2_L |= 0x01;
            }
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L];
            if(T1_L & 0x80) {
                AF_L |= CF;
            }
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            HL_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x16: /* RL  (IY+d)        */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L << 1;
            if(AF_L & CF) {
                T2_L |= 0x01;
            }
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L];
            if(T1_L & 0x80) {
                AF_L |= CF;
            }
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x17: /* LD A,RL  (IY+d)   */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L << 1;
            if(AF_L & CF) {
                T2_L |= 0x01;
            }
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L];
            if(T1_L & 0x80) {
                AF_L |= CF;
            }
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            AF_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x18: /* LD B,RR  (IY+d)   */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L >> 1;
            if(AF_L & CF) {
                T2_L |= 0x80;
            }
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L];
            if(T1_L & 0x01) {
                AF_L |= CF;
            }
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            BC_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x19: /* LD C,RR  (IY+d)   */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L >> 1;
            if(AF_L & CF) {
                T2_L |= 0x80;
            }
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L];
            if(T1_L & 0x01) {
                AF_L |= CF;
            }
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            BC_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x1a: /* LD D,RR  (IY+d)   */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L >> 1;
            if(AF_L & CF) {
                T2_L |= 0x80;
            }
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L];
            if(T1_L & 0x01) {
                AF_L |= CF;
            }
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            DE_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x1b: /* LD E,RR  (IY+d)   */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L >> 1;
            if(AF_L & CF) {
                T2_L |= 0x80;
            }
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L];
            if(T1_L & 0x01) {
                AF_L |= CF;
            }
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            DE_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x1c: /* LD H,RR  (IY+d)   */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L >> 1;
            if(AF_L & CF) {
                T2_L |= 0x80;
            }
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L];
            if(T1_L & 0x01) {
                AF_L |= CF;
            }
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            HL_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x1d: /* LD L,RR  (IY+d)   */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L >> 1;
            if(AF_L & CF) {
                T2_L |= 0x80;
            }
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L];
            if(T1_L & 0x01) {
                AF_L |= CF;
            }
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            HL_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x1e: /* RR  (IY+d)        */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L >> 1;
            if(AF_L & CF) {
                T2_L |= 0x80;
            }
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L];
            if(T1_L & 0x01) {
                AF_L |= CF;
            }
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x1f: /* LD A,RR  (IY+d)   */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L >> 1;
            if(AF_L & CF) {
                T2_L |= 0x80;
            }
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L];
            if(T1_L & 0x01) {
                AF_L |= CF;
            }
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            AF_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x20: /* LD B,SLA (IY+d)   */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = ((int8_t) T1_L) << 1;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L];
            if(T1_L & 0x80) {
                AF_L |= CF;
            }
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            BC_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x21: /* LD C,SLA (IY+d)   */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = ((int8_t) T1_L) << 1;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L];
            if(T1_L & 0x80) {
                AF_L |= CF;
            }
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            BC_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x22: /* LD D,SLA (IY+d)   */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = ((int8_t) T1_L) << 1;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L];
            if(T1_L & 0x80) {
                AF_L |= CF;
            }
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            DE_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x23: /* LD E,SLA (IY+d)   */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = ((int8_t) T1_L) << 1;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L];
            if(T1_L & 0x80) {
                AF_L |= CF;
            }
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            DE_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x24: /* LD H,SLA (IY+d)   */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = ((int8_t) T1_L) << 1;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L];
            if(T1_L & 0x80) {
                AF_L |= CF;
            }
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            HL_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x25: /* LD L,SLA (IY+d)   */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = ((int8_t) T1_L) << 1;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L];
            if(T1_L & 0x80) {
                AF_L |= CF;
            }
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            HL_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x26: /* SLA (IY+d)        */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = ((int8_t) T1_L) << 1;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L];
            if(T1_L & 0x80) {
                AF_L |= CF;
            }
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x27: /* LD A,SLA (IY+d)   */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = ((int8_t) T1_L) << 1;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L];
            if(T1_L & 0x80) {
                AF_L |= CF;
            }
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            AF_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x28: /* LD B,SRA (IY+d)   */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = ((int8_t) T1_L) >> 1;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L];
            if(T1_L & 0x01) {
                AF_L |= CF;
            }
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            BC_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x29: /* LD C,SRA (IY+d)   */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = ((int8_t) T1_L) >> 1;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L];
            if(T1_L & 0x01) {
                AF_L |= CF;
            }
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            BC_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x2a: /* LD D,SRA (IY+d)   */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = ((int8_t) T1_L) >> 1;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L];
            if(T1_L & 0x01) {
                AF_L |= CF;
            }
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            DE_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x2b: /* LD E,SRA (IY+d)   */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = ((int8_t) T1_L) >> 1;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L];
            if(T1_L & 0x01) {
                AF_L |= CF;
            }
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            DE_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x2c: /* LD H,SRA (IY+d)   */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = ((int8_t) T1_L) >> 1;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L];
            if(T1_L & 0x01) {
                AF_L |= CF;
            }
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            HL_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x2d: /* LD L,SRA (IY+d)   */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = ((int8_t) T1_L) >> 1;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L];
            if(T1_L & 0x01) {
                AF_L |= CF;
            }
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            HL_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x2e: /* SRA (IY+d)        */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = ((int8_t) T1_L) >> 1;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L];
            if(T1_L & 0x01) {
                AF_L |= CF;
            }
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x2f: /* LD A,SRA (IY+d)   */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = ((int8_t) T1_L) >> 1;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L];
            if(T1_L & 0x01) {
                AF_L |= CF;
            }
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            AF_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x30: /* LD B,SLL (IY+d)   */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = (T1_L << 1) | 0x01;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L];
            if(T1_L & 0x80) {
                AF_L |= CF;
            }
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            BC_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x31: /* LD C,SLL (IY+d)   */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = (T1_L << 1) | 0x01;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L];
            if(T1_L & 0x80) {
                AF_L |= CF;
            }
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            BC_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x32: /* LD D,SLL (IY+d)   */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = (T1_L << 1) | 0x01;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L];
            if(T1_L & 0x80) {
                AF_L |= CF;
            }
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            DE_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x33: /* LD E,SLL (IY+d)   */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = (T1_L << 1) | 0x01;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L];
            if(T1_L & 0x80) {
                AF_L |= CF;
            }
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            DE_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x34: /* LD H,SLL (IY+d)   */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = (T1_L << 1) | 0x01;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L];
            if(T1_L & 0x80) {
                AF_L |= CF;
            }
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            HL_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x35: /* LD L,SLL (IY+d)   */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = (T1_L << 1) | 0x01;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L];
            if(T1_L & 0x80) {
                AF_L |= CF;
            }
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            HL_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x36: /* SLL (IY+d)        */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = (T1_L << 1) | 0x01;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L];
            if(T1_L & 0x80) {
                AF_L |= CF;
            }
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x37: /* LD A,SLL (IY+d)   */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = (T1_L << 1) | 0x01;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L];
            if(T1_L & 0x80) {
                AF_L |= CF;
            }
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            AF_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x38: /* LD B,SRL (IY+d)   */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = (T1_L >> 1) & 0x7f;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L];
            if(T1_L & 0x01) {
                AF_L |= CF;
            }
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            BC_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x39: /* LD C,SRL (IY+d)   */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = (T1_L >> 1) & 0x7f;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L];
            if(T1_L & 0x01) {
                AF_L |= CF;
            }
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            BC_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x3a: /* LD D,SRL (IY+d)   */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = (T1_L >> 1) & 0x7f;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L];
            if(T1_L & 0x01) {
                AF_L |= CF;
            }
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            DE_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x3b: /* LD E,SRL (IY+d)   */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = (T1_L >> 1) & 0x7f;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L];
            if(T1_L & 0x01) {
                AF_L |= CF;
            }
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            DE_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x3c: /* LD H,SRL (IY+d)   */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = (T1_L >> 1) & 0x7f;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L];
            if(T1_L & 0x01) {
                AF_L |= CF;
            }
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            HL_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x3d: /* LD L,SRL (IY+d)   */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = (T1_L >> 1) & 0x7f;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L];
            if(T1_L & 0x01) {
                AF_L |= CF;
            }
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            HL_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x3e: /* SRL (IY+d)        */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = (T1_L >> 1) & 0x7f;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L];
            if(T1_L & 0x01) {
                AF_L |= CF;
            }
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x3f: /* LD A,SRL (IY+d)   */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = (T1_L >> 1) & 0x7f;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L];
            if(T1_L & 0x01) {
                AF_L |= CF;
            }
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            AF_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x40: /* BIT 0,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x01;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L] | (AF_L & CF) | HF;
            M_CYCLES += 5;
            T_STATES += 20;
            I_PERIOD -= 20;
            break;
        case 0x41: /* BIT 0,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x01;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L] | (AF_L & CF) | HF;
            M_CYCLES += 5;
            T_STATES += 20;
            I_PERIOD -= 20;
            break;
        case 0x42: /* BIT 0,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x01;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L] | (AF_L & CF) | HF;
            M_CYCLES += 5;
            T_STATES += 20;
            I_PERIOD -= 20;
            break;
        case 0x43: /* BIT 0,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x01;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L] | (AF_L & CF) | HF;
            M_CYCLES += 5;
            T_STATES += 20;
            I_PERIOD -= 20;
            break;
        case 0x44: /* BIT 0,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x01;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L] | (AF_L & CF) | HF;
            M_CYCLES += 5;
            T_STATES += 20;
            I_PERIOD -= 20;
            break;
        case 0x45: /* BIT 0,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x01;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L] | (AF_L & CF) | HF;
            M_CYCLES += 5;
            T_STATES += 20;
            I_PERIOD -= 20;
            break;
        case 0x46: /* BIT 0,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x01;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L] | (AF_L & CF) | HF;
            M_CYCLES += 5;
            T_STATES += 20;
            I_PERIOD -= 20;
            break;
        case 0x47: /* BIT 0,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x01;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L] | (AF_L & CF) | HF;
            M_CYCLES += 5;
            T_STATES += 20;
            I_PERIOD -= 20;
            break;
        case 0x48: /* BIT 1,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x02;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L] | (AF_L & CF) | HF;
            M_CYCLES += 5;
            T_STATES += 20;
            I_PERIOD -= 20;
            break;
        case 0x49: /* BIT 1,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x02;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L] | (AF_L & CF) | HF;
            M_CYCLES += 5;
            T_STATES += 20;
            I_PERIOD -= 20;
            break;
        case 0x4a: /* BIT 1,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x02;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L] | (AF_L & CF) | HF;
            M_CYCLES += 5;
            T_STATES += 20;
            I_PERIOD -= 20;
            break;
        case 0x4b: /* BIT 1,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x02;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L] | (AF_L & CF) | HF;
            M_CYCLES += 5;
            T_STATES += 20;
            I_PERIOD -= 20;
            break;
        case 0x4c: /* BIT 1,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x02;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L] | (AF_L & CF) | HF;
            M_CYCLES += 5;
            T_STATES += 20;
            I_PERIOD -= 20;
            break;
        case 0x4d: /* BIT 1,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x02;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L] | (AF_L & CF) | HF;
            M_CYCLES += 5;
            T_STATES += 20;
            I_PERIOD -= 20;
            break;
        case 0x4e: /* BIT 1,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x02;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L] | (AF_L & CF) | HF;
            M_CYCLES += 5;
            T_STATES += 20;
            I_PERIOD -= 20;
            break;
        case 0x4f: /* BIT 1,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x02;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L] | (AF_L & CF) | HF;
            M_CYCLES += 5;
            T_STATES += 20;
            I_PERIOD -= 20;
            break;
        case 0x50: /* BIT 2,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x04;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L] | (AF_L & CF) | HF;
            M_CYCLES += 5;
            T_STATES += 20;
            I_PERIOD -= 20;
            break;
        case 0x51: /* BIT 2,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x04;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L] | (AF_L & CF) | HF;
            M_CYCLES += 5;
            T_STATES += 20;
            I_PERIOD -= 20;
            break;
        case 0x52: /* BIT 2,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x04;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L] | (AF_L & CF) | HF;
            M_CYCLES += 5;
            T_STATES += 20;
            I_PERIOD -= 20;
            break;
        case 0x53: /* BIT 2,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x04;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L] | (AF_L & CF) | HF;
            M_CYCLES += 5;
            T_STATES += 20;
            I_PERIOD -= 20;
            break;
        case 0x54: /* BIT 2,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x04;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L] | (AF_L & CF) | HF;
            M_CYCLES += 5;
            T_STATES += 20;
            I_PERIOD -= 20;
            break;
        case 0x55: /* BIT 2,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x04;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L] | (AF_L & CF) | HF;
            M_CYCLES += 5;
            T_STATES += 20;
            I_PERIOD -= 20;
            break;
        case 0x56: /* BIT 2,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x04;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L] | (AF_L & CF) | HF;
            M_CYCLES += 5;
            T_STATES += 20;
            I_PERIOD -= 20;
            break;
        case 0x57: /* BIT 2,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x04;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L] | (AF_L & CF) | HF;
            M_CYCLES += 5;
            T_STATES += 20;
            I_PERIOD -= 20;
            break;
        case 0x58: /* BIT 3,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x08;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L] | (AF_L & CF) | HF;
            M_CYCLES += 5;
            T_STATES += 20;
            I_PERIOD -= 20;
            break;
        case 0x59: /* BIT 3,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x08;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L] | (AF_L & CF) | HF;
            M_CYCLES += 5;
            T_STATES += 20;
            I_PERIOD -= 20;
            break;
        case 0x5a: /* BIT 3,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x08;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L] | (AF_L & CF) | HF;
            M_CYCLES += 5;
            T_STATES += 20;
            I_PERIOD -= 20;
            break;
        case 0x5b: /* BIT 3,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x08;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L] | (AF_L & CF) | HF;
            M_CYCLES += 5;
            T_STATES += 20;
            I_PERIOD -= 20;
            break;
        case 0x5c: /* BIT 3,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x08;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L] | (AF_L & CF) | HF;
            M_CYCLES += 5;
            T_STATES += 20;
            I_PERIOD -= 20;
            break;
        case 0x5d: /* BIT 3,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x08;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L] | (AF_L & CF) | HF;
            M_CYCLES += 5;
            T_STATES += 20;
            I_PERIOD -= 20;
            break;
        case 0x5e: /* BIT 3,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x08;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L] | (AF_L & CF) | HF;
            M_CYCLES += 5;
            T_STATES += 20;
            I_PERIOD -= 20;
            break;
        case 0x5f: /* BIT 3,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x08;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L] | (AF_L & CF) | HF;
            M_CYCLES += 5;
            T_STATES += 20;
            I_PERIOD -= 20;
            break;
        case 0x60: /* BIT 4,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x10;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L] | (AF_L & CF) | HF;
            M_CYCLES += 5;
            T_STATES += 20;
            I_PERIOD -= 20;
            break;
        case 0x61: /* BIT 4,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x10;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L] | (AF_L & CF) | HF;
            M_CYCLES += 5;
            T_STATES += 20;
            I_PERIOD -= 20;
            break;
        case 0x62: /* BIT 4,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x10;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L] | (AF_L & CF) | HF;
            M_CYCLES += 5;
            T_STATES += 20;
            I_PERIOD -= 20;
            break;
        case 0x63: /* BIT 4,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x10;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L] | (AF_L & CF) | HF;
            M_CYCLES += 5;
            T_STATES += 20;
            I_PERIOD -= 20;
            break;
        case 0x64: /* BIT 4,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x10;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L] | (AF_L & CF) | HF;
            M_CYCLES += 5;
            T_STATES += 20;
            I_PERIOD -= 20;
            break;
        case 0x65: /* BIT 4,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x10;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L] | (AF_L & CF) | HF;
            M_CYCLES += 5;
            T_STATES += 20;
            I_PERIOD -= 20;
            break;
        case 0x66: /* BIT 4,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x10;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L] | (AF_L & CF) | HF;
            M_CYCLES += 5;
            T_STATES += 20;
            I_PERIOD -= 20;
            break;
        case 0x67: /* BIT 4,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x10;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L] | (AF_L & CF) | HF;
            M_CYCLES += 5;
            T_STATES += 20;
            I_PERIOD -= 20;
            break;
        case 0x68: /* BIT 5,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x20;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L] | (AF_L & CF) | HF;
            M_CYCLES += 5;
            T_STATES += 20;
            I_PERIOD -= 20;
            break;
        case 0x69: /* BIT 5,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x20;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L] | (AF_L & CF) | HF;
            M_CYCLES += 5;
            T_STATES += 20;
            I_PERIOD -= 20;
            break;
        case 0x6a: /* BIT 5,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x20;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L] | (AF_L & CF) | HF;
            M_CYCLES += 5;
            T_STATES += 20;
            I_PERIOD -= 20;
            break;
        case 0x6b: /* BIT 5,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x20;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L] | (AF_L & CF) | HF;
            M_CYCLES += 5;
            T_STATES += 20;
            I_PERIOD -= 20;
            break;
        case 0x6c: /* BIT 5,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x20;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L] | (AF_L & CF) | HF;
            M_CYCLES += 5;
            T_STATES += 20;
            I_PERIOD -= 20;
            break;
        case 0x6d: /* BIT 5,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x20;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L] | (AF_L & CF) | HF;
            M_CYCLES += 5;
            T_STATES += 20;
            I_PERIOD -= 20;
            break;
        case 0x6e: /* BIT 5,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x20;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L] | (AF_L & CF) | HF;
            M_CYCLES += 5;
            T_STATES += 20;
            I_PERIOD -= 20;
            break;
        case 0x6f: /* BIT 5,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x20;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L] | (AF_L & CF) | HF;
            M_CYCLES += 5;
            T_STATES += 20;
            I_PERIOD -= 20;
            break;
        case 0x70: /* BIT 6,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x40;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L] | (AF_L & CF) | HF;
            M_CYCLES += 5;
            T_STATES += 20;
            I_PERIOD -= 20;
            break;
        case 0x71: /* BIT 6,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x40;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L] | (AF_L & CF) | HF;
            M_CYCLES += 5;
            T_STATES += 20;
            I_PERIOD -= 20;
            break;
        case 0x72: /* BIT 6,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x40;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L] | (AF_L & CF) | HF;
            M_CYCLES += 5;
            T_STATES += 20;
            I_PERIOD -= 20;
            break;
        case 0x73: /* BIT 6,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x40;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L] | (AF_L & CF) | HF;
            M_CYCLES += 5;
            T_STATES += 20;
            I_PERIOD -= 20;
            break;
        case 0x74: /* BIT 6,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x40;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L] | (AF_L & CF) | HF;
            M_CYCLES += 5;
            T_STATES += 20;
            I_PERIOD -= 20;
            break;
        case 0x75: /* BIT 6,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x40;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L] | (AF_L & CF) | HF;
            M_CYCLES += 5;
            T_STATES += 20;
            I_PERIOD -= 20;
            break;
        case 0x76: /* BIT 6,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x40;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L] | (AF_L & CF) | HF;
            M_CYCLES += 5;
            T_STATES += 20;
            I_PERIOD -= 20;
            break;
        case 0x77: /* BIT 6,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x40;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L] | (AF_L & CF) | HF;
            M_CYCLES += 5;
            T_STATES += 20;
            I_PERIOD -= 20;
            break;
        case 0x78: /* BIT 7,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x80;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L] | (AF_L & CF) | HF;
            M_CYCLES += 5;
            T_STATES += 20;
            I_PERIOD -= 20;
            break;
        case 0x79: /* BIT 7,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x80;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L] | (AF_L & CF) | HF;
            M_CYCLES += 5;
            T_STATES += 20;
            I_PERIOD -= 20;
            break;
        case 0x7a: /* BIT 7,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x80;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L] | (AF_L & CF) | HF;
            M_CYCLES += 5;
            T_STATES += 20;
            I_PERIOD -= 20;
            break;
        case 0x7b: /* BIT 7,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x80;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L] | (AF_L & CF) | HF;
            M_CYCLES += 5;
            T_STATES += 20;
            I_PERIOD -= 20;
            break;
        case 0x7c: /* BIT 7,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x80;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L] | (AF_L & CF) | HF;
            M_CYCLES += 5;
            T_STATES += 20;
            I_PERIOD -= 20;
            break;
        case 0x7d: /* BIT 7,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x80;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L] | (AF_L & CF) | HF;
            M_CYCLES += 5;
            T_STATES += 20;
            I_PERIOD -= 20;
            break;
        case 0x7e: /* BIT 7,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x80;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L] | (AF_L & CF) | HF;
            M_CYCLES += 5;
            T_STATES += 20;
            I_PERIOD -= 20;
            break;
        case 0x7f: /* BIT 7,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x80;
            AF_L = (T2_L & (SF | YF | XF)) | PZSTable[T2_L] | (AF_L & CF) | HF;
            M_CYCLES += 5;
            T_STATES += 20;
            I_PERIOD -= 20;
            break;
        case 0x80: /* LD B,RES 0,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0xfe;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            BC_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x81: /* LD C,RES 0,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0xfe;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            BC_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x82: /* LD D,RES 0,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0xfe;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            DE_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x83: /* LD E,RES 0,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0xfe;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            DE_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x84: /* LD H,RES 0,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0xfe;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            HL_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x85: /* LD L,RES 0,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0xfe;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            HL_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x86: /* RES 0,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0xfe;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x87: /* LD A,RES 0,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0xfe;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            AF_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x88: /* LD B,RES 1,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0xfd;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            BC_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x89: /* LD C,RES 1,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0xfd;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            BC_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x8a: /* LD D,RES 1,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0xfd;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            DE_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x8b: /* LD E,RES 1,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0xfd;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            DE_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x8c: /* LD H,RES 1,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0xfd;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            HL_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x8d: /* LD L,RES 1,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0xfd;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            HL_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x8e: /* RES 1,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0xfd;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x8f: /* LD A,RES 1,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0xfd;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            AF_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x90: /* LD B,RES 2,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0xfb;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            BC_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x91: /* LD C,RES 2,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0xfb;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            BC_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x92: /* LD D,RES 2,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0xfb;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            DE_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x93: /* LD E,RES 2,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0xfb;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            DE_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x94: /* LD H,RES 2,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0xfb;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            HL_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x95: /* LD L,RES 2,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0xfb;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            HL_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x96: /* RES 2,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0xfb;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x97: /* LD A,RES 2,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0xfb;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            AF_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x98: /* LD B,RES 3,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0xf7;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            BC_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x99: /* LD C,RES 3,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0xf7;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            BC_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x9a: /* LD D,RES 3,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0xf7;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            DE_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x9b: /* LD E,RES 3,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0xf7;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            DE_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x9c: /* LD H,RES 3,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0xf7;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            HL_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x9d: /* LD L,RES 3,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0xf7;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            HL_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x9e: /* RES 3,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0xf7;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0x9f: /* LD A,RES 3,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0xf7;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            AF_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xa0: /* LD B,RES 4,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0xef;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            BC_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xa1: /* LD C,RES 4,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0xef;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            BC_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xa2: /* LD D,RES 4,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0xef;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            DE_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xa3: /* LD E,RES 4,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0xef;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            DE_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xa4: /* LD H,RES 4,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0xef;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            HL_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xa5: /* LD L,RES 4,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0xef;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            HL_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xa6: /* RES 4,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0xef;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xa7: /* LD A,RES 4,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0xef;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            AF_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xa8: /* LD B,RES 5,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0xdf;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            BC_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xa9: /* LD C,RES 5,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0xdf;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            BC_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xaa: /* LD D,RES 5,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0xdf;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            DE_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xab: /* LD E,RES 5,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0xdf;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            DE_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xac: /* LD H,RES 5,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0xdf;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            HL_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xad: /* LD L,RES 5,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0xdf;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            HL_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xae: /* RES 5,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0xdf;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xaf: /* LD A,RES 5,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0xdf;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            AF_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xb0: /* LD B,RES 6,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0xbf;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            BC_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xb1: /* LD C,RES 6,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0xbf;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            BC_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xb2: /* LD D,RES 6,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0xbf;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            DE_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xb3: /* LD E,RES 6,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0xbf;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            DE_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xb4: /* LD H,RES 6,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0xbf;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            HL_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xb5: /* LD L,RES 6,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0xbf;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            HL_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xb6: /* RES 6,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0xbf;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xb7: /* LD A,RES 6,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0xbf;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            AF_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xb8: /* LD B,RES 7,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x7f;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            BC_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xb9: /* LD C,RES 7,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x7f;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            BC_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xba: /* LD D,RES 7,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x7f;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            DE_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xbb: /* LD E,RES 7,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x7f;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            DE_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xbc: /* LD H,RES 7,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x7f;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            HL_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xbd: /* LD L,RES 7,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x7f;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            HL_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xbe: /* RES 7,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x7f;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xbf: /* LD A,RES 7,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L & 0x7f;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            AF_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xc0: /* LD B,SET 0,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L | 0x01;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            BC_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xc1: /* LD C,SET 0,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L | 0x01;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            BC_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xc2: /* LD D,SET 0,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L | 0x01;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            DE_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xc3: /* LD E,SET 0,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L | 0x01;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            DE_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xc4: /* LD H,SET 0,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L | 0x01;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            HL_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xc5: /* LD L,SET 0,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L | 0x01;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            HL_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xc6: /* SET 0,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L | 0x01;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xc7: /* LD A,SET 0,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L | 0x01;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            AF_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xc8: /* LD B,SET 1,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L | 0x02;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            BC_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xc9: /* LD C,SET 1,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L | 0x02;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            BC_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xca: /* LD D,SET 1,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L | 0x02;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            DE_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xcb: /* LD E,SET 1,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L | 0x02;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            DE_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xcc: /* LD H,SET 1,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L | 0x02;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            HL_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xcd: /* LD L,SET 1,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L | 0x02;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            HL_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xce: /* SET 1,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L | 0x02;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xcf: /* LD A,SET 1,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L | 0x02;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            AF_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xd0: /* LD B,SET 2,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L | 0x04;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            BC_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xd1: /* LD C,SET 2,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L | 0x04;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            BC_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xd2: /* LD D,SET 2,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L | 0x04;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            DE_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xd3: /* LD E,SET 2,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L | 0x04;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            DE_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xd4: /* LD H,SET 2,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L | 0x04;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            HL_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xd5: /* LD L,SET 2,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L | 0x04;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            HL_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xd6: /* SET 2,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L | 0x04;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xd7: /* LD A,SET 2,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L | 0x04;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            AF_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xd8: /* LD B,SET 3,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L | 0x08;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            BC_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xd9: /* LD C,SET 3,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L | 0x08;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            BC_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xda: /* LD D,SET 3,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L | 0x08;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            DE_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xdb: /* LD E,SET 3,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L | 0x08;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            DE_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xdc: /* LD H,SET 3,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L | 0x08;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            HL_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xdd: /* LD L,SET 3,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L | 0x08;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            HL_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xde: /* SET 3,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L | 0x08;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xdf: /* LD A,SET 3,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L | 0x08;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            AF_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xe0: /* LD B,SET 4,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L | 0x10;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            BC_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xe1: /* LD C,SET 4,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L | 0x10;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            BC_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xe2: /* LD D,SET 4,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L | 0x10;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            DE_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xe3: /* LD E,SET 4,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L | 0x10;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            DE_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xe4: /* LD H,SET 4,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L | 0x10;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            HL_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xe5: /* LD L,SET 4,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L | 0x10;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            HL_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xe6: /* SET 4,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L | 0x10;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xe7: /* LD A,SET 4,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L | 0x10;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            AF_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xe8: /* LD B,SET 5,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L | 0x20;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            BC_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xe9: /* LD C,SET 5,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L | 0x20;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            BC_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xea: /* LD D,SET 5,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L | 0x20;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            DE_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xeb: /* LD E,SET 5,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L | 0x20;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            DE_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xec: /* LD H,SET 5,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L | 0x20;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            HL_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xed: /* LD L,SET 5,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L | 0x20;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            HL_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xee: /* SET 5,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L | 0x20;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xef: /* LD A,SET 5,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L | 0x20;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            AF_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xf0: /* LD B,SET 6,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L | 0x40;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            BC_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xf1: /* LD C,SET 6,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L | 0x40;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            BC_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xf2: /* LD D,SET 6,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L | 0x40;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            DE_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xf3: /* LD E,SET 6,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L | 0x40;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            DE_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xf4: /* LD H,SET 6,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L | 0x40;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            HL_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xf5: /* LD L,SET 6,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L | 0x40;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            HL_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xf6: /* SET 6,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L | 0x40;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xf7: /* LD A,SET 6,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L | 0x40;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            AF_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xf8: /* LD B,SET 7,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L | 0x80;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            BC_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xf9: /* LD C,SET 7,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L | 0x80;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            BC_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xfa: /* LD D,SET 7,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L | 0x80;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            DE_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xfb: /* LD E,SET 7,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L | 0x80;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            DE_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xfc: /* LD H,SET 7,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L | 0x80;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            HL_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xfd: /* LD L,SET 7,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L | 0x80;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            HL_L = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xfe: /* SET 7,(IY+d)      */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L | 0x80;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
        case 0xff: /* LD A,SET 7,(IY+d) */
            T1_L = (*IFACE.mreq_rd)(self, WZ_W);
            T2_L = T1_L | 0x80;
            (*IFACE.mreq_wr)(self, WZ_W, T2_L);
            AF_H = T2_L;
            M_CYCLES += 6;
            T_STATES += 23;
            I_PERIOD -= 23;
            break;
