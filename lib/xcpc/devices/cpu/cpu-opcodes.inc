        case 0x00: /* nop */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_nop();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x01: /* ld bc,nn */
            {
                constexpr uint32_t m_cycles = (2);
                constexpr uint32_t t_states = (10);
                m_ld_r16_i16(BC_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x02: /* ld (bc),a */
            {
                constexpr uint32_t m_cycles = (2);
                constexpr uint32_t t_states = (7);
                m_ld_ind_r16_r08(BC_W, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x03: /* inc bc */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (6);
                m_inc_r16(BC_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x04: /* inc b */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_inc_r08(BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x05: /* dec b */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_dec_r08(BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x06: /* ld b,n */
            {
                constexpr uint32_t m_cycles = (2);
                constexpr uint32_t t_states = (7);
                m_ld_r08_i08(BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x07: /* rlca */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_rlca();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x08: /* ex af,af' */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_ex_r16_r16(AF_W, AF_P);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x09: /* add hl,bc */
            {
                constexpr uint32_t m_cycles = (3);
                constexpr uint32_t t_states = (11);
                m_add_r16_r16(HL_W, BC_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x0a: /* ld a,(bc) */
            {
                constexpr uint32_t m_cycles = (2);
                constexpr uint32_t t_states = (7);
                m_ld_r08_ind_r16(AF_H, BC_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x0b: /* dec bc */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (6);
                m_dec_r16(BC_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x0c: /* inc c */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_inc_r08(BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x0d: /* dec c */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_dec_r08(BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x0e: /* ld c,n */
            {
                constexpr uint32_t m_cycles = (2);
                constexpr uint32_t t_states = (7);
                m_ld_r08_i08(BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x0f: /* rrca */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_rrca();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x10: /* djnz d */
            {
                constexpr uint32_t m_cycles = (2);
                constexpr uint32_t t_states = (8);
                m_djnz_i08();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x11: /* ld de,nn */
            {
                constexpr uint32_t m_cycles = (2);
                constexpr uint32_t t_states = (10);
                m_ld_r16_i16(DE_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x12: /* ld (de),a */
            {
                constexpr uint32_t m_cycles = (2);
                constexpr uint32_t t_states = (7);
                m_ld_ind_r16_r08(DE_W, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x13: /* inc de */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (6);
                m_inc_r16(DE_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x14: /* inc d */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_inc_r08(DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x15: /* dec d */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_dec_r08(DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x16: /* ld d,n */
            {
                constexpr uint32_t m_cycles = (2);
                constexpr uint32_t t_states = (7);
                m_ld_r08_i08(DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x17: /* rla */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_rla();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x18: /* jr d */
            {
                constexpr uint32_t m_cycles = (3);
                constexpr uint32_t t_states = (12);
                m_jr_i08();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x19: /* add hl,de */
            {
                constexpr uint32_t m_cycles = (3);
                constexpr uint32_t t_states = (11);
                m_add_r16_r16(HL_W, DE_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x1a: /* ld a,(de) */
            {
                constexpr uint32_t m_cycles = (2);
                constexpr uint32_t t_states = (7);
                m_ld_r08_ind_r16(AF_H, DE_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x1b: /* dec de */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (6);
                m_dec_r16(DE_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x1c: /* inc e */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_inc_r08(DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x1d: /* dec e */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_dec_r08(DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x1e: /* ld e,n */
            {
                constexpr uint32_t m_cycles = (2);
                constexpr uint32_t t_states = (7);
                m_ld_r08_i08(DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x1f: /* rra */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_rra();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x20: /* jr nz,d */
            {
                constexpr uint32_t m_cycles = (2);
                constexpr uint32_t t_states = (7);
                m_jr_nz_i08();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x21: /* ld hl,nn */
            {
                constexpr uint32_t m_cycles = (2);
                constexpr uint32_t t_states = (10);
                m_ld_r16_i16(HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x22: /* ld (nn),hl */
            {
                constexpr uint32_t m_cycles = (5);
                constexpr uint32_t t_states = (16);
                m_ld_ind_i16_r16(HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x23: /* inc hl */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (6);
                m_inc_r16(HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x24: /* inc h */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_inc_r08(HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x25: /* dec h */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_dec_r08(HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x26: /* ld h,n */
            {
                constexpr uint32_t m_cycles = (2);
                constexpr uint32_t t_states = (7);
                m_ld_r08_i08(HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x27: /* daa */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_daa();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x28: /* jr z,d */
            {
                constexpr uint32_t m_cycles = (2);
                constexpr uint32_t t_states = (7);
                m_jr_z_i08();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x29: /* add hl,hl */
            {
                constexpr uint32_t m_cycles = (3);
                constexpr uint32_t t_states = (11);
                m_add_r16_r16(HL_W, HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x2a: /* ld hl,(nn) */
            {
                constexpr uint32_t m_cycles = (5);
                constexpr uint32_t t_states = (16);
                m_ld_r16_ind_i16(HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x2b: /* dec hl */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (6);
                m_dec_r16(HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x2c: /* inc l */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_inc_r08(HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x2d: /* dec l */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_dec_r08(HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x2e: /* ld l,n */
            {
                constexpr uint32_t m_cycles = (2);
                constexpr uint32_t t_states = (7);
                m_ld_r08_i08(HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x2f: /* cpl */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_cpl();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x30: /* jr nc,d */
            {
                constexpr uint32_t m_cycles = (2);
                constexpr uint32_t t_states = (7);
                m_jr_nc_i08();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x31: /* ld sp,nn */
            {
                constexpr uint32_t m_cycles = (2);
                constexpr uint32_t t_states = (10);
                m_ld_r16_i16(SP_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x32: /* ld (nn),a */
            {
                constexpr uint32_t m_cycles = (4);
                constexpr uint32_t t_states = (13);
                m_ld_ind_i16_r08(AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x33: /* inc sp */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (6);
                m_inc_r16(SP_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x34: /* inc (hl) */
            {
                constexpr uint32_t m_cycles = (3);
                constexpr uint32_t t_states = (11);
                m_inc_ind_r16(HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x35: /* dec (hl) */
            {
                constexpr uint32_t m_cycles = (3);
                constexpr uint32_t t_states = (11);
                m_dec_ind_r16(HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x36: /* ld (hl),n */
            {
                constexpr uint32_t m_cycles = (3);
                constexpr uint32_t t_states = (10);
                m_ld_ind_r16_i08(HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x37: /* scf */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_scf();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x38: /* jr c,d */
            {
                constexpr uint32_t m_cycles = (2);
                constexpr uint32_t t_states = (7);
                m_jr_c_i08();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x39: /* add hl,sp */
            {
                constexpr uint32_t m_cycles = (3);
                constexpr uint32_t t_states = (11);
                m_add_r16_r16(HL_W, SP_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x3a: /* ld a,(nn) */
            {
                constexpr uint32_t m_cycles = (4);
                constexpr uint32_t t_states = (13);
                m_ld_r08_ind_i16(AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x3b: /* dec sp */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (6);
                m_dec_r16(SP_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x3c: /* inc a */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_inc_r08(AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x3d: /* dec a */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_dec_r08(AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x3e: /* ld a,n */
            {
                constexpr uint32_t m_cycles = (2);
                constexpr uint32_t t_states = (7);
                m_ld_r08_i08(AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x3f: /* ccf */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_ccf();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x40: /* ld b,b */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_ld_r08_r08(BC_H, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x41: /* ld b,c */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_ld_r08_r08(BC_H, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x42: /* ld b,d */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_ld_r08_r08(BC_H, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x43: /* ld b,e */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_ld_r08_r08(BC_H, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x44: /* ld b,h */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_ld_r08_r08(BC_H, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x45: /* ld b,l */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_ld_r08_r08(BC_H, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x46: /* ld b,(hl) */
            {
                constexpr uint32_t m_cycles = (2);
                constexpr uint32_t t_states = (7);
                m_ld_r08_ind_r16(BC_H, HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x47: /* ld b,a */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_ld_r08_r08(BC_H, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x48: /* ld c,b */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_ld_r08_r08(BC_L, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x49: /* ld c,c */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_ld_r08_r08(BC_L, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x4a: /* ld c,d */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_ld_r08_r08(BC_L, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x4b: /* ld c,e */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_ld_r08_r08(BC_L, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x4c: /* ld c,h */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_ld_r08_r08(BC_L, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x4d: /* ld c,l */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_ld_r08_r08(BC_L, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x4e: /* ld c,(hl) */
            {
                constexpr uint32_t m_cycles = (2);
                constexpr uint32_t t_states = (7);
                m_ld_r08_ind_r16(BC_L, HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x4f: /* ld c,a */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_ld_r08_r08(BC_L, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x50: /* ld d,b */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_ld_r08_r08(DE_H, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x51: /* ld d,c */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_ld_r08_r08(DE_H, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x52: /* ld d,d */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_ld_r08_r08(DE_H, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x53: /* ld d,e */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_ld_r08_r08(DE_H, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x54: /* ld d,h */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_ld_r08_r08(DE_H, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x55: /* ld d,l */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_ld_r08_r08(DE_H, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x56: /* ld d,(hl) */
            {
                constexpr uint32_t m_cycles = (2);
                constexpr uint32_t t_states = (7);
                m_ld_r08_ind_r16(DE_H, HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x57: /* ld d,a */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_ld_r08_r08(DE_H, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x58: /* ld e,b */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_ld_r08_r08(DE_L, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x59: /* ld e,c */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_ld_r08_r08(DE_L, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x5a: /* ld e,d */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_ld_r08_r08(DE_L, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x5b: /* ld e,e */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_ld_r08_r08(DE_L, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x5c: /* ld e,h */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_ld_r08_r08(DE_L, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x5d: /* ld e,l */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_ld_r08_r08(DE_L, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x5e: /* ld e,(hl) */
            {
                constexpr uint32_t m_cycles = (2);
                constexpr uint32_t t_states = (7);
                m_ld_r08_ind_r16(DE_L, HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x5f: /* ld e,a */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_ld_r08_r08(DE_L, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x60: /* ld h,b */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_ld_r08_r08(HL_H, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x61: /* ld h,c */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_ld_r08_r08(HL_H, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x62: /* ld h,d */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_ld_r08_r08(HL_H, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x63: /* ld h,e */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_ld_r08_r08(HL_H, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x64: /* ld h,h */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_ld_r08_r08(HL_H, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x65: /* ld h,l */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_ld_r08_r08(HL_H, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x66: /* ld h,(hl) */
            {
                constexpr uint32_t m_cycles = (2);
                constexpr uint32_t t_states = (7);
                m_ld_r08_ind_r16(HL_H, HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x67: /* ld h,a */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_ld_r08_r08(HL_H, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x68: /* ld l,b */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_ld_r08_r08(HL_L, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x69: /* ld l,c */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_ld_r08_r08(HL_L, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x6a: /* ld l,d */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_ld_r08_r08(HL_L, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x6b: /* ld l,e */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_ld_r08_r08(HL_L, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x6c: /* ld l,h */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_ld_r08_r08(HL_L, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x6d: /* ld l,l */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_ld_r08_r08(HL_L, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x6e: /* ld l,(hl) */
            {
                constexpr uint32_t m_cycles = (2);
                constexpr uint32_t t_states = (7);
                m_ld_r08_ind_r16(HL_L, HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x6f: /* ld l,a */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_ld_r08_r08(HL_L, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x70: /* ld (hl),b */
            {
                constexpr uint32_t m_cycles = (2);
                constexpr uint32_t t_states = (7);
                m_ld_ind_r16_r08(HL_W, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x71: /* ld (hl),c */
            {
                constexpr uint32_t m_cycles = (2);
                constexpr uint32_t t_states = (7);
                m_ld_ind_r16_r08(HL_W, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x72: /* ld (hl),d */
            {
                constexpr uint32_t m_cycles = (2);
                constexpr uint32_t t_states = (7);
                m_ld_ind_r16_r08(HL_W, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x73: /* ld (hl),e */
            {
                constexpr uint32_t m_cycles = (2);
                constexpr uint32_t t_states = (7);
                m_ld_ind_r16_r08(HL_W, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x74: /* ld (hl),h */
            {
                constexpr uint32_t m_cycles = (2);
                constexpr uint32_t t_states = (7);
                m_ld_ind_r16_r08(HL_W, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x75: /* ld (hl),l */
            {
                constexpr uint32_t m_cycles = (2);
                constexpr uint32_t t_states = (7);
                m_ld_ind_r16_r08(HL_W, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x76: /* halt */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_halt();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x77: /* ld (hl),a */
            {
                constexpr uint32_t m_cycles = (2);
                constexpr uint32_t t_states = (7);
                m_ld_ind_r16_r08(HL_W, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x78: /* ld a,b */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_ld_r08_r08(AF_H, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x79: /* ld a,c */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_ld_r08_r08(AF_H, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x7a: /* ld a,d */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_ld_r08_r08(AF_H, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x7b: /* ld a,e */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_ld_r08_r08(AF_H, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x7c: /* ld a,h */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_ld_r08_r08(AF_H, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x7d: /* ld a,l */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_ld_r08_r08(AF_H, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x7e: /* ld a,(hl) */
            {
                constexpr uint32_t m_cycles = (2);
                constexpr uint32_t t_states = (7);
                m_ld_r08_ind_r16(AF_H, HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x7f: /* ld a,a */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_ld_r08_r08(AF_H, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x80: /* add a,b */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_add_r08_r08(AF_H, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x81: /* add a,c */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_add_r08_r08(AF_H, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x82: /* add a,d */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_add_r08_r08(AF_H, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x83: /* add a,e */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_add_r08_r08(AF_H, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x84: /* add a,h */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_add_r08_r08(AF_H, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x85: /* add a,l */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_add_r08_r08(AF_H, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x86: /* add a,(hl) */
            {
                constexpr uint32_t m_cycles = (2);
                constexpr uint32_t t_states = (7);
                m_add_r08_ind_r16(AF_H, HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x87: /* add a,a */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_add_r08_r08(AF_H, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x88: /* adc a,b */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_adc_r08_r08(AF_H, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x89: /* adc a,c */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_adc_r08_r08(AF_H, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x8a: /* adc a,d */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_adc_r08_r08(AF_H, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x8b: /* adc a,e */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_adc_r08_r08(AF_H, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x8c: /* adc a,h */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_adc_r08_r08(AF_H, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x8d: /* adc a,l */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_adc_r08_r08(AF_H, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x8e: /* adc a,(hl) */
            {
                constexpr uint32_t m_cycles = (2);
                constexpr uint32_t t_states = (7);
                m_adc_r08_ind_r16(AF_H, HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x8f: /* adc a,a */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_adc_r08_r08(AF_H, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x90: /* sub a,b */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_sub_r08_r08(AF_H, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x91: /* sub a,c */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_sub_r08_r08(AF_H, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x92: /* sub a,d */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_sub_r08_r08(AF_H, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x93: /* sub a,e */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_sub_r08_r08(AF_H, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x94: /* sub a,h */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_sub_r08_r08(AF_H, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x95: /* sub a,l */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_sub_r08_r08(AF_H, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x96: /* sub a,(hl) */
            {
                constexpr uint32_t m_cycles = (2);
                constexpr uint32_t t_states = (7);
                m_sub_r08_ind_r16(AF_H, HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x97: /* sub a,a */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_sub_r08_r08(AF_H, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x98: /* sbc a,b */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_sbc_r08_r08(AF_H, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x99: /* sbc a,c */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_sbc_r08_r08(AF_H, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x9a: /* sbc a,d */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_sbc_r08_r08(AF_H, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x9b: /* sbc a,e */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_sbc_r08_r08(AF_H, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x9c: /* sbc a,h */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_sbc_r08_r08(AF_H, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x9d: /* sbc a,l */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_sbc_r08_r08(AF_H, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x9e: /* sbc a,(hl) */
            {
                constexpr uint32_t m_cycles = (2);
                constexpr uint32_t t_states = (7);
                m_sbc_r08_ind_r16(AF_H, HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x9f: /* sbc a,a */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_sbc_r08_r08(AF_H, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xa0: /* and a,b */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_and_r08_r08(AF_H, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xa1: /* and a,c */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_and_r08_r08(AF_H, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xa2: /* and a,d */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_and_r08_r08(AF_H, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xa3: /* and a,e */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_and_r08_r08(AF_H, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xa4: /* and a,h */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_and_r08_r08(AF_H, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xa5: /* and a,l */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_and_r08_r08(AF_H, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xa6: /* and a,(hl) */
            {
                constexpr uint32_t m_cycles = (2);
                constexpr uint32_t t_states = (7);
                m_and_r08_ind_r16(AF_H, HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xa7: /* and a,a */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_and_r08_r08(AF_H, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xa8: /* xor a,b */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_xor_r08_r08(AF_H, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xa9: /* xor a,c */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_xor_r08_r08(AF_H, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xaa: /* xor a,d */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_xor_r08_r08(AF_H, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xab: /* xor a,e */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_xor_r08_r08(AF_H, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xac: /* xor a,h */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_xor_r08_r08(AF_H, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xad: /* xor a,l */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_xor_r08_r08(AF_H, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xae: /* xor a,(hl) */
            {
                constexpr uint32_t m_cycles = (2);
                constexpr uint32_t t_states = (7);
                m_xor_r08_ind_r16(AF_H, HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xaf: /* xor a,a */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_xor_r08_r08(AF_H, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xb0: /* or a,b */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_or_r08_r08(AF_H, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xb1: /* or a,c */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_or_r08_r08(AF_H, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xb2: /* or a,d */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_or_r08_r08(AF_H, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xb3: /* or a,e */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_or_r08_r08(AF_H, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xb4: /* or a,h */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_or_r08_r08(AF_H, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xb5: /* or a,l */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_or_r08_r08(AF_H, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xb6: /* or a,(hl) */
            {
                constexpr uint32_t m_cycles = (2);
                constexpr uint32_t t_states = (7);
                m_or_r08_ind_r16(AF_H, HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xb7: /* or a,a */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_or_r08_r08(AF_H, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xb8: /* cp a,b */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_cp_r08_r08(AF_H, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xb9: /* cp a,c */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_cp_r08_r08(AF_H, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xba: /* cp a,d */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_cp_r08_r08(AF_H, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xbb: /* cp a,e */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_cp_r08_r08(AF_H, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xbc: /* cp a,h */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_cp_r08_r08(AF_H, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xbd: /* cp a,l */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_cp_r08_r08(AF_H, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xbe: /* cp a,(hl) */
            {
                constexpr uint32_t m_cycles = (2);
                constexpr uint32_t t_states = (7);
                m_cp_r08_ind_r16(AF_H, HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xbf: /* cp a,a */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_cp_r08_r08(AF_H, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xc0: /* ret nz */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (5);
                m_ret_nz();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xc1: /* pop bc */
            {
                constexpr uint32_t m_cycles = (3);
                constexpr uint32_t t_states = (10);
                m_pop_r16(BC_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xc2: /* jp nz,nn */
            {
                constexpr uint32_t m_cycles = (3);
                constexpr uint32_t t_states = (10);
                m_jp_nz_i16();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xc3: /* jp nn */
            {
                constexpr uint32_t m_cycles = (3);
                constexpr uint32_t t_states = (10);
                m_jp_i16();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xc4: /* call nz,nn */
            {
                constexpr uint32_t m_cycles = (3);
                constexpr uint32_t t_states = (10);
                m_call_nz_i16();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xc5: /* push bc */
            {
                constexpr uint32_t m_cycles = (3);
                constexpr uint32_t t_states = (11);
                m_push_r16(BC_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xc6: /* add a,n */
            {
                constexpr uint32_t m_cycles = (2);
                constexpr uint32_t t_states = (7);
                m_add_r08_i08(AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xc7: /* rst $00 */
            {
                constexpr uint32_t m_cycles = (3);
                constexpr uint32_t t_states = (11);
                m_rst_vec16(VECTOR_00H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xc8: /* ret z */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (5);
                m_ret_z();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xc9: /* ret */
            {
                constexpr uint32_t m_cycles = (3);
                constexpr uint32_t t_states = (10);
                m_ret();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xca: /* jp z,nn */
            {
                constexpr uint32_t m_cycles = (3);
                constexpr uint32_t t_states = (10);
                m_jp_z_i16();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xcb: /* prefix $cb */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_nop();
                m_consume(m_cycles, t_states);
            }
            goto fetch_cb_opcode;
        case 0xcc: /* call z,nn */
            {
                constexpr uint32_t m_cycles = (3);
                constexpr uint32_t t_states = (10);
                m_call_z_i16();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xcd: /* call nn */
            {
                constexpr uint32_t m_cycles = (5);
                constexpr uint32_t t_states = (17);
                m_call_i16();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xce: /* adc a,n */
            {
                constexpr uint32_t m_cycles = (2);
                constexpr uint32_t t_states = (7);
                m_adc_r08_i08(AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xcf: /* rst $08 */
            {
                constexpr uint32_t m_cycles = (3);
                constexpr uint32_t t_states = (11);
                m_rst_vec16(VECTOR_08H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xd0: /* ret nc */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (5);
                m_ret_nc();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xd1: /* pop de */
            {
                constexpr uint32_t m_cycles = (3);
                constexpr uint32_t t_states = (10);
                m_pop_r16(DE_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xd2: /* jp nc,nn */
            {
                constexpr uint32_t m_cycles = (3);
                constexpr uint32_t t_states = (10);
                m_jp_nc_i16();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xd3: /* out (n),a */
            {
                constexpr uint32_t m_cycles = (3);
                constexpr uint32_t t_states = (11);
                m_out_ind_i08_r08(AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xd4: /* call nc,nn */
            {
                constexpr uint32_t m_cycles = (3);
                constexpr uint32_t t_states = (10);
                m_call_nc_i16();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xd5: /* push de */
            {
                constexpr uint32_t m_cycles = (3);
                constexpr uint32_t t_states = (11);
                m_push_r16(DE_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xd6: /* sub a,n */
            {
                constexpr uint32_t m_cycles = (2);
                constexpr uint32_t t_states = (7);
                m_sub_r08_i08(AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xd7: /* rst $10 */
            {
                constexpr uint32_t m_cycles = (3);
                constexpr uint32_t t_states = (11);
                m_rst_vec16(VECTOR_10H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xd8: /* ret c */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (5);
                m_ret_c();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xd9: /* exx */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_exx();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xda: /* jp c,nn */
            {
                constexpr uint32_t m_cycles = (3);
                constexpr uint32_t t_states = (10);
                m_jp_c_i16();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xdb: /* in a,(n) */
            {
                constexpr uint32_t m_cycles = (3);
                constexpr uint32_t t_states = (11);
                m_in_r08_ind_i08(AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xdc: /* call c,nn */
            {
                constexpr uint32_t m_cycles = (3);
                constexpr uint32_t t_states = (10);
                m_call_c_i16();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xdd: /* prefix $dd */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_nop();
                m_consume(m_cycles, t_states);
            }
            goto fetch_dd_opcode;
        case 0xde: /* sbc a,n */
            {
                constexpr uint32_t m_cycles = (2);
                constexpr uint32_t t_states = (7);
                m_sbc_r08_i08(AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xdf: /* rst $18 */
            {
                constexpr uint32_t m_cycles = (3);
                constexpr uint32_t t_states = (11);
                m_rst_vec16(VECTOR_18H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xe0: /* ret po */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (5);
                m_ret_po();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xe1: /* pop hl */
            {
                constexpr uint32_t m_cycles = (3);
                constexpr uint32_t t_states = (10);
                m_pop_r16(HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xe2: /* jp po,nn */
            {
                constexpr uint32_t m_cycles = (3);
                constexpr uint32_t t_states = (10);
                m_jp_po_i16();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xe3: /* ex (sp),hl */
            {
                constexpr uint32_t m_cycles = (5);
                constexpr uint32_t t_states = (19);
                m_ex_ind_r16_r16(SP_W, HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xe4: /* call po,nn */
            {
                constexpr uint32_t m_cycles = (3);
                constexpr uint32_t t_states = (10);
                m_call_po_i16();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xe5: /* push hl */
            {
                constexpr uint32_t m_cycles = (3);
                constexpr uint32_t t_states = (11);
                m_push_r16(HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xe6: /* and a,n */
            {
                constexpr uint32_t m_cycles = (2);
                constexpr uint32_t t_states = (7);
                m_and_r08_i08(AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xe7: /* rst $20 */
            {
                constexpr uint32_t m_cycles = (3);
                constexpr uint32_t t_states = (11);
                m_rst_vec16(VECTOR_20H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xe8: /* ret pe */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (5);
                m_ret_pe();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xe9: /* jp hl */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_jp_r16(HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xea: /* jp pe,nn */
            {
                constexpr uint32_t m_cycles = (3);
                constexpr uint32_t t_states = (10);
                m_jp_pe_i16();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xeb: /* ex de,hl */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_ex_r16_r16(DE_W, HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xec: /* call pe,nn */
            {
                constexpr uint32_t m_cycles = (3);
                constexpr uint32_t t_states = (10);
                m_call_pe_i16();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xed: /* prefix $ed */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_nop();
                m_consume(m_cycles, t_states);
            }
            goto fetch_ed_opcode;
        case 0xee: /* xor a,n */
            {
                constexpr uint32_t m_cycles = (2);
                constexpr uint32_t t_states = (7);
                m_xor_r08_i08(AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xef: /* rst $28 */
            {
                constexpr uint32_t m_cycles = (3);
                constexpr uint32_t t_states = (11);
                m_rst_vec16(VECTOR_28H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xf0: /* ret p */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (5);
                m_ret_p();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xf1: /* pop af */
            {
                constexpr uint32_t m_cycles = (3);
                constexpr uint32_t t_states = (10);
                m_pop_r16(AF_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xf2: /* jp p,nn */
            {
                constexpr uint32_t m_cycles = (3);
                constexpr uint32_t t_states = (10);
                m_jp_p_i16();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xf3: /* di */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_di();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xf4: /* call p,nn */
            {
                constexpr uint32_t m_cycles = (3);
                constexpr uint32_t t_states = (10);
                m_call_p_i16();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xf5: /* push af */
            {
                constexpr uint32_t m_cycles = (3);
                constexpr uint32_t t_states = (11);
                m_push_r16(AF_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xf6: /* or a,n */
            {
                constexpr uint32_t m_cycles = (2);
                constexpr uint32_t t_states = (7);
                m_or_r08_i08(AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xf7: /* rst $30 */
            {
                constexpr uint32_t m_cycles = (3);
                constexpr uint32_t t_states = (11);
                m_rst_vec16(VECTOR_30H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xf8: /* ret m */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (5);
                m_ret_m();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xf9: /* ld sp,hl */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (6);
                m_ld_r16_r16(SP_W, HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xfa: /* jp m,nn */
            {
                constexpr uint32_t m_cycles = (3);
                constexpr uint32_t t_states = (10);
                m_jp_m_i16();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xfb: /* ei */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_ei();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xfc: /* call m,nn */
            {
                constexpr uint32_t m_cycles = (3);
                constexpr uint32_t t_states = (10);
                m_call_m_i16();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xfd: /* prefix $fd */
            {
                constexpr uint32_t m_cycles = (1);
                constexpr uint32_t t_states = (4);
                m_nop();
                m_consume(m_cycles, t_states);
            }
            goto fetch_fd_opcode;
        case 0xfe: /* cp a,n */
            {
                constexpr uint32_t m_cycles = (2);
                constexpr uint32_t t_states = (7);
                m_cp_r08_i08(AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xff: /* rst $38 */
            {
                constexpr uint32_t m_cycles = (3);
                constexpr uint32_t t_states = (11);
                m_rst_vec16(VECTOR_38H);
                m_consume(m_cycles, t_states);
            }
            break;
