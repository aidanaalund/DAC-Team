import matplotlib.pyplot as plt
import csv
import numpy as np
import csvmaker
#random

def plot(portname,file,samples,title):
    
    #initialize variables
    x = []
    y = []
    headerCheck = 0
    plotTitle = title
    fileName = file
    port = portname
    
    #makes .csv
    #input into the method is port used, file name, and amount of data points
    csvmaker.makecsv(port,file,samples)
           
    #displays data
    #TODO: add the ability to define the amount of lines we're plotting e.g. 3 different sensor values
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
                y.append(float(row[1]))
                
    
    x2 = np.linspace(x[0],x[-1],len(x))
    
    plt.plot(x2, y, color = 'g', linestyle = 'dashed',
             marker = 'o',label = "Trend Line")
    
    plt.xticks(rotation = 25)
    plt.title(plotTitle, fontsize = 20)
    plt.grid()
    plt.legend()
    plt.show()
    
if __name__ == '__main__':
    plot('COM5','test.csv',10,'test plot')
