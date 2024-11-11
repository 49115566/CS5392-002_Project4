# Project 4: Search Engine

This project implements a search engine that can process and search through documents. The search engine can be used in two different ways: through a command-line interface or a graphical user interface (GUI). 

## Table of Contents
- [Project Structure](#project-structure)
- [Dependencies](#dependencies)
- [Building the Project](#building-the-project)
- [Running the Project](#running-the-project)
    - [Command-Line Interface](#command-line-interface)
    - [Socket-Based Interface with GUI](#socket-based-interface-with-gui)
- [Usage](#usage)
    - [Command-Line Interface](#command-line-interface-usage)
    - [Socket-Based Interface with GUI](#socket-based-interface-with-gui-usage)

## Project Structure

```
.
├── main.cpp
├── socketSearch.cpp
├── searchGUI.py
├── searchEngine.h
├── searchEngine.cpp
├── wordmap.h
├── wordmap.cpp
├── rapidjson/
├── docs/
├── fnsavefile.csv
├── osavefile.csv
├── nsavefile.csv
├── wsavefile.csv
└── README.md
```

## Dependencies

- C++17
- RapidJSON
- PyQt5 (for the GUI)
- POSIX compliant system (for socket programming)

## Building the Project

To build the project, you need to compile the C++ files. You can use `g++`:

```sh
g++ -std=c++17 -o main main.cpp searchEngine.cpp wordmap.cpp -lstdc++fs
g++ -std=c++17 -o socketSearch socketSearch.cpp searchEngine.cpp wordmap.cpp -lstdc++fs
```

## Running the Project

### Command-Line Interface

To run the command-line interface version, execute the `main` binary:

```sh
./main
```

### Socket-Based Interface with GUI

To run the socket-based interface, first start the socket server:

```sh
./socketSearch
```

Then, in another terminal, run the GUI:

```sh
python3 searchGUI.py
```

## Usage

### Command-Line Interface Usage

1. Run the `main` binary.
2. Enter your search query when prompted.
3. View the search results in the terminal.
4. Choose whether to perform another search.

### Socket-Based Interface with GUI Usage

1. Start the socket server by running `./socketSearch`.
2. Run the GUI by executing `python3 searchGUI.py`.
3. Enter your search query in the GUI.
4. View the search results in the GUI.
5. Navigate through the results using the "Previous" and "Next" buttons.
6. Double-click on a result to view the file content.
