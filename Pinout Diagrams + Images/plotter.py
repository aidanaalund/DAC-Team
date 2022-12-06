import matplotlib.pyplot as plt
import csv
import numpy as np

"""
When using PuTTY to make the .csv this will read, uncheck 'Include Header', 
change session logging to 'printable output', and make sure the file name given 
matches with the file name used in this code (e.g temperaturedata.csv)

Change the variable fileName as needed to use with different files
"""

x = []
y = []
headerCheck = 0
plotTitle = ""
fileName = 'temperaturedata.csv'

  
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
