#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include <fstream>
#include <string>
#include <vector>
#include <instruction_set.hpp>
#include <chrono>
#include <thread>


// ROM CONSTANTS
#define ENTRY_POINT 0x0FFC

void disassemble(std::vector<uint8_t> rom, uint16_t entry_point);
bool is_already_analyzed(uint16_t address);
bool is_already_analyzed(uint16_t address, int id);


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

    std::string program_str = program_list[0].get_program();

    // save in file
    
    

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

bool is_already_analyzed(uint16_t address, int id)
{
    for (int i = 0; i < program_list.size(); i++)
    {
        if (program_list[i].get_address() >= address && program_list[i].get_address() <= address + 2)
        {
            if (program_list[i].get_id() != id)
            {
                std::cout << "Address: " << address << " already analyzed" << std::endl;
                return true;
            }
        }
    }
    return false;
}

void disassemble(std::vector<uint8_t> rom, uint16_t entry_point)
{
    int id = 0;

    std::cout << "entry point hex : 0x" << intToHex(entry_point)<< std::endl;
    
    // check if entry point already analyzed
    for (int i = 0; i < program_list.size(); i++)
    {
        //std::cout<< "program_list_size:" << program_list.size() << std::endl;
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

    //printf("program node address: %d\n", program_node.get_address());

    int offset = 0;
    
    Instruction* ActualInstruction = nullptr;

    bool quit = false;

    while(!is_already_analyzed(entry_point + offset, id) && entry_point + offset < rom.size() || quit )
    {
        // get opcode
        uint8_t opcode = rom[entry_point + offset];

        if(ActualInstruction != nullptr)
        {
            ActualInstruction->~Instruction();
            ActualInstruction = nullptr;
        }
        

        for (int i = 0; i < Instruction_Set.size(); i++)
        {
            if (Instruction_Set[i].get_opcode() == opcode)
            {
                ActualInstruction = &Instruction_Set[i];
                break;
            }
        }

        if (ActualInstruction != nullptr)
        {
            // get size
            int size = ActualInstruction->get_size();

            std::vector<uint8_t> instruct_bytes;
            std::string instruct_str;

            for (int i = 0; i < size; i++)
            {
                instruct_str += intToHex(rom[entry_point + offset + i]) + " ";
                instruct_bytes.push_back(rom[entry_point + offset + i]);
            }

            if( ActualInstruction->get_name() == "JMP" || 
                ActualInstruction->get_name() == "BPL" || 
                ActualInstruction->get_name() == "BMI" || 
                ActualInstruction->get_name() == "BVC" || 
                ActualInstruction->get_name() == "BVS" || 
                ActualInstruction->get_name() == "BCC" || 
                ActualInstruction->get_name() == "BCS" || 
                ActualInstruction->get_name() == "BNE" || 
                ActualInstruction->get_name() == "BEQ" || 
                ActualInstruction->get_name() == "JSR"
             )
            {
                std::cout<<"Reccurtion" << std::endl;
                if(ActualInstruction->get_size() == 2)
                {
                    int ep = entry_point + offset + ActualInstruction->get_size() - rom[entry_point + offset + 1];
                    disassemble(rom , ep);
                }
                else
                {
                    int ep = ( ( rom[entry_point + offset + 2] - 0xF0  ) << 8 ) + rom[entry_point + offset + 1];
                    disassemble(rom,ep);
                }
            }

            // get instruction
            std::string instruction = ActualInstruction->disasm(instruct_bytes);
            std::cout << " 0x" << intToHex(entry_point + offset) << " |  " << instruct_str  << " |  " << instruction.c_str() << std::endl ;

            // set program
            program_node.set_program(program_node.get_program() + instruction + "\n");


            // set size
            program_node.set_size( program_node.get_size() + size);

            // increment offset
            offset += size;
        }
        else
        {
            printf("%s\n", "Unknown opcode");
            offset++;
            program_node.set_program(program_node.get_program() + "Unknown opcode" + "\n");
            program_node.set_size( program_node.get_size() + 1 );
        }
        //_sleep(5);
        /*if (ActualInstruction->get_name() == "RTI") {
            quit = true;
        }*/
    }
    std::string filname = "node" + std::to_string(id) + ".txt" ;
    std::ofstream file_out(filname);
    file_out << program_node.get_program();
    file_out.close();
}