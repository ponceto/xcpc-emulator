        case 0x00: /* RLC B             */
            T1_L = BC_H;
            T2_L = (T1_L << 1) | (T1_L >> 7);
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L];
            if(T1_L & 0x80) {
                AF_L |= _CF;
            }
            BC_H = T2_L;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x01: /* RLC C             */
            T1_L = BC_L;
            T2_L = (T1_L << 1) | (T1_L >> 7);
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L];
            if(T1_L & 0x80) {
                AF_L |= _CF;
            }
            BC_L = T2_L;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x02: /* RLC D             */
            T1_L = DE_H;
            T2_L = (T1_L << 1) | (T1_L >> 7);
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L];
            if(T1_L & 0x80) {
                AF_L |= _CF;
            }
            DE_H = T2_L;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x03: /* RLC E             */
            T1_L = DE_L;
            T2_L = (T1_L << 1) | (T1_L >> 7);
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L];
            if(T1_L & 0x80) {
                AF_L |= _CF;
            }
            DE_L = T2_L;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x04: /* RLC H             */
            T1_L = HL_H;
            T2_L = (T1_L << 1) | (T1_L >> 7);
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L];
            if(T1_L & 0x80) {
                AF_L |= _CF;
            }
            HL_H = T2_L;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x05: /* RLC L             */
            T1_L = HL_L;
            T2_L = (T1_L << 1) | (T1_L >> 7);
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L];
            if(T1_L & 0x80) {
                AF_L |= _CF;
            }
            HL_L = T2_L;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x06: /* RLC (HL)          */
            T1_L = (*IFACE.mreq_rd)(self, HL_W);
            T2_L = (T1_L << 1) | (T1_L >> 7);
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L];
            if(T1_L & 0x80) {
                AF_L |= _CF;
            }
            (*IFACE.mreq_wr)(self, HL_W, T2_L);
            M_CYCLES += 4;
            T_STATES += 15;
            T_PERIOD -= 15;
            break;
        case 0x07: /* RLC A             */
            T1_L = AF_H;
            T2_L = (T1_L << 1) | (T1_L >> 7);
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L];
            if(T1_L & 0x80) {
                AF_L |= _CF;
            }
            AF_H = T2_L;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x08: /* RRC B             */
            T1_L = BC_H;
            T2_L = (T1_L >> 1) | (T1_L << 7);
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L];
            if(T1_L & 0x01) {
                AF_L |= _CF;
            }
            BC_H = T2_L;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x09: /* RRC C             */
            T1_L = BC_L;
            T2_L = (T1_L >> 1) | (T1_L << 7);
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L];
            if(T1_L & 0x01) {
                AF_L |= _CF;
            }
            BC_L = T2_L;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x0a: /* RRC D             */
            T1_L = DE_H;
            T2_L = (T1_L >> 1) | (T1_L << 7);
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L];
            if(T1_L & 0x01) {
                AF_L |= _CF;
            }
            DE_H = T2_L;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x0b: /* RRC E             */
            T1_L = DE_L;
            T2_L = (T1_L >> 1) | (T1_L << 7);
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L];
            if(T1_L & 0x01) {
                AF_L |= _CF;
            }
            DE_L = T2_L;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x0c: /* RRC H             */
            T1_L = HL_H;
            T2_L = (T1_L >> 1) | (T1_L << 7);
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L];
            if(T1_L & 0x01) {
                AF_L |= _CF;
            }
            HL_H = T2_L;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x0d: /* RRC L             */
            T1_L = HL_L;
            T2_L = (T1_L >> 1) | (T1_L << 7);
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L];
            if(T1_L & 0x01) {
                AF_L |= _CF;
            }
            HL_L = T2_L;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x0e: /* RRC (HL)          */
            T1_L = (*IFACE.mreq_rd)(self, HL_W);
            T2_L = (T1_L >> 1) | (T1_L << 7);
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L];
            if(T1_L & 0x01) {
                AF_L |= _CF;
            }
            (*IFACE.mreq_wr)(self, HL_W, T2_L);
            M_CYCLES += 4;
            T_STATES += 15;
            T_PERIOD -= 15;
            break;
        case 0x0f: /* RRC A             */
            T1_L = AF_H;
            T2_L = (T1_L >> 1) | (T1_L << 7);
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L];
            if(T1_L & 0x01) {
                AF_L |= _CF;
            }
            AF_H = T2_L;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x10: /* RL  B             */
            T1_L = BC_H;
            T2_L = T1_L << 1;
            if(AF_L & _CF) {
                T2_L |= 0x01;
            }
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L];
            if(T1_L & 0x80) {
                AF_L |= _CF;
            }
            BC_H = T2_L;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x11: /* RL  C             */
            T1_L = BC_L;
            T2_L = T1_L << 1;
            if(AF_L & _CF) {
                T2_L |= 0x01;
            }
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L];
            if(T1_L & 0x80) {
                AF_L |= _CF;
            }
            BC_L = T2_L;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x12: /* RL  D             */
            T1_L = DE_H;
            T2_L = T1_L << 1;
            if(AF_L & _CF) {
                T2_L |= 0x01;
            }
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L];
            if(T1_L & 0x80) {
                AF_L |= _CF;
            }
            DE_H = T2_L;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x13: /* RL  E             */
            T1_L = DE_L;
            T2_L = T1_L << 1;
            if(AF_L & _CF) {
                T2_L |= 0x01;
            }
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L];
            if(T1_L & 0x80) {
                AF_L |= _CF;
            }
            DE_L = T2_L;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x14: /* RL  H             */
            T1_L = HL_H;
            T2_L = T1_L << 1;
            if(AF_L & _CF) {
                T2_L |= 0x01;
            }
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L];
            if(T1_L & 0x80) {
                AF_L |= _CF;
            }
            HL_H = T2_L;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x15: /* RL  L             */
            T1_L = HL_L;
            T2_L = T1_L << 1;
            if(AF_L & _CF) {
                T2_L |= 0x01;
            }
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L];
            if(T1_L & 0x80) {
                AF_L |= _CF;
            }
            HL_L = T2_L;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x16: /* RL  (HL)          */
            T1_L = (*IFACE.mreq_rd)(self, HL_W);
            T2_L = T1_L << 1;
            if(AF_L & _CF) {
                T2_L |= 0x01;
            }
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L];
            if(T1_L & 0x80) {
                AF_L |= _CF;
            }
            (*IFACE.mreq_wr)(self, HL_W, T2_L);
            M_CYCLES += 4;
            T_STATES += 15;
            T_PERIOD -= 15;
            break;
        case 0x17: /* RL  A             */
            T1_L = AF_H;
            T2_L = T1_L << 1;
            if(AF_L & _CF) {
                T2_L |= 0x01;
            }
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L];
            if(T1_L & 0x80) {
                AF_L |= _CF;
            }
            AF_H = T2_L;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x18: /* RR  B             */
            T1_L = BC_H;
            T2_L = T1_L >> 1;
            if(AF_L & _CF) {
                T2_L |= 0x80;
            }
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L];
            if(T1_L & 0x01) {
                AF_L |= _CF;
            }
            BC_H = T2_L;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x19: /* RR  C             */
            T1_L = BC_L;
            T2_L = T1_L >> 1;
            if(AF_L & _CF) {
                T2_L |= 0x80;
            }
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L];
            if(T1_L & 0x01) {
                AF_L |= _CF;
            }
            BC_L = T2_L;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x1a: /* RR  D             */
            T1_L = DE_H;
            T2_L = T1_L >> 1;
            if(AF_L & _CF) {
                T2_L |= 0x80;
            }
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L];
            if(T1_L & 0x01) {
                AF_L |= _CF;
            }
            DE_H = T2_L;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x1b: /* RR  E             */
            T1_L = DE_L;
            T2_L = T1_L >> 1;
            if(AF_L & _CF) {
                T2_L |= 0x80;
            }
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L];
            if(T1_L & 0x01) {
                AF_L |= _CF;
            }
            DE_L = T2_L;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x1c: /* RR  H             */
            T1_L = HL_H;
            T2_L = T1_L >> 1;
            if(AF_L & _CF) {
                T2_L |= 0x80;
            }
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L];
            if(T1_L & 0x01) {
                AF_L |= _CF;
            }
            HL_H = T2_L;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x1d: /* RR  L             */
            T1_L = HL_L;
            T2_L = T1_L >> 1;
            if(AF_L & _CF) {
                T2_L |= 0x80;
            }
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L];
            if(T1_L & 0x01) {
                AF_L |= _CF;
            }
            HL_L = T2_L;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x1e: /* RR  (HL)          */
            T1_L = (*IFACE.mreq_rd)(self, HL_W);
            T2_L = T1_L >> 1;
            if(AF_L & _CF) {
                T2_L |= 0x80;
            }
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L];
            if(T1_L & 0x01) {
                AF_L |= _CF;
            }
            (*IFACE.mreq_wr)(self, HL_W, T2_L);
            M_CYCLES += 4;
            T_STATES += 15;
            T_PERIOD -= 15;
            break;
        case 0x1f: /* RR  A             */
            T1_L = AF_H;
            T2_L = T1_L >> 1;
            if(AF_L & _CF) {
                T2_L |= 0x80;
            }
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L];
            if(T1_L & 0x01) {
                AF_L |= _CF;
            }
            AF_H = T2_L;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x20: /* SLA B             */
            T1_L = BC_H;
            T2_L = ((int8_t) T1_L) << 1;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L];
            if(T1_L & 0x80) {
                AF_L |= _CF;
            }
            BC_H = T2_L;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x21: /* SLA C             */
            T1_L = BC_L;
            T2_L = ((int8_t) T1_L) << 1;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L];
            if(T1_L & 0x80) {
                AF_L |= _CF;
            }
            BC_L = T2_L;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x22: /* SLA D             */
            T1_L = DE_H;
            T2_L = ((int8_t) T1_L) << 1;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L];
            if(T1_L & 0x80) {
                AF_L |= _CF;
            }
            DE_H = T2_L;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x23: /* SLA E             */
            T1_L = DE_L;
            T2_L = ((int8_t) T1_L) << 1;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L];
            if(T1_L & 0x80) {
                AF_L |= _CF;
            }
            DE_L = T2_L;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x24: /* SLA H             */
            T1_L = HL_H;
            T2_L = ((int8_t) T1_L) << 1;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L];
            if(T1_L & 0x80) {
                AF_L |= _CF;
            }
            HL_H = T2_L;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x25: /* SLA L             */
            T1_L = HL_L;
            T2_L = ((int8_t) T1_L) << 1;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L];
            if(T1_L & 0x80) {
                AF_L |= _CF;
            }
            HL_L = T2_L;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x26: /* SLA (HL)          */
            T1_L = (*IFACE.mreq_rd)(self, HL_W);
            T2_L = ((int8_t) T1_L) << 1;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L];
            if(T1_L & 0x80) {
                AF_L |= _CF;
            }
            (*IFACE.mreq_wr)(self, HL_W, T2_L);
            M_CYCLES += 4;
            T_STATES += 15;
            T_PERIOD -= 15;
            break;
        case 0x27: /* SLA A             */
            T1_L = AF_H;
            T2_L = ((int8_t) T1_L) << 1;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L];
            if(T1_L & 0x80) {
                AF_L |= _CF;
            }
            AF_H = T2_L;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x28: /* SRA B             */
            T1_L = BC_H;
            T2_L = ((int8_t) T1_L) >> 1;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L];
            if(T1_L & 0x01) {
                AF_L |= _CF;
            }
            BC_H = T2_L;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x29: /* SRA C             */
            T1_L = BC_L;
            T2_L = ((int8_t) T1_L) >> 1;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L];
            if(T1_L & 0x01) {
                AF_L |= _CF;
            }
            BC_L = T2_L;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x2a: /* SRA D             */
            T1_L = DE_H;
            T2_L = ((int8_t) T1_L) >> 1;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L];
            if(T1_L & 0x01) {
                AF_L |= _CF;
            }
            DE_H = T2_L;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x2b: /* SRA E             */
            T1_L = DE_L;
            T2_L = ((int8_t) T1_L) >> 1;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L];
            if(T1_L & 0x01) {
                AF_L |= _CF;
            }
            DE_L = T2_L;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x2c: /* SRA H             */
            T1_L = HL_H;
            T2_L = ((int8_t) T1_L) >> 1;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L];
            if(T1_L & 0x01) {
                AF_L |= _CF;
            }
            HL_H = T2_L;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x2d: /* SRA L             */
            T1_L = HL_L;
            T2_L = ((int8_t) T1_L) >> 1;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L];
            if(T1_L & 0x01) {
                AF_L |= _CF;
            }
            HL_L = T2_L;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x2e: /* SRA (HL)          */
            T1_L = (*IFACE.mreq_rd)(self, HL_W);
            T2_L = ((int8_t) T1_L) >> 1;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L];
            if(T1_L & 0x01) {
                AF_L |= _CF;
            }
            (*IFACE.mreq_wr)(self, HL_W, T2_L);
            M_CYCLES += 4;
            T_STATES += 15;
            T_PERIOD -= 15;
            break;
        case 0x2f: /* SRA A             */
            T1_L = AF_H;
            T2_L = ((int8_t) T1_L) >> 1;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L];
            if(T1_L & 0x01) {
                AF_L |= _CF;
            }
            AF_H = T2_L;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x30: /* SLL B             */
            T1_L = BC_H;
            T2_L = (T1_L << 1) | 0x01;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L];
            if(T1_L & 0x80) {
                AF_L |= _CF;
            }
            BC_H = T2_L;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x31: /* SLL C             */
            T1_L = BC_L;
            T2_L = (T1_L << 1) | 0x01;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L];
            if(T1_L & 0x80) {
                AF_L |= _CF;
            }
            BC_L = T2_L;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x32: /* SLL D             */
            T1_L = DE_H;
            T2_L = (T1_L << 1) | 0x01;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L];
            if(T1_L & 0x80) {
                AF_L |= _CF;
            }
            DE_H = T2_L;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x33: /* SLL E             */
            T1_L = DE_L;
            T2_L = (T1_L << 1) | 0x01;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L];
            if(T1_L & 0x80) {
                AF_L |= _CF;
            }
            DE_L = T2_L;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x34: /* SLL H             */
            T1_L = HL_H;
            T2_L = (T1_L << 1) | 0x01;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L];
            if(T1_L & 0x80) {
                AF_L |= _CF;
            }
            HL_H = T2_L;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x35: /* SLL L             */
            T1_L = HL_L;
            T2_L = (T1_L << 1) | 0x01;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L];
            if(T1_L & 0x80) {
                AF_L |= _CF;
            }
            HL_L = T2_L;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x36: /* SLL (HL)          */
            T1_L = (*IFACE.mreq_rd)(self, HL_W);
            T2_L = (T1_L << 1) | 0x01;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L];
            if(T1_L & 0x80) {
                AF_L |= _CF;
            }
            (*IFACE.mreq_wr)(self, HL_W, T2_L);
            M_CYCLES += 4;
            T_STATES += 15;
            T_PERIOD -= 15;
            break;
        case 0x37: /* SLL A             */
            T1_L = AF_H;
            T2_L = (T1_L << 1) | 0x01;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L];
            if(T1_L & 0x80) {
                AF_L |= _CF;
            }
            AF_H = T2_L;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x38: /* SRL B             */
            T1_L = BC_H;
            T2_L = (T1_L >> 1) & 0x7f;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L];
            if(T1_L & 0x01) {
                AF_L |= _CF;
            }
            BC_H = T2_L;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x39: /* SRL C             */
            T1_L = BC_L;
            T2_L = (T1_L >> 1) & 0x7f;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L];
            if(T1_L & 0x01) {
                AF_L |= _CF;
            }
            BC_L = T2_L;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x3a: /* SRL D             */
            T1_L = DE_H;
            T2_L = (T1_L >> 1) & 0x7f;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L];
            if(T1_L & 0x01) {
                AF_L |= _CF;
            }
            DE_H = T2_L;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x3b: /* SRL E             */
            T1_L = DE_L;
            T2_L = (T1_L >> 1) & 0x7f;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L];
            if(T1_L & 0x01) {
                AF_L |= _CF;
            }
            DE_L = T2_L;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x3c: /* SRL H             */
            T1_L = HL_H;
            T2_L = (T1_L >> 1) & 0x7f;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L];
            if(T1_L & 0x01) {
                AF_L |= _CF;
            }
            HL_H = T2_L;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x3d: /* SRL L             */
            T1_L = HL_L;
            T2_L = (T1_L >> 1) & 0x7f;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L];
            if(T1_L & 0x01) {
                AF_L |= _CF;
            }
            HL_L = T2_L;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x3e: /* SRL (HL)          */
            T1_L = (*IFACE.mreq_rd)(self, HL_W);
            T2_L = (T1_L >> 1) & 0x7f;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L];
            if(T1_L & 0x01) {
                AF_L |= _CF;
            }
            (*IFACE.mreq_wr)(self, HL_W, T2_L);
            M_CYCLES += 4;
            T_STATES += 15;
            T_PERIOD -= 15;
            break;
        case 0x3f: /* SRL A             */
            T1_L = AF_H;
            T2_L = (T1_L >> 1) & 0x7f;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L];
            if(T1_L & 0x01) {
                AF_L |= _CF;
            }
            AF_H = T2_L;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x40: /* BIT 0,B           */
            T1_L = BC_H;
            T2_L = T1_L & 0x01;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L] | (AF_L & _CF) | _HF;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x41: /* BIT 0,C           */
            T1_L = BC_L;
            T2_L = T1_L & 0x01;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L] | (AF_L & _CF) | _HF;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x42: /* BIT 0,D           */
            T1_L = DE_H;
            T2_L = T1_L & 0x01;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L] | (AF_L & _CF) | _HF;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x43: /* BIT 0,E           */
            T1_L = DE_L;
            T2_L = T1_L & 0x01;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L] | (AF_L & _CF) | _HF;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x44: /* BIT 0,H           */
            T1_L = HL_H;
            T2_L = T1_L & 0x01;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L] | (AF_L & _CF) | _HF;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x45: /* BIT 0,L           */
            T1_L = HL_L;
            T2_L = T1_L & 0x01;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L] | (AF_L & _CF) | _HF;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x46: /* BIT 0,(HL)        */
            T1_L = (*IFACE.mreq_rd)(self, HL_W);
            T2_L = T1_L & 0x01;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L] | (AF_L & _CF) | _HF;
            M_CYCLES += 3;
            T_STATES += 12;
            T_PERIOD -= 12;
            break;
        case 0x47: /* BIT 0,A           */
            T1_L = AF_H;
            T2_L = T1_L & 0x01;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L] | (AF_L & _CF) | _HF;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x48: /* BIT 1,B           */
            T1_L = BC_H;
            T2_L = T1_L & 0x02;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L] | (AF_L & _CF) | _HF;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x49: /* BIT 1,C           */
            T1_L = BC_L;
            T2_L = T1_L & 0x02;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L] | (AF_L & _CF) | _HF;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x4a: /* BIT 1,D           */
            T1_L = DE_H;
            T2_L = T1_L & 0x02;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L] | (AF_L & _CF) | _HF;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x4b: /* BIT 1,E           */
            T1_L = DE_L;
            T2_L = T1_L & 0x02;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L] | (AF_L & _CF) | _HF;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x4c: /* BIT 1,H           */
            T1_L = HL_H;
            T2_L = T1_L & 0x02;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L] | (AF_L & _CF) | _HF;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x4d: /* BIT 1,L           */
            T1_L = HL_L;
            T2_L = T1_L & 0x02;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L] | (AF_L & _CF) | _HF;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x4e: /* BIT 1,(HL)        */
            T1_L = (*IFACE.mreq_rd)(self, HL_W);
            T2_L = T1_L & 0x02;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L] | (AF_L & _CF) | _HF;
            M_CYCLES += 3;
            T_STATES += 12;
            T_PERIOD -= 12;
            break;
        case 0x4f: /* BIT 1,A           */
            T1_L = AF_H;
            T2_L = T1_L & 0x02;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L] | (AF_L & _CF) | _HF;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x50: /* BIT 2,B           */
            T1_L = BC_H;
            T2_L = T1_L & 0x04;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L] | (AF_L & _CF) | _HF;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x51: /* BIT 2,C           */
            T1_L = BC_L;
            T2_L = T1_L & 0x04;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L] | (AF_L & _CF) | _HF;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x52: /* BIT 2,D           */
            T1_L = DE_H;
            T2_L = T1_L & 0x04;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L] | (AF_L & _CF) | _HF;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x53: /* BIT 2,E           */
            T1_L = DE_L;
            T2_L = T1_L & 0x04;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L] | (AF_L & _CF) | _HF;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x54: /* BIT 2,H           */
            T1_L = HL_H;
            T2_L = T1_L & 0x04;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L] | (AF_L & _CF) | _HF;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x55: /* BIT 2,L           */
            T1_L = HL_L;
            T2_L = T1_L & 0x04;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L] | (AF_L & _CF) | _HF;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x56: /* BIT 2,(HL)        */
            T1_L = (*IFACE.mreq_rd)(self, HL_W);
            T2_L = T1_L & 0x04;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L] | (AF_L & _CF) | _HF;
            M_CYCLES += 3;
            T_STATES += 12;
            T_PERIOD -= 12;
            break;
        case 0x57: /* BIT 2,A           */
            T1_L = AF_H;
            T2_L = T1_L & 0x04;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L] | (AF_L & _CF) | _HF;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x58: /* BIT 3,B           */
            T1_L = BC_H;
            T2_L = T1_L & 0x08;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L] | (AF_L & _CF) | _HF;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x59: /* BIT 3,C           */
            T1_L = BC_L;
            T2_L = T1_L & 0x08;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L] | (AF_L & _CF) | _HF;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x5a: /* BIT 3,D           */
            T1_L = DE_H;
            T2_L = T1_L & 0x08;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L] | (AF_L & _CF) | _HF;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x5b: /* BIT 3,E           */
            T1_L = DE_L;
            T2_L = T1_L & 0x08;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L] | (AF_L & _CF) | _HF;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x5c: /* BIT 3,H           */
            T1_L = HL_H;
            T2_L = T1_L & 0x08;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L] | (AF_L & _CF) | _HF;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x5d: /* BIT 3,L           */
            T1_L = HL_L;
            T2_L = T1_L & 0x08;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L] | (AF_L & _CF) | _HF;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x5e: /* BIT 3,(HL)        */
            T1_L = (*IFACE.mreq_rd)(self, HL_W);
            T2_L = T1_L & 0x08;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L] | (AF_L & _CF) | _HF;
            M_CYCLES += 3;
            T_STATES += 12;
            T_PERIOD -= 12;
            break;
        case 0x5f: /* BIT 3,A           */
            T1_L = AF_H;
            T2_L = T1_L & 0x08;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L] | (AF_L & _CF) | _HF;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x60: /* BIT 4,B           */
            T1_L = BC_H;
            T2_L = T1_L & 0x10;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L] | (AF_L & _CF) | _HF;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x61: /* BIT 4,C           */
            T1_L = BC_L;
            T2_L = T1_L & 0x10;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L] | (AF_L & _CF) | _HF;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x62: /* BIT 4,D           */
            T1_L = DE_H;
            T2_L = T1_L & 0x10;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L] | (AF_L & _CF) | _HF;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x63: /* BIT 4,E           */
            T1_L = DE_L;
            T2_L = T1_L & 0x10;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L] | (AF_L & _CF) | _HF;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x64: /* BIT 4,H           */
            T1_L = HL_H;
            T2_L = T1_L & 0x10;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L] | (AF_L & _CF) | _HF;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x65: /* BIT 4,L           */
            T1_L = HL_L;
            T2_L = T1_L & 0x10;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L] | (AF_L & _CF) | _HF;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x66: /* BIT 4,(HL)        */
            T1_L = (*IFACE.mreq_rd)(self, HL_W);
            T2_L = T1_L & 0x10;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L] | (AF_L & _CF) | _HF;
            M_CYCLES += 3;
            T_STATES += 12;
            T_PERIOD -= 12;
            break;
        case 0x67: /* BIT 4,A           */
            T1_L = AF_H;
            T2_L = T1_L & 0x10;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L] | (AF_L & _CF) | _HF;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x68: /* BIT 5,B           */
            T1_L = BC_H;
            T2_L = T1_L & 0x20;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L] | (AF_L & _CF) | _HF;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x69: /* BIT 5,C           */
            T1_L = BC_L;
            T2_L = T1_L & 0x20;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L] | (AF_L & _CF) | _HF;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x6a: /* BIT 5,D           */
            T1_L = DE_H;
            T2_L = T1_L & 0x20;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L] | (AF_L & _CF) | _HF;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x6b: /* BIT 5,E           */
            T1_L = DE_L;
            T2_L = T1_L & 0x20;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L] | (AF_L & _CF) | _HF;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x6c: /* BIT 5,H           */
            T1_L = HL_H;
            T2_L = T1_L & 0x20;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L] | (AF_L & _CF) | _HF;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x6d: /* BIT 5,L           */
            T1_L = HL_L;
            T2_L = T1_L & 0x20;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L] | (AF_L & _CF) | _HF;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x6e: /* BIT 5,(HL)        */
            T1_L = (*IFACE.mreq_rd)(self, HL_W);
            T2_L = T1_L & 0x20;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L] | (AF_L & _CF) | _HF;
            M_CYCLES += 3;
            T_STATES += 12;
            T_PERIOD -= 12;
            break;
        case 0x6f: /* BIT 5,A           */
            T1_L = AF_H;
            T2_L = T1_L & 0x20;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L] | (AF_L & _CF) | _HF;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x70: /* BIT 6,B           */
            T1_L = BC_H;
            T2_L = T1_L & 0x40;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L] | (AF_L & _CF) | _HF;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x71: /* BIT 6,C           */
            T1_L = BC_L;
            T2_L = T1_L & 0x40;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L] | (AF_L & _CF) | _HF;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x72: /* BIT 6,D           */
            T1_L = DE_H;
            T2_L = T1_L & 0x40;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L] | (AF_L & _CF) | _HF;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x73: /* BIT 6,E           */
            T1_L = DE_L;
            T2_L = T1_L & 0x40;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L] | (AF_L & _CF) | _HF;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x74: /* BIT 6,H           */
            T1_L = HL_H;
            T2_L = T1_L & 0x40;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L] | (AF_L & _CF) | _HF;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x75: /* BIT 6,L           */
            T1_L = HL_L;
            T2_L = T1_L & 0x40;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L] | (AF_L & _CF) | _HF;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x76: /* BIT 6,(HL)        */
            T1_L = (*IFACE.mreq_rd)(self, HL_W);
            T2_L = T1_L & 0x40;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L] | (AF_L & _CF) | _HF;
            M_CYCLES += 3;
            T_STATES += 12;
            T_PERIOD -= 12;
            break;
        case 0x77: /* BIT 6,A           */
            T1_L = AF_H;
            T2_L = T1_L & 0x40;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L] | (AF_L & _CF) | _HF;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x78: /* BIT 7,B           */
            T1_L = BC_H;
            T2_L = T1_L & 0x80;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L] | (AF_L & _CF) | _HF;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x79: /* BIT 7,C           */
            T1_L = BC_L;
            T2_L = T1_L & 0x80;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L] | (AF_L & _CF) | _HF;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x7a: /* BIT 7,D           */
            T1_L = DE_H;
            T2_L = T1_L & 0x80;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L] | (AF_L & _CF) | _HF;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x7b: /* BIT 7,E           */
            T1_L = DE_L;
            T2_L = T1_L & 0x80;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L] | (AF_L & _CF) | _HF;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x7c: /* BIT 7,H           */
            T1_L = HL_H;
            T2_L = T1_L & 0x80;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L] | (AF_L & _CF) | _HF;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x7d: /* BIT 7,L           */
            T1_L = HL_L;
            T2_L = T1_L & 0x80;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L] | (AF_L & _CF) | _HF;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x7e: /* BIT 7,(HL)        */
            T1_L = (*IFACE.mreq_rd)(self, HL_W);
            T2_L = T1_L & 0x80;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L] | (AF_L & _CF) | _HF;
            M_CYCLES += 3;
            T_STATES += 12;
            T_PERIOD -= 12;
            break;
        case 0x7f: /* BIT 7,A           */
            T1_L = AF_H;
            T2_L = T1_L & 0x80;
            AF_L = (T2_L & (_SF | _5F | _3F)) | PZSTable[T2_L] | (AF_L & _CF) | _HF;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x80: /* RES 0,B           */
            BC_H &= 0xfe;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x81: /* RES 0,C           */
            BC_L &= 0xfe;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x82: /* RES 0,D           */
            DE_H &= 0xfe;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x83: /* RES 0,E           */
            DE_L &= 0xfe;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x84: /* RES 0,H           */
            HL_H &= 0xfe;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x85: /* RES 0,L           */
            HL_L &= 0xfe;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x86: /* RES 0,(HL)        */
            T1_L = (*IFACE.mreq_rd)(self, HL_W);
            T2_L = T1_L & 0xfe;
            (*IFACE.mreq_wr)(self, HL_W, T2_L);
            M_CYCLES += 4;
            T_STATES += 15;
            T_PERIOD -= 15;
            break;
        case 0x87: /* RES 0,A           */
            AF_H &= 0xfe;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x88: /* RES 1,B           */
            BC_H &= 0xfd;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x89: /* RES 1,C           */
            BC_L &= 0xfd;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x8a: /* RES 1,D           */
            DE_H &= 0xfd;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x8b: /* RES 1,E           */
            DE_L &= 0xfd;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x8c: /* RES 1,H           */
            HL_H &= 0xfd;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x8d: /* RES 1,L           */
            HL_L &= 0xfd;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x8e: /* RES 1,(HL)        */
            T1_L = (*IFACE.mreq_rd)(self, HL_W);
            T2_L = T1_L & 0xfd;
            (*IFACE.mreq_wr)(self, HL_W, T2_L);
            M_CYCLES += 4;
            T_STATES += 15;
            T_PERIOD -= 15;
            break;
        case 0x8f: /* RES 1,A           */
            AF_H &= 0xfd;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x90: /* RES 2,B           */
            BC_H &= 0xfb;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x91: /* RES 2,C           */
            BC_L &= 0xfb;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x92: /* RES 2,D           */
            DE_H &= 0xfb;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x93: /* RES 2,E           */
            DE_L &= 0xfb;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x94: /* RES 2,H           */
            HL_H &= 0xfb;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x95: /* RES 2,L           */
            HL_L &= 0xfb;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x96: /* RES 2,(HL)        */
            T1_L = (*IFACE.mreq_rd)(self, HL_W);
            T2_L = T1_L & 0xfb;
            (*IFACE.mreq_wr)(self, HL_W, T2_L);
            M_CYCLES += 4;
            T_STATES += 15;
            T_PERIOD -= 15;
            break;
        case 0x97: /* RES 2,A           */
            AF_H &= 0xfb;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x98: /* RES 3,B           */
            BC_H &= 0xf7;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x99: /* RES 3,C           */
            BC_L &= 0xf7;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x9a: /* RES 3,D           */
            DE_H &= 0xf7;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x9b: /* RES 3,E           */
            DE_L &= 0xf7;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x9c: /* RES 3,H           */
            HL_H &= 0xf7;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x9d: /* RES 3,L           */
            HL_L &= 0xf7;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0x9e: /* RES 3,(HL)        */
            T1_L = (*IFACE.mreq_rd)(self, HL_W);
            T2_L = T1_L & 0xf7;
            (*IFACE.mreq_wr)(self, HL_W, T2_L);
            M_CYCLES += 4;
            T_STATES += 15;
            T_PERIOD -= 15;
            break;
        case 0x9f: /* RES 3,A           */
            AF_H &= 0xf7;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xa0: /* RES 4,B           */
            BC_H &= 0xef;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xa1: /* RES 4,C           */
            BC_L &= 0xef;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xa2: /* RES 4,D           */
            DE_H &= 0xef;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xa3: /* RES 4,E           */
            DE_L &= 0xef;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xa4: /* RES 4,H           */
            HL_H &= 0xef;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xa5: /* RES 4,L           */
            HL_L &= 0xef;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xa6: /* RES 4,(HL)        */
            T1_L = (*IFACE.mreq_rd)(self, HL_W);
            T2_L = T1_L & 0xef;
            (*IFACE.mreq_wr)(self, HL_W, T2_L);
            M_CYCLES += 4;
            T_STATES += 15;
            T_PERIOD -= 15;
            break;
        case 0xa7: /* RES 4,A           */
            AF_H &= 0xef;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xa8: /* RES 5,B           */
            BC_H &= 0xdf;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xa9: /* RES 5,C           */
            BC_L &= 0xdf;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xaa: /* RES 5,D           */
            DE_H &= 0xdf;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xab: /* RES 5,E           */
            DE_L &= 0xdf;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xac: /* RES 5,H           */
            HL_H &= 0xdf;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xad: /* RES 5,L           */
            HL_L &= 0xdf;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xae: /* RES 5,(HL)        */
            T1_L = (*IFACE.mreq_rd)(self, HL_W);
            T2_L = T1_L & 0xdf;
            (*IFACE.mreq_wr)(self, HL_W, T2_L);
            M_CYCLES += 4;
            T_STATES += 15;
            T_PERIOD -= 15;
            break;
        case 0xaf: /* RES 5,A           */
            AF_H &= 0xdf;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xb0: /* RES 6,B           */
            BC_H &= 0xbf;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xb1: /* RES 6,C           */
            BC_L &= 0xbf;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xb2: /* RES 6,D           */
            DE_H &= 0xbf;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xb3: /* RES 6,E           */
            DE_L &= 0xbf;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xb4: /* RES 6,H           */
            HL_H &= 0xbf;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xb5: /* RES 6,L           */
            HL_L &= 0xbf;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xb6: /* RES 6,(HL)        */
            T1_L = (*IFACE.mreq_rd)(self, HL_W);
            T2_L = T1_L & 0xbf;
            (*IFACE.mreq_wr)(self, HL_W, T2_L);
            M_CYCLES += 4;
            T_STATES += 15;
            T_PERIOD -= 15;
            break;
        case 0xb7: /* RES 6,A           */
            AF_H &= 0xbf;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xb8: /* RES 7,B           */
            BC_H &= 0x7f;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xb9: /* RES 7,C           */
            BC_L &= 0x7f;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xba: /* RES 7,D           */
            DE_H &= 0x7f;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xbb: /* RES 7,E           */
            DE_L &= 0x7f;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xbc: /* RES 7,H           */
            HL_H &= 0x7f;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xbd: /* RES 7,L           */
            HL_L &= 0x7f;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xbe: /* RES 7,(HL)        */
            T1_L = (*IFACE.mreq_rd)(self, HL_W);
            T2_L = T1_L & 0x7f;
            (*IFACE.mreq_wr)(self, HL_W, T2_L);
            M_CYCLES += 4;
            T_STATES += 15;
            T_PERIOD -= 15;
            break;
        case 0xbf: /* RES 7,A           */
            AF_H &= 0x7f;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xc0: /* SET 0,B           */
            BC_H |= 0x01;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xc1: /* SET 0,C           */
            BC_L |= 0x01;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xc2: /* SET 0,D           */
            DE_H |= 0x01;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xc3: /* SET 0,E           */
            DE_L |= 0x01;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xc4: /* SET 0,H           */
            HL_H |= 0x01;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xc5: /* SET 0,L           */
            HL_L |= 0x01;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xc6: /* SET 0,(HL)        */
            T1_L = (*IFACE.mreq_rd)(self, HL_W);
            T2_L = T1_L | 0x01;
            (*IFACE.mreq_wr)(self, HL_W, T2_L);
            M_CYCLES += 4;
            T_STATES += 15;
            T_PERIOD -= 15;
            break;
        case 0xc7: /* SET 0,A           */
            AF_H |= 0x01;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xc8: /* SET 1,B           */
            BC_H |= 0x02;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xc9: /* SET 1,C           */
            BC_L |= 0x02;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xca: /* SET 1,D           */
            DE_H |= 0x02;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xcb: /* SET 1,E           */
            DE_L |= 0x02;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xcc: /* SET 1,H           */
            HL_H |= 0x02;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xcd: /* SET 1,L           */
            HL_L |= 0x02;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xce: /* SET 1,(HL)        */
            T1_L = (*IFACE.mreq_rd)(self, HL_W);
            T2_L = T1_L | 0x02;
            (*IFACE.mreq_wr)(self, HL_W, T2_L);
            M_CYCLES += 4;
            T_STATES += 15;
            T_PERIOD -= 15;
            break;
        case 0xcf: /* SET 1,A           */
            AF_H |= 0x02;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xd0: /* SET 2,B           */
            BC_H |= 0x04;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xd1: /* SET 2,C           */
            BC_L |= 0x04;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xd2: /* SET 2,D           */
            DE_H |= 0x04;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xd3: /* SET 2,E           */
            DE_L |= 0x04;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xd4: /* SET 2,H           */
            HL_H |= 0x04;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xd5: /* SET 2,L           */
            HL_L |= 0x04;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xd6: /* SET 2,(HL)        */
            T1_L = (*IFACE.mreq_rd)(self, HL_W);
            T2_L = T1_L | 0x04;
            (*IFACE.mreq_wr)(self, HL_W, T2_L);
            M_CYCLES += 4;
            T_STATES += 15;
            T_PERIOD -= 15;
            break;
        case 0xd7: /* SET 2,A           */
            AF_H |= 0x04;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xd8: /* SET 3,B           */
            BC_H |= 0x08;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xd9: /* SET 3,C           */
            BC_L |= 0x08;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xda: /* SET 3,D           */
            DE_H |= 0x08;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xdb: /* SET 3,E           */
            DE_L |= 0x08;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xdc: /* SET 3,H           */
            HL_H |= 0x08;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xdd: /* SET 3,L           */
            HL_L |= 0x08;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xde: /* SET 3,(HL)        */
            T1_L = (*IFACE.mreq_rd)(self, HL_W);
            T2_L = T1_L | 0x08;
            (*IFACE.mreq_wr)(self, HL_W, T2_L);
            M_CYCLES += 4;
            T_STATES += 15;
            T_PERIOD -= 15;
            break;
        case 0xdf: /* SET 3,A           */
            AF_H |= 0x08;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xe0: /* SET 4,B           */
            BC_H |= 0x10;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xe1: /* SET 4,C           */
            BC_L |= 0x10;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xe2: /* SET 4,D           */
            DE_H |= 0x10;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xe3: /* SET 4,E           */
            DE_L |= 0x10;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xe4: /* SET 4,H           */
            HL_H |= 0x10;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xe5: /* SET 4,L           */
            HL_L |= 0x10;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xe6: /* SET 4,(HL)        */
            T1_L = (*IFACE.mreq_rd)(self, HL_W);
            T2_L = T1_L | 0x10;
            (*IFACE.mreq_wr)(self, HL_W, T2_L);
            M_CYCLES += 4;
            T_STATES += 15;
            T_PERIOD -= 15;
            break;
        case 0xe7: /* SET 4,A           */
            AF_H |= 0x10;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xe8: /* SET 5,B           */
            BC_H |= 0x20;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xe9: /* SET 5,C           */
            BC_L |= 0x20;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xea: /* SET 5,D           */
            DE_H |= 0x20;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xeb: /* SET 5,E           */
            DE_L |= 0x20;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xec: /* SET 5,H           */
            HL_H |= 0x20;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xed: /* SET 5,L           */
            HL_L |= 0x20;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xee: /* SET 5,(HL)        */
            T1_L = (*IFACE.mreq_rd)(self, HL_W);
            T2_L = T1_L | 0x20;
            (*IFACE.mreq_wr)(self, HL_W, T2_L);
            M_CYCLES += 4;
            T_STATES += 15;
            T_PERIOD -= 15;
            break;
        case 0xef: /* SET 5,A           */
            AF_H |= 0x20;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xf0: /* SET 6,B           */
            BC_H |= 0x40;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xf1: /* SET 6,C           */
            BC_L |= 0x40;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xf2: /* SET 6,D           */
            DE_H |= 0x40;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xf3: /* SET 6,E           */
            DE_L |= 0x40;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xf4: /* SET 6,H           */
            HL_H |= 0x40;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xf5: /* SET 6,L           */
            HL_L |= 0x40;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xf6: /* SET 6,(HL)        */
            T1_L = (*IFACE.mreq_rd)(self, HL_W);
            T2_L = T1_L | 0x40;
            (*IFACE.mreq_wr)(self, HL_W, T2_L);
            M_CYCLES += 4;
            T_STATES += 15;
            T_PERIOD -= 15;
            break;
        case 0xf7: /* SET 6,A           */
            AF_H |= 0x40;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xf8: /* SET 7,B           */
            BC_H |= 0x80;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xf9: /* SET 7,C           */
            BC_L |= 0x80;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xfa: /* SET 7,D           */
            DE_H |= 0x80;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xfb: /* SET 7,E           */
            DE_L |= 0x80;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xfc: /* SET 7,H           */
            HL_H |= 0x80;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xfd: /* SET 7,L           */
            HL_L |= 0x80;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
        case 0xfe: /* SET 7,(HL)        */
            T1_L = (*IFACE.mreq_rd)(self, HL_W);
            T2_L = T1_L | 0x80;
            (*IFACE.mreq_wr)(self, HL_W, T2_L);
            M_CYCLES += 4;
            T_STATES += 15;
            T_PERIOD -= 15;
            break;
        case 0xff: /* SET 7,A           */
            AF_H |= 0x80;
            M_CYCLES += 2;
            T_STATES += 8;
            T_PERIOD -= 8;
            break;
