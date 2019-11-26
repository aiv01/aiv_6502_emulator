#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct mos6502
{
    uint8_t a;
    uint8_t x;
    uint8_t y;

    uint16_t pc;
    uint8_t sp;
    uint8_t flags;

    int interrupt;
    int nmi;
    int rst;
    int rdy;

    uint8_t (*read)(struct mos6502 *cpu, uint16_t address);
    void (*write)(struct mos6502 *cpu, uint16_t address, uint8_t value);

    int (*opcodes[256])(struct mos6502 *cpu);
} mos6502_t;

#define CARRY 1
#define ZERO 1 << 1
#define INTERRUPT 1 << 2
#define DECIMAL 1 << 3
#define OVERFLOW 1 << 6
#define NEGATIVE 1 << 7

void mos6502_set_flag(mos6502_t *cpu, int flag, int value);
int mos6502_get_flag(mos6502_t *cpu, int flag);

uint8_t mos6502_read8(mos6502_t *cpu, uint16_t address);
void mos6502_write8(mos6502_t *cpu, uint16_t address, uint8_t value);

uint16_t mos6502_read16(mos6502_t *cpu, uint16_t address);
void mos6502_write16(mos6502_t *cpu, uint16_t address, uint16_t value);

int mos6502_init(mos6502_t *cpu);

int mos6502_tick(mos6502_t *cpu);

void mos6502_register_opcode(mos6502_t *cpu, uint8_t opcode, int (*func)(mos6502_t *cpu));

void mos6502_register_lda(mos6502_t *cpu);
void mos6502_register_adc(mos6502_t *cpu);
void mos6502_register_and(mos6502_t *cpu);
void mos6502_register_asl(mos6502_t *cpu);
void mos6502_register_bcc(mos6502_t *cpu);
void mos6502_register_bcs(mos6502_t *cpu);
void mos6502_register_beq(mos6502_t *cpu);
void mos6502_register_bit(mos6502_t *cpu);
void mos6502_register_bmi(mos6502_t *cpu);
void mos6502_register_bne(mos6502_t *cpu);
void mos6502_register_bpl(mos6502_t *cpu);
void mos6502_register_brk(mos6502_t *cpu);
void mos6502_register_bvc(mos6502_t *cpu);
void mos6502_register_bvs(mos6502_t *cpu);
void mos6502_register_clc(mos6502_t *cpu);
void mos6502_register_cld(mos6502_t *cpu);
void mos6502_register_cli(mos6502_t *cpu);
void mos6502_register_clv(mos6502_t *cpu);
void mos6502_register_cmp(mos6502_t *cpu);
void mos6502_register_cpx(mos6502_t *cpu);
void mos6502_register_cpy(mos6502_t *cpu);
void mos6502_register_dec(mos6502_t *cpu);
void mos6502_register_dex(mos6502_t *cpu);
void mos6502_register_dey(mos6502_t *cpu);
void mos6502_register_eor(mos6502_t *cpu);
void mos6502_register_inc(mos6502_t *cpu);
void mos6502_register_inx(mos6502_t *cpu);
void mos6502_register_iny(mos6502_t *cpu);
void mos6502_register_jmp(mos6502_t *cpu);
void mos6502_register_jsr(mos6502_t *cpu);
void mos6502_register_lda(mos6502_t *cpu);
void mos6502_register_ldx(mos6502_t *cpu);
void mos6502_register_ldy(mos6502_t *cpu);
void mos6502_register_lsr(mos6502_t *cpu);
void mos6502_register_nop(mos6502_t *cpu);
void mos6502_register_ora(mos6502_t *cpu);
void mos6502_register_pha(mos6502_t *cpu);
void mos6502_register_php(mos6502_t *cpu);
void mos6502_register_pla(mos6502_t *cpu);
void mos6502_register_plp(mos6502_t *cpu);
void mos6502_register_rol(mos6502_t *cpu);
void mos6502_register_ror(mos6502_t *cpu);
void mos6502_register_rti(mos6502_t *cpu);
void mos6502_register_rts(mos6502_t *cpu);
void mos6502_register_sbc(mos6502_t *cpu);
void mos6502_register_sec(mos6502_t *cpu);
void mos6502_register_sed(mos6502_t *cpu);
void mos6502_register_sei(mos6502_t *cpu);
void mos6502_register_sta(mos6502_t *cpu);
void mos6502_register_stx(mos6502_t *cpu);
void mos6502_register_sty(mos6502_t *cpu);
void mos6502_register_tax(mos6502_t *cpu);
void mos6502_register_tay(mos6502_t *cpu);
void mos6502_register_tsx(mos6502_t *cpu);
void mos6502_register_txa(mos6502_t *cpu);
void mos6502_register_txs(mos6502_t *cpu);
void mos6502_register_tya(mos6502_t *cpu);

#ifdef _TEST
void mos6502_test_wrapper(const char *name, int (*func)(mos6502_t *cpu));
#define RUN_TEST(func) mos6502_test_wrapper(#func, func);

void test_mos6502_core();
void test_mos6502_lda();
void test_mos6502_adc();
void test_mos6502_and();
void test_mos6502_asl();
void test_mos6502_bcc();
void test_mos6502_bcs();
void test_mos6502_beq();
void test_mos6502_bit();
void test_mos6502_bmi();
void test_mos6502_bne();
void test_mos6502_bpl();
void test_mos6502_brk();
void test_mos6502_bvc();
void test_mos6502_bvs();
void test_mos6502_clc();
void test_mos6502_cld();
void test_mos6502_cli();
void test_mos6502_clv();
void test_mos6502_cmp();
void test_mos6502_cpx();
void test_mos6502_cpy();
void test_mos6502_dec();
void test_mos6502_dex();
void test_mos6502_dey();
void test_mos6502_eor();
void test_mos6502_inc();
void test_mos6502_inx();
void test_mos6502_iny();
void test_mos6502_jmp();
void test_mos6502_jsr();
void test_mos6502_lda();
void test_mos6502_ldx();
void test_mos6502_ldy();
void test_mos6502_lsr();
void test_mos6502_nop();
void test_mos6502_ora();
void test_mos6502_pha();
void test_mos6502_php();
void test_mos6502_pla();
void test_mos6502_plp();
void test_mos6502_rol();
void test_mos6502_ror();
void test_mos6502_rti();
void test_mos6502_rts();
void test_mos6502_sbc();
void test_mos6502_sec();
void test_mos6502_sed();
void test_mos6502_sei();
void test_mos6502_sta();
void test_mos6502_stx();
void test_mos6502_sty();
void test_mos6502_tax();
void test_mos6502_tay();
void test_mos6502_tsx();
void test_mos6502_txa();
void test_mos6502_txs();
void test_mos6502_tya();
#endif