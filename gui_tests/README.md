## Readme

GUITest is a test application which we show different use cases of PythonQt:
1) Show the usage of contexts 
2) Add objects to contexts
3) Show the usage of cpp class wrappers
4) Show the usage of cpp class decorators

### Contexts
The test application has two contexts available.
- Context `Ctx GUI`: This context provides an object `gui` which can control the graphical user interface,
in particular change the color of the gui by using the buttons 'red, blue and green'.
- Context `Ctx GUI + meta`: This context also has access to the `gui` object.
Additionally, it provides access to the `meta object`. The `meta object` can reset the gui color (`reset_color`), 
can clear the output (`clear_output`) and can create a `FileHandler` object with `create_file()`, which represents a file access.

### `gui object`
- `gui.red()`: Hits the red button of the gui
- `gui.blue()`: Hits the blue button of the gui
- `gui.green()`: Hits the green button of the gui
 
### `meta object`
- `meta.clear_output()`: Clears the output
- `meta.reset_color()`: Resets the gui color
- `meta.create_filehandle(filename)`: Creates a FileWrapper of passed filename

### `FileHandle class`
- `var_name.create_file()`: Creates the file in the file system. The content of the file is the current date.
- `var_name.delete_file()`: Deletes the file
- `var_name.file_exists()`: Returns true if the file exists, false otherwise
