# tntouch - Template Name Touch

Tntouch is a simple command line tool for creating files using predefined names (templates).

Tntouch is an Open Source project covered fully by MIT License. This project was 
originally conceived as a learning project.

## Installation

### Prerequisites

Ensure that the following tools are installed on your system before proceeding: 

- GCC: The GNU Compiler Collection for building the project.
- GNU Make: A build automation tool for running the provided Makefile.
- Autotools: Required to generate the build system files. This includes:
    - autoconf: Generates the configure script from configure.ac.
    - automake: Generates Makefile.in from Makefile.am.

You can check if these are installed using the following commands:

```
gcc --version
make --version
```

If they are not installed, you can install them using your system's package manager:

Debian/Ubuntu:
```
sudo apt update
sudo apt install gcc make autoconf automake
```

Fedora:
```
sudo dnf install gcc make autoconf automake
```

Arch Linux:
```
sudo pacman -S gcc make autoconf automake
```

macOS (via Homebrew):
```
brew install gcc make autoconf automake
```

### Installation Steps

Clone the repository:
```
git clone https://github.com/pavroto/tntouch.git
cd tntouch
```

Prepare the Build System with Autotools
Run the following command to initialize the build system:
```
autoreconf --install
```
This will:

- Generate the configure script.
- Create other required files such as Makefile.in and config.h.in.
- Ensure all necessary files are present for the build process.

Configure:
```
./configure
```

Build the project:
```
make
```

Install the binary:
```
sudo make install
```

Verify the installation:
```
tntouch --version
```

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


## Why?

Well... first, it's an assignment for my university course.

Second, I am the one who likes to create files with names like 
`2024-11-28-SystemProgramming-Networking.txt` for notes, so later it is easier 
for me to find it with `find` tool or `telescope.nvim` in my huge university materials archive. 
But writing dates over and over again is just exhausting, so I decided to both pass the 
assignment and create a tool that would solve my own problem. And technically, that is why the tool 
is designed to work the way it works.

Also, `tntouch` doesn't touch ¯\\\_(ツ)\_/¯

I mean, original `touch` from GNU Coreutils was designed to "update the access and modification 
times of each FILE to the current time", but I did not want my tool to do that, so I just made 
it to return an error if the file with the same name already exists.
