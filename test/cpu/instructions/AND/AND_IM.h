#include <cxxtest/TestSuite.h>
#include <cpu.h>
#include <mem.h>

class AND_Immediate_Tests : public CxxTest::TestSuite 
{
public:
    CPU*  cpu;
    Mem* mem;
    static constexpr Byte opcode = 0x29;
    static constexpr Byte op_size = 2;
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

    void test_Should_ClearZero_ClearNegative( void ) {
        const Byte A_val    = 0b10011010;
        const Byte test_val = 0b00010010;
        const Byte expect_val = A_val & test_val;
        const Byte d[] = {opcode, test_val, 0xFF};

        mem->LoadFromData(d, sizeof(d));

        cpu->A = A_val;
        cpu->Zero = 1;
        cpu->Negative = 1;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->A, expect_val);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(cpu->Zero, 0);
        TS_ASSERT_EQUALS(cpu->Negative, 0);
    }

    void test_Should_SetZero_ClearNegative( void ) {
        const Byte A_val    = 0b10011010;
        const Byte test_val = 0b01100101;
        const Byte expect_val = A_val & test_val;
        const Byte d[] = {opcode, test_val, 0xFF};

        mem->LoadFromData(d, sizeof(d));

        cpu->A = A_val;
        cpu->Zero = 0;
        cpu->Negative = 1;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->A, expect_val);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(cpu->Zero, 1);
        TS_ASSERT_EQUALS(cpu->Negative, 0);
    }

    void test_Should_SetNegative_ClearZero( void ) {
        const Byte A_val    = 0b10011010;
        const Byte test_val = 0b10100100;
        const Byte expect_val = A_val & test_val;
        const Byte d[] = {opcode, test_val, 0xFF};

        mem->LoadFromData(d, sizeof(d));

        cpu->A = A_val;
        cpu->Zero = 1;
        cpu->Negative = 0;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->A, expect_val);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(cpu->Zero, 0);
        TS_ASSERT_EQUALS(cpu->Negative, 1);
    }
};