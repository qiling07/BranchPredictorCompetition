#!/bin/bash

# Check if the directory path is provided as an argument
if [ -z "$1" ]; then
    echo "Usage: $0 <directory_containing_traces>"
    exit 1
fi

# Directory containing the trace files and path to the predictor program
TRACE_DIR="$1"
PREDICTOR="./predictor"

# Start and end values for bimodal history lengths
START_HISTORY=5
END_HISTORY=20

# Output file to store the results
OUTPUT_FILE="bimodal_test_results.csv"

# Clear the output file if it exists and add the header
> "$OUTPUT_FILE"
echo "TESTCASE,history_bits,misp_rate" >> "$OUTPUT_FILE"

# Loop through each trace file in the specified directory
for TRACE_FILE in "$TRACE_DIR"/*.bz2
do
    TESTCASE=$(basename "$TRACE_FILE")
    
    # Loop through the specified range of bimodal history lengths
    for ((historyLen=$START_HISTORY; historyLen<=$END_HISTORY; historyLen++))
    do
        echo "Testing $TESTCASE with --bimodal:$historyLen ..."
        
        # Run the predictor command and extract the misprediction rate
        misp_rate=$(bunzip2 -kc "$TRACE_FILE" | "$PREDICTOR" --bimodal:$historyLen | grep "Misprediction Rate" | awk '{print $3}')
        
        # Print the result to the console
        echo "Trace: $TESTCASE, History Bits: $historyLen, Misprediction Rate: $misp_rate%"

        # Write the result to the output file
        echo "$TESTCASE,$historyLen,$misp_rate" >> "$OUTPUT_FILE"
    done
done

echo "Testing completed. Results saved to $OUTPUT_FILE."
