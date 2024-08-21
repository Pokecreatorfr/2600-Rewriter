#include <iostream>
#include <vector>
#include <string>
#include <nlohmann/json.hpp>



struct Mem2600
{
    uint8_t A;
    uint8_t X;
    uint8_t Y;
    bool C;
    bool Z;
    bool I;
    bool D;
    bool B;
    // inused bit
    bool V;
    bool N;
    uint16_t PC;
    uint8_t SP;
    uint8_t TIA[64];
    uint8_t RAM[128];
    uint8_t RIOT[32];
    std::vector<uint8_t> ROM;
};

uint8_t readMemory(Mem2600 &memory, uint16_t adress)
{
    int x = adress & 0xF000 >> 12;
    int y = adress & 0x0F00 >> 8;
    int z = adress & 0x00F0 >> 4;
    if(x%2 == 1)
    {
        return memory.ROM[adress & 0x0FFF];
    }
    else
    {
        if(z <= 0x0 && z < 0x8)
        {
            return memory.TIA[adress & 0x003F];
        }
        else if(
            y == 0x2 || y == 0x3 || y == 0x6 || y == 0x7 || y == 0xA || y == 0xB || y == 0xE || y == 0xF
        )
        {
            return memory.RIOT[adress & 0x001F];
        }
        else
        {
            return memory.RAM[adress & 0x007F];
        }
    }
}

bool writeMemory(Mem2600 &memory, uint16_t adress, uint8_t value)
{
    int x = adress & 0xF000 >> 12;
    int y = adress & 0x0F00 >> 8;
    int z = adress & 0x00F0 >> 4;
    if(x%2 == 1)
    {
        return false;
    }
    else
    {
        if(z <= 0x0 && z < 0x8)
        {
            memory.TIA[adress & 0x003F] = value;
            return true;
        }
        else if(
            y == 0x2 || y == 0x3 || y == 0x6 || y == 0x7 || y == 0xA || y == 0xB || y == 0xE || y == 0xF
        )
        {
            memory.RIOT[adress & 0x001F] = value;
            return true;
        }
        else
        {
            memory.RAM[adress & 0x007F] = value;
            return true;
        }
    }
}


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

std::string intToHex(int n , int min_digit)
{
    std::string result;
    std::string hex = "0123456789ABCDEF";
    while (n != 0)
    {
        result = hex[n % 16] + result;
        n = n / 16;
    }
    while(result.size() < min_digit)
    {
        result = "0" + result;
    }
    return result;
}

enum AdressMode {
    Immediate,
    ZeroPage,
    ZeroPageX,
    ZeroPageY,
    Absolute,
    AbsoluteX,
    AbsoluteY,
    Indirect,
    IndirectX,
    IndirectY,
    Relative,
    Implied,
    Accumulator,
};

class Instruction
{
public:
    Instruction(std::string name, uint8_t opcode, uint8_t size, AdressMode adressMode)
    {
        this->name = name;
        this->opcode = opcode;
        this->size = size;
        this->adressMode = adressMode;
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
        std::string dasm = this->name + " ";
        switch (this->adressMode)
        {
        case Immediate:
            dasm += "#" + intToHex(bytes[1] , 2);
            break;
        case ZeroPage:
            dasm += intToHex(bytes[1] , 2);
            break;
        case ZeroPageX:
            dasm += intToHex(bytes[1] , 2) + ",X";
            break;
        case ZeroPageY:
            dasm += intToHex(bytes[1] , 2) + ",Y";
            break;
        case Absolute:
            dasm += intToHex(bytes[2] , 2) + intToHex(bytes[1] , 2);
            break;
        case AbsoluteX:
            dasm += intToHex(bytes[2] , 2) + intToHex(bytes[1] , 2) + ",X";
            break;
        case AbsoluteY:
            dasm += intToHex(bytes[2] , 2) + intToHex(bytes[1] , 2) + ",Y";
            break;
        case Indirect:
            dasm += + "(" + intToHex(bytes[2], 2) + intToHex(bytes[1] , 2) + ")";
            break;
        case IndirectX:
            dasm += + "(" + intToHex(bytes[1] , 2) + ",X)";
            break;
        case IndirectY:
            dasm += + "(" + intToHex(bytes[1] , 2) + "),Y";
            break;
        case Relative:
            dasm += intToHex(bytes[1] , 2);
            break;
        case Implied:
            /* code */
            break;
        case Accumulator:
            dasm += "A";
            break; 

        default:
            break;
        }
        return dasm;
    }
    uint16_t get_adress(std::vector<uint8_t> bytes, Mem2600 &memory)
    {
        switch (this->adressMode)
        {
        case ZeroPage:
            return (memory.PC & 0xFF00) + bytes[1];
            break;
        case ZeroPageX:
            return (memory.PC & 0xFF00) + (bytes[1] + memory.X) % 0x100;
            break;
        case ZeroPageY:
            return (memory.PC & 0xFF00) + (bytes[1] + memory.Y) % 0x100;
            break;
        case Absolute:
            return (bytes[2] << 8) + bytes[1];
            break;
        case AbsoluteX:
            return (bytes[2] << 8) + bytes[1] + memory.X;
            break;
        case AbsoluteY:
            return (bytes[2] << 8) + bytes[1] + memory.Y;
            break;
        case Indirect:
            return (readMemory(memory, (bytes[2] << 8) + bytes[1] + 1) << 8) + readMemory(memory, (bytes[2] << 8) + bytes[1]);
            break;
        case IndirectX:
            return (readMemory(memory, (bytes[1] + memory.X + 1) % 0x100) << 8) + readMemory(memory, (bytes[1] + memory.X) % 0x100);
            break;
        case IndirectY:
            return (readMemory(memory, bytes[1] + 1) << 8) + readMemory(memory, bytes[1]) + memory.Y;
            break;
        case Relative:
            return memory.PC + (int8_t)bytes[1];
            break;
        case Implied:
            break;
        case Accumulator:
            break;
        default:
            break;
        };
        return 0;
    }
    uint8_t getValue(std::vector<uint8_t> bytes, Mem2600 &memory)
    {
        uint8_t value = 0;
        if (this->adressMode == Immediate)
        {
            value = bytes[1];
        }
        else if (this->adressMode == Accumulator)
        {
            value = memory.A;
        }
        else
        {
            value = readMemory(memory, get_adress(bytes, memory));
        }
    }
    void execute(std::vector<uint8_t> bytes, Mem2600 &memory)
    {

        // Fait à l'aide de http://6502.org/tutorials/6502opcodes.html

        if (this->name == "ADC")
        {
            uint8_t value = getValue(bytes, memory);

            if (memory.D)
            {
                uint16_t result = memory.A + value + memory.C;
                memory.C = result > 0x99;
                memory.V = (~(memory.A ^ value) & (memory.A ^ result) & 0x80) != 0;
                memory.N = (result & 0x80) != 0;
                memory.Z = (result & 0xFF) == 0;
                if ((memory.A & 0xF) + (value & 0xF) + memory.C > 0x9)
                {
                    result += 0x6;
                }
                memory.A = result;
            }
            else
            {
                uint16_t result = memory.A + value + memory.C;
                memory.C = result > 0xFF;
                memory.V = (~(memory.A ^ value) & (memory.A ^ result) & 0x80) != 0;
                memory.N = (result & 0x80) != 0;
                memory.Z = (result & 0xFF) == 0;
                memory.A = result;
            }
        }

        else if(this->name == "AND")
        {
            uint8_t value = getValue(bytes, memory);
            memory.A &= value;
            memory.N = (memory.A & 0x80) != 0;
            memory.Z = memory.A == 0;
        }

        else if(this->name == "ASL")
        {
            uint8_t value = getValue(bytes, memory);
            memory.C = (value & 0x80) != 0;
            value <<= 1;
            memory.N = (value & 0x80) != 0;
            memory.Z = value == 0;
            if(this->adressMode == Accumulator)
            {
                memory.A = value;
            }
            else
            {
                writeMemory(memory, get_adress(bytes, memory), value);
            }
        }

        else if(this->name == "BIT")
        {
            uint8_t value = getValue(bytes, memory);
            memory.V = (value & 0x40) != 0;
            memory.N = (value & 0x80) != 0;
            memory.Z = (memory.A & value) == 0;
        }


        // Branch instruction
        else if (this->name == "BCC")
        {
            if (memory.C == 0)
            {
                memory.PC = get_adress(bytes, memory);
            }
        }
        else if (this->name == "BCS")
        {
            if (memory.C == 1)
            {
                memory.PC = get_adress(bytes, memory);
            }
        }
        else if (this->name == "BEQ")
        {
            if (memory.Z == 1)
            {
                memory.PC = get_adress(bytes, memory);
            }
        }
        else if (this->name == "BMI")
        {
            if (memory.N == 1)
            {
                memory.PC = get_adress(bytes, memory);
            }
        }
        else if (this->name == "BNE")
        {
            if (memory.Z == 0)
            {
                memory.PC = get_adress(bytes, memory);
            }
        }
        else if (this->name == "BPL")
        {
            if (memory.N == 0)
            {
                memory.PC = get_adress(bytes, memory);
            }
        }
        else if (this->name == "BVC")
        {
            if (memory.V == 0)
            {
                memory.PC = get_adress(bytes, memory);
            }
        }
        else if (this->name == "BVS")
        {
            if (memory.V == 1)
            {
                memory.PC = get_adress(bytes, memory);
            }
        }

        else if(this->name == "BRK")
        {
            memory.B = 1;
        }

        else if(this->name == "CMP")
        {
            uint8_t value = getValue(bytes, memory);
            uint16_t result = memory.A - value;
            memory.C = memory.A >= value;
            memory.N = (result & 0x80) != 0;
            memory.Z = (result & 0xFF) == 0;
        }
        
        else if(this->name == "CPX")
        {
            uint8_t value = getValue(bytes, memory);
            uint16_t result = memory.X - value;
            memory.C = memory.X >= value;
            memory.N = (result & 0x80) != 0;
            memory.Z = (result & 0xFF) == 0;
        }

        else if(this->name == "CPY")
        {
            uint8_t value = getValue(bytes, memory);
            uint16_t result = memory.Y - value;
            memory.C = memory.Y >= value;
            memory.N = (result & 0x80) != 0;
            memory.Z = (result & 0xFF) == 0;
        }

        else if(this->name == "DEC")
        {
            uint8_t value = getValue(bytes, memory);
            value--;
            memory.N = (value & 0x80) != 0;
            memory.Z = value == 0;
            writeMemory(memory, get_adress(bytes, memory), value);
        }

        else if(this->name == "EOR")
        {
            uint8_t value = getValue(bytes, memory);
            memory.A ^= value;
            memory.N = (memory.A & 0x80) != 0;
            memory.Z = memory.A == 0;
        }

                // Flags Instructions
        else if (this->name == "BRK")
        {
            memory.I = 1;
        }
        else if (this->name == "RTI")
        {
            memory.I = 1;
        }
        else if (this->name == "PHP")
        {
            memory.I = 1;
        }
        else if (this->name == "PLP")
        {
            memory.I = 1;
        }
        else if (this->name == "PHA")
        {
            memory.I = 1;
        }
        else if (this->name == "PLA")
        {
            memory.I = 1;
        }
        else if (this->name == "DEY")
        {
            memory.I = 1;
        }
        else if (this->name == "TAY")
        {
            memory.I = 1;
        }
        else if (this->name == "INY")
        {
            memory.I = 1;
        }
        else if (this->name == "INX")
        {
            memory.I = 1;
        }
        else if (this->name == "CLC")
        {
            memory.I = 1;
        }
        else if (this->name == "SEC")
        {
            memory.I = 1;
        }
        else if (this->name == "CLI")
        {
            memory.I = 1;
        }
        else if (this->name == "SEI")
        {
            memory.I = 1;
        }
        else if (this->name == "CLV")
        {
            memory.I = 1;
        }
        else if (this->name == "CLD")
        {
            memory.I = 1;
        }
        else if (this->name == "SED")
        {
            memory.I = 1;
        }
        else if (this->name == "CLC")
        {
            memory.C = 0;
        }
        else if (this->name == "SEC")
        {
            memory.C = 1;
        }
        else if (this->name == "CLI")
        {
            memory.I = 0;
        }
        else if (this->name == "SEI")
        {
            memory.I = 1;
        }
        else if (this->name == "CLV")
        {
            memory.V = 0;
        }
        else if (this->name == "CLD")
        {
            memory.D = 0;
        }
        else if (this->name == "SED")
        {
            memory.D = 1;
        }


        else if(this->name == "INC")
        {
            uint8_t value = getValue(bytes, memory);
            value++;
            memory.N = (value & 0x80) != 0;
            memory.Z = value == 0;
            writeMemory(memory, get_adress(bytes, memory), value);
        }

        // Jump instruction
        else if (this->name == "JMP")
        {
            memory.PC = get_adress(bytes, memory);
        }

        // Jump instruction
        else if (this->name == "JSR")
        {
            memory.PC -= 1;
            writeMemory(memory, 0x100 + memory.SP, (memory.PC & 0xFF00) >> 8);
            memory.SP--;
            writeMemory(memory, 0x100 + memory.SP, memory.PC & 0x00FF);
            memory.SP--;
            memory.PC = get_adress(bytes, memory);
        }

        else if (this->name == "LDA")
        {
            memory.A = getValue(bytes, memory);
            memory.N = (memory.A & 0x80) != 0;
            memory.Z = memory.A == 0;
        }

        else if (this->name == "LDX")
        {
            memory.X = getValue(bytes, memory);
            memory.N = (memory.X & 0x80) != 0;
            memory.Z = memory.X == 0;
        }

        else if (this->name == "LDY")
        {
            memory.Y = getValue(bytes, memory);
            memory.N = (memory.Y & 0x80) != 0;
            memory.Z = memory.Y == 0;
        }

        else if (this->name == "LSR")
        {
            uint8_t value = getValue(bytes, memory);
            memory.C = (value & 0x01) != 0;
            value >>= 1;
            memory.N = (value & 0x80) != 0;
            memory.Z = value == 0;
            if (this->adressMode == Accumulator)
            {
                memory.A = value;
            }
            else
            {
                writeMemory(memory, get_adress(bytes, memory), value);
            }
        }
        
        else if (this->name == "NOP")
        {

        }

        else if (this->name == "ORA")
        {
            uint8_t value = getValue(bytes, memory);
            memory.A |= value;
            memory.N = (memory.A & 0x80) != 0;
            memory.Z = memory.A == 0;
        }

        else if (this->name == "TAX")
        {
            memory.X = memory.A;
            memory.N = (memory.X & 0x80) != 0;
            memory.Z = memory.X == 0;
        }

        else if (this->name == "TXA")
        {
            memory.A = memory.X;
            memory.N = (memory.A & 0x80) != 0;
            memory.Z = memory.A == 0;
        }

        else if (this->name == "DEX")
        {
            memory.X--;
            memory.N = (memory.X & 0x80) != 0;
            memory.Z = memory.X == 0;
        }

        else if (this->name == "INX")
        {
            memory.X++;
            memory.N = (memory.X & 0x80) != 0;
            memory.Z = memory.X == 0;
        }

        else if (this->name == "TAY")
        {
            memory.Y = memory.A;
            memory.N = (memory.Y & 0x80) != 0;
            memory.Z = memory.Y == 0;
        }

        else if (this->name == "TYA")
        {
            memory.A = memory.Y;
            memory.N = (memory.A & 0x80) != 0;
            memory.Z = memory.A == 0;
        }

        else if (this->name == "DEY")
        {
            memory.Y--;
            memory.N = (memory.Y & 0x80) != 0;
            memory.Z = memory.Y == 0;
        }

        else if (this->name == "INY")
        {
            memory.Y++;
            memory.N = (memory.Y & 0x80) != 0;
            memory.Z = memory.Y == 0;
        }

        else if (this->name == "ROL")
        {
            uint8_t value = getValue(bytes, memory);
            uint8_t oldC = memory.C;
            memory.C = (value & 0x80) != 0;
            value <<= 1;
            value += oldC;
            memory.N = (value & 0x80) != 0;
            memory.Z = value == 0;
            if (this->adressMode == Accumulator)
            {
                memory.A = value;
            }
            else
            {
                writeMemory(memory, get_adress(bytes, memory), value);
            }
        }

        else if (this->name == "ROR")
        {
            uint8_t value = getValue(bytes, memory);
            uint8_t oldC = memory.C;
            memory.C = (value & 0x01) != 0;
            value >>= 1;
            value += oldC << 7;
            memory.N = (value & 0x80) != 0;
            memory.Z = value == 0;
            if (this->adressMode == Accumulator)
            {
                memory.A = value;
            }
            else
            {
                writeMemory(memory, get_adress(bytes, memory), value);
            }
        }

        else if (this->name == "RTI")
        {
            memory.SP++;
            memory.PC = readMemory(memory, 0x100 + memory.SP);
            memory.SP++;
            memory.PC += readMemory(memory, 0x100 + memory.SP) << 8;
            memory.SP++;
            memory.PC += 1;
        }

        else if (this->name == "RTS")
        {
            memory.SP++;
            memory.PC = readMemory(memory, 0x100 + memory.SP);
            memory.SP++;
            memory.PC += readMemory(memory, 0x100 + memory.SP) << 8;
        }

        else if (this->name == "SBC")
        {
            uint8_t value = getValue(bytes, memory);
            if (memory.D)
            {
                uint16_t result = memory.A - value - (1 - memory.C);
                memory.C = result < 0x100;
                memory.V = ((memory.A ^ result) & (memory.A ^ value) & 0x80) != 0;
                memory.N = (result & 0x80) != 0;
                memory.Z = (result & 0xFF) == 0;
                if ((memory.A & 0xF) - (value & 0xF) - (1 - memory.C) < 0)
                {
                    result -= 0x6;
                }
                memory.A = result;
            }
            else
            {
                uint16_t result = memory.A - value - (1 - memory.C);
                memory.C = result < 0x100;
                memory.V = ((memory.A ^ result) & (memory.A ^ value) & 0x80) != 0;
                memory.N = (result & 0x80) != 0;
                memory.Z = (result & 0xFF) == 0;
                memory.A = result;
            }
        }
        
        else if (this->name == "STA")
        {
            writeMemory(memory, get_adress(bytes, memory), memory.A);
        }

        // Stack Instructions
        else if (this->name == "TSX")
        {
            memory.SP = memory.X;
        }

        else if (this->name == "TSY")
        {
            memory.SP = memory.Y;
        }

        else if (this->name == "PHA")
        {
            writeMemory(memory, 0x100 + memory.SP, memory.A);
        }

        else if (this->name == "PLA")
        {
            memory.A = readMemory(memory, 0x100 + memory.SP);
        }

        else if (this->name == "PHP")
        {
            uint8_t flags = 0;
            flags |= memory.C << 0;
            flags |= memory.Z << 1;
            flags |= memory.I << 2;
            flags |= memory.D << 3;
            flags |= memory.B << 4;
            flags |= 0x1 << 5;
            flags |= memory.V << 6;
            flags |= memory.N << 7;

            writeMemory(memory, 0x100 + memory.SP, flags);
            

        }

        else if (this->name == "PLP")
        {
            uint8_t flags = readMemory(memory, 0x100 + memory.SP);
            memory.C = flags & 0x1;
            memory.Z = flags & 0x2;
            memory.I = flags & 0x4;
            memory.D = flags & 0x8;
            memory.B = flags & 0x10;
            memory.V = flags & 0x40;
            memory.N = flags & 0x80;
        }

        else if (this->name == "STX")
        {
            writeMemory(memory, get_adress(bytes, memory), memory.X);
        }

        else if (this->name == "STY")
        {
            writeMemory(memory, get_adress(bytes, memory), memory.Y);
        }

    }
private:
    std::string name;
    uint8_t opcode;
    uint8_t size;
    uint8_t cycles;
    AdressMode adressMode;
};

std::vector<Instruction> Instruction_Set;

bool buildInstructionSetVector()
{
    // Chemin du fichier JSON
    std::string filename = "6502_instructions.json";

    // Flux de fichier pour lire le JSON
    std::ifstream file(filename);

    // Vérification si le fichier s'est ouvert correctement
    if (!file.is_open()) {
        std::cerr << "Erreur: Impossible d'ouvrir le fichier " << filename << std::endl;
        return 1;
    }

    // JSON à parser
    nlohmann::json jsonData = nlohmann::json::parse(file);

    // Fermer le fichier
    file.close();
    
    for(int i = 0 ; i < jsonData.size() ; i++)
    {
        std::string name = jsonData[i]["name"];
        std::string opcode_str = jsonData[i]["opcode"].get<std::string>().substr(1);
        int opcode = std::stoi(opcode_str , 0 , 16);
        int size = std::stoi(jsonData[i]["bytes"].get<std::string>());
        AdressMode adressMode;
        std::string mode = jsonData[i]["mode"];
        if (mode == "Immediate") {
            adressMode = Immediate;
        } else if (mode == "ZeroPage") {
            adressMode = ZeroPage;
        } else if (mode == "ZeroPage,X") {
            adressMode = ZeroPageX;
        } else if (mode == "ZeroPage,Y") {
            adressMode = ZeroPageY;
        } else if (mode == "Absolute") {
            adressMode = Absolute;
        } else if (mode == "Absolute,X") {
            adressMode = AbsoluteX;
        } else if (mode == "Absolute,Y") {
            adressMode = AbsoluteY;
        } else if (mode == "Indirect") {
            adressMode = Indirect;
        } else if (mode == "(Indirect,X)") {
            adressMode = IndirectX;
        } else if (mode == "(Indirect),Y") {
            adressMode = IndirectY;
        } else if (mode == "Relative") {
            adressMode = Relative;
        } else if (mode == "Implied") {
            adressMode = Implied;
        }else if (mode == "Accumulator") {
            adressMode = Accumulator;
        } else {
            std::cerr << "Error: Unknown addressing mode" << std::endl;
            return 1;
        }

        Instruction_Set.push_back(Instruction(name, opcode, size, adressMode));
    }



    return 0;
}