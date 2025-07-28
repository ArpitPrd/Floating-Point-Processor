#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cassert>

using namespace std;

struct FPRegister {

    /*
        Args:
            - f32: use if fp32
            - f64: use if fp64
            - is_64bit: true if in fp64 else false
            - busy: true if current in use by the DES Engine

        Reason:
            Convinience for registering the values

        Caution:
            Intialise before use
    */

    union {
        float f32;
        double f64;
    };
    bool is_64bit;   // true if f64, false if f32
    bool busy;       // used for stalling logic
};

FPRegister reg_file[32];


struct Instruction {

    /*
        Args:
            - arrival_cycle: time of arrival of instr
            - op: opcode of the instr
            - is_double: operations double or single
            - dst: destination register int
            - src1, src2: sources of register int
            - id: identification of the instruction

        Reason:
            Encapsulates information in an instruction

        Caution:
            Initialize before use
    */

    int arrival_cycle;
    string op;
    bool is_double;
    int dst, src1, src2;
    int id;
};

struct Output {
    
    /*
        Args:
            index - ID of the instr
            instr - exact RISC style syntax
            issue - clock cycle at which issued
            start - clock cycle at which processing by the DES Eng
            complete - clock cycle at which processing complete by DES Eng
            writeback - clock cycle when result written back
            result - (double) result value // NOTE: must be converted to .6f whenever needed
                    storing in higher precision is no harm can be converted to lower
        
        Reason:
            Encapsulates every information contained in the output

        Caution:
            Intialize before use
    */
    
    int index;
    string instr;
    int issue;
    int start;
    int complete;
    int writeback;
    double result;
};

vector<Instruction> parse_input_file(string filename) {
    /*
        Reason:
            parses the input file and indentifies:
            - arrival_cycle: (int) cycle of arrival of instr
            - op: (string) opcode of the instr
            - is_double: (bool) operations double or single
            - dst: (int) destination register int
            - src1, src2: (int, int) sources of register int
            - id: (int) identification of the instruction, internally maintained by the
            code no user interference

        Caution:
            If any info type is incorrect then typecasted implicitly
    */
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

void DESEngine(Instruction instr) {
    /*
        The engine that identifies all the components
        executes the information
        and stores them in a vector Output format
        
        Here the actual pipelining shall be happening
    */

}

float bin32_to_float32(string bin) {
    /*
    Args:
        bin: (string) sign (1) + exp (7) + mantisa (24)

    Caution:
        string will be checked for 32 length, if not abort
    */

    assert(bin.size()==32);
    float sign = 1.0 ? bin[0]==0 : -1.0;
    
}

void to_json(Output out, string filename) {
    /*
        general purpose json saver
    */
}

void to_csv(Output out, string filename) {
    /*
        general purpose csv saver
    */
}

int main(int argc, char* argv[]) {
    /*
        Executes the main content
        Flow:
            - indentify files
            - pass the input to DES Engine 
            - save them csv or json as per requiement
    */
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

