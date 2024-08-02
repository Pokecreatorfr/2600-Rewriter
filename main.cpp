#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include <fstream>
#include <string>
#include <vector>
#include <instruction_set.hpp>


// ROM CONSTANTS
#define ENTRY_POINT 0x0FFC

void disassemble(std::vector<uint8_t> rom, uint16_t entry_point);

class Program_Node
{
public:
    Program_Node(uint16_t address, int id)
    {
        this->address = address;
        this->id = id;
    }
    std::string get_program()
    {
        return program;
    }
    int get_id()
    {
        return id;
    }
    uint16_t get_address()
    {
        return address;
    }
    void set_program(std::string program)
    {
        this->program = program;
    }
    void set_id(int id)
    {
        this->id = id;
    }
    void set_address(uint16_t address)
    {
        this->address = address;
    }
    void set_size(int size)
    {
        this->size = size;
    }
    int get_size()
    {
        return size;
    }
private:
    uint16_t address;
    int id;
    std::string program;
    int size = 0;
};


std::vector<Program_Node> program_list;

int main(int argc, char *argv[])
{
    // get cmd arguments
    //std::string arg1 = argv[1];
    std::string arg1 = "C:/Users/mathe/Documents/GitHub/2600-Rewriter/Donkey Kong (USA).a26";

    // print cmd arguments
    std::cout << "arg1: " << arg1 << std::endl;

    std::vector<uint8_t> rom;

    // read rom file
    FILE *file = fopen(arg1.c_str(), "rb");
    if (file == NULL)
    {
        std::cout << "Error: Could not open file" << std::endl;
        return 1;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    rom.resize(size);
    fread(rom.data(), 1, size, file);
    fclose(file);

    // print rom size
    std::cout << "rom size: " << size << std::endl;

    // read entry point
    uint16_t entry_point = rom[ENTRY_POINT] | (rom[ENTRY_POINT + 1] - 0xF0) << 8;

    // print entry point
    std::cout << "entry point: " << entry_point << std::endl;
    std::cout <<  std::to_string(rom[ENTRY_POINT])  << std::endl;
    std::cout << std::to_string(rom[ENTRY_POINT + 1] - 0xF0) << std::endl;

    // disassemble
    disassemble(rom, entry_point);
    

    return 0;
}

bool is_already_analyzed(uint16_t address)
{
    for (int i = 0; i < program_list.size(); i++)
    {
        if (program_list[i].get_address() >= address && program_list[i].get_address() <= address + 2)
        {
            std::cout << "Address: " << address << " already analyzed" << std::endl;
            return true;
        }
    }
    return false;
}

void disassemble(std::vector<uint8_t> rom, uint16_t entry_point)
{
    int id = 0;
    // check if entry point already analyzed
    for (int i = 0; i <= program_list.size(); i++)
    {
        if (is_already_analyzed(entry_point))
        {
            printf("%s\n", "Entry point already analyzed");
            return;
        }
        while( program_list[i].get_id() >= id)
        {
            id++;
        }
    }

    printf("node id: %d\n", id);

    // create new program node
    Program_Node program_node(entry_point, id);

    // add program node to program list
    program_list.push_back(program_node);

    printf("program node address: %d\n", program_node.get_address());

    int offset = 0;
    
    while(!is_already_analyzed(entry_point + offset) || entry_point + offset > rom.size())
    {
        
        // get opcode
        uint8_t opcode = rom[entry_point + offset];

        printf("offset: %d\n", offset);

        Instruction* ActualInstruction = nullptr;

        

        for (int i = 0; i < Instruction_Set.size(); i++)
        {
            if (Instruction_Set[i].get_opcode() == opcode)
            {
                ActualInstruction = &Instruction_Set[i];
                break;
            }
        }

        if (ActualInstruction == nullptr)
        {
            // get size
            int size = Instruction_Set[opcode].get_size();

            std::vector<uint8_t> instruct_bytes;
            for (int i = 0; i < size; i++)
            {
                instruct_bytes.push_back(rom[entry_point + offset + i]);
            }

            // get instruction
            std::string instruction = Instruction_Set[opcode].disasm(instruct_bytes) + "\n";
            printf("%s", instruction.c_str());

            // set program
            program_node.set_program(program_node.get_program() + instruction);


            // set size
            program_node.set_size( program_node.get_size() + size);

            // increment offset
            offset += size;
        }
        else
        {
            printf("%s\n", "Unknown opcode");
            offset++;
        }
    }
}