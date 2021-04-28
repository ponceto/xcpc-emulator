        case 0x00: /* nop */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_nop();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x01: /* ld bc,nn */
            {
                const uint32_t m_cycles = (3 - 1);
                const uint32_t t_states = (14 - 4);
                m_ld_r16_i16(BC_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x02: /* ld (bc),a */
            {
                const uint32_t m_cycles = (3 - 1);
                const uint32_t t_states = (11 - 4);
                m_ld_ind_r16_r08(BC_W, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x03: /* inc bc */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (10 - 4);
                m_inc_r16(BC_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x04: /* inc b */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_inc_r08(BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x05: /* dec b */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_dec_r08(BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x06: /* ld b,n */
            {
                const uint32_t m_cycles = (3 - 1);
                const uint32_t t_states = (11 - 4);
                m_ld_r08_i08(BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x07: /* rlca */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_rlca();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x08: /* ex af,af' */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_ex_r16_r16(AF_W, AF_P);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x09: /* add ix,bc */
            {
                const uint32_t m_cycles = (4 - 1);
                const uint32_t t_states = (15 - 4);
                m_add_r16_r16(IX_W, BC_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x0a: /* ld a,(bc) */
            {
                const uint32_t m_cycles = (3 - 1);
                const uint32_t t_states = (11 - 4);
                m_ld_r08_ind_r16(AF_H, BC_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x0b: /* dec bc */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (10 - 4);
                m_dec_r16(BC_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x0c: /* inc c */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_inc_r08(BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x0d: /* dec c */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_dec_r08(BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x0e: /* ld c,n */
            {
                const uint32_t m_cycles = (3 - 1);
                const uint32_t t_states = (11 - 4);
                m_ld_r08_i08(BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x0f: /* rrca */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_rrca();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x10: /* djnz d */
            {
                const uint32_t m_cycles = (3 - 1);
                const uint32_t t_states = (12 - 4);
                m_djnz_i08();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x11: /* ld de,nn */
            {
                const uint32_t m_cycles = (3 - 1);
                const uint32_t t_states = (14 - 4);
                m_ld_r16_i16(DE_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x12: /* ld (de),a */
            {
                const uint32_t m_cycles = (3 - 1);
                const uint32_t t_states = (11 - 4);
                m_ld_ind_r16_r08(DE_W, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x13: /* inc de */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (10 - 4);
                m_inc_r16(DE_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x14: /* inc d */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_inc_r08(DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x15: /* dec d */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_dec_r08(DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x16: /* ld d,n */
            {
                const uint32_t m_cycles = (3 - 1);
                const uint32_t t_states = (11 - 4);
                m_ld_r08_i08(DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x17: /* rla */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_rla();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x18: /* jr d */
            {
                const uint32_t m_cycles = (4 - 1);
                const uint32_t t_states = (16 - 4);
                m_jr_i08();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x19: /* add ix,de */
            {
                const uint32_t m_cycles = (4 - 1);
                const uint32_t t_states = (15 - 4);
                m_add_r16_r16(IX_W, DE_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x1a: /* ld a,(de) */
            {
                const uint32_t m_cycles = (3 - 1);
                const uint32_t t_states = (11 - 4);
                m_ld_r08_ind_r16(AF_H, DE_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x1b: /* dec de */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (10 - 4);
                m_dec_r16(DE_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x1c: /* inc e */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_inc_r08(DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x1d: /* dec e */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_dec_r08(DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x1e: /* ld e,n */
            {
                const uint32_t m_cycles = (3 - 1);
                const uint32_t t_states = (11 - 4);
                m_ld_r08_i08(DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x1f: /* rra */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_rra();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x20: /* jr nz,d */
            {
                const uint32_t m_cycles = (3 - 1);
                const uint32_t t_states = (11 - 4);
                m_jr_nz_i08();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x21: /* ld ix,nn */
            {
                const uint32_t m_cycles = (3 - 1);
                const uint32_t t_states = (14 - 4);
                m_ld_r16_i16(IX_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x22: /* ld (nn),ix */
            {
                const uint32_t m_cycles = (6 - 1);
                const uint32_t t_states = (20 - 4);
                m_ld_ind_i16_r16(IX_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x23: /* inc ix */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (10 - 4);
                m_inc_r16(IX_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x24: /* inc ix.h */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_inc_r08(IX_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x25: /* dec ix.h */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_dec_r08(IX_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x26: /* ld ix.h,n */
            {
                const uint32_t m_cycles = (3 - 1);
                const uint32_t t_states = (11 - 4);
                m_ld_r08_i08(IX_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x27: /* daa */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_daa();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x28: /* jr z,d */
            {
                const uint32_t m_cycles = (3 - 1);
                const uint32_t t_states = (11 - 4);
                m_jr_z_i08();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x29: /* add ix,ix */
            {
                const uint32_t m_cycles = (4 - 1);
                const uint32_t t_states = (15 - 4);
                m_add_r16_r16(IX_W, IX_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x2a: /* ld ix,(nn) */
            {
                const uint32_t m_cycles = (6 - 1);
                const uint32_t t_states = (20 - 4);
                m_ld_r16_ind_i16(IX_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x2b: /* dec ix */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (10 - 4);
                m_dec_r16(IX_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x2c: /* inc ix.l */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_inc_r08(IX_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x2d: /* dec ix.l */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_dec_r08(IX_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x2e: /* ld ix.l,n */
            {
                const uint32_t m_cycles = (3 - 1);
                const uint32_t t_states = (11 - 4);
                m_ld_r08_i08(IX_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x2f: /* cpl */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_cpl();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x30: /* jr nc,d */
            {
                const uint32_t m_cycles = (3 - 1);
                const uint32_t t_states = (11 - 4);
                m_jr_nc_i08();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x31: /* ld sp,nn */
            {
                const uint32_t m_cycles = (3 - 1);
                const uint32_t t_states = (14 - 4);
                m_ld_r16_i16(SP_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x32: /* ld (nn),a */
            {
                const uint32_t m_cycles = (5 - 1);
                const uint32_t t_states = (17 - 4);
                m_ld_ind_i16_r08(AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x33: /* inc sp */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (10 - 4);
                m_inc_r16(SP_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x34: /* inc (ix+d) */
            {
                const uint32_t m_cycles = (6 - 1);
                const uint32_t t_states = (23 - 4);
                m_inc_ind_r16_plus_i08(IX_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x35: /* dec (ix+d) */
            {
                const uint32_t m_cycles = (6 - 1);
                const uint32_t t_states = (23 - 4);
                m_dec_ind_r16_plus_i08(IX_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x36: /* ld (ix+d),n */
            {
                const uint32_t m_cycles = (5 - 1);
                const uint32_t t_states = (19 - 4);
                m_ld_ind_r16_plus_i08_i08(IX_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x37: /* scf */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_scf();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x38: /* jr c,d */
            {
                const uint32_t m_cycles = (3 - 1);
                const uint32_t t_states = (11 - 4);
                m_jr_c_i08();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x39: /* add ix,sp */
            {
                const uint32_t m_cycles = (4 - 1);
                const uint32_t t_states = (15 - 4);
                m_add_r16_r16(IX_W, SP_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x3a: /* ld a,(nn) */
            {
                const uint32_t m_cycles = (5 - 1);
                const uint32_t t_states = (17 - 4);
                m_ld_r08_ind_i16(AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x3b: /* dec sp */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (10 - 4);
                m_dec_r16(SP_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x3c: /* inc a */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_inc_r08(AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x3d: /* dec a */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_dec_r08(AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x3e: /* ld a,n */
            {
                const uint32_t m_cycles = (3 - 1);
                const uint32_t t_states = (11 - 4);
                m_ld_r08_i08(AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x3f: /* ccf */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_ccf();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x40: /* ld b,b */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_ld_r08_r08(BC_H, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x41: /* ld b,c */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_ld_r08_r08(BC_H, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x42: /* ld b,d */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_ld_r08_r08(BC_H, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x43: /* ld b,e */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_ld_r08_r08(BC_H, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x44: /* ld b,ix.h */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_ld_r08_r08(BC_H, IX_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x45: /* ld b,ix.l */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_ld_r08_r08(BC_H, IX_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x46: /* ld b,(ix+d) */
            {
                const uint32_t m_cycles = (5 - 1);
                const uint32_t t_states = (19 - 4);
                m_ld_r08_ind_r16_plus_i08(BC_H, IX_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x47: /* ld b,a */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_ld_r08_r08(BC_H, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x48: /* ld c,b */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_ld_r08_r08(BC_L, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x49: /* ld c,c */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_ld_r08_r08(BC_L, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x4a: /* ld c,d */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_ld_r08_r08(BC_L, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x4b: /* ld c,e */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_ld_r08_r08(BC_L, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x4c: /* ld c,ix.h */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_ld_r08_r08(BC_L, IX_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x4d: /* ld c,ix.l */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_ld_r08_r08(BC_L, IX_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x4e: /* ld c,(ix+d) */
            {
                const uint32_t m_cycles = (5 - 1);
                const uint32_t t_states = (19 - 4);
                m_ld_r08_ind_r16_plus_i08(BC_L, IX_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x4f: /* ld c,a */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_ld_r08_r08(BC_L, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x50: /* ld d,b */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_ld_r08_r08(DE_H, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x51: /* ld d,c */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_ld_r08_r08(DE_H, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x52: /* ld d,d */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_ld_r08_r08(DE_H, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x53: /* ld d,e */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_ld_r08_r08(DE_H, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x54: /* ld d,ix.h */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_ld_r08_r08(DE_H, IX_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x55: /* ld d,ix.l */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_ld_r08_r08(DE_H, IX_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x56: /* ld d,(ix+d) */
            {
                const uint32_t m_cycles = (5 - 1);
                const uint32_t t_states = (19 - 4);
                m_ld_r08_ind_r16_plus_i08(DE_H, IX_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x57: /* ld d,a */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_ld_r08_r08(DE_H, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x58: /* ld e,b */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_ld_r08_r08(DE_L, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x59: /* ld e,c */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_ld_r08_r08(DE_L, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x5a: /* ld e,d */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_ld_r08_r08(DE_L, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x5b: /* ld e,e */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_ld_r08_r08(DE_L, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x5c: /* ld e,ix.h */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_ld_r08_r08(DE_L, IX_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x5d: /* ld e,ix.l */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_ld_r08_r08(DE_L, IX_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x5e: /* ld e,(ix+d) */
            {
                const uint32_t m_cycles = (5 - 1);
                const uint32_t t_states = (19 - 4);
                m_ld_r08_ind_r16_plus_i08(DE_L, IX_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x5f: /* ld e,a */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_ld_r08_r08(DE_L, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x60: /* ld ix.h,b */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_ld_r08_r08(IX_H, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x61: /* ld ix.h,c */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_ld_r08_r08(IX_H, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x62: /* ld ix.h,d */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_ld_r08_r08(IX_H, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x63: /* ld ix.h,e */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_ld_r08_r08(IX_H, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x64: /* ld ix.h,ix.h */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_ld_r08_r08(IX_H, IX_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x65: /* ld ix.h,ix.l */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_ld_r08_r08(IX_H, IX_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x66: /* ld h,(ix+d) */
            {
                const uint32_t m_cycles = (5 - 1);
                const uint32_t t_states = (19 - 4);
                m_ld_r08_ind_r16_plus_i08(HL_H, IX_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x67: /* ld ix.h,a */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_ld_r08_r08(IX_H, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x68: /* ld ix.l,b */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_ld_r08_r08(IX_L, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x69: /* ld ix.l,c */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_ld_r08_r08(IX_L, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x6a: /* ld ix.l,d */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_ld_r08_r08(IX_L, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x6b: /* ld ix.l,e */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_ld_r08_r08(IX_L, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x6c: /* ld ix.l,ix.h */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_ld_r08_r08(IX_L, IX_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x6d: /* ld ix.l,ix.l */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_ld_r08_r08(IX_L, IX_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x6e: /* ld l,(ix+d) */
            {
                const uint32_t m_cycles = (5 - 1);
                const uint32_t t_states = (19 - 4);
                m_ld_r08_ind_r16_plus_i08(HL_L, IX_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x6f: /* ld ix.l,a */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_ld_r08_r08(IX_L, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x70: /* ld (ix+d),b */
            {
                const uint32_t m_cycles = (5 - 1);
                const uint32_t t_states = (19 - 4);
                m_ld_ind_r16_plus_i08_r08(IX_W, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x71: /* ld (ix+d),c */
            {
                const uint32_t m_cycles = (5 - 1);
                const uint32_t t_states = (19 - 4);
                m_ld_ind_r16_plus_i08_r08(IX_W, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x72: /* ld (ix+d),d */
            {
                const uint32_t m_cycles = (5 - 1);
                const uint32_t t_states = (19 - 4);
                m_ld_ind_r16_plus_i08_r08(IX_W, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x73: /* ld (ix+d),e */
            {
                const uint32_t m_cycles = (5 - 1);
                const uint32_t t_states = (19 - 4);
                m_ld_ind_r16_plus_i08_r08(IX_W, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x74: /* ld (ix+d),h */
            {
                const uint32_t m_cycles = (5 - 1);
                const uint32_t t_states = (19 - 4);
                m_ld_ind_r16_plus_i08_r08(IX_W, HL_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x75: /* ld (ix+d),l */
            {
                const uint32_t m_cycles = (5 - 1);
                const uint32_t t_states = (19 - 4);
                m_ld_ind_r16_plus_i08_r08(IX_W, HL_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x76: /* halt */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_halt();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x77: /* ld (ix+d),a */
            {
                const uint32_t m_cycles = (5 - 1);
                const uint32_t t_states = (19 - 4);
                m_ld_ind_r16_plus_i08_r08(IX_W, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x78: /* ld a,b */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_ld_r08_r08(AF_H, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x79: /* ld a,c */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_ld_r08_r08(AF_H, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x7a: /* ld a,d */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_ld_r08_r08(AF_H, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x7b: /* ld a,e */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_ld_r08_r08(AF_H, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x7c: /* ld a,ix.h */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_ld_r08_r08(AF_H, IX_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x7d: /* ld a,ix.l */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_ld_r08_r08(AF_H, IX_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x7e: /* ld a,(ix+d) */
            {
                const uint32_t m_cycles = (5 - 1);
                const uint32_t t_states = (19 - 4);
                m_ld_r08_ind_r16_plus_i08(AF_H, IX_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x7f: /* ld a,a */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_ld_r08_r08(AF_H, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x80: /* add a,b */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_add_r08_r08(AF_H, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x81: /* add a,c */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_add_r08_r08(AF_H, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x82: /* add a,d */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_add_r08_r08(AF_H, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x83: /* add a,e */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_add_r08_r08(AF_H, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x84: /* add a,ix.h */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_add_r08_r08(AF_H, IX_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x85: /* add a,ix.l */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_add_r08_r08(AF_H, IX_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x86: /* add a,(ix+d) */
            {
                const uint32_t m_cycles = (5 - 1);
                const uint32_t t_states = (19 - 4);
                m_add_r08_ind_r16_plus_i08(AF_H, IX_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x87: /* add a,a */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_add_r08_r08(AF_H, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x88: /* adc a,b */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_adc_r08_r08(AF_H, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x89: /* adc a,c */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_adc_r08_r08(AF_H, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x8a: /* adc a,d */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_adc_r08_r08(AF_H, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x8b: /* adc a,e */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_adc_r08_r08(AF_H, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x8c: /* adc a,ix.h */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_adc_r08_r08(AF_H, IX_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x8d: /* adc a,ix.l */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_adc_r08_r08(AF_H, IX_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x8e: /* adc a,(ix+d) */
            {
                const uint32_t m_cycles = (5 - 1);
                const uint32_t t_states = (19 - 4);
                m_adc_r08_ind_r16_plus_i08(AF_H, IX_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x8f: /* adc a,a */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_adc_r08_r08(AF_H, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x90: /* sub a,b */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_sub_r08_r08(AF_H, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x91: /* sub a,c */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_sub_r08_r08(AF_H, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x92: /* sub a,d */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_sub_r08_r08(AF_H, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x93: /* sub a,e */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_sub_r08_r08(AF_H, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x94: /* sub a,ix.h */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_sub_r08_r08(AF_H, IX_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x95: /* sub a,ix.l */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_sub_r08_r08(AF_H, IX_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x96: /* sub a,(ix+d) */
            {
                const uint32_t m_cycles = (5 - 1);
                const uint32_t t_states = (19 - 4);
                m_sub_r08_ind_r16_plus_i08(AF_H, IX_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x97: /* sub a,a */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_sub_r08_r08(AF_H, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x98: /* sbc a,b */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_sbc_r08_r08(AF_H, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x99: /* sbc a,c */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_sbc_r08_r08(AF_H, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x9a: /* sbc a,d */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_sbc_r08_r08(AF_H, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x9b: /* sbc a,e */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_sbc_r08_r08(AF_H, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x9c: /* sbc a,ix.h */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_sbc_r08_r08(AF_H, IX_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x9d: /* sbc a,ix.l */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_sbc_r08_r08(AF_H, IX_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x9e: /* sbc a,(ix+d) */
            {
                const uint32_t m_cycles = (5 - 1);
                const uint32_t t_states = (19 - 4);
                m_sbc_r08_ind_r16_plus_i08(AF_H, IX_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0x9f: /* sbc a,a */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_sbc_r08_r08(AF_H, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xa0: /* and a,b */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_and_r08_r08(AF_H, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xa1: /* and a,c */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_and_r08_r08(AF_H, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xa2: /* and a,d */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_and_r08_r08(AF_H, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xa3: /* and a,e */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_and_r08_r08(AF_H, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xa4: /* and a,ix.h */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_and_r08_r08(AF_H, IX_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xa5: /* and a,ix.l */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_and_r08_r08(AF_H, IX_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xa6: /* and a,(ix+d) */
            {
                const uint32_t m_cycles = (5 - 1);
                const uint32_t t_states = (19 - 4);
                m_and_r08_ind_r16_plus_i08(AF_H, IX_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xa7: /* and a,a */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_and_r08_r08(AF_H, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xa8: /* xor a,b */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_xor_r08_r08(AF_H, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xa9: /* xor a,c */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_xor_r08_r08(AF_H, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xaa: /* xor a,d */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_xor_r08_r08(AF_H, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xab: /* xor a,e */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_xor_r08_r08(AF_H, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xac: /* xor a,ix.h */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_xor_r08_r08(AF_H, IX_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xad: /* xor a,ix.l */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_xor_r08_r08(AF_H, IX_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xae: /* xor a,(ix+d) */
            {
                const uint32_t m_cycles = (5 - 1);
                const uint32_t t_states = (19 - 4);
                m_xor_r08_ind_r16_plus_i08(AF_H, IX_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xaf: /* xor a,a */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_xor_r08_r08(AF_H, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xb0: /* or a,b */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_or_r08_r08(AF_H, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xb1: /* or a,c */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_or_r08_r08(AF_H, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xb2: /* or a,d */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_or_r08_r08(AF_H, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xb3: /* or a,e */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_or_r08_r08(AF_H, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xb4: /* or a,ix.h */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_or_r08_r08(AF_H, IX_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xb5: /* or a,ix.l */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_or_r08_r08(AF_H, IX_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xb6: /* or a,(ix+d) */
            {
                const uint32_t m_cycles = (5 - 1);
                const uint32_t t_states = (19 - 4);
                m_or_r08_ind_r16_plus_i08(AF_H, IX_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xb7: /* or a,a */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_or_r08_r08(AF_H, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xb8: /* cp a,b */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_cp_r08_r08(AF_H, BC_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xb9: /* cp a,c */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_cp_r08_r08(AF_H, BC_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xba: /* cp a,d */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_cp_r08_r08(AF_H, DE_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xbb: /* cp a,e */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_cp_r08_r08(AF_H, DE_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xbc: /* cp a,ix.h */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_cp_r08_r08(AF_H, IX_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xbd: /* cp a,ix.l */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_cp_r08_r08(AF_H, IX_L);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xbe: /* cp a,(ix+d) */
            {
                const uint32_t m_cycles = (5 - 1);
                const uint32_t t_states = (19 - 4);
                m_cp_r08_ind_r16_plus_i08(AF_H, IX_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xbf: /* cp a,a */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_cp_r08_r08(AF_H, AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xc0: /* ret nz */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (9 - 4);
                m_ret_nz();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xc1: /* pop bc */
            {
                const uint32_t m_cycles = (4 - 1);
                const uint32_t t_states = (14 - 4);
                m_pop_r16(BC_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xc2: /* jp nz,nn */
            {
                const uint32_t m_cycles = (4 - 1);
                const uint32_t t_states = (14 - 4);
                m_jp_nz_i16();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xc3: /* jp nn */
            {
                const uint32_t m_cycles = (4 - 1);
                const uint32_t t_states = (14 - 4);
                m_jp_i16();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xc4: /* call nz,nn */
            {
                const uint32_t m_cycles = (4 - 1);
                const uint32_t t_states = (14 - 4);
                m_call_nz_i16();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xc5: /* push bc */
            {
                const uint32_t m_cycles = (4 - 1);
                const uint32_t t_states = (15 - 4);
                m_push_r16(BC_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xc6: /* add a,n */
            {
                const uint32_t m_cycles = (3 - 1);
                const uint32_t t_states = (11 - 4);
                m_add_r08_i08(AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xc7: /* rst $00 */
            {
                const uint32_t m_cycles = (4 - 1);
                const uint32_t t_states = (15 - 4);
                m_rst_vec16(VECTOR_00H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xc8: /* ret z */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (9 - 4);
                m_ret_z();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xc9: /* ret */
            {
                const uint32_t m_cycles = (4 - 1);
                const uint32_t t_states = (14 - 4);
                m_ret();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xca: /* jp z,nn */
            {
                const uint32_t m_cycles = (4 - 1);
                const uint32_t t_states = (14 - 4);
                m_jp_z_i16();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xcb: /* prefix $dd,$cb */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_prefix();
                m_consume(m_cycles, t_states);
            }
            goto fetch_ddcb_opcode;
        case 0xcc: /* call z,nn */
            {
                const uint32_t m_cycles = (4 - 1);
                const uint32_t t_states = (14 - 4);
                m_call_z_i16();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xcd: /* call nn */
            {
                const uint32_t m_cycles = (6 - 1);
                const uint32_t t_states = (21 - 4);
                m_call_i16();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xce: /* adc a,n */
            {
                const uint32_t m_cycles = (3 - 1);
                const uint32_t t_states = (11 - 4);
                m_adc_r08_i08(AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xcf: /* rst $08 */
            {
                const uint32_t m_cycles = (4 - 1);
                const uint32_t t_states = (15 - 4);
                m_rst_vec16(VECTOR_08H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xd0: /* ret nc */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (9 - 4);
                m_ret_nc();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xd1: /* pop de */
            {
                const uint32_t m_cycles = (4 - 1);
                const uint32_t t_states = (14 - 4);
                m_pop_r16(DE_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xd2: /* jp nc,nn */
            {
                const uint32_t m_cycles = (4 - 1);
                const uint32_t t_states = (14 - 4);
                m_jp_nc_i16();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xd3: /* out (n),a */
            {
                const uint32_t m_cycles = (4 - 1);
                const uint32_t t_states = (15 - 4);
                m_out_ind_i08_r08(AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xd4: /* call nc,nn */
            {
                const uint32_t m_cycles = (4 - 1);
                const uint32_t t_states = (14 - 4);
                m_call_nc_i16();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xd5: /* push de */
            {
                const uint32_t m_cycles = (4 - 1);
                const uint32_t t_states = (15 - 4);
                m_push_r16(DE_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xd6: /* sub a,n */
            {
                const uint32_t m_cycles = (3 - 1);
                const uint32_t t_states = (11 - 4);
                m_sub_r08_i08(AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xd7: /* rst $10 */
            {
                const uint32_t m_cycles = (4 - 1);
                const uint32_t t_states = (15 - 4);
                m_rst_vec16(VECTOR_10H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xd8: /* ret c */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (9 - 4);
                m_ret_c();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xd9: /* exx */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_exx();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xda: /* jp c,nn */
            {
                const uint32_t m_cycles = (4 - 1);
                const uint32_t t_states = (14 - 4);
                m_jp_c_i16();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xdb: /* in a,(n) */
            {
                const uint32_t m_cycles = (4 - 1);
                const uint32_t t_states = (15 - 4);
                m_in_r08_ind_i08(AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xdc: /* call c,nn */
            {
                const uint32_t m_cycles = (4 - 1);
                const uint32_t t_states = (14 - 4);
                m_call_c_i16();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xdd: /* prefix $dd,$dd */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_prefix();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xde: /* sbc a,n */
            {
                const uint32_t m_cycles = (3 - 1);
                const uint32_t t_states = (11 - 4);
                m_sbc_r08_i08(AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xdf: /* rst $18 */
            {
                const uint32_t m_cycles = (4 - 1);
                const uint32_t t_states = (15 - 4);
                m_rst_vec16(VECTOR_18H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xe0: /* ret po */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (9 - 4);
                m_ret_po();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xe1: /* pop ix */
            {
                const uint32_t m_cycles = (4 - 1);
                const uint32_t t_states = (14 - 4);
                m_pop_r16(IX_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xe2: /* jp po,nn */
            {
                const uint32_t m_cycles = (4 - 1);
                const uint32_t t_states = (14 - 4);
                m_jp_po_i16();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xe3: /* ex (sp),ix */
            {
                const uint32_t m_cycles = (6 - 1);
                const uint32_t t_states = (23 - 4);
                m_ex_ind_r16_r16(SP_W, IX_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xe4: /* call po,nn */
            {
                const uint32_t m_cycles = (4 - 1);
                const uint32_t t_states = (14 - 4);
                m_call_po_i16();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xe5: /* push ix */
            {
                const uint32_t m_cycles = (4 - 1);
                const uint32_t t_states = (15 - 4);
                m_push_r16(IX_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xe6: /* and a,n */
            {
                const uint32_t m_cycles = (3 - 1);
                const uint32_t t_states = (11 - 4);
                m_and_r08_i08(AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xe7: /* rst $20 */
            {
                const uint32_t m_cycles = (4 - 1);
                const uint32_t t_states = (15 - 4);
                m_rst_vec16(VECTOR_20H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xe8: /* ret pe */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (9 - 4);
                m_ret_pe();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xe9: /* jp ix */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_jp_r16(IX_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xea: /* jp pe,nn */
            {
                const uint32_t m_cycles = (4 - 1);
                const uint32_t t_states = (14 - 4);
                m_jp_pe_i16();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xeb: /* ex de,hl */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_ex_r16_r16(DE_W, HL_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xec: /* call pe,nn */
            {
                const uint32_t m_cycles = (4 - 1);
                const uint32_t t_states = (14 - 4);
                m_call_pe_i16();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xed: /* prefix $dd,$ed */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_prefix();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xee: /* xor a,n */
            {
                const uint32_t m_cycles = (3 - 1);
                const uint32_t t_states = (11 - 4);
                m_xor_r08_i08(AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xef: /* rst $28 */
            {
                const uint32_t m_cycles = (4 - 1);
                const uint32_t t_states = (15 - 4);
                m_rst_vec16(VECTOR_28H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xf0: /* ret p */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (9 - 4);
                m_ret_p();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xf1: /* pop af */
            {
                const uint32_t m_cycles = (4 - 1);
                const uint32_t t_states = (14 - 4);
                m_pop_r16(AF_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xf2: /* jp p,nn */
            {
                const uint32_t m_cycles = (4 - 1);
                const uint32_t t_states = (14 - 4);
                m_jp_p_i16();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xf3: /* di */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_di();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xf4: /* call p,nn */
            {
                const uint32_t m_cycles = (4 - 1);
                const uint32_t t_states = (14 - 4);
                m_call_p_i16();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xf5: /* push af */
            {
                const uint32_t m_cycles = (4 - 1);
                const uint32_t t_states = (15 - 4);
                m_push_r16(AF_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xf6: /* or a,n */
            {
                const uint32_t m_cycles = (3 - 1);
                const uint32_t t_states = (11 - 4);
                m_or_r08_i08(AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xf7: /* rst $30 */
            {
                const uint32_t m_cycles = (4 - 1);
                const uint32_t t_states = (15 - 4);
                m_rst_vec16(VECTOR_30H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xf8: /* ret m */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (9 - 4);
                m_ret_m();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xf9: /* ld sp,ix */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (10 - 4);
                m_ld_r16_r16(SP_W, IX_W);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xfa: /* jp m,nn */
            {
                const uint32_t m_cycles = (4 - 1);
                const uint32_t t_states = (14 - 4);
                m_jp_m_i16();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xfb: /* ei */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_ei();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xfc: /* call m,nn */
            {
                const uint32_t m_cycles = (4 - 1);
                const uint32_t t_states = (14 - 4);
                m_call_m_i16();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xfd: /* prefix $dd,$fd */
            {
                const uint32_t m_cycles = (2 - 1);
                const uint32_t t_states = (8 - 4);
                m_prefix();
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xfe: /* cp a,n */
            {
                const uint32_t m_cycles = (3 - 1);
                const uint32_t t_states = (11 - 4);
                m_cp_r08_i08(AF_H);
                m_consume(m_cycles, t_states);
            }
            break;
        case 0xff: /* rst $38 */
            {
                const uint32_t m_cycles = (4 - 1);
                const uint32_t t_states = (15 - 4);
                m_rst_vec16(VECTOR_38H);
                m_consume(m_cycles, t_states);
            }
            break;
