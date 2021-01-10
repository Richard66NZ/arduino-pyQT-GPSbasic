#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
@date: 10 January 2021

This source code is provided by Richard J Smith 'as is' and 'with all faults'. The provider makes no 
representations or warranties of any kind concerning the safety, suitability, inaccuracies, 
typographical errors, or other harmful components of this software.

@author: Richard J Smith

The following code has been trialled only on macOS system and will not function correctly on Windows. 
This code is only intended as a quick/rough demonstration of - using an Arduino UNO to measure GPS data, 
connecting from a Python pyQT script to an Arduino UNO board via serial connection, transfering GPS data
one way from Arduino UNO to Python script running on macOS, using QT Designer to layout windows for 
display of data.
"""

from PyQt5 import QtCore, QtSerialPort
from PyQt5.QtWidgets import QMainWindow, QApplication
from PyQt5.uic import loadUi
import serial.tools.list_ports
import arrow

class MyWindow(QMainWindow):
    
    def __init__(self):   
        QMainWindow.__init__(self)
        loadUi("GPSviewer.ui",self)

        self.setWindowTitle("GPS display in Python from Arduino")
        self.Cbutton.clicked.connect(self.on_connect)
        self.Dbutton.clicked.connect(self.on_disconnect)
        
        self.Qbutton.clicked.connect(self.on_Quitclick)        
        self.GPSLabel_1.setText('----')
        self.GPSLabel_2.setText('----')
        self.GPSLabel_6.setText('----')
        self.GPSLabel_3.setText('----')
        self.GPSLabel_4.setText('----')
        self.GPSLabel_5.setText('----')
        self.GPSLabel_7.setText('----')
        self.GPSLabel_8.setText('----')  
        self.Dbutton.setEnabled(False)
        self.MessageLabel.setText('Welcome to GPS data viewer')
        
        #list available serial ports and add to comboBox
        ports = serial.tools.list_ports.comports()
        for port, desc, hwid in sorted(ports):
            self.comboBox.addItem("{}".format(port))            
              
    @QtCore.pyqtSlot()
    def receive(self):
        while self.serial.canReadLine():
            text = self.serial.readLine().data().decode()
            text = text.rstrip('\r\n')

            if text.startswith('##'):
                self.MessageLabel.setText('')
                x = text[2:][:-3].split(",")
                print('\nLatitude = ', x[2]) # latitude
                print('Longtitude = ', x[4]) # longtitude
                print('Satellites = ', x[6]) #         
                print('Speed (knots) = ', x[8]) #         
                print('Speed (km/h) = ', x[10]) #         
                print('Altitude = ', x[12]) #         
                dateToPrint = arrow.get(x[16], 'D/M/YYYY')            
                print('Date = ', dateToPrint.format('D MMM YYYY')) #    
                timeToPrint = arrow.get(x[18], 'H:m:s')                    
                print('Time = ', timeToPrint.format('HH:mm:ss')) #        

                self.GPSLabel_1.setText(x[2])
                self.GPSLabel_2.setText(x[4])
                self.GPSLabel_6.setText(x[6])
                self.GPSLabel_3.setText(x[12])
                self.GPSLabel_4.setText(x[8])
                self.GPSLabel_5.setText(x[10])
                self.GPSLabel_7.setText(dateToPrint.format('D MMM YYYY'))
                self.GPSLabel_8.setText(timeToPrint.format('HH:mm:ss'))
  
    def on_connect(self): #connect button has been pressed
        self.serial = QtSerialPort.QSerialPort(
            self.comboBox.currentText(),
            baudRate=QtSerialPort.QSerialPort.Baud9600,
            readyRead=self.receive
        )        
        self.Cbutton.setEnabled(False)
        self.Dbutton.setEnabled(True)
        self.MessageLabel.setText('Trying to connect to Arduino') #Trying to connect to Arduino - please wait
        self.MessageLabel.repaint()
        self.Cbutton.repaint()
        self.Dbutton.repaint()
        if not self.serial.isOpen():
            if not self.serial.open(QtCore.QIODevice.ReadWrite):
                self.button.setChecked(False)   

    def on_disconnect(self): #disconnect button has been pressed
        self.GPSLabel_1.clear()
        self.GPSLabel_1.setText('----')
        self.GPSLabel_2.setText('----')
        self.GPSLabel_6.setText('----')
        self.GPSLabel_3.setText('----')
        self.GPSLabel_4.setText('----')
        self.GPSLabel_5.setText('----')        
        self.GPSLabel_7.setText('----')
        self.GPSLabel_8.setText('----')  
        self.GPSLabel_1.repaint()     
        self.serial.close()  
        self.Cbutton.setEnabled(True)
        self.Dbutton.setEnabled(False)
        self.Cbutton.repaint()
        self.Dbutton.repaint()
                    
    def on_Quitclick(self): #Quit button has been pressed         
        self.serial.close()
        self.close()

if __name__ == '__main__':
    import sys
    app = QApplication([])
    w = MyWindow()
    w.show()
    sys.exit(app.exec_())