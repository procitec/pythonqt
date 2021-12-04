## Readme

GUITest is a test application which demonstrates different use cases of PythonQt:
1) Show the usage of contexts 
2) Add objects to contexts
3) Show the usage of cpp class wrappers
4) Show the usage of cpp class decorators

### Contexts
The test application has two contexts available.
- Context `Ctx GUI`: This context provides an object `gui` which can control the graphical user interface,
in particular change the color of the gui by using the buttons 'red, blue and green'.
- Context `Ctx GUI + meta`: This context does also have access to the `gui` object as `Ctx GUI`.
Additionally, it provides access to the `meta object`. The `meta object` can reset the gui color (`reset_color`), 
can clear the output (`clear_output`) and can create a `FileHandler` object with `create_file()`, which represents a file access.

### `gui object`
- `gui.red()`: Hits the red button of the gui
- `gui.blue()`: Hits the blue button of the gui
- `gui.green()`: Hits the green button of the gui
- `gui.create_file(filename)`: Creates a file with given filename. Used for unittests
 
### `meta object`
- `meta.clear_output()`: Clears the output
- `meta.reset_color()`: Resets the gui color
- `meta.create_filehandle(filename)`: Creates a FileHandle instance of passed filename

### `FileHandle` class
This class represents a file handle. With this, `PythonQtCppWrapperFactory` and insertion of user types are tested. 
- `var_name.create_file()`: Creates the file in the file system. The content of the file is the current date.
- `var_name.delete_file()`: Deletes the file
- `var_name.file_exists()`: Returns true if the file exists, false otherwise
- 
### `FileDec`: An inserted Decorator class
Instanciate a new type FileDec directly (not by a WrapperFactory).
Therefore the type must be imported:
```
from PythonQt.example import FileDec
fd = FileDec("test.txt")
print(fd.exists())
```

- `exists()`: The FileDec type only has an exists method. This returns True/False, depending on file existence

# Autotest usage with `test.py`
Copy the `test.py` file to your binary directory and call `GuiTest` with the filename as an argument.
```
$ ./GuiTest test.py
Auto test enabled by passing a file test.py
```
You will see a message that the content of the file is automatically passed to the interpreter.
The output of the unittest is shown in the output window of the gui.

Pay attention: Because the unittests makes intensive use of contexts, added objects, classes and class decorators
only the first test method can be run without the gui. 
All other tests depend on objects and contexts injected in the specific python environment and cannot be executed
without the gui.
