"""
 Make sure all of the libraries are installed in your environment.
 The makecsv function call is currently commented out since we are
 not collecting and plotting data simultaneously. If you want to do that,
 simply uncomment the line.
"""
import matplotlib.pyplot as plt
import csv
import numpy as np
import pandas as pd
import csvmaker
import scipy.stats as stats
import seaborn as sns
import scienceplots

# Plots all of the data in the csv


def plot(portname, file, samples):

    # initialize variables: x-axis, y-axis data for all 3 sensors
    fileName = file
    port = portname

    # makes .csv
    # input into the method is port used, file name, and amount of data points
    # csvmaker.makecsv(port,file,samples)

    # 'df' refers to dataframe, a data type in pandas
    # think of it like a spreadsheet of all of our data!
    # convert the csv to a dataframe, and print the first 5 rows of it
    df = pd.read_csv(fileName)
    print(df.head())
    plt.rcParams['figure.dpi'] = 200
    # Sets the style
    plt.style.use(['science', 'no-latex'])

    # Creates a plot that consists of all temperature lines
    # Currently uses matplotlib as the plotting backend.
    # To specify another backend (Seaborn, Bokeh, ect.), input it as an argument
    # Or set a session wide backend with pd.options.plotting.backend
    if 'Temperature (C)' in df.columns:
        plt.rc('xtick', labelsize=8)
        plt.rc('ytick', labelsize=8)
        fig, ax = plt.subplots()
        plt.ylabel('Temperature (C)')
        df.plot.line(ax=ax, x='Time (ms)', y='Temperature (C)', marker='.')
        if 'Temperature2 (C)' in df.columns:
            df.plot.line(ax=ax, x='Time (ms)',
                         y='Temperature2 (C)', marker='.')
        if 'Temperature3 (C)' in df.columns:
            df.plot.line(ax=ax, x='Time (ms)',
                         y='Temperature3 (C)', marker='.')
        plt.grid(True)
        plt.legend(loc='upper center', fontsize="6")
        plt.show()

    # TODO: create new plots as we hook up new sensors!

    # TODO: CO2 plot here

    # TODO: Humidity plot here

    # TODO: Flow plot here

    # TODO: code for saving the plot(s) as a png


# if-name-main check when running directly
if __name__ == '__main__':
    # this uses USB port COM5 to collect 10 samples and write it to test.csv
    # currently the title argument is unused in the code, but feel free to use it
    plot('COM5', 'test.csv', 10)
