# OS Lab 4 — Processes & Threads

This repository contains solutions for the three exercises from Lab 4.

## Repository structure

- `ex1.c` — two child processes created by the main process; PID, PPID, and execution time.
- `ex2.c` — repeated `fork()` experiment with `n` from the command line.
- `ex3.c` — a simplistic shell using `fork()` and `execve()`.
- `screenshots/` — put screenshots of terminal output here.

> Note: the slides contain inconsistent example/file numbering.  
> This repository uses `ex1.c`, `ex2.c`, and `ex3.c` to match Exercise 1, 2, and 3.

---

## Environment

The programs use POSIX/Linux APIs such as `fork()`, `waitpid()`, `unistd.h`, and `execve()`.

On Windows, compile and run them inside WSL.

Example:

```bash
cd /mnt/c/Users/<YOUR_WINDOWS_USER>/path/to/OS-Lab4
```

---

# Exercise 1

## Goal

Create two new processes. Neither child should be the child of the other: both must have the main process as their parent.

Print for all three processes:

- PID
- PPID
- execution time in milliseconds

## Compile

```bash
gcc -Wall -Wextra -std=c11 ex1.c -o ex1
```

## Run

```bash
./ex1
```

Example output:

```text
Child 1    | PID = 501    | PPID = 500    | Time = 0.004 ms
Child 2    | PID = 502    | PPID = 500    | Time = 0.003 ms
Main       | PID = 500    | PPID = 300    | Time = 0.120 ms
```

## Observations

Both child processes have the PID of the main process as their PPID. This confirms that both children were created directly by the main process.

The exact PID values change between runs because the operating system assigns process IDs dynamically.

The execution times are very small and may differ between runs.

## Problems / issues

The code cannot be compiled with the normal MinGW toolchain on Windows because `fork()`, `waitpid()`, and `sys/wait.h` are POSIX/Linux features. The program was compiled and run using GCC inside WSL.

---

# Exercise 2

## Goal

Call `fork()` inside a loop `n` times and sleep for 5 seconds after every call.

`n` must be read from the command line.

The experiment is performed for:

- `n = 3`
- `n = 5`

The process tree is observed with `pstree`.

## Compile

```bash
gcc -Wall -Wextra -std=c11 ex2.c -o ex2
```

## Run for n = 3

```bash
./ex2 3 &
```

The shell prints the PID of the original background process. Use it with:

```bash
pstree -p <PID>
```

Run `pstree` several times while the program is still running.

### Result for n = 3

After 3 successful fork stages:

```text
2^3 = 8 processes in total
```

That means:

```text
1 original process + 7 newly created processes
```

## Run for n = 5

```bash
./ex2 5 &
pstree -p <PID>
```

### Result for n = 5

After 5 successful fork stages:

```text
2^5 = 32 processes in total
```

That means:

```text
1 original process + 31 newly created processes
```

## Observations

The number of processes grows exponentially rather than linearly.

After each `fork()`, both the parent and the newly created child continue executing the loop. Therefore, on the next iteration every existing process can create another process.

So, after `n` fork stages, the total number of processes is:

```text
2^n
```

For `n = 3`, there are 8 total processes.  
For `n = 5`, there are 32 total processes.

The `sleep(5)` calls make the intermediate process tree easier to observe with `pstree`.

## Problems / issues

At first it is easy to assume that calling `fork()` three times creates only three child processes. In reality, newly created children also continue the loop, so the number of processes grows exponentially.

Large values of `n` should be avoided because they can create many processes very quickly.

---

# Exercise 3

## Goal

Create a simplistic shell that:

- reads user input;
- runs commands such as `pwd`, `ls`, and `pstree`;
- supports commands with arguments, such as `ls -l`;
- creates a separate process with `fork()`;
- executes the requested program using `execve()`;
- does not use `system()`;
- does not require the shell operator `&` to run the child process in the background.

## Compile

```bash
gcc -Wall -Wextra -std=c11 ex3.c -o ex3
```

## Run

```bash
./ex3
```

Example session:

```text
myshell> pwd
[started PID 700]

myshell> ls -l
[started PID 701]

myshell> sleep 3
[started PID 702]

myshell> pstree
[started PID 703]

myshell> exit
```

The shell immediately returns to the prompt after creating the child process, so commands run in separate background processes.

## Observations

`fork()` creates a child process. The child then calls `execve()`, which replaces the child process image with the requested program.

If `execve()` succeeds, it does not return to the old child program. Code after a successful `execve()` call is therefore not executed.

Commands with arguments must be split into separate strings before they are passed to `execve()`.

The parent shell does not perform a blocking `wait()`, so it can accept another command while the previous command is still running.

## Problems / issues

One issue is command lookup. `execve()` requires a path to an executable, so the shell searches the directories listed in the `PATH` environment variable.

Another issue is zombie processes. Since commands run in the background, the shell periodically calls `waitpid(..., WNOHANG)` to reap child processes that have already finished.

Interactive programs such as `top` may behave awkwardly in this very simple shell because the shell and the child share the same terminal.

---

# Building everything

You can compile all programs manually:

```bash
gcc -Wall -Wextra -std=c11 ex1.c -o ex1
gcc -Wall -Wextra -std=c11 ex2.c -o ex2
gcc -Wall -Wextra -std=c11 ex3.c -o ex3
```

Or use:

```bash
make
```

Remove compiled files with:

```bash
make clean
```

---

# Suggested screenshots

Add terminal screenshots such as:

- `screenshots/ex1.png`
- `screenshots/ex2_n3.png`
- `screenshots/ex2_n5.png`
- `screenshots/ex3.png`

For Exercise 2, make sure at least one screenshot shows the output of `pstree -p`.
