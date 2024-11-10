import sys
from PyQt5.QtWidgets import QApplication, QWidget, QVBoxLayout, QLineEdit, QPushButton, QTextEdit
import socket

class SearchGUI(QWidget):
    def __init__(self):
        super().__init__()
        self.initUI()

    def initUI(self):
        self.setWindowTitle('Search Engine')
        self.setGeometry(100, 100, 400, 300)

        layout = QVBoxLayout()

        self.queryInput = QLineEdit(self)
        self.queryInput.setPlaceholderText('Enter search query')
        layout.addWidget(self.queryInput)

        self.searchButton = QPushButton('Search', self)
        self.searchButton.clicked.connect(self.performSearch)
        layout.addWidget(self.searchButton)

        self.resultsOutput = QTextEdit(self)
        self.resultsOutput.setReadOnly(True)
        layout.addWidget(self.resultsOutput)

        self.setLayout(layout)

    def performSearch(self):
        query = self.queryInput.text()
        results = self.sendQueryToBackend(query)
        self.resultsOutput.setText(results)

    def sendQueryToBackend(self, query):
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.connect(('localhost', 12345))
            s.sendall(query.encode())
            data = s.recv(1024)
        return data.decode()

if __name__ == '__main__':
    app = QApplication(sys.argv)
    gui = SearchGUI()
    gui.show()
    sys.exit(app.exec_())