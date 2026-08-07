        case 0x00: /* rlc (iy+d),b */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_rlc_ind_r16_plus_i08_r08(IY_W, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x01: /* rlc (iy+d),c */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_rlc_ind_r16_plus_i08_r08(IY_W, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x02: /* rlc (iy+d),d */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_rlc_ind_r16_plus_i08_r08(IY_W, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x03: /* rlc (iy+d),e */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_rlc_ind_r16_plus_i08_r08(IY_W, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x04: /* rlc (iy+d),h */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_rlc_ind_r16_plus_i08_r08(IY_W, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x05: /* rlc (iy+d),l */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_rlc_ind_r16_plus_i08_r08(IY_W, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x06: /* rlc (iy+d) */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_rlc_ind_r16_plus_i08(IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x07: /* rlc (iy+d),a */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_rlc_ind_r16_plus_i08_r08(IY_W, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x08: /* rrc (iy+d),b */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_rrc_ind_r16_plus_i08_r08(IY_W, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x09: /* rrc (iy+d),c */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_rrc_ind_r16_plus_i08_r08(IY_W, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x0a: /* rrc (iy+d),d */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_rrc_ind_r16_plus_i08_r08(IY_W, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x0b: /* rrc (iy+d),e */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_rrc_ind_r16_plus_i08_r08(IY_W, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x0c: /* rrc (iy+d),h */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_rrc_ind_r16_plus_i08_r08(IY_W, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x0d: /* rrc (iy+d),l */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_rrc_ind_r16_plus_i08_r08(IY_W, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x0e: /* rrc (iy+d) */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_rrc_ind_r16_plus_i08(IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x0f: /* rrc (iy+d),a */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_rrc_ind_r16_plus_i08_r08(IY_W, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x10: /* rl (iy+d),b */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_rl_ind_r16_plus_i08_r08(IY_W, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x11: /* rl (iy+d),c */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_rl_ind_r16_plus_i08_r08(IY_W, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x12: /* rl (iy+d),d */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_rl_ind_r16_plus_i08_r08(IY_W, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x13: /* rl (iy+d),e */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_rl_ind_r16_plus_i08_r08(IY_W, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x14: /* rl (iy+d),h */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_rl_ind_r16_plus_i08_r08(IY_W, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x15: /* rl (iy+d),l */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_rl_ind_r16_plus_i08_r08(IY_W, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x16: /* rl (iy+d) */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_rl_ind_r16_plus_i08(IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x17: /* rl (iy+d),a */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_rl_ind_r16_plus_i08_r08(IY_W, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x18: /* rr (iy+d),b */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_rr_ind_r16_plus_i08_r08(IY_W, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x19: /* rr (iy+d),c */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_rr_ind_r16_plus_i08_r08(IY_W, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x1a: /* rr (iy+d),d */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_rr_ind_r16_plus_i08_r08(IY_W, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x1b: /* rr (iy+d),e */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_rr_ind_r16_plus_i08_r08(IY_W, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x1c: /* rr (iy+d),h */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_rr_ind_r16_plus_i08_r08(IY_W, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x1d: /* rr (iy+d),l */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_rr_ind_r16_plus_i08_r08(IY_W, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x1e: /* rr (iy+d) */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_rr_ind_r16_plus_i08(IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x1f: /* rr (iy+d),a */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_rr_ind_r16_plus_i08_r08(IY_W, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x20: /* sla (iy+d),b */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_sla_ind_r16_plus_i08_r08(IY_W, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x21: /* sla (iy+d),c */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_sla_ind_r16_plus_i08_r08(IY_W, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x22: /* sla (iy+d),d */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_sla_ind_r16_plus_i08_r08(IY_W, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x23: /* sla (iy+d),e */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_sla_ind_r16_plus_i08_r08(IY_W, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x24: /* sla (iy+d),h */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_sla_ind_r16_plus_i08_r08(IY_W, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x25: /* sla (iy+d),l */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_sla_ind_r16_plus_i08_r08(IY_W, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x26: /* sla (iy+d) */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_sla_ind_r16_plus_i08(IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x27: /* sla (iy+d),a */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_sla_ind_r16_plus_i08_r08(IY_W, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x28: /* sra (iy+d),b */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_sra_ind_r16_plus_i08_r08(IY_W, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x29: /* sra (iy+d),c */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_sra_ind_r16_plus_i08_r08(IY_W, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x2a: /* sra (iy+d),d */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_sra_ind_r16_plus_i08_r08(IY_W, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x2b: /* sra (iy+d),e */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_sra_ind_r16_plus_i08_r08(IY_W, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x2c: /* sra (iy+d),h */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_sra_ind_r16_plus_i08_r08(IY_W, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x2d: /* sra (iy+d),l */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_sra_ind_r16_plus_i08_r08(IY_W, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x2e: /* sra (iy+d) */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_sra_ind_r16_plus_i08(IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x2f: /* sra (iy+d),a */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_sra_ind_r16_plus_i08_r08(IY_W, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x30: /* sll (iy+d),b */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_sll_ind_r16_plus_i08_r08(IY_W, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x31: /* sll (iy+d),c */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_sll_ind_r16_plus_i08_r08(IY_W, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x32: /* sll (iy+d),d */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_sll_ind_r16_plus_i08_r08(IY_W, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x33: /* sll (iy+d),e */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_sll_ind_r16_plus_i08_r08(IY_W, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x34: /* sll (iy+d),h */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_sll_ind_r16_plus_i08_r08(IY_W, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x35: /* sll (iy+d),l */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_sll_ind_r16_plus_i08_r08(IY_W, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x36: /* sll (iy+d) */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_sll_ind_r16_plus_i08(IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x37: /* sll (iy+d),a */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_sll_ind_r16_plus_i08_r08(IY_W, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x38: /* srl (iy+d),b */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_srl_ind_r16_plus_i08_r08(IY_W, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x39: /* srl (iy+d),c */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_srl_ind_r16_plus_i08_r08(IY_W, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x3a: /* srl (iy+d),d */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_srl_ind_r16_plus_i08_r08(IY_W, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x3b: /* srl (iy+d),e */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_srl_ind_r16_plus_i08_r08(IY_W, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x3c: /* srl (iy+d),h */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_srl_ind_r16_plus_i08_r08(IY_W, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x3d: /* srl (iy+d),l */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_srl_ind_r16_plus_i08_r08(IY_W, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x3e: /* srl (iy+d) */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_srl_ind_r16_plus_i08(IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x3f: /* srl (iy+d),a */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_srl_ind_r16_plus_i08_r08(IY_W, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x40: /* bit 0,(iy+d) */
            {
                constexpr uint32_t m_cycles = (5 - 2);
                constexpr uint32_t t_states = (20 - 8);
                m_bit_b_ind_r16_plus_i08(BIT0, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x41: /* bit 0,(iy+d) */
            {
                constexpr uint32_t m_cycles = (5 - 2);
                constexpr uint32_t t_states = (20 - 8);
                m_bit_b_ind_r16_plus_i08(BIT0, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x42: /* bit 0,(iy+d) */
            {
                constexpr uint32_t m_cycles = (5 - 2);
                constexpr uint32_t t_states = (20 - 8);
                m_bit_b_ind_r16_plus_i08(BIT0, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x43: /* bit 0,(iy+d) */
            {
                constexpr uint32_t m_cycles = (5 - 2);
                constexpr uint32_t t_states = (20 - 8);
                m_bit_b_ind_r16_plus_i08(BIT0, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x44: /* bit 0,(iy+d) */
            {
                constexpr uint32_t m_cycles = (5 - 2);
                constexpr uint32_t t_states = (20 - 8);
                m_bit_b_ind_r16_plus_i08(BIT0, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x45: /* bit 0,(iy+d) */
            {
                constexpr uint32_t m_cycles = (5 - 2);
                constexpr uint32_t t_states = (20 - 8);
                m_bit_b_ind_r16_plus_i08(BIT0, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x46: /* bit 0,(iy+d) */
            {
                constexpr uint32_t m_cycles = (5 - 2);
                constexpr uint32_t t_states = (20 - 8);
                m_bit_b_ind_r16_plus_i08(BIT0, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x47: /* bit 0,(iy+d) */
            {
                constexpr uint32_t m_cycles = (5 - 2);
                constexpr uint32_t t_states = (20 - 8);
                m_bit_b_ind_r16_plus_i08(BIT0, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x48: /* bit 1,(iy+d) */
            {
                constexpr uint32_t m_cycles = (5 - 2);
                constexpr uint32_t t_states = (20 - 8);
                m_bit_b_ind_r16_plus_i08(BIT1, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x49: /* bit 1,(iy+d) */
            {
                constexpr uint32_t m_cycles = (5 - 2);
                constexpr uint32_t t_states = (20 - 8);
                m_bit_b_ind_r16_plus_i08(BIT1, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x4a: /* bit 1,(iy+d) */
            {
                constexpr uint32_t m_cycles = (5 - 2);
                constexpr uint32_t t_states = (20 - 8);
                m_bit_b_ind_r16_plus_i08(BIT1, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x4b: /* bit 1,(iy+d) */
            {
                constexpr uint32_t m_cycles = (5 - 2);
                constexpr uint32_t t_states = (20 - 8);
                m_bit_b_ind_r16_plus_i08(BIT1, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x4c: /* bit 1,(iy+d) */
            {
                constexpr uint32_t m_cycles = (5 - 2);
                constexpr uint32_t t_states = (20 - 8);
                m_bit_b_ind_r16_plus_i08(BIT1, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x4d: /* bit 1,(iy+d) */
            {
                constexpr uint32_t m_cycles = (5 - 2);
                constexpr uint32_t t_states = (20 - 8);
                m_bit_b_ind_r16_plus_i08(BIT1, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x4e: /* bit 1,(iy+d) */
            {
                constexpr uint32_t m_cycles = (5 - 2);
                constexpr uint32_t t_states = (20 - 8);
                m_bit_b_ind_r16_plus_i08(BIT1, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x4f: /* bit 1,(iy+d) */
            {
                constexpr uint32_t m_cycles = (5 - 2);
                constexpr uint32_t t_states = (20 - 8);
                m_bit_b_ind_r16_plus_i08(BIT1, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x50: /* bit 2,(iy+d) */
            {
                constexpr uint32_t m_cycles = (5 - 2);
                constexpr uint32_t t_states = (20 - 8);
                m_bit_b_ind_r16_plus_i08(BIT2, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x51: /* bit 2,(iy+d) */
            {
                constexpr uint32_t m_cycles = (5 - 2);
                constexpr uint32_t t_states = (20 - 8);
                m_bit_b_ind_r16_plus_i08(BIT2, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x52: /* bit 2,(iy+d) */
            {
                constexpr uint32_t m_cycles = (5 - 2);
                constexpr uint32_t t_states = (20 - 8);
                m_bit_b_ind_r16_plus_i08(BIT2, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x53: /* bit 2,(iy+d) */
            {
                constexpr uint32_t m_cycles = (5 - 2);
                constexpr uint32_t t_states = (20 - 8);
                m_bit_b_ind_r16_plus_i08(BIT2, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x54: /* bit 2,(iy+d) */
            {
                constexpr uint32_t m_cycles = (5 - 2);
                constexpr uint32_t t_states = (20 - 8);
                m_bit_b_ind_r16_plus_i08(BIT2, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x55: /* bit 2,(iy+d) */
            {
                constexpr uint32_t m_cycles = (5 - 2);
                constexpr uint32_t t_states = (20 - 8);
                m_bit_b_ind_r16_plus_i08(BIT2, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x56: /* bit 2,(iy+d) */
            {
                constexpr uint32_t m_cycles = (5 - 2);
                constexpr uint32_t t_states = (20 - 8);
                m_bit_b_ind_r16_plus_i08(BIT2, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x57: /* bit 2,(iy+d) */
            {
                constexpr uint32_t m_cycles = (5 - 2);
                constexpr uint32_t t_states = (20 - 8);
                m_bit_b_ind_r16_plus_i08(BIT2, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x58: /* bit 3,(iy+d) */
            {
                constexpr uint32_t m_cycles = (5 - 2);
                constexpr uint32_t t_states = (20 - 8);
                m_bit_b_ind_r16_plus_i08(BIT3, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x59: /* bit 3,(iy+d) */
            {
                constexpr uint32_t m_cycles = (5 - 2);
                constexpr uint32_t t_states = (20 - 8);
                m_bit_b_ind_r16_plus_i08(BIT3, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x5a: /* bit 3,(iy+d) */
            {
                constexpr uint32_t m_cycles = (5 - 2);
                constexpr uint32_t t_states = (20 - 8);
                m_bit_b_ind_r16_plus_i08(BIT3, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x5b: /* bit 3,(iy+d) */
            {
                constexpr uint32_t m_cycles = (5 - 2);
                constexpr uint32_t t_states = (20 - 8);
                m_bit_b_ind_r16_plus_i08(BIT3, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x5c: /* bit 3,(iy+d) */
            {
                constexpr uint32_t m_cycles = (5 - 2);
                constexpr uint32_t t_states = (20 - 8);
                m_bit_b_ind_r16_plus_i08(BIT3, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x5d: /* bit 3,(iy+d) */
            {
                constexpr uint32_t m_cycles = (5 - 2);
                constexpr uint32_t t_states = (20 - 8);
                m_bit_b_ind_r16_plus_i08(BIT3, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x5e: /* bit 3,(iy+d) */
            {
                constexpr uint32_t m_cycles = (5 - 2);
                constexpr uint32_t t_states = (20 - 8);
                m_bit_b_ind_r16_plus_i08(BIT3, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x5f: /* bit 3,(iy+d) */
            {
                constexpr uint32_t m_cycles = (5 - 2);
                constexpr uint32_t t_states = (20 - 8);
                m_bit_b_ind_r16_plus_i08(BIT3, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x60: /* bit 4,(iy+d) */
            {
                constexpr uint32_t m_cycles = (5 - 2);
                constexpr uint32_t t_states = (20 - 8);
                m_bit_b_ind_r16_plus_i08(BIT4, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x61: /* bit 4,(iy+d) */
            {
                constexpr uint32_t m_cycles = (5 - 2);
                constexpr uint32_t t_states = (20 - 8);
                m_bit_b_ind_r16_plus_i08(BIT4, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x62: /* bit 4,(iy+d) */
            {
                constexpr uint32_t m_cycles = (5 - 2);
                constexpr uint32_t t_states = (20 - 8);
                m_bit_b_ind_r16_plus_i08(BIT4, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x63: /* bit 4,(iy+d) */
            {
                constexpr uint32_t m_cycles = (5 - 2);
                constexpr uint32_t t_states = (20 - 8);
                m_bit_b_ind_r16_plus_i08(BIT4, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x64: /* bit 4,(iy+d) */
            {
                constexpr uint32_t m_cycles = (5 - 2);
                constexpr uint32_t t_states = (20 - 8);
                m_bit_b_ind_r16_plus_i08(BIT4, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x65: /* bit 4,(iy+d) */
            {
                constexpr uint32_t m_cycles = (5 - 2);
                constexpr uint32_t t_states = (20 - 8);
                m_bit_b_ind_r16_plus_i08(BIT4, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x66: /* bit 4,(iy+d) */
            {
                constexpr uint32_t m_cycles = (5 - 2);
                constexpr uint32_t t_states = (20 - 8);
                m_bit_b_ind_r16_plus_i08(BIT4, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x67: /* bit 4,(iy+d) */
            {
                constexpr uint32_t m_cycles = (5 - 2);
                constexpr uint32_t t_states = (20 - 8);
                m_bit_b_ind_r16_plus_i08(BIT4, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x68: /* bit 5,(iy+d) */
            {
                constexpr uint32_t m_cycles = (5 - 2);
                constexpr uint32_t t_states = (20 - 8);
                m_bit_b_ind_r16_plus_i08(BIT5, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x69: /* bit 5,(iy+d) */
            {
                constexpr uint32_t m_cycles = (5 - 2);
                constexpr uint32_t t_states = (20 - 8);
                m_bit_b_ind_r16_plus_i08(BIT5, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x6a: /* bit 5,(iy+d) */
            {
                constexpr uint32_t m_cycles = (5 - 2);
                constexpr uint32_t t_states = (20 - 8);
                m_bit_b_ind_r16_plus_i08(BIT5, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x6b: /* bit 5,(iy+d) */
            {
                constexpr uint32_t m_cycles = (5 - 2);
                constexpr uint32_t t_states = (20 - 8);
                m_bit_b_ind_r16_plus_i08(BIT5, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x6c: /* bit 5,(iy+d) */
            {
                constexpr uint32_t m_cycles = (5 - 2);
                constexpr uint32_t t_states = (20 - 8);
                m_bit_b_ind_r16_plus_i08(BIT5, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x6d: /* bit 5,(iy+d) */
            {
                constexpr uint32_t m_cycles = (5 - 2);
                constexpr uint32_t t_states = (20 - 8);
                m_bit_b_ind_r16_plus_i08(BIT5, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x6e: /* bit 5,(iy+d) */
            {
                constexpr uint32_t m_cycles = (5 - 2);
                constexpr uint32_t t_states = (20 - 8);
                m_bit_b_ind_r16_plus_i08(BIT5, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x6f: /* bit 5,(iy+d) */
            {
                constexpr uint32_t m_cycles = (5 - 2);
                constexpr uint32_t t_states = (20 - 8);
                m_bit_b_ind_r16_plus_i08(BIT5, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x70: /* bit 6,(iy+d) */
            {
                constexpr uint32_t m_cycles = (5 - 2);
                constexpr uint32_t t_states = (20 - 8);
                m_bit_b_ind_r16_plus_i08(BIT6, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x71: /* bit 6,(iy+d) */
            {
                constexpr uint32_t m_cycles = (5 - 2);
                constexpr uint32_t t_states = (20 - 8);
                m_bit_b_ind_r16_plus_i08(BIT6, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x72: /* bit 6,(iy+d) */
            {
                constexpr uint32_t m_cycles = (5 - 2);
                constexpr uint32_t t_states = (20 - 8);
                m_bit_b_ind_r16_plus_i08(BIT6, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x73: /* bit 6,(iy+d) */
            {
                constexpr uint32_t m_cycles = (5 - 2);
                constexpr uint32_t t_states = (20 - 8);
                m_bit_b_ind_r16_plus_i08(BIT6, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x74: /* bit 6,(iy+d) */
            {
                constexpr uint32_t m_cycles = (5 - 2);
                constexpr uint32_t t_states = (20 - 8);
                m_bit_b_ind_r16_plus_i08(BIT6, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x75: /* bit 6,(iy+d) */
            {
                constexpr uint32_t m_cycles = (5 - 2);
                constexpr uint32_t t_states = (20 - 8);
                m_bit_b_ind_r16_plus_i08(BIT6, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x76: /* bit 6,(iy+d) */
            {
                constexpr uint32_t m_cycles = (5 - 2);
                constexpr uint32_t t_states = (20 - 8);
                m_bit_b_ind_r16_plus_i08(BIT6, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x77: /* bit 6,(iy+d) */
            {
                constexpr uint32_t m_cycles = (5 - 2);
                constexpr uint32_t t_states = (20 - 8);
                m_bit_b_ind_r16_plus_i08(BIT6, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x78: /* bit 7,(iy+d) */
            {
                constexpr uint32_t m_cycles = (5 - 2);
                constexpr uint32_t t_states = (20 - 8);
                m_bit_b_ind_r16_plus_i08(BIT7, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x79: /* bit 7,(iy+d) */
            {
                constexpr uint32_t m_cycles = (5 - 2);
                constexpr uint32_t t_states = (20 - 8);
                m_bit_b_ind_r16_plus_i08(BIT7, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x7a: /* bit 7,(iy+d) */
            {
                constexpr uint32_t m_cycles = (5 - 2);
                constexpr uint32_t t_states = (20 - 8);
                m_bit_b_ind_r16_plus_i08(BIT7, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x7b: /* bit 7,(iy+d) */
            {
                constexpr uint32_t m_cycles = (5 - 2);
                constexpr uint32_t t_states = (20 - 8);
                m_bit_b_ind_r16_plus_i08(BIT7, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x7c: /* bit 7,(iy+d) */
            {
                constexpr uint32_t m_cycles = (5 - 2);
                constexpr uint32_t t_states = (20 - 8);
                m_bit_b_ind_r16_plus_i08(BIT7, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x7d: /* bit 7,(iy+d) */
            {
                constexpr uint32_t m_cycles = (5 - 2);
                constexpr uint32_t t_states = (20 - 8);
                m_bit_b_ind_r16_plus_i08(BIT7, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x7e: /* bit 7,(iy+d) */
            {
                constexpr uint32_t m_cycles = (5 - 2);
                constexpr uint32_t t_states = (20 - 8);
                m_bit_b_ind_r16_plus_i08(BIT7, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x7f: /* bit 7,(iy+d) */
            {
                constexpr uint32_t m_cycles = (5 - 2);
                constexpr uint32_t t_states = (20 - 8);
                m_bit_b_ind_r16_plus_i08(BIT7, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x80: /* res 0,(iy+d),b */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_res_b_ind_r16_plus_i08_r08(BIT0, IY_W, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x81: /* res 0,(iy+d),c */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_res_b_ind_r16_plus_i08_r08(BIT0, IY_W, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x82: /* res 0,(iy+d),d */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_res_b_ind_r16_plus_i08_r08(BIT0, IY_W, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x83: /* res 0,(iy+d),e */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_res_b_ind_r16_plus_i08_r08(BIT0, IY_W, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x84: /* res 0,(iy+d),h */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_res_b_ind_r16_plus_i08_r08(BIT0, IY_W, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x85: /* res 0,(iy+d),l */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_res_b_ind_r16_plus_i08_r08(BIT0, IY_W, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x86: /* res 0,(iy+d) */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_res_b_ind_r16_plus_i08(BIT0, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x87: /* res 0,(iy+d),a */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_res_b_ind_r16_plus_i08_r08(BIT0, IY_W, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x88: /* res 1,(iy+d),b */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_res_b_ind_r16_plus_i08_r08(BIT1, IY_W, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x89: /* res 1,(iy+d),c */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_res_b_ind_r16_plus_i08_r08(BIT1, IY_W, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x8a: /* res 1,(iy+d),d */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_res_b_ind_r16_plus_i08_r08(BIT1, IY_W, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x8b: /* res 1,(iy+d),e */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_res_b_ind_r16_plus_i08_r08(BIT1, IY_W, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x8c: /* res 1,(iy+d),h */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_res_b_ind_r16_plus_i08_r08(BIT1, IY_W, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x8d: /* res 1,(iy+d),l */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_res_b_ind_r16_plus_i08_r08(BIT1, IY_W, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x8e: /* res 1,(iy+d) */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_res_b_ind_r16_plus_i08(BIT1, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x8f: /* res 1,(iy+d),a */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_res_b_ind_r16_plus_i08_r08(BIT1, IY_W, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x90: /* res 2,(iy+d),b */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_res_b_ind_r16_plus_i08_r08(BIT2, IY_W, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x91: /* res 2,(iy+d),c */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_res_b_ind_r16_plus_i08_r08(BIT2, IY_W, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x92: /* res 2,(iy+d),d */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_res_b_ind_r16_plus_i08_r08(BIT2, IY_W, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x93: /* res 2,(iy+d),e */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_res_b_ind_r16_plus_i08_r08(BIT2, IY_W, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x94: /* res 2,(iy+d),h */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_res_b_ind_r16_plus_i08_r08(BIT2, IY_W, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x95: /* res 2,(iy+d),l */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_res_b_ind_r16_plus_i08_r08(BIT2, IY_W, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x96: /* res 2,(iy+d) */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_res_b_ind_r16_plus_i08(BIT2, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x97: /* res 2,(iy+d),a */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_res_b_ind_r16_plus_i08_r08(BIT2, IY_W, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x98: /* res 3,(iy+d),b */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_res_b_ind_r16_plus_i08_r08(BIT3, IY_W, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x99: /* res 3,(iy+d),c */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_res_b_ind_r16_plus_i08_r08(BIT3, IY_W, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x9a: /* res 3,(iy+d),d */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_res_b_ind_r16_plus_i08_r08(BIT3, IY_W, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x9b: /* res 3,(iy+d),e */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_res_b_ind_r16_plus_i08_r08(BIT3, IY_W, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x9c: /* res 3,(iy+d),h */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_res_b_ind_r16_plus_i08_r08(BIT3, IY_W, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x9d: /* res 3,(iy+d),l */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_res_b_ind_r16_plus_i08_r08(BIT3, IY_W, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x9e: /* res 3,(iy+d) */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_res_b_ind_r16_plus_i08(BIT3, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x9f: /* res 3,(iy+d),a */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_res_b_ind_r16_plus_i08_r08(BIT3, IY_W, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xa0: /* res 4,(iy+d),b */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_res_b_ind_r16_plus_i08_r08(BIT4, IY_W, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xa1: /* res 4,(iy+d),c */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_res_b_ind_r16_plus_i08_r08(BIT4, IY_W, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xa2: /* res 4,(iy+d),d */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_res_b_ind_r16_plus_i08_r08(BIT4, IY_W, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xa3: /* res 4,(iy+d),e */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_res_b_ind_r16_plus_i08_r08(BIT4, IY_W, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xa4: /* res 4,(iy+d),h */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_res_b_ind_r16_plus_i08_r08(BIT4, IY_W, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xa5: /* res 4,(iy+d),l */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_res_b_ind_r16_plus_i08_r08(BIT4, IY_W, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xa6: /* res 4,(iy+d) */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_res_b_ind_r16_plus_i08(BIT4, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xa7: /* res 4,(iy+d),a */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_res_b_ind_r16_plus_i08_r08(BIT4, IY_W, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xa8: /* res 5,(iy+d),b */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_res_b_ind_r16_plus_i08_r08(BIT5, IY_W, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xa9: /* res 5,(iy+d),c */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_res_b_ind_r16_plus_i08_r08(BIT5, IY_W, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xaa: /* res 5,(iy+d),d */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_res_b_ind_r16_plus_i08_r08(BIT5, IY_W, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xab: /* res 5,(iy+d),e */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_res_b_ind_r16_plus_i08_r08(BIT5, IY_W, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xac: /* res 5,(iy+d),h */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_res_b_ind_r16_plus_i08_r08(BIT5, IY_W, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xad: /* res 5,(iy+d),l */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_res_b_ind_r16_plus_i08_r08(BIT5, IY_W, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xae: /* res 5,(iy+d) */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_res_b_ind_r16_plus_i08(BIT5, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xaf: /* res 5,(iy+d),a */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_res_b_ind_r16_plus_i08_r08(BIT5, IY_W, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xb0: /* res 6,(iy+d),b */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_res_b_ind_r16_plus_i08_r08(BIT6, IY_W, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xb1: /* res 6,(iy+d),c */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_res_b_ind_r16_plus_i08_r08(BIT6, IY_W, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xb2: /* res 6,(iy+d),d */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_res_b_ind_r16_plus_i08_r08(BIT6, IY_W, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xb3: /* res 6,(iy+d),e */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_res_b_ind_r16_plus_i08_r08(BIT6, IY_W, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xb4: /* res 6,(iy+d),h */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_res_b_ind_r16_plus_i08_r08(BIT6, IY_W, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xb5: /* res 6,(iy+d),l */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_res_b_ind_r16_plus_i08_r08(BIT6, IY_W, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xb6: /* res 6,(iy+d) */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_res_b_ind_r16_plus_i08(BIT6, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xb7: /* res 6,(iy+d),a */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_res_b_ind_r16_plus_i08_r08(BIT6, IY_W, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xb8: /* res 7,(iy+d),b */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_res_b_ind_r16_plus_i08_r08(BIT7, IY_W, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xb9: /* res 7,(iy+d),c */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_res_b_ind_r16_plus_i08_r08(BIT7, IY_W, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xba: /* res 7,(iy+d),d */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_res_b_ind_r16_plus_i08_r08(BIT7, IY_W, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xbb: /* res 7,(iy+d),e */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_res_b_ind_r16_plus_i08_r08(BIT7, IY_W, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xbc: /* res 7,(iy+d),h */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_res_b_ind_r16_plus_i08_r08(BIT7, IY_W, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xbd: /* res 7,(iy+d),l */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_res_b_ind_r16_plus_i08_r08(BIT7, IY_W, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xbe: /* res 7,(iy+d) */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_res_b_ind_r16_plus_i08(BIT7, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xbf: /* res 7,(iy+d),a */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_res_b_ind_r16_plus_i08_r08(BIT7, IY_W, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xc0: /* set 0,(iy+d),b */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_set_b_ind_r16_plus_i08_r08(BIT0, IY_W, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xc1: /* set 0,(iy+d),c */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_set_b_ind_r16_plus_i08_r08(BIT0, IY_W, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xc2: /* set 0,(iy+d),d */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_set_b_ind_r16_plus_i08_r08(BIT0, IY_W, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xc3: /* set 0,(iy+d),e */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_set_b_ind_r16_plus_i08_r08(BIT0, IY_W, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xc4: /* set 0,(iy+d),h */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_set_b_ind_r16_plus_i08_r08(BIT0, IY_W, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xc5: /* set 0,(iy+d),l */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_set_b_ind_r16_plus_i08_r08(BIT0, IY_W, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xc6: /* set 0,(iy+d) */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_set_b_ind_r16_plus_i08(BIT0, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xc7: /* set 0,(iy+d),a */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_set_b_ind_r16_plus_i08_r08(BIT0, IY_W, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xc8: /* set 1,(iy+d),b */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_set_b_ind_r16_plus_i08_r08(BIT1, IY_W, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xc9: /* set 1,(iy+d),c */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_set_b_ind_r16_plus_i08_r08(BIT1, IY_W, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xca: /* set 1,(iy+d),d */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_set_b_ind_r16_plus_i08_r08(BIT1, IY_W, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xcb: /* set 1,(iy+d),e */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_set_b_ind_r16_plus_i08_r08(BIT1, IY_W, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xcc: /* set 1,(iy+d),h */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_set_b_ind_r16_plus_i08_r08(BIT1, IY_W, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xcd: /* set 1,(iy+d),l */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_set_b_ind_r16_plus_i08_r08(BIT1, IY_W, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xce: /* set 1,(iy+d) */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_set_b_ind_r16_plus_i08(BIT1, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xcf: /* set 1,(iy+d),a */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_set_b_ind_r16_plus_i08_r08(BIT1, IY_W, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xd0: /* set 2,(iy+d),b */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_set_b_ind_r16_plus_i08_r08(BIT2, IY_W, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xd1: /* set 2,(iy+d),c */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_set_b_ind_r16_plus_i08_r08(BIT2, IY_W, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xd2: /* set 2,(iy+d),d */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_set_b_ind_r16_plus_i08_r08(BIT2, IY_W, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xd3: /* set 2,(iy+d),e */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_set_b_ind_r16_plus_i08_r08(BIT2, IY_W, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xd4: /* set 2,(iy+d),h */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_set_b_ind_r16_plus_i08_r08(BIT2, IY_W, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xd5: /* set 2,(iy+d),l */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_set_b_ind_r16_plus_i08_r08(BIT2, IY_W, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xd6: /* set 2,(iy+d) */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_set_b_ind_r16_plus_i08(BIT2, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xd7: /* set 2,(iy+d),a */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_set_b_ind_r16_plus_i08_r08(BIT2, IY_W, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xd8: /* set 3,(iy+d),b */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_set_b_ind_r16_plus_i08_r08(BIT3, IY_W, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xd9: /* set 3,(iy+d),c */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_set_b_ind_r16_plus_i08_r08(BIT3, IY_W, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xda: /* set 3,(iy+d),d */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_set_b_ind_r16_plus_i08_r08(BIT3, IY_W, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xdb: /* set 3,(iy+d),e */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_set_b_ind_r16_plus_i08_r08(BIT3, IY_W, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xdc: /* set 3,(iy+d),h */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_set_b_ind_r16_plus_i08_r08(BIT3, IY_W, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xdd: /* set 3,(iy+d),l */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_set_b_ind_r16_plus_i08_r08(BIT3, IY_W, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xde: /* set 3,(iy+d) */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_set_b_ind_r16_plus_i08(BIT3, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xdf: /* set 3,(iy+d),a */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_set_b_ind_r16_plus_i08_r08(BIT3, IY_W, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xe0: /* set 4,(iy+d),b */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_set_b_ind_r16_plus_i08_r08(BIT4, IY_W, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xe1: /* set 4,(iy+d),c */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_set_b_ind_r16_plus_i08_r08(BIT4, IY_W, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xe2: /* set 4,(iy+d),d */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_set_b_ind_r16_plus_i08_r08(BIT4, IY_W, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xe3: /* set 4,(iy+d),e */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_set_b_ind_r16_plus_i08_r08(BIT4, IY_W, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xe4: /* set 4,(iy+d),h */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_set_b_ind_r16_plus_i08_r08(BIT4, IY_W, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xe5: /* set 4,(iy+d),l */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_set_b_ind_r16_plus_i08_r08(BIT4, IY_W, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xe6: /* set 4,(iy+d) */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_set_b_ind_r16_plus_i08(BIT4, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xe7: /* set 4,(iy+d),a */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_set_b_ind_r16_plus_i08_r08(BIT4, IY_W, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xe8: /* set 5,(iy+d),b */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_set_b_ind_r16_plus_i08_r08(BIT5, IY_W, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xe9: /* set 5,(iy+d),c */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_set_b_ind_r16_plus_i08_r08(BIT5, IY_W, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xea: /* set 5,(iy+d),d */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_set_b_ind_r16_plus_i08_r08(BIT5, IY_W, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xeb: /* set 5,(iy+d),e */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_set_b_ind_r16_plus_i08_r08(BIT5, IY_W, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xec: /* set 5,(iy+d),h */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_set_b_ind_r16_plus_i08_r08(BIT5, IY_W, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xed: /* set 5,(iy+d),l */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_set_b_ind_r16_plus_i08_r08(BIT5, IY_W, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xee: /* set 5,(iy+d) */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_set_b_ind_r16_plus_i08(BIT5, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xef: /* set 5,(iy+d),a */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_set_b_ind_r16_plus_i08_r08(BIT5, IY_W, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xf0: /* set 6,(iy+d),b */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_set_b_ind_r16_plus_i08_r08(BIT6, IY_W, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xf1: /* set 6,(iy+d),c */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_set_b_ind_r16_plus_i08_r08(BIT6, IY_W, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xf2: /* set 6,(iy+d),d */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_set_b_ind_r16_plus_i08_r08(BIT6, IY_W, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xf3: /* set 6,(iy+d),e */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_set_b_ind_r16_plus_i08_r08(BIT6, IY_W, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xf4: /* set 6,(iy+d),h */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_set_b_ind_r16_plus_i08_r08(BIT6, IY_W, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xf5: /* set 6,(iy+d),l */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_set_b_ind_r16_plus_i08_r08(BIT6, IY_W, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xf6: /* set 6,(iy+d) */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_set_b_ind_r16_plus_i08(BIT6, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xf7: /* set 6,(iy+d),a */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_set_b_ind_r16_plus_i08_r08(BIT6, IY_W, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xf8: /* set 7,(iy+d),b */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_set_b_ind_r16_plus_i08_r08(BIT7, IY_W, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xf9: /* set 7,(iy+d),c */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_set_b_ind_r16_plus_i08_r08(BIT7, IY_W, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xfa: /* set 7,(iy+d),d */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_set_b_ind_r16_plus_i08_r08(BIT7, IY_W, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xfb: /* set 7,(iy+d),e */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_set_b_ind_r16_plus_i08_r08(BIT7, IY_W, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xfc: /* set 7,(iy+d),h */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_set_b_ind_r16_plus_i08_r08(BIT7, IY_W, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xfd: /* set 7,(iy+d),l */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_set_b_ind_r16_plus_i08_r08(BIT7, IY_W, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xfe: /* set 7,(iy+d) */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_set_b_ind_r16_plus_i08(BIT7, IY_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xff: /* set 7,(iy+d),a */
            {
                constexpr uint32_t m_cycles = (6 - 2);
                constexpr uint32_t t_states = (23 - 8);
                m_set_b_ind_r16_plus_i08_r08(BIT7, IY_W, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
