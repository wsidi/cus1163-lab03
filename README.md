# Lab 3: Basic Process Management and IPC

#### Learning Objectives

- Understand fork() system call - How processes are created and copied
- Understand wait() system call - How parent processes manage child lifecycle
- Practice basic pipe() communication - Simple inter-process data transfer

#### Prerequisites

- Basic C programming knowledge
- Familiarity with system calls from previous labs
- Understanding of file descriptors and I/O operations

#### Introduction

Process management is a fundamental aspect of operating systems. This lab explores the three most important process
management concepts: process creation with fork(), process cleanup with wait(), and basic inter-process communication
with pipes.

You'll implement a simple producer-consumer system where producers generate sequential numbers and consumers add them
up. The focus is entirely on understanding process behavior - the arithmetic is just busy work to give the processes
something to do.

#### What You'll Implement

You need to write just 2 functions:

1. `run_basic_demo()` - Creates one producer and one consumer child process
2. `run_multiple_pairs()` - Creates multiple producer-consumer pairs

Everything else is provided for you.

#### Lab File Structure

**main.c (PROVIDED - Complete)**

This file contains the main entry point for your lab application. It presents a simple menu-driven interface for the
user, allowing them to select between running the basic producer-consumer demo, running multiple producer-consumer
pairs, or exiting the program.

The file handles user input, calls functions implemented in process_manager.c, and displays results and status messages.
This file is fully implemented and requires no modifications.

**process_manager.h (PROVIDED)**

This header file contains function prototypes for the process management operations implemented in process_manager.c. It
includes all necessary system headers for process operations, function prototypes for process creation and management,
constants used throughout the lab, and helper function declarations.

No changes are necessary for this file.

**process_manager.c (TEMPLATE - TO COMPLETE)**

This file contains the core logic for your process management operations. You will implement process creation using
fork(), pipe creation and management for inter-process communication, parent process coordination and cleanup using
wait(), and multiple process pair management.

The main functions are provided with TODO comments indicating where you must fill in the missing code. Follow the
instructions and hints to complete the required functionality using system calls.

#### Project Setup

1. Download the three files
2. Complete the 6 TODOs in process_manager.c
3. Compile: `gcc -o lab3 main.c process_manager.c`
4. Run: `./lab3`

#### The Functions You'll Implement

#### Function 1: run_basic_demo() - Understanding Basic Process Creation

This function demonstrates the fundamental process creation pattern used throughout operating systems. Think of it like
a factory manager who needs to create two workers - one to produce items and another to package them, with a conveyor
belt between them for communication.

The function teaches you how fork() creates a copy of the current process, how parent and child processes can execute
different code paths, how pipe() enables communication between related processes, and how wait() ensures proper process
cleanup.

Here's what happens step by step:

1. Parent process creates a pipe for communication
2. Parent forks a producer child that generates numbers 1-5
3. Parent forks a consumer child that receives and adds the numbers
4. Parent waits for both children to complete their work
5. All processes terminate cleanly

#### Function 2: run_multiple_pairs() - Scaling Process Management

This function demonstrates how operating systems manage multiple concurrent processes efficiently. It's like a factory
manager creating multiple production lines, each with their own producer-consumer pair and conveyor belt, all running
simultaneously.

The function teaches you how to create and coordinate multiple process pairs, how each pair operates independently with
its own communication channel, how the parent process tracks and manages multiple children, and how proper resource
cleanup prevents system resource leaks.

Here's what happens:

1. Parent creates multiple independent producer-consumer pairs
2. Each pair gets its own pipe and sequential number range (1-5, 6-10, etc.)
3. All pairs run concurrently, demonstrating process parallelism
4. Parent tracks all child processes and waits for each to complete
5. Resources are properly cleaned up to prevent zombie processes

#### Expected Output

**Option 1: Basic Producer-Consumer Demo**

```
Starting basic producer-consumer demonstration...

Parent process (PID: 15234) creating children...
Created producer child (PID: 15235)
Created consumer child (PID: 15236)

Producer (PID: 15235) starting...
Consumer (PID: 15236) starting...

Producer: Sent number 1
Producer: Sent number 2
Producer: Sent number 3
Producer: Sent number 4
Producer: Sent number 5
Producer: Finished sending 5 numbers

Consumer: Received 1, running sum: 1
Consumer: Received 2, running sum: 3
Consumer: Received 3, running sum: 6
Consumer: Received 4, running sum: 10
Consumer: Received 5, running sum: 15
Consumer: Final sum: 15

Producer child (PID: 15235) exited with status 0
Consumer child (PID: 15236) exited with status 0

SUCCESS: Basic producer-consumer completed!
```

**Option 2: Multiple Producer-Consumer Pairs**

```
Running multiple producer-consumer pairs...

Parent creating 2 producer-consumer pairs...

=== Pair 1 ===
Producer (PID: 15240) starting...
Consumer (PID: 15241) starting...
Producer: Sent number 1
Consumer: Received 1, running sum: 1
[... continues with numbers 2-5 ...]
Consumer: Final sum: 15

=== Pair 2 ===
Producer (PID: 15242) starting...
Consumer (PID: 15243) starting...
Producer: Sent number 6
Consumer: Received 6, running sum: 6
[... continues with numbers 7-10 ...]
Consumer: Final sum: 40

All pairs completed successfully!
Child (PID: 15240) exited with status 0
Child (PID: 15241) exited with status 0
Child (PID: 15242) exited with status 0
Child (PID: 15243) exited with status 0

SUCCESS: Multiple pairs completed!
```

#### Key System Calls You'll Use

#### fork() - Process Creation

The fork() system call creates a new process by copying the current process. It's one of the most important system calls
in Unix-like operating systems. When you call fork(), the operating system creates an identical copy of your process,
including all variables, open files, and program state.

The magic happens in the return value. In the parent process, fork() returns the process ID (PID) of the newly created
child. In the child process, fork() returns 0. If something goes wrong, fork() returns -1.

Here's the typical pattern:

```c
pid_t pid = fork();
if (pid == 0) {
    // Child process code
    child_function();
    exit(0);
} else if (pid > 0) {
    // Parent process code
    printf("Created child (PID: %d)\n", pid);
} else {
    perror("fork failed");
}
```

#### pipe() - Inter-Process Communication

The pipe() system call creates a communication channel between processes. Think of it as creating a one-way tube where
one process can write data and another can read it. The pipe() call creates two file descriptors: pipe_fd[0] for reading
and pipe_fd[1] for writing.

The key rule with pipes is that you must close the unused ends. If the producer doesn't close the read end, or the
consumer doesn't close the write end, your processes might hang waiting for data that will never come.

Here's how to use pipes:

```c
int pipe_fd[2];
if (pipe(pipe_fd) == -1) {
    perror("pipe failed");
    return -1;
}

// Producer (writer)
close(pipe_fd[0]); // Close read end
write(pipe_fd[1], &data, sizeof(data));
close(pipe_fd[1]);

// Consumer (reader)
close(pipe_fd[1]); // Close write end
read(pipe_fd[0], &data, sizeof(data));
close(pipe_fd[0]);
```

#### wait() and waitpid() - Process Cleanup

When a child process finishes, it doesn't immediately disappear. Instead, it becomes a "zombie" process that hangs
around until its parent calls wait() to collect its exit status. If the parent never calls wait(), these zombie
processes accumulate and can eventually exhaust system resources.

The wait() system call makes the parent process pause until one of its children exits. The waitpid() variant lets you
wait for a specific child process. Both functions return the PID of the child that exited and provide its exit status.

Here's the pattern:

```c
int status;
pid_t child_pid = waitpid(specific_pid, &status, 0);
printf("Child %d exited with status %d\n", child_pid, status);
```

#### Implementation Guide

#### TODO 1: Create Pipe for Communication

```c
int pipe_fd[2];
if (pipe(pipe_fd) == -1) {
    perror("pipe");
    return -1;
}
```

#### TODO 2: Fork Producer Process

```c
producer_pid = fork();
if (producer_pid == 0) {
    // Child process - becomes producer
    close(pipe_fd[0]); // Close read end
    producer_process(pipe_fd[1], 1); // Start with number 1
} else if (producer_pid > 0) {
    printf("Created producer child (PID: %d)\n", producer_pid);
} else {
    perror("fork");
    return -1;
}
```

#### TODO 3: Fork Consumer Process

```c
consumer_pid = fork();
if (consumer_pid == 0) {
    // Child process - becomes consumer
    close(pipe_fd[1]); // Close write end
    consumer_process(pipe_fd[0], 0); // Pair ID 0 for basic demo
} else if (consumer_pid > 0) {
    printf("Created consumer child (PID: %d)\n", consumer_pid);
} else {
    perror("fork");
    return -1;
}
```

#### TODO 4: Parent Cleanup and Wait

```c
// Parent doesn't use pipe, so close both ends
close(pipe_fd[0]);
close(pipe_fd[1]);

// Wait for both children to complete
int status;
waitpid(producer_pid, &status, 0);
printf("Producer child (PID: %d) exited with status %d\n", producer_pid, status);

waitpid(consumer_pid, &status, 0);
printf("Consumer child (PID: %d) exited with status %d\n", consumer_pid, status);
```

#### Why These Concepts Matter

Process creation through fork() is how every program you run gets started. When you type a command in your terminal, the
shell forks a new process to run that command. Web servers fork to handle multiple clients simultaneously. Database
systems fork to process queries in parallel.

Inter-process communication through pipes is equally fundamental. When you type `ls | grep pattern` in your terminal,
the shell creates a pipe between the ls and grep processes. System services use pipes to communicate with each other.
Data processing pipelines move information between different stages using pipes.

Process management through wait() keeps the system running smoothly. Without proper cleanup, systems would be filled
with zombie processes consuming memory and process table entries. Every well-behaved parent process must wait for its
children.

#### Common Mistakes to Avoid

Forgetting to close pipe ends is probably the most common mistake. If you don't close the unused ends of a pipe,
processes will hang waiting for data that never comes. The rule is simple: close the read end in the writer process, and
close the write end in the reader process.

Not calling wait() creates zombie processes. These are processes that have finished running but haven't been cleaned up
by their parent. They consume system resources and can eventually cause problems.

Getting the fork() logic wrong can create infinite loops of process creation that can crash your system. Always check
the return value of fork() and handle all three cases: child (0), parent (positive PID), and error (-1).

Confusing pipe directions is another common issue. Remember that pipe_fd[0] is for reading and pipe_fd[1] is for
writing. Don't try to read from the write end or write to the read end.

#### Analysis Questions

After completing the lab, think about these questions:

1. What happens to memory when fork() creates a new process? Does the child get a complete copy of the parent's memory?

2. Why must unused pipe ends be closed? What would happen if you forgot to close them?

3. What are zombie processes and how does wait() prevent them? What would happen to your system if zombie processes
   accumulated?

4. How does the kernel track parent-child relationships? What happens if a parent process dies before its children?

#### What You're Really Learning

The techniques you're practicing in this lab are the building blocks of modern computing. Web servers use these patterns
to create worker processes that handle client requests. Database systems fork processes to handle concurrent queries.
Operating system shells use fork and pipes to run commands and create pipelines.

Even though you're just adding numbers, you're learning the fundamental process management patterns that power
everything from simple command-line tools to complex distributed systems. Every time you open a web browser, start a
program, or run a command, these same system calls are working behind the scenes.

The number-adding is just busy work to give your processes something to do. The real learning is understanding how
fork() creates processes, how pipes let them communicate, and how wait() keeps everything running smoothly.

#### Compilation and Execution

```bash
gcc -o lab3 main.c process_manager.c
./lab3
```

#### Submission Requirements

After completing your work:

```
git add .
git commit -m "completed lab 2 - process information reader"
git push origin main
```

Include:

- Complete the 6 TODOs in process_manager.c
- Screenshots showing program output
- Test that both menu options work correctly
- Verify no zombie processes remain (all children properly waited for)
- Ensure predictable output with sequential numbers