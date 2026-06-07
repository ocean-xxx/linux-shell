# MySH

A Unix-like shell implemented in C.

This project is built to explore operating system concepts, process management,
command execution, pipes, redirection, environment variables, and custom command-line utilities.

## Features

- Built-in commands
  - cd
  - exit
- Command history
- Environment variable support
- Input redirection (`<`)
- Output redirection (`>`)
- Pipes (`|`)
- Background execution (`&`)

The shell supports several self-developed command-line tools:

| Command | Description |
|----------|-------------|
| mycat | Display file contents |
| mygrep | Search for patterns in files |
| mywc | Count lines, words, and characters |
| myls | List directory contents |
| myfind | Search files recursively |
| mystat | Display file information |
| myjobs | Display background jobs |

Current status:

- mycat     Completed
- mygrep    In Progress
- mywc      In Progress
- myls      In Progress
- myfind    In Progress
- mystat    In Progress
- myjobs    In Progress

---

## Environment Variables

The shell uses the PATH environment variable to locate executable files.

Example:

PATH=/home/bo-yang/bin:/usr/local/bin