#include "mos6502.h"

int mos6502_init(mos6502_t *cpu)
{
    memset(cpu, 0, sizeof(mos6502_t));

    mos6502_register_lda(cpu);
    mos6502_register_and(cpu);
    mos6502_register_tax(cpu);

    cpu->rst = 1;
    cpu->rdy = 1;

    return 0;
}

void mos6502_set_flag(mos6502_t *cpu, int flag, int value)
{
    if (value)
    {
        cpu->flags |= flag;
    }
    else
    {
        cpu->flags &= ~flag;
    }
}

int mos6502_get_flag(mos6502_t *cpu, int flag)
{
    return (cpu->flags & flag) != 0;
}

uint8_t mos6502_read8(mos6502_t *cpu, uint16_t address)
{
    return cpu->read(cpu, address);
}

void mos6502_write8(mos6502_t *cpu, uint16_t address, uint8_t value)
{
    cpu->write(cpu, address, value);
}

uint16_t mos6502_read16(mos6502_t *cpu, uint16_t address)
{
    uint16_t low = (uint16_t)mos6502_read8(cpu, address);
    uint16_t high = (uint16_t)mos6502_read8(cpu, address + 1);
    return (high << 8) | low;
}

void mos6502_write16(mos6502_t *cpu, uint16_t address, uint16_t value)
{
    uint8_t low = (uint8_t)(value & 0xFF);
    uint8_t high = (uint8_t)((value >> 8) & 0xFF);
    mos6502_write8(cpu, address, low);
    mos6502_write8(cpu, address + 1, high);
}

int mos6502_tick(mos6502_t *cpu)
{
    if (cpu->rst)
    {
        cpu->pc = mos6502_read16(cpu, 0xFFFC);
        cpu->rst = 0;
    }

    if (!cpu->rdy)
    {
        return 0;
    }

    uint8_t opcode = mos6502_read8(cpu, cpu->pc++);
    if (!cpu->opcodes[opcode])
    {
        return -1;
    }

    return cpu->opcodes[opcode](cpu);
}

void mos6502_register_opcode(mos6502_t *cpu, uint8_t opcode, int (*func)(mos6502_t *cpu))
{
    if (cpu->opcodes[opcode])
    {
        fprintf(stderr, "opcode %u already registered\n", opcode);
        exit(1);
    }
    cpu->opcodes[opcode] = func;
}

#ifdef _TEST

static int test_write8(mos6502_t *cpu)
{
    mos6502_write8(cpu, 0x8000, 0xff);
    return mos6502_read8(cpu, 0x8000) == 0xff;
}

static int test_write16(mos6502_t *cpu)
{
    mos6502_write16(cpu, 0x8000, 0x0102);
    return mos6502_read16(cpu, 0x8000) == 0x0102;
}


static int test_tick(mos6502_t *cpu)
{
    int ticks = mos6502_tick(cpu);
    return cpu->pc == 0x8001;
}

void test_mos6502_core()
{
    RUN_TEST(test_write8);
    RUN_TEST(test_write16);
    RUN_TEST(test_tick);
}
#endif