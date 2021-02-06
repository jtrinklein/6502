#include <cxxtest/TestSuite.h>
#include <cpu.h>
#include <mem.h>

class LDX_ZeroPage_Tests : public CxxTest::TestSuite 
{
public:
    CPU*  cpu;
    Mem* mem;
    static constexpr Byte opcode = 0xA6;
    static constexpr Byte op_size = 2;
    static constexpr Byte op_cycles = 3;

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
        cpu->X = 0x11;
        cpu->Zero = 0;
        cpu->Negative = 1;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cpu->X, test_val);
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
        cpu->X = 0x00;
        cpu->Zero = 1;
        cpu->Negative = 1;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cpu->X, nonzero_val);
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
        cpu->X = 0x00;
        cpu->Zero = 1;
        cpu->Negative = 0;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cpu->X, test_val);
        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->PC - pc_start, op_size);
        TS_ASSERT_EQUALS(cpu->Zero, 0);
        TS_ASSERT_EQUALS(cpu->Negative, 1);
    }
};