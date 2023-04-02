"""
make sure pyserial is installed in your environment!
when running, make sure to have the sensors connected 
or comment out the makecsv line so the program will
only rely on a premade csv.
"""
import matplotlib.pyplot as plt
import csv
import numpy as np
import csvmaker

def plot(portname,file,samples,title):
    
    #initialize variables: x-axis, y-axis data for all 3 sensors
    x = []
    y1 = []
    y2 = []
    y3 = []
    headerCheck = 0
    plotTitle = title
    fileName = file
    port = portname
    
    #makes .csv
    #input into the method is port used, file name, and amount of data points
    csvmaker.makecsv(port,file,samples)
           
    #displays data
    with open(fileName,'r') as csvfile:
        lines = csv.reader(csvfile, delimiter=',')
        for row in lines:
            if headerCheck == 0:
                plotTitle = row[0]
                headerCheck += 1
            elif headerCheck == 1:
                plt.xlabel(row[0])
                plt.ylabel(row[1])
                headerCheck += 1
            else:
                x.append(int(row[0]))
                y1.append(float(row[1]))
                y2.append(float(row[2]))
                y3.append(float(row[3]))
   #creates the linspace data type from the time values 
    xlinspace = np.linspace(x[0],x[-1],len(x))
    
    #creates 3 plots using the data and styles them
    plt.plot(xlinspace, y1, color = 'g', linestyle = 'dashed',
             marker = 'o',label = "Sensor 1")

    plt.plot(xlinspace, y2, color = 'r', linestyle = 'dashed',
             marker = 'o',label = "Sensor 2")
    
    plt.plot(xlinspace, y3, color = 'b', linestyle = 'dashed',
             marker = 'o',label = "Sensor 3")
    
    #formats the graph and makes it visible
    plt.xticks(rotation = 25)
    plt.title(plotTitle, fontsize = 20)
    plt.grid()
    plt.legend()
    plt.show()
    
#if-name-main check when running directly    
if __name__ == '__main__':
    plot('COM5','test.csv',10,'test plot')
