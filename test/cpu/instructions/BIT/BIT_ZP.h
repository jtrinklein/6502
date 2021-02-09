#include <cxxtest/TestSuite.h>
#include <cpu.h>
#include <mem.h>

class BIT_ZeroPage_Tests : public CxxTest::TestSuite 
{
public:
    CPU*  cpu;
    Mem* mem;
    static constexpr Byte opcode = 0x24;
    static constexpr Byte op_size = 2;
    static constexpr Byte op_cycles = 3;

    static constexpr Byte overflow_bit = 0b01000000;
    static constexpr Byte negative_bit = 0b10000000;

    void setUp() {
        mem= new Mem();
        cpu = new CPU(mem);
        cpu->PC = 0x0000;
    }

    void tearDown() {
        delete cpu;
        delete mem;
    }

    void test_Should_SetZero_SetOverflow_SetNegative( void ) {
        const Byte test_val = 0b00010010 | overflow_bit | negative_bit;
        const Byte mask_val = 0b00001001;
        const Byte zp_addr = 0x02;
        const Byte d[] = {opcode, zp_addr, test_val};
        mem->LoadFromData(d, sizeof(d));

        cpu->A = mask_val;
        cpu->Zero = 0;
        cpu->Overflow = 0;
        cpu->Negative = 0;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cpu->A, mask_val);
        TS_ASSERT_EQUALS(mem->ReadByte(zp_addr), test_val);

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(cpu->Zero, 1);
        TS_ASSERT_EQUALS(cpu->Overflow, 1);
        TS_ASSERT_EQUALS(cpu->Negative, 1);
    }

    void test_Should_SetNotZero_SetOverflow_SetNegative( void ) {
        const Byte test_val = 0b00001001 | overflow_bit | negative_bit;
        const Byte mask_val = 0b00001001;
        const Byte zp_addr = 0x02;
        const Byte d[] = {opcode, zp_addr, test_val};
        mem->LoadFromData(d, sizeof(d));

        cpu->A = mask_val;
        cpu->Zero = 1;
        cpu->Overflow = 0;
        cpu->Negative = 0;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cpu->A, mask_val);
        TS_ASSERT_EQUALS(mem->ReadByte(zp_addr), test_val);

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(cpu->Zero, 0);
        TS_ASSERT_EQUALS(cpu->Overflow, 1);
        TS_ASSERT_EQUALS(cpu->Negative, 1);
    }

    void test_Should_SetZero_SetNotOverflow_SetNegative( void ) {
        const Byte test_val = 0b00110110 | negative_bit;
        const Byte mask_val = 0b00001001 | overflow_bit;
        const Byte zp_addr = 0x02;
        const Byte d[] = {opcode, zp_addr, test_val};
        mem->LoadFromData(d, sizeof(d));

        cpu->A = mask_val;
        cpu->Zero = 0;
        cpu->Overflow = 1;
        cpu->Negative = 0;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cpu->A, mask_val);
        TS_ASSERT_EQUALS(mem->ReadByte(zp_addr), test_val);

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(cpu->Zero, 1);
        TS_ASSERT_EQUALS(cpu->Overflow, 0);
        TS_ASSERT_EQUALS(cpu->Negative, 1);
    }

    void test_Should_SetNotZero_SetNotOverflow_SetNegative( void ) {
        const Byte test_val = 0b00000110 | negative_bit;
        const Byte mask_val = 0b00001001 | negative_bit | overflow_bit;
        const Byte zp_addr = 0x02;
        const Byte d[] = {opcode, zp_addr, test_val};
        mem->LoadFromData(d, sizeof(d));

        cpu->A = mask_val;
        cpu->Zero = 1;
        cpu->Overflow = 1;
        cpu->Negative = 0;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cpu->A, mask_val);
        TS_ASSERT_EQUALS(mem->ReadByte(zp_addr), test_val);

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(cpu->Zero, 0);
        TS_ASSERT_EQUALS(cpu->Overflow, 0);
        TS_ASSERT_EQUALS(cpu->Negative, 1);
    }

    void test_Should_SetZero_SetOverflow_SetNotNegative( void ) {
        const Byte test_val = 0b00010010 | overflow_bit;
        const Byte mask_val = 0b00101101;
        const Byte zp_addr = 0x02;
        const Byte d[] = {opcode, zp_addr, test_val};
        mem->LoadFromData(d, sizeof(d));

        cpu->A = mask_val;
        cpu->Zero = 0;
        cpu->Overflow = 0;
        cpu->Negative = 1;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cpu->A, mask_val);
        TS_ASSERT_EQUALS(mem->ReadByte(zp_addr), test_val);

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(cpu->Zero, 1);
        TS_ASSERT_EQUALS(cpu->Overflow, 1);
        TS_ASSERT_EQUALS(cpu->Negative, 0);
    }

    void test_Should_SetNotZero_SetOverflow_SetNotNegative( void ) {
        const Byte test_val = 0b00000001 | overflow_bit;
        const Byte mask_val = 0b00011111;
        const Byte zp_addr = 0x02;
        const Byte d[] = {opcode, zp_addr, test_val};
        mem->LoadFromData(d, sizeof(d));

        cpu->A = mask_val;
        cpu->Zero = 1;
        cpu->Overflow = 0;
        cpu->Negative = 1;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cpu->A, mask_val);
        TS_ASSERT_EQUALS(mem->ReadByte(zp_addr), test_val);

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(cpu->Zero, 0);
        TS_ASSERT_EQUALS(cpu->Overflow, 1);
        TS_ASSERT_EQUALS(cpu->Negative, 0);
    }

    void test_Should_SetZero_SetNotOverflow_SetNotNegative( void ) {
        const Byte test_val = 0b00110110;
        const Byte mask_val = 0b00001001 | overflow_bit;
        const Byte zp_addr = 0x02;
        const Byte d[] = {opcode, zp_addr, test_val};
        mem->LoadFromData(d, sizeof(d));

        cpu->A = mask_val;
        cpu->Zero = 0;
        cpu->Overflow = 1;
        cpu->Negative = 1;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cpu->A, mask_val);
        TS_ASSERT_EQUALS(mem->ReadByte(zp_addr), test_val);

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(cpu->Zero, 1);
        TS_ASSERT_EQUALS(cpu->Overflow, 0);
        TS_ASSERT_EQUALS(cpu->Negative, 0);
    }

    void test_Should_SetNotZero_SetNotOverflow_SetNotNegative( void ) {
        const Byte test_val = 0b00100110 ;
        const Byte mask_val = 0b00101001 | negative_bit | overflow_bit;
        const Byte zp_addr = 0x02;
        const Byte d[] = {opcode, zp_addr, test_val};
        mem->LoadFromData(d, sizeof(d));

        cpu->A = mask_val;
        cpu->Zero = 1;
        cpu->Overflow = 1;
        cpu->Negative = 1;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cpu->A, mask_val);
        TS_ASSERT_EQUALS(mem->ReadByte(zp_addr), test_val);

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(cpu->Zero, 0);
        TS_ASSERT_EQUALS(cpu->Overflow, 0);
        TS_ASSERT_EQUALS(cpu->Negative, 0);
    }
};