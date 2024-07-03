from utility import *
import matplotlib.pyplot as plt
import numpy as np
import numpy as np
import seaborn as sns
import subprocess

table = []
filename = input("Give me a filename: ")
err = input("Epsilon (sample): ")
shift = input("Shift: ")
tests = int(input("Number of tests: "))

sum = 0

for i in range(tests):
    command = ["./approximate.exe", filename, err, shift]
    result = subprocess.run(command, capture_output=True, text=True)
    dist = float(result.stdout)
    table.append(dist)
    sum += dist
print(sum / tests)
x_values = table

plt.figure(figsize=(16, 9))
sns.kdeplot(
    x_values,
    label=f"Underlying Distribution (Actual distance: {filename.split('_')[1]})",
    color="blue",
)

plt.scatter(x_values, np.zeros_like(x_values), label="Samples", color="red")

plt.xlabel("Values")
plt.ylabel("Frequency")
plt.title("Approximating Underlying Distribution from Samples")

plt.legend()

plt.grid(True)
plt.show()
