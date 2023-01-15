import serial as ps
import csv

#initializes reader from the provided USB port
arduino_port = 'COM5'
baud = 9600
fileName = 'test.csv'
samples = 10
print_labels = False
points = []
arduino = ps.Serial(arduino_port, baud)
print("Connected to Arduino port: " + arduino_port)

#reads the serial output
while(samples > 0):
    #because we're reading byte data from the serial monitor, we have to 'decode' it
    values = arduino.readline().decode('utf-8').rstrip().split()

    #points is a list of lists
    points.append(values)
    samples = samples-1
    
with open(fileName, 'w', newline='') as csv_file:
    writer = csv.writer(csv_file)
    writer.writerows(points)

print("Created file")
