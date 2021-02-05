#include <cxxtest/TestSuite.h>
#include <cpu.h>
#include <mem.h>

class LDA_Immediate_Tests : public CxxTest::TestSuite 
{
public:
    CPU*  cpu;
    Mem* mem;
    static constexpr Byte opcode = 0xA9;
    static constexpr Byte op_size = 2;
    static constexpr Byte op_cycles = 2;
    static constexpr Word program_start = 0x8000;

    void setUp() {
        mem= new Mem();
        cpu = new CPU(mem);
        cpu->PC = program_start;
    }

    void tearDown() {
        delete cpu;
        delete mem;
    }

    void test_WithZero( void ) {
        const Byte test_val = 0x00;
        const Byte d[] = {opcode, test_val, 0xFF};
        mem->LoadFromDataAtOffset(d, sizeof(d), program_start);
        cpu->A = 0x11;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->A, test_val);
        TS_ASSERT_EQUALS(cpu->PC - program_start, op_size);
        TS_ASSERT_EQUALS(cpu->Zero, 1);
        TS_ASSERT_EQUALS(cpu->Negative, 0);
    }

    void test_WithNonZero( void ) {
        const Byte test_val = 0x67;
        const Byte d[] = {opcode, test_val, 0xFF};
        mem->LoadFromDataAtOffset(d, sizeof(d), program_start);
        cpu->A = 0x00;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->A, test_val);
        TS_ASSERT_EQUALS(cpu->PC - program_start, op_size);
        TS_ASSERT_EQUALS(cpu->Zero, 0);
        TS_ASSERT_EQUALS(cpu->Negative, 0);
    }

    void test_WithNegative( void ) {
        const Byte test_val = 0xE7;
        const Byte d[] = {opcode, test_val, 0xFF};
        mem->LoadFromDataAtOffset(d, sizeof(d), program_start);
        cpu->A = 0x00;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->A, test_val);
        TS_ASSERT_EQUALS(cpu->PC - program_start, op_size);
        TS_ASSERT_EQUALS(cpu->Zero, 0);
        TS_ASSERT_EQUALS(cpu->Negative, 1);
    }
    
};