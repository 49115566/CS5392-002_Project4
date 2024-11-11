import sys  # Importing the sys module
from PyQt5.QtWidgets import QApplication, QWidget, QVBoxLayout, QLineEdit, QPushButton, QTextEdit, QListView, QMessageBox, QHBoxLayout, QLabel  # Importing necessary PyQt5 widgets
from PyQt5.QtCore import QThread, pyqtSignal, QAbstractListModel, Qt  # Importing necessary PyQt5 core components
import socket  # Importing the socket module
import os  # Importing the os module
import json  # Importing the json module

class SearchThread(QThread):  # Defining the SearchThread class inheriting from QThread
    results_ready = pyqtSignal(str)  # Defining a custom signal

    def __init__(self, query):  # Initializing the SearchThread class
        super().__init__()  # Calling the superclass constructor
        self.query = query  # Storing the query

    def run(self):  # Defining the run method
        results = self.sendQueryToBackend(self.query)  # Sending the query to the backend and getting results
        self.results_ready.emit(results)  # Emitting the results_ready signal with the results

    def sendQueryToBackend(self, query):  # Defining the sendQueryToBackend method
        BUFFER_SIZE = 8192  # Increased buffer size  # Setting the buffer size
        data = b""  # Initializing an empty byte string
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:  # Creating a socket
            s.connect(('localhost', 12345))  # Connecting to the backend server
            s.sendall(query.encode())  # Sending the query
            while True:  # Receiving data in a loop
                part = s.recv(BUFFER_SIZE)  # Receiving a part of the data
                data += part  # Adding the part to the data
                if len(part) < BUFFER_SIZE:  # Breaking the loop if the part is smaller than the buffer size
                    break
        return data.decode()  # Decoding and returning the data

class ResultsModel(QAbstractListModel):  # Defining the ResultsModel class inheriting from QAbstractListModel
    def __init__(self, results=None, parent=None):  # Initializing the ResultsModel class
        super().__init__(parent)  # Calling the superclass constructor
        self.results = results or []  # Storing the results

    def rowCount(self, parent=None):  # Defining the rowCount method
        return len(self.results)  # Returning the number of results

    def data(self, index, role):  # Defining the data method
        if role == Qt.DisplayRole:  # Checking if the role is DisplayRole
            return self.results[index.row()]  # Returning the result at the given index

class SearchGUI(QWidget):  # Defining the SearchGUI class inheriting from QWidget
    def __init__(self):  # Initializing the SearchGUI class
        super().__init__()  # Calling the superclass constructor
        self.results = []  # Initializing an empty list for results
        self.current_page = 0  # Initializing the current page to 0
        self.results_per_page = 100  # Adjust this number as needed  # Setting the number of results per page
        self.initUI()  # Initializing the UI

    def initUI(self):  # Defining the initUI method
        self.setWindowTitle('Search Engine')  # Setting the window title
        self.setGeometry(100, 100, 600, 400)  # Setting the window geometry

        layout = QVBoxLayout()  # Creating a vertical box layout

        query_label = QLabel('Search Query:', self)  # Creating a label for the search query
        layout.addWidget(query_label)  # Adding the label to the layout

        self.queryInput = QLineEdit(self)  # Creating a line edit for the search query input
        self.queryInput.setPlaceholderText('Enter search query')  # Setting the placeholder text
        self.queryInput.setToolTip('Type your search query here')  # Setting the tooltip
        layout.addWidget(self.queryInput)  # Adding the line edit to the layout

        self.searchButton = QPushButton('Search', self)  # Creating a search button
        self.searchButton.setToolTip('Click to perform search')  # Setting the tooltip
        self.searchButton.clicked.connect(self.performSearch)  # Connecting the button click to the performSearch method
        layout.addWidget(self.searchButton)  # Adding the button to the layout

        results_label = QLabel('Search Results:', self)  # Creating a label for the search results
        layout.addWidget(results_label)  # Adding the label to the layout

        self.resultsList = QListView(self)  # Creating a list view for the search results
        self.resultsList.setToolTip('Double-click to view file content')  # Setting the tooltip
        self.resultsList.setModel(ResultsModel(self.results))  # Setting the model for the list view
        self.resultsList.doubleClicked.connect(self.viewFile)  # Connecting the double-click signal to the viewFile method
        layout.addWidget(self.resultsList)  # Adding the list view to the layout

        file_content_label = QLabel('File Content:', self)  # Creating a label for the file content
        layout.addWidget(file_content_label)  # Adding the label to the layout

        self.fileContent = QTextEdit(self)  # Creating a text edit for the file content
        self.fileContent.setReadOnly(True)  # Setting the text edit to read-only
        layout.addWidget(self.fileContent)  # Adding the text edit to the layout

        nav_layout = QHBoxLayout()  # Creating a horizontal box layout for navigation
        self.prevButton = QPushButton('Previous', self)  # Creating a previous button
        self.prevButton.setToolTip('Go to previous page of results')  # Setting the tooltip
        self.prevButton.clicked.connect(self.prevPage)  # Connecting the button click to the prevPage method
        nav_layout.addWidget(self.prevButton)  # Adding the button to the navigation layout

        self.nextButton = QPushButton('Next', self)  # Creating a next button
        self.nextButton.setToolTip('Go to next page of results')  # Setting the tooltip
        self.nextButton.clicked.connect(self.nextPage)  # Connecting the button click to the nextPage method
        nav_layout.addWidget(self.nextButton)  # Adding the button to the navigation layout

        self.pageInfoLabel = QLabel(self)  # Creating a label for the page information
        nav_layout.addWidget(self.pageInfoLabel)  # Adding the label to the navigation layout

        layout.addLayout(nav_layout)  # Adding the navigation layout to the main layout

        self.setLayout(layout)  # Setting the main layout for the widget

    def performSearch(self):  # Defining the performSearch method
        query = self.queryInput.text()  # Getting the query from the input
        if query:  # Checking if the query is not empty
            self.searchThread = SearchThread(query)  # Creating a SearchThread with the query
            self.searchThread.results_ready.connect(self.handleResults)  # Connecting the results_ready signal to the handleResults method
            self.searchThread.start()  # Starting the search thread
        else:  # If the query is empty
            QMessageBox.warning(self, 'Input Error', 'Please enter a search query.')  # Showing a warning message

    def handleResults(self, results):  # Defining the handleResults method
        self.results = results.split('\n')  # Splitting the results by newline
        self.current_page = 0  # Resetting the current page to 0
        self.updateResultsList()  # Updating the results list

    def updateResultsList(self):  # Defining the updateResultsList method
        start_index = self.current_page * self.results_per_page  # Calculating the start index
        end_index = start_index + self.results_per_page  # Calculating the end index
        self.resultsList.model().results = self.results[start_index:end_index]  # Updating the results in the model
        self.resultsList.model().layoutChanged.emit()  # Emitting the layoutChanged signal
        self.prevButton.setEnabled(self.current_page > 0)  # Enabling/disabling the previous button
        self.nextButton.setEnabled(end_index < len(self.results))  # Enabling/disabling the next button
        total_pages = (len(self.results) + self.results_per_page - 1) // self.results_per_page  # Calculating the total number of pages
        self.pageInfoLabel.setText(f"Page {self.current_page + 1} of {total_pages}; {self.results_per_page} results per page")  # Updating the page information label

    def prevPage(self):  # Defining the prevPage method
        if self.current_page > 0:  # Checking if the current page is greater than 0
            self.current_page -= 1  # Decrementing the current page
            self.updateResultsList()  # Updating the results list

    def nextPage(self):  # Defining the nextPage method
        if (self.current_page + 1) * self.results_per_page < len(self.results):  # Checking if there are more results
            self.current_page += 1  # Incrementing the current page
            self.updateResultsList()  # Updating the results list

    def viewFile(self, index):  # Defining the viewFile method
        file_path = self.resultsList.model().data(index, Qt.DisplayRole)  # Getting the file path from the model
        if os.path.exists(file_path):  # Checking if the file exists
            with open(file_path, 'r') as file:  # Opening the file
                content = file.read()  # Reading the file content
                try:  # Trying to parse the content as JSON
                    json_content = json.loads(content)  # Parsing the content as JSON
                    formatted_content = self.json_to_html(json_content)  # Formatting the JSON content as HTML
                except json.JSONDecodeError:  # If the content is not valid JSON
                    formatted_content = content  # Using the raw content
                self.fileContent.setHtml(formatted_content)  # Setting the formatted content in the text edit
        else:  # If the file does not exist
            QMessageBox.warning(self, 'File Error', f'File not found: {file_path}')  # Showing a warning message

    def json_to_html(self, json_content):  # Defining the json_to_html method
        html_content = "<html><body>"  # Initializing the HTML content
        html_content += f"<h1>{json_content.get('title', 'No Title')}</h1>"  # Adding the title
        html_content += f"<p><strong>Author:</strong> {json_content.get('author', 'Unknown')}</p>"  # Adding the author
        html_content += f"<p><strong>Published:</strong> {json_content['thread'].get('published', 'Unknown')}</p>"  # Adding the published date
        html_content += f"<p><strong>URL:</strong> <a href='{json_content.get('url', '#')}'>{json_content.get('url', 'No URL')}</a></p>"  # Adding the URL
        html_content += f"<p><strong>Text:</strong> {json_content.get('text', 'No Text')}</p>"  # Adding the text
        html_content += "<h2>Entities</h2>"  # Adding the entities header
        html_content += "<ul>"  # Starting the entities list
        for person in json_content['entities'].get('persons', []):  # Iterating over the persons
            html_content += f"<li>{person.get('name', 'Unknown')}</li>"  # Adding each person to the list
        html_content += "</ul>"  # Ending the entities list
        html_content += "<h2>Locations</h2>"  # Adding the locations header
        html_content += "<ul>"  # Starting the locations list
        for location in json_content['entities'].get('locations', []):  # Iterating over the locations
            html_content += f"<li>{location.get('name', 'Unknown')}</li>"  # Adding each location to the list
        html_content += "</ul>"  # Ending the locations list
        html_content += "<h2>Organizations</h2>"  # Adding the organizations header
        html_content += "<ul>"  # Starting the organizations list
        for organization in json_content['entities'].get('organizations', []):  # Iterating over the organizations
            html_content += f"<li>{organization.get('name', 'Unknown')}</li>"  # Adding each organization to the list
        html_content += "</ul>"  # Ending the organizations list
        html_content += "</body></html>"  # Ending the HTML content
        return html_content  # Returning the HTML content

if __name__ == '__main__':  # Checking if the script is run directly
    app = QApplication(sys.argv)  # Creating a QApplication
    gui = SearchGUI()  # Creating an instance of SearchGUI
    gui.show()  # Showing the GUI
    sys.exit(app.exec_())  # Running the application event loop