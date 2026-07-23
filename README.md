# Simple Unix-like Shell in C

## A Mini Shell project written in C that replicates basic Unix shell behavior

This Mini Shell project copies and executes basic commands along some built-in commands I made.
This project is my introduction to C programming and system-level programming, which could help me understand the basics of it.

## External commands

The shell can execute any system-installed program through PATH resolution, such as:
* ls
* cat
* grep
* mkdir
* rm
* touch
* cp
* mv
* ps
* pwd
* chmod
* chown
* find
* which

## The built-in commands I added:
These commands are implemented internally in the shell:
* cd
* echo
* env
* history
* exit

They replicate simplified behavior of their Unix equivalents

## Parser Features
- [x] Environment variable expansion (`$VAR`)
- [x] Single quotes (`'...'`)
- [x] Double quotes (`"..."`)
- [ ] Escape character (`\`)
- [ ] Pipes (`|`)
- [x] Input redirection (`<`)
- [x] Output redirection (`>`)
- [ ] Append redirection (`>>`)
- [ ] Here-document (`<<`)


## How to install this project
1. Clone this project
2. Build the project:

- If you use a Makefile:

      make

- If you don’t have a Makefile yet:

      gcc -Wall -Wextra -Werror *.c -o minishell

3. Run the project:

       ./minishell

## Use it as you wish
Since it's a learning project, you can use it however you want.

## A bug appeared?
As I'm a beginner in C, there could possibly be a lot of bugs, so please share with me if you've found one!

## Known issues

## Currently working on
Enhancing parser so it can handle different cases like $, <>, | etc.
