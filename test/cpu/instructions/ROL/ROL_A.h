#include <cxxtest/TestSuite.h>
#include <cpu.h>
#include <mem.h>

class ROL_A_Tests : public CxxTest::TestSuite 
{
public:
    CPU*  cpu;
    Mem* mem;
    static constexpr Byte opcode = 0x2A;
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

    void test_WithZero_NoCarrySet( void ) {
        const Byte test_val  = 0b00000000;
        const Byte expect_val = 0b00000000;
        const Byte d[] = {opcode, 0xFF};

        mem->LoadFromData(d, sizeof(d));

        cpu->A = test_val;
        cpu->Carry = 0;
        cpu->Zero = 0;
        cpu->Negative = 1;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->A, expect_val);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(cpu->Zero, 1);
        TS_ASSERT_EQUALS(cpu->Negative, 0);
        TS_ASSERT_EQUALS(cpu->Carry, 0);
    }

    void test_WithZero_CarrySet( void ) {
        const Byte test_val  = 0b00000000;
        const Byte expect_val = 0b00000001;
        const Byte d[] = {opcode, 0xFF};

        mem->LoadFromData(d, sizeof(d));

        cpu->A = test_val;
        cpu->Carry = 1;
        cpu->Zero = 1;
        cpu->Negative = 1;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->A, expect_val);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(cpu->Zero, 0);
        TS_ASSERT_EQUALS(cpu->Negative, 0);
        TS_ASSERT_EQUALS(cpu->Carry, 0);
    }

    void test_WithNonZero_NoCarrySet_NoCarry( void ) {
        const Byte test_val  = 0b01001011;
        const Byte expect_val = 0b10010110;
        const Byte d[] = {opcode, 0xFF};

        mem->LoadFromData(d, sizeof(d));

        cpu->A = test_val;
        cpu->Carry = 0;
        cpu->Zero = 1;
        cpu->Negative = 0;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cpu->A, expect_val);
        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(cpu->Zero, 0);
        TS_ASSERT_EQUALS(cpu->Negative, 1);
        TS_ASSERT_EQUALS(cpu->Carry, 0);
    }

    void test_WithNonZero_NoCarrySet_WithCarry( void ) {
        const Byte test_val  = 0b11001010;
        const Byte expect_val = 0b10010100;
        const Byte d[] = {opcode, 0xFF};

        mem->LoadFromData(d, sizeof(d));

        cpu->A = test_val;
        cpu->Carry = 0;
        cpu->Zero = 1;
        cpu->Negative = 0;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cpu->A, expect_val);
        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(cpu->Zero, 0);
        TS_ASSERT_EQUALS(cpu->Negative, 1);
        TS_ASSERT_EQUALS(cpu->Carry, 1);
    }

    void test_WithNonZero_CarrySet_NoCarry( void ) {
        const Byte test_val  = 0b00101011;
        const Byte expect_val = 0b01010111;
        const Byte d[] = {opcode, 0xFF};

        mem->LoadFromData(d, sizeof(d));

        cpu->A = test_val;
        cpu->Carry = 1;
        cpu->Zero = 1;
        cpu->Negative = 1;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cpu->A, expect_val);
        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(cpu->Zero, 0);
        TS_ASSERT_EQUALS(cpu->Negative, 0);
        TS_ASSERT_EQUALS(cpu->Carry, 0);
    }

    void test_WithNonZero_CarrySet_WithCarry( void ) {
        const Byte test_val  = 0b11001010;
        const Byte expect_val = 0b10010101;
        const Byte d[] = {opcode, 0xFF};

        mem->LoadFromData(d, sizeof(d));

        cpu->A = test_val;
        cpu->Carry = 1;
        cpu->Zero = 1;
        cpu->Negative = 0;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cpu->A, expect_val);
        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(cpu->Zero, 0);
        TS_ASSERT_EQUALS(cpu->Negative, 1);
        TS_ASSERT_EQUALS(cpu->Carry, 1);
    }
};