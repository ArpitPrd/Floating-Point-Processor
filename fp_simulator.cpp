#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cassert>
#include <bitset>
#include <cstdint>
#include <cstring>
#include <queue>

using namespace std;

enum EventType {ISSUE, START, COMPLETE, WRITEBACK};

/**
 * @brief Convinience for registering the values
 * 
 * @param f32 use if fp32
 * @param f64 use if fp64
 * @param is_64bit true if in fp64 else false
 * @warning Intialise before use
 */
struct FPRegister {

    union {
        float f32;
        double f64;
    };
    bool is_64bit;   // true if f64, false if f32
    bool busy;       // used for stalling logic
};

FPRegister reg_file[32];

/**
 * @brief Encapsulates information in an instruction
 * 
 * @param arrival_cycle time of arrival of instr
 * @param op opcode of the instr
 * @param is_double operations double or single
 * @param dst destination register int
 * @param src1, src2 sources of register int
 * @param id identification of the instruction
 * @warning Initialize before use
 */
struct Instruction {

    int arrival_cycle;
    string op;
    bool is_double;
    int dst, src1, src2;
    int id;
};

/**
 * @brief Encapsulates every information contained in the Event
 * 
 * @param index ID of the instr
 * @param instr exact RISC style syntax
 * @param issue clock cycle at which issued
 * @param start clock cycle at which processing by the DES Eng
 * @param complete clock cycle at which processing complete by DES Eng
 * @param writeback clock cycle when result written back
 * @param result result value 
 * 
 * NOTE: result must be converted to .6f whenever needed storing in higher precision is no harm can be converted to lower
 * @warning Intialize before use
 */
struct Event {
    
    int index;
    Instruction instr;
    int issue;
    int start;
    EventType type;
    int complete;
    int writeback;
    double result;
};

/**
 * @brief parses input file and converts them Instruction format
 * 
 * parses the input file and indentifies:
 * 
 *  arrival_cycle: (int) cycle of arrival of instr
 * 
 *  op: (string) opcode of the instr
 * 
 *  is_double: (bool) operations double or single
 * 
 *  dst: (int) destination register int
 * 
 *  src1, src2: (int, int) sources of register int
 * 
 *  id: (int) identification of the instruction, internally maintained by the code no user interference
 * 
 * @param filename string name of the file to be parsed
 * @return list of instructions
 * @throw if any of the conversion to get ints is invalid
 */
vector<Instruction> parse_input_file(string filename) {
    ifstream infile(filename);
    vector<Instruction> instructions;
    string line;
    int inst_id = 0;

    while (getline(infile, line)) {
        if (line.empty()) continue;

        istringstream iss(line);
        int cycle;
        string opcode_full, rd, rs1, rs2;

        iss >> cycle >> opcode_full >> rd >> rs1 >> rs2;

        Instruction instr;
        instr.arrival_cycle = cycle;
        instr.id = inst_id++;

        // Split opcode and precision suffix
        size_t dot_pos = opcode_full.find('.');
        instr.op = opcode_full.substr(0, dot_pos);
        instr.is_double = (opcode_full.substr(dot_pos + 1) == "D");

        // Parse registers: strip 'R' and convert to int
        instr.dst = stoi(rd.substr(1));
        instr.src1 = stoi(rs1.substr(1));
        instr.src2 = stoi(rs2.substr(1));

        instructions.push_back(instr);
    }

    return instructions;

}

/**
 * @brief Engine running the pipeling
 * 
 * @param instrs all the instructions to pipeline
 * @return None
 * @warning TODO
 */
void DESEngine(vector<Instruction> instrs) {
    int clock_ticks = instrs.size();

    queue<Event> events;
    int clock_tick = 0;
    while (events.size() != 0) {

    }
}   

/**
 * @brief converts bin string to fp32
 * 
 * @param bin string of 0's and 1's
 * @return double value interpreted from the bin
 * @throw length of bin must be 32, else exit
 */
float bin32_to_float32(string bin) {
    if (bin.size()!=32) {
        throw std::invalid_argument("Binary string must be 32 bits");
    }

    bitset<32> bits(bin);
    uint32_t intval = bits.to_ulong();
    float result;
    std::memcpy(&result, &intval, sizeof(result));

    return result;
}

/**
 * @brief converts bin string to fp64
 * 
 * @param bin string of 0's and 1's
 * @return double value interpreted from the bin
 * @throw length of bin must be 64, else exit
 */
double bin_to_float64(string bin) {
    
    if (bin.size() != 64) {
        throw std::invalid_argument("Binary string must be 64 bits");
    }

    std::bitset<64> bits(bin);
    uint64_t intVal = bits.to_ullong();
    double result;
    std::memcpy(&result, &intVal, sizeof(result));
    return result;
}

/**
 * @brief generla purpose json saver
 * 
 * @param out Event file type
 * @param filename string of the file name
 * @return None
 * @throw unable to open files
 */
void to_json(Event out, string filename) {

}

/**
 * @brief general purpose csv saver
 * 
 * @param out Event storage list
 * @param filename string of the file name
 * @throw unable to open file
 */
void to_csv(Event out, string filename) {
    /*
        general purpose csv saver
    */
}

/**
 * @brief Executes the main content
 * 
 * Flow 
 *  - identifies files
 *  - pass the input to DES Engine
 *  - save them to json as per requirement
 */
int main(int argc, char* argv[]) {
    
    if (argc != 3) {
        cerr << "Usage: ./fp_simulator <input_trace> <output_csv>\n";
        return 1;
    }

    string input_trace = argv[1];
    string output_csv = argv[2];

    
    auto instructions = parse_input_file(input_trace);
    // TODO: Run simulation
    // TODO: Write results to output_csv

    return 0;
}

