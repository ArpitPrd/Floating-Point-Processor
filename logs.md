[Day 1]

- Plan to write basic outline of code and identify initial bugs and troubles that may persist 
- Learn the concepts:
    - IEEE 754 FP ops
    - DES

[Day 2]

- Script the conversions from bin to float
- Read and Implement the Pipelining features properly (DES Engine)

[Day 3]

- Corrected some logical error
    - Complete is the actual complete cycle when compute is over
    - Write back must be corrected:
        - Suppose there are two instructions, i1: ADD R1 R2 R3 and i2: SUB R4 R5 R1, suppose i1 completes at 5th cycle (i.e., witten at 5th cycle, the register is occupied till 5th cycle). And at 5th cycle the second instruction arrives using R1
        - Possible explanation is a register in the middle