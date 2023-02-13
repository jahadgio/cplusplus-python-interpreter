# cplusplus-python-interpreter
Python interpreter constructed in C++. Capable of reading, displaying, and running simple .py files, as well as executing certain commands in the command line.

## Commands:
- **quit or quit()**
  - Exits the interpreter interface
- **help or help(<command>)**
  - When no parameter is given, then “help” or “help()” will enter the help utility. When in the help utility, the prompt will be “help>” instead of “>>>”.  Following are the commands that can be entered at the “help>” prompt:
- **read(<filename>.py)**
  - <filename>.py parameter has to be provided, which is a PySub program file. It ends in the “py” extension because it should a valid Python program, since PySub is a subset of the Python language) and should be able to be run by the Python interpreter
  - Clears any lines that are stored in the program data structure, and then reads program lines from the file given as a parameter (<filename.py>) and stores the lines into the program data structure
- **show or show()**
  - Shows the lines of the program that are stored in program data structure. Include line number when displaying each line of code in the program. 
  - Can also include argument "tokens" (show(tokens)) to display how tokens are categorized after the read command has been entered. 
  - Can also include argument "variables" (show(variables)) to display the variables stored in the symbol table.
- **clear or clear()**
  - Clears out or deletes any lines that are stored in the program data structure
