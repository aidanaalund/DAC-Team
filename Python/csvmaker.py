import serial as ps
import csv

"""
#creates data
ser = ps.Serial(arduino_port, baud)
print("Connected to Arduino port:" + arduino_port)
file = open(newFileName, "w") #w overwrites, a appends data
print("Created file")
"""
arduino_port = "COM8"
baud = 9600
fileName = 'test.csv'
samples = 10
print_labels = False

#TODO: make a method that will create a .csv file from Arduino serial output
