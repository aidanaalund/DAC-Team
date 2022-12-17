import serial as ps
import csv


#creates data
arduino_port = 'COM8'
baud = 9600
fileName = 'test.csv'
samples = 10
print_labels = False
arduino = ps.Serial(arduino_port, baud)
print("Connected to Arduino port: " + arduino_port)

#TODO: make a method that will create a .csv file from Arduino serial output
while(samples > 0):
    values = arduino.readline().split(',')
    time_ms = values[0]
    temp_c = values[1]
    
    with open(fileName, 'w', newline='') as csv_file:
        writer = csv.writer(csv_file)
        writer.writerow([time_ms, temp_c])
    samples = samples-1



print("Created file")
