"""
make sure pyserial is installed in your environment!
when running, make sure to have the sensors connected 
or comment out the makecsv line so the program will
only rely on a premade csv.
"""
import matplotlib.pyplot as plt
import csv
import numpy as np
import pandas as pd
import csvmaker
import scipy.stats as stats

def plot(portname,file,samples,title):
    
    # initialize variables: x-axis, y-axis data for all 3 sensors
    plotTitle = title
    fileName = file
    port = portname
    
    # makes .csv
    # input into the method is port used, file name, and amount of data points
    #csvmaker.makecsv(port,file,samples)
    
    # 'df' refers to dataframe, a data type in pandas
    # think of it like a spreadsheet of all of our data!
    df = pd.read_csv(fileName)
    print(df.head())
    
    # create a plot that consists of all temperature lines
    fig, ax = plt.subplots()
    plt.ylabel('Temperature (C)')
    df.plot.line(ax=ax, x='Time (ms)', y='Temperature (C)', marker = 'o')
    df.plot.line(ax=ax, x='Time (ms)', y='Temperature2 (C)', marker = 'o')
    df.plot.line(ax=ax, x='Time (ms)', y='Temperature3 (C)', marker = 'o')
    plt.grid(True)
    plt.legend(loc='upper center')
    plt.show()
    
    # TODO: create new plots as we hook up new sensors!
 
    # TODO: code for saving the plot(s) as a png

# if-name-main check when running directly    
if __name__ == '__main__':
    plot('COM5','test.csv',10,'Temperature vs. Time')
