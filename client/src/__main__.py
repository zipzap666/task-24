import os
from pathlib import Path
import sys
from Client import Client
from PyQt5.QtGui import QGuiApplication
from PyQt5.QtQml import QQmlApplicationEngine

if __name__ == "__main__":
    app = QGuiApplication(sys.argv)
    app.setOrganizationName('NIC')
    app.setOrganizationDomain('STUDENTS')
    engine = QQmlApplicationEngine()
    client = Client()

    engine.rootContext().setContextProperty("client", client)
    engine.load(os.fspath(Path(__file__).resolve().parent / "main.qml"))
    if not engine.rootObjects():
        sys.exit(-1)
    sys.exit(app.exec_())