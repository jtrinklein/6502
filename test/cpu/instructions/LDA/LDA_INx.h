#include <cxxtest/TestSuite.h>
#include <cpu.h>
#include <mem.h>

class LDA_Indirect_X_Tests : public CxxTest::TestSuite 
{
public:
    CPU*  cpu;
    Mem* mem;
    static constexpr Byte opcode = 0xA1;
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

    void test_WithZero( void ) {
        const Byte test_val = 0x00;
        const Byte offset = 0x02;
        const Word addr = 0x21FF;
        const Byte d[] = {
            opcode, offset,
            0x02, 0x00,
            0,0, // for address
            0xFF
        };
        mem->LoadFromData(d, sizeof(d));
        mem->WriteWord(0x0004, addr);

        mem->WriteByte(addr-1, 0xff);
        mem->WriteByte(addr, test_val);
        mem->WriteByte(addr+1, 0xff);

        const Word pc_start = 0x0000;
        cpu->PC = pc_start;
        cpu->X = 0x02;
        cpu->A = 0x11;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(cpu->A, test_val);
        TS_ASSERT_EQUALS(cpu->Zero, 1);
        TS_ASSERT_EQUALS(cpu->Negative, 0);
        TS_ASSERT_EQUALS(cpu->X, 0x02);
    }

    void test_WithZeroAndAddressWrap( void ) {
        const Byte test_val = 0x00;
        const Byte offset = 0xFF;
        const Word addr = 0x1234;
        const Byte d[] = {
            opcode, offset,
            0x02, 0x00,
            0,0, // for address
            0xFF
        };
        mem->LoadFromData(d, sizeof(d));
        
        mem->WriteWord(0x0004, addr); // correct rollover address should be 0x0004, write the address there
        mem->WriteWord(addr, test_val);

        const Word bad_address = 0x4321;
        mem->WriteWord(0x0104, bad_address); // 0xFF + 0x05 = 0x104, write bad address to 0x0104
        mem->WriteByte(bad_address, 0xFF);

        const Word pc_start = 0x0000;
        cpu->PC = pc_start;
        cpu->X = 0x05;
        cpu->A = 0x11;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->A, test_val);
        TS_ASSERT_EQUALS(cpu->Zero, 1);
        TS_ASSERT_EQUALS(cpu->Negative, 0);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(cpu->X, 0x05);
    }

    void test_WithNonZero( void ) {
        const Byte test_val = 0x67;
        const Byte offset = 0x04;
        const Word addr = 0x6578;
        const Byte d[] = {
            opcode, offset,
            0x02, 0x01, 0xFF
        };
        mem->LoadFromData(d, sizeof(d));
        mem->WriteWord(0x00C4, addr);
        mem->WriteByte(addr, test_val);

        cpu->PC = 0x0000;
        cpu->X = 0xC0;
        cpu->A = 0x00;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->A, test_val);
        TS_ASSERT_EQUALS(cpu->Zero, 0);
        TS_ASSERT_EQUALS(cpu->Negative, 0);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(cpu->X, 0xC0);
    }

    void test_WithNegative( void ) {
        const Byte test_val = 0xE7;
        const Byte offset = 0x12;
        const Byte X_val = 0x1F;
        const Word addr = 0x6578;
        const Byte d[] = {
            opcode, offset,
            0x03, 0x04, 0xFF
        };
        mem->LoadFromData(d, sizeof(d));
        mem->WriteWord(0x0000 + offset + X_val, addr);
        mem->WriteByte(addr, test_val);

        cpu->PC = 0x0000;
        cpu->X = X_val;
        cpu->A = 0x00;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->A, test_val);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(cpu->Zero, 0);
        TS_ASSERT_EQUALS(cpu->Negative, 1);
        TS_ASSERT_EQUALS(cpu->X, X_val);
    }
    
};