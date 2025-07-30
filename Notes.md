# Floating Point Processor

## Technqiues

TOLEARN: Git commit history structure

| Technique | Usage | Example |
|--|--|--|
TODO | What must be done | grep -rn "TODO" src/
FIXME | Issues to be resolved | grep -rn "FIXME" src/
assert | assert the truth value of statements | assert(registers[5].busy == false)


## Plan

📌 WEEK 1: Foundations
1. Understand IEEE 754 Floating Point Representation

    📘 Topics:

        IEEE 754 format for 32-bit (single) and 64-bit (double) floats

        Bit layout: Sign, Exponent, Mantissa

        Special values: NaN, Infinity, Denormals, Zero

    ✅ Practice:

        Write functions to encode/decode float/double to binary manually

        Validate using std::bitset, memcpy, and reinterpret_cast

2. Study Floating Point Operations in C/C++

    Use float and double for operations

    Understand rounding modes, overflow, underflow, and precision issues

📌 WEEK 2: Discrete Event Simulation (DES) Engine
3. Understand Discrete-Event Simulation

    📘 Learn:

        Concepts of events, event queue, simulation clock

        Event types: ISSUE, START, COMPLETE, WRITEBACK

        Use priority queues for time-based event ordering

    ✅ Implement:

        A simple DES framework:

            Class Event with timestamp, type, and instruction_id

            A priority_queue<Event> sorted by timestamp

            A simulation loop that pops and executes events

📌 WEEK 3: Processor Modeling
4. Design Floating Point Register File

    32 registers: R0–R31, each can store:

        a float (32-bit)

        or a double (64-bit)

    ✅ Design:

    struct FPRegister {
        union {
            float f32;
            double f64;
        };
        bool is64Bit;
        bool busy;
    };

5. Functional Units and Pipeline Modeling

    Functional Units (FUs): ADD, MUL, DIV, etc. — each has:

        Latency (e.g., ADD: 3 cycles, MUL: 5 cycles)

        Availability (1 or more parallel units)

    Implement stalling if FU is busy

    Track:

        When an FU is free

        Which instruction is using it

📌 WEEK 4: Integration & Simulation
6. Instruction Handling

    Define a format:

    struct Instruction {
        std::string op; // ADD, MUL, etc.
        int dst, src1, src2;
        bool is64Bit;
        float fsrc1, fsrc2;  // or double
    };

7. DES Event Processing

    For each instruction:

        Issue → START → COMPLETE → WRITEBACK

        Each stage depends on FU and register availability

        Update simulation clock and state

8. Metrics & Debugging

    Output:

        Per-instruction timing

        Resource usage

        Register contents

    Use logging or CSV output to trace instruction flow and detect bugs


## Points to keep in mind

1. Issue is time of check of availability 
2. Start is the time when exactly start
3. curr_time_stamp is exactly when the resources become free
4. any time that is denoted here is the exact time of availability