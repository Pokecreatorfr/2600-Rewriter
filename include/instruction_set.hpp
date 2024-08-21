#include <iostream>
#include <vector>
#include <string>
#include <nlohmann/json.hpp>



struct Mem2600
{
    uint8_t A;
    uint8_t X;
    uint8_t Y;
    bool N;
    bool V;
    bool Z;
    bool C;
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
        if(y == )
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
        // n'utiliser cette methode que pour les instructions ou il y a un saut
        // exemple : JMP, BCC, BCS, BEQ, BMI, BNE, BPL, BVC, BVS
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
            return (memory.RAM[(bytes[2] << 8) + bytes[1] + 1] << 8) + memory.RAM[(bytes[2] << 8) + bytes[1]];
            break;
        case IndirectX:
            return (memory.RAM[(bytes[1] + memory.X + 1) % 0x100] << 8) + memory.RAM[(bytes[1] + memory.X)];
            break;
        case IndirectY:
            return (memory.RAM[(bytes[1] + 1) % 0x100] << 8) + memory.RAM[bytes[1]] + memory.Y;
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
    void execute(std::vector<uint8_t> bytes, Mem2600 &memory)
    {
        switch (this->opcode)
        {
            case "ADC":

                break;
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