#include <iostream>
#include <vector>
#include <string>
#include <nlohmann/json.hpp>


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