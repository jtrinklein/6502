#include "mem.h"
#include <fstream>
#include <iostream>

Mem::Mem() : m_data(nullptr), m_log_enabled(false) {

}

Mem::~Mem() {
    Unload();
}


void Mem::LoadFromFile(std::string path) {
    Unload();
    
    std::ifstream memfile;
    m_data = new Byte[max_mem_size];
    memfile.open(path, std::ios_base::binary | std::ios_base::in);
    memfile.read((char*)m_data, max_mem_size);
    memfile.close();
}

void Mem::LoadFromData(const Byte* data, size_t byteCount) {
    Unload();
    m_data = new Byte[max_mem_size];
    auto copyCount = byteCount < max_mem_size ? byteCount : max_mem_size;
    memcpy(m_data, data, copyCount);
}

void Mem::LoadFromDataAtOffset(const Byte* data, size_t byteCount, size_t offset) {
    Unload();

    m_data = new Byte[max_mem_size];

    auto copyCount = (byteCount + offset) < max_mem_size ? byteCount : max_mem_size - offset;
    memcpy(m_data+offset, data, copyCount);
}

void Mem::Unload() {
    if (m_data != nullptr) {
        delete[] m_data;
        m_data = nullptr;
    }
}


Byte Mem::ReadByteInternal(Word addr) {
    return m_data ? m_data[addr] : 0x0;
}
Byte Mem::ReadByte(Word addr) {
    Byte val = ReadByteInternal(addr);
    
    if (m_log_enabled) printf("read Byte (%02X) from addr: 0x%04X\n", val, addr);
    return val;
}

Byte Mem::WriteByteInternal(Word addr, Byte data) {
    if (!m_data) {
        return 0x0;
    }
    m_data[addr] = data;
}
Byte Mem::WriteByte(Word addr, Byte data) {
    WriteByteInternal(addr, data);
    if (m_log_enabled) printf("wrote Byte (%02X) to addr: 0x%04X\n", data, addr);
    return data;
}

Word Mem::ReadWord(Word addr) {
    if (!m_data) {
        return 0x00;
    }
    Byte b1 = ReadByteInternal(addr),
        b2 = ReadByteInternal(addr+1);
    Word val = (b2 << 8) | b1;
    if (m_log_enabled) printf("read Word (%04X) from addr: 0x%04X\n", val, addr);
    return val;
}

Word Mem::WriteWord(Word addr, Word data) {
    if (!m_data) {
        return 0x0;
    }
    Byte low(data & 0xFF), high((data >> 8) & 0xFF);
    WriteByteInternal(addr, low);
    WriteByteInternal(addr+1, high);
    
    if (m_log_enabled) printf("wrote Word (%04X) to addr: 0x%04X\n", data, addr);
    return data;
}
