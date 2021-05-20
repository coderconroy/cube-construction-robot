import csv
import matplotlib.pyplot as plt

# Read pressure data from file
pressure_data = None
timestamps = None
with open('pressure-data.csv', 'r') as file:
    reader = csv.reader(file, quoting=csv.QUOTE_NONE)
    row_count = 0
    for row in reader:
        if row_count == 0:
            pressure_data = row
        elif row_count == 2:
            timestamps = row
        row_count += 1

# Convert data strings to numerical format
pressure_data = [int(datum) for datum in pressure_data]
timestamps = [float(timestamp) / 3600 for timestamp in timestamps]

# Generate data plot
plt.plot(timestamps, pressure_data)
plt.ylabel('Pressure (10-bit ADC reading)')
plt.xlabel('Time (h)')
plt.xlim([0, 12])
plt.grid(True)
plt.show()
