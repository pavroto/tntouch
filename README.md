# tntouch - Template Name Touch

Tntouch is a simple command line tool for creating files using predefined names (templates).

Tntouch is an Open Source project covered fully by MIT License. This project was 
originally conceived as a learning project.


## Usage


### 1. Set a default template
To set a default template, use the `-d` flag:
```bash
tntouch -d ~/Template/dated
```
This stores the path to ~/Template/dated as the default template in 
$XDG_CONFIG_HOME/tntouch/default or ~/.config/tntouch/default (depending 
on your system's configuration).


### 2. Create a File Using the Default Template
After setting the default template, simply run:
```bash
tntouch
```
This creates a file in the current working directory with a name generated from the default template. 
For example, if the template is:
```
$( date +"%Y-%m-%d" )-$( whoami )-\i(Foo).txt
```
The resulting file might be:
```
2024-11-28-user-Foo.txt
```


### 3. Specify a Value for `\i`
To replace the `\i` placeholder in the template with a custom value, provide it as an argument:
```bash
tntouch Bar
```
This would create a file named:
```
2024-11-28-user-Bar.txt
```


### 4. Use a Non-Default Template
To use a template other than the default, specify it with the `-t` flag:
```
tntouch -t /path/to/template
```
This uses the specified template to create a file.



## Template Syntax
Templates are plain text files containing the desired filename format. They support the following syntax:

1. Shell Commands:
    - Enclose shell commands in $(...).
    - The output of the command will be included in the filename.
    - Example:
    ```
    $( date +"%Y-%m-%d" )
    ```
    - Result: `2024-11-28`

2. Placeholder (`\i`):
    - `\i(DefaultValue)` will be replaced with a provided value or the default if no value is specified.
    - Example:
    ```
    \i(Foo)
    ```
    - With no input: `Foo`
    - With input `Bar`: `Bar`

**_NOTE:_** To use '(' and ')' characters inside DefaultValue, please use '\\(' and '\\)' respectively.

3. Literal text:
    - All other text is used as-is in the resulting filename.

