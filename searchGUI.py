import sys
from PyQt5.QtWidgets import QApplication, QWidget, QVBoxLayout, QLineEdit, QPushButton, QTextEdit, QListView, QMessageBox, QHBoxLayout, QLabel
from PyQt5.QtCore import QThread, pyqtSignal, QAbstractListModel, Qt
import socket
import os
import json

class SearchThread(QThread):
    results_ready = pyqtSignal(str)

    def __init__(self, query):
        super().__init__()
        self.query = query

    def run(self):
        results = self.sendQueryToBackend(self.query)
        self.results_ready.emit(results)

    def sendQueryToBackend(self, query):
        BUFFER_SIZE = 8192  # Increased buffer size
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

class ResultsModel(QAbstractListModel):
    def __init__(self, results=None, parent=None):
        super().__init__(parent)
        self.results = results or []

    def rowCount(self, parent=None):
        return len(self.results)

    def data(self, index, role):
        if role == Qt.DisplayRole:
            return self.results[index.row()]

class SearchGUI(QWidget):
    def __init__(self):
        super().__init__()
        self.results = []
        self.current_page = 0
        self.results_per_page = 100  # Adjust this number as needed
        self.initUI()

    def initUI(self):
        self.setWindowTitle('Search Engine')
        self.setGeometry(100, 100, 600, 400)

        layout = QVBoxLayout()

        query_label = QLabel('Search Query:', self)
        layout.addWidget(query_label)

        self.queryInput = QLineEdit(self)
        self.queryInput.setPlaceholderText('Enter search query')
        self.queryInput.setToolTip('Type your search query here')
        layout.addWidget(self.queryInput)

        self.searchButton = QPushButton('Search', self)
        self.searchButton.setToolTip('Click to perform search')
        self.searchButton.clicked.connect(self.performSearch)
        layout.addWidget(self.searchButton)

        results_label = QLabel('Search Results:', self)
        layout.addWidget(results_label)

        self.resultsList = QListView(self)
        self.resultsList.setToolTip('Double-click to view file content')
        self.resultsList.setModel(ResultsModel(self.results))
        self.resultsList.doubleClicked.connect(self.viewFile)
        layout.addWidget(self.resultsList)

        file_content_label = QLabel('File Content:', self)
        layout.addWidget(file_content_label)

        self.fileContent = QTextEdit(self)
        self.fileContent.setReadOnly(True)
        layout.addWidget(self.fileContent)

        nav_layout = QHBoxLayout()
        self.prevButton = QPushButton('Previous', self)
        self.prevButton.setToolTip('Go to previous page of results')
        self.prevButton.clicked.connect(self.prevPage)
        nav_layout.addWidget(self.prevButton)

        self.nextButton = QPushButton('Next', self)
        self.nextButton.setToolTip('Go to next page of results')
        self.nextButton.clicked.connect(self.nextPage)
        nav_layout.addWidget(self.nextButton)

        self.pageInfoLabel = QLabel(self)
        nav_layout.addWidget(self.pageInfoLabel)

        layout.addLayout(nav_layout)

        self.setLayout(layout)

    def performSearch(self):
        query = self.queryInput.text()
        if query:
            self.searchThread = SearchThread(query)
            self.searchThread.results_ready.connect(self.handleResults)
            self.searchThread.start()
        else:
            QMessageBox.warning(self, 'Input Error', 'Please enter a search query.')

    def handleResults(self, results):
        self.results = results.split('\n')
        self.current_page = 0
        self.updateResultsList()

    def updateResultsList(self):
        start_index = self.current_page * self.results_per_page
        end_index = start_index + self.results_per_page
        self.resultsList.model().results = self.results[start_index:end_index]
        self.resultsList.model().layoutChanged.emit()
        self.prevButton.setEnabled(self.current_page > 0)
        self.nextButton.setEnabled(end_index < len(self.results))
        total_pages = (len(self.results) + self.results_per_page - 1) // self.results_per_page
        self.pageInfoLabel.setText(f"Page {self.current_page + 1} of {total_pages}; {self.results_per_page} results per page")

    def prevPage(self):
        if self.current_page > 0:
            self.current_page -= 1
            self.updateResultsList()

    def nextPage(self):
        if (self.current_page + 1) * self.results_per_page < len(self.results):
            self.current_page += 1
            self.updateResultsList()

    def viewFile(self, index):
        file_path = self.resultsList.model().data(index, Qt.DisplayRole)
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