from random import randint

import matplotlib.pyplot as plt
from utility import *

filename = input("File name: ")
command = ["./steps.exe", filename]
result = subprocess.run(command, capture_output=True, text=True)
outputs = result.stdout.split()
x_points = [float(outputs[i]) for i in range(len(outputs)) if i % 2 == 0]
y_points = [float(outputs[i]) for i in range(len(outputs)) if i % 2 == 1]

plt.scatter(
    x_points, y_points, color="blue", marker="o", label=f"Image - {filename}", s=0.1
)
plt.plot(x_points, y_points, color="red", label="Connecting Line")
plt.title("Graph")
plt.xlabel("X values")
plt.ylabel("Y values")
plt.legend()
plt.show()
