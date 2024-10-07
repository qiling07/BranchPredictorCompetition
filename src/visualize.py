import pandas as pd
import matplotlib.pyplot as plt
import sys
import os

assert(len(sys.argv) == 2)
filename = sys.argv[1]
# Load the full data from the provided CSV file
data = pd.read_csv(filename)

# Plot the misprediction rates for different history lengths for each testcase
plt.figure(figsize=(12, 8))

# Plot each testcase's data as a separate line
for testcase in data['TESTCASE'].unique():
    testcase_data = data[data['TESTCASE'] == testcase]
    plt.plot(testcase_data['history_bits'], testcase_data['misp_rate'], label=testcase, marker='o')

# Set plot labels, title, and legend
plt.xlabel('History Bits Length')
plt.ylabel('Misprediction Rate (%)')
plt.title('Misprediction Rate vs. Num of History Bits')
plt.legend(title='Testcase', bbox_to_anchor=(1.05, 1), loc='upper left')
plt.grid(True)

# Display the plot
plt.tight_layout()
new_filename = os.path.splitext(filename)[0] + '.png'
plt.savefig(new_filename, dpi=400)

