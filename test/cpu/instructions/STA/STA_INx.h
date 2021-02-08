#include <cxxtest/TestSuite.h>
#include <cpu.h>
#include <mem.h>

class STA_Indirect_X_Tests : public CxxTest::TestSuite 
{
public:
    CPU*  cpu;
    Mem* mem;
    static constexpr Byte opcode = 0x81;
    static constexpr Byte op_size = 2;
    static constexpr Byte op_cycles = 6;

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
        const Byte offset = 0x02;
        const Word addr = 0x21FF;
        const Byte zp_addr = 0x7A;
        const Byte d[] = { opcode, zp_addr, 0xFF };
        mem->LoadFromData(d, sizeof(d));
        mem->WriteWord(0x0000 + offset + zp_addr, addr);

        mem->WriteByte(addr-1, 0xff);
        mem->WriteByte(addr,   0xff);
        mem->WriteByte(addr+1, 0xff);

        cpu->X = offset;
        cpu->A = test_val;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(mem->ReadByte(addr), test_val);
    }    
    void test_WithNonZero( void ) {
        const Byte test_val = 0xf8;
        const Byte offset = 0xC8;
        const Byte zp_addr = 0x04;
        const Word addr = 0x81FF;
        const Byte d[] = { opcode, zp_addr, 0xFF };
        mem->LoadFromData(d, sizeof(d));
        mem->WriteWord(0x0000 + offset + zp_addr, addr);

        mem->WriteByte(addr-1, 0);
        mem->WriteByte(addr,   0);
        mem->WriteByte(addr+1, 0);

        cpu->X = offset;
        cpu->A = test_val;

        auto cycles = cpu->RunOneInstruction();

        TS_ASSERT_EQUALS(cycles, op_cycles);
        TS_ASSERT_EQUALS(cpu->PC, op_size);
        TS_ASSERT_EQUALS(mem->ReadByte(addr), test_val);
    }    
};