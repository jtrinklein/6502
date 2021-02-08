#include <cxxtest/TestSuite.h>
#include <cpu.h>
#include <mem.h>

class TXA_Implied_Tests : public CxxTest::TestSuite 
{
public:
    CPU*  cpu;
    Mem* mem;
    static constexpr Byte opcode = 0x8A;
    static constexpr Byte op_size = 1;
    static constexpr Byte op_cycles = 2;

    void setUp() {
        mem= new Mem();
        cpu = new CPU(mem);
        cpu->PC = 0x0000;
    }

    void tearDown() {
        delete cpu;
        delete mem;
    }

    void test_WithZero( void ) {
        const Byte test_val = 0x00;
        const Byte d[] = {opcode, 0xFF};
        mem->LoadFromData(d, sizeof(d));
        cpu->X = test_val;
        cpu->A = 0x11;
        cpu->Zero = 0;
        cpu->Negative = 1;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->A, test_val);
        TS_ASSERT_EQUALS(cpu->X, test_val);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(cpu->Zero, 1);
        TS_ASSERT_EQUALS(cpu->Negative, 0);
    }

    void test_WithNonZero( void ) {
        const Byte test_val = 0x67;
        const Byte d[] = {opcode, 0xFF};
        mem->LoadFromData(d, sizeof(d));
        cpu->X = test_val;
        cpu->A = 0x00;
        cpu->Zero = 1;
        cpu->Negative = 1;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->A, test_val);
        TS_ASSERT_EQUALS(cpu->X, test_val);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(cpu->Zero, 0);
        TS_ASSERT_EQUALS(cpu->Negative, 0);
    }

    void test_WithNegative( void ) {
        const Byte test_val = 0xE7;
        const Byte d[] = {opcode, 0xFF};
        mem->LoadFromData(d, sizeof(d));
        cpu->X = test_val;
        cpu->A = 0x00;
        cpu->Zero = 1;
        cpu->Negative = 0;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->A, test_val);
        TS_ASSERT_EQUALS(cpu->X, test_val);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(cpu->Zero, 0);
        TS_ASSERT_EQUALS(cpu->Negative, 1);
    }
    
};