#include "mos6502.h"

static int16_t get_page(mos6502_t *cpu)
{
    int16_t page = cpu->pc;
    page = page >> 8;
    page = page & 0xFF;

    return page;
}

static int get_ticks_no_branch(mos6502_t *cpu)
{
    cpu->pc++;

    return 2;
}

static int get_ticks_branch(mos6502_t *cpu)
{
    int16_t initial_page = get_page(cpu);

    int8_t distance = cpu->read(cpu, cpu->pc);
    cpu->pc += distance;

    int16_t final_page = get_page(cpu);

    int ticks = -1;

    if (initial_page == final_page)
        ticks = 3;
    else
        ticks = 4;

    return ticks;
}

static int get_ticks_branch_flag_set(mos6502_t *cpu, int flag)
{
    int ticks = -1;

    if (!flag)
        ticks = get_ticks_no_branch(cpu);
    else
        ticks = get_ticks_branch(cpu);

    return ticks;
}

static int get_ticks_branch_flag_clear(mos6502_t *cpu, int flag)
{
    int ticks = -1;

    if (flag)
        ticks = get_ticks_no_branch(cpu);
    else
        ticks = get_ticks_branch(cpu);

    return ticks;
}

static int bpl(mos6502_t *cpu)
{
    return get_ticks_branch_flag_set(cpu, mos6502_get_flag(cpu, NEGATIVE));
}
void mos6502_register_bpl(mos6502_t *cpu)
{
    mos6502_register_opcode(cpu, 0x10, bpl);
}

static int bmi(mos6502_t *cpu)
{
    return get_ticks_branch_flag_clear(cpu, mos6502_get_flag(cpu, NEGATIVE));
}
void mos6502_register_bmi(mos6502_t *cpu)
{
    mos6502_register_opcode(cpu, 0x30, bmi);
}

static int bvc(mos6502_t *cpu)
{
    return get_ticks_branch_flag_clear(cpu, mos6502_get_flag(cpu, OVERFLOW));
}
void mos6502_register_bvc(mos6502_t *cpu)
{
    mos6502_register_opcode(cpu, 0x50, bvc);
}

static int bvs(mos6502_t *cpu)
{
    return get_ticks_branch_flag_set(cpu, mos6502_get_flag(cpu, OVERFLOW));
}
void mos6502_register_bvs(mos6502_t *cpu)
{
    mos6502_register_opcode(cpu, 0x70, bvs);
}

static int bcc(mos6502_t *cpu)
{
    return get_ticks_branch_flag_clear(cpu, mos6502_get_flag(cpu, CARRY));
}
void mos6502_register_bcc(mos6502_t *cpu)
{
    mos6502_register_opcode(cpu, 0x90, bcc);
}

static int bcs(mos6502_t *cpu)
{
    return get_ticks_branch_flag_set(cpu, mos6502_get_flag(cpu, CARRY));
}
void mos6502_register_bcs(mos6502_t *cpu)
{
    mos6502_register_opcode(cpu, 0xB0, bcs);
}

static int bne(mos6502_t *cpu)
{
    return get_ticks_branch_flag_clear(cpu, mos6502_get_flag(cpu, ZERO));
}
void mos6502_register_bne(mos6502_t *cpu)
{
    mos6502_register_opcode(cpu, 0xD0, bne);
}

static int beq(mos6502_t *cpu)
{
    return get_ticks_branch_flag_set(cpu, mos6502_get_flag(cpu, ZERO));
}
void mos6502_register_beq(mos6502_t *cpu)
{
    mos6502_register_opcode(cpu, 0xF0, beq);
}

#ifdef _TEST
static int test_bpl_no_branch(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0x8000, 0x10);
    mos6502_write8(cpu, 0x8001, 0x5);
    int ticks = mos6502_tick(cpu);
    return ticks == 2 && cpu->pc == 0x8002 && cpu->flags == 0;
}

static int test_bpl_branch(mos6502_t *cpu)
{
    mos6502_set_flag(cpu, NEGATIVE, 0x1);
    mos6502_write8(cpu, 0x8000, 0x10);
    mos6502_write8(cpu, 0x8001, 0x5);
    int ticks = mos6502_tick(cpu);
    return ticks == 3 && cpu->pc == 0x8006 && cpu->flags == NEGATIVE;
}

static int test_bpl_page_boundary(mos6502_t *cpu)
{
    mos6502_set_flag(cpu, NEGATIVE, 0x1);
    mos6502_write8(cpu, 0x8000, 0x10);
    mos6502_write8(cpu, 0x8001, -5);
    int ticks = mos6502_tick(cpu);
    return ticks == 4 && cpu->pc == 0x7FFC && cpu->flags == NEGATIVE;
}

void test_mos6502_bpl()
{
    RUN_TEST(test_bpl_no_branch);
    RUN_TEST(test_bpl_branch);
    RUN_TEST(test_bpl_page_boundary);
}

static int test_bmi_no_branch(mos6502_t *cpu)
{
    mos6502_set_flag(cpu, NEGATIVE, 0x1);
    mos6502_write8(cpu, 0x8000, 0x30);
    mos6502_write8(cpu, 0x8001, 0x5);
    int ticks = mos6502_tick(cpu);
    return ticks == 2 && cpu->pc == 0x8002 && cpu->flags == NEGATIVE;
}

static int test_bmi_branch(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0x8000, 0x30);
    mos6502_write8(cpu, 0x8001, 0x5);
    int ticks = mos6502_tick(cpu);
    return ticks == 3 && cpu->pc == 0x8006 && cpu->flags == 0;
}

static int test_bmi_page_boundary(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0x8000, 0x30);
    mos6502_write8(cpu, 0x8001, -5);
    int ticks = mos6502_tick(cpu);
    return ticks == 4 && cpu->pc == 0x7FFC && cpu->flags == 0;
}

void test_mos6502_bmi()
{
    RUN_TEST(test_bmi_no_branch);
    RUN_TEST(test_bmi_branch);
    RUN_TEST(test_bmi_page_boundary);
}

static int test_bvc_no_branch(mos6502_t *cpu)
{
    mos6502_set_flag(cpu, OVERFLOW, 0x1);
    mos6502_write8(cpu, 0x8000, 0x50);
    mos6502_write8(cpu, 0x8001, 0x5);
    int ticks = mos6502_tick(cpu);
    return ticks == 2 && cpu->pc == 0x8002 && cpu->flags == OVERFLOW;
}

static int test_bvc_branch(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0x8000, 0x50);
    mos6502_write8(cpu, 0x8001, 0x5);
    int ticks = mos6502_tick(cpu);
    return ticks == 3 && cpu->pc == 0x8006 && cpu->flags == 0;
}

static int test_bvc_page_boundary(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0x8000, 0x50);
    mos6502_write8(cpu, 0x8001, -5);
    int ticks = mos6502_tick(cpu);
    return ticks == 4 && cpu->pc == 0x7FFC && cpu->flags == 0;
}

void test_mos6502_bvc()
{
    RUN_TEST(test_bvc_no_branch);
    RUN_TEST(test_bvc_branch);
    RUN_TEST(test_bvc_page_boundary);
}

static int test_bvs_no_branch(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0x8000, 0x70);
    mos6502_write8(cpu, 0x8001, 0x5);
    int ticks = mos6502_tick(cpu);
    return ticks == 2 && cpu->pc == 0x8002 && cpu->flags == 0;
}

static int test_bvs_branch(mos6502_t *cpu)
{
    mos6502_set_flag(cpu, OVERFLOW, 0x1);
    mos6502_write8(cpu, 0x8000, 0x70);
    mos6502_write8(cpu, 0x8001, 0x5);
    int ticks = mos6502_tick(cpu);
    return ticks == 3 && cpu->pc == 0x8006 && cpu->flags == OVERFLOW;
}

static int test_bvs_page_boundary(mos6502_t *cpu)
{
    mos6502_set_flag(cpu, OVERFLOW, 0x1);
    mos6502_write8(cpu, 0x8000, 0x70);
    mos6502_write8(cpu, 0x8001, -5);
    int ticks = mos6502_tick(cpu);
    return ticks == 4 && cpu->pc == 0x7FFC && cpu->flags == OVERFLOW;
}

void test_mos6502_bvs()
{
    RUN_TEST(test_bvs_no_branch);
    RUN_TEST(test_bvs_branch);
    RUN_TEST(test_bvs_page_boundary);
}

static int test_bcc_no_branch(mos6502_t *cpu)
{
    mos6502_set_flag(cpu, CARRY, 0x1);
    mos6502_write8(cpu, 0x8000, 0x90);
    mos6502_write8(cpu, 0x8001, 0x5);
    int ticks = mos6502_tick(cpu);
    return ticks == 2 && cpu->pc == 0x8002 && cpu->flags == CARRY;
}

static int test_bcc_branch(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0x8000, 0x90);
    mos6502_write8(cpu, 0x8001, 0x5);
    int ticks = mos6502_tick(cpu);
    return ticks == 3 && cpu->pc == 0x8006 && cpu->flags == 0;
}

static int test_bcc_page_boundary(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0x8000, 0x90);
    mos6502_write8(cpu, 0x8001, -5);
    int ticks = mos6502_tick(cpu);
    return ticks == 4 && cpu->pc == 0x7FFC && cpu->flags == 0;
}

void test_mos6502_bcc()
{
    RUN_TEST(test_bcc_no_branch);
    RUN_TEST(test_bcc_branch);
    RUN_TEST(test_bcc_page_boundary);
}

static int test_bcs_no_branch(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0x8000, 0xB0);
    mos6502_write8(cpu, 0x8001, 0x5);
    int ticks = mos6502_tick(cpu);
    return ticks == 2 && cpu->pc == 0x8002 && cpu->flags == 0;
}

static int test_bcs_branch(mos6502_t *cpu)
{
    mos6502_set_flag(cpu, CARRY, 0x1);
    mos6502_write8(cpu, 0x8000, 0xB0);
    mos6502_write8(cpu, 0x8001, 0x5);
    int ticks = mos6502_tick(cpu);
    return ticks == 3 && cpu->pc == 0x8006 && cpu->flags == CARRY;
}

static int test_bcs_page_boundary(mos6502_t *cpu)
{
    mos6502_set_flag(cpu, CARRY, 0x1);
    mos6502_write8(cpu, 0x8000, 0xB0);
    mos6502_write8(cpu, 0x8001, -5);
    int ticks = mos6502_tick(cpu);
    return ticks == 4 && cpu->pc == 0x7FFC && cpu->flags == CARRY;
}

void test_mos6502_bcs()
{
    RUN_TEST(test_bcs_no_branch);
    RUN_TEST(test_bcs_branch);
    RUN_TEST(test_bcs_page_boundary);
}

static int test_bne_no_branch(mos6502_t *cpu)
{
    mos6502_set_flag(cpu, ZERO, 0x1);
    mos6502_write8(cpu, 0x8000, 0xD0);
    mos6502_write8(cpu, 0x8001, 0x5);
    int ticks = mos6502_tick(cpu);
    return ticks == 2 && cpu->pc == 0x8002 && cpu->flags == ZERO;
}

static int test_bne_branch(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0x8000, 0xD0);
    mos6502_write8(cpu, 0x8001, 0x5);
    int ticks = mos6502_tick(cpu);
    return ticks == 3 && cpu->pc == 0x8006 && cpu->flags == 0;
}

static int test_bne_page_boundary(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0x8000, 0xD0);
    mos6502_write8(cpu, 0x8001, -5);
    int ticks = mos6502_tick(cpu);
    return ticks == 4 && cpu->pc == 0x7FFC && cpu->flags == 0;
}

void test_mos6502_bne()
{
    RUN_TEST(test_bne_no_branch);
    RUN_TEST(test_bne_branch);
    RUN_TEST(test_bne_page_boundary);
}

static int test_beq_no_branch(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0x8000, 0xF0);
    mos6502_write8(cpu, 0x8001, 0x5);
    int ticks = mos6502_tick(cpu);
    return ticks == 2 && cpu->pc == 0x8002 && cpu->flags == 0;
}

static int test_beq_branch(mos6502_t *cpu)
{
    mos6502_set_flag(cpu, ZERO, 0x1);
    mos6502_write8(cpu, 0x8000, 0xF0);
    mos6502_write8(cpu, 0x8001, 0x5);
    int ticks = mos6502_tick(cpu);
    return ticks == 3 && cpu->pc == 0x8006 && cpu->flags == ZERO;
}

static int test_beq_page_boundary(mos6502_t *cpu)
{
    mos6502_set_flag(cpu, ZERO, 0x1);
    mos6502_write8(cpu, 0x8000, 0xF0);
    mos6502_write8(cpu, 0x8001, -5);
    int ticks = mos6502_tick(cpu);
    return ticks == 4 && cpu->pc == 0x7FFC && cpu->flags == ZERO;
}

void test_mos6502_beq()
{
    RUN_TEST(test_beq_no_branch);
    RUN_TEST(test_beq_branch);
    RUN_TEST(test_beq_page_boundary);
}
#endif