#include <stdio.h>
#include <stdint.h>
#include <fstream>
#include <string>
#include <vector>
#include <instruction_set.hpp>
#include <chrono>
#include <thread>
#include <algorithm>


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


std::vector<Program_Node*> program_list;
bool* analysis_table = nullptr;
std::vector<std::vector<int>> jump_offsets;

void add_to_jump_offsets(int adr)
{
    for(int i = 0  ; i < jump_offsets.size(); i++)
    {
        if(jump_offsets[i][0] == adr)
        {
            jump_offsets[i][1]++;
            return;
        }
    }
    jump_offsets.push_back({adr,1});
}

int main(int argc, char *argv[])
{

    buildInstructionSetVector();

    // get cmd arguments
    //std::string arg1 = argv[1];
    std::string arg1 = "C:/Users/MatheoVIGNAUD/Documents/2600-Rewriter/Donkey Kong (USA).a26";

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

    analysis_table = new bool[size];

    // disassemble
    disassemble(rom, entry_point);

    // save analysis table in file

    //order jump_offsets 
    std::sort(jump_offsets.begin(), jump_offsets.end(), [](const std::vector<int>& a, const std::vector<int>& b) {
        return a[0] < b[0];  // Compare les premiers éléments de chaque vecteur
    });

    std::string analysis_table_str = "-----Jumps offsets-----\n\n";
    for(int i = 0 ; i < jump_offsets.size() ; i++)
    {
        analysis_table_str += "0xF" + intToHex(jump_offsets[i][0] , 3) + "   |   " + std::to_string(jump_offsets[i][1])+"\n";
    }
    analysis_table_str += "\n\n-----Analysis Map-----\n\n" ;
    for (int i = 0; i < size; i++)
    {
        analysis_table_str += analysis_table[i] ? "1" : "0" ;
        if(i % 16 == 0 && i != 0)
        {
            analysis_table_str += "\n";
        }
    }
    std::ofstream file_out("analysis_table.txt");
    file_out << analysis_table_str;
    file_out.close();


    // free memory
    delete[] analysis_table;
    

    return 0;
}

bool is_already_analyzed(uint16_t address)
{
    std::cout << "number of nodes : " << std::to_string(program_list.size()) << std::endl;
    for (int i = 0; i < program_list.size(); i++)
    {
        if (program_list[i]->get_address() <= address && program_list[i]->get_address() + program_list[i]->get_size() -1 >= address)
        {
            std::cout << "Address: 0x" << intToHex(address) << " already analyzed in node :" << std::to_string(program_list[i]->get_id()) << std::endl;
            return true;
        }
    }
    return false;
}

bool is_already_analyzed(uint16_t address, int id)
{
    for (int i = 0; i < program_list.size(); i++)
    {
        if (program_list[i]->get_address() <= address && program_list[i]->get_address() + program_list[i]->get_size() -1 >= address)
        {
            if (program_list[i]->get_id() != id)
            {
                std::cout << "Address: 0x" << intToHex(address) << " already analyzed in node :" << std::to_string(program_list[i]->get_id()) << std::endl;
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
    if (is_already_analyzed(entry_point))
    {
        std::cout << "Entry point already analyzed" << std::endl;
        return;
    }
    for (int i = 0; i < program_list.size(); i++)
    {
        if( program_list[i]->get_id() >= id)
        {
            id = program_list[i]->get_id() + 1;
        }
    }

    printf("node id: %d\n", id);

    // create new program node
    Program_Node* program_node = new Program_Node(entry_point, id);

    // add program node to program list
    program_list.push_back(program_node);

    std::cout << "program node id: " << program_node->get_id() << std::endl;

    //printf("program node address: %d\n", program_node.get_address());

    int offset = 0;
    
    Instruction* ActualInstruction = nullptr;

    bool quit = false;

    while(!is_already_analyzed(entry_point + offset, id) && entry_point + offset < rom.size() && !quit )
    {
        // get opcode
        uint8_t opcode = rom[entry_point + offset];

        if(ActualInstruction != nullptr)
        {
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

            std::string gotoadr = "";

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
                    std::cout<< "Branch ::::::::::::::::::::" << std::to_string((int8_t)rom[entry_point + offset + 1]) << std::endl;
                    int ep = entry_point + offset + ActualInstruction->get_size() + (int8_t)rom[entry_point + offset + 1];
                    add_to_jump_offsets(ep);
                    disassemble(rom , ep);
                    gotoadr = "  GOTO $0x" + intToHex(ep , 3);
                }
                else
                {
                    int ep = ( ( rom[entry_point + offset + 2] & 0xF  ) << 8 ) + rom[entry_point + offset + 1];

                    disassemble(rom,ep);
                    gotoadr = "  GOTO $0x" + intToHex(ep , 3);
                }
            }

            // get instruction
            std::string instruction = " 0xF" +  intToHex(entry_point + offset , 3)+ " |  "  + ActualInstruction->disasm(instruct_bytes);
            std::cout << instruction << std::endl ;

            // increment offset
            offset += size;

            // set program
            //std::cout << "print instruction in node : " << std::to_string(id) << std::endl ;
            program_node->set_program(program_node->get_program() + instruction + gotoadr +"\n");


            // set size
            program_node->set_size(offset);

            
            if (ActualInstruction->get_name() == "RTS" || ActualInstruction->get_name() == "JMP") {
                quit = true;
            }

           for (int i = 0; i <= size; i++)
            {
                analysis_table[entry_point + offset + i] = true;
            }
        }
        else
        {
            printf("%s\n", "Unknown opcode");
            offset++;
            program_node->set_program(program_node->get_program() + "Unknown opcode" + "\n");
            program_node->set_size( program_node->get_size() + 1 );
        }
        //_sleep(10);
    }
    std::cout << "write to file node : " << std::to_string(id) << std::endl ;
    std::string filname = "node" + std::to_string(id) + ".txt" ;
    std::ofstream file_out(filname);
    file_out << program_node->get_program();
    file_out.close();
    return;
}