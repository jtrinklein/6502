#include <cxxtest/TestSuite.h>
#include <cpu.h>
#include <mem.h>

class LDA_ZeroPage_Tests : public CxxTest::TestSuite 
{
public:
    CPU*  cpu;
    Mem* mem;
    static constexpr Byte opcode = 0xA5;
    static constexpr Byte op_cycles = 3;
    static constexpr Byte op_size = 2;

    void setUp() {
        mem= new Mem();
        cpu = new CPU(mem);
    }

    void tearDown() {
        delete cpu;
        delete mem;
    }

    void test_WithZero( void ) {
        const Byte test_val = 0x00;
        const Byte zp_addr = 0x01;
        const Byte d[] = {0x01, test_val, opcode, zp_addr, 0xFF};
        mem->LoadFromData(d, sizeof(d));

        const Word pc_start = 0x0002;
        cpu->PC = pc_start;
        cpu->A = 0x11;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cpu->A, test_val);
        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->PC - pc_start, op_size);
        TS_ASSERT_EQUALS(cpu->Zero, 1);
        TS_ASSERT_EQUALS(cpu->Negative, 0);
    }

    void test_WithNonZero( void ) {
        const Byte nonzero_val = 0x67;
        const Byte zp_addr = 0x01;
        const Byte d[] = {0x00, nonzero_val, 0x1, opcode, zp_addr, 0xFF};
        mem->LoadFromData(d, sizeof(d));
        
        const Word pc_start = 0x0003;
        cpu->PC = pc_start;
        cpu->A = 0x00;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cpu->A, nonzero_val);
        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->PC - pc_start, op_size);
        TS_ASSERT_EQUALS(cpu->Zero, 0);
        TS_ASSERT_EQUALS(cpu->Negative, 0);
    }

    void test_WithNegative( void ) {
        const Byte test_val = 0xE7;
        const Byte zp_addr = 0x00;
        const Byte d[] = {test_val, 0x01, opcode, zp_addr, 0xFF};
        mem->LoadFromData(d, sizeof(d));
        const Word pc_start = 0x0002;
        cpu->PC = pc_start;
        cpu->A = 0x00;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cpu->A, test_val);
        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->PC - pc_start, op_size);
        TS_ASSERT_EQUALS(cpu->Zero, 0);
        TS_ASSERT_EQUALS(cpu->Negative, 1);
    }
};