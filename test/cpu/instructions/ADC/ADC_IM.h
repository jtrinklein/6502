#include <cxxtest/TestSuite.h>
#include <cpu.h>
#include <mem.h>

class ADC_Immediate_Tests : public CxxTest::TestSuite 
{
public:
    CPU*  cpu;
    Mem* mem;
    static constexpr Byte opcode = 0x69;
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

    void test_Should_add_with_nonzero( void ) {
        const Byte A_val      = 0b00101010;
        const Byte test_val   = 0b00010010;
        const Byte expect_val = 0b00111100;
        const Byte d[] = {opcode, test_val, 0xFF};

        mem->LoadFromData(d, sizeof(d));

        cpu->Carry = 0;
        cpu->A = A_val;
        cpu->Zero = 1;
        cpu->Negative = 1;
        cpu->Overflow = 1;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->A, expect_val);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(cpu->Zero, 0);
        TS_ASSERT_EQUALS(cpu->Negative, 0);
        TS_ASSERT_EQUALS(cpu->Overflow, 0);
        TS_ASSERT_EQUALS(cpu->Carry, 0);
    }

    void test_Should_add_with_zero( void ) {
        const Byte A_val      = 0b00000000;
        const Byte test_val   = 0b00000000;
        const Byte expect_val = 0b00000000;
        const Byte d[] = {opcode, test_val, 0xFF};

        mem->LoadFromData(d, sizeof(d));

        cpu->Carry = 0;
        cpu->A = A_val;
        cpu->Zero = 0;
        cpu->Negative = 1;
        cpu->Overflow = 1;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->A, expect_val);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(cpu->Zero, 1);
        TS_ASSERT_EQUALS(cpu->Negative, 0);
        TS_ASSERT_EQUALS(cpu->Overflow, 0);
        TS_ASSERT_EQUALS(cpu->Carry, 0);
    }

    void test_Should_add_with_nonzero_and_carry( void ) {
        const Byte A_val      = 0b00101010;
        const Byte test_val   = 0b00010010;
        const Byte expect_val = 0b00111101;
        const Byte d[] = {opcode, test_val, 0xFF};

        mem->LoadFromData(d, sizeof(d));

        cpu->Carry = 1;
        cpu->A = A_val;
        cpu->Zero = 1;
        cpu->Negative = 1;
        cpu->Overflow = 1;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->A, expect_val);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(cpu->Zero, 0);
        TS_ASSERT_EQUALS(cpu->Negative, 0);
        TS_ASSERT_EQUALS(cpu->Overflow, 0);
        TS_ASSERT_EQUALS(cpu->Carry, 0);
    }

    void test_Should_add_with_zero_and_carry( void ) {
        const Byte A_val      = 0b00000000;
        const Byte test_val   = 0b00000000;
        const Byte expect_val = 0b00000001;
        const Byte d[] = {opcode, test_val, 0xFF};

        mem->LoadFromData(d, sizeof(d));

        cpu->Carry = 1;
        cpu->A = A_val;
        cpu->Zero = 1;
        cpu->Negative = 1;
        cpu->Overflow = 1;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->A, expect_val);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(cpu->Zero, 0);
        TS_ASSERT_EQUALS(cpu->Negative, 0);
        TS_ASSERT_EQUALS(cpu->Overflow, 0);
        TS_ASSERT_EQUALS(cpu->Carry, 0);
    }


    void test_Should_set_negative_but_not_overflow( void ) {
        const Byte A_val      = 0b00101010;
        const Byte test_val   = 0b10010010;
        const Byte expect_val = 0b10111100;
        const Byte d[] = {opcode, test_val, 0xFF};

        mem->LoadFromData(d, sizeof(d));

        cpu->Carry = 0;
        cpu->A = A_val;
        cpu->Zero = 1;
        cpu->Negative = 0;
        cpu->Overflow = 1;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->A, expect_val);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(cpu->Zero, 0);
        TS_ASSERT_EQUALS(cpu->Negative, 1);
        TS_ASSERT_EQUALS(cpu->Overflow, 0);
        TS_ASSERT_EQUALS(cpu->Carry, 0);
    }

    void test_Should_set_negative_and_overflow( void ) {
        const Byte A_val      = 0b01101010;
        const Byte test_val   = 0b01010010;
        const Byte expect_val = 0b10111100;
        const Byte d[] = {opcode, test_val, 0xFF};

        mem->LoadFromData(d, sizeof(d));

        cpu->Carry = 0;
        cpu->A = A_val;
        cpu->Zero = 1;
        cpu->Negative = 0;
        cpu->Overflow = 0;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->A, expect_val);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(cpu->Zero, 0);
        TS_ASSERT_EQUALS(cpu->Negative, 1);
        TS_ASSERT_EQUALS(cpu->Overflow, 1);
        TS_ASSERT_EQUALS(cpu->Carry, 0);
    }

    void test_Should_set_carry_from_overflowing_bit_7( void ) {
        const Byte A_val      = 0b11010100;
        const Byte test_val   = 0b10100100;
        const Byte expect_val = 0b01111000;
        const Byte d[] = {opcode, test_val, 0xFF};

        mem->LoadFromData(d, sizeof(d));

        cpu->Carry = 0;
        cpu->A = A_val;
        cpu->Zero = 1;
        cpu->Negative = 0;
        cpu->Overflow = 0;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->A, expect_val);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(cpu->Zero, 0);
        TS_ASSERT_EQUALS(cpu->Negative, 0);
        TS_ASSERT_EQUALS(cpu->Overflow, 0);
        TS_ASSERT_EQUALS(cpu->Carry, 1);
    }

    void test_Should_work_in_DecimalMode( void ) {
        const Byte A_val      = 0x12;
        const Byte test_val   = 0x34;
        const Byte expect_val = 0x46;
        const Byte d[] = {opcode, test_val, 0xFF};

        mem->LoadFromData(d, sizeof(d));

        cpu->DecimalMode = 1;
        cpu->Carry = 0;
        cpu->A = A_val;
        cpu->Zero = 1;
        cpu->Negative = 1;
        cpu->Overflow = 1;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->A, expect_val);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(cpu->Zero, 0);
        TS_ASSERT_EQUALS(cpu->Negative, 0);
        TS_ASSERT_EQUALS(cpu->Overflow, 0);
        TS_ASSERT_EQUALS(cpu->Carry, 0);
    }

    void test_Should_add_Carry_in_DecimalMode( void ) {
        const Byte A_val      = 0x11;
        const Byte test_val   = 0x22;
        const Byte expect_val = 0x34;
        const Byte d[] = {opcode, test_val, 0xFF};

        mem->LoadFromData(d, sizeof(d));

        cpu->DecimalMode = 1;
        cpu->Carry = 1;
        cpu->A = A_val;
        cpu->Zero = 1;
        cpu->Negative = 1;
        cpu->Overflow = 1;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->A, expect_val);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(cpu->Zero, 0);
        TS_ASSERT_EQUALS(cpu->Negative, 0);
        TS_ASSERT_EQUALS(cpu->Overflow, 0);
        TS_ASSERT_EQUALS(cpu->Carry, 0);
    }

    void test_Should_work_in_DecimalMode_with_halfcarry( void ) {
        const Byte A_val      = 0x39;
        const Byte test_val   = 0x14;
        const Byte expect_val = 0x53;
        const Byte d[] = {opcode, test_val, 0xFF};

        mem->LoadFromData(d, sizeof(d));

        cpu->DecimalMode = 1;
        cpu->Carry = 0;
        cpu->A = A_val;
        cpu->Zero = 1;
        cpu->Negative = 1;
        cpu->Overflow = 1;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->A, expect_val);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(cpu->Zero, 0);
        TS_ASSERT_EQUALS(cpu->Negative, 0);
        TS_ASSERT_EQUALS(cpu->Overflow, 0);
        TS_ASSERT_EQUALS(cpu->Carry, 0);
    }

    void test_Should_work_in_DecimalMode_with_halfcarry_and_fullcarry( void ) {
        const Byte A_val      = 0x39;
        const Byte test_val   = 0x74;
        const Byte expect_val = 0x13;
        const Byte d[] = {opcode, test_val, 0xFF};

        mem->LoadFromData(d, sizeof(d));

        cpu->DecimalMode = 1;
        cpu->Carry = 0;
        cpu->A = A_val;
        cpu->Zero = 1;
        cpu->Negative = 1;
        cpu->Overflow = 1;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->A, expect_val);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(cpu->Zero, 0);
        TS_ASSERT_EQUALS(cpu->Negative, 0);
        TS_ASSERT_EQUALS(cpu->Overflow, 1);
        TS_ASSERT_EQUALS(cpu->Carry, 1);
    }

};