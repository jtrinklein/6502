#include <cxxtest/TestSuite.h>
#include <cpu.h>
#include <mem.h>

class BVS_Realtive_Tests : public CxxTest::TestSuite 
{
public:
    CPU*  cpu;
    Mem* mem;
    static constexpr Byte opcode = 0x70;
    static constexpr Byte op_size = 2;
    static constexpr Byte op_cycles = 2;

    void setUp() {
        mem= new Mem();
        cpu = new CPU(mem);
    }

    void tearDown() {
        delete cpu;
        delete mem;
    }

    void test_WithPositiveValue_WithOverflowSet( void ) {
        const Byte relative_value = 0x18;
        const Byte d[] = {opcode, relative_value};
        const Word pc_start = 0x8000;

        mem->LoadFromDataAtOffset(d, sizeof(d), pc_start);

        cpu->PC = pc_start;
        cpu->Overflow = 1;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cycles, op_cycles + 1);
        TS_ASSERT_EQUALS(cpu->PC, pc_start + op_size + relative_value);
        TS_ASSERT_EQUALS(cpu->Overflow, 1);
    }

    void test_WithPositiveValue_WithOverflowSet_WithPageCross( void ) {
        const Byte relative_value = 0x42;
        const Byte d[] = {opcode, relative_value};
        const Word pc_start = 0x80F0;

        mem->LoadFromDataAtOffset(d, sizeof(d), pc_start);

        cpu->PC = pc_start;
        cpu->Overflow = 1;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cycles, op_cycles + 2);
        TS_ASSERT_EQUALS(cpu->PC, pc_start + op_size + relative_value);
        TS_ASSERT_EQUALS(cpu->Overflow, 1);
    }

    void test_WithPositiveValue_WithOverflowNotSet( void ) {
        const Byte relative_value = 0x0F;
        const Byte d[] = {opcode, relative_value};
        const Word pc_start = 0x8000;

        mem->LoadFromDataAtOffset(d, sizeof(d), pc_start);

        cpu->PC = pc_start;
        cpu->Overflow = 0;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->PC, pc_start + op_size);
        TS_ASSERT_EQUALS(cpu->Overflow, 0);
    }

    void test_WithNegativeValue_WithOverflowSet( void ) {
        const Byte relative_value = 0xF1;
        const Byte offset = (0x100 - relative_value) & 0xFF;
        const Byte d[] = {opcode, relative_value};
        const Word pc_start = 0x8080;
        mem->LoadFromDataAtOffset(d, sizeof(d), pc_start);

        cpu->PC = pc_start;
        cpu->Overflow = 1;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cycles, op_cycles + 1);
        TS_ASSERT_EQUALS(cpu->PC, pc_start + op_size - offset);
        TS_ASSERT_EQUALS(cpu->Overflow, 1);
    }

    void test_WithNegativeValue_WithOverflowSet_WithPageCross( void ) {
        const Byte relative_value = 0x98;
        const Byte offset = (0x100 - relative_value) & 0xFF;
        const Byte d[] = {opcode, relative_value};
        const Word pc_start = 0x8000;

        mem->LoadFromDataAtOffset(d, sizeof(d), pc_start);

        cpu->PC = pc_start;
        cpu->Overflow = 1;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cycles, op_cycles + 2);
        TS_ASSERT_EQUALS(cpu->PC, pc_start + op_size - offset);
        TS_ASSERT_EQUALS(cpu->Overflow, 1);
    }

    void test_WithNegativeValue_WithOverflowNotSet( void ) {
        const Byte relative_value = 0xC3;
        const Byte d[] = {opcode, relative_value};
        const Word pc_start = 0x7777;

        mem->LoadFromDataAtOffset(d, sizeof(d), pc_start);

        cpu->PC = pc_start;
        cpu->Overflow = 0;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->PC, pc_start + op_size);
        TS_ASSERT_EQUALS(cpu->Overflow, 0);
    }
};