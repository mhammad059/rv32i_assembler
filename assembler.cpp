#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <map>

class Inst;
enum type {
    R, I, S, B, U, J
};
int inst_addr = 0;
std::vector<std::string> dest_label;
std::vector<int> dest_addr;
std::vector<Inst*> instPTRS;

// inst map
std::map <std::string, uint32_t> instMAP = {
    {"lb", I}, {"lh", I}, {"lw", I}, {"lbu", I}, {"lhu", I}, {"addi", I}, {"slli", I}, {"slti", I}, {"sltiu", I}, {"xori", I},
    {"srli", I}, {"srai", I}, {"ori", I}, {"andi", I}, {"auipc", U}, {"sb", S}, {"sh", S}, {"sw", S}, {"add", R}, {"sub", R},
    {"sll", R}, {"slt", R}, {"sltu", R}, {"xor", R}, {"srl", R}, {"sra", R}, {"or", R}, {"and", R}, {"lui", U}, {"beq", B},
    {"bne", B}, {"blt", B}, {"bge", B}, {"bltu", B}, {"bgeu", B}, {"jalr", I}, {"jal", J}

};

// register map
std::map <std::string, uint32_t> reg = {

    // original
    {"x0", 0}, {"x1", 1}, {"x2", 2},{"x3", 3}, {"x4", 4}, {"x5", 5},{"x6", 6}, {"x7", 7}, {"x8", 8},{"x9", 9}, 
    {"x10", 10}, {"x11", 11}, {"x12", 12},{"x13", 13}, {"x14", 14}, {"x15", 15},{"x16", 16}, {"x17", 17}, {"x18", 18},{"x19", 19},
    {"x20", 20}, {"x21", 21}, {"x22", 22},{"x23", 23}, {"x24", 24}, {"x25", 25},{"x26", 26}, {"x27", 27}, {"x28", 28},{"x29", 29},
    {"x30", 30}, {"x31", 31},

    // ABI
    {"zero", 0}, {"ra", 1}, {"sp", 2},{"gp", 3}, {"tp", 4}, {"t0", 5},{"t1", 6}, {"t2", 7}, {"s0", 8},{"s1", 9}, 
    {"a0", 10}, {"a1", 11}, {"a2", 12},{"a3", 13}, {"a4", 14}, {"a5", 15},{"a6", 16}, {"a7", 17}, {"s2", 18},{"s3", 19},
    {"s4", 20}, {"s5", 21}, {"s6", 22},{"s7", 23}, {"s8", 24}, {"s9", 25},{"s10", 26}, {"s11", 27}, {"t3", 28},{"t4", 29},
    {"t5", 30}, {"t6", 31}
};

// remove extra helper characters from string
std::string remove_helper_characters(std::string inpString){
    for(int i = 0; i < inpString.length(); i++)
        if(inpString[i] == ',' || inpString[i] == '(' || inpString[i] == ')') inpString[i] = ' ';
    return inpString;
}

// returns values of space seperated string based on index
std::string space_seperated_string(std::string inpString, int index){
    int str_start;
    int str_end;

    for(int i = 0; i < inpString.length(); i++){
        str_start = i; // store substring starting

        while(inpString[i] != ' '){
            i++;
            if(inpString[i] == ' ' || inpString[i] == '\n' || inpString[i] == '\0'){
                str_end = i; // store substring ending
                index--;
                break;
            }
        }
        if(index < 0) break;
    }
    return inpString.substr(str_start,str_end-str_start);
}

// returns sliced string based on index
std::string string_trim(std::string inst, int index){
    return space_seperated_string(remove_helper_characters(inst), index);
}

// prints hex inst
void printHEX(uint32_t inst){
    std::cout << std::hex << std::setfill('0') << std::setw(8) << std::uppercase << inst << std::endl;
}

void writeHEX_toFile(uint32_t inst, std::ofstream& file){
    file << std::hex << std::setfill('0') << std::setw(8) << std::uppercase << inst << std::endl;
}

void writeBIN_toFile(uint32_t inst, std::ofstream& file){
    file.write(reinterpret_cast<char*>(&inst), sizeof(inst));
}

class Inst {
protected:
    uint32_t instBIN;
    uint32_t opcode;
public:
    virtual void compute_binINST() = 0;
    virtual void set_instSTR(std::string instSTR) = 0;
    uint32_t get_binINST(){
        return instBIN;
    }
};

class RType : public Inst {
    uint32_t rs1;
    uint32_t rs2;
    uint32_t rd;
    uint32_t func3;
    uint32_t func7;
    std::map <std::string, uint32_t> func3_map = {
        {"add", 0}, {"sub", 0}, {"sll", 1}, {"slt", 2}, {"sltu", 3}, {"xor", 4}, 
        {"srl", 5}, {"sra", 5}, {"or", 6}, {"and", 7}
    };
public:
    void set_instSTR(std::string instSTR){
        std::string instName = string_trim(instSTR, 0);

        opcode = 51;
        rd = reg[string_trim(instSTR, 1)];
        rs1 = reg[string_trim(instSTR, 2)];
        rs2 = reg[string_trim(instSTR, 3)];

        if(instName == "sub" || instName == "sra") func7 = 32;
        else func7 = 0;
        
        func3 = func3_map[instName];
    }

    void compute_binINST(){
        // bit manip and set
        instBIN = (func7 << 25) + (rs2 << 20) + (rs1 << 15) + (func3 << 12) + (rd << 7) + opcode;
    }
};

class IType : public Inst {
    uint32_t rs1;
    uint32_t rd;
    uint32_t imm;
    uint32_t func3;
    uint32_t func7;
    std::map <std::string, uint32_t> func3_map = {
        {"lb", 0}, {"lh", 1}, {"lw", 2}, {"lbu", 4}, {"lhu", 5}, {"addi", 0}, {"slli", 1}, {"slti", 2}, {"sltiu", 3}, {"xori", 4},
        {"srli", 5}, {"srai", 5}, {"ori", 6}, {"andi", 7}, {"jalr", 0}
    };
    std::map <std::string, uint32_t> opcodeMAP = {
        {"lb", 3}, {"lh", 3}, {"lw", 3}, {"lbu", 3}, {"lhu", 3}, {"addi", 19}, {"slli", 19}, {"slti", 19}, {"sltiu", 19}, {"xori", 19},
        {"srli", 19}, {"srai", 19}, {"ori", 19}, {"andi", 19}, {"jalr", 103}
    };
public:
    void set_instSTR(std::string instSTR){
        std::string instName = string_trim(instSTR, 0);

        opcode = opcodeMAP[string_trim(instSTR, 0)];
        // std::cout << opcode << std::endl; // debug
        rd = reg[string_trim(instSTR, 1)];

        if(opcode == 3){ // load inst have different order
            imm = std::stoi(string_trim(instSTR, 2), nullptr, 0);
            rs1 = reg[string_trim(instSTR, 3)];
        } else {
            imm = std::stoi(string_trim(instSTR, 3), nullptr, 0);
            rs1 = reg[string_trim(instSTR, 2)];
        }
        
        if(instName == "srai") func7 = 32;
        else func7 = 0;

        func3 = func3_map[instName];
    }

    void compute_binINST(){
        // bit manip and set
        instBIN = (func7 << 25) + (imm << 20) + (rs1 << 15) + (func3 << 12) + (rd << 7) + opcode;
    }
};

class SType : public Inst {
    uint32_t rs1;
    uint32_t rs2;
    uint32_t imm;
    uint32_t func3;
    std::map <std::string, uint32_t> func3_map = {
        {"sb", 0}, {"sh", 1}, {"sw", 2}
    };
public:
    void set_instSTR(std::string instSTR){
        std::string instName = string_trim(instSTR, 0);

        opcode = 35;
        rs2 = reg[string_trim(instSTR, 1)];
        imm = std::stoi(string_trim(instSTR, 2), nullptr, 0);
        rs1 = reg[string_trim(instSTR, 3)];

        func3 = func3_map[instName];
    }

    void compute_binINST(){
        // bit manip and set
        instBIN = ((0xFFE0 & imm) << 20) + (rs2 << 20) + (rs1 << 15) + (func3 << 12) + ((0x1F & imm) << 7) + opcode;
    }
};

class BType : public Inst {
    uint32_t rs1;
    uint32_t rs2;
    uint32_t imm;
    uint32_t func3;
    std::string label;
    std::map <std::string, uint32_t> func3_map = {
        {"beq", 0}, {"bne", 1}, {"blt", 4}, {"bge", 5}, {"bltu", 6}, {"bgeu", 7}
    };
public:
    void set_instSTR(std::string instSTR){
        std::string instName = string_trim(instSTR, 0);

        opcode = 99;

        // imm is just holding value of inst_addr for now
        // will be updated when calling get function
        imm = -inst_addr; 

        rs1 = reg[string_trim(instSTR, 1)];
        rs2 = reg[string_trim(instSTR, 2)];
        label = string_trim(instSTR, 3);

        func3 = func3_map[instName];
    }
        
    // bit manip and set
    void compute_binINST(){
        for(int i = 0; i < dest_label.size(); i++){
            if(dest_label[i] == label){
                imm += dest_addr[i]; // create offset
                break;
            }
        }
        
        imm = imm << 2;;

        //                  12                 0011 1110 0000 | 10:5                                                 1 1110     |   4:1                 11
        instBIN = (!!((1 << 12) & imm) << 31) + ((0x3E0 & imm) << 25-5) + (rs2 << 20) + (rs1 << 15) + (func3 << 12) + ((0x1E & imm) << 8-1) + (!!((1 << 11) & imm) << 7) + opcode;
    }
};


class UType : public Inst {
    uint32_t rd;
    uint32_t imm;
    std::map <std::string, uint32_t> opcode_map = {
        {"auipc", 23}, {"lui", 55}
    };
public:
    void set_instSTR(std::string instSTR){
        std::string instName = string_trim(instSTR, 0);

        opcode = opcode_map[instName];

        rd = reg[string_trim(instSTR, 1)];
        imm = std::stoi(string_trim(instSTR, 2), nullptr, 0);
    }

    void compute_binINST(){
        instBIN = (imm << 12) + (rd << 7) + opcode;
    }
};

class JType : public Inst {
    uint32_t rd;
    uint32_t imm;
    std::string label;
public:
    void set_instSTR(std::string instSTR){
        std::string instName = string_trim(instSTR, 0);

        opcode = 111;

        // imm is just holding value of inst_addr for now
        // will be updated when calling get function
        imm = -inst_addr; 

        rd = reg[string_trim(instSTR, 1)];
        label = string_trim(instSTR, 2);
    }

    void compute_binINST(){
        // bit manip and return
        for(int i = 0; i < dest_label.size(); i++){
            if(dest_label[i] == label){
                imm += dest_addr[i]; // create offset
                break;
            }
        }
        
        imm = imm << 2;;

        //                  20                   10:1                              1111 1111 0000 0000 | 19:12       
        instBIN = (!!((1 << 20) & imm) << 31) + ((0x7FE & imm) << 20) + (!!((1 << 11) & imm) << 20) + (0xFF000 & imm) + (rd << 7) + opcode;
    }
};

int main(int argc, char* argv[]){

    if(argc != 4){
        std::cerr << "Invalid number of arguments! -> " << argc << std::endl;
        return 1;
    }

    int str_start;
    int break_flag = 0;

    Inst* _instPTR;
    std::string line;
    std::ifstream source(argv[1]);
    std::ofstream destination(argv[2]);

    if(!destination.is_open()){
        std::cerr << "Unable to open file!" << std::endl;
        return 1;
    }

    std::string instName;

    while(getline(source, line)){
        instName = string_trim(line, 0);

        // check if there's a label or empty line
        if(instName == "\n" || instName == "\0" || instName[instName.length()-1] == ':'){
            if (instName[instName.length()-1] == ':'){
            dest_label.push_back(instName.substr(0,instName.length()-1)); // store jump dest label
            dest_addr.push_back(inst_addr); // store jump dest address
            }
            continue; // don't compute instruction, continue
        }

        switch (instMAP[instName])
        {
        // R, I, S, B, U, J
        case R:
            _instPTR = new RType;
            break;
        case I:
            _instPTR = new IType;
            break;
        case S:
            _instPTR = new SType;
            break;
        case B:
            _instPTR = new BType;
            break;
        case U:
            _instPTR = new UType;
            break;
        case J:
            _instPTR = new JType;
            break;
        default:
            break;
        }

        _instPTR->set_instSTR(line);
        instPTRS.push_back(_instPTR); // add current instPTR to our vector
        inst_addr++;
    }


    // prints and deallocates memory
    for(int i = 0; i < instPTRS.size(); i++){
        instPTRS[i]->compute_binINST();

        // printHEX(instPTRS[i]->get_binINST());
        if(std::string(argv[3]) == "-h")
            writeHEX_toFile(instPTRS[i]->get_binINST(), destination);
        else if(std::string(argv[3]) == "-b")
            writeBIN_toFile(instPTRS[i]->get_binINST(), destination);
        else {
            std::cerr << "Invalid output type! ->" << argv[3] << "<-" << std::endl << "Use hex or bin" << std::endl;
            return 1;
        }

        delete instPTRS[i];
    }

    source.close();
    destination.close();

    return 0;
}