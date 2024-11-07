## Project Plan

### 1. Create a Map with Parsed Terms
- Parse the terms from the input files.
- Use the parsed terms as keys in a map.
- Store a list of file paths (as strings) as the values associated with each key.

### 2. Persist the Map
- Implement a method to save the map to a file or database.
- Ensure the map can be loaded from the saved state, avoiding the need to recreate it every time the program runs.

### 3. Develop a Parser
- Create a parser that reads the input files.
- Extract and return important information from the files.

### 4. Build a User Interface
- Design a user interface that presents options to the user.
- Display some text from the files to provide context.
- Allow the user to make selections based on the presented options.