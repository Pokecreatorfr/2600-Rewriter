#include <instruction_set.hpp>

class Adress
{
    public:
        Adress(uint16_t adress)
        {
            this->adress = adress;
        };
        ~Adress();
        uint16_t getAdress()
        {
            return adress;
        };
        void setInstruction(Instruction* instruction)
        {
            this->instruction = instruction;
        };
        Instruction* getInstruction()
        {
            return instruction;
        };
    private:
        uint16_t adress;
        Instruction* instruction;
};

class SimulatedInstruction
{
    public:
        SimulatedInstruction(std::vector<uint8_t> bytes, Instruction* instruction, Adress* adress)
        {
            this->bytes = bytes;
            this->instruction = instruction;
            this->adress = adress;
        };
        ~SimulatedInstruction();
        std::vector<uint8_t> getBytes()
        {
            return bytes;
        };
        Instruction* getInstruction()
        {
            return instruction;
        };
        Adress* getAdress()
        {
            return adress;
        };
        Adress* getReadAdress()
        {
            return readAdress;
        };
        Adress* getWriteAdress()
        {
            return writeAdress;
        };
        std::vector<Adress*> getNextAdresses()
        {
            return nextAdresses;
        };
        void setReadAdress(Adress* readAdress)
        {
            this->readAdress = readAdress;
        };
        void setWriteAdress(Adress* writeAdress)
        {
            this->writeAdress = writeAdress;
        };
        void addReadAdress(Adress* readAdress)
        {
            nextAdresses.push_back(readAdress);
        };
    private:
        std::vector<uint8_t> bytes;
        Instruction* instruction = nullptr;
        Adress* adress= nullptr;
        Adress* readAdress = nullptr;
        Adress* writeAdress = nullptr;
        std::vector<Adress*> nextAdresses;
};