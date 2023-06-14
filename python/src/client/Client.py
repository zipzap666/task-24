from PyQt5.QtCore import QObject, pyqtSlot, pyqtSignal, QTimer
from PyQt5.QtNetwork import QTcpSocket
from google.protobuf.internal.encoder import _VarintBytes

import sys
from os.path import dirname, join, abspath
sys.path.insert(0, abspath(join(dirname(__file__), '..')))

from common.parser.parsTools import *
from common.proto.message_pb2 import WrapperMessage


class Client(QObject):
    def __init__(self):
        super().__init__()
        self.socket = QTcpSocket()
        self.parser = DelimitedMessagesStreamParser(WrapperMessage)
        self.socket.connected.connect(self.onConnected)
        self.socket.readyRead.connect(self.onReadyRead)
        self.socket.disconnected.connect(self.startReconnect)
        self.timeOut = 0
        self.iterations = 0
        self.reconnectTimer = QTimer()
        self.reconnectTimer.timeout.connect(self.reconnect)
        self.address = ''
        self.port = 0

        self.timerSlow = QTimer()
        self.timerSlow.timeout.connect(self.onSendSlowRequest)
        self.timeToSleep = 0

        self.timerFast = QTimer()
        self.timerFast.timeout.connect(self.onSendFastRequest)

    messageParsed = pyqtSignal(str, arguments=['message'])
    logGui = pyqtSignal(str, arguments=['message'])

    @pyqtSlot(str, int, int)
    def connectToHost(self, address, port, timeOut):
        self.logGui.emit(f"INFO: CONNECTING TO {address}:{port}")
        self.address = address
        self.port = port
        self.timeOut = timeOut
        self.connect()

    def connect(self):
        self.socket.close()
        self.socket.connectToHost(self.address,  self.port)

    def onConnected(self):
        self.reconnectTimer.stop()

    def reconnect(self):
        if self.iterations >= self.timeOut:
            self.socket.close()
            self.reconnectTimer.stop()
        else:
            self.logGui.emit(f"INFO: TRY TO RECONNECT TO {self.address}:{self.port} ATTEMP: {self.iterations + 1}")
            self.iterations += 1
            self.connect()

    def startReconnect(self):
        self.stopSending()
        self.logGui.emit(f"INFO: CONNECTION LOST WITH {self.address}:{self.port}")
        self.iterations = 0
        self.reconnectTimer.start(1000)
        
    @pyqtSlot(int)
    def sendFastRequest(self, time):
        if not self.socket.isOpen():
            return
        self.stopSending()
        self.logGui.emit(f"INFO: START SENDING RESPONSE FOR FAST REQUEST")
        if not self.socket.isOpen():
            return
        if time < 10:
            time = 10
        
        self.timerFast.start(time)

    def onSendFastRequest(self):
        wrapperMessage = WrapperMessage()
        wrapperMessage.request_for_fast_response.SetInParent()
        message = _VarintBytes(wrapperMessage.ByteSize()) + wrapperMessage.SerializeToString()

        self.socket.write(message)
        self.logGui.emit(f"INFO: SENDED RESPONSE FOR FAST REQUEST")

    @pyqtSlot(int, int)
    def sendSlowRequest(self, timeToSleep, time):
        if not self.socket.isOpen():
            return
        self.stopSending()
        self.logGui.emit(f"INFO: START SENDING RESPONSE FOR SLOW REQUEST")
        if not self.socket.isOpen():
            return
        if time < 10:
            time = 10
        
        self.timeToSleep = timeToSleep
        self.timerSlow.start(time)

    def onSendSlowRequest(self):
        wrapperMessage = WrapperMessage()
        wrapperMessage.request_for_slow_response.time_in_seconds_to_sleep = self.timeToSleep
        message = _VarintBytes(wrapperMessage.ByteSize()) + wrapperMessage.SerializeToString()

        self.socket.write(message)
        self.logGui.emit(f"INFO: SENDED RESPONSE FOR SLOW REQUEST")

    @pyqtSlot()
    def stopSending(self):
        self.logGui.emit(f"INFO: STOP SENDING")
        self.timerFast.stop()
        self.timerSlow.stop()

    def onReadyRead(self):
        self.logGui.emit(f"INFO: DATA RECEIVED")
        message = self.socket.readAll().data()
        data = self.parser.parse(message)
        for item in data:
            if item.HasField("slow_response"):
                self.messageParsed.emit(str(item.slow_response.connected_client_count))
            elif item.HasField("fast_response"):
                self.messageParsed.emit(str(item.fast_response.current_date_time))