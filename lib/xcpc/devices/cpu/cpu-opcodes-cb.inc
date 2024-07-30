        case 0x00: /* rlc b */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_rlc_r08(BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x01: /* rlc c */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_rlc_r08(BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x02: /* rlc d */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_rlc_r08(DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x03: /* rlc e */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_rlc_r08(DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x04: /* rlc h */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_rlc_r08(HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x05: /* rlc l */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_rlc_r08(HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x06: /* rlc (hl) */
            {
                constexpr uint32_t m_cycles = (4 - 1);
                constexpr uint32_t t_states = (15 - 4);
                m_rlc_ind_r16(HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x07: /* rlc a */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_rlc_r08(AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x08: /* rrc b */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_rrc_r08(BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x09: /* rrc c */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_rrc_r08(BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x0a: /* rrc d */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_rrc_r08(DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x0b: /* rrc e */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_rrc_r08(DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x0c: /* rrc h */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_rrc_r08(HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x0d: /* rrc l */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_rrc_r08(HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x0e: /* rrc (hl) */
            {
                constexpr uint32_t m_cycles = (4 - 1);
                constexpr uint32_t t_states = (15 - 4);
                m_rrc_ind_r16(HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x0f: /* rrc a */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_rrc_r08(AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x10: /* rl b */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_rl_r08(BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x11: /* rl c */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_rl_r08(BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x12: /* rl d */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_rl_r08(DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x13: /* rl e */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_rl_r08(DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x14: /* rl h */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_rl_r08(HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x15: /* rl l */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_rl_r08(HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x16: /* rl (hl) */
            {
                constexpr uint32_t m_cycles = (4 - 1);
                constexpr uint32_t t_states = (15 - 4);
                m_rl_ind_r16(HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x17: /* rl a */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_rl_r08(AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x18: /* rr b */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_rr_r08(BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x19: /* rr c */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_rr_r08(BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x1a: /* rr d */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_rr_r08(DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x1b: /* rr e */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_rr_r08(DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x1c: /* rr h */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_rr_r08(HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x1d: /* rr l */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_rr_r08(HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x1e: /* rr (hl) */
            {
                constexpr uint32_t m_cycles = (4 - 1);
                constexpr uint32_t t_states = (15 - 4);
                m_rr_ind_r16(HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x1f: /* rr a */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_rr_r08(AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x20: /* sla b */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_sla_r08(BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x21: /* sla c */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_sla_r08(BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x22: /* sla d */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_sla_r08(DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x23: /* sla e */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_sla_r08(DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x24: /* sla h */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_sla_r08(HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x25: /* sla l */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_sla_r08(HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x26: /* sla (hl) */
            {
                constexpr uint32_t m_cycles = (4 - 1);
                constexpr uint32_t t_states = (15 - 4);
                m_sla_ind_r16(HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x27: /* sla a */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_sla_r08(AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x28: /* sra b */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_sra_r08(BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x29: /* sra c */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_sra_r08(BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x2a: /* sra d */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_sra_r08(DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x2b: /* sra e */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_sra_r08(DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x2c: /* sra h */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_sra_r08(HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x2d: /* sra l */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_sra_r08(HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x2e: /* sra (hl) */
            {
                constexpr uint32_t m_cycles = (4 - 1);
                constexpr uint32_t t_states = (15 - 4);
                m_sra_ind_r16(HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x2f: /* sra a */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_sra_r08(AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x30: /* sll b */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_sll_r08(BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x31: /* sll c */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_sll_r08(BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x32: /* sll d */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_sll_r08(DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x33: /* sll e */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_sll_r08(DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x34: /* sll h */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_sll_r08(HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x35: /* sll l */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_sll_r08(HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x36: /* sll (hl) */
            {
                constexpr uint32_t m_cycles = (4 - 1);
                constexpr uint32_t t_states = (15 - 4);
                m_sll_ind_r16(HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x37: /* sll a */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_sll_r08(AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x38: /* srl b */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_srl_r08(BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x39: /* srl c */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_srl_r08(BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x3a: /* srl d */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_srl_r08(DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x3b: /* srl e */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_srl_r08(DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x3c: /* srl h */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_srl_r08(HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x3d: /* srl l */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_srl_r08(HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x3e: /* srl (hl) */
            {
                constexpr uint32_t m_cycles = (4 - 1);
                constexpr uint32_t t_states = (15 - 4);
                m_srl_ind_r16(HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x3f: /* srl a */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_srl_r08(AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x40: /* bit 0,b */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_bit_b_r08(BIT0, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x41: /* bit 0,c */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_bit_b_r08(BIT0, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x42: /* bit 0,d */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_bit_b_r08(BIT0, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x43: /* bit 0,e */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_bit_b_r08(BIT0, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x44: /* bit 0,h */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_bit_b_r08(BIT0, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x45: /* bit 0,l */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_bit_b_r08(BIT0, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x46: /* bit 0,(hl) */
            {
                constexpr uint32_t m_cycles = (3 - 1);
                constexpr uint32_t t_states = (12 - 4);
                m_bit_b_ind_r16(BIT0, HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x47: /* bit 0,a */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_bit_b_r08(BIT0, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x48: /* bit 1,b */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_bit_b_r08(BIT1, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x49: /* bit 1,c */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_bit_b_r08(BIT1, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x4a: /* bit 1,d */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_bit_b_r08(BIT1, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x4b: /* bit 1,e */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_bit_b_r08(BIT1, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x4c: /* bit 1,h */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_bit_b_r08(BIT1, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x4d: /* bit 1,l */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_bit_b_r08(BIT1, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x4e: /* bit 1,(hl) */
            {
                constexpr uint32_t m_cycles = (3 - 1);
                constexpr uint32_t t_states = (12 - 4);
                m_bit_b_ind_r16(BIT1, HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x4f: /* bit 1,a */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_bit_b_r08(BIT1, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x50: /* bit 2,b */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_bit_b_r08(BIT2, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x51: /* bit 2,c */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_bit_b_r08(BIT2, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x52: /* bit 2,d */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_bit_b_r08(BIT2, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x53: /* bit 2,e */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_bit_b_r08(BIT2, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x54: /* bit 2,h */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_bit_b_r08(BIT2, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x55: /* bit 2,l */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_bit_b_r08(BIT2, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x56: /* bit 2,(hl) */
            {
                constexpr uint32_t m_cycles = (3 - 1);
                constexpr uint32_t t_states = (12 - 4);
                m_bit_b_ind_r16(BIT2, HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x57: /* bit 2,a */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_bit_b_r08(BIT2, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x58: /* bit 3,b */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_bit_b_r08(BIT3, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x59: /* bit 3,c */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_bit_b_r08(BIT3, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x5a: /* bit 3,d */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_bit_b_r08(BIT3, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x5b: /* bit 3,e */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_bit_b_r08(BIT3, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x5c: /* bit 3,h */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_bit_b_r08(BIT3, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x5d: /* bit 3,l */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_bit_b_r08(BIT3, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x5e: /* bit 3,(hl) */
            {
                constexpr uint32_t m_cycles = (3 - 1);
                constexpr uint32_t t_states = (12 - 4);
                m_bit_b_ind_r16(BIT3, HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x5f: /* bit 3,a */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_bit_b_r08(BIT3, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x60: /* bit 4,b */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_bit_b_r08(BIT4, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x61: /* bit 4,c */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_bit_b_r08(BIT4, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x62: /* bit 4,d */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_bit_b_r08(BIT4, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x63: /* bit 4,e */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_bit_b_r08(BIT4, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x64: /* bit 4,h */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_bit_b_r08(BIT4, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x65: /* bit 4,l */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_bit_b_r08(BIT4, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x66: /* bit 4,(hl) */
            {
                constexpr uint32_t m_cycles = (3 - 1);
                constexpr uint32_t t_states = (12 - 4);
                m_bit_b_ind_r16(BIT4, HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x67: /* bit 4,a */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_bit_b_r08(BIT4, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x68: /* bit 5,b */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_bit_b_r08(BIT5, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x69: /* bit 5,c */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_bit_b_r08(BIT5, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x6a: /* bit 5,d */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_bit_b_r08(BIT5, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x6b: /* bit 5,e */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_bit_b_r08(BIT5, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x6c: /* bit 5,h */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_bit_b_r08(BIT5, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x6d: /* bit 5,l */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_bit_b_r08(BIT5, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x6e: /* bit 5,(hl) */
            {
                constexpr uint32_t m_cycles = (3 - 1);
                constexpr uint32_t t_states = (12 - 4);
                m_bit_b_ind_r16(BIT5, HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x6f: /* bit 5,a */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_bit_b_r08(BIT5, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x70: /* bit 6,b */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_bit_b_r08(BIT6, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x71: /* bit 6,c */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_bit_b_r08(BIT6, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x72: /* bit 6,d */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_bit_b_r08(BIT6, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x73: /* bit 6,e */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_bit_b_r08(BIT6, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x74: /* bit 6,h */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_bit_b_r08(BIT6, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x75: /* bit 6,l */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_bit_b_r08(BIT6, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x76: /* bit 6,(hl) */
            {
                constexpr uint32_t m_cycles = (3 - 1);
                constexpr uint32_t t_states = (12 - 4);
                m_bit_b_ind_r16(BIT6, HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x77: /* bit 6,a */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_bit_b_r08(BIT6, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x78: /* bit 7,b */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_bit_b_r08(BIT7, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x79: /* bit 7,c */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_bit_b_r08(BIT7, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x7a: /* bit 7,d */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_bit_b_r08(BIT7, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x7b: /* bit 7,e */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_bit_b_r08(BIT7, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x7c: /* bit 7,h */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_bit_b_r08(BIT7, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x7d: /* bit 7,l */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_bit_b_r08(BIT7, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x7e: /* bit 7,(hl) */
            {
                constexpr uint32_t m_cycles = (3 - 1);
                constexpr uint32_t t_states = (12 - 4);
                m_bit_b_ind_r16(BIT7, HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x7f: /* bit 7,a */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_bit_b_r08(BIT7, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x80: /* res 0,b */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_res_b_r08(BIT0, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x81: /* res 0,c */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_res_b_r08(BIT0, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x82: /* res 0,d */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_res_b_r08(BIT0, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x83: /* res 0,e */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_res_b_r08(BIT0, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x84: /* res 0,h */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_res_b_r08(BIT0, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x85: /* res 0,l */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_res_b_r08(BIT0, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x86: /* res 0,(hl) */
            {
                constexpr uint32_t m_cycles = (4 - 1);
                constexpr uint32_t t_states = (15 - 4);
                m_res_b_ind_r16(BIT0, HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x87: /* res 0,a */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_res_b_r08(BIT0, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x88: /* res 1,b */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_res_b_r08(BIT1, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x89: /* res 1,c */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_res_b_r08(BIT1, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x8a: /* res 1,d */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_res_b_r08(BIT1, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x8b: /* res 1,e */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_res_b_r08(BIT1, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x8c: /* res 1,h */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_res_b_r08(BIT1, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x8d: /* res 1,l */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_res_b_r08(BIT1, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x8e: /* res 1,(hl) */
            {
                constexpr uint32_t m_cycles = (4 - 1);
                constexpr uint32_t t_states = (15 - 4);
                m_res_b_ind_r16(BIT1, HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x8f: /* res 1,a */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_res_b_r08(BIT1, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x90: /* res 2,b */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_res_b_r08(BIT2, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x91: /* res 2,c */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_res_b_r08(BIT2, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x92: /* res 2,d */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_res_b_r08(BIT2, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x93: /* res 2,e */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_res_b_r08(BIT2, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x94: /* res 2,h */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_res_b_r08(BIT2, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x95: /* res 2,l */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_res_b_r08(BIT2, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x96: /* res 2,(hl) */
            {
                constexpr uint32_t m_cycles = (4 - 1);
                constexpr uint32_t t_states = (15 - 4);
                m_res_b_ind_r16(BIT2, HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x97: /* res 2,a */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_res_b_r08(BIT2, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x98: /* res 3,b */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_res_b_r08(BIT3, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x99: /* res 3,c */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_res_b_r08(BIT3, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x9a: /* res 3,d */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_res_b_r08(BIT3, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x9b: /* res 3,e */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_res_b_r08(BIT3, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x9c: /* res 3,h */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_res_b_r08(BIT3, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x9d: /* res 3,l */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_res_b_r08(BIT3, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x9e: /* res 3,(hl) */
            {
                constexpr uint32_t m_cycles = (4 - 1);
                constexpr uint32_t t_states = (15 - 4);
                m_res_b_ind_r16(BIT3, HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x9f: /* res 3,a */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_res_b_r08(BIT3, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xa0: /* res 4,b */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_res_b_r08(BIT4, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xa1: /* res 4,c */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_res_b_r08(BIT4, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xa2: /* res 4,d */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_res_b_r08(BIT4, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xa3: /* res 4,e */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_res_b_r08(BIT4, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xa4: /* res 4,h */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_res_b_r08(BIT4, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xa5: /* res 4,l */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_res_b_r08(BIT4, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xa6: /* res 4,(hl) */
            {
                constexpr uint32_t m_cycles = (4 - 1);
                constexpr uint32_t t_states = (15 - 4);
                m_res_b_ind_r16(BIT4, HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xa7: /* res 4,a */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_res_b_r08(BIT4, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xa8: /* res 5,b */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_res_b_r08(BIT5, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xa9: /* res 5,c */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_res_b_r08(BIT5, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xaa: /* res 5,d */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_res_b_r08(BIT5, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xab: /* res 5,e */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_res_b_r08(BIT5, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xac: /* res 5,h */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_res_b_r08(BIT5, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xad: /* res 5,l */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_res_b_r08(BIT5, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xae: /* res 5,(hl) */
            {
                constexpr uint32_t m_cycles = (4 - 1);
                constexpr uint32_t t_states = (15 - 4);
                m_res_b_ind_r16(BIT5, HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xaf: /* res 5,a */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_res_b_r08(BIT5, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xb0: /* res 6,b */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_res_b_r08(BIT6, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xb1: /* res 6,c */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_res_b_r08(BIT6, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xb2: /* res 6,d */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_res_b_r08(BIT6, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xb3: /* res 6,e */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_res_b_r08(BIT6, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xb4: /* res 6,h */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_res_b_r08(BIT6, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xb5: /* res 6,l */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_res_b_r08(BIT6, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xb6: /* res 6,(hl) */
            {
                constexpr uint32_t m_cycles = (4 - 1);
                constexpr uint32_t t_states = (15 - 4);
                m_res_b_ind_r16(BIT6, HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xb7: /* res 6,a */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_res_b_r08(BIT6, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xb8: /* res 7,b */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_res_b_r08(BIT7, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xb9: /* res 7,c */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_res_b_r08(BIT7, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xba: /* res 7,d */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_res_b_r08(BIT7, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xbb: /* res 7,e */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_res_b_r08(BIT7, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xbc: /* res 7,h */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_res_b_r08(BIT7, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xbd: /* res 7,l */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_res_b_r08(BIT7, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xbe: /* res 7,(hl) */
            {
                constexpr uint32_t m_cycles = (4 - 1);
                constexpr uint32_t t_states = (15 - 4);
                m_res_b_ind_r16(BIT7, HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xbf: /* res 7,a */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_res_b_r08(BIT7, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xc0: /* set 0,b */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_set_b_r08(BIT0, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xc1: /* set 0,c */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_set_b_r08(BIT0, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xc2: /* set 0,d */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_set_b_r08(BIT0, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xc3: /* set 0,e */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_set_b_r08(BIT0, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xc4: /* set 0,h */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_set_b_r08(BIT0, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xc5: /* set 0,l */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_set_b_r08(BIT0, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xc6: /* set 0,(hl) */
            {
                constexpr uint32_t m_cycles = (4 - 1);
                constexpr uint32_t t_states = (15 - 4);
                m_set_b_ind_r16(BIT0, HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xc7: /* set 0,a */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_set_b_r08(BIT0, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xc8: /* set 1,b */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_set_b_r08(BIT1, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xc9: /* set 1,c */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_set_b_r08(BIT1, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xca: /* set 1,d */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_set_b_r08(BIT1, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xcb: /* set 1,e */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_set_b_r08(BIT1, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xcc: /* set 1,h */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_set_b_r08(BIT1, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xcd: /* set 1,l */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_set_b_r08(BIT1, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xce: /* set 1,(hl) */
            {
                constexpr uint32_t m_cycles = (4 - 1);
                constexpr uint32_t t_states = (15 - 4);
                m_set_b_ind_r16(BIT1, HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xcf: /* set 1,a */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_set_b_r08(BIT1, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xd0: /* set 2,b */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_set_b_r08(BIT2, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xd1: /* set 2,c */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_set_b_r08(BIT2, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xd2: /* set 2,d */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_set_b_r08(BIT2, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xd3: /* set 2,e */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_set_b_r08(BIT2, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xd4: /* set 2,h */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_set_b_r08(BIT2, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xd5: /* set 2,l */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_set_b_r08(BIT2, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xd6: /* set 2,(hl) */
            {
                constexpr uint32_t m_cycles = (4 - 1);
                constexpr uint32_t t_states = (15 - 4);
                m_set_b_ind_r16(BIT2, HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xd7: /* set 2,a */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_set_b_r08(BIT2, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xd8: /* set 3,b */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_set_b_r08(BIT3, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xd9: /* set 3,c */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_set_b_r08(BIT3, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xda: /* set 3,d */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_set_b_r08(BIT3, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xdb: /* set 3,e */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_set_b_r08(BIT3, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xdc: /* set 3,h */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_set_b_r08(BIT3, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xdd: /* set 3,l */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_set_b_r08(BIT3, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xde: /* set 3,(hl) */
            {
                constexpr uint32_t m_cycles = (4 - 1);
                constexpr uint32_t t_states = (15 - 4);
                m_set_b_ind_r16(BIT3, HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xdf: /* set 3,a */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_set_b_r08(BIT3, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xe0: /* set 4,b */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_set_b_r08(BIT4, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xe1: /* set 4,c */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_set_b_r08(BIT4, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xe2: /* set 4,d */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_set_b_r08(BIT4, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xe3: /* set 4,e */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_set_b_r08(BIT4, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xe4: /* set 4,h */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_set_b_r08(BIT4, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xe5: /* set 4,l */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_set_b_r08(BIT4, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xe6: /* set 4,(hl) */
            {
                constexpr uint32_t m_cycles = (4 - 1);
                constexpr uint32_t t_states = (15 - 4);
                m_set_b_ind_r16(BIT4, HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xe7: /* set 4,a */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_set_b_r08(BIT4, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xe8: /* set 5,b */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_set_b_r08(BIT5, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xe9: /* set 5,c */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_set_b_r08(BIT5, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xea: /* set 5,d */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_set_b_r08(BIT5, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xeb: /* set 5,e */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_set_b_r08(BIT5, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xec: /* set 5,h */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_set_b_r08(BIT5, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xed: /* set 5,l */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_set_b_r08(BIT5, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xee: /* set 5,(hl) */
            {
                constexpr uint32_t m_cycles = (4 - 1);
                constexpr uint32_t t_states = (15 - 4);
                m_set_b_ind_r16(BIT5, HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xef: /* set 5,a */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_set_b_r08(BIT5, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xf0: /* set 6,b */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_set_b_r08(BIT6, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xf1: /* set 6,c */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_set_b_r08(BIT6, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xf2: /* set 6,d */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_set_b_r08(BIT6, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xf3: /* set 6,e */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_set_b_r08(BIT6, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xf4: /* set 6,h */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_set_b_r08(BIT6, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xf5: /* set 6,l */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_set_b_r08(BIT6, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xf6: /* set 6,(hl) */
            {
                constexpr uint32_t m_cycles = (4 - 1);
                constexpr uint32_t t_states = (15 - 4);
                m_set_b_ind_r16(BIT6, HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xf7: /* set 6,a */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_set_b_r08(BIT6, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xf8: /* set 7,b */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_set_b_r08(BIT7, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xf9: /* set 7,c */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_set_b_r08(BIT7, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xfa: /* set 7,d */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_set_b_r08(BIT7, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xfb: /* set 7,e */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_set_b_r08(BIT7, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xfc: /* set 7,h */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_set_b_r08(BIT7, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xfd: /* set 7,l */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_set_b_r08(BIT7, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xfe: /* set 7,(hl) */
            {
                constexpr uint32_t m_cycles = (4 - 1);
                constexpr uint32_t t_states = (15 - 4);
                m_set_b_ind_r16(BIT7, HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xff: /* set 7,a */
            {
                constexpr uint32_t m_cycles = (2 - 1);
                constexpr uint32_t t_states = (8 - 4);
                m_set_b_r08(BIT7, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
