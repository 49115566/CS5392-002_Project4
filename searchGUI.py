import sys
from PyQt5.QtWidgets import QApplication, QWidget, QVBoxLayout, QLineEdit, QPushButton, QTextEdit, QListWidget, QMessageBox
import socket
import os
import json

class SearchGUI(QWidget):
    def __init__(self):
        super().__init__()
        self.initUI()

    def initUI(self):
        self.setWindowTitle('Search Engine')
        self.setGeometry(100, 100, 600, 400)

        layout = QVBoxLayout()

        self.queryInput = QLineEdit(self)
        self.queryInput.setPlaceholderText('Enter search query')
        layout.addWidget(self.queryInput)

        self.searchButton = QPushButton('Search', self)
        self.searchButton.clicked.connect(self.performSearch)
        layout.addWidget(self.searchButton)

        self.resultsList = QListWidget(self)
        self.resultsList.itemDoubleClicked.connect(self.viewFile)
        layout.addWidget(self.resultsList)

        self.fileContent = QTextEdit(self)
        self.fileContent.setReadOnly(True)
        layout.addWidget(self.fileContent)

        self.setLayout(layout)

    def performSearch(self):
        query = self.queryInput.text()
        if query:
            results = self.sendQueryToBackend(query)
            self.resultsList.clear()
            for result in results.split('\n'):
                if result.strip():
                    self.resultsList.addItem(result)
        else:
            QMessageBox.warning(self, 'Input Error', 'Please enter a search query.')

    def sendQueryToBackend(self, query):
        BUFFER_SIZE = 4096
        data = b""
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.connect(('localhost', 12345))
            s.sendall(query.encode())
            while True:
                part = s.recv(BUFFER_SIZE)
                data += part
                if len(part) < BUFFER_SIZE:
                    break
        return data.decode()

    def viewFile(self, item):
        file_path = item.text()
        if os.path.exists(file_path):
            with open(file_path, 'r') as file:
                content = file.read()
                try:
                    json_content = json.loads(content)
                    formatted_content = self.json_to_html(json_content)
                except json.JSONDecodeError:
                    formatted_content = content
                self.fileContent.setHtml(formatted_content)
        else:
            QMessageBox.warning(self, 'File Error', f'File not found: {file_path}')

    def json_to_html(self, json_content):
        html_content = "<html><body>"
        html_content += f"<h1>{json_content.get('title', 'No Title')}</h1>"
        html_content += f"<p><strong>Author:</strong> {json_content.get('author', 'Unknown')}</p>"
        html_content += f"<p><strong>Published:</strong> {json_content['thread'].get('published', 'Unknown')}</p>"
        html_content += f"<p><strong>URL:</strong> <a href='{json_content.get('url', '#')}'>{json_content.get('url', 'No URL')}</a></p>"
        html_content += f"<p><strong>Text:</strong> {json_content.get('text', 'No Text')}</p>"
        html_content += "<h2>Entities</h2>"
        html_content += "<ul>"
        for person in json_content['entities'].get('persons', []):
            html_content += f"<li>{person.get('name', 'Unknown')}</li>"
        html_content += "</ul>"
        html_content += "<h2>Locations</h2>"
        html_content += "<ul>"
        for location in json_content['entities'].get('locations', []):
            html_content += f"<li>{location.get('name', 'Unknown')}</li>"
        html_content += "</ul>"
        html_content += "<h2>Organizations</h2>"
        html_content += "<ul>"
        for organization in json_content['entities'].get('organizations', []):
            html_content += f"<li>{organization.get('name', 'Unknown')}</li>"
        html_content += "</ul>"
        html_content += "</body></html>"
        return html_content

if __name__ == '__main__':
    app = QApplication(sys.argv)
    gui = SearchGUI()
    gui.show()
    sys.exit(app.exec_())