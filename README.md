## Brief of the Assignment

- This is a Discrete Event Simulator based model that emulates instruction based on a floating point processor.
- On a high level the algorithm to perform this is split into two parts:
    - A DES Engine: takes input for the queue of events to be scheduled
    - Process Handler: handles the entry and execution of an event in the pipeline

## Instructions to Run this code

1. Open terminal in the folder where fp_simulator.cpp is placed
2. Run the following command, here input_trace is the input test cases and output file is the desired file path
    ```
    make | ./fp_simulator <input_trace> <output_file>
    ```

## Descriptions

0. **Event Simulation**
    - DES Engine:
        - Event: wrapper for instruction along with a cycle instance associated with it
        - Events Priority Queue: maintains all the events based on their cycle instance
        - cycle instacne allows scheduling/ stalling instruction by updating this values and re-pushing this event in the queue of pending events
        - Exits any time an exception occurs, such as 0/0
    - Better than running all cycles in a for loop to increase the run time of the code

1. **Assumptions on Hardware**:
    - **Functional Unit**:
        - a .S and .D opcode differ both in terms of latency and precision
        - The program lets avail a functional unit when the previous instruction (which was using this functinal block) has started its writeback
        - All operations operate on IEEE 754
    - **Register File**:
        - Each register has a port of access (since simulating "floating point processor" and not the larger scale processor)
        - Each register's read cycle is zero
        - Each register's write cycle is also zero
        - Basically the register file is assumed to have infinite porting for read but one port per register for write
    - **Pipeline stages**:
        - There is no precise definition of pipeline here so we are storing just the issue, start, complete and writeback cycle of instruction execution


2. **Issue**
    - Any instruction can be issued one at a time. 
    - If their arrival cycles coincide then based on the index, the smaller one is executed first
    - Indexes are assigned based on arrival cycles and ties are broken at random
    - Therefore issue_cycle = arrival_cycle, unless there are multiple instructions arriving at the same time

3. **Start**
    - This is the cycle at which all the resources are available which includes the functional unit, operands and the destination register for execution
    - The instruction is delayed if any of the operands are not available for the instruction to execute.
    - The destination register's availibility is updated to the time of the earliest potential cycle when all the resources are available, disallowing incoming instructions to use dirty value of the destination register 

4. **COMPLETE**
    - This the last cycle when the functional unit operates on an instruction 
    - complete = start + latency - 1

5. **WRITEBACK**
    - This is the cycle when the information is written back into the destination register
    - Assuming the write cycles to be zero, from the instance of writeback the register is available for processing
    - writeback = complete + 1

6. **RESULT**
    - All the computations are performed in fp64 and scaled down to fp 32 whenever required
    - All the register values are initially assumed to be zero and remain zero until a division is encountered
    - If an exception is encountered during the execute stage, the event of this instrcution is recorded upto the complete, after which it writeback is assigned as -1
    - since the events are executed out of order, events that get completed before the exception instrcuction even though whose arrival cycle is greater than the instruction in consideration get listed in output.csv
    - These instrcution mainly include the ones that do not use the resources pre-occupied pre-exception and latency of completion is lesser

## Visualization

[Gantt Chart](plot.png)

Note:
- for MOV operations start and complete are the same by the assumptions mentioned above, therefore no block from start to complete

## Warnings

1. Mixed precision operations not supported