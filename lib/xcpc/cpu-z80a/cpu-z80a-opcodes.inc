#if 0
        case 0x00: /* nop */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_nop();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x01: /* ld bc,nn */
            {
                const uint32_t m_cycles = (2);
                const uint32_t t_states = (10);
                m_ld_r16_i16(BC_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x02: /* ld (bc),a */
            {
                const uint32_t m_cycles = (2);
                const uint32_t t_states = (7);
                m_ld_ind_r16_r08(BC_W, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x03: /* inc bc */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (6);
                m_inc_r16(BC_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x04: /* inc b */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_inc_r08(BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x05: /* dec b */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_dec_r08(BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x06: /* ld b,n */
            {
                const uint32_t m_cycles = (2);
                const uint32_t t_states = (7);
                m_ld_r08_i08(BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x07: /* rlca */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_rlca();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x08: /* ex af,af' */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_ex_r16_r16(AF_W, AF_P);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x09: /* add hl,bc */
            {
                const uint32_t m_cycles = (3);
                const uint32_t t_states = (11);
                m_add_r16_r16(HL_W, BC_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x0a: /* ld a,(bc) */
            {
                const uint32_t m_cycles = (2);
                const uint32_t t_states = (7);
                m_ld_r08_ind_r16(AF_H, BC_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x0b: /* dec bc */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (6);
                m_dec_r16(BC_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x0c: /* inc c */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_inc_r08(BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x0d: /* dec c */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_dec_r08(BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x0e: /* ld c,n */
            {
                const uint32_t m_cycles = (2);
                const uint32_t t_states = (7);
                m_ld_r08_i08(BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x0f: /* rrca */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_rrca();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x10: /* djnz d */
            {
                const uint32_t m_cycles = (2);
                const uint32_t t_states = (8);
                m_djnz_i08();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x11: /* ld de,nn */
            {
                const uint32_t m_cycles = (2);
                const uint32_t t_states = (10);
                m_ld_r16_i16(DE_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x12: /* ld (de),a */
            {
                const uint32_t m_cycles = (2);
                const uint32_t t_states = (7);
                m_ld_ind_r16_r08(DE_W, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x13: /* inc de */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (6);
                m_inc_r16(DE_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x14: /* inc d */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_inc_r08(DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x15: /* dec d */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_dec_r08(DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x16: /* ld d,n */
            {
                const uint32_t m_cycles = (2);
                const uint32_t t_states = (7);
                m_ld_r08_i08(DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x17: /* rla */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_rla();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x18: /* jr d */
            {
                const uint32_t m_cycles = (3);
                const uint32_t t_states = (12);
                m_jr_i08();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x19: /* add hl,de */
            {
                const uint32_t m_cycles = (3);
                const uint32_t t_states = (11);
                m_add_r16_r16(HL_W, DE_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x1a: /* ld a,(de) */
            {
                const uint32_t m_cycles = (2);
                const uint32_t t_states = (7);
                m_ld_r08_ind_r16(AF_H, DE_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x1b: /* dec de */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (6);
                m_dec_r16(DE_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x1c: /* inc e */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_inc_r08(DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x1d: /* dec e */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_dec_r08(DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x1e: /* ld e,n */
            {
                const uint32_t m_cycles = (2);
                const uint32_t t_states = (7);
                m_ld_r08_i08(DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x1f: /* rra */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_rra();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x20: /* jr nz,d */
            {
                const uint32_t m_cycles = (2);
                const uint32_t t_states = (7);
                m_jr_nz_i08();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x21: /* ld hl,nn */
            {
                const uint32_t m_cycles = (2);
                const uint32_t t_states = (10);
                m_ld_r16_i16(HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x22: /* ld (nn),hl */
            {
                const uint32_t m_cycles = (5);
                const uint32_t t_states = (16);
                m_ld_ind_i16_r16(HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x23: /* inc hl */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (6);
                m_inc_r16(HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x24: /* inc h */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_inc_r08(HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x25: /* dec h */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_dec_r08(HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x26: /* ld h,n */
            {
                const uint32_t m_cycles = (2);
                const uint32_t t_states = (7);
                m_ld_r08_i08(HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x27: /* daa */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_daa();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x28: /* jr z,d */
            {
                const uint32_t m_cycles = (2);
                const uint32_t t_states = (7);
                m_jr_z_i08();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x29: /* add hl,hl */
            {
                const uint32_t m_cycles = (3);
                const uint32_t t_states = (11);
                m_add_r16_r16(HL_W, HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x2a: /* ld hl,(nn) */
            {
                const uint32_t m_cycles = (5);
                const uint32_t t_states = (16);
                m_ld_r16_ind_i16(HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x2b: /* dec hl */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (6);
                m_dec_r16(HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x2c: /* inc l */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_inc_r08(HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x2d: /* dec l */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_dec_r08(HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x2e: /* ld l,n */
            {
                const uint32_t m_cycles = (2);
                const uint32_t t_states = (7);
                m_ld_r08_i08(HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x2f: /* cpl */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_cpl();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x30: /* jr nc,d */
            {
                const uint32_t m_cycles = (2);
                const uint32_t t_states = (7);
                m_jr_nc_i08();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x31: /* ld sp,nn */
            {
                const uint32_t m_cycles = (2);
                const uint32_t t_states = (10);
                m_ld_r16_i16(SP_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x32: /* ld (nn),a */
            {
                const uint32_t m_cycles = (4);
                const uint32_t t_states = (13);
                m_ld_ind_i16_r08(AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x33: /* inc sp */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (6);
                m_inc_r16(SP_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x34: /* inc (hl) */
            {
                const uint32_t m_cycles = (3);
                const uint32_t t_states = (11);
                m_inc_ind_r16(HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x35: /* dec (hl) */
            {
                const uint32_t m_cycles = (3);
                const uint32_t t_states = (11);
                m_dec_ind_r16(HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x36: /* ld (hl),n */
            {
                const uint32_t m_cycles = (3);
                const uint32_t t_states = (10);
                m_ld_ind_r16_i08(HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x37: /* scf */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_scf();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x38: /* jr c,d */
            {
                const uint32_t m_cycles = (2);
                const uint32_t t_states = (7);
                m_jr_c_i08();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x39: /* add hl,sp */
            {
                const uint32_t m_cycles = (3);
                const uint32_t t_states = (11);
                m_add_r16_r16(HL_W, SP_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x3a: /* ld a,(nn) */
            {
                const uint32_t m_cycles = (4);
                const uint32_t t_states = (13);
                m_ld_r08_ind_i16(AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x3b: /* dec sp */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (6);
                m_dec_r16(SP_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x3c: /* inc a */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_inc_r08(AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x3d: /* dec a */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_dec_r08(AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x3e: /* ld a,n */
            {
                const uint32_t m_cycles = (2);
                const uint32_t t_states = (7);
                m_ld_r08_i08(AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x3f: /* ccf */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_ccf();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x40: /* ld b,b */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_ld_r08_r08(BC_H, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x41: /* ld b,c */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_ld_r08_r08(BC_H, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x42: /* ld b,d */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_ld_r08_r08(BC_H, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x43: /* ld b,e */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_ld_r08_r08(BC_H, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x44: /* ld b,h */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_ld_r08_r08(BC_H, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x45: /* ld b,l */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_ld_r08_r08(BC_H, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x46: /* ld b,(hl) */
            {
                const uint32_t m_cycles = (2);
                const uint32_t t_states = (7);
                m_ld_r08_ind_r16(BC_H, HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x47: /* ld b,a */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_ld_r08_r08(BC_H, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x48: /* ld c,b */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_ld_r08_r08(BC_L, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x49: /* ld c,c */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_ld_r08_r08(BC_L, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x4a: /* ld c,d */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_ld_r08_r08(BC_L, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x4b: /* ld c,e */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_ld_r08_r08(BC_L, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x4c: /* ld c,h */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_ld_r08_r08(BC_L, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x4d: /* ld c,l */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_ld_r08_r08(BC_L, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x4e: /* ld c,(hl) */
            {
                const uint32_t m_cycles = (2);
                const uint32_t t_states = (7);
                m_ld_r08_ind_r16(BC_L, HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x4f: /* ld c,a */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_ld_r08_r08(BC_L, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x50: /* ld d,b */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_ld_r08_r08(DE_H, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x51: /* ld d,c */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_ld_r08_r08(DE_H, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x52: /* ld d,d */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_ld_r08_r08(DE_H, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x53: /* ld d,e */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_ld_r08_r08(DE_H, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x54: /* ld d,h */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_ld_r08_r08(DE_H, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x55: /* ld d,l */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_ld_r08_r08(DE_H, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x56: /* ld d,(hl) */
            {
                const uint32_t m_cycles = (2);
                const uint32_t t_states = (7);
                m_ld_r08_ind_r16(DE_H, HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x57: /* ld d,a */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_ld_r08_r08(DE_H, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x58: /* ld e,b */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_ld_r08_r08(DE_L, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x59: /* ld e,c */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_ld_r08_r08(DE_L, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x5a: /* ld e,d */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_ld_r08_r08(DE_L, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x5b: /* ld e,e */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_ld_r08_r08(DE_L, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x5c: /* ld e,h */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_ld_r08_r08(DE_L, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x5d: /* ld e,l */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_ld_r08_r08(DE_L, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x5e: /* ld e,(hl) */
            {
                const uint32_t m_cycles = (2);
                const uint32_t t_states = (7);
                m_ld_r08_ind_r16(DE_L, HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x5f: /* ld e,a */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_ld_r08_r08(DE_L, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x60: /* ld h,b */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_ld_r08_r08(HL_H, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x61: /* ld h,c */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_ld_r08_r08(HL_H, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x62: /* ld h,d */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_ld_r08_r08(HL_H, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x63: /* ld h,e */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_ld_r08_r08(HL_H, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x64: /* ld h,h */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_ld_r08_r08(HL_H, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x65: /* ld h,l */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_ld_r08_r08(HL_H, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x66: /* ld h,(hl) */
            {
                const uint32_t m_cycles = (2);
                const uint32_t t_states = (7);
                m_ld_r08_ind_r16(HL_H, HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x67: /* ld h,a */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_ld_r08_r08(HL_H, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x68: /* ld l,b */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_ld_r08_r08(HL_L, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x69: /* ld l,c */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_ld_r08_r08(HL_L, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x6a: /* ld l,d */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_ld_r08_r08(HL_L, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x6b: /* ld l,e */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_ld_r08_r08(HL_L, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x6c: /* ld l,h */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_ld_r08_r08(HL_L, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x6d: /* ld l,l */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_ld_r08_r08(HL_L, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x6e: /* ld l,(hl) */
            {
                const uint32_t m_cycles = (2);
                const uint32_t t_states = (7);
                m_ld_r08_ind_r16(HL_L, HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x6f: /* ld l,a */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_ld_r08_r08(HL_L, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x70: /* ld (hl),b */
            {
                const uint32_t m_cycles = (2);
                const uint32_t t_states = (7);
                m_ld_ind_r16_r08(HL_W, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x71: /* ld (hl),c */
            {
                const uint32_t m_cycles = (2);
                const uint32_t t_states = (7);
                m_ld_ind_r16_r08(HL_W, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x72: /* ld (hl),d */
            {
                const uint32_t m_cycles = (2);
                const uint32_t t_states = (7);
                m_ld_ind_r16_r08(HL_W, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x73: /* ld (hl),e */
            {
                const uint32_t m_cycles = (2);
                const uint32_t t_states = (7);
                m_ld_ind_r16_r08(HL_W, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x74: /* ld (hl),h */
            {
                const uint32_t m_cycles = (2);
                const uint32_t t_states = (7);
                m_ld_ind_r16_r08(HL_W, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x75: /* ld (hl),l */
            {
                const uint32_t m_cycles = (2);
                const uint32_t t_states = (7);
                m_ld_ind_r16_r08(HL_W, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x76: /* halt */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_halt();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x77: /* ld (hl),a */
            {
                const uint32_t m_cycles = (2);
                const uint32_t t_states = (7);
                m_ld_ind_r16_r08(HL_W, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x78: /* ld a,b */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_ld_r08_r08(AF_H, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x79: /* ld a,c */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_ld_r08_r08(AF_H, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x7a: /* ld a,d */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_ld_r08_r08(AF_H, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x7b: /* ld a,e */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_ld_r08_r08(AF_H, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x7c: /* ld a,h */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_ld_r08_r08(AF_H, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x7d: /* ld a,l */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_ld_r08_r08(AF_H, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x7e: /* ld a,(hl) */
            {
                const uint32_t m_cycles = (2);
                const uint32_t t_states = (7);
                m_ld_r08_ind_r16(AF_H, HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x7f: /* ld a,a */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_ld_r08_r08(AF_H, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x80: /* add a,b */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_add_r08_r08(AF_H, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x81: /* add a,c */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_add_r08_r08(AF_H, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x82: /* add a,d */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_add_r08_r08(AF_H, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x83: /* add a,e */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_add_r08_r08(AF_H, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x84: /* add a,h */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_add_r08_r08(AF_H, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x85: /* add a,l */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_add_r08_r08(AF_H, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x86: /* add a,(hl) */
            {
                const uint32_t m_cycles = (2);
                const uint32_t t_states = (7);
                m_add_r08_ind_r16(AF_H, HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x87: /* add a,a */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_add_r08_r08(AF_H, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x88: /* adc a,b */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_adc_r08_r08(AF_H, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x89: /* adc a,c */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_adc_r08_r08(AF_H, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x8a: /* adc a,d */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_adc_r08_r08(AF_H, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x8b: /* adc a,e */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_adc_r08_r08(AF_H, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x8c: /* adc a,h */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_adc_r08_r08(AF_H, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x8d: /* adc a,l */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_adc_r08_r08(AF_H, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x8e: /* adc a,(hl) */
            {
                const uint32_t m_cycles = (2);
                const uint32_t t_states = (7);
                m_adc_r08_ind_r16(AF_H, HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x8f: /* adc a,a */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_adc_r08_r08(AF_H, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x90: /* sub a,b */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_sub_r08_r08(AF_H, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x91: /* sub a,c */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_sub_r08_r08(AF_H, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x92: /* sub a,d */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_sub_r08_r08(AF_H, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x93: /* sub a,e */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_sub_r08_r08(AF_H, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x94: /* sub a,h */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_sub_r08_r08(AF_H, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x95: /* sub a,l */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_sub_r08_r08(AF_H, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x96: /* sub a,(hl) */
            {
                const uint32_t m_cycles = (2);
                const uint32_t t_states = (7);
                m_sub_r08_ind_r16(AF_H, HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x97: /* sub a,a */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_sub_r08_r08(AF_H, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x98: /* sbc a,b */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_sbc_r08_r08(AF_H, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x99: /* sbc a,c */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_sbc_r08_r08(AF_H, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x9a: /* sbc a,d */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_sbc_r08_r08(AF_H, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x9b: /* sbc a,e */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_sbc_r08_r08(AF_H, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x9c: /* sbc a,h */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_sbc_r08_r08(AF_H, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x9d: /* sbc a,l */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_sbc_r08_r08(AF_H, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x9e: /* sbc a,(hl) */
            {
                const uint32_t m_cycles = (2);
                const uint32_t t_states = (7);
                m_sbc_r08_ind_r16(AF_H, HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x9f: /* sbc a,a */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_sbc_r08_r08(AF_H, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xa0: /* and a,b */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_and_r08_r08(AF_H, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xa1: /* and a,c */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_and_r08_r08(AF_H, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xa2: /* and a,d */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_and_r08_r08(AF_H, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xa3: /* and a,e */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_and_r08_r08(AF_H, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xa4: /* and a,h */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_and_r08_r08(AF_H, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xa5: /* and a,l */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_and_r08_r08(AF_H, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xa6: /* and a,(hl) */
            {
                const uint32_t m_cycles = (2);
                const uint32_t t_states = (7);
                m_and_r08_ind_r16(AF_H, HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xa7: /* and a,a */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_and_r08_r08(AF_H, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xa8: /* xor a,b */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_xor_r08_r08(AF_H, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xa9: /* xor a,c */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_xor_r08_r08(AF_H, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xaa: /* xor a,d */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_xor_r08_r08(AF_H, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xab: /* xor a,e */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_xor_r08_r08(AF_H, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xac: /* xor a,h */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_xor_r08_r08(AF_H, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xad: /* xor a,l */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_xor_r08_r08(AF_H, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xae: /* xor a,(hl) */
            {
                const uint32_t m_cycles = (2);
                const uint32_t t_states = (7);
                m_xor_r08_ind_r16(AF_H, HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xaf: /* xor a,a */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_xor_r08_r08(AF_H, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xb0: /* or a,b */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_or_r08_r08(AF_H, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xb1: /* or a,c */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_or_r08_r08(AF_H, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xb2: /* or a,d */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_or_r08_r08(AF_H, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xb3: /* or a,e */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_or_r08_r08(AF_H, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xb4: /* or a,h */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_or_r08_r08(AF_H, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xb5: /* or a,l */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_or_r08_r08(AF_H, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xb6: /* or a,(hl) */
            {
                const uint32_t m_cycles = (2);
                const uint32_t t_states = (7);
                m_or_r08_ind_r16(AF_H, HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xb7: /* or a,a */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_or_r08_r08(AF_H, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xb8: /* cp a,b */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_cp_r08_r08(AF_H, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xb9: /* cp a,c */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_cp_r08_r08(AF_H, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xba: /* cp a,d */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_cp_r08_r08(AF_H, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xbb: /* cp a,e */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_cp_r08_r08(AF_H, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xbc: /* cp a,h */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_cp_r08_r08(AF_H, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xbd: /* cp a,l */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_cp_r08_r08(AF_H, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xbe: /* cp a,(hl) */
            {
                const uint32_t m_cycles = (2);
                const uint32_t t_states = (7);
                m_cp_r08_ind_r16(AF_H, HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xbf: /* cp a,a */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_cp_r08_r08(AF_H, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xc0: /* ret nz */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (5);
                m_ret_nz();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xc1: /* pop bc */
            {
                const uint32_t m_cycles = (3);
                const uint32_t t_states = (10);
                m_pop_r16(BC_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xc2: /* jp nz,nn */
            {
                const uint32_t m_cycles = (3);
                const uint32_t t_states = (10);
                m_jp_nz_i16();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xc3: /* jp nn */
            {
                const uint32_t m_cycles = (3);
                const uint32_t t_states = (10);
                m_jp_i16();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xc4: /* call nz,nn */
            {
                const uint32_t m_cycles = (3);
                const uint32_t t_states = (10);
                m_call_nz_i16();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xc5: /* push bc */
            {
                const uint32_t m_cycles = (3);
                const uint32_t t_states = (11);
                m_push_r16(BC_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xc6: /* add a,n */
            {
                const uint32_t m_cycles = (2);
                const uint32_t t_states = (7);
                m_add_r08_i08(AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xc7: /* rst $00 */
            {
                const uint32_t m_cycles = (3);
                const uint32_t t_states = (11);
                m_rst_vec16(VECTOR_00H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xc8: /* ret z */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (5);
                m_ret_z();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xc9: /* ret */
            {
                const uint32_t m_cycles = (3);
                const uint32_t t_states = (10);
                m_ret();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xca: /* jp z,nn */
            {
                const uint32_t m_cycles = (3);
                const uint32_t t_states = (10);
                m_jp_z_i16();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xcb: /* prefix $cb */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_prefix();
                m_consume(m_cycles, t_states);
            }
            goto fetch_cb_opcode;
        case 0xcc: /* call z,nn */
            {
                const uint32_t m_cycles = (3);
                const uint32_t t_states = (10);
                m_call_z_i16();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xcd: /* call nn */
            {
                const uint32_t m_cycles = (5);
                const uint32_t t_states = (17);
                m_call_i16();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xce: /* adc a,n */
            {
                const uint32_t m_cycles = (2);
                const uint32_t t_states = (7);
                m_adc_r08_i08(AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xcf: /* rst $08 */
            {
                const uint32_t m_cycles = (3);
                const uint32_t t_states = (11);
                m_rst_vec16(VECTOR_08H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xd0: /* ret nc */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (5);
                m_ret_nc();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xd1: /* pop de */
            {
                const uint32_t m_cycles = (3);
                const uint32_t t_states = (10);
                m_pop_r16(DE_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xd2: /* jp nc,nn */
            {
                const uint32_t m_cycles = (3);
                const uint32_t t_states = (10);
                m_jp_nc_i16();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xd3: /* out (n),a */
            {
                const uint32_t m_cycles = (3);
                const uint32_t t_states = (11);
                m_out_ind_i08_r08(AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xd4: /* call nc,nn */
            {
                const uint32_t m_cycles = (3);
                const uint32_t t_states = (10);
                m_call_nc_i16();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xd5: /* push de */
            {
                const uint32_t m_cycles = (3);
                const uint32_t t_states = (11);
                m_push_r16(DE_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xd6: /* sub a,n */
            {
                const uint32_t m_cycles = (2);
                const uint32_t t_states = (7);
                m_sub_r08_i08(AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xd7: /* rst $10 */
            {
                const uint32_t m_cycles = (3);
                const uint32_t t_states = (11);
                m_rst_vec16(VECTOR_10H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xd8: /* ret c */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (5);
                m_ret_c();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xd9: /* exx */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_exx();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xda: /* jp c,nn */
            {
                const uint32_t m_cycles = (3);
                const uint32_t t_states = (10);
                m_jp_c_i16();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xdb: /* in a,(n) */
            {
                const uint32_t m_cycles = (3);
                const uint32_t t_states = (11);
                m_in_r08_ind_i08(AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xdc: /* call c,nn */
            {
                const uint32_t m_cycles = (3);
                const uint32_t t_states = (10);
                m_call_c_i16();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xdd: /* prefix $dd */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_prefix();
                m_consume(m_cycles, t_states);
            }
            goto fetch_dd_opcode;
        case 0xde: /* sbc a,n */
            {
                const uint32_t m_cycles = (2);
                const uint32_t t_states = (7);
                m_sbc_r08_i08(AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xdf: /* rst $18 */
            {
                const uint32_t m_cycles = (3);
                const uint32_t t_states = (11);
                m_rst_vec16(VECTOR_18H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xe0: /* ret po */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (5);
                m_ret_po();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xe1: /* pop hl */
            {
                const uint32_t m_cycles = (3);
                const uint32_t t_states = (10);
                m_pop_r16(HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xe2: /* jp po,nn */
            {
                const uint32_t m_cycles = (3);
                const uint32_t t_states = (10);
                m_jp_po_i16();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xe3: /* ex (sp),hl */
            {
                const uint32_t m_cycles = (5);
                const uint32_t t_states = (19);
                m_ex_ind_r16_r16(SP_W, HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xe4: /* call po,nn */
            {
                const uint32_t m_cycles = (3);
                const uint32_t t_states = (10);
                m_call_po_i16();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xe5: /* push hl */
            {
                const uint32_t m_cycles = (3);
                const uint32_t t_states = (11);
                m_push_r16(HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xe6: /* and a,n */
            {
                const uint32_t m_cycles = (2);
                const uint32_t t_states = (7);
                m_and_r08_i08(AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xe7: /* rst $20 */
            {
                const uint32_t m_cycles = (3);
                const uint32_t t_states = (11);
                m_rst_vec16(VECTOR_20H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xe8: /* ret pe */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (5);
                m_ret_pe();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xe9: /* jp hl */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_jp_r16(HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xea: /* jp pe,nn */
            {
                const uint32_t m_cycles = (3);
                const uint32_t t_states = (10);
                m_jp_pe_i16();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xeb: /* ex de,hl */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_ex_r16_r16(DE_W, HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xec: /* call pe,nn */
            {
                const uint32_t m_cycles = (3);
                const uint32_t t_states = (10);
                m_call_pe_i16();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xed: /* prefix $ed */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_prefix();
                m_consume(m_cycles, t_states);
            }
            goto fetch_ed_opcode;
        case 0xee: /* xor a,n */
            {
                const uint32_t m_cycles = (2);
                const uint32_t t_states = (7);
                m_xor_r08_i08(AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xef: /* rst $28 */
            {
                const uint32_t m_cycles = (3);
                const uint32_t t_states = (11);
                m_rst_vec16(VECTOR_28H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xf0: /* ret p */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (5);
                m_ret_p();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xf1: /* pop af */
            {
                const uint32_t m_cycles = (3);
                const uint32_t t_states = (10);
                m_pop_r16(AF_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xf2: /* jp p,nn */
            {
                const uint32_t m_cycles = (3);
                const uint32_t t_states = (10);
                m_jp_p_i16();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xf3: /* di */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_di();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xf4: /* call p,nn */
            {
                const uint32_t m_cycles = (3);
                const uint32_t t_states = (10);
                m_call_p_i16();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xf5: /* push af */
            {
                const uint32_t m_cycles = (3);
                const uint32_t t_states = (11);
                m_push_r16(AF_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xf6: /* or a,n */
            {
                const uint32_t m_cycles = (2);
                const uint32_t t_states = (7);
                m_or_r08_i08(AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xf7: /* rst $30 */
            {
                const uint32_t m_cycles = (3);
                const uint32_t t_states = (11);
                m_rst_vec16(VECTOR_30H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xf8: /* ret m */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (5);
                m_ret_m();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xf9: /* ld sp,hl */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (6);
                m_ld_r16_r16(SP_W, HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xfa: /* jp m,nn */
            {
                const uint32_t m_cycles = (3);
                const uint32_t t_states = (10);
                m_jp_m_i16();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xfb: /* ei */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_ei();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xfc: /* call m,nn */
            {
                const uint32_t m_cycles = (3);
                const uint32_t t_states = (10);
                m_call_m_i16();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xfd: /* prefix $fd */
            {
                const uint32_t m_cycles = (1);
                const uint32_t t_states = (4);
                m_prefix();
                m_consume(m_cycles, t_states);
            }
            goto fetch_fd_opcode;
        case 0xfe: /* cp a,n */
            {
                const uint32_t m_cycles = (2);
                const uint32_t t_states = (7);
                m_cp_r08_i08(AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xff: /* rst $38 */
            {
                const uint32_t m_cycles = (3);
                const uint32_t t_states = (11);
                m_rst_vec16(VECTOR_38H);
                m_consume(m_cycles, t_states);
            }
            break;
#else
#include "cpu-z80a-opcodes-op.inc"
    case 0x00: /* NOP               */
      m_nop();
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x03: /* INC BC            */
      m_inc_r16(BC_W);
      M_CYCLES += 1;
      T_STATES += 6;
      I_PERIOD -= 6;
      goto epilog;
    case 0x06: /* LD B,n            */
      m_ld_r08_i08(BC_H);
      M_CYCLES += 2;
      T_STATES += 7;
      I_PERIOD -= 7;
      goto epilog;
    case 0x07: /* RLCA              */
      m_rlca();
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x0b: /* DEC BC            */
      m_dec_r16(BC_W);
      M_CYCLES += 1;
      T_STATES += 6;
      I_PERIOD -= 6;
      goto epilog;
    case 0x0e: /* LD C,n            */
      m_ld_r08_i08(BC_L);
      M_CYCLES += 2;
      T_STATES += 7;
      I_PERIOD -= 7;
      goto epilog;
    case 0x0f: /* RRCA              */
      m_rrca();
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x13: /* INC DE            */
      m_inc_r16(DE_W);
      M_CYCLES += 1;
      T_STATES += 6;
      I_PERIOD -= 6;
      goto epilog;
    case 0x16: /* LD D,n            */
      m_ld_r08_i08(DE_H);
      M_CYCLES += 2;
      T_STATES += 7;
      I_PERIOD -= 7;
      goto epilog;
    case 0x17: /* RLA               */
      m_rla();
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x1b: /* DEC DE            */
      m_dec_r16(DE_W);
      M_CYCLES += 1;
      T_STATES += 6;
      I_PERIOD -= 6;
      goto epilog;
    case 0x1e: /* LD E,n            */
      m_ld_r08_i08(DE_L);
      M_CYCLES += 2;
      T_STATES += 7;
      I_PERIOD -= 7;
      goto epilog;
    case 0x1f: /* RRA               */
      m_rra();
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x23: /* INC HL            */
      m_inc_r16(HL_W);
      M_CYCLES += 1;
      T_STATES += 6;
      I_PERIOD -= 6;
      goto epilog;
    case 0x26: /* LD H,n            */
      m_ld_r08_i08(HL_H);
      M_CYCLES += 2;
      T_STATES += 7;
      I_PERIOD -= 7;
      goto epilog;
    case 0x27: /* DAA               */
      m_daa();
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x2b: /* DEC HL            */
      m_dec_r16(HL_W);
      M_CYCLES += 1;
      T_STATES += 6;
      I_PERIOD -= 6;
      goto epilog;
    case 0x2e: /* LD L,n            */
      m_ld_r08_i08(HL_L);
      M_CYCLES += 2;
      T_STATES += 7;
      I_PERIOD -= 7;
      goto epilog;
    case 0x33: /* INC SP            */
      m_inc_r16(SP_W);
      M_CYCLES += 1;
      T_STATES += 6;
      I_PERIOD -= 6;
      goto epilog;
    case 0x3b: /* DEC SP            */
      m_dec_r16(SP_W);
      M_CYCLES += 1;
      T_STATES += 6;
      I_PERIOD -= 6;
      goto epilog;
    case 0x3e: /* LD A,n            */
      m_ld_r08_i08(AF_H);
      M_CYCLES += 2;
      T_STATES += 7;
      I_PERIOD -= 7;
      goto epilog;
    case 0x40: /* LD B,B            */
      m_ld_r08_r08(BC_H, BC_H);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x41: /* LD B,C            */
      m_ld_r08_r08(BC_H, BC_L);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x42: /* LD B,D            */
      m_ld_r08_r08(BC_H, DE_H);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x43: /* LD B,E            */
      m_ld_r08_r08(BC_H, DE_L);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x44: /* LD B,H            */
      m_ld_r08_r08(BC_H, HL_H);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x45: /* LD B,L            */
      m_ld_r08_r08(BC_H, HL_L);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x46: /* LD B,(HL)         */
      m_ld_r08_ind_r16(BC_H, HL_W);
      M_CYCLES += 2;
      T_STATES += 7;
      I_PERIOD -= 7;
      goto epilog;
    case 0x47: /* LD B,A            */
      m_ld_r08_r08(BC_H, AF_H);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x48: /* LD C,B            */
      m_ld_r08_r08(BC_L, BC_H);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x49: /* LD C,C            */
      m_ld_r08_r08(BC_L, BC_L);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x4a: /* LD C,D            */
      m_ld_r08_r08(BC_L, DE_H);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x4b: /* LD C,E            */
      m_ld_r08_r08(BC_L, DE_L);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x4c: /* LD C,H            */
      m_ld_r08_r08(BC_L, HL_H);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x4d: /* LD C,L            */
      m_ld_r08_r08(BC_L, HL_L);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x4e: /* LD C,(HL)         */
      m_ld_r08_ind_r16(BC_L, HL_W);
      M_CYCLES += 2;
      T_STATES += 7;
      I_PERIOD -= 7;
      goto epilog;
    case 0x4f: /* LD C,A            */
      m_ld_r08_r08(BC_L, AF_H);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x50: /* LD D,B            */
      m_ld_r08_r08(DE_H, BC_H);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x51: /* LD D,C            */
      m_ld_r08_r08(DE_H, BC_L);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x52: /* LD D,D            */
      m_ld_r08_r08(DE_H, DE_H);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x53: /* LD D,E            */
      m_ld_r08_r08(DE_H, DE_L);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x54: /* LD D,H            */
      m_ld_r08_r08(DE_H, HL_H);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x55: /* LD D,L            */
      m_ld_r08_r08(DE_H, HL_L);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x56: /* LD D,(HL)         */
      m_ld_r08_ind_r16(DE_H, HL_W);
      M_CYCLES += 2;
      T_STATES += 7;
      I_PERIOD -= 7;
      goto epilog;
    case 0x57: /* LD D,A            */
      m_ld_r08_r08(DE_H, AF_H);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x58: /* LD E,B            */
      m_ld_r08_r08(DE_L, BC_H);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x59: /* LD E,C            */
      m_ld_r08_r08(DE_L, BC_L);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x5a: /* LD E,D            */
      m_ld_r08_r08(DE_L, DE_H);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x5b: /* LD E,E            */
      m_ld_r08_r08(DE_L, DE_L);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x5c: /* LD E,H            */
      m_ld_r08_r08(DE_L, HL_H);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x5d: /* LD E,L            */
      m_ld_r08_r08(DE_L, HL_L);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x5e: /* LD E,(HL)         */
      m_ld_r08_ind_r16(DE_L, HL_W);
      M_CYCLES += 2;
      T_STATES += 7;
      I_PERIOD -= 7;
      goto epilog;
    case 0x5f: /* LD E,A            */
      m_ld_r08_r08(DE_L, AF_H);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x60: /* LD H,B            */
      m_ld_r08_r08(HL_H, BC_H);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x61: /* LD H,C            */
      m_ld_r08_r08(HL_H, BC_L);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x62: /* LD H,D            */
      m_ld_r08_r08(HL_H, DE_H);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x63: /* LD H,E            */
      m_ld_r08_r08(HL_H, DE_L);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x64: /* LD H,H            */
      m_ld_r08_r08(HL_H, HL_H);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x65: /* LD H,L            */
      m_ld_r08_r08(HL_H, HL_L);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x66: /* LD H,(HL)         */
      m_ld_r08_ind_r16(HL_H, HL_W);
      M_CYCLES += 2;
      T_STATES += 7;
      I_PERIOD -= 7;
      goto epilog;
    case 0x67: /* LD H,A            */
      m_ld_r08_r08(HL_H, AF_H);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x68: /* LD L,B            */
      m_ld_r08_r08(HL_L, BC_H);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x69: /* LD L,C            */
      m_ld_r08_r08(HL_L, BC_L);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x6a: /* LD L,D            */
      m_ld_r08_r08(HL_L, DE_H);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x6b: /* LD L,E            */
      m_ld_r08_r08(HL_L, DE_L);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x6c: /* LD L,H            */
      m_ld_r08_r08(HL_L, HL_H);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x6d: /* LD L,L            */
      m_ld_r08_r08(HL_L, HL_L);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x6e: /* LD L,(HL)         */
      m_ld_r08_ind_r16(HL_L, HL_W);
      M_CYCLES += 2;
      T_STATES += 7;
      I_PERIOD -= 7;
      goto epilog;
    case 0x6f: /* LD L,A            */
      m_ld_r08_r08(HL_L, AF_H);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x70: /* LD (HL),B         */
      m_ld_ind_r16_r08(HL_W, BC_H);
      M_CYCLES += 2;
      T_STATES += 7;
      I_PERIOD -= 7;
      goto epilog;
    case 0x71: /* LD (HL),C         */
      m_ld_ind_r16_r08(HL_W, BC_L);
      M_CYCLES += 2;
      T_STATES += 7;
      I_PERIOD -= 7;
      goto epilog;
    case 0x72: /* LD (HL),D         */
      m_ld_ind_r16_r08(HL_W, DE_H);
      M_CYCLES += 2;
      T_STATES += 7;
      I_PERIOD -= 7;
      goto epilog;
    case 0x73: /* LD (HL),E         */
      m_ld_ind_r16_r08(HL_W, DE_L);
      M_CYCLES += 2;
      T_STATES += 7;
      I_PERIOD -= 7;
      goto epilog;
    case 0x74: /* LD (HL),H         */
      m_ld_ind_r16_r08(HL_W, HL_H);
      M_CYCLES += 2;
      T_STATES += 7;
      I_PERIOD -= 7;
      goto epilog;
    case 0x75: /* LD (HL),L         */
      m_ld_ind_r16_r08(HL_W, HL_L);
      M_CYCLES += 2;
      T_STATES += 7;
      I_PERIOD -= 7;
      goto epilog;
    case 0x76: /* HALT              */
      m_halt();
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x77: /* LD (HL),A         */
      m_ld_ind_r16_r08(HL_W, AF_H);
      M_CYCLES += 2;
      T_STATES += 7;
      I_PERIOD -= 7;
      goto epilog;
    case 0x78: /* LD A,B            */
      m_ld_r08_r08(AF_H, BC_H);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x79: /* LD A,C            */
      m_ld_r08_r08(AF_H, BC_L);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x7a: /* LD A,D            */
      m_ld_r08_r08(AF_H, DE_H);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x7b: /* LD A,E            */
      m_ld_r08_r08(AF_H, DE_L);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x7c: /* LD A,H            */
      m_ld_r08_r08(AF_H, HL_H);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x7d: /* LD A,L            */
      m_ld_r08_r08(AF_H, HL_L);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x7e: /* LD A,(HL)         */
      m_ld_r08_ind_r16(AF_H, HL_W);
      M_CYCLES += 2;
      T_STATES += 7;
      I_PERIOD -= 7;
      goto epilog;
    case 0x7f: /* LD A,A            */
      m_ld_r08_r08(AF_H, AF_H);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x80: /* ADD A,B           */
      m_add_r08_r08(AF_H, BC_H);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x81: /* ADD A,C           */
      m_add_r08_r08(AF_H, BC_L);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x82: /* ADD A,D           */
      m_add_r08_r08(AF_H, DE_H);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x83: /* ADD A,E           */
      m_add_r08_r08(AF_H, DE_L);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x84: /* ADD A,H           */
      m_add_r08_r08(AF_H, HL_H);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x85: /* ADD A,L           */
      m_add_r08_r08(AF_H, HL_L);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x86: /* ADD A,(HL)        */
      m_add_r08_ind_r16(AF_H, HL_W);
      M_CYCLES += 2;
      T_STATES += 7;
      I_PERIOD -= 7;
      goto epilog;
    case 0x87: /* ADD A,A           */
      m_add_r08_r08(AF_H, AF_H);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x88: /* ADC A,B           */
      m_adc_r08_r08(AF_H, BC_H);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x89: /* ADC A,C           */
      m_adc_r08_r08(AF_H, BC_L);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x8a: /* ADC A,D           */
      m_adc_r08_r08(AF_H, DE_H);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x8b: /* ADC A,E           */
      m_adc_r08_r08(AF_H, DE_L);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x8c: /* ADC A,H           */
      m_adc_r08_r08(AF_H, HL_H);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x8d: /* ADC A,L           */
      m_adc_r08_r08(AF_H, HL_L);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x8e: /* ADC A,(HL)        */
      m_adc_r08_ind_r16(AF_H, HL_W);
      M_CYCLES += 2;
      T_STATES += 7;
      I_PERIOD -= 7;
      goto epilog;
    case 0x8f: /* ADC A,A           */
      m_adc_r08_r08(AF_H, AF_H);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x90: /* SUB A,B           */
      m_sub_r08_r08(AF_H, BC_H);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x91: /* SUB A,C           */
      m_sub_r08_r08(AF_H, BC_L);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x92: /* SUB A,D           */
      m_sub_r08_r08(AF_H, DE_H);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x93: /* SUB A,E           */
      m_sub_r08_r08(AF_H, DE_L);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x94: /* SUB A,H           */
      m_sub_r08_r08(AF_H, HL_H);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x95: /* SUB A,L           */
      m_sub_r08_r08(AF_H, HL_L);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x96: /* SUB A,(HL)        */
      m_sub_r08_ind_r16(AF_H, HL_W);
      M_CYCLES += 2;
      T_STATES += 7;
      I_PERIOD -= 7;
      goto epilog;
    case 0x97: /* SUB A,A           */
      m_sub_r08_r08(AF_H, AF_H);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x98: /* SBC A,B           */
      m_sbc_r08_r08(AF_H, BC_H);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x99: /* SBC A,C           */
      m_sbc_r08_r08(AF_H, BC_L);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x9a: /* SBC A,D           */
      m_sbc_r08_r08(AF_H, DE_H);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x9b: /* SBC A,E           */
      m_sbc_r08_r08(AF_H, DE_L);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x9c: /* SBC A,H           */
      m_sbc_r08_r08(AF_H, HL_H);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x9d: /* SBC A,L           */
      m_sbc_r08_r08(AF_H, HL_L);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0x9e: /* SBC A,(HL)        */
      m_sbc_r08_ind_r16(AF_H, HL_W);
      M_CYCLES += 2;
      T_STATES += 7;
      I_PERIOD -= 7;
      goto epilog;
    case 0x9f: /* SBC A,A           */
      m_sbc_r08_r08(AF_H, AF_H);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0xa0: /* AND A,B           */
      m_and_r08_r08(AF_H, BC_H);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0xa1: /* AND A,C           */
      m_and_r08_r08(AF_H, BC_L);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0xa2: /* AND A,D           */
      m_and_r08_r08(AF_H, DE_H);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0xa3: /* AND A,E           */
      m_and_r08_r08(AF_H, DE_L);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0xa4: /* AND A,H           */
      m_and_r08_r08(AF_H, HL_H);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0xa5: /* AND A,L           */
      m_and_r08_r08(AF_H, HL_L);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0xa6: /* AND A,(HL)        */
      m_and_r08_ind_r16(AF_H, HL_W);
      M_CYCLES += 2;
      T_STATES += 7;
      I_PERIOD -= 7;
      goto epilog;
    case 0xa7: /* AND A,A           */
      m_and_r08_r08(AF_H, AF_H);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0xa8: /* XOR A,B           */
      m_xor_r08_r08(AF_H, BC_H);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0xa9: /* XOR A,C           */
      m_xor_r08_r08(AF_H, BC_L);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0xaa: /* XOR A,D           */
      m_xor_r08_r08(AF_H, DE_H);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0xab: /* XOR A,E           */
      m_xor_r08_r08(AF_H, DE_L);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0xac: /* XOR A,H           */
      m_xor_r08_r08(AF_H, HL_H);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0xad: /* XOR A,L           */
      m_xor_r08_r08(AF_H, HL_L);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0xae: /* XOR A,(HL)        */
      m_xor_r08_ind_r16(AF_H, HL_W);
      M_CYCLES += 2;
      T_STATES += 7;
      I_PERIOD -= 7;
      goto epilog;
    case 0xaf: /* XOR A,A           */
      m_xor_r08_r08(AF_H, AF_H);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0xb0: /* OR A,B            */
      m_or_r08_r08(AF_H, BC_H);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0xb1: /* OR A,C            */
      m_or_r08_r08(AF_H, BC_L);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0xb2: /* OR A,D            */
      m_or_r08_r08(AF_H, DE_H);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0xb3: /* OR A,E            */
      m_or_r08_r08(AF_H, DE_L);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0xb4: /* OR A,H            */
      m_or_r08_r08(AF_H, HL_H);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0xb5: /* OR A,L            */
      m_or_r08_r08(AF_H, HL_L);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0xb6: /* OR A,(HL)         */
      m_or_r08_ind_r16(AF_H, HL_W);
      M_CYCLES += 2;
      T_STATES += 7;
      I_PERIOD -= 7;
      goto epilog;
    case 0xb7: /* OR A,A            */
      m_or_r08_r08(AF_H, AF_H);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0xb8: /* CP A,B            */
      m_cp_r08_r08(AF_H, BC_H);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0xb9: /* CP A,C            */
      m_cp_r08_r08(AF_H, BC_L);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0xba: /* CP A,D            */
      m_cp_r08_r08(AF_H, DE_H);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0xbb: /* CP A,E            */
      m_cp_r08_r08(AF_H, DE_L);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0xbc: /* CP A,H            */
      m_cp_r08_r08(AF_H, HL_H);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0xbd: /* CP A,L            */
      m_cp_r08_r08(AF_H, HL_L);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0xbe: /* CP A,(HL)         */
      m_cp_r08_ind_r16(AF_H, HL_W);
      M_CYCLES += 2;
      T_STATES += 7;
      I_PERIOD -= 7;
      goto epilog;
    case 0xbf: /* CP A,A            */
      m_cp_r08_r08(AF_H, AF_H);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case 0xc6: /* ADD A,n           */
      m_add_r08_i08(AF_H);
      M_CYCLES += 2;
      T_STATES += 7;
      I_PERIOD -= 7;
      goto epilog;
    case 0xce: /* ADC A,n           */
      m_adc_r08_i08(AF_H);
      M_CYCLES += 2;
      T_STATES += 7;
      I_PERIOD -= 7;
      goto epilog;
    case 0xd6: /* SUB A,n           */
      m_sub_r08_i08(AF_H);
      M_CYCLES += 2;
      T_STATES += 7;
      I_PERIOD -= 7;
      goto epilog;
    case 0xde: /* SBC A,n           */
      m_sbc_r08_i08(AF_H);
      M_CYCLES += 2;
      T_STATES += 7;
      I_PERIOD -= 7;
      goto epilog;
    case 0xe6: /* AND A,n           */
      m_and_r08_i08(AF_H);
      M_CYCLES += 2;
      T_STATES += 7;
      I_PERIOD -= 7;
      goto epilog;
    case 0xee: /* XOR A,n           */
      m_xor_r08_i08(AF_H);
      M_CYCLES += 2;
      T_STATES += 7;
      I_PERIOD -= 7;
      goto epilog;
    case 0xf6: /* OR A,n            */
      m_or_r08_i08(AF_H);
      M_CYCLES += 2;
      T_STATES += 7;
      I_PERIOD -= 7;
      goto epilog;
    case 0xfe: /* CP A,n            */
      m_cp_r08_i08(AF_H);
      M_CYCLES += 2;
      T_STATES += 7;
      I_PERIOD -= 7;
      goto epilog;
    case 0xcb:
      m_prefix();
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto fetch_cb_opcode;
    case 0xed:
      m_prefix();
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto fetch_ed_opcode;
    case 0xdd:
      m_prefix();
      M_CYCLES += 0;
      T_STATES += 0;
      I_PERIOD -= 0;
      goto fetch_dd_opcode;
    case 0xfd:
      m_prefix();
      M_CYCLES += 0;
      T_STATES += 0;
      I_PERIOD -= 0;
      goto fetch_fd_opcode;

    case LD_BC_WORD:
      m_ld_r16_i16(BC_W);
      M_CYCLES += 2;
      T_STATES += 10;
      I_PERIOD -= 10;
      goto epilog;
    case LD_DE_WORD:
      m_ld_r16_i16(DE_W);
      M_CYCLES += 2;
      T_STATES += 10;
      I_PERIOD -= 10;
      goto epilog;
    case LD_HL_WORD:
      m_ld_r16_i16(HL_W);
      M_CYCLES += 2;
      T_STATES += 10;
      I_PERIOD -= 10;
      goto epilog;
    case LD_SP_WORD:
      m_ld_r16_i16(SP_W);
      M_CYCLES += 2;
      T_STATES += 10;
      I_PERIOD -= 10;
      goto epilog;

    case RST00:
      m_rst_vec16(VECTOR_00H);
      M_CYCLES += 3;
      T_STATES += 11;
      I_PERIOD -= 11;
      goto epilog;
    case RST08:
      m_rst_vec16(VECTOR_08H);
      M_CYCLES += 3;
      T_STATES += 11;
      I_PERIOD -= 11;
      goto epilog;
    case RST10:
      m_rst_vec16(VECTOR_10H);
      M_CYCLES += 3;
      T_STATES += 11;
      I_PERIOD -= 11;
      goto epilog;
    case RST18:
      m_rst_vec16(VECTOR_18H);
      M_CYCLES += 3;
      T_STATES += 11;
      I_PERIOD -= 11;
      goto epilog;
    case RST20:
      m_rst_vec16(VECTOR_20H);
      M_CYCLES += 3;
      T_STATES += 11;
      I_PERIOD -= 11;
      goto epilog;
    case RST28:
      m_rst_vec16(VECTOR_28H);
      M_CYCLES += 3;
      T_STATES += 11;
      I_PERIOD -= 11;
      goto epilog;
    case RST30:
      m_rst_vec16(VECTOR_30H);
      M_CYCLES += 3;
      T_STATES += 11;
      I_PERIOD -= 11;
      goto epilog;
    case RST38:
      m_rst_vec16(VECTOR_38H);
      M_CYCLES += 3;
      T_STATES += 11;
      I_PERIOD -= 11;
      goto epilog;

    case DI:
      m_di();
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case EI:
      m_ei();
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;

    case INC_B:
      m_inc_r08(BC_H);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case INC_C:
      m_inc_r08(BC_L);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case INC_D:
      m_inc_r08(DE_H);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case INC_E:
      m_inc_r08(DE_L);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case INC_H:
      m_inc_r08(HL_H);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case INC_L:
      m_inc_r08(HL_L);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case INC_A:
      m_inc_r08(AF_H);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;

    case DEC_B:
      m_dec_r08(BC_H);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case DEC_C:
      m_dec_r08(BC_L);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case DEC_D:
      m_dec_r08(DE_H);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case DEC_E:
      m_dec_r08(DE_L);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case DEC_H:
      m_dec_r08(HL_H);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case DEC_L:
      m_dec_r08(HL_L);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case DEC_A:
      m_dec_r08(AF_H);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;

    case EXX:
      m_exx();
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;

    case PUSH_BC:
      m_push_r16(BC_W);
      M_CYCLES += 3;
      T_STATES += 11;
      I_PERIOD -= 11;
      goto epilog;
    case PUSH_DE:
      m_push_r16(DE_W);
      M_CYCLES += 3;
      T_STATES += 11;
      I_PERIOD -= 11;
      goto epilog;
    case PUSH_HL:
      m_push_r16(HL_W);
      M_CYCLES += 3;
      T_STATES += 11;
      I_PERIOD -= 11;
      goto epilog;
    case PUSH_AF:
      m_push_r16(AF_W);
      M_CYCLES += 3;
      T_STATES += 11;
      I_PERIOD -= 11;
      goto epilog;

    case POP_BC:
      m_pop_r16(BC_W);
      M_CYCLES += 3;
      T_STATES += 10;
      I_PERIOD -= 10;
      goto epilog;
    case POP_DE:
      m_pop_r16(DE_W);
      M_CYCLES += 3;
      T_STATES += 10;
      I_PERIOD -= 10;
      goto epilog;
    case POP_HL:
      m_pop_r16(HL_W);
      M_CYCLES += 3;
      T_STATES += 10;
      I_PERIOD -= 10;
      goto epilog;
    case POP_AF:
      m_pop_r16(AF_W);
      M_CYCLES += 3;
      T_STATES += 10;
      I_PERIOD -= 10;
      goto epilog;

    case JR:
      m_jr_i08();
      M_CYCLES += 3;
      T_STATES += 12;
      I_PERIOD -= 12;
      goto epilog;
    case JR_NZ:
      m_jr_nz_i08();
      M_CYCLES += 2;
      T_STATES += 7;
      I_PERIOD -= 7;
      goto epilog;
    case JR_Z:
      m_jr_z_i08();
      M_CYCLES += 2;
      T_STATES += 7;
      I_PERIOD -= 7;
      goto epilog;
    case JR_NC:
      m_jr_nc_i08();
      M_CYCLES += 2;
      T_STATES += 7;
      I_PERIOD -= 7;
      goto epilog;
    case JR_C:
      m_jr_c_i08();
      M_CYCLES += 2;
      T_STATES += 7;
      I_PERIOD -= 7;
      goto epilog;

    case JP:
      m_jp_i16();
      M_CYCLES += 3;
      T_STATES += 10;
      I_PERIOD -= 10;
      goto epilog;
    case JP_NZ:
      m_jp_nz_i16();
      M_CYCLES += 3;
      T_STATES += 10;
      I_PERIOD -= 10;
      goto epilog;
    case JP_Z:
      m_jp_z_i16();
      M_CYCLES += 3;
      T_STATES += 10;
      I_PERIOD -= 10;
      goto epilog;
    case JP_NC:
      m_jp_nc_i16();
      M_CYCLES += 3;
      T_STATES += 10;
      I_PERIOD -= 10;
      goto epilog;
    case JP_C:
      m_jp_c_i16();
      M_CYCLES += 3;
      T_STATES += 10;
      I_PERIOD -= 10;
      goto epilog;
    case JP_PO:
      m_jp_po_i16();
      M_CYCLES += 3;
      T_STATES += 10;
      I_PERIOD -= 10;
      goto epilog;
    case JP_PE:
      m_jp_pe_i16();
      M_CYCLES += 3;
      T_STATES += 10;
      I_PERIOD -= 10;
      goto epilog;
    case JP_P:
      m_jp_p_i16();
      M_CYCLES += 3;
      T_STATES += 10;
      I_PERIOD -= 10;
      goto epilog;
    case JP_M:
      m_jp_m_i16();
      M_CYCLES += 3;
      T_STATES += 10;
      I_PERIOD -= 10;
      goto epilog;

    case RET:
      m_ret();
      M_CYCLES += 3;
      T_STATES += 10;
      I_PERIOD -= 10;
      goto epilog;
    case RET_NZ:
      m_ret_nz();
      M_CYCLES += 3;
      T_STATES += 11;
      I_PERIOD -= 11;
      goto epilog;
    case RET_Z:
      m_ret_z();
      M_CYCLES += 3;
      T_STATES += 11;
      I_PERIOD -= 11;
      goto epilog;
    case RET_NC:
      m_ret_nc();
      M_CYCLES += 3;
      T_STATES += 11;
      I_PERIOD -= 11;
      goto epilog;
    case RET_C:
      m_ret_c();
      M_CYCLES += 3;
      T_STATES += 11;
      I_PERIOD -= 11;
      goto epilog;
    case RET_PO:
      m_ret_po();
      M_CYCLES += 3;
      T_STATES += 11;
      I_PERIOD -= 11;
      goto epilog;
    case RET_PE:
      m_ret_pe();
      M_CYCLES += 3;
      T_STATES += 11;
      I_PERIOD -= 11;
      goto epilog;
    case RET_P:
      m_ret_p();
      M_CYCLES += 3;
      T_STATES += 11;
      I_PERIOD -= 11;
      goto epilog;
    case RET_M:
      m_ret_m();
      M_CYCLES += 3;
      T_STATES += 11;
      I_PERIOD -= 11;
      goto epilog;

    case LD_PC_HL:
      m_jp_r16(HL_W);
      M_CYCLES += 1;
      T_STATES += 4;
      I_PERIOD -= 4;
      goto epilog;
    case LD_SP_HL:
      m_ld_r16_r16(SP_W, HL_W);
      M_CYCLES += 1;
      T_STATES += 6;
      I_PERIOD -= 6;
      goto epilog;

#endif
