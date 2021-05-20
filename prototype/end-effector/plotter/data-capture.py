from time import time
import serial
import csv


ATM_THRESH = 105  # Unsigned 10-bit atmospheric pressure upper threshold
ATM_COUNT_THRESH = 10  # Number of consecutive atmospheric pressure readings to end test
atm_pres_count = 0  # Count number of consecutive atmospheric pressure readings
pressure_data = []  # Raw unsigned 10-bit ADC pressure readings
timestamps = []  # Timestamps for pressure data
timestamp_offset = 0  # First timestamp


# Initialise serial communication
ser = serial.Serial()
ser.baudrate = 19200
ser.port = 'COM11'
ser.open()

while True:
    # Receive and store pressure readings and timestamps
    pressure = int.from_bytes(ser.read(2), byteorder='big', signed=False)
    timestamp = time()  # Time since epoch (s)
    pressure_data.append(pressure)
    if len(timestamps) == 0:
        timestamp_offset = timestamp
    timestamps.append(timestamp - timestamp_offset)
    print(pressure, ':', timestamps[len(timestamps) - 1])

    # Check if pressure is at atmospheric levels (i.e. cube has been dropped)
    if pressure <= ATM_THRESH:
        atm_pres_count += 1
        if atm_pres_count == ATM_COUNT_THRESH:
            break
    else:
        atm_pres_count = 0

# Write pressure data to file
with open('pressure-data.csv', 'w') as file:
    wr = csv.writer(file)
    wr.writerow(pressure_data)
    wr.writerow(timestamps)
