//========================================================//
//  predictor.c                                           //
//  Source file for the Branch Predictor                  //
//                                                        //
//  Implement the various branch predictors below as      //
//  described in the README                               //
//========================================================//
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "predictor.h"

//
// TODO:Student Information
//
const char *studentName = "Qi Ling";
const char *studentID   = "037771523";
const char *email       = "ling102@purdue.edu";

//------------------------------------//
//      Predictor Configuration       //
//------------------------------------//

// Handy Global for use in output routines
const char *bpName[5] = { "Static", "Gshare",
                          "Tournament", "Custom", "Bimodal" };

int ghistoryBits; // Number of bits used for Global History
int lhistoryBits; // Number of bits used for Local History
int bhistoryBits; // Number of bits used for Bimodal History
int pcIndexBits;  // Number of bits used for PC index
int bpType;       // Branch Prediction Type
int verbose;

//------------------------------------//
//      Predictor Data Structures     //
//------------------------------------//

//
//TODO: Add your own Branch Predictor data structures here
//
#define false 0
#define true 1
#define ST 3 // Strongly Taken (11)
#define WT 2 // Weakly Taken (10)
#define WN 1 // Weakly Not Taken (01)
#define SN 0 // Strongly Not Taken (00)

// bimodal branch predictor with 2-bit saturation counters
uint8_t *bht;                   // Branch History Table (2-bit counters)
uint32_t bht_size;              // Size of the Branch History Table
void
init_bimodal()
{
    bht_size = 1 << bhistoryBits;      // Calculate the size of the BHT as 2^bhistoryBits
    bht = (uint8_t *)malloc(bht_size * sizeof(uint8_t));

    // Initialize all counters in the BHT to Weakly Taken (10)
    for (uint32_t i = 0; i < bht_size; i++) {
        bht[i] = WN;
    }
}

uint8_t
predict_bimodal(uint32_t pc)
{
    uint32_t index = pc & (bht_size - 1); // Use the lower bits of PC to index into the BHT
    uint8_t counter = bht[index];

    // Predict taken if the counter is in state WT or ST
    return (counter >= WT) ? 1 : 0; // 1 for taken, 0 for not taken
}

void
train_bimodal(uint32_t pc, uint8_t outcome)
{
    uint32_t index = pc & (bht_size - 1); // Use the lower bits of PC to index into the BHT
    uint8_t counter = bht[index];

    // Update the 2-bit saturating counter based on the actual outcome
    if (outcome == 1) { // If the actual outcome is taken
        if (counter < ST) {
            bht[index]++; // Increment the counter towards Strongly Taken (ST)
        }
    } else { // If the actual outcome is not taken
        if (counter > SN) {
            bht[index]--; // Decrement the counter towards Strongly Not Taken (SN)
        }
    }
}

void cleanup_bimodal()
{
    assert(bht != NULL);
    free(bht);
    bht = NULL;
}


//------------------------------------//
//        Predictor Functions         //
//------------------------------------//

// Initialize the predictor
//
void
init_predictor()
{
  //
  //TODO: Initialize Branch Predictor Data Structures
  //
  switch (bpType) {
    case STATIC:
	    break;
    case BIMODAL:
	    init_bimodal();
	    break;
    case GSHARE:
    case TOURNAMENT:
    case CUSTOM:
    default:
	    assert(false && "Not implemented");
  }
}

// Make a prediction for conditional branch instruction at PC 'pc'
// Returning TAKEN indicates a prediction of taken; returning NOTTAKEN
// indicates a prediction of not taken
//
uint8_t
make_prediction(uint32_t pc)
{
  //
  //TODO: Implement prediction scheme
  //

  // Make a prediction based on the bpType
  switch (bpType) {
    case STATIC:
      return TAKEN;
    case BIMODAL:
      return predict_bimodal(pc);
    case GSHARE:
    case TOURNAMENT:
    case CUSTOM:
    default:
	    assert(false && "Not implemented");
  }

  // If there is not a compatable bpType then return NOTTAKEN
  return NOTTAKEN;
}

// Train the predictor the last executed branch at PC 'pc' and with
// outcome 'outcome' (true indicates that the branch was taken, false
// indicates that the branch was not taken)
//
void
train_predictor(uint32_t pc, uint8_t outcome)
{
  //
  //TODO: Implement Predictor training
  //
  switch (bpType) {
    case STATIC:
      break;
    case BIMODAL:
      train_bimodal(pc, outcome);
      break;
    case GSHARE:
    case TOURNAMENT:
    case CUSTOM:
    default:
	    assert(false && "Not implemented");
  }
}

void
cleanup_predictor()
{
  switch (bpType) {
    case STATIC:
      break;
    case BIMODAL:
      cleanup_bimodal();
      break;
    case GSHARE:
    case TOURNAMENT:
    case CUSTOM:
    default:
	    assert(false && "Not implemented");
  }
}
