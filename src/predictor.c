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

#define false 0
#define true 1
#define ST 3 // Strongly Taken (11)
#define WT 2 // Weakly Taken (10)
#define WN 1 // Weakly Not Taken (01)
#define SN 0 // Strongly Not Taken (00)

// bimodal branch predictor with 2-bit saturation counters
uint8_t *bht_bimodal;                   // Branch History Table (2-bit counters)
uint32_t bht_size_bimodal;              // Size of the Branch History Table
void
init_bimodal()
{
    bht_size_bimodal = 1 << bhistoryBits;      // Calculate the size of the BHT as 2^bhistoryBits
    bht_bimodal = (uint8_t *)malloc(bht_size_bimodal * sizeof(uint8_t));

    // Initialize all counters in the BHT to Weakly Taken (10)
    for (uint32_t i = 0; i < bht_size_bimodal; i++) {
        bht_bimodal[i] = WN;
    }
}

uint8_t
predict_bimodal(uint32_t pc)
{
    uint32_t index = pc & (bht_size_bimodal - 1); // Use the lower bits of PC to index into the BHT
    uint8_t counter = bht_bimodal[index];

    // Predict taken if the counter is in state WT or ST
    return (counter >= WT) ? 1 : 0; // 1 for taken, 0 for not taken
}

void
train_bimodal(uint32_t pc, uint8_t outcome)
{
    uint32_t index = pc & (bht_size_bimodal - 1); // Use the lower bits of PC to index into the BHT
    uint8_t counter = bht_bimodal[index];

    // Update the 2-bit saturating counter based on the actual outcome
    if (outcome == 1) { // If the actual outcome is taken
        if (counter < ST) {
            bht_bimodal[index]++; // Increment the counter towards Strongly Taken (ST)
        }
    } else { // If the actual outcome is not taken
        if (counter > SN) {
            bht_bimodal[index]--; // Decrement the counter towards Strongly Not Taken (SN)
        }
    }
}

void
cleanup_bimodal()
{
    assert(bht_bimodal != NULL);
    free(bht_bimodal);
    bht_bimodal = NULL;
}

// Gshare branch predictor with 2-bit saturation counters
uint8_t *bht_gshare;            // Branch History Table (2-bit counters) for Gshare
uint32_t bht_size_gshare;       // Size of the Branch History Table for Gshare
uint32_t ghr_gshare = 0;        // Global History Register for Gshare

void
init_gshare()
{
    bht_size_gshare = 1 << ghistoryBits;     // Calculate the size of the BHT as 2^ghistoryBits
    bht_gshare = (uint8_t *)malloc(bht_size_gshare * sizeof(uint8_t));

    // Initialize all counters in the BHT to Weakly Taken (10)
    for (uint32_t i = 0; i < bht_size_gshare; i++) {
        bht_gshare[i] = WN;
    }

    // Initialize the GHR to zero
    ghr_gshare = 0;
}

uint8_t
predict_gshare(uint32_t pc)
{
    // XOR the global history register with the lower bits of the PC
    uint32_t index = (pc ^ (ghr_gshare)) & (bht_size_gshare - 1);  // Ensure we index within the BHT bounds
    uint8_t counter = bht_gshare[index];

    // Predict taken if the counter is in state WT or ST
    return (counter >= WT) ? 1 : 0; // 1 for taken, 0 for not taken
}

void
train_gshare(uint32_t pc, uint8_t outcome)
{
    // XOR the global history register with the lower bits of the PC
    uint32_t index = (pc ^ (ghr_gshare)) & (bht_size_gshare - 1);  // Ensure we index within the BHT bounds
    uint8_t counter = bht_gshare[index];

    // Update the 2-bit saturating counter based on the actual outcome
    if (outcome == 1) { // If the actual outcome is taken
        if (counter < ST) {
            bht_gshare[index]++; // Increment the counter towards Strongly Taken (ST)
        }
    } else { // If the actual outcome is not taken
        if (counter > SN) {
            bht_gshare[index]--; // Decrement the counter towards Strongly Not Taken (SN)
        }
    }

    // Update the Global History Register (shift left and add the new outcome)
    ghr_gshare = ((ghr_gshare << 1) | outcome) & ((1 << ghistoryBits) - 1);  // Keep only ghistoryBits bits
}

void
cleanup_gshare()
{
    assert(bht_gshare != NULL);
    free(bht_gshare);
    bht_gshare = NULL;
}

// Gshare branch predictor with 2-bit saturation counters
uint8_t *bht_gshare2;            // Branch History Table (2-bit counters) for Gshare
uint32_t bht_size_gshare2;       // Size of the Branch History Table for Gshare
uint32_t ghr_gshare2 = 0;        // Global History Register for Gshare

void
init_gshare2()
{
    bht_size_gshare2 = 1 << 12;     // Calculate the size of the BHT as 2^ghistoryBits
    bht_gshare2 = (uint8_t *)malloc(bht_size_gshare2 * sizeof(uint8_t));

    // Initialize all counters in the BHT to Weakly Taken (10)
    for (uint32_t i = 0; i < bht_size_gshare2; i++) {
        bht_gshare2[i] = WN;
    }

    // Initialize the GHR to zero
    ghr_gshare2 = 0;
}

uint8_t
predict_gshare2(uint32_t pc)
{
    // XOR the global history register with the lower bits of the PC
    uint32_t index = (pc ^ (ghr_gshare2 << 9)) & (bht_size_gshare2 - 1);  // Ensure we index within the BHT bounds
    uint8_t counter = bht_gshare2[index];

    // Predict taken if the counter is in state WT or ST
    return (counter >= WT) ? 1 : 0; // 1 for taken, 0 for not taken
}

void
train_gshare2(uint32_t pc, uint8_t outcome)
{
    // XOR the global history register with the lower bits of the PC
    uint32_t index = (pc ^ (ghr_gshare2 << 9)) & (bht_size_gshare2 - 1);  // Ensure we index within the BHT bounds
    uint8_t counter = bht_gshare2[index];

    // Update the 2-bit saturating counter based on the actual outcome
    if (outcome == 1) { // If the actual outcome is taken
        if (counter < ST) {
            bht_gshare2[index]++; // Increment the counter towards Strongly Taken (ST)
        }
    } else { // If the actual outcome is not taken
        if (counter > SN) {
            bht_gshare2[index]--; // Decrement the counter towards Strongly Not Taken (SN)
        }
    }

    // Update the Global History Register (shift left and add the new outcome)
    ghr_gshare2 = ((ghr_gshare2 << 1) | outcome) & (bht_size_gshare2 - 1);  // Keep only ghistoryBits bits
}

void
cleanup_gshare2()
{
    assert(bht_gshare2 != NULL);
    free(bht_gshare2);
    bht_gshare2 = NULL;
}

// tournament branch predictor with 2-bit saturation counters
uint8_t *global_pht_tournament;  // Global Pattern History Table (PHT) for tournament
uint8_t *local_pht_tournament;   // Local Pattern History Table (PHT) for tournament
uint32_t *lht_tournament;        // Local History Table (LHT) for tournament
uint8_t *choice_pht_tournament;  // Choice predictor to choose between global and local
uint32_t ghr_tournament;         // Global History Register for tournament

// Sizes for the tables
uint32_t global_pht_size_tournament;
uint32_t local_pht_size_tournament;
uint32_t lht_size_tournament;
uint32_t choice_pht_size_tournament;

void
init_tournament()
{
    // Initialize sizes for the tables based on the configuration parameters
    global_pht_size_tournament = 1 << ghistoryBits;
    local_pht_size_tournament = 1 << lhistoryBits;
    lht_size_tournament = 1 << pcIndexBits;
    choice_pht_size_tournament = 1 << ghistoryBits;

    // Allocate memory for the tables
    global_pht_tournament = (uint8_t *)malloc(global_pht_size_tournament * sizeof(uint8_t));
    local_pht_tournament = (uint8_t *)malloc(local_pht_size_tournament * sizeof(uint8_t));
    lht_tournament = (uint32_t *)malloc(lht_size_tournament * sizeof(uint32_t));
    choice_pht_tournament = (uint8_t *)malloc(choice_pht_size_tournament * sizeof(uint8_t));

    // Initialize all entries in the tables to their default states
    for (uint32_t i = 0; i < global_pht_size_tournament; i++) {
        global_pht_tournament[i] = WN; // Weakly Not Taken
    }
    for (uint32_t i = 0; i < local_pht_size_tournament; i++) {
        local_pht_tournament[i] = WN; // Weakly Not Taken
    }
    for (uint32_t i = 0; i < lht_size_tournament; i++) {
        lht_tournament[i] = 0; // Initialize local history to zero
    }
    for (uint32_t i = 0; i < choice_pht_size_tournament; i++) {
        choice_pht_tournament[i] = WN; // Weakly favor global predictor initially
    }

    // Initialize the global history register to zero
    ghr_tournament = 0;

}

uint8_t
predict_tournament(uint32_t pc)
{
    // Index into the local history table using the PC
    uint32_t local_history_index = pc & (lht_size_tournament - 1);
    uint32_t local_history = lht_tournament[local_history_index];
    uint32_t local_pht_index = local_history & (local_pht_size_tournament - 1);

    // Index into the global PHT and choice PHT using the global history register
    uint32_t global_pht_index = ghr_tournament & (global_pht_size_tournament - 1);
    uint32_t choice_index = ghr_tournament & (choice_pht_size_tournament - 1);

    // Get predictions from the local, global, and choice predictors
    uint8_t local_prediction = (local_pht_tournament[local_pht_index] >= WT) ? 1 : 0;
    uint8_t global_prediction = (global_pht_tournament[global_pht_index] >= WT) ? 1 : 0;
    uint8_t choice = choice_pht_tournament[choice_index];

    // Use the choice predictor to select between local and global predictions
    return (choice >= WT) ? local_prediction : global_prediction;
}

void
train_tournament(uint32_t pc, uint8_t outcome)
{
    // Update indices for local and global predictors
    uint32_t local_history_index = pc & (lht_size_tournament - 1);
    uint32_t local_history = lht_tournament[local_history_index];
    uint32_t local_pht_index = local_history & (local_pht_size_tournament - 1);

    uint32_t global_pht_index = ghr_tournament & (global_pht_size_tournament - 1);
    uint32_t choice_index = ghr_tournament & (choice_pht_size_tournament - 1);

    // Get current predictions
    uint8_t local_prediction = (local_pht_tournament[local_pht_index] >= WT) ? 1 : 0;
    uint8_t global_prediction = (global_pht_tournament[global_pht_index] >= WT) ? 1 : 0;

    // Update the choice predictor based on which prediction was correct
    if (local_prediction != global_prediction) {
        if (global_prediction == outcome) {
            if (choice_pht_tournament[choice_index] > SN) choice_pht_tournament[choice_index]--;
        } else {
            if (choice_pht_tournament[choice_index] < ST) choice_pht_tournament[choice_index]++;
        }
    }

    // Update the local and global PHTs with the actual outcome
    if (outcome == 1) { // Branch taken
        if (local_pht_tournament[local_pht_index] < ST) local_pht_tournament[local_pht_index]++;
        if (global_pht_tournament[global_pht_index] < ST) global_pht_tournament[global_pht_index]++;
    } else { // Branch not taken
        if (local_pht_tournament[local_pht_index] > SN) local_pht_tournament[local_pht_index]--;
        if (global_pht_tournament[global_pht_index] > SN) global_pht_tournament[global_pht_index]--;
    }

    // Update the local history table and the global history register
    lht_tournament[local_history_index] = ((local_history << 1) | outcome) & ((1 << lhistoryBits) - 1);
    ghr_tournament = ((ghr_tournament << 1) | outcome) & ((1 << ghistoryBits) - 1);
}

void
cleanup_tournament()
{
    free(global_pht_tournament);
    free(local_pht_tournament);
    free(lht_tournament);
    free(choice_pht_tournament);
}

// hybrid branch predictor 
uint8_t *choice_pht_hybrid;  // Choice predictor to choose between global and local
uint32_t ghr_hybrid;         // Global History Register for tournament

// Sizes for the tables
uint32_t choice_pht_size_hybrid;

void
init_hybrid()
{
    ghistoryBits = 12;
    lhistoryBits = 12;
    pcIndexBits = 11;

    // Initialize sizes for the tables based on the configuration parameters
    choice_pht_size_hybrid = 1 << 12;

    choice_pht_hybrid = (uint8_t *)malloc(choice_pht_size_hybrid * sizeof(uint8_t));

    // Initialize all entries in the tables to their default states
    for (uint32_t i = 0; i < choice_pht_size_hybrid; i++) {
        choice_pht_hybrid[i] = SN;
    }

    // Initialize the global history register to zero
    ghr_hybrid = 0;

    init_tournament();
    init_gshare2();
}

uint8_t
predict_hybrid(uint32_t pc)
{
    uint32_t choice_index = pc & (choice_pht_size_hybrid - 1);

    uint8_t tournament_prediction = predict_tournament(pc);
    uint8_t gshare_prediction = predict_gshare2(pc);
    uint8_t choice = choice_pht_hybrid[choice_index];

    // Use the choice predictor to select between local and global predictions
    return (choice >= WT) ? gshare_prediction : tournament_prediction;
}

void
train_hybrid(uint32_t pc, uint8_t outcome)
{
    uint32_t choice_index = pc & (choice_pht_size_hybrid - 1);

    // Get current predictions
    uint8_t tournament_prediction = predict_tournament(pc);
    uint8_t gshare_prediction = predict_gshare2(pc);

    // Update the choice predictor based on which prediction was correct
    if (tournament_prediction != gshare_prediction) {
        if (tournament_prediction == outcome) {
            if (choice_pht_hybrid[choice_index] > SN) choice_pht_hybrid[choice_index]--;
        } else {
            if (choice_pht_hybrid[choice_index] < ST) choice_pht_hybrid[choice_index]++;
        }
    }

    // Update the local history table and the global history register
    ghr_hybrid= ((ghr_hybrid << 1) | outcome) & ((1 << ghistoryBits) - 1);

    train_tournament(pc, outcome);
    train_gshare2(pc, outcome);
}

void
cleanup_hybrid()
{
	cleanup_tournament();
	cleanup_gshare2();
	free(choice_pht_hybrid);
}

// Custom Perceptron predictor
int ghistoryBits_perceptron;   // Number of bits for global history
int num_perceptrons;           // Number of perceptrons in the table
int perceptron_threshold;      // Threshold for training the perceptrons

// Components of the Perceptron Branch Predictor
int8_t **perceptron_table;     // Table of perceptrons (weights)
uint32_t ghr_perceptron;       // Global History Register for perceptron predictor

// Initialize the Perceptron Branch Predictor
void init_perceptron()
{
    ghistoryBits_perceptron = 58;
    num_perceptrons = 1<<12;
    perceptron_threshold = 70;

    // Allocate memory for the perceptron table
    perceptron_table = (int8_t **)malloc(num_perceptrons * sizeof(int8_t *));
    for (int i = 0; i < num_perceptrons; i++) {
        perceptron_table[i] = (int8_t *)malloc((ghistoryBits_perceptron + 1) * sizeof(int8_t));
        for (int j = 0; j <= ghistoryBits_perceptron; j++) {
            perceptron_table[i][j] = 0; // Initialize all weights to 0
        }
    }

    // Initialize the Global History Register to zero
    ghr_perceptron = 0;
}

// Predict the outcome of a branch using the Perceptron Branch Predictor
uint8_t predict_perceptron(uint32_t pc)
{
    int perceptron_index = pc % num_perceptrons;  // Select a perceptron based on the PC
    int8_t *weights = perceptron_table[perceptron_index];

    // Compute the dot product of the weights and the global history
    int y = weights[0];  // Bias term
    for (int i = 0; i < ghistoryBits_perceptron; i++) {
        int history_bit = (ghr_perceptron >> i) & 1;
        y += weights[i + 1] * (history_bit ? 1 : -1);
    }

    // Predict taken if y > 0, not taken otherwise
    return (y >= 0) ? 1 : 0;
}

// Train the Perceptron Branch Predictor based on the actual outcome
void train_perceptron(uint32_t pc, uint8_t outcome)
{
    int perceptron_index = pc % num_perceptrons;  // Select a perceptron based on the PC
    int8_t *weights = perceptron_table[perceptron_index];

    // Compute the dot product of the weights and the global history to get y
    int y = weights[0];  // Bias term
    for (int i = 0; i < ghistoryBits_perceptron; i++) {
        int history_bit = (ghr_perceptron >> i) & 1;
        y += weights[i + 1] * (history_bit ? 1 : -1);
    }

    // Convert the branch outcome to +1 for taken and -1 for not taken
    int actual = outcome ? 1 : -1;

    // Update weights if the prediction was incorrect or |y| <= threshold
    if ((y >= 0) != (actual == 1) || abs(y) <= perceptron_threshold) {
        weights[0] += actual;  // Update the bias term
        for (int i = 0; i < ghistoryBits_perceptron; i++) {
            int history_bit = (ghr_perceptron >> i) & 1;
            weights[i + 1] += actual * (history_bit ? 1 : -1);
        }
    }

    // Update the Global History Register with the actual outcome
    ghr_perceptron = ((ghr_perceptron << 1) | outcome) & ((1 << ghistoryBits_perceptron) - 1);
}

// Free the allocated memory for the Perceptron Branch Predictor
void cleanup_perceptron()
{
    for (int i = 0; i < num_perceptrons; i++) {
        free(perceptron_table[i]);
    }
    free(perceptron_table);
}

//------------------------------------//
//        Predictor Functions         //
//------------------------------------//

// Initialize the predictor
//
void
init_predictor()
{
  switch (bpType) {
    case STATIC:
	    break;
    case BIMODAL:
	    init_bimodal();
	    break;
    case GSHARE:
	    init_gshare();
	    break;
    case TOURNAMENT:
	    init_tournament();
	    break;
    case CUSTOM:
	    // init_tage();
	    // init_perceptron();
	    init_hybrid();
	    break;
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
  // Make a prediction based on the bpType
  switch (bpType) {
    case STATIC:
      return TAKEN;
    case BIMODAL:
      return predict_bimodal(pc);
    case GSHARE:
      return predict_gshare(pc);
    case TOURNAMENT:
      return predict_tournament(pc);
    case CUSTOM:
      // return predict_tage(pc);
      // return predict_perceptron(pc);
      return predict_hybrid(pc);
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
  switch (bpType) {
    case STATIC:
      break;
    case BIMODAL:
      train_bimodal(pc, outcome);
      break;
    case GSHARE:
      train_gshare(pc, outcome);
      break;
    case TOURNAMENT:
      train_tournament(pc, outcome);
      break;
    case CUSTOM:
      // train_tage(pc, outcome);
      // train_perceptron(pc, outcome);
      train_hybrid(pc, outcome);
      break;
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
      cleanup_gshare();
      break;
    case TOURNAMENT:
      cleanup_tournament();
      break;
    case CUSTOM:
      // cleanup_tage();
      // cleanup_perceptron();
      cleanup_hybrid();
      break;
    default:
	    assert(false && "Not implemented");
  }
}
