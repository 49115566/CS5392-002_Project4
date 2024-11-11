# Design Documentation for Project 4: Search Engine

## Table of Contents
- [Introduction](#introduction)
- [System Overview](#system-overview)
- [Components](#components)
    - [WordMap](#wordmap)
    - [SearchEngine](#searchengine)
    - [Main Program](#main-program)
    - [Socket-Based Server](#socket-based-server)
    - [Graphical User Interface (GUI)](#graphical-user-interface-gui)
- [Data Flow](#data-flow)
- [Error Handling](#error-handling)
- [Future Enhancements](#future-enhancements)

## Introduction
This document provides an overview of the design and architecture of the search engine implemented in Project 4. The search engine can process and search through documents using either a command-line interface or a graphical user interface (GUI).

## System Overview
The search engine consists of several components that work together to provide search functionality. The main components are:
- `WordMap`: Manages the mapping of words to file paths.
- `SearchEngine`: Provides the core search functionality.
- `Main Program`: Command-line interface for the search engine.
- `Socket-Based Server`: Allows the search engine to be accessed over a network.
- `Graphical User Interface (GUI)`: Provides a user-friendly interface for searching.

## Components

### WordMap
The `WordMap` class is responsible for managing the mapping of words to file paths. It uses several unordered maps to store associations between words, organizations, names, and file paths.

#### Key Methods
- `associateOrg`: Associates an organization with a file path.
- `associateName`: Associates a name with a file path.
- `associateWord`: Associates a word with a file path.
- `disassociate`: Removes an association between a word and a file path.
- `getFilesByOrg`: Retrieves files associated with an organization.
- `getFilesByName`: Retrieves files associated with a name.
- `getOtherFilesByWord`: Retrieves files not associated with a specific word.
- `getFilesByWord`: Retrieves files associated with a word.
- `save`: Saves the mappings to files.
- `load`: Loads the mappings from files.

### SearchEngine
The `SearchEngine` class provides the core search functionality. It uses the `WordMap` class to manage word-to-file associations and perform searches.

#### Key Methods
- `search`: Searches for files containing the specified search terms.
- `buildFromScratch`: Processes data from a folder and builds the word mappings.
- `parse`: Parses search terms into individual words.
- `getRelevantData`: Extracts relevant data from JSON files.

### Main Program
The main program (`main.cpp`) provides a command-line interface for the search engine. It allows users to enter search queries and view the results in the terminal.

### Socket-Based Server
The socket-based server (`socketSearch.cpp`) allows the search engine to be accessed over a network. It listens for incoming connections, processes search queries, and sends back the results.

### Graphical User Interface (GUI)
The GUI (`searchGUI.py`) provides a user-friendly interface for the search engine. It allows users to enter search queries, view results, and navigate through the results using a graphical interface.

## Data Flow
1. **Initialization**: The `SearchEngine` is initialized with the folder path and file paths for saving/loading mappings.
2. **Loading Data**: The `WordMap` attempts to load existing mappings from files. If loading fails, it builds the mappings from scratch by processing the documents in the specified folder.
3. **Search Query**: The user enters a search query through the command-line interface or GUI.
4. **Processing Query**: The `SearchEngine` parses the search terms and retrieves the relevant files using the `WordMap`.
5. **Displaying Results**: The search results are displayed in the terminal or GUI.

## Error Handling
- **File I/O Errors**: Errors related to file operations (e.g., opening, reading, writing) are handled by checking the success of these operations and printing error messages if they fail.
- **Network Errors**: Errors related to socket operations (e.g., binding, listening, accepting connections) are handled by checking the success of these operations and printing error messages if they fail.
- **JSON Parsing Errors**: Errors related to parsing JSON files are handled by checking the success of the parsing operation and printing error messages if they fail.

## Future Enhancements
- **Improved Search Algorithms**: Implement more advanced search algorithms to improve the accuracy and relevance of search results.
- **User Authentication**: Add user authentication to the socket-based server to restrict access to authorized users.
- **Enhanced GUI**: Improve the GUI with additional features such as filtering, sorting, and advanced search options.
