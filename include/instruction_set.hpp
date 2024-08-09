#include <vector>
#include <string>


std::string intToHex(int n)
{
    std::string result;
    std::string hex = "0123456789ABCDEF";
    while (n != 0)
    {
        result = hex[n % 16] + result;
        n = n / 16;
    }
    return result;
}

class Instruction
{
public:
    Instruction(std::string name, uint8_t opcode, uint8_t size, std::string disasmstr)
    {
        this->name = name;
        this->opcode = opcode;
        this->size = size;
        this->disasmstr = disasmstr;
    }
    ~Instruction()
    {
        ;
    }
    std::string get_name()
    {
        return name;
    }
    uint8_t get_opcode()
    {
        return opcode;
    }
    uint8_t get_size()
    {
        return size;
    }
    uint8_t get_cycles()
    {
        return cycles;
    }
    void set_name(std::string name)
    {
        this->name = name;
    }
    void set_opcode(uint8_t opcode)
    {
        this->opcode = opcode;
    }
    void set_size(uint8_t size)
    {
        this->size = size;
    }
    std::string disasm(std::vector<uint8_t> bytes)
    {
        if (bytes.size() != size)
        {
            return "Invalid instruction size";
        }

        std::string bytes_str = "";
        if (size > 1)
        {
            if(size == 2)
            {
                bytes_str = "0x" + intToHex(bytes[1]);
            }
            else
            { 
                bytes_str = "0x" + intToHex(bytes[2]) + intToHex(bytes[1]);
            }
        }

        if(disasmstr.find("%s") != std::string::npos)
        {

            std::string disasmstr = this->disasmstr;
            disasmstr.replace(disasmstr.find("$%s"), 3, bytes_str);
            return disasmstr;
        }
        else
        {
            return disasmstr;
        }

    }
private:
    std::string name;
    uint8_t opcode;
    uint8_t size;
    uint8_t cycles;
    std::string disasmstr;
};

std::vector<Instruction> Instruction_Set = {
    // Legal opcodes
    Instruction("BRK", 0x00, 1, "BRK"),
    Instruction("ORA", 0x01, 2, "ORA (X,$%s)"),
    Instruction("ORA", 0x05, 2, "ORA $%s"),
    Instruction("ASL", 0x06, 2, "ASL $%s"),
    Instruction("PHP", 0x08, 1, "PHP"),
    Instruction("ORA", 0x09, 2, "ORA #$%s"),
    Instruction("ASL", 0x0A, 1, "ASL A"),
    Instruction("ORA", 0x0D, 3, "ORA $%s"),
    Instruction("ASL", 0x0E, 3, "ASL $%s"),
    Instruction("BPL", 0x10, 2, "BPL $%s"),
    Instruction("ORA", 0x11, 2, "ORA (Y,$%s)"),
    Instruction("ORA", 0x15, 2, "ORA $%s,X"),
    Instruction("ASL", 0x16, 2, "ASL $%s,X"),
    Instruction("CLC", 0x18, 1, "CLC"),
    Instruction("ORA", 0x19, 3, "ORA $%s,Y"),
    Instruction("ORA", 0x1D, 3, "ORA $%s,X"),
    Instruction("ASL", 0x1E, 3, "ASL $%s,X"),
    Instruction("JSR", 0x20, 3, "JSR $%s"),
    Instruction("AND", 0x21, 2, "AND (X,$%s)"),
    Instruction("BIT", 0x24, 2, "BIT $%s"),
    Instruction("AND", 0x25, 2, "AND $%s"),
    Instruction("ROL", 0x26, 2, "ROL $%s"),
    Instruction("PLP", 0x28, 1, "PLP"),
    Instruction("AND", 0x29, 2, "AND #$%s"),
    Instruction("ROL", 0x2A, 1, "ROL A"),
    Instruction("BIT", 0x2C, 3, "BIT $%s"),
    Instruction("AND", 0x2D, 3, "AND $%s"),
    Instruction("ROL", 0x2E, 3, "ROL $%s"),
    Instruction("BMI", 0x30, 2, "BMI $%s"),
    Instruction("AND", 0x31, 2, "AND (Y,$%s)"),
    Instruction("AND", 0x35, 2, "AND $%s,X"),
    Instruction("ROL", 0x36, 2, "ROL $%s,X"),
    Instruction("SEC", 0x38, 1, "SEC"),
    Instruction("AND", 0x39, 3, "AND $%s,Y"),
    Instruction("AND", 0x3D, 3, "AND $%s,X"),
    Instruction("ROL", 0x3E, 3, "ROL $%s,X"),
    Instruction("RTI", 0x40, 1, "RTI"),
    Instruction("EOR", 0x41, 2, "EOR (X,$%s)"),
    Instruction("EOR", 0x45, 2, "EOR $%s"),
    Instruction("LSR", 0x46, 2, "LSR $%s"),
    Instruction("PHA", 0x48, 1, "PHA"),
    Instruction("EOR", 0x49, 2, "EOR #$%s"),
    Instruction("LSR", 0x4A, 1, "LSR A"),
    Instruction("JMP", 0x4C, 3, "JMP $%s"),
    Instruction("EOR", 0x4D, 3, "EOR $%s"),
    Instruction("LSR", 0x4E, 3, "LSR $%s"),
    Instruction("BVC", 0x50, 2, "BVC $%s"),
    Instruction("EOR", 0x51, 2, "EOR (Y,$%s)"),
    Instruction("EOR", 0x55, 2, "EOR $%s,X"),
    Instruction("LSR", 0x56, 2, "LSR $%s,X"),
    Instruction("CLI", 0x58, 1, "CLI"),
    Instruction("EOR", 0x59, 3, "EOR $%s,Y"),
    Instruction("EOR", 0x5D, 3, "EOR $%s,X"),
    Instruction("LSR", 0x5E, 3, "LSR $%s,X"),
    Instruction("RTS", 0x60, 1, "RTS"),
    Instruction("ADC", 0x61, 2, "ADC (X,$%s)"),
    Instruction("ADC", 0x65, 2, "ADC $%s"),
    Instruction("ROR", 0x66, 2, "ROR $%s"),
    Instruction("PLA", 0x68, 1, "PLA"),
    Instruction("ADC", 0x69, 2, "ADC #$%s"),
    Instruction("ROR", 0x6A, 1, "ROR A"),
    Instruction("JMP", 0x6C, 3, "JMP ($%s)"),
    Instruction("ADC", 0x6D, 3, "ADC $%s"),
    Instruction("ROR", 0x6E, 3, "ROR $%s"),
    Instruction("BVS", 0x70, 2, "BVS $%s"),
    Instruction("ADC", 0x71, 2, "ADC (Y,$%s)"),
    Instruction("ADC", 0x75, 2, "ADC $%s,X"),
    Instruction("ROR", 0x76, 2, "ROR $%s,X"),
    Instruction("SEI", 0x78, 1, "SEI"),
    Instruction("ADC", 0x79, 3, "ADC $%s,Y"),
    Instruction("ADC", 0x7D, 3, "ADC $%s,X"),
    Instruction("ROR", 0x7E, 3, "ROR $%s,X"),
    Instruction("STA", 0x81, 2, "STA (X,$%s)"),
    Instruction("STY", 0x84, 2, "STY $%s"),
    Instruction("STA", 0x85, 2, "STA $%s"),
    Instruction("STX", 0x86, 2, "STX $%s"),
    Instruction("DEY", 0x88, 1, "DEY"),
    Instruction("TXA", 0x8A, 1, "TXA"),
    Instruction("STY", 0x8C, 3, "STY $%s"),
    Instruction("STA", 0x8D, 3, "STA $%s"),
    Instruction("STX", 0x8E, 3, "STX $%s"),
    Instruction("BCC", 0x90, 2, "BCC $%s"),
    Instruction("STA", 0x91, 2, "STA (Y,$%s)"),
    Instruction("STY", 0x94, 2, "STY $%s,X"),
    Instruction("STA", 0x95, 2, "STA $%s,X"),
    Instruction("STX", 0x96, 2, "STX $%s,Y"),
    Instruction("TYA", 0x98, 1, "TYA"),
    Instruction("STA", 0x99, 3, "STA $%s,Y"),
    Instruction("TXS", 0x9A, 1, "TXS"),
    Instruction("STA", 0x9D, 3, "STA $%s,X"),
    Instruction("LDY", 0xA0, 2, "LDY #$%s"),
    Instruction("LDA", 0xA1, 2, "LDA (X,$%s)"),
    Instruction("LDX", 0xA2, 2, "LDX #$%s"),
    Instruction("LDY", 0xA4, 2, "LDY $%s"),
    Instruction("LDA", 0xA5, 2, "LDA $%s"),
    Instruction("LDX", 0xA6, 2, "LDX $%s"),
    Instruction("TAY", 0xA8, 1, "TAY"),
    Instruction("LDA", 0xA9, 2, "LDA #$%s"),
    Instruction("TAX", 0xAA, 1, "TAX"),
    Instruction("LDY", 0xAC, 3, "LDY $%s"),
    Instruction("LDA", 0xAD, 3, "LDA $%s"),
    Instruction("LDX", 0xAE, 3, "LDX $%s"),
    Instruction("BCS", 0xB0, 2, "BCS $%s"),
    Instruction("LDA", 0xB1, 2, "LDA (Y,$%s)"),
    Instruction("LDY", 0xB4, 2, "LDY $%s,X"),
    Instruction("LDA", 0xB5, 2, "LDA $%s,X"),
    Instruction("LDX", 0xB6, 2, "LDX $%s,Y"),
    Instruction("CLV", 0xB8, 1, "CLV"),
    Instruction("LDA", 0xB9, 3, "LDA $%s,Y"),
    Instruction("TSX", 0xBA, 1, "TSX"),
    Instruction("LDY", 0xBC, 3, "LDY $%s,X"),
    Instruction("LDA", 0xBD, 3, "LDA $%s,X"),
    Instruction("LDX", 0xBE, 3, "LDX $%s,Y"),
    Instruction("CPY", 0xC0, 2, "CPY #$%s"),
    Instruction("CMP", 0xC1, 2, "CMP (X,$%s)"),
    Instruction("CPY", 0xC4, 2, "CPY $%s"),
    Instruction("CMP", 0xC5, 2, "CMP $%s"),
    Instruction("DEC", 0xC6, 2, "DEC $%s"),
    Instruction("INY", 0xC8, 1, "INY"),
    Instruction("CMP", 0xC9, 2, "CMP #$%s"),
    Instruction("DEX", 0xCA, 1, "DEX"),
    Instruction("CPY", 0xCC, 3, "CPY $%s"),
    Instruction("CMP", 0xCD, 3, "CMP $%s"),
    Instruction("DEC", 0xCE, 3, "DEC $%s"),
    Instruction("BNE", 0xD0, 2, "BNE $%s"),
    Instruction("CMP", 0xD1, 2, "CMP (Y,$%s)"),
    Instruction("CMP", 0xD5, 2, "CMP $%s,X"),
    Instruction("DEC", 0xD6, 2, "DEC $%s,X"),
    Instruction("CLD", 0xD8, 1, "CLD"),
    Instruction("CMP", 0xD9, 3, "CMP $%s,Y"),
    Instruction("CMP", 0xDD, 3, "CMP $%s,X"),
    Instruction("DEC", 0xDE, 3, "DEC $%s,X"),
    Instruction("CPX", 0xE0, 2, "CPX #$%s"),
    Instruction("SBC", 0xE1, 2, "SBC (X,$%s)"),
    Instruction("CPX", 0xE4, 2, "CPX $%s"),
    Instruction("SBC", 0xE5, 2, "SBC $%s"),
    Instruction("INC", 0xE6, 2, "INC $%s"),
    Instruction("INX", 0xE8, 1, "INX"),
    Instruction("SBC", 0xE9, 2, "SBC #$%s"),
    Instruction("NOP", 0xEA, 1, "NOP"),
    Instruction("CPX", 0xEC, 3, "CPX $%s"),
    Instruction("SBC", 0xED, 3, "SBC $%s"),
    Instruction("INC", 0xEE, 3, "INC $%s"),
    Instruction("BEQ", 0xF0, 2, "BEQ $%s"),
    Instruction("SBC", 0xF1, 2, "SBC (Y,$%s)"),
    Instruction("SBC", 0xF5, 2, "SBC $%s,X"),
    Instruction("INC", 0xF6, 2, "INC $%s,X"),
    Instruction("SED", 0xF8, 1, "SED"),
    Instruction("SBC", 0xF9, 3, "SBC $%s,Y"),
    Instruction("SBC", 0xFD, 3, "SBC $%s,X"),
    Instruction("INC", 0xFE, 3, "INC $%s,X"),
    // Illegal opcodes
    Instruction("SLO", 0x03, 2, "SLO (X,$%s)"),
    Instruction("SLO", 0x07, 2, "SLO $%s"),
    Instruction("SLO", 0x0F, 3, "SLO $%s"),
    Instruction("SLO", 0x13, 2, "SLO (Y,$%s)"),
    Instruction("SLO", 0x17, 2, "SLO $%s,X"),
    Instruction("SLO", 0x1B, 3, "SLO $%s,Y"),
    Instruction("SLO", 0x1F, 3, "SLO $%s,X"),
    Instruction("RLA", 0x23, 2, "RLA (X,$%s)"),
    Instruction("RLA", 0x27, 2, "RLA $%s"),
    Instruction("RLA", 0x2F, 3, "RLA $%s"),
    Instruction("RLA", 0x33, 2, "RLA (Y,$%s)"),
    Instruction("RLA", 0x37, 2, "RLA $%s,X"),
    Instruction("RLA", 0x3B, 3, "RLA $%s,Y"),
    Instruction("RLA", 0x3F, 3, "RLA $%s,X"),
    Instruction("SRE", 0x43, 2, "SRE (X,$%s)"),
    Instruction("SRE", 0x47, 2, "SRE $%s"),
    Instruction("SRE", 0x4F, 3, "SRE $%s"),
    Instruction("SRE", 0x53, 2, "SRE (Y,$%s)"),
    Instruction("SRE", 0x57, 2, "SRE $%s,X"),
    Instruction("SRE", 0x5B, 3, "SRE $%s,Y"),
    Instruction("SRE", 0x5F, 3, "SRE $%s,X"),
    Instruction("RRA", 0x63, 2, "RRA (X,$%s)"),
    Instruction("RRA", 0x67, 2, "RRA $%s"),
    Instruction("RRA", 0x6F, 3, "RRA $%s"),
    Instruction("RRA", 0x73, 2, "RRA (Y,$%s)"),
    Instruction("RRA", 0x77, 2, "RRA $%s,X"),
    Instruction("RRA", 0x7B, 3, "RRA $%s,Y"),
    Instruction("RRA", 0x7F, 3, "RRA $%s,X"),
    Instruction("SAX", 0x83, 2, "SAX (X,$%s)"),
    Instruction("SAX", 0x87, 2, "SAX $%s"),
    Instruction("SAX", 0x8F, 3, "SAX $%s"),
    Instruction("SAX", 0x97, 2, "SAX $%s,Y"),
    Instruction("LAX", 0xA3, 2, "LAX (X,$%s)"),
    Instruction("LAX", 0xA7, 2, "LAX $%s"),
    Instruction("LAX", 0xAF, 3, "LAX $%s"),
    Instruction("LAX", 0xB3, 2, "LAX (Y,$%s)"),
    Instruction("LAX", 0xB7, 2, "LAX $%s,Y"),
    Instruction("LAX", 0xBF, 3, "LAX $%s,Y"),
    Instruction("DCP", 0xC3, 2, "DCP (X,$%s)"),
    Instruction("DCP", 0xC7, 2, "DCP $%s"),
    Instruction("DCP", 0xCF, 3, "DCP $%s"),
    Instruction("DCP", 0xD3, 2, "DCP (Y,$%s)"),
    Instruction("DCP", 0xD7, 2, "DCP $%s,X"),
    Instruction("DCP", 0xDB, 3, "DCP $%s,Y"),
    Instruction("DCP", 0xDF, 3, "DCP $%s,X"),
    Instruction("ISC", 0xE3, 2, "ISC (X,$%s)"),
    Instruction("ISC", 0xE7, 2, "ISC $%s"),
    Instruction("ISC", 0xEF, 3, "ISC $%s"),
    Instruction("ISC", 0xF3, 2, "ISC (Y,$%s)"),
    Instruction("ISC", 0xF7, 2, "ISC $%s,X"),
    Instruction("ISC", 0xFB, 3, "ISC $%s,Y"),
    Instruction("ISC", 0xFF, 3, "ISC $%s,X")
};