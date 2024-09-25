# Branch Predictor Project

## Table of Contents
  * [Running your predictor](#running-your-predictor)
  * [Traces](#traces)
  * [Implementing the predictors](#implementing-the-predictors)
    - [Bimodal](#bimodal)
    - [Gshare](#gshare)
    - [Tournament](#tournament)
    - [Custom](#custom)
    - [Things to note](#things-to-note)
  * [Grading](#grading)
    - [Grading the custom predictor](#grading-the-custom-predictor)
  * [Turn-in Instructions](#turn-in-instructions)


## Running your predictor

In order to build your predictor you simply need to run `make` in the src/ directory of the project. You can then run the program on an uncompressed trace as follows:   

`./predictor <options> [<trace>]`

If no trace file is provided then the predictor will read in input from STDIN. Some of the traces we provided are rather large when uncompressed so we have distributed them compressed with bzip2 (included in the Docker image).  If you want to run your predictor on a compressed trace, then you can do so by doing the following:

`bunzip2 -kc trace.bz2 | ./predictor <options>`

In either case the `<options>` that can be used to change the type of predictor
being run are as follows:

```
  --help       Print usage message
  --verbose    Outputs all predictions made by your
               mechanism. Will be used for correctness
               grading.
  --<type>     Branch prediction scheme. Available
               types are:
        static
        bimodal
        gshare:<# ghistory>
        tournament:<# ghistory>:<# lhistory>:<# index>
        custom
```
An example of running a gshare predictor with 10 bits of history would be:   

`bunzip2 -kc ../traces/int1_bz2 | ./predictor --gshare:10`

## Traces

These predictors will make predictions based on traces of real programs.  Each line in the trace file contains the address of a branch in hex as well as its outcome (Not Taken = 0, Taken = 1):

```
<Address> <Outcome>
Sample Trace from int_1:

0x40d7f9 0
0x40d81e 1
0x40d7f9 1
0x40d81e 0
```

We provide test traces to you to aid in testing your project but we strongly suggest that you create your own custom traces to use for debugging.



## Implementing the predictors

There are 3 methods which need to be implemented in the predictor.c file.
They are: **init_predictor**, **make_prediction**, and **train_predictor**.

`void init_predictor();`

This will be run before any predictions are made.  This is where you will initialize any data structures or values you need for a particular branch predictor 'bpType'.  All switches will be set prior to this function being called.

`uint8_t make_prediction(uint32_t pc);`

You will be given the PC of a branch and are required to make a prediction of TAKEN or NOTTAKEN which will then be checked back in the main execution loop. You may want to break up the implementation of each type of branch predictor into separate functions to improve readability.

`void train_predictor(uint32_t pc, uint8_t outcome);`

Once a prediction is made a call to train_predictor will be made so that you can update any relevant data structures based on the true outcome of the branch. You may want to break up the implementation of each type of branch predictor into separate functions to improve readability.

#### Bimodal
```
Configuration:
    bhistoryBits // Indicates the number of bits used to index BHT
```
Here we want to find the number of bits that maximizes the success of bimodal predictor accross all test cases provided, basically minimize the average misprediction rate per branch minimize(sum(misprediction_case_i)/sum(num_branches_i)). In this case the number is set on selecting the bimodal predictor for the program and therefore there is no commandline option to change bhistoryBits. 
#### Gshare

```
Configuration:
    ghistoryBits    // Indicates the length of Global History kept
```
The Gshare predictor is characterized by XORing the global history register with the lower bits (same length as the global history) of the branch's address.  This XORed value is then used to index into a 1D BHT of 2-bit predictors.

#### Tournament
```
Configuration:
    ghistoryBits    // Indicates the length of Global History kept
    lhistoryBits    // Indicates the length of Local History kept in the PHT
    pcIndexBits     // Indicates the number of bits used to index the PHT
```

This is extra points. You will be implementing the Tournament Predictor popularized by the Alpha 21264.  The difference between the Alpha 21264's predictor and the one you will be implementing is that all of the underlying counters in yours will be 2-bit predictors.  You should NOT use a 3-bit counter as used in one of the structure of the Alpha 21264's predictor.  See the Alpha 21264 paper for more information on the general structure of this predictor.  The 'ghistoryBits' will be used to size the global and choice predictors while the 'lhistoryBits' and 'pcIndexBits' will be used to size the local predictor.

#### Custom

This is extra points, and gets you in the compatition. You now have the opportunity to be creative and design your own predictor. You can use an already implemented predictor with set values if you believe it can compete.The only requirement is that the total size of your custom predictor must not exceed (64K + 256) bits (not bytes) of stored data and that your custom predictor must outperform both the Gshare and Tournament predictors (details below).

#### Things to note

All history should be initialized to NOTTAKEN.  History registers should be updated by shifting in new history to the least significant bit position.
```
Ex. 4 bits of history, outcome of next branch is NT
  T NT T NT   <<  NT
  Result: NT T NT NT
```
```
All 2-bit predictors should be initialized to WN (Weakly Not Taken).
They should also have the following state transitions:

        NT      NT      NT
      ----->  ----->  ----->
    ST      WT      WN      SN
      <-----  <-----  <-----
        T       T       T
```

The Choice Predictor used to select which predictor to use in the Alpha 21264 Tournament predictor should be initialized to Weakly select the Global Predictor.

## Grading
To be updated.

#### Grading the custom predictor


## Turn-in instructions

