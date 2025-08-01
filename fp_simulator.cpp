#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cassert>
#include <bitset>
#include <cstdint>
#include <cstring>
#include <queue>
#include <map>
#include <algorithm>
#include <set>
#include <limits>
#include <filesystem>

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
    int free_at;  // time upto which busy
};

struct FunctionalUnit {
    int free_at;
    int latency;

    FunctionalUnit(bool ibusy, int ifreed_time_stamp, int ilatency) : free_at(ifreed_time_stamp), latency(ilatency) {}

};

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
    EventType type;
    Instruction instr;
    int issue;
    int start;
    int complete;
    int writeback;
    int curr_time;
    double result;

    Event(
        int index,
        EventType type, 
        int issue,
        int start,
        int complete,
        int writeback,
        int curr_time,
        double result
    ) {
        this-> index = index;
        this->type = type;
        this->issue = issue;
        this->start = start;
        this->complete = complete;
        this->writeback = writeback;
        this->curr_time = curr_time;
        this->result = result;
    }
};

struct EventCompArrCycle {
    bool operator()(const Event &e1, const Event &e2) {
        if (e1.curr_time > e2.curr_time) {
            return true;
        }
        else if (e1.curr_time < e2.curr_time) {
            return false;
        }
        else {
            if (e1.instr.arrival_cycle < e2.instr.arrival_cycle) {
                return true;
            }
            else {
                return false;
            }
        }
        return false;
    }
};

struct CompEventByIndex {
    bool operator()(const Event &e1, const Event &e2) {
        return e1.index > e2.index;
    }
};


/*
    Logging Defs
*/
map<string, FunctionalUnit> functional_units;
FPRegister reg_file[32];
priority_queue<Event, vector<Event>, CompEventByIndex> events_by_index;
map<EventType, int> pipeline_use_after;


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
    if (!filesystem::exists(filename)) {
        throw runtime_error("input trace does not exits");
    }
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
        if (dot_pos >= opcode_full.size()) {
            throw runtime_error("invalid opcode");
        }
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


priority_queue<Event, vector<Event>, EventCompArrCycle> prepare_pq_from_instrs(vector<Instruction> instrs) {
    priority_queue<Event, vector<Event>, EventCompArrCycle> event_pq;
    for (auto instr : instrs) {
        // int index;
        // EventType type;
        // Instruction instr;
        // int issue;
        // int start;
        // int complete;
        // int writeback;
        // int curr_time;
        // double result;
        Event e = {
            instr.id,
            ISSUE,
            instr.id,
            instr.arrival_cycle,
            instr.arrival_cycle,
            instr.arrival_cycle,
            instr.arrival_cycle,
            0.0
        };
        event_pq.push(e);
    }

    return event_pq;
}

bool is_reg_available(int reg_num, int curr_time) {
    if (reg_file[reg_num].free_at <= curr_time) {
        return true;
    }
    return false;
}

bool is_pipeline_available(EventType stage, int curr_time) {
    if (pipeline_use_after[stage] <= curr_time) {
        return true;
    }
    return false;
}

bool is_reg_available_list(vector<int> regs, int curr_time) {
    bool avail = true;
    for (int reg : regs) {
        avail = avail && is_reg_available(reg, curr_time);
    }
    return avail;
}

bool is_all_resource_available(vector<int> regs, EventType stage, int curr_time) {
    if (is_reg_available_list(regs, curr_time) && is_pipeline_available(stage, curr_time)) {
        return true;
    }
    return false;
}

int next_available_cycle(vector<int> regs, EventType stage){
    int time = pipeline_use_after[stage];
    for (int reg:regs) {
        time = max(reg_file[reg].free_at, time);
    }
}

/**
 * @brief computes result from an instruction
 * 
 * Now supports FMOV.S and FMOV.D
 * 
 * @param instr get result for this instruction
 * @return computed result always in double, scale down to float at your end if is_64bit
 * 
 * @note currently sum of fp32 and fp64 is not supported becuase of lack of information of final conversion
 */
double compute_result(Instruction &instr) {
    string op = instr.op;
    
    double val1, val2, res;
    
    // Determine operand values based on bit width
    if (instr.is_double) {
        val1 = reg_file[instr.src1].f64;
        if (instr.src2 != NULL) val2 = reg_file[instr.src2].f64;
    } else {
        val1 = static_cast<double>(reg_file[instr.src1].f32);
        if (instr.src2 != NULL) val2 = static_cast<double>(reg_file[instr.src2].f32);
    }

    
    
    if (op == "FADD.S" || op == "FADD.D") {
        res = val1 + val2;
    } else if (op == "FSUB.S" || op == "FSUB.D") {
        res = val1 - val2;
    } else if (op == "FMUL.S" || op == "FMUL.D") {
        res = val1 * val2;
    } else if (op == "FDIV.S" || op == "FDIV.D") {
        if (val2 == 0) {
            res = std::numeric_limits<double>::quiet_NaN(); // NAN if 0/0
        } else {
            res = val1 / val2;
        }
    } else if (op == "FMOV.S" || "FMOV.D") {
        res = val1;
    }

    return res;
}


void process_event(Event &event, priority_queue<Event, vector<Event>, EventCompArrCycle> &pending_events) {
    int o1 = event.instr.src1, o2 = event.instr.src2, res = event.instr.dst;
    string op = event.instr.op;
    int time = event.curr_time;
    EventType type = event.type;
    switch (event.type)
    {
    case ISSUE:
        // schedule the event
        event.issue = time;
        pipeline_use_after[event.type]=time;
        
        event.type = START;

        pending_events.push(event);
        break;
    
    case START:
        if (is_all_resource_available({o1, o2, res}, event.type, time)) {
            event.start = time;
            int op_latency = functional_units[op].latency;
            int upd_time = time + op_latency;
            // update only result reg, because that is being written
            reg_file[res].free_at = upd_time;
            
            // update the functional units because of singular presence
            functional_units[op].free_at = upd_time;
            // update the pipeline avail time
            pipeline_use_after[event.type] = upd_time;
            
            // update curr time stamp
            event.curr_time = upd_time;

            // compute in between start and complete
            event.result = compute_result(event.instr);
            if (event.instr.is_double){
                reg_file[event.instr.dst].f64 = event.result;
            }
            else {
                reg_file[event.instr.dst].f32 = static_cast<float>(event.result);
            }

            event.type = COMPLETE;
            event.complete = upd_time - 1;

            event.type=WRITEBACK;
        }
        else {
            event.curr_time = next_available_cycle({o1, o2, res}, event.type);
        }
        pending_events.push(event);
        break;

    case WRITEBACK:
        // dont push back the event after this
        if (pipeline_use_after[event.type]<=time) {
            event.writeback = event.curr_time;
            events_by_index.push(event);
        }
        else {
            event.curr_time = pipeline_use_after[event.type];
            pending_events.push(event);
        }
        break;
    
    default:
        break;
    }
}

bool check_nan(FPRegister reg_file[32]) {
    for (int i=0; i<32; i++){
        if (reg_file[i].is_64bit) {
            if (reg_file[i].f64==std::numeric_limits<double>::quiet_NaN()) {
                return true;
            }
        }
        else {
            if (reg_file[i].f32==std::numeric_limits<float>::quiet_NaN()) {
                return true;
            }
        }
    }
    return false;
}

/**
 * @brief Main engine running the Discrete time simulation
 * 
 * @param pending_events must contain Event type DS in pq format
 * @return None
 */
void DESEngine(priority_queue<Event, vector<Event>, EventCompArrCycle> pending_events) {
    
    while (pending_events.size() != 0) {
        if (check_nan(reg_file)) {
            break;
        }
        Event event = pending_events.top();
        pending_events.pop();
        process_event(event, pending_events);
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
 * @warning make sure the last line has backsslash n
 */
void to_csv(vector<tuple<int,string,int,int,int,int,int>> entries, string filename) {
    ofstream outputFile;
    outputFile.open(filename, ios::out);

    if (!outputFile.is_open()) {
        cerr << "Error opening the file" << endl;
    }

    for (auto entry : entries) {
        outputFile 
        << get<0>(entry) << ","
        << get<1>(entry) << ","
        << get<2>(entry) << ","
        << get<3>(entry) << ","
        << get<4>(entry) << ","
        << get<5>(entry) << ","
        << get<6>(entry) << ",";
    }

    outputFile.close();
}

vector<tuple<int,string,int,int,int,int,int>> organize_info(priority_queue<Event, vector<Event>, CompEventByIndex> events_by_index) {

    vector<tuple<int,string,int,int,int,int,int>> res;
    priority_queue<Event, vector<Event>, CompEventByIndex> events_by_index_temp;
    while (!events_by_index.empty()) {
        Event event = events_by_index.top();
        events_by_index.pop();
        
        Instruction instr = event.instr;
        string risc_op = instr.op + " " + "R" + to_string(instr.src1) + " " + "R" + to_string(instr.src2);
        res.push_back(
            {event.index,risc_op,event.issue,event.start,event.complete,event.writeback,event.result}
        );
    }
}

/**
 * @brief Executes the main content
 * 
 * Flow 
 *  - identifies files
 *  - pass the input to DES Engine
 *  - save them to json as per requirement
 * 
 * @param argc, argv whatever written in the terminal
 * @return successful execution
 */
int main(int argc, char* argv[]) {
    
    if (argc != 3) {
        cerr << "Usage: ./fp_simulator <input_trace> <output_csv>\n";
        return 1;
    }

    string input_trace = argv[1];
    string output_csv = argv[2];

    /*
        to initialize:
        map<string, FunctionalUnit> functional_units;
        FPRegister reg_file[32];
    */
    
    functional_units = {
        {"FADD.S", FunctionalUnit(false, 0, 3)},
        {"FADD.D", FunctionalUnit(false, 0, 5)},
        {"FSUB.S", FunctionalUnit(false, 0, 3)},
        {"FSUB.D", FunctionalUnit(false, 0, 5)},
        {"FMUL.S", FunctionalUnit(false, 0, 4)},
        {"FMUL.D", FunctionalUnit(false, 0, 6)},
        {"FDIV.S", FunctionalUnit(false, 0, 10)},
        {"FDIV.D", FunctionalUnit(false, 0, 16)},
    };

    for (int i=0; i<32; i++) {
        reg_file[i].f32 = 0.0;
        reg_file[i].f64 = 0.0;
        reg_file[i].free_at = 0;
        reg_file[i].is_64bit = false;
    }

    pipeline_use_after[ISSUE]=0;
    pipeline_use_after[START]=0;
    pipeline_use_after[COMPLETE]=0;
    pipeline_use_after[WRITEBACK]=0;

    auto instructions = parse_input_file(input_trace);
    // TODO: Run simulation
    priority_queue<Event, vector<Event>, EventCompArrCycle> pending_events = prepare_pq_from_instrs(instructions);
    
    DESEngine(pending_events);
    // TODO: Write results to output_csv
    vector<tuple<int,string,int,int,int,int,int>> organized_info =  organize_info(events_by_index);
    to_csv(organized_info, output_csv);
    return 0;
}