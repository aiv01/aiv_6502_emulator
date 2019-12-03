#include "cpu/mos6502.h"

typedef struct mos6502_test
{
    mos6502_t base;
    uint8_t memory[65536];
} mos6502_test_t;

uint8_t mos6502_test_read(mos6502_t *cpu, uint16_t address)
{
    mos6502_test_t *cpu_test = (mos6502_test_t *)cpu;
    return cpu_test->memory[address];
}

void mos6502_test_write(mos6502_t *cpu, uint16_t address, uint8_t value)
{
    mos6502_test_t *cpu_test = (mos6502_test_t *)cpu;
    cpu_test->memory[address] = value;
}

static uint64_t tests_succeded = 0;
static uint64_t tests_failed = 0;

void mos6502_test_wrapper(const char *name, int (*func)(mos6502_t *cpu))
{
    mos6502_test_t cpu;
    mos6502_init((mos6502_t *)&cpu);

    cpu.base.read = mos6502_test_read;
    cpu.base.write = mos6502_test_write;

    memset(cpu.memory, 0, 65536);

    mos6502_write16((mos6502_t *)&cpu, 0xfffc, 0x8000);

    if (!func((mos6502_t *)&cpu))
    {
        fprintf(stderr, "TEST %s FAILED\n", name);
        tests_failed++;
    }
    else
    {
        tests_succeded++;
    }
    

}

int main(int argc, char **argv)
{
    test_mos6502_core();
    test_mos6502_lda();
    test_mos6502_ldx();
    test_mos6502_and();
    test_mos6502_tay();
   

    fprintf(stdout, "Tests succeded: %llu failed: %llu\n", tests_succeded, tests_failed);
    return 0;
}