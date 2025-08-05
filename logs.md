## Day 1

- Plan to write basic outline of code and identify initial bugs and troubles that may persist 
- Learn the concepts:
    - IEEE 754 FP ops
    - DES

## Day 2

- Script the conversions from bin to float
- Read and Implement the Pipelining features properly (DES Engine)

## Day 3

- Corrected some logical error
    - Complete is the actual complete cycle when compute is over
    - Write back must be corrected:
        - Suppose there are two instructions, i1: ADD R1 R2 R3 and i2: SUB R4 R5 R1, suppose i1 completes at 5th cycle (i.e., witten at 5th cycle, the register is occupied till 5th cycle). And at 5th cycle the second instruction arrives using R1
        - Possible explanation is a register in the middle

## Day 4

- refractor the code to keep all the data values in fp64 and scale down as per requirement
- run the code once at least to see some basic results
- make updates to the fact the index may not correspond to the arrival times

## Day 5

- performed a major correction regarding the cycle of availability of a register, which had not been done correctly before. The issue was we only updated the current time but left the register un attended, for any new instruction that comes it sees the fact that it has its resources available, but actually does not
- The fact is that current time is not linked to reg_file free time and they can head their independent directions
- functional unti for both .S and .D are assumed to be the same expect their latencies
- the code must immedeatily stop when nan is encountered
- write the description of what the code actually is in README.md

## Day 6

- Provide a header to the README.md describing the repo
- check the requirements of the pdf
- upload on moodle
- make a version where only single unit supports one unit operations
- also run with the expected description
- support float overflow in single unit version

## Day 7

- Some of the code was written using LLMs, must be understood, especially the simulation part
- memcpy is a new technique learnt

## Future

- Build next projects with gem5 simulator