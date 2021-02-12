#include <cxxtest/TestSuite.h>
#include <cpu.h>
#include <mem.h>

class AND_Indirect_X_Tests : public CxxTest::TestSuite 
{
public:
    CPU*  cpu;
    Mem* mem;
    static constexpr Byte opcode = 0x21;
    static constexpr Byte op_size = 2;
    static constexpr Byte op_cycles = 6;

    void setUp() {
        mem= new Mem();
        cpu = new CPU(mem);
    }

    void tearDown() {
        delete cpu;
        delete mem;
    }

    void test_Should_SetZero_ClearNegative( void ) {
        const Byte A_val    = 0b10101001;
        const Byte test_val = 0b00010010;
        const Byte expect_val = A_val & test_val;
        const Word zp_addr = 0x54;
        const Byte offset = 0x01;
        const Byte d[] = {opcode, zp_addr, 0xFF};
        const Word addr = 0xF00D;

        mem->LoadFromData(d, sizeof(d));
        mem->WriteWord((zp_addr +offset)& 0xFF, addr);
        mem->WriteByte(addr, test_val);

        cpu->X = offset;
        cpu->A = A_val;
        cpu->Zero = 0;
        cpu->Negative = 1;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(cpu->A, expect_val);
        TS_ASSERT_EQUALS(cpu->Zero, 1);
        TS_ASSERT_EQUALS(cpu->Negative, 0);
    }

    void test_Should_ClearZero_ClearNegative( void ) {
        const Byte A_val    = 0b10101001;
        const Byte test_val = 0b00110010;
        const Byte expect_val = A_val & test_val;
        const Word zp_addr = 0x8F;
        const Byte offset = 0x12;
        const Byte d[] = {opcode, zp_addr};
        const Word addr = 0xDEAD;

        mem->LoadFromData(d, sizeof(d));
        mem->WriteWord((zp_addr +offset)& 0xFF, addr);
        mem->WriteByte(addr, test_val);

        cpu->X = offset;
        cpu->A = A_val;
        cpu->Zero = 1;
        cpu->Negative = 1;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(cpu->A, expect_val);
        TS_ASSERT_EQUALS(cpu->Zero, 0);
        TS_ASSERT_EQUALS(cpu->Negative, 0);
    }

    void test_Should_ClearZero_SetNegative( void ) {
        const Byte A_val    = 0b10101001;
        const Byte test_val = 0b10010010;
        const Byte expect_val = A_val & test_val;
        const Word zp_addr = 0xAB;
        const Byte offset = 0x01;
        const Byte d[] = {opcode, zp_addr};
        const Word addr = 0xFADE;

        mem->LoadFromData(d, sizeof(d));
        mem->WriteWord((zp_addr +offset)& 0xFF, addr);
        mem->WriteByte(addr, test_val);

        cpu->X = offset;
        cpu->A = A_val;
        cpu->Zero = 1;
        cpu->Negative = 0;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(cpu->A, expect_val);
        TS_ASSERT_EQUALS(cpu->Zero, 0);
        TS_ASSERT_EQUALS(cpu->Negative, 1);
    }

    void test_Should_ClearZero_SetNegative_WithWrap( void ) {
        const Byte A_val    = 0b10101001;
        const Byte test_val = 0b10010010;
        const Byte expect_val = A_val & test_val;
        const Word zp_addr = 0xAB;
        const Byte offset = 0xFF;
        const Byte d[] = {opcode, zp_addr};
        const Word addr = 0xBEEF;

        mem->LoadFromData(d, sizeof(d));
        mem->WriteWord((zp_addr +offset)& 0xFF, addr);
        mem->WriteByte(addr, test_val);

        cpu->X = offset;
        cpu->A = A_val;
        cpu->Zero = 1;
        cpu->Negative = 0;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(cpu->A, expect_val);
        TS_ASSERT_EQUALS(cpu->Zero, 0);
        TS_ASSERT_EQUALS(cpu->Negative, 1);
    }
};