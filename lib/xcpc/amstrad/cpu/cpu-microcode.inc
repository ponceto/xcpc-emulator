// ---------------------------------------------------------------------------
// low-level pseudo micro-instructions
// ---------------------------------------------------------------------------

#define m_mreq_m1(addr, data) \
    do { \
        static_cast<void>(data = MREQ_M1((SELF), (addr), (0x00))); \
    } while(0)

#define m_mreq_rd(addr, data) \
    do { \
        static_cast<void>(data = MREQ_RD((SELF), (addr), (0x00))); \
    } while(0)

#define m_mreq_wr(addr, data) \
    do { \
        static_cast<void>(MREQ_WR((SELF), (addr), (data))); \
    } while(0)

#define m_iorq_m1(port, data) \
    do { \
        static_cast<void>(data = IORQ_M1((SELF), (port), (0x00))); \
    } while(0)

#define m_iorq_rd(port, data) \
    do { \
        static_cast<void>(data = IORQ_RD((SELF), (port), (0x00))); \
    } while(0)

#define m_iorq_wr(port, data) \
    do { \
        static_cast<void>(IORQ_WR((SELF), (port), (data))); \
    } while(0)

#define m_load_rg(dest, data) \
    do { \
        static_cast<void>(dest = (data)); \
    } while(0)

#define m_addu_rg(reg, data) \
    do { \
        static_cast<void>(reg += (data)); \
    } while(0)

#define m_subu_rg(reg, data) \
    do { \
        static_cast<void>(reg -= (data)); \
    } while(0)

#define m_andu_rg(reg, data) \
    do { \
        static_cast<void>(reg &= (data)); \
    } while(0)

#define m_ioru_rg(reg, data) \
    do { \
        static_cast<void>(reg |= (data)); \
    } while(0)

#define m_xoru_rg(reg, data) \
    do { \
        static_cast<void>(reg ^= (data)); \
    } while(0)

#define m_setu_bit(dest, mask) \
    do { \
        static_cast<void>(dest |= (mask)); \
    } while(0)

#define m_clru_bit(dest, mask) \
    do { \
        static_cast<void>(dest &= ~(mask)); \
    } while(0)

// ---------------------------------------------------------------------------
// miscellaneous pseudo micro-instructions
// ---------------------------------------------------------------------------

#define m_backup_pc() \
    do { \
        m_load_rg(OP_P, PC_W); \
    } while(0)

#define m_restore_pc() \
    do { \
        m_load_rg(PC_W, OP_P); \
    } while(0)

#define m_refresh_dram() \
    do { \
        IR_L = ((IR_L + 0) & 0x80) \
             | ((IR_L + 1) & 0x7f) \
             ; \
    } while(0)

#define m_consume(cycles, states) \
    do { \
        m_addu_rg(M_CYCLES, cycles); \
        m_addu_rg(T_STATES, states); \
        m_addu_rg(I_PERIOD, states); \
    } while(0)

// ---------------------------------------------------------------------------
// fetch pseudo micro-instructions
// ---------------------------------------------------------------------------

#define m_fetch_opcode() \
    do { \
        m_mreq_m1(PC_W, OP_L); \
        m_addu_rg(PC_W, 0x01); \
    } while(0)

#define m_fetch_cb_opcode() \
    do { \
        m_mreq_m1(PC_W, OP_L); \
        m_addu_rg(PC_W, 0x01); \
    } while(0)

#define m_fetch_dd_opcode() \
    do { \
        m_mreq_m1(PC_W, OP_L); \
        m_addu_rg(PC_W, 0x01); \
    } while(0)

#define m_fetch_ed_opcode() \
    do { \
        m_mreq_m1(PC_W, OP_L); \
        m_addu_rg(PC_W, 0x01); \
    } while(0)

#define m_fetch_fd_opcode() \
    do { \
        m_mreq_m1(PC_W, OP_L); \
        m_addu_rg(PC_W, 0x01); \
    } while(0)

#define m_fetch_ddcb_opcode() \
    do { \
        WZ_W = IX_W + SBYTE(MREQ_RD(SELF, PC_W++, 0x00)); \
        m_mreq_rd(PC_W, OP_L); \
        m_addu_rg(PC_W, 0x01); \
    } while(0)

#define m_fetch_fdcb_opcode() \
    do { \
        WZ_W = IY_W + SBYTE(MREQ_RD(SELF, PC_W++, 0x00)); \
        m_mreq_rd(PC_W, OP_L); \
        m_addu_rg(PC_W, 0x01); \
    } while(0)

// ---------------------------------------------------------------------------
// illegal pseudo micro-instructions
// ---------------------------------------------------------------------------

#define m_illegal() \
    do { \
    } while(0)

#define m_illegal_cb() \
    do { \
    } while(0)

#define m_illegal_dd() \
    do { \
    } while(0)

#define m_illegal_ed() \
    do { \
    } while(0)

#define m_illegal_fd() \
    do { \
    } while(0)

#define m_illegal_ddcb() \
    do { \
    } while(0)

#define m_illegal_fdcb() \
    do { \
    } while(0)

// ---------------------------------------------------------------------------
// nop - no operation
// ---------------------------------------------------------------------------

#define m_nop() \
    do { \
    } while(0)

// ---------------------------------------------------------------------------
// di - disable interrupts
// ---------------------------------------------------------------------------

#define m_di() \
    do { \
        m_clru_bit(ST_L, (ST_IFF1)); \
        m_clru_bit(ST_L, (ST_IFF2)); \
        m_clru_bit(ST_L, (ST_AEI)); \
    } while(0)

// ---------------------------------------------------------------------------
// ei - enable interrupts
// ---------------------------------------------------------------------------

#define m_ei() \
    do { \
        m_setu_bit(ST_L, (ST_IFF1)); \
        m_setu_bit(ST_L, (ST_IFF2)); \
        m_setu_bit(ST_L, (ST_AEI)); \
    } while(0)

// ---------------------------------------------------------------------------
// halt - halt processor
// ---------------------------------------------------------------------------

#define m_halt() \
    do { \
        m_setu_bit(ST_L, (ST_HLT)); \
    } while(0)

// ---------------------------------------------------------------------------
// im 0 - interrupt mode 0
// ---------------------------------------------------------------------------

#define m_im_0() \
    do { \
        m_clru_bit(ST_L, (ST_IM1)); \
        m_clru_bit(ST_L, (ST_IM2)); \
    } while(0)

// ---------------------------------------------------------------------------
// im 1 - interrupt mode 1
// ---------------------------------------------------------------------------

#define m_im_1() \
    do { \
        m_setu_bit(ST_L, (ST_IM1)); \
        m_clru_bit(ST_L, (ST_IM2)); \
    } while(0)

// ---------------------------------------------------------------------------
// im 2 - interrupt mode 2
// ---------------------------------------------------------------------------

#define m_im_2() \
    do { \
        m_clru_bit(ST_L, (ST_IM1)); \
        m_setu_bit(ST_L, (ST_IM2)); \
    } while(0)

// ---------------------------------------------------------------------------
// im 3 - interrupt mode 3
// ---------------------------------------------------------------------------

#define m_im_3() \
    do { \
        m_setu_bit(ST_L, (ST_IM1)); \
        m_setu_bit(ST_L, (ST_IM2)); \
    } while(0)

// ---------------------------------------------------------------------------
// input group
// ---------------------------------------------------------------------------

/*
 * in r08,(r08)
 */

#define m_in_r08_ind_r08(data, port) \
    do { \
        m_load_rg(R1_H, BC_H); \
        m_load_rg(R1_L, port); \
        m_iorq_rd(R1_W, R0_L); \
        m_load_rg(data, R0_L); \
        R3_L = PZS[R0_L]; \
        AF_L = /* SF is affected     */ (SF & (R3_L)) \
             | /* ZF is affected     */ (ZF & (R3_L)) \
             | /* YF is undocumented */ (YF & (R3_L)) \
             | /* HF is reset        */ (HF & (0x00)) \
             | /* XF is undocumented */ (XF & (R3_L)) \
             | /* PF is affected     */ (PF & (R3_L)) \
             | /* NF is reset        */ (NF & (0x00)) \
             | /* CF is not affected */ (CF & (AF_L)) \
             ; \
    } while(0)

/*
 * in r08,(i08)
 */

#define m_in_r08_ind_i08(reg1) \
    do { \
        m_load_rg(R0_W, AF_W); \
        m_mreq_rd(PC_W, R0_L); \
        m_addu_rg(PC_W, 0x01); \
        m_iorq_rd(R0_W, reg1); \
    } while(0)

/*
 * ini
 */

#define m_ini() \
    do { \
        m_iorq_rd(BC_W, R0_L); \
        m_mreq_wr(HL_W, R0_L); \
        m_load_rg(WZ_W, BC_W); \
        m_subu_rg(BC_H, 0x01); \
        m_addu_rg(HL_W, 0x01); \
        R3_L = PZS[BC_H]; \
        AF_L = /* SF is unknown      */ (SF & (R3_L)) \
             | /* ZF is affected     */ (ZF & (R3_L)) \
             | /* YF is undocumented */ (YF & (R3_L)) \
             | /* HF is unknown      */ (HF & (0x00)) \
             | /* XF is undocumented */ (XF & (R3_L)) \
             | /* PF is unknown      */ (PF & (0x00)) \
             | /* NF is set          */ (NF & (0xff)) \
             | /* CF is not affected */ (CF & (AF_L)) \
             ; \
    } while(0)

/*
 * inir
 */

#define m_inir() \
    do { \
        m_iorq_rd(BC_W, R1_L); \
        m_mreq_wr(HL_W, R1_L); \
        m_load_rg(WZ_W, BC_W); \
        m_addu_rg(HL_W, 0x01); \
        m_subu_rg(BC_H, 0x01); \
        if(BC_H != 0) { \
            m_restore_pc(); \
            m_consume(1, 5); \
        } \
        R3_L = PZS[BC_H]; \
        AF_L = /* SF is unknown      */ (SF & (R3_L)) \
             | /* ZF is affected     */ (ZF & (R3_L)) \
             | /* YF is undocumented */ (YF & (R3_L)) \
             | /* HF is unknown      */ (HF & (0x00)) \
             | /* XF is undocumented */ (XF & (R3_L)) \
             | /* PF is unknown      */ (PF & (0x00)) \
             | /* NF is set          */ (NF & (0xff)) \
             | /* CF is not affected */ (CF & (AF_L)) \
             ; \
    } while(0)

/*
 * ind
 */

#define m_ind() \
    do { \
        m_iorq_rd(BC_W, R0_L); \
        m_mreq_wr(HL_W, R0_L); \
        m_load_rg(WZ_W, BC_W); \
        m_subu_rg(BC_H, 0x01); \
        m_subu_rg(HL_W, 0x01); \
        R3_L = PZS[BC_H]; \
        AF_L = /* SF is unknown      */ (SF & (R3_L)) \
             | /* ZF is affected     */ (ZF & (R3_L)) \
             | /* YF is undocumented */ (YF & (R3_L)) \
             | /* HF is unknown      */ (HF & (0x00)) \
             | /* XF is undocumented */ (XF & (R3_L)) \
             | /* PF is unknown      */ (PF & (0x00)) \
             | /* NF is set          */ (NF & (0xff)) \
             | /* CF is not affected */ (CF & (AF_L)) \
             ; \
    } while(0)

/*
 * indr
 */

#define m_indr() \
    do { \
        m_iorq_rd(BC_W, R1_L); \
        m_mreq_wr(HL_W, R1_L); \
        m_load_rg(WZ_W, BC_W); \
        m_subu_rg(HL_W, 0x01); \
        m_subu_rg(BC_H, 0x01); \
        if(BC_H != 0) { \
            m_restore_pc(); \
            m_consume(1, 5); \
        } \
        R3_L = PZS[BC_H]; \
        AF_L = /* SF is unknown      */ (SF & (R3_L)) \
             | /* ZF is affected     */ (ZF & (R3_L)) \
             | /* YF is undocumented */ (YF & (R3_L)) \
             | /* HF is unknown      */ (HF & (0x00)) \
             | /* XF is undocumented */ (XF & (R3_L)) \
             | /* PF is unknown      */ (PF & (0x00)) \
             | /* NF is set          */ (NF & (0xff)) \
             | /* CF is not affected */ (CF & (AF_L)) \
             ; \
    } while(0)

// ---------------------------------------------------------------------------
// output group
// ---------------------------------------------------------------------------

/*
 * out (r08),r08
 */

#define m_out_ind_r08_r08(port, data) \
    do { \
        m_load_rg(R1_H, BC_H); \
        m_load_rg(R1_L, port); \
        m_iorq_wr(R1_W, data); \
    } while(0)

/*
 * out (i08),r08
 */

#define m_out_ind_i08_r08(reg1) \
    do { \
        m_load_rg(R0_W, AF_W); \
        m_mreq_rd(PC_W, R0_L); \
        m_addu_rg(PC_W, 0x01); \
        m_iorq_wr(R0_W, reg1); \
    } while(0)

/*
 * outi
 */

#define m_outi() \
    do { \
        m_mreq_rd(HL_W, R0_L); \
        m_subu_rg(BC_H, 0x01); \
        m_addu_rg(HL_W, 0x01); \
        m_load_rg(WZ_W, BC_W); \
        m_iorq_wr(BC_W, R0_L); \
        R3_L = PZS[BC_H]; \
        AF_L = /* SF is unknown      */ (SF & (R3_L)) \
             | /* ZF is affected     */ (ZF & (R3_L)) \
             | /* YF is undocumented */ (YF & (R3_L)) \
             | /* HF is unknown      */ (HF & (0x00)) \
             | /* XF is undocumented */ (XF & (R3_L)) \
             | /* PF is unknown      */ (PF & (0x00)) \
             | /* NF is set          */ (NF & (0xff)) \
             | /* CF is not affected */ (CF & (AF_L)) \
             ; \
    } while(0)

/*
 * otir
 */

#define m_otir() \
    do { \
        m_mreq_rd(HL_W, R1_L); \
        m_addu_rg(HL_W, 0x01); \
        m_subu_rg(BC_H, 0x01); \
        m_load_rg(WZ_W, BC_W); \
        m_iorq_wr(BC_W, R1_L); \
        if(BC_H != 0) { \
            m_restore_pc(); \
            m_consume(1, 5); \
        } \
        R3_L = PZS[BC_H]; \
        AF_L = /* SF is unknown      */ (SF & (R3_L)) \
             | /* ZF is affected     */ (ZF & (R3_L)) \
             | /* YF is undocumented */ (YF & (R3_L)) \
             | /* HF is unknown      */ (HF & (0x00)) \
             | /* XF is undocumented */ (XF & (R3_L)) \
             | /* PF is unknown      */ (PF & (0x00)) \
             | /* NF is set          */ (NF & (0xff)) \
             | /* CF is not affected */ (CF & (AF_L)) \
             ; \
    } while(0)

/*
 * outd
 */

#define m_outd() \
    do { \
        m_mreq_rd(HL_W, R0_L); \
        m_subu_rg(BC_H, 0x01); \
        m_subu_rg(HL_W, 0x01); \
        m_load_rg(WZ_W, BC_W); \
        m_iorq_wr(BC_W, R0_L); \
        R3_L = PZS[BC_H]; \
        AF_L = /* SF is unknown      */ (SF & (R3_L)) \
             | /* ZF is affected     */ (ZF & (R3_L)) \
             | /* YF is undocumented */ (YF & (R3_L)) \
             | /* HF is unknown      */ (HF & (0x00)) \
             | /* XF is undocumented */ (XF & (R3_L)) \
             | /* PF is unknown      */ (PF & (0x00)) \
             | /* NF is set          */ (NF & (0xff)) \
             | /* CF is not affected */ (CF & (AF_L)) \
             ; \
    } while(0)

/*
 * otdr
 */

#define m_otdr() \
    do { \
        m_mreq_rd(HL_W, R1_L); \
        m_subu_rg(HL_W, 0x01); \
        m_subu_rg(BC_H, 0x01); \
        m_load_rg(WZ_W, BC_W); \
        m_iorq_wr(BC_W, R1_L); \
        if(BC_H != 0) { \
            m_restore_pc(); \
            m_consume(1, 5); \
        } \
        R3_L = PZS[BC_H]; \
        AF_L = /* SF is unknown      */ (SF & (R3_L)) \
             | /* ZF is affected     */ (ZF & (R3_L)) \
             | /* YF is undocumented */ (YF & (R3_L)) \
             | /* HF is unknown      */ (HF & (0x00)) \
             | /* XF is undocumented */ (XF & (R3_L)) \
             | /* PF is unknown      */ (PF & (0x00)) \
             | /* NF is set          */ (NF & (0xff)) \
             | /* CF is not affected */ (CF & (AF_L)) \
             ; \
    } while(0)

// ---------------------------------------------------------------------------
// 8-bit load group
// ---------------------------------------------------------------------------

/*
 * ld r08,r08
 */

#define m_ld_r08_r08(reg1, reg2) \
    do { \
        m_load_rg(reg1, reg2); \
    } while(0)

/*
 * ld r08,i08
 */

#define m_ld_r08_i08(reg1) \
    do { \
        m_mreq_rd(PC_W, reg1); \
        m_addu_rg(PC_W, 0x01); \
    } while(0)

/*
 * ld r08,(r16)
 */

#define m_ld_r08_ind_r16(reg1, reg2) \
    do { \
        m_mreq_rd(reg2, reg1); \
    } while(0)

/*
 * ld (r16),r08
 */

#define m_ld_ind_r16_r08(reg1, reg2) \
    do { \
        m_mreq_wr(reg1, reg2); \
    } while(0)

/*
 * ld r08,(i16)
 */

#define m_ld_r08_ind_i16(reg1) \
    do { \
        m_mreq_rd(PC_W, WZ_L); \
        m_addu_rg(PC_W, 0x01); \
        m_mreq_rd(PC_W, WZ_H); \
        m_addu_rg(PC_W, 0x01); \
        m_mreq_rd(WZ_W, reg1); \
    } while(0)

/*
 * ld (i16),r08
 */

#define m_ld_ind_i16_r08(reg1) \
    do { \
        m_mreq_rd(PC_W, WZ_L); \
        m_addu_rg(PC_W, 0x01); \
        m_mreq_rd(PC_W, WZ_H); \
        m_addu_rg(PC_W, 0x01); \
        m_mreq_wr(WZ_W, reg1); \
    } while(0)

/*
 * ld (r16),i08
 */

#define m_ld_ind_r16_i08(reg1) \
    do { \
        m_mreq_rd(PC_W, R0_L); \
        m_addu_rg(PC_W, 0x01); \
        m_mreq_wr(reg1, R0_L); \
    } while(0)

/*
 * ld r08,(r16+i08)
 */

#define m_ld_r08_ind_r16_plus_i08(reg1, reg2) \
    do { \
        WZ_W = reg2 + SBYTE(MREQ_RD(SELF, PC_W++, 0x00)); \
        m_mreq_rd(WZ_W, reg1); \
    } while(0)

/*
 * ld (r16+i08),r08
 */

#define m_ld_ind_r16_plus_i08_r08(reg1, reg2) \
    do { \
        WZ_W = reg1 + SBYTE(MREQ_RD(SELF, PC_W++, 0x00)); \
        m_mreq_wr(WZ_W, reg2); \
    } while(0)

/*
 * ld (r16+i08),i08
 */

#define m_ld_ind_r16_plus_i08_i08(reg1) \
    do { \
        WZ_W = reg1 + SBYTE(MREQ_RD(SELF, PC_W++, 0x00)); \
        m_mreq_rd(PC_W, R0_L); \
        m_addu_rg(PC_W, 0x01); \
        m_mreq_wr(WZ_W, R0_L); \
    } while(0)

/*
 * ld a,i
 */

#define m_ld_a_i() \
    do { \
        m_load_rg(AF_H, IR_H); \
        R3_L = PZS[AF_H]; \
        AF_L = /* SF is affected     */ (SF & (R3_L)) \
             | /* ZF is affected     */ (ZF & (R3_L)) \
             | /* YF is undocumented */ (YF & (R3_L)) \
             | /* HF is reset        */ (HF & (0x00)) \
             | /* XF is undocumented */ (XF & (R3_L)) \
             | /* PF is affected     */ (PF & (HAS_IFF2 ? 0xff : 0x00)) \
             | /* NF is reset        */ (NF & (0x00)) \
             | /* CF is not affected */ (CF & (AF_L)) \
             ; \
    } while(0)

/*
 * ld i,a
 */

#define m_ld_i_a() \
    do { \
        m_load_rg(IR_H, AF_H); \
    } while(0)

/*
 * ld a,r
 */

#define m_ld_a_r() \
    do { \
        m_load_rg(AF_H, IR_L); \
        R3_L = PZS[AF_H]; \
        AF_L = /* SF is affected     */ (SF & (R3_L)) \
             | /* ZF is affected     */ (ZF & (R3_L)) \
             | /* YF is undocumented */ (YF & (R3_L)) \
             | /* HF is reset        */ (HF & (0x00)) \
             | /* XF is undocumented */ (XF & (R3_L)) \
             | /* PF is affected     */ (PF & (HAS_IFF2 ? 0xff : 0x00)) \
             | /* NF is reset        */ (NF & (0x00)) \
             | /* CF is not affected */ (CF & (AF_L)) \
             ; \
    } while(0)

/*
 * ld r,a
 */

#define m_ld_r_a() \
    do { \
        m_load_rg(IR_L, AF_H); \
    } while(0)

// ---------------------------------------------------------------------------
// 16-bit load group : ld/push/pop
// ---------------------------------------------------------------------------

/*
 * ld r16,r16
 */

#define m_ld_r16_r16(reg1, reg2) \
    do { \
        m_load_rg(reg1, reg2); \
    } while(0)

/*
 * ld r16,i16
 */

#define m_ld_r16_i16(reg1) \
    do { \
        m_mreq_rd(PC_W, R0_L); \
        m_addu_rg(PC_W, 0x01); \
        m_mreq_rd(PC_W, R0_H); \
        m_addu_rg(PC_W, 0x01); \
        m_load_rg(reg1, R0_W); \
    } while(0)

/*
 * ld r16,(i16)
 */

#define m_ld_r16_ind_i16(reg1) \
    do { \
        m_mreq_rd(PC_W, WZ_L); \
        m_addu_rg(PC_W, 0x01); \
        m_mreq_rd(PC_W, WZ_H); \
        m_addu_rg(PC_W, 0x01); \
        m_mreq_rd(WZ_W, R0_L); \
        m_addu_rg(WZ_W, 0x01); \
        m_mreq_rd(WZ_W, R0_H); \
        m_addu_rg(WZ_W, 0x01); \
        m_load_rg(reg1, R0_W); \
    } while(0)

/*
 * ld (i16),r16
 */

#define m_ld_ind_i16_r16(reg1) \
    do { \
        m_load_rg(R0_W, reg1); \
        m_mreq_rd(PC_W, WZ_L); \
        m_addu_rg(PC_W, 0x01); \
        m_mreq_rd(PC_W, WZ_H); \
        m_addu_rg(PC_W, 0x01); \
        m_mreq_wr(WZ_W, R0_L); \
        m_addu_rg(WZ_W, 0x01); \
        m_mreq_wr(WZ_W, R0_H); \
        m_addu_rg(WZ_W, 0x01); \
    } while(0)

/*
 * push r16
 */

#define m_push_r16(reg1) \
    do { \
        m_load_rg(R0_W, reg1); \
        m_subu_rg(SP_W, 0x01); \
        m_mreq_wr(SP_W, R0_H); \
        m_subu_rg(SP_W, 0x01); \
        m_mreq_wr(SP_W, R0_L); \
    } while(0)

/*
 * pop r16
 */

#define m_pop_r16(reg1) \
    do { \
        m_mreq_rd(SP_W, R0_L); \
        m_addu_rg(SP_W, 0x01); \
        m_mreq_rd(SP_W, R0_H); \
        m_addu_rg(SP_W, 0x01); \
        m_load_rg(reg1, R0_W); \
    } while(0)

// ---------------------------------------------------------------------------
// general-purpose arithmetic group
// ---------------------------------------------------------------------------

/*
 * daa
 */

#define m_daa() \
    do { \
        R1_R = AF_H; \
        R2_R = 0x00; \
        if(((AF_L & HF) != 0) || ((R1_L & 0x0f) > 0x09)) { \
            R2_L |= 0x06; \
        } \
        if(((AF_L & CF) != 0) || ((R1_L & 0xff) > 0x99)) { \
            R2_L |= 0x60; \
            AF_L |= CF; \
        } \
        if((AF_L & NF) == 0) { \
            R0_R = R1_R + R2_R; \
        } \
        else { \
            R0_R = R1_R - R2_R; \
        } \
        R3_L = PZS[R0_L]; \
        AF_H = R0_L; \
        AF_L = /* SF is affected     */ (SF & (R3_L)) \
             | /* ZF is affected     */ (ZF & (R3_L)) \
             | /* YF is undocumented */ (YF & (R0_L)) \
             | /* HF is affected     */ (HF & (R0_L ^ R1_L ^ R2_L)) \
             | /* XF is undocumented */ (XF & (R0_L)) \
             | /* PF is affected     */ (PF & (R3_L)) \
             | /* NF is not affected */ (NF & (AF_L)) \
             | /* CF is affected     */ (CF & (AF_L)) \
             ; \
    } while(0)

/*
 * cpl
 */

#define m_cpl() \
    do { \
        AF_H = ~AF_H; \
        AF_L = /* SF is not affected */ (SF & (AF_L)) \
             | /* ZF is not affected */ (ZF & (AF_L)) \
             | /* YF is undocumented */ (YF & (AF_H)) \
             | /* HF is set          */ (HF & (0xff)) \
             | /* XF is undocumented */ (XF & (AF_H)) \
             | /* PF is not affected */ (PF & (AF_L)) \
             | /* NF is set          */ (NF & (0xff)) \
             | /* CF is not affected */ (CF & (AF_L)) \
             ; \
    } while(0)

/*
 * neg
 */

#define m_neg() \
    do { \
        R1_L = 0x00; \
        R2_L = AF_H; \
        R0_W = R1_L - R2_L; \
        R3_L = PZS[R0_L]; \
        AF_H = R0_L; \
        AF_L = /* SF is affected     */ (SF & (R3_L)) \
             | /* ZF is affected     */ (ZF & (R3_L)) \
             | /* YF is undocumented */ (YF & (R3_L)) \
             | /* HF is set          */ (HF & (R0_L ^ R1_L ^ R2_L)) \
             | /* XF is undocumented */ (XF & (R3_L)) \
             | /* VF is affected     */ (VF & ((((R1_L & ~R2_L & ~R0_L) | (~R1_L & R2_L & R0_L)) & SF) != 0 ? 0xff : 0x00)) \
             | /* NF is set          */ (NF & (0xff)) \
             | /* CF is affected     */ (CF & (R0_H & CF)) \
             ; \
    } while(0)

/*
 * ccf
 */

#define m_ccf() \
    do { \
        AF_L = /* SF is not affected */ (SF & (AF_L)) \
             | /* ZF is not affected */ (ZF & (AF_L)) \
             | /* YF is undocumented */ (YF & (AF_H)) \
             | /* HF is affected     */ (HF & ((AF_L & CF) != 0 ? 0xff : 0x00)) \
             | /* XF is undocumented */ (XF & (AF_H)) \
             | /* PF is not affected */ (PF & (AF_L)) \
             | /* NF is reset        */ (NF & (0x00)) \
             | /* CF is inverted     */ (CF & ((AF_L & CF) != 0 ? 0x00 : 0xff)) \
             ; \
    } while(0)

/*
 * scf
 */

#define m_scf() \
    do { \
        AF_L = /* SF is not affected */ (SF & (AF_L)) \
             | /* ZF is not affected */ (ZF & (AF_L)) \
             | /* YF is undocumented */ (YF & (AF_H)) \
             | /* HF is reset        */ (HF & (0x00)) \
             | /* XF is undocumented */ (XF & (AF_H)) \
             | /* PF is not affected */ (PF & (AF_L)) \
             | /* NF is reset        */ (NF & (0x00)) \
             | /* CF is set          */ (CF & (0xff)) \
             ; \
    } while(0)

// ---------------------------------------------------------------------------
// 8-bit arithmetic group : add
// ---------------------------------------------------------------------------

/*
 * add r08,r08
 */

#define m_add_r08_r08(reg1, reg2) \
    do { \
        R1_L = reg1; \
        R2_L = reg2; \
        R0_W = R1_L + R2_L; \
        R3_L = PZS[R0_L]; \
        AF_H = R0_L; \
        AF_L = /* SF is affected     */ (SF & (R3_L)) \
             | /* ZF is affected     */ (ZF & (R3_L)) \
             | /* YF is undocumented */ (YF & (R3_L)) \
             | /* HF is set          */ (HF & (R0_L ^ R1_L ^ R2_L)) \
             | /* XF is undocumented */ (XF & (R3_L)) \
             | /* VF is affected     */ (VF & ((((R1_L & R2_L & ~R0_L) | (~R1_L & ~R2_L & R0_L)) & SF) != 0 ? 0xff : 0x00)) \
             | /* NF is reset        */ (NF & (0x00)) \
             | /* CF is affected     */ (CF & (R0_H & CF)) \
             ; \
    } while(0)

/*
 * add r08,i08
 */

#define m_add_r08_i08(reg1) \
    do { \
        R1_L = reg1; \
        R2_L = MREQ_RD(SELF, PC_W++, 0x00); \
        R0_W = R1_L + R2_L; \
        R3_L = PZS[R0_L]; \
        AF_H = R0_L; \
        AF_L = /* SF is affected     */ (SF & (R3_L)) \
             | /* ZF is affected     */ (ZF & (R3_L)) \
             | /* YF is undocumented */ (YF & (R3_L)) \
             | /* HF is set          */ (HF & (R0_L ^ R1_L ^ R2_L)) \
             | /* XF is undocumented */ (XF & (R3_L)) \
             | /* VF is affected     */ (VF & ((((R1_L & R2_L & ~R0_L) | (~R1_L & ~R2_L & R0_L)) & SF) != 0 ? 0xff : 0x00)) \
             | /* NF is reset        */ (NF & (0x00)) \
             | /* CF is affected     */ (CF & (R0_H & CF)) \
             ; \
    } while(0)

/*
 * add r08,(r16)
 */

#define m_add_r08_ind_r16(reg1, reg2) \
    do { \
        R1_L = reg1; \
        R2_L = MREQ_RD(SELF, reg2, 0x00); \
        R0_W = R1_L + R2_L; \
        R3_L = PZS[R0_L]; \
        AF_H = R0_L; \
        AF_L = /* SF is affected     */ (SF & (R3_L)) \
             | /* ZF is affected     */ (ZF & (R3_L)) \
             | /* YF is undocumented */ (YF & (R3_L)) \
             | /* HF is set          */ (HF & (R0_L ^ R1_L ^ R2_L)) \
             | /* XF is undocumented */ (XF & (R3_L)) \
             | /* VF is affected     */ (VF & ((((R1_L & R2_L & ~R0_L) | (~R1_L & ~R2_L & R0_L)) & SF) != 0 ? 0xff : 0x00)) \
             | /* NF is reset        */ (NF & (0x00)) \
             | /* CF is affected     */ (CF & (R0_H & CF)) \
             ; \
    } while(0)

/*
 * add r08,(r16+i08)
 */

#define m_add_r08_ind_r16_plus_i08(reg1, reg2) \
    do { \
        WZ_W = reg2 + SBYTE(MREQ_RD(SELF, PC_W++, 0x00)); \
        R1_L = reg1; \
        R2_L = MREQ_RD(SELF, WZ_W, 0x00); \
        m_add_r08_r08(R1_L, R2_L); \
    } while(0)

// ---------------------------------------------------------------------------
// 8-bit arithmetic group : adc
// ---------------------------------------------------------------------------

/*
 * adc r08,r08
 */

#define m_adc_r08_r08(reg1, reg2) \
    do { \
        R1_L = reg1; \
        R2_L = reg2; \
        R0_W = R1_L + R2_L + ((AF_L & CF) != 0 ? 0x01 : 0x00); \
        R3_L = PZS[R0_L]; \
        AF_H = R0_L; \
        AF_L = /* SF is affected     */ (SF & (R3_L)) \
             | /* ZF is affected     */ (ZF & (R3_L)) \
             | /* YF is undocumented */ (YF & (R3_L)) \
             | /* HF is set          */ (HF & (R0_L ^ R1_L ^ R2_L)) \
             | /* XF is undocumented */ (XF & (R3_L)) \
             | /* VF is affected     */ (VF & ((((R1_L & R2_L & ~R0_L) | (~R1_L & ~R2_L & R0_L)) & SF) != 0 ? 0xff : 0x00)) \
             | /* NF is reset        */ (NF & (0x00)) \
             | /* CF is affected     */ (CF & (R0_H & CF)) \
             ; \
    } while(0)

/*
 * adc r08,i08
 */

#define m_adc_r08_i08(reg1) \
    do { \
        R1_L = reg1; \
        R2_L = MREQ_RD(SELF, PC_W++, 0x00); \
        R0_W = R1_L + R2_L + ((AF_L & CF) != 0 ? 0x01 : 0x00); \
        R3_L = PZS[R0_L]; \
        AF_H = R0_L; \
        AF_L = /* SF is affected     */ (SF & (R3_L)) \
             | /* ZF is affected     */ (ZF & (R3_L)) \
             | /* YF is undocumented */ (YF & (R3_L)) \
             | /* HF is set          */ (HF & (R0_L ^ R1_L ^ R2_L)) \
             | /* XF is undocumented */ (XF & (R3_L)) \
             | /* VF is affected     */ (VF & ((((R1_L & R2_L & ~R0_L) | (~R1_L & ~R2_L & R0_L)) & SF) != 0 ? 0xff : 0x00)) \
             | /* NF is reset        */ (NF & (0x00)) \
             | /* CF is affected     */ (CF & (R0_H & CF)) \
             ; \
    } while(0)

/*
 * adc reg08,(reg16)
 */

#define m_adc_r08_ind_r16(reg1, reg2) \
    do { \
        R1_L = reg1; \
        R2_L = MREQ_RD(SELF, reg2, 0x00); \
        R0_W = R1_L + R2_L + ((AF_L & CF) != 0 ? 0x01 : 0x00); \
        R3_L = PZS[R0_L]; \
        AF_H = R0_L; \
        AF_L = /* SF is affected     */ (SF & (R3_L)) \
             | /* ZF is affected     */ (ZF & (R3_L)) \
             | /* YF is undocumented */ (YF & (R3_L)) \
             | /* HF is set          */ (HF & (R0_L ^ R1_L ^ R2_L)) \
             | /* XF is undocumented */ (XF & (R3_L)) \
             | /* VF is affected     */ (VF & ((((R1_L & R2_L & ~R0_L) | (~R1_L & ~R2_L & R0_L)) & SF) != 0 ? 0xff : 0x00)) \
             | /* NF is reset        */ (NF & (0x00)) \
             | /* CF is affected     */ (CF & (R0_H & CF)) \
             ; \
    } while(0)

/*
 * adc r08,(r16+i08)
 */

#define m_adc_r08_ind_r16_plus_i08(reg1, reg2) \
    do { \
        WZ_W = reg2 + SBYTE(MREQ_RD(SELF, PC_W++, 0x00)); \
        R1_L = reg1; \
        R2_L = MREQ_RD(SELF, WZ_W, 0x00); \
        m_adc_r08_r08(R1_L, R2_L); \
    } while(0)

// ---------------------------------------------------------------------------
// 8-bit arithmetic group : sub
// ---------------------------------------------------------------------------

/*
 * sub r08,r08
 */

#define m_sub_r08_r08(reg1, reg2) \
    do { \
        R1_L = reg1; \
        R2_L = reg2; \
        R0_W = R1_L - R2_L; \
        R3_L = PZS[R0_L]; \
        AF_H = R0_L; \
        AF_L = /* SF is affected     */ (SF & (R3_L)) \
             | /* ZF is affected     */ (ZF & (R3_L)) \
             | /* YF is undocumented */ (YF & (R3_L)) \
             | /* HF is set          */ (HF & (R0_L ^ R1_L ^ R2_L)) \
             | /* XF is undocumented */ (XF & (R3_L)) \
             | /* VF is affected     */ (VF & ((((R1_L & ~R2_L & ~R0_L) | (~R1_L & R2_L & R0_L)) & SF) != 0 ? 0xff : 0x00)) \
             | /* NF is set          */ (NF & (0xff)) \
             | /* CF is affected     */ (CF & (R0_H & CF)) \
             ; \
    } while(0)

/*
 * sub r08,i08
 */

#define m_sub_r08_i08(reg1) \
    do { \
        R1_L = reg1; \
        R2_L = MREQ_RD(SELF, PC_W++, 0x00); \
        R0_W = R1_L - R2_L; \
        R3_L = PZS[R0_L]; \
        AF_H = R0_L; \
        AF_L = /* SF is affected     */ (SF & (R3_L)) \
             | /* ZF is affected     */ (ZF & (R3_L)) \
             | /* YF is undocumented */ (YF & (R3_L)) \
             | /* HF is set          */ (HF & (R0_L ^ R1_L ^ R2_L)) \
             | /* XF is undocumented */ (XF & (R3_L)) \
             | /* VF is affected     */ (VF & ((((R1_L & ~R2_L & ~R0_L) | (~R1_L & R2_L & R0_L)) & SF) != 0 ? 0xff : 0x00)) \
             | /* NF is set          */ (NF & (0xff)) \
             | /* CF is affected     */ (CF & (R0_H & CF)) \
             ; \
    } while(0)

/*
 * sub r08,(r16)
 */

#define m_sub_r08_ind_r16(reg1, reg2) \
    do { \
        R1_L = reg1; \
        R2_L = MREQ_RD(SELF, reg2, 0x00); \
        R0_W = R1_L - R2_L; \
        R3_L = PZS[R0_L]; \
        AF_H = R0_L; \
        AF_L = /* SF is affected     */ (SF & (R3_L)) \
             | /* ZF is affected     */ (ZF & (R3_L)) \
             | /* YF is undocumented */ (YF & (R3_L)) \
             | /* HF is set          */ (HF & (R0_L ^ R1_L ^ R2_L)) \
             | /* XF is undocumented */ (XF & (R3_L)) \
             | /* VF is affected     */ (VF & ((((R1_L & ~R2_L & ~R0_L) | (~R1_L & R2_L & R0_L)) & SF) != 0 ? 0xff : 0x00)) \
             | /* NF is set          */ (NF & (0xff)) \
             | /* CF is affected     */ (CF & (R0_H & CF)) \
             ; \
    } while(0)

/*
 * sub r08,(r16+i08)
 */

#define m_sub_r08_ind_r16_plus_i08(reg1, reg2) \
    do { \
        WZ_W = reg2 + SBYTE(MREQ_RD(SELF, PC_W++, 0x00)); \
        R1_L = reg1; \
        R2_L = MREQ_RD(SELF, WZ_W, 0x00); \
        m_sub_r08_r08(R1_L, R2_L); \
    } while(0)

// ---------------------------------------------------------------------------
// 8-bit arithmetic group : sbc
// ---------------------------------------------------------------------------

/*
 * sbc r08,r08
 */

#define m_sbc_r08_r08(reg1, reg2) \
    do { \
        R1_L = reg1; \
        R2_L = reg2; \
        R0_W = R1_L - R2_L - ((AF_L & CF) != 0 ? 0x01 : 0x00); \
        R3_L = PZS[R0_L]; \
        AF_H = R0_L; \
        AF_L = /* SF is affected     */ (SF & (R3_L)) \
             | /* ZF is affected     */ (ZF & (R3_L)) \
             | /* YF is undocumented */ (YF & (R3_L)) \
             | /* HF is set          */ (HF & (R0_L ^ R1_L ^ R2_L)) \
             | /* XF is undocumented */ (XF & (R3_L)) \
             | /* VF is affected     */ (VF & ((((R1_L & ~R2_L & ~R0_L) | (~R1_L & R2_L & R0_L)) & SF) != 0 ? 0xff : 0x00)) \
             | /* NF is set          */ (NF & (0xff)) \
             | /* CF is affected     */ (CF & (R0_H & CF)) \
             ; \
    } while(0)

/*
 * sbc r08,i08
 */

#define m_sbc_r08_i08(reg1) \
    do { \
        R1_L = reg1; \
        R2_L = MREQ_RD(SELF, PC_W++, 0x00); \
        R0_W = R1_L - R2_L - ((AF_L & CF) != 0 ? 0x01 : 0x00); \
        R3_L = PZS[R0_L]; \
        AF_H = R0_L; \
        AF_L = /* SF is affected     */ (SF & (R3_L)) \
             | /* ZF is affected     */ (ZF & (R3_L)) \
             | /* YF is undocumented */ (YF & (R3_L)) \
             | /* HF is set          */ (HF & (R0_L ^ R1_L ^ R2_L)) \
             | /* XF is undocumented */ (XF & (R3_L)) \
             | /* VF is affected     */ (VF & ((((R1_L & ~R2_L & ~R0_L) | (~R1_L & R2_L & R0_L)) & SF) != 0 ? 0xff : 0x00)) \
             | /* NF is set          */ (NF & (0xff)) \
             | /* CF is affected     */ (CF & (R0_H & CF)) \
             ; \
    } while(0)

/*
 * sbc r08,(r16)
 */

#define m_sbc_r08_ind_r16(reg1, reg2) \
    do { \
        R1_L = reg1; \
        R2_L = MREQ_RD(SELF, reg2, 0x00); \
        R0_W = R1_L - R2_L - ((AF_L & CF) != 0 ? 0x01 : 0x00); \
        R3_L = PZS[R0_L]; \
        AF_H = R0_L; \
        AF_L = /* SF is affected     */ (SF & (R3_L)) \
             | /* ZF is affected     */ (ZF & (R3_L)) \
             | /* YF is undocumented */ (YF & (R3_L)) \
             | /* HF is set          */ (HF & (R0_L ^ R1_L ^ R2_L)) \
             | /* XF is undocumented */ (XF & (R3_L)) \
             | /* VF is affected     */ (VF & ((((R1_L & ~R2_L & ~R0_L) | (~R1_L & R2_L & R0_L)) & SF) != 0 ? 0xff : 0x00)) \
             | /* NF is set          */ (NF & (0xff)) \
             | /* CF is affected     */ (CF & (R0_H & CF)) \
             ; \
    } while(0)

/*
 * sbc r08,(r16+i08)
 */

#define m_sbc_r08_ind_r16_plus_i08(reg1, reg2) \
    do { \
        WZ_W = reg2 + SBYTE(MREQ_RD(SELF, PC_W++, 0x00)); \
        R1_L = reg1; \
        R2_L = MREQ_RD(SELF, WZ_W, 0x00); \
        m_sbc_r08_r08(R1_L, R2_L); \
    } while(0)

// ---------------------------------------------------------------------------
// 8-bit arithmetic group : and
// ---------------------------------------------------------------------------

/*
 * and r08,r08
 */

#define m_and_r08_r08(reg1, reg2) \
    do { \
        R1_L = reg1; \
        R2_L = reg2; \
        R0_L = R1_L & R2_L; \
        R3_L = PZS[R0_L]; \
        AF_H = R0_L; \
        AF_L = /* SF is affected     */ (SF & (R3_L)) \
             | /* ZF is affected     */ (ZF & (R3_L)) \
             | /* YF is undocumented */ (YF & (R3_L)) \
             | /* HF is set          */ (HF & (0xff)) \
             | /* XF is undocumented */ (XF & (R3_L)) \
             | /* PF is affected     */ (PF & (R3_L)) \
             | /* NF is reset        */ (NF & (0x00)) \
             | /* CF is reset        */ (CF & (0x00)) \
             ; \
    } while(0)

/*
 * and r08,i08
 */

#define m_and_r08_i08(reg1) \
    do { \
        R1_L = reg1; \
        R2_L = MREQ_RD(SELF, PC_W++, 0x00); \
        R0_L = R1_L & R2_L; \
        R3_L = PZS[R0_L]; \
        AF_H = R0_L; \
        AF_L = /* SF is affected     */ (SF & (R3_L)) \
             | /* ZF is affected     */ (ZF & (R3_L)) \
             | /* YF is undocumented */ (YF & (R3_L)) \
             | /* HF is set          */ (HF & (0xff)) \
             | /* XF is undocumented */ (XF & (R3_L)) \
             | /* PF is affected     */ (PF & (R3_L)) \
             | /* NF is reset        */ (NF & (0x00)) \
             | /* CF is reset        */ (CF & (0x00)) \
             ; \
    } while(0)

/*
 * and r08,(r16)
 */

#define m_and_r08_ind_r16(reg1, reg2) \
    do { \
        R1_L = reg1; \
        R2_L = MREQ_RD(SELF, reg2, 0x00); \
        R0_L = R1_L & R2_L; \
        R3_L = PZS[R0_L]; \
        AF_H = R0_L; \
        AF_L = /* SF is affected     */ (SF & (R3_L)) \
             | /* ZF is affected     */ (ZF & (R3_L)) \
             | /* YF is undocumented */ (YF & (R3_L)) \
             | /* HF is set          */ (HF & (0xff)) \
             | /* XF is undocumented */ (XF & (R3_L)) \
             | /* PF is affected     */ (PF & (R3_L)) \
             | /* NF is reset        */ (NF & (0x00)) \
             | /* CF is reset        */ (CF & (0x00)) \
             ; \
    } while(0)

/*
 * and r08,(r16+i08)
 */

#define m_and_r08_ind_r16_plus_i08(reg1, reg2) \
    do { \
        WZ_W = reg2 + SBYTE(MREQ_RD(SELF, PC_W++, 0x00)); \
        R1_L = reg1; \
        R2_L = MREQ_RD(SELF, WZ_W, 0x00); \
        m_and_r08_r08(R1_L, R2_L); \
    } while(0)

// ---------------------------------------------------------------------------
// 8-bit arithmetic group : xor
// ---------------------------------------------------------------------------

/*
 * xor r08,r08
 */

#define m_xor_r08_r08(reg1, reg2) \
    do { \
        R1_L = reg1; \
        R2_L = reg2; \
        R0_L = R1_L ^ R2_L; \
        R3_L = PZS[R0_L]; \
        AF_H = R0_L; \
        AF_L = /* SF is affected     */ (SF & (R3_L)) \
             | /* ZF is affected     */ (ZF & (R3_L)) \
             | /* YF is undocumented */ (YF & (R3_L)) \
             | /* HF is reset        */ (HF & (0x00)) \
             | /* XF is undocumented */ (XF & (R3_L)) \
             | /* PF is affected     */ (PF & (R3_L)) \
             | /* NF is reset        */ (NF & (0x00)) \
             | /* CF is reset        */ (CF & (0x00)) \
             ; \
    } while(0)

/*
 * xor r08,i08
 */

#define m_xor_r08_i08(reg1) \
    do { \
        R1_L = reg1; \
        R2_L = MREQ_RD(SELF, PC_W++, 0x00); \
        R0_L = R1_L ^ R2_L; \
        R3_L = PZS[R0_L]; \
        AF_H = R0_L; \
        AF_L = /* SF is affected     */ (SF & (R3_L)) \
             | /* ZF is affected     */ (ZF & (R3_L)) \
             | /* YF is undocumented */ (YF & (R3_L)) \
             | /* HF is set          */ (HF & (0x00)) \
             | /* XF is undocumented */ (XF & (R3_L)) \
             | /* PF is affected     */ (PF & (R3_L)) \
             | /* NF is reset        */ (NF & (0x00)) \
             | /* CF is reset        */ (CF & (0x00)) \
             ; \
    } while(0)

/*
 * xor r08,(r16)
 */

#define m_xor_r08_ind_r16(reg1, reg2) \
    do { \
        R1_L = reg1; \
        R2_L = MREQ_RD(SELF, reg2, 0x00); \
        R0_L = R1_L ^ R2_L; \
        R3_L = PZS[R0_L]; \
        AF_H = R0_L; \
        AF_L = /* SF is affected     */ (SF & (R3_L)) \
             | /* ZF is affected     */ (ZF & (R3_L)) \
             | /* YF is undocumented */ (YF & (R3_L)) \
             | /* HF is set          */ (HF & (0x00)) \
             | /* XF is undocumented */ (XF & (R3_L)) \
             | /* PF is affected     */ (PF & (R3_L)) \
             | /* NF is reset        */ (NF & (0x00)) \
             | /* CF is reset        */ (CF & (0x00)) \
             ; \
    } while(0)

/*
 * xor r08,(r16+i08)
 */

#define m_xor_r08_ind_r16_plus_i08(reg1, reg2) \
    do { \
        WZ_W = reg2 + SBYTE(MREQ_RD(SELF, PC_W++, 0x00)); \
        R1_L = reg1; \
        R2_L = MREQ_RD(SELF, WZ_W, 0x00); \
        m_xor_r08_r08(R1_L, R2_L); \
    } while(0)

// ---------------------------------------------------------------------------
// 8-bit arithmetic group : or
// ---------------------------------------------------------------------------

/*
 * or r08,r08
 */

#define m_or_r08_r08(reg1, reg2) \
    do { \
        R1_L = reg1; \
        R2_L = reg2; \
        R0_L = R1_L | R2_L; \
        R3_L = PZS[R0_L]; \
        AF_H = R0_L; \
        AF_L = /* SF is affected     */ (SF & (R3_L)) \
             | /* ZF is affected     */ (ZF & (R3_L)) \
             | /* YF is undocumented */ (YF & (R3_L)) \
             | /* HF is reset        */ (HF & (0x00)) \
             | /* XF is undocumented */ (XF & (R3_L)) \
             | /* PF is affected     */ (PF & (R3_L)) \
             | /* NF is reset        */ (NF & (0x00)) \
             | /* CF is reset        */ (CF & (0x00)) \
             ; \
    } while(0)

/*
 * or r08,i08
 */

#define m_or_r08_i08(reg1) \
    do { \
        R1_L = reg1; \
        R2_L = MREQ_RD(SELF, PC_W++, 0x00); \
        R0_L = R1_L | R2_L; \
        R3_L = PZS[R0_L]; \
        AF_H = R0_L; \
        AF_L = /* SF is affected     */ (SF & (R3_L)) \
             | /* ZF is affected     */ (ZF & (R3_L)) \
             | /* YF is undocumented */ (YF & (R3_L)) \
             | /* HF is set          */ (HF & (0x00)) \
             | /* XF is undocumented */ (XF & (R3_L)) \
             | /* PF is affected     */ (PF & (R3_L)) \
             | /* NF is reset        */ (NF & (0x00)) \
             | /* CF is reset        */ (CF & (0x00)) \
             ; \
    } while(0)

/*
 * or r08,(r16)
 */

#define m_or_r08_ind_r16(reg1, reg2) \
    do { \
        R1_L = reg1; \
        R2_L = MREQ_RD(SELF, reg2, 0x00); \
        R0_L = R1_L | R2_L; \
        R3_L = PZS[R0_L]; \
        AF_H = R0_L; \
        AF_L = /* SF is affected     */ (SF & (R3_L)) \
             | /* ZF is affected     */ (ZF & (R3_L)) \
             | /* YF is undocumented */ (YF & (R3_L)) \
             | /* HF is set          */ (HF & (0x00)) \
             | /* XF is undocumented */ (XF & (R3_L)) \
             | /* PF is affected     */ (PF & (R3_L)) \
             | /* NF is reset        */ (NF & (0x00)) \
             | /* CF is reset        */ (CF & (0x00)) \
             ; \
    } while(0)

/*
 * or r08,(r16+i08)
 */

#define m_or_r08_ind_r16_plus_i08(reg1, reg2) \
    do { \
        WZ_W = reg2 + SBYTE(MREQ_RD(SELF, PC_W++, 0x00)); \
        R1_L = reg1; \
        R2_L = MREQ_RD(SELF, WZ_W, 0x00); \
        m_or_r08_r08(R1_L, R2_L); \
    } while(0)

// ---------------------------------------------------------------------------
// 8-bit arithmetic group : cp
// ---------------------------------------------------------------------------

/*
 * cp r08,r08
 */

#define m_cp_r08_r08(reg1, reg2) \
    do { \
        R1_L = reg1; \
        R2_L = reg2; \
        R0_W = R1_L - R2_L; \
        R3_L = PZS[R0_L]; \
        AF_L = /* SF is affected     */ (SF & (R3_L)) \
             | /* ZF is affected     */ (ZF & (R3_L)) \
             | /* YF is undocumented */ (YF & (R2_L)) \
             | /* HF is affected     */ (HF & (R0_L ^ R1_L ^ R2_L)) \
             | /* XF is undocumented */ (XF & (R2_L)) \
             | /* VF is affected     */ (VF & ((((R1_L & ~R2_L & ~R0_L) | (~R1_L & R2_L & R0_L)) & SF) != 0 ? 0xff : 0x00)) \
             | /* NF is set          */ (NF & (0xff)) \
             | /* CF is reset        */ (CF & (R0_H & CF)) \
             ; \
    } while(0)

/*
 * cp r08,i08
 */

#define m_cp_r08_i08(reg1) \
    do { \
        R1_L = reg1; \
        R2_L = MREQ_RD(SELF, PC_W++, 0x00); \
        R0_W = R1_L - R2_L; \
        R3_L = PZS[R0_L]; \
        AF_L = /* SF is affected     */ (SF & (R3_L)) \
             | /* ZF is affected     */ (ZF & (R3_L)) \
             | /* YF is undocumented */ (YF & (R2_L)) \
             | /* HF is affected     */ (HF & (R0_L ^ R1_L ^ R2_L)) \
             | /* XF is undocumented */ (XF & (R2_L)) \
             | /* VF is affected     */ (VF & ((((R1_L & ~R2_L & ~R0_L) | (~R1_L & R2_L & R0_L)) & SF) != 0 ? 0xff : 0x00)) \
             | /* NF is set          */ (NF & (0xff)) \
             | /* CF is reset        */ (CF & (R0_H & CF)) \
             ; \
    } while(0)

/*
 * cp r08,(r16)
 */

#define m_cp_r08_ind_r16(reg1, reg2) \
    do { \
        R1_L = reg1; \
        R2_L = MREQ_RD(SELF, reg2, 0x00); \
        R0_W = R1_L - R2_L; \
        R3_L = PZS[R0_L]; \
        AF_L = /* SF is affected     */ (SF & (R3_L)) \
             | /* ZF is affected     */ (ZF & (R3_L)) \
             | /* YF is undocumented */ (YF & (R2_L)) \
             | /* HF is affected     */ (HF & (R0_L ^ R1_L ^ R2_L)) \
             | /* XF is undocumented */ (XF & (R2_L)) \
             | /* VF is affected     */ (VF & ((((R1_L & ~R2_L & ~R0_L) | (~R1_L & R2_L & R0_L)) & SF) != 0 ? 0xff : 0x00)) \
             | /* NF is set          */ (NF & (0xff)) \
             | /* CF is reset        */ (CF & (R0_H & CF)) \
             ; \
    } while(0)

/*
 * cp r08,(r16+i08)
 */

#define m_cp_r08_ind_r16_plus_i08(reg1, reg2) \
    do { \
        WZ_W = reg2 + SBYTE(MREQ_RD(SELF, PC_W++, 0x00)); \
        R1_L = reg1; \
        R2_L = MREQ_RD(SELF, WZ_W, 0x00); \
        m_cp_r08_r08(R1_L, R2_L); \
    } while(0)

// ---------------------------------------------------------------------------
// 8-bit arithmetic group : inc
// ---------------------------------------------------------------------------

/*
 * inc r08
 */

#define m_inc_r08(reg1) \
    do { \
        R1_L = reg1; \
        R2_L = 1; \
        R0_W = R1_L + R2_L; \
        R3_L = PZS[R0_L]; \
        AF_L = /* SF is affected     */ (SF & (R3_L)) \
             | /* ZF is affected     */ (ZF & (R3_L)) \
             | /* YF is undocumented */ (YF & (R3_L)) \
             | /* HF is set          */ (HF & (R0_L ^ R1_L ^ R2_L)) \
             | /* XF is undocumented */ (XF & (R3_L)) \
             | /* VF is affected     */ (VF & (R1_L == 0x7f ? 0xff : 0x00)) \
             | /* NF is reset        */ (NF & (0x00)) \
             | /* CF is not affected */ (CF & (AF_L)) \
             ; \
        reg1 = R0_L; \
    } while(0)

/*
 * inc (r16)
 */

#define m_inc_ind_r16(reg1) \
    do { \
        m_mreq_rd(reg1, R1_L); \
        R2_L = 1; \
        R0_W = R1_L + R2_L; \
        R3_L = PZS[R0_L]; \
        AF_L = /* SF is affected     */ (SF & (R3_L)) \
             | /* ZF is affected     */ (ZF & (R3_L)) \
             | /* YF is undocumented */ (YF & (R3_L)) \
             | /* HF is set          */ (HF & (R0_L ^ R1_L ^ R2_L)) \
             | /* XF is undocumented */ (XF & (R3_L)) \
             | /* VF is affected     */ (VF & (R1_L == 0x7f ? 0xff : 0x00)) \
             | /* NF is reset        */ (NF & (0x00)) \
             | /* CF is not affected */ (CF & (AF_L)) \
             ; \
        m_mreq_wr(reg1, R0_L); \
    } while(0)

/*
 * inc (r16+i08)
 */

#define m_inc_ind_r16_plus_i08(reg1) \
    do { \
        WZ_W = reg1 + SBYTE(MREQ_RD(SELF, PC_W++, 0x00)); \
        m_mreq_rd(WZ_W, R1_L); \
        m_inc_r08(R1_L); \
        m_mreq_wr(WZ_W, R1_L); \
    } while(0)

// ---------------------------------------------------------------------------
// 8-bit arithmetic group : dec
// ---------------------------------------------------------------------------

/*
 * dec r08
 */

#define m_dec_r08(reg1) \
    do { \
        R1_L = reg1; \
        R2_L = 1; \
        R0_W = R1_L - R2_L; \
        R3_L = PZS[R0_L]; \
        AF_L = /* SF is affected     */ (SF & (R3_L)) \
             | /* ZF is affected     */ (ZF & (R3_L)) \
             | /* YF is undocumented */ (YF & (R3_L)) \
             | /* HF is set          */ (HF & (R0_L ^ R1_L ^ R2_L)) \
             | /* XF is undocumented */ (XF & (R3_L)) \
             | /* VF is affected     */ (VF & (R1_L == 0x80 ? 0xff : 0x00)) \
             | /* NF is set          */ (NF & (0xff)) \
             | /* CF is not affected */ (CF & (AF_L)) \
             ; \
        reg1 = R0_L; \
    } while(0)

/*
 * dec (r16)
 */

#define m_dec_ind_r16(reg1) \
    do { \
        m_mreq_rd(reg1, R1_L); \
        R2_L = 1; \
        R0_W = R1_L - R2_L; \
        R3_L = PZS[R0_L]; \
        AF_L = /* SF is affected     */ (SF & (R3_L)) \
             | /* ZF is affected     */ (ZF & (R3_L)) \
             | /* YF is undocumented */ (YF & (R3_L)) \
             | /* HF is set          */ (HF & (R0_L ^ R1_L ^ R2_L)) \
             | /* XF is undocumented */ (XF & (R3_L)) \
             | /* VF is affected     */ (VF & (R1_L == 0x80 ? 0xff : 0x00)) \
             | /* NF is set          */ (NF & (0xff)) \
             | /* CF is not affected */ (CF & (AF_L)) \
             ; \
        m_mreq_wr(reg1, R0_L); \
    } while(0)

/*
 * dec (r16+i08)
 */

#define m_dec_ind_r16_plus_i08(reg1) \
    do { \
        WZ_W = reg1 + SBYTE(MREQ_RD(SELF, PC_W++, 0x00)); \
        m_mreq_rd(WZ_W, R1_L); \
        m_dec_r08(R1_L); \
        m_mreq_wr(WZ_W, R1_L); \
    } while(0)

// ---------------------------------------------------------------------------
// 16-bit arithmetic group : add
// ---------------------------------------------------------------------------

/*
 * add r16,r16
 */

#define m_add_r16_r16(reg1, reg2) \
    do { \
        R1_W = reg1; \
        R2_W = reg2; \
        R0_W = (R1_W + R2_W); \
        AF_L = /* SF is not affected */ (SF & (AF_L)) \
             | /* ZF is not affected */ (ZF & (AF_L)) \
             | /* YF is undocumented */ (YF & (R0_H)) \
             | /* HF is affected     */ (HF & ((R0_W ^ R1_W ^ R2_W) & 0x1000 ? 0xff : 0x00)) \
             | /* XF is undocumented */ (XF & (R0_H)) \
             | /* VF is not affected */ (VF & (AF_L)) \
             | /* NF is reset        */ (NF & (0x00)) \
             | /* CF is affected     */ (CF & ((SLONG(R1_W) + SLONG(R2_W)) & 0x10000 ? 0xff : 0x00)) \
             ; \
        reg1 = R0_W; \
    } while(0)

// ---------------------------------------------------------------------------
// 16-bit arithmetic group : adc
// ---------------------------------------------------------------------------

/*
 * adc r16,r16
 */

#define m_adc_r16_r16(reg1, reg2) \
    do { \
        R1_W = reg1; \
        R2_W = reg2; \
        R3_W = ((AF_L & CF) != 0 ? 1 : 0); \
        R0_W = (R1_W + R2_W + R3_W); \
        AF_L = /* SF is affected     */ (SF & (R0_H)) \
             | /* ZF is affected     */ (ZF & (R0_W == 0 ? 0xff : 0x00)) \
             | /* YF is undocumented */ (YF & (R0_H)) \
             | /* HF is affected     */ (HF & ((R0_W ^ R1_W ^ R2_W) & 0x1000 ? 0xff : 0x00)) \
             | /* XF is undocumented */ (XF & (R0_H)) \
             | /* VF is affected     */ (VF & (~(R1_W ^ R2_W) & (R2_W ^ R0_W) & 0x8000 ? 0xff : 0x00)) \
             | /* NF is reset        */ (NF & (0x00)) \
             | /* CF is affected     */ (CF & ((SLONG(R1_W) + SLONG(R2_W) + SLONG(R3_W)) & 0x10000 ? 0xff : 0x00)) \
             ; \
        reg1 = R0_W; \
    } while(0)

// ---------------------------------------------------------------------------
// 16-bit arithmetic group : sbc
// ---------------------------------------------------------------------------

/*
 * sbc r16,r16
 */

#define m_sbc_r16_r16(reg1, reg2) \
    do { \
        R1_W = reg1; \
        R2_W = reg2; \
        R3_W = ((AF_L & CF) != 0 ? 1 : 0); \
        R0_W = (R1_W - R2_W - R3_W); \
        AF_L = /* SF is affected     */ (SF & (R0_H)) \
             | /* ZF is affected     */ (ZF & (R0_W == 0 ? 0xff : 0x00)) \
             | /* YF is undocumented */ (YF & (R0_H)) \
             | /* HF is affected     */ (HF & ((R0_W ^ R1_W ^ R2_W) & 0x1000 ? 0xff : 0x00)) \
             | /* XF is undocumented */ (XF & (R0_H)) \
             | /* VF is affected     */ (VF & ((R1_W ^ R2_W) & (R1_W ^ R0_W) & 0x8000 ? 0xff : 0x00)) \
             | /* NF is reset        */ (NF & (0xff)) \
             | /* CF is affected     */ (CF & ((SLONG(R1_W) - SLONG(R2_W) - SLONG(R3_W)) & 0x10000 ? 0xff : 0x00)) \
             ; \
        reg1 = R0_W; \
    } while(0)

// ---------------------------------------------------------------------------
// 16-bit arithmetic group : inc
// ---------------------------------------------------------------------------

/*
 * inc r16
 */

#define m_inc_r16(reg1) \
    do { \
        ++reg1; \
    } while(0)

// ---------------------------------------------------------------------------
// 16-bit arithmetic group : dec
// ---------------------------------------------------------------------------

/*
 * dec r16
 */

#define m_dec_r16(reg1) \
    do { \
        --reg1; \
    } while(0)

// ---------------------------------------------------------------------------
// jump group : djnz
// ---------------------------------------------------------------------------

/*
 * djnz i08
 */

#define m_djnz_i08() \
    do { \
        --BC_H; \
        if(BC_H != 0) { \
            WZ_W = PC_W + SBYTE(MREQ_RD(SELF, PC_W, 0x00)) + 1; \
            PC_W = WZ_W; \
            m_consume(1, 5); \
        } \
        else { \
            PC_W += 1; \
        } \
    } while(0)

// ---------------------------------------------------------------------------
// jump group : jp
// ---------------------------------------------------------------------------

/*
 * jp r16
 */

#define m_jp_r16(reg1) \
    do { \
        PC_W = reg1; \
    } while(0)

/*
 * jp i16
 */

#define m_jp_i16() \
    do { \
        m_mreq_rd(PC_W++, WZ_L); \
        m_mreq_rd(PC_W++, WZ_H); \
        PC_W = WZ_W; \
    } while(0)

/*
 * jp nz,i16
 */

#define m_jp_nz_i16() \
    do { \
        if((AF_L & ZF) == 0) { \
            m_mreq_rd(PC_W++, WZ_L); \
            m_mreq_rd(PC_W++, WZ_H); \
        } \
        else { \
            WZ_W = PC_W + 2; \
        } \
        PC_W = WZ_W; \
    } while(0)

/*
 * jp z,i16
 */

#define m_jp_z_i16() \
    do { \
        if((AF_L & ZF) != 0) { \
            m_mreq_rd(PC_W++, WZ_L); \
            m_mreq_rd(PC_W++, WZ_H); \
        } \
        else { \
            WZ_W = PC_W + 2; \
        } \
        PC_W = WZ_W; \
    } while(0)

/*
 * jp nc,i16
 */

#define m_jp_nc_i16() \
    do { \
        if((AF_L & CF) == 0) { \
            m_mreq_rd(PC_W++, WZ_L); \
            m_mreq_rd(PC_W++, WZ_H); \
        } \
        else { \
            WZ_W = PC_W + 2; \
        } \
        PC_W = WZ_W; \
    } while(0)

/*
 * jp c,i16
 */

#define m_jp_c_i16() \
    do { \
        if((AF_L & CF) != 0) { \
            m_mreq_rd(PC_W++, WZ_L); \
            m_mreq_rd(PC_W++, WZ_H); \
        } \
        else { \
            WZ_W = PC_W + 2; \
        } \
        PC_W = WZ_W; \
    } while(0)

/*
 * jp po,i16
 */

#define m_jp_po_i16() \
    do { \
        if((AF_L & PF) == 0) { \
            m_mreq_rd(PC_W++, WZ_L); \
            m_mreq_rd(PC_W++, WZ_H); \
        } \
        else { \
            WZ_W = PC_W + 2; \
        } \
        PC_W = WZ_W; \
    } while(0)

/*
 * jp pe,i16
 */

#define m_jp_pe_i16() \
    do { \
        if((AF_L & PF) != 0) { \
            m_mreq_rd(PC_W++, WZ_L); \
            m_mreq_rd(PC_W++, WZ_H); \
        } \
        else { \
            WZ_W = PC_W + 2; \
        } \
        PC_W = WZ_W; \
    } while(0)

/*
 * jp p,i16
 */

#define m_jp_p_i16() \
    do { \
        if((AF_L & SF) == 0) { \
            m_mreq_rd(PC_W++, WZ_L); \
            m_mreq_rd(PC_W++, WZ_H); \
        } \
        else { \
            WZ_W = PC_W + 2; \
        } \
        PC_W = WZ_W; \
    } while(0)

/*
 * jp m,i16
 */

#define m_jp_m_i16() \
    do { \
        if((AF_L & SF) != 0) { \
            m_mreq_rd(PC_W++, WZ_L); \
            m_mreq_rd(PC_W++, WZ_H); \
        } \
        else { \
            WZ_W = PC_W + 2; \
        } \
        PC_W = WZ_W; \
    } while(0)

// ---------------------------------------------------------------------------
// jump group : jr
// ---------------------------------------------------------------------------

/*
 * jr i08
 */

#define m_jr_i08() \
    do { \
        WZ_W = PC_W + SBYTE(MREQ_RD(SELF, PC_W, 0x00)) + 1; \
        PC_W = WZ_W; \
    } while(0)

/*
 * jr nz,i08
 */

#define m_jr_nz_i08() \
    do { \
        if((AF_L & ZF) == 0) { \
            WZ_W = PC_W + SBYTE(MREQ_RD(SELF, PC_W, 0x00)) + 1; \
            PC_W = WZ_W; \
            m_consume(1, 5); \
        } \
        else { \
            PC_W += 1; \
        } \
    } while(0)

/*
 * jr z,i08
 */

#define m_jr_z_i08() \
    do { \
        if((AF_L & ZF) != 0) { \
            WZ_W = PC_W + SBYTE(MREQ_RD(SELF, PC_W, 0x00)) + 1; \
            PC_W = WZ_W; \
            m_consume(1, 5); \
        } \
        else { \
            PC_W += 1; \
        } \
    } while(0)

/*
 * jr nc,i08
 */

#define m_jr_nc_i08() \
    do { \
        if((AF_L & CF) == 0) { \
            WZ_W = PC_W + SBYTE(MREQ_RD(SELF, PC_W, 0x00)) + 1; \
            PC_W = WZ_W; \
            m_consume(1, 5); \
        } \
        else { \
            PC_W += 1; \
        } \
    } while(0)

/*
 * jr c,i08
 */

#define m_jr_c_i08() \
    do { \
        if((AF_L & CF) != 0) { \
            WZ_W = PC_W + SBYTE(MREQ_RD(SELF, PC_W, 0x00)) + 1; \
            PC_W = WZ_W; \
            m_consume(1, 5); \
        } \
        else { \
            PC_W += 1; \
        } \
    } while(0)

// ---------------------------------------------------------------------------
// call and return group : rst
// ---------------------------------------------------------------------------

/*
 * rst p
 */

#define m_rst_vec16(addr) \
    do { \
        m_mreq_wr(--SP_W, PC_H); \
        m_mreq_wr(--SP_W, PC_L); \
        m_load_rg(WZ_W, addr); \
        m_load_rg(PC_W, WZ_W); \
    } while(0)

// ---------------------------------------------------------------------------
// call and return group : call
// ---------------------------------------------------------------------------

/*
 * call i16
 */

#define m_call_i16() \
    do { \
        m_mreq_rd(PC_W++, WZ_L); \
        m_mreq_rd(PC_W++, WZ_H); \
        m_mreq_wr(--SP_W, PC_H); \
        m_mreq_wr(--SP_W, PC_L); \
        m_load_rg(PC_W  , WZ_W); \
    } while(0)

/*
 * call nz,i16
 */

#define m_call_nz_i16() \
    do { \
        if((AF_L & ZF) == 0) { \
            m_mreq_rd(PC_W++, WZ_L); \
            m_mreq_rd(PC_W++, WZ_H); \
            m_mreq_wr(--SP_W, PC_H); \
            m_mreq_wr(--SP_W, PC_L); \
            m_load_rg(PC_W  , WZ_W); \
            m_consume(2, 7); \
        } \
        else { \
            m_load_rg(WZ_W, PC_W + 2); \
            m_load_rg(PC_W, WZ_W); \
        } \
    } while(0)

/*
 * call z,i16
 */

#define m_call_z_i16() \
    do { \
        if((AF_L & ZF) != 0) { \
            m_mreq_rd(PC_W++, WZ_L); \
            m_mreq_rd(PC_W++, WZ_H); \
            m_mreq_wr(--SP_W, PC_H); \
            m_mreq_wr(--SP_W, PC_L); \
            m_load_rg(PC_W  , WZ_W); \
            m_consume(2, 7); \
        } \
        else { \
            m_load_rg(WZ_W, PC_W + 2); \
            m_load_rg(PC_W, WZ_W); \
        } \
    } while(0)

/*
 * call nc,i16
 */

#define m_call_nc_i16() \
    do { \
        if((AF_L & CF) == 0) { \
            m_mreq_rd(PC_W++, WZ_L); \
            m_mreq_rd(PC_W++, WZ_H); \
            m_mreq_wr(--SP_W, PC_H); \
            m_mreq_wr(--SP_W, PC_L); \
            m_load_rg(PC_W  , WZ_W); \
            m_consume(2, 7); \
        } \
        else { \
            m_load_rg(WZ_W, PC_W + 2); \
            m_load_rg(PC_W, WZ_W); \
        } \
    } while(0)

/*
 * call c,i16
 */

#define m_call_c_i16() \
    do { \
        if((AF_L & CF) != 0) { \
            m_mreq_rd(PC_W++, WZ_L); \
            m_mreq_rd(PC_W++, WZ_H); \
            m_mreq_wr(--SP_W, PC_H); \
            m_mreq_wr(--SP_W, PC_L); \
            m_load_rg(PC_W  , WZ_W); \
            m_consume(2, 7); \
        } \
        else { \
            m_load_rg(WZ_W, PC_W + 2); \
            m_load_rg(PC_W, WZ_W); \
        } \
    } while(0)

/*
 * call po,i16
 */

#define m_call_po_i16() \
    do { \
        if((AF_L & PF) == 0) { \
            m_mreq_rd(PC_W++, WZ_L); \
            m_mreq_rd(PC_W++, WZ_H); \
            m_mreq_wr(--SP_W, PC_H); \
            m_mreq_wr(--SP_W, PC_L); \
            m_load_rg(PC_W  , WZ_W); \
            m_consume(2, 7); \
        } \
        else { \
            m_load_rg(WZ_W, PC_W + 2); \
            m_load_rg(PC_W, WZ_W); \
        } \
    } while(0)

/*
 * call pe,i16
 */

#define m_call_pe_i16() \
    do { \
        if((AF_L & PF) != 0) { \
            m_mreq_rd(PC_W++, WZ_L); \
            m_mreq_rd(PC_W++, WZ_H); \
            m_mreq_wr(--SP_W, PC_H); \
            m_mreq_wr(--SP_W, PC_L); \
            m_load_rg(PC_W  , WZ_W); \
            m_consume(2, 7); \
        } \
        else { \
            m_load_rg(WZ_W, PC_W + 2); \
            m_load_rg(PC_W, WZ_W); \
        } \
    } while(0)

/*
 * call p,i16
 */

#define m_call_p_i16() \
    do { \
        if((AF_L & SF) == 0) { \
            m_mreq_rd(PC_W++, WZ_L); \
            m_mreq_rd(PC_W++, WZ_H); \
            m_mreq_wr(--SP_W, PC_H); \
            m_mreq_wr(--SP_W, PC_L); \
            m_load_rg(PC_W  , WZ_W); \
            m_consume(2, 7); \
        } \
        else { \
            m_load_rg(WZ_W, PC_W + 2); \
            m_load_rg(PC_W, WZ_W); \
        } \
    } while(0)

/*
 * call m,i16
 */

#define m_call_m_i16() \
    do { \
        if((AF_L & SF) != 0) { \
            m_mreq_rd(PC_W++, WZ_L); \
            m_mreq_rd(PC_W++, WZ_H); \
            m_mreq_wr(--SP_W, PC_H); \
            m_mreq_wr(--SP_W, PC_L); \
            m_load_rg(PC_W  , WZ_W); \
            m_consume(2, 7); \
        } \
        else { \
            m_load_rg(WZ_W, PC_W + 2); \
            m_load_rg(PC_W, WZ_W); \
        } \
    } while(0)

// ---------------------------------------------------------------------------
// call and return group : ret
// ---------------------------------------------------------------------------

/*
 * ret
 */

#define m_ret() \
    do { \
        m_mreq_rd(SP_W++, WZ_L); \
        m_mreq_rd(SP_W++, WZ_H); \
        m_load_rg(PC_W  , WZ_W); \
    } while(0)

/*
 * ret nz
 */

#define m_ret_nz() \
    do { \
        if((AF_L & ZF) == 0) { \
            m_mreq_rd(SP_W++, WZ_L); \
            m_mreq_rd(SP_W++, WZ_H); \
            m_load_rg(PC_W  , WZ_W); \
            m_consume(2, 6); \
        } \
    } while(0)

/*
 * ret z
 */

#define m_ret_z() \
    do { \
        if((AF_L & ZF) != 0) { \
            m_mreq_rd(SP_W++, WZ_L); \
            m_mreq_rd(SP_W++, WZ_H); \
            m_load_rg(PC_W  , WZ_W); \
            m_consume(2, 6); \
        } \
    } while(0)

/*
 * ret nc
 */

#define m_ret_nc() \
    do { \
        if((AF_L & CF) == 0) { \
            m_mreq_rd(SP_W++, WZ_L); \
            m_mreq_rd(SP_W++, WZ_H); \
            m_load_rg(PC_W  , WZ_W); \
            m_consume(2, 6); \
        } \
    } while(0)

/*
 * ret c
 */

#define m_ret_c() \
    do { \
        if((AF_L & CF) != 0) { \
            m_mreq_rd(SP_W++, WZ_L); \
            m_mreq_rd(SP_W++, WZ_H); \
            m_load_rg(PC_W  , WZ_W); \
            m_consume(2, 6); \
        } \
    } while(0)

/*
 * ret po
 */

#define m_ret_po() \
    do { \
        if((AF_L & PF) == 0) { \
            m_mreq_rd(SP_W++, WZ_L); \
            m_mreq_rd(SP_W++, WZ_H); \
            m_load_rg(PC_W  , WZ_W); \
            m_consume(2, 6); \
        } \
    } while(0)

/*
 * ret pe
 */

#define m_ret_pe() \
    do { \
        if((AF_L & PF) != 0) { \
            m_mreq_rd(SP_W++, WZ_L); \
            m_mreq_rd(SP_W++, WZ_H); \
            m_load_rg(PC_W  , WZ_W); \
            m_consume(2, 6); \
        } \
    } while(0)

/*
 * ret p
 */

#define m_ret_p() \
    do { \
        if((AF_L & SF) == 0) { \
            m_mreq_rd(SP_W++, WZ_L); \
            m_mreq_rd(SP_W++, WZ_H); \
            m_load_rg(PC_W  , WZ_W); \
            m_consume(2, 6); \
        } \
    } while(0)

/*
 * ret m
 */

#define m_ret_m() \
    do { \
        if((AF_L & SF) != 0) { \
            m_mreq_rd(SP_W++, WZ_L); \
            m_mreq_rd(SP_W++, WZ_H); \
            m_load_rg(PC_W  , WZ_W); \
            m_consume(2, 6); \
        } \
    } while(0)

// ---------------------------------------------------------------------------
// call and return group : reti/retn
// ---------------------------------------------------------------------------

/*
 * reti
 */

#define m_reti() \
    do { \
        m_mreq_rd(SP_W++, WZ_L); \
        m_mreq_rd(SP_W++, WZ_H); \
        m_load_rg(PC_W  , WZ_W); \
    } while(0)

/*
 * retn
 */

#define m_retn() \
    do { \
        if((ST_L & ST_IFF2) != 0) { \
            m_setu_bit(ST_L, (ST_IFF1)); \
        } \
        else { \
            m_clru_bit(ST_L, (ST_IFF1)); \
        } \
        m_mreq_rd(SP_W++, WZ_L); \
        m_mreq_rd(SP_W++, WZ_H); \
        m_load_rg(PC_W  , WZ_W); \
    } while(0)

// ---------------------------------------------------------------------------
// exchange group : ex/exx
// ---------------------------------------------------------------------------

/*
 * exx
 */

#define m_exx() \
    do { \
        m_load_rg(R0_W, BC_W); \
        m_load_rg(BC_W, BC_P); \
        m_load_rg(BC_P, R0_W); \
        m_load_rg(R0_W, DE_W); \
        m_load_rg(DE_W, DE_P); \
        m_load_rg(DE_P, R0_W); \
        m_load_rg(R0_W, HL_W); \
        m_load_rg(HL_W, HL_P); \
        m_load_rg(HL_P, R0_W); \
    } while(0)

/*
 * ex r16,r16
 */

#define m_ex_r16_r16(reg1, reg2) \
    do { \
        m_load_rg(R0_W, reg1); \
        m_load_rg(reg1, reg2); \
        m_load_rg(reg2, R0_W); \
    } while(0)

/*
 * ex (r16),r16
 */

#define m_ex_ind_r16_r16(reg1, reg2) \
    do { \
        m_load_rg(R1_W, reg1); \
        m_load_rg(R2_W, reg2); \
        m_mreq_rd((R1_W + 0), R0_L); \
        m_mreq_rd((R1_W + 1), R0_H); \
        m_mreq_wr((R1_W + 0), R2_L); \
        m_mreq_wr((R1_W + 1), R2_H); \
        m_load_rg(reg2, R0_W); \
    } while(0)

// ---------------------------------------------------------------------------
// block transfer group : ldi/ldir/ldd/lddr
// ---------------------------------------------------------------------------

/*
 * ldi
 */

#define m_ldi() \
    do { \
        R1_L = AF_H; \
        m_mreq_rd(HL_W, R2_L); \
        m_mreq_wr(DE_W, R2_L); \
        R0_L = R1_L + R2_L; \
        ++HL_W; \
        ++DE_W; \
        --BC_W; \
        AF_L = /* SF is not affected */ (SF & (AF_L)) \
             | /* ZF is not affected */ (ZF & (AF_L)) \
             | /* YF is undocumented */ (YF & ((R0_L & BIT1) != 0 ? 0xff : 0x00)) \
             | /* HF is reset        */ (HF & (0x00)) \
             | /* XF is undocumented */ (XF & ((R0_L & BIT3) != 0 ? 0xff : 0x00)) \
             | /* PF is affected     */ (PF & (BC_W != 0 ? 0xff : 0x00)) \
             | /* NF is reset        */ (NF & (0x00)) \
             | /* CF is not affected */ (CF & (AF_L)) \
             ; \
    } while(0)

/*
 * ldd
 */

#define m_ldd() \
    do { \
        R1_L = AF_H; \
        m_mreq_rd(HL_W, R2_L); \
        m_mreq_wr(DE_W, R2_L); \
        R0_L = R1_L + R2_L; \
        --HL_W; \
        --DE_W; \
        --BC_W; \
        AF_L = /* SF is not affected */ (SF & (AF_L)) \
             | /* ZF is not affected */ (ZF & (AF_L)) \
             | /* YF is undocumented */ (YF & ((R0_L & BIT1) != 0 ? 0xff : 0x00)) \
             | /* HF is reset        */ (HF & (0x00)) \
             | /* XF is undocumented */ (XF & ((R0_L & BIT3) != 0 ? 0xff : 0x00)) \
             | /* PF is affected     */ (PF & (BC_W != 0 ? 0xff : 0x00)) \
             | /* NF is reset        */ (NF & (0x00)) \
             | /* CF is not affected */ (CF & (AF_L)) \
             ; \
    } while(0)

/*
 * ldir
 */

#define m_ldir() \
    do { \
        R1_L = AF_H; \
        m_mreq_rd(HL_W, R2_L); \
        m_mreq_wr(DE_W, R2_L); \
        R0_L = R1_L + R2_L; \
        ++HL_W; \
        ++DE_W; \
        --BC_W; \
        AF_L = /* SF is not affected */ (SF & (AF_L)) \
             | /* ZF is not affected */ (ZF & (AF_L)) \
             | /* YF is undocumented */ (YF & ((R0_L & BIT1) != 0 ? 0xff : 0x00)) \
             | /* HF is reset        */ (HF & (0x00)) \
             | /* XF is undocumented */ (XF & ((R0_L & BIT3) != 0 ? 0xff : 0x00)) \
             | /* PF is affected     */ (PF & (BC_W != 0 ? 0xff : 0x00)) \
             | /* NF is reset        */ (NF & (0x00)) \
             | /* CF is not affected */ (CF & (AF_L)) \
             ; \
        if(BC_W != 0) { \
            m_restore_pc(); \
            m_consume(1, 5); \
        } \
        else { \
            WZ_W = PC_W; \
        } \
    } while(0)

/*
 * lddr
 */

#define m_lddr() \
    do { \
        R1_L = AF_H; \
        m_mreq_rd(HL_W, R2_L); \
        m_mreq_wr(DE_W, R2_L); \
        R0_L = R1_L + R2_L; \
        --HL_W; \
        --DE_W; \
        --BC_W; \
        AF_L = /* SF is not affected */ (SF & (AF_L)) \
             | /* ZF is not affected */ (ZF & (AF_L)) \
             | /* YF is undocumented */ (YF & ((R0_L & BIT1) != 0 ? 0xff : 0x00)) \
             | /* HF is reset        */ (HF & (0x00)) \
             | /* XF is undocumented */ (XF & ((R0_L & BIT3) != 0 ? 0xff : 0x00)) \
             | /* PF is affected     */ (PF & (BC_W != 0 ? 0xff : 0x00)) \
             | /* NF is reset        */ (NF & (0x00)) \
             | /* CF is not affected */ (CF & (AF_L)) \
             ; \
        if(BC_W != 0) { \
            m_restore_pc(); \
            m_consume(1, 5); \
        } \
        else { \
            WZ_W = PC_W; \
        } \
    } while(0)

// ---------------------------------------------------------------------------
// search group : cpi/cpir/cpd/cpdr
// ---------------------------------------------------------------------------

/*
 * cpi
 */

#define m_cpi() \
    do { \
        R1_L = AF_H; \
        m_mreq_rd(HL_W, R2_L); \
        R0_L = R1_L - R2_L; \
        ++HL_W; \
        --BC_W; \
        R3_L = PZS[R0_L]; \
        AF_L = /* SF is affected     */ (SF & (R3_L)) \
             | /* ZF is affected     */ (ZF & (R3_L)) \
             | /* YF is undocumented */ (YF & (0x00)) \
             | /* HF is affected     */ (HF & (R0_L ^ R1_L ^ R2_L)) \
             | /* XF is undocumented */ (XF & (0x00)) \
             | /* PF is affected     */ (PF & (BC_W != 0 ? 0xff : 0x00)) \
             | /* NF is set          */ (NF & (0xff)) \
             | /* CF is not affected */ (CF & (AF_L)) \
             ; \
        R0_L = R0_L - ((AF_L & HF) != 0 ? 1 : 0); \
        AF_L = AF_L \
             | /* YF is undocumented */ (YF & ((R0_L & BIT1) != 0 ? 0xff : 0x00)) \
             | /* XF is undocumented */ (XF & ((R0_L & BIT3) != 0 ? 0xff : 0x00)) \
             ; \
    } while(0)

/*
 * cpd
 */

#define m_cpd() \
    do { \
        R1_L = AF_H; \
        m_mreq_rd(HL_W, R2_L); \
        R0_L = R1_L - R2_L; \
        --HL_W; \
        --BC_W; \
        R3_L = PZS[R0_L]; \
        AF_L = /* SF is affected     */ (SF & (R3_L)) \
             | /* ZF is affected     */ (ZF & (R3_L)) \
             | /* YF is undocumented */ (YF & (0x00)) \
             | /* HF is affected     */ (HF & (R0_L ^ R1_L ^ R2_L)) \
             | /* XF is undocumented */ (XF & (0x00)) \
             | /* PF is affected     */ (PF & (BC_W != 0 ? 0xff : 0x00)) \
             | /* NF is set          */ (NF & (0xff)) \
             | /* CF is not affected */ (CF & (AF_L)) \
             ; \
        R0_L = R0_L - ((AF_L & HF) != 0 ? 1 : 0); \
        AF_L = AF_L \
             | /* YF is undocumented */ (YF & ((R0_L & BIT1) != 0 ? 0xff : 0x00)) \
             | /* XF is undocumented */ (XF & ((R0_L & BIT3) != 0 ? 0xff : 0x00)) \
             ; \
    } while(0)

/*
 * cpir
 */

#define m_cpir() \
    do { \
        R1_L = AF_H; \
        m_mreq_rd(HL_W, R2_L); \
        R0_L = R1_L - R2_L; \
        ++HL_W; \
        --BC_W; \
        R3_L = PZS[R0_L]; \
        AF_L = /* SF is affected     */ (SF & (R3_L)) \
             | /* ZF is affected     */ (ZF & (R3_L)) \
             | /* YF is undocumented */ (YF & (0x00)) \
             | /* HF is affected     */ (HF & (R0_L ^ R1_L ^ R2_L)) \
             | /* XF is undocumented */ (XF & (0x00)) \
             | /* PF is affected     */ (PF & (BC_W != 0 ? 0xff : 0x00)) \
             | /* NF is set          */ (NF & (0xff)) \
             | /* CF is not affected */ (CF & (AF_L)) \
             ; \
        if((BC_W != 0) && (R0_L != 0)) { \
            m_restore_pc(); \
            m_consume(1, 5); \
        } \
        R0_L = R0_L - ((AF_L & HF) != 0 ? 1 : 0); \
        AF_L = AF_L \
             | /* YF is undocumented */ (YF & ((R0_L & BIT1) != 0 ? 0xff : 0x00)) \
             | /* XF is undocumented */ (XF & ((R0_L & BIT3) != 0 ? 0xff : 0x00)) \
             ; \
    } while(0)

/*
 * cpdr
 */

#define m_cpdr() \
    do { \
        R1_L = AF_H; \
        m_mreq_rd(HL_W, R2_L); \
        R0_L = R1_L - R2_L; \
        --HL_W; \
        --BC_W; \
        R3_L = PZS[R0_L]; \
        AF_L = /* SF is affected     */ (SF & (R3_L)) \
             | /* ZF is affected     */ (ZF & (R3_L)) \
             | /* YF is undocumented */ (YF & (0x00)) \
             | /* HF is affected     */ (HF & (R0_L ^ R1_L ^ R2_L)) \
             | /* XF is undocumented */ (XF & (0x00)) \
             | /* PF is affected     */ (PF & (BC_W != 0 ? 0xff : 0x00)) \
             | /* NF is set          */ (NF & (0xff)) \
             | /* CF is not affected */ (CF & (AF_L)) \
             ; \
        if((BC_W != 0) && (R0_L != 0)) { \
            m_restore_pc(); \
            m_consume(1, 5); \
        } \
        R0_L = R0_L - ((AF_L & HF) != 0 ? 1 : 0); \
        AF_L = AF_L \
             | /* YF is undocumented */ (YF & ((R0_L & BIT1) != 0 ? 0xff : 0x00)) \
             | /* XF is undocumented */ (XF & ((R0_L & BIT3) != 0 ? 0xff : 0x00)) \
             ; \
    } while(0)

// ---------------------------------------------------------------------------
// rotate and shift group
// ---------------------------------------------------------------------------

/*
 * rlca
 */

#define m_rlca() \
    do { \
        R1_L = AF_H; \
        R0_L = (UBYTE(R1_L) << 1) | (UBYTE(R1_L) >> 7); \
        AF_H = R0_L; \
        AF_L = /* SF is not affected */ (SF & (AF_L)) \
             | /* ZF is not affected */ (ZF & (AF_L)) \
             | /* YF is undocumented */ (YF & (R0_L)) \
             | /* HF is reset        */ (HF & (0x00)) \
             | /* XF is undocumented */ (XF & (R0_L)) \
             | /* PF is not affected */ (PF & (AF_L)) \
             | /* NF is reset        */ (NF & (0x00)) \
             | /* CF is data bit7    */ (CF & (R1_L & BIT7 ? 0xff : 0x00)) \
             ; \
    } while(0)

/*
 * rrca
 */

#define m_rrca() \
    do { \
        R1_L = AF_H; \
        R0_L = (UBYTE(R1_L) >> 1) | (UBYTE(R1_L) << 7); \
        AF_H = R0_L; \
        AF_L = /* SF is not affected */ (SF & (AF_L)) \
             | /* ZF is not affected */ (ZF & (AF_L)) \
             | /* YF is undocumented */ (YF & (R0_L)) \
             | /* HF is reset        */ (HF & (0x00)) \
             | /* XF is undocumented */ (XF & (R0_L)) \
             | /* PF is not affected */ (PF & (AF_L)) \
             | /* NF is reset        */ (NF & (0x00)) \
             | /* CF is data bit0    */ (CF & (R1_L & BIT0 ? 0xff : 0x00)) \
             ; \
    } while(0)

/*
 * rla
 */

#define m_rla() \
    do { \
        R1_L = AF_H; \
        R0_L = (UBYTE(R1_L) << 1) | (AF_L & CF ? BIT0 : 0x00); \
        AF_H = R0_L; \
        AF_L = /* SF is not affected */ (SF & (AF_L)) \
             | /* ZF is not affected */ (ZF & (AF_L)) \
             | /* YF is undocumented */ (YF & (R0_L)) \
             | /* HF is reset        */ (HF & (0x00)) \
             | /* XF is undocumented */ (XF & (R0_L)) \
             | /* PF is not affected */ (PF & (AF_L)) \
             | /* NF is reset        */ (NF & (0x00)) \
             | /* CF is data bit7    */ (CF & (R1_L & BIT7 ? 0xff : 0x00)) \
             ; \
    } while(0)

/*
 * rra
 */

#define m_rra() \
    do { \
        R1_L = AF_H; \
        R0_L = (UBYTE(R1_L) >> 1) | (AF_L & CF ? BIT7 : 0x00); \
        AF_H = R0_L; \
        AF_L = /* SF is not affected */ (SF & (AF_L)) \
             | /* ZF is not affected */ (ZF & (AF_L)) \
             | /* YF is undocumented */ (YF & (R0_L)) \
             | /* HF is reset        */ (HF & (0x00)) \
             | /* XF is undocumented */ (XF & (R0_L)) \
             | /* PF is not affected */ (PF & (AF_L)) \
             | /* NF is reset        */ (NF & (0x00)) \
             | /* CF is data bit0    */ (CF & (R1_L & BIT0 ? 0xff : 0x00)) \
             ; \
    } while(0)

/*
 * rld
 */

#define m_rld() \
    do { \
        m_mreq_rd(HL_W, R1_L); \
        R0_L = ((LO_NIBBLE(R1_L) << 4) | (LO_NIBBLE(AF_H) >> 0)); \
        AF_H = ((HI_NIBBLE(AF_H) << 0) | (HI_NIBBLE(R1_L) >> 4)); \
        R3_L = PZS[AF_H]; \
        AF_L = /* SF is affected     */ (SF & (R3_L)) \
             | /* ZF is affected     */ (ZF & (R3_L)) \
             | /* YF is undocumented */ (YF & (AF_H)) \
             | /* HF is reset        */ (HF & (0x00)) \
             | /* XF is undocumented */ (XF & (AF_H)) \
             | /* PF is affected     */ (PF & (R3_L)) \
             | /* NF is reset        */ (NF & (0x00)) \
             | /* CF is not affected */ (CF & (AF_L)) \
             ; \
        m_mreq_wr(HL_W, R0_L); \
    } while(0)

/*
 * rrd
 */

#define m_rrd() \
    do { \
        m_mreq_rd(HL_W, R1_L); \
        R0_L = ((LO_NIBBLE(AF_H) << 4) | (HI_NIBBLE(R1_L) >> 4)); \
        AF_H = ((HI_NIBBLE(AF_H) << 0) | (LO_NIBBLE(R1_L) >> 0)); \
        R3_L = PZS[AF_H]; \
        AF_L = /* SF is affected     */ (SF & (R3_L)) \
             | /* ZF is affected     */ (ZF & (R3_L)) \
             | /* YF is undocumented */ (YF & (AF_H)) \
             | /* HF is reset        */ (HF & (0x00)) \
             | /* XF is undocumented */ (XF & (AF_H)) \
             | /* PF is affected     */ (PF & (R3_L)) \
             | /* NF is reset        */ (NF & (0x00)) \
             | /* CF is not affected */ (CF & (AF_L)) \
             ; \
        m_mreq_wr(HL_W, R0_L); \
    } while(0)

// ---------------------------------------------------------------------------
// rotate and shift group
// ---------------------------------------------------------------------------

/*
 * rlc r08
 */

#define m_rlc_r08(data) \
    do { \
        R1_L = data; \
        R0_L = (UBYTE(R1_L) << 1) | (UBYTE(R1_L) >> 7); \
        R3_L = PZS[R0_L]; \
        AF_L = /* SF is affected     */ (SF & (R3_L)) \
             | /* ZF is affected     */ (ZF & (R3_L)) \
             | /* YF is undocumented */ (YF & (R3_L)) \
             | /* HF is reset        */ (HF & (0x00)) \
             | /* XF is undocumented */ (XF & (R3_L)) \
             | /* PF is affected     */ (PF & (R3_L)) \
             | /* NF is reset        */ (NF & (0x00)) \
             | /* CF is data bit7    */ (CF & (R1_L & BIT7 ? 0xff : 0x00)) \
             ; \
        data = R0_L; \
    } while(0)

/*
 * rlc (r16)
 */

#define m_rlc_ind_r16(addr) \
    do { \
        m_mreq_rd(addr, R1_L); \
        R0_L = (UBYTE(R1_L) << 1) | (UBYTE(R1_L) >> 7); \
        R3_L = PZS[R0_L]; \
        AF_L = /* SF is affected     */ (SF & (R3_L)) \
             | /* ZF is affected     */ (ZF & (R3_L)) \
             | /* YF is undocumented */ (YF & (R3_L)) \
             | /* HF is reset        */ (HF & (0x00)) \
             | /* XF is undocumented */ (XF & (R3_L)) \
             | /* PF is affected     */ (PF & (R3_L)) \
             | /* NF is reset        */ (NF & (0x00)) \
             | /* CF is data bit7    */ (CF & (R1_L & BIT7 ? 0xff : 0x00)) \
             ; \
        m_mreq_wr(addr, R0_L); \
    } while(0)

/*
 * rrc r08
 */

#define m_rrc_r08(data) \
    do { \
        R1_L = data; \
        R0_L = (UBYTE(R1_L) >> 1) | (UBYTE(R1_L) << 7); \
        R3_L = PZS[R0_L]; \
        AF_L = /* SF is affected     */ (SF & (R3_L)) \
             | /* ZF is affected     */ (ZF & (R3_L)) \
             | /* YF is undocumented */ (YF & (R3_L)) \
             | /* HF is reset        */ (HF & (0x00)) \
             | /* XF is undocumented */ (XF & (R3_L)) \
             | /* PF is affected     */ (PF & (R3_L)) \
             | /* NF is reset        */ (NF & (0x00)) \
             | /* CF is data bit0    */ (CF & (R1_L & BIT0 ? 0xff : 0x00)) \
             ; \
        data = R0_L; \
    } while(0)

/*
 * rrc (r16)
 */

#define m_rrc_ind_r16(addr) \
    do { \
        m_mreq_rd(addr, R1_L); \
        R0_L = (UBYTE(R1_L) >> 1) | (UBYTE(R1_L) << 7); \
        R3_L = PZS[R0_L]; \
        AF_L = /* SF is affected     */ (SF & (R3_L)) \
             | /* ZF is affected     */ (ZF & (R3_L)) \
             | /* YF is undocumented */ (YF & (R3_L)) \
             | /* HF is reset        */ (HF & (0x00)) \
             | /* XF is undocumented */ (XF & (R3_L)) \
             | /* PF is affected     */ (PF & (R3_L)) \
             | /* NF is reset        */ (NF & (0x00)) \
             | /* CF is data bit0    */ (CF & (R1_L & BIT0 ? 0xff : 0x00)) \
             ; \
        m_mreq_wr(addr, R0_L); \
    } while(0)

/*
 * rl r08
 */

#define m_rl_r08(data) \
    do { \
        R1_L = data; \
        R0_L = (UBYTE(R1_L) << 1) | (AF_L & CF ? BIT0 : 0x00); \
        R3_L = PZS[R0_L]; \
        AF_L = /* SF is affected     */ (SF & (R3_L)) \
             | /* ZF is affected     */ (ZF & (R3_L)) \
             | /* YF is undocumented */ (YF & (R3_L)) \
             | /* HF is reset        */ (HF & (0x00)) \
             | /* XF is undocumented */ (XF & (R3_L)) \
             | /* PF is affected     */ (PF & (R3_L)) \
             | /* NF is reset        */ (NF & (0x00)) \
             | /* CF is data bit7    */ (CF & (R1_L & BIT7 ? 0xff : 0x00)) \
             ; \
        data = R0_L; \
    } while(0)

/*
 * rl (r16)
 */

#define m_rl_ind_r16(addr) \
    do { \
        m_mreq_rd(addr, R1_L); \
        R0_L = (UBYTE(R1_L) << 1) | (AF_L & CF ? BIT0 : 0x00); \
        R3_L = PZS[R0_L]; \
        AF_L = /* SF is affected     */ (SF & (R3_L)) \
             | /* ZF is affected     */ (ZF & (R3_L)) \
             | /* YF is undocumented */ (YF & (R3_L)) \
             | /* HF is reset        */ (HF & (0x00)) \
             | /* XF is undocumented */ (XF & (R3_L)) \
             | /* PF is affected     */ (PF & (R3_L)) \
             | /* NF is reset        */ (NF & (0x00)) \
             | /* CF is data bit7    */ (CF & (R1_L & BIT7 ? 0xff : 0x00)) \
             ; \
        m_mreq_wr(addr, R0_L); \
    } while(0)

/*
 * rr r08
 */

#define m_rr_r08(data) \
    do { \
        R1_L = data; \
        R0_L = (UBYTE(R1_L) >> 1) | (AF_L & CF ? BIT7 : 0x00); \
        R3_L = PZS[R0_L]; \
        AF_L = /* SF is affected     */ (SF & (R3_L)) \
             | /* ZF is affected     */ (ZF & (R3_L)) \
             | /* YF is undocumented */ (YF & (R3_L)) \
             | /* HF is reset        */ (HF & (0x00)) \
             | /* XF is undocumented */ (XF & (R3_L)) \
             | /* PF is affected     */ (PF & (R3_L)) \
             | /* NF is reset        */ (NF & (0x00)) \
             | /* CF is data bit0    */ (CF & (R1_L & BIT0 ? 0xff : 0x00)) \
             ; \
        data = R0_L; \
    } while(0)

/*
 * rr (r16)
 */

#define m_rr_ind_r16(addr) \
    do { \
        m_mreq_rd(addr, R1_L); \
        R0_L = (UBYTE(R1_L) >> 1) | (AF_L & CF ? BIT7 : 0x00); \
        R3_L = PZS[R0_L]; \
        AF_L = /* SF is affected     */ (SF & (R3_L)) \
             | /* ZF is affected     */ (ZF & (R3_L)) \
             | /* YF is undocumented */ (YF & (R3_L)) \
             | /* HF is reset        */ (HF & (0x00)) \
             | /* XF is undocumented */ (XF & (R3_L)) \
             | /* PF is affected     */ (PF & (R3_L)) \
             | /* NF is reset        */ (NF & (0x00)) \
             | /* CF is data bit0    */ (CF & (R1_L & BIT0 ? 0xff : 0x00)) \
             ; \
        m_mreq_wr(addr, R0_L); \
    } while(0)

/*
 * sla r08
 */

#define m_sla_r08(data) \
    do { \
        R1_L = data; \
        R0_L = (SBYTE(R1_L) << 1); \
        R3_L = PZS[R0_L]; \
        AF_L = /* SF is affected     */ (SF & (R3_L)) \
             | /* ZF is affected     */ (ZF & (R3_L)) \
             | /* YF is undocumented */ (YF & (R3_L)) \
             | /* HF is reset        */ (HF & (0x00)) \
             | /* XF is undocumented */ (XF & (R3_L)) \
             | /* PF is affected     */ (PF & (R3_L)) \
             | /* NF is reset        */ (NF & (0x00)) \
             | /* CF is data bit7    */ (CF & (R1_L & BIT7 ? 0xff : 0x00)) \
             ; \
        data = R0_L; \
    } while(0)

/*
 * sla (r16)
 */

#define m_sla_ind_r16(addr) \
    do { \
        m_mreq_rd(addr, R1_L); \
        R0_L = (SBYTE(R1_L) << 1); \
        R3_L = PZS[R0_L]; \
        AF_L = /* SF is affected     */ (SF & (R3_L)) \
             | /* ZF is affected     */ (ZF & (R3_L)) \
             | /* YF is undocumented */ (YF & (R3_L)) \
             | /* HF is reset        */ (HF & (0x00)) \
             | /* XF is undocumented */ (XF & (R3_L)) \
             | /* PF is affected     */ (PF & (R3_L)) \
             | /* NF is reset        */ (NF & (0x00)) \
             | /* CF is data bit7    */ (CF & (R1_L & BIT7 ? 0xff : 0x00)) \
             ; \
        m_mreq_wr(addr, R0_L); \
    } while(0)

/*
 * sra r08
 */

#define m_sra_r08(data) \
    do { \
        R1_L = data; \
        R0_L = (SBYTE(R1_L) >> 1); \
        R3_L = PZS[R0_L]; \
        AF_L = /* SF is affected     */ (SF & (R3_L)) \
             | /* ZF is affected     */ (ZF & (R3_L)) \
             | /* YF is undocumented */ (YF & (R3_L)) \
             | /* HF is reset        */ (HF & (0x00)) \
             | /* XF is undocumented */ (XF & (R3_L)) \
             | /* PF is affected     */ (PF & (R3_L)) \
             | /* NF is reset        */ (NF & (0x00)) \
             | /* CF is data bit0    */ (CF & (R1_L & BIT0 ? 0xff : 0x00)) \
             ; \
        data = R0_L; \
    } while(0)

/*
 * sra r16
 */

#define m_sra_ind_r16(addr) \
    do { \
        m_mreq_rd(addr, R1_L); \
        R0_L = (SBYTE(R1_L) >> 1); \
        R3_L = PZS[R0_L]; \
        AF_L = /* SF is affected     */ (SF & (R3_L)) \
             | /* ZF is affected     */ (ZF & (R3_L)) \
             | /* YF is undocumented */ (YF & (R3_L)) \
             | /* HF is reset        */ (HF & (0x00)) \
             | /* XF is undocumented */ (XF & (R3_L)) \
             | /* PF is affected     */ (PF & (R3_L)) \
             | /* NF is reset        */ (NF & (0x00)) \
             | /* CF is data bit0    */ (CF & (R1_L & BIT0 ? 0xff : 0x00)) \
             ; \
        m_mreq_wr(addr, R0_L); \
    } while(0)

/*
 * sll r08
 */

#define m_sll_r08(data) \
    do { \
        R1_L = data; \
        R0_L = (UBYTE(R1_L) << 1) | (BIT0); \
        R3_L = PZS[R0_L]; \
        AF_L = /* SF is affected     */ (SF & (R3_L)) \
             | /* ZF is affected     */ (ZF & (R3_L)) \
             | /* YF is undocumented */ (YF & (R3_L)) \
             | /* HF is reset        */ (HF & (0x00)) \
             | /* XF is undocumented */ (XF & (R3_L)) \
             | /* PF is affected     */ (PF & (R3_L)) \
             | /* NF is reset        */ (NF & (0x00)) \
             | /* CF is data bit7    */ (CF & (R1_L & BIT7 ? 0xff : 0x00)) \
             ; \
        data = R0_L; \
    } while(0)

/*
 * sll (r16)
 */

#define m_sll_ind_r16(addr) \
    do { \
        m_mreq_rd(addr, R1_L); \
        R0_L = (UBYTE(R1_L) << 1) | (BIT0); \
        R3_L = PZS[R0_L]; \
        AF_L = /* SF is affected     */ (SF & (R3_L)) \
             | /* ZF is affected     */ (ZF & (R3_L)) \
             | /* YF is undocumented */ (YF & (R3_L)) \
             | /* HF is reset        */ (HF & (0x00)) \
             | /* XF is undocumented */ (XF & (R3_L)) \
             | /* PF is affected     */ (PF & (R3_L)) \
             | /* NF is reset        */ (NF & (0x00)) \
             | /* CF is data bit7    */ (CF & (R1_L & BIT7 ? 0xff : 0x00)) \
             ; \
        m_mreq_wr(addr, R0_L); \
    } while(0)

/*
 * srl r08
 */

#define m_srl_r08(data) \
    do { \
        R1_L = data; \
        R0_L = (UBYTE(R1_L) >> 1) & (~BIT7); \
        R3_L = PZS[R0_L]; \
        AF_L = /* SF is affected     */ (SF & (R3_L)) \
             | /* ZF is affected     */ (ZF & (R3_L)) \
             | /* YF is undocumented */ (YF & (R3_L)) \
             | /* HF is reset        */ (HF & (0x00)) \
             | /* XF is undocumented */ (XF & (R3_L)) \
             | /* PF is affected     */ (PF & (R3_L)) \
             | /* NF is reset        */ (NF & (0x00)) \
             | /* CF is data bit0    */ (CF & (R1_L & BIT0 ? 0xff : 0x00)) \
             ; \
        data = R0_L; \
    } while(0)

/*
 * srl (r16)
 */

#define m_srl_ind_r16(addr) \
    do { \
        m_mreq_rd(addr, R1_L); \
        R0_L = (UBYTE(R1_L) >> 1) & (~BIT7); \
        R3_L = PZS[R0_L]; \
        AF_L = /* SF is affected     */ (SF & (R3_L)) \
             | /* ZF is affected     */ (ZF & (R3_L)) \
             | /* YF is undocumented */ (YF & (R3_L)) \
             | /* HF is reset        */ (HF & (0x00)) \
             | /* XF is undocumented */ (XF & (R3_L)) \
             | /* PF is affected     */ (PF & (R3_L)) \
             | /* NF is reset        */ (NF & (0x00)) \
             | /* CF is data bit0    */ (CF & (R1_L & BIT0 ? 0xff : 0x00)) \
             ; \
        m_mreq_wr(addr, R0_L); \
    } while(0)

// ---------------------------------------------------------------------------
// bit test/reset/set group
// ---------------------------------------------------------------------------

/*
 * bit r08
 */

#define m_bit_b_r08(mask, data) \
    do { \
        R1_L = data; \
        R0_L = (R1_L & mask); \
        R3_L = PZS[R0_L]; \
        AF_L = /* SF is undocumented */ (SF & (R0_L)) \
             | /* ZF is affected     */ (ZF & (R3_L)) \
             | /* YF is undocumented */ (YF & (R1_L)) \
             | /* HF is set          */ (HF & (0xff)) \
             | /* XF is undocumented */ (XF & (R1_L)) \
             | /* PF is undocumented */ (PF & (R0_L == 0x00 ? 0xff : 0x00)) \
             | /* NF is reset        */ (NF & (0x00)) \
             | /* CF is not affected */ (CF & (AF_L)) \
             ; \
    } while(0)

/*
 * bit (r16)
 */

#define m_bit_b_ind_r16(mask, addr) \
    do { \
        m_load_rg(WZ_W, addr); \
        m_mreq_rd(WZ_W, R1_L); \
        R0_L = (R1_L & mask); \
        R3_L = PZS[R0_L]; \
        AF_L = /* SF is undocumented */ (SF & (R0_L)) \
             | /* ZF is affected     */ (ZF & (R3_L)) \
             | /* YF is undocumented */ (YF & (WZ_H)) \
             | /* HF is set          */ (HF & (0xff)) \
             | /* XF is undocumented */ (XF & (WZ_H)) \
             | /* PF is undocumented */ (PF & (R0_L == 0x00 ? 0xff : 0x00)) \
             | /* NF is reset        */ (NF & (0x00)) \
             | /* CF is not affected */ (CF & (AF_L)) \
             ; \
    } while(0)

/*
 * res r08
 */

#define m_res_b_r08(mask, data) \
    do { \
        R1_L = data; \
        R0_L = (R1_L & ~mask); \
        data = R0_L; \
    } while(0)

/*
 * res (r16)
 */

#define m_res_b_ind_r16(mask, addr) \
    do { \
        m_mreq_rd(addr, R1_L); \
        R0_L = (R1_L & ~mask); \
        m_mreq_wr(addr, R0_L); \
    } while(0)

/*
 * set r08
 */

#define m_set_b_r08(mask, data) \
    do { \
        R1_L = data; \
        R0_L = (R1_L | mask); \
        data = R0_L; \
    } while(0)

/*
 * set (r16)
 */

#define m_set_b_ind_r16(mask, addr) \
    do { \
        m_mreq_rd(addr, R1_L); \
        R0_L = (R1_L | mask); \
        m_mreq_wr(addr, R0_L); \
    } while(0)

// ---------------------------------------------------------------------------
// ddcb/fdcb instructions : indexed rotate/shift
// ---------------------------------------------------------------------------

/*
 * rlc (r16+i08)
 */

#define m_rlc_ind_r16_plus_i08(reg1) \
    do { \
        m_rlc_ind_r16(WZ_W); \
    } while(0)

/*
 * rlc (r16+i08),r08
 */

#define m_rlc_ind_r16_plus_i08_r08(reg1, reg2) \
    do { \
        m_rlc_ind_r16(WZ_W); \
        m_ld_r08_r08(reg2, R0_L); \
    } while(0)

/*
 * rrc (r16+i08)
 */

#define m_rrc_ind_r16_plus_i08(reg1) \
    do { \
        m_rrc_ind_r16(WZ_W); \
    } while(0)

/*
 * rrc (r16+i08),r08
 */

#define m_rrc_ind_r16_plus_i08_r08(reg1, reg2) \
    do { \
        m_rrc_ind_r16(WZ_W); \
        m_ld_r08_r08(reg2, R0_L); \
    } while(0)

/*
 * rl (r16+i08)
 */

#define m_rl_ind_r16_plus_i08(reg1) \
    do { \
        m_rl_ind_r16(WZ_W); \
    } while(0)

/*
 * rl (r16+i08),r08
 */

#define m_rl_ind_r16_plus_i08_r08(reg1, reg2) \
    do { \
        m_rl_ind_r16(WZ_W); \
        m_ld_r08_r08(reg2, R0_L); \
    } while(0)

/*
 * rr (r16+i08)
 */

#define m_rr_ind_r16_plus_i08(reg1) \
    do { \
        m_rr_ind_r16(WZ_W); \
    } while(0)

/*
 * rr (r16+i08),r08
 */

#define m_rr_ind_r16_plus_i08_r08(reg1, reg2) \
    do { \
        m_rr_ind_r16(WZ_W); \
        m_ld_r08_r08(reg2, R0_L); \
    } while(0)

/*
 * sla (r16+i08)
 */

#define m_sla_ind_r16_plus_i08(reg1) \
    do { \
        m_sla_ind_r16(WZ_W); \
    } while(0)

/*
 * sla (r16+i08),r08
 */

#define m_sla_ind_r16_plus_i08_r08(reg1, reg2) \
    do { \
        m_sla_ind_r16(WZ_W); \
        m_ld_r08_r08(reg2, R0_L); \
    } while(0)

/*
 * sra (r16+i08)
 */

#define m_sra_ind_r16_plus_i08(reg1) \
    do { \
        m_sra_ind_r16(WZ_W); \
    } while(0)

/*
 * sra (r16+i08),r08
 */

#define m_sra_ind_r16_plus_i08_r08(reg1, reg2) \
    do { \
        m_sra_ind_r16(WZ_W); \
        m_ld_r08_r08(reg2, R0_L); \
    } while(0)

/*
 * sll (r16+i08)
 */

#define m_sll_ind_r16_plus_i08(reg1) \
    do { \
        m_sll_ind_r16(WZ_W); \
    } while(0)

/*
 * sll (r16+i08),r08
 */

#define m_sll_ind_r16_plus_i08_r08(reg1, reg2) \
    do { \
        m_sll_ind_r16(WZ_W); \
        m_ld_r08_r08(reg2, R0_L); \
    } while(0)

/*
 * srl (r16+i08)
 */

#define m_srl_ind_r16_plus_i08(reg1) \
    do { \
        m_srl_ind_r16(WZ_W); \
    } while(0)

/*
 * srl (r16+i08),r08
 */

#define m_srl_ind_r16_plus_i08_r08(reg1, reg2) \
    do { \
        m_srl_ind_r16(WZ_W); \
        m_ld_r08_r08(reg2, R0_L); \
    } while(0)

// ---------------------------------------------------------------------------
// ddcb/fdcb instructions : indexed bit test/reset/set
// ---------------------------------------------------------------------------

/*
 * bit b,(r16+i08)
 */

#define m_bit_b_ind_r16_plus_i08(mask, reg1) \
    do { \
        m_bit_b_ind_r16(mask, WZ_W); \
    } while(0)

/*
 * res b,(r16+i08)
 */

#define m_res_b_ind_r16_plus_i08(mask, reg1) \
    do { \
        m_res_b_ind_r16(mask, WZ_W); \
    } while(0)

/*
 * res b,(r16+i08),r08
 */

#define m_res_b_ind_r16_plus_i08_r08(mask, reg1, reg2) \
    do { \
        m_res_b_ind_r16(mask, WZ_W); \
        m_ld_r08_r08(reg2, R0_L); \
    } while(0)

/*
 * set b,(r16+i08)
 */

#define m_set_b_ind_r16_plus_i08(mask, reg1) \
    do { \
        m_set_b_ind_r16(mask, WZ_W); \
    } while(0)

/*
 * set b,(r16+i08),r08
 */

#define m_set_b_ind_r16_plus_i08_r08(mask, reg1, reg2) \
    do { \
        m_set_b_ind_r16(mask, WZ_W); \
        m_ld_r08_r08(reg2, R0_L); \
    } while(0)

// ---------------------------------------------------------------------------
// special instructions
// ---------------------------------------------------------------------------

auto m_interrupt_mode = [&]() -> uint8_t
{
    return (HAS_IM1 ? 1 : 0)
         | (HAS_IM2 ? 2 : 0)
         ;
};

auto m_after_ei = [&]() -> bool
{
    if((ST_L & ST_AEI) != 0) {
        ST_L &= ~(ST_AEI);
        return true;
    }
    return false;
};

auto m_take_nmi = [&]() -> bool
{
    if((ST_L & ST_NMI) != 0) {
        ST_L &= ~(ST_HLT | ST_NMI | ST_IFF1);
        return true;
    }
    return false;
};

auto m_take_int = [&]() -> bool
{
    if((ST_L & ST_IFF1) == 0) {
        return false;
    }
    if((ST_L & ST_INT) != 0) {
        ST_L &= ~(ST_HLT | ST_INT | ST_IFF2 | ST_IFF1);
        return true;
    }
    return false;
};

auto m_halted = [&]() -> bool
{
    if((ST_L & ST_HLT) != 0) {
        return true;
    }
    return false;
};

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
