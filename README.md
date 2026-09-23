# Process Termination Message System

A Linux-based systems programming project that monitors and reports the termination of child processes created by a parent process — built to demonstrate core OS concepts like process creation, synchronization, and signal-based termination.

**Course:** Operating Systems and Systems Programming (25CS2104E) — 2026–27, Term-I
**Section:** 08 | **Team:** 10

## Team Members

| Reg. No. | Name |
|---|---|
| 2520030432 | K Anusri |
| 2520030440 | K Anshu |
| 2520030193 | K Dheeraj |
| 2520030028 | B Mohith |

## Abstract

In Linux, a parent process must collect information about a terminated child to determine how and why it ended. This project implements a system where a parent process spawns multiple child processes using `fork()`, each performing a distinct task before terminating — either normally or under simulated abnormal conditions.

The parent then uses `wait()` / `waitpid()` to collect each child's termination status, and inspects it using macros like `WIFEXITED()`, `WEXITSTATUS()`, and `WIFSIGNALED()` to report the child's PID, exit status, and whether it exited normally or was killed by a signal. Properly collecting this status prevents terminated children from lingering as zombie processes.

## Problem Statement

When multiple child processes are created in a Linux system, the parent needs a reliable mechanism to detect their termination and extract meaningful information about how they ended. Without proper handling, terminated children remain as zombie processes and waste system resources.

This project builds a Process Termination Message System that creates multiple child processes, monitors their termination, collects exit status via `wait()`/`waitpid()`, identifies normal vs. signal-based termination, and displays a clear termination message for each — while preventing zombies.

## Objectives

1. Create multiple child processes using the `fork()` system call.
2. Detect and monitor the termination of child processes.
3. Use `wait()` or `waitpid()` to collect and analyze child-process termination status.
4. Identify normal vs. abnormal termination and prevent child processes from becoming zombies.

## Methodology

The system is implemented and tested on a Linux/Ubuntu platform in C. A parent process creates multiple children via `fork()`; each child performs a simple, distinct task before terminating. Some children exit normally via `exit()`; others simulate abnormal termination via signals. The parent waits for each child and analyzes its status with `WIFEXITED()`, `WEXITSTATUS()`, and `WIFSIGNALED()`, then prints the PID and termination outcome — ensuring every child is reaped and no zombies are left behind.

**Flow:**

```
Create Child Processes (fork())
        ↓
   Execute Child Task
        ↓
Terminate Normally or via Signal
        ↓
Parent Collects Status (wait()/waitpid())
        ↓
Analyse Status (WIFEXITED() / WEXITSTATUS() / WIFSIGNALED())
        ↓
   Display Termination Message
```

## Operating System Concepts / Linux APIs Used

| API / Concept | Purpose |
|---|---|
| `fork()` | Creates multiple child processes |
| `wait()` | Allows the parent to wait for child termination |
| `waitpid()` | Collects the termination status of specific child processes |
| `getpid()` | Obtains and displays the PID of processes |
| `exit()` | Allows a child process to terminate normally |
| Signals | Demonstrates signal-based process termination |
| `WIFEXITED()` | Checks whether a child terminated normally |
| `WEXITSTATUS()` | Obtains the exit status of a normally terminated child |
| `WIFSIGNALED()` | Checks whether a child was terminated by a signal |

## Tools / Platforms / Software

- **Linux / Ubuntu** — OS environment for implementation and testing
- **C Programming Language** — core development language
- **GCC Compiler** — compiling the source
- **VS Code / Text Editor** — writing and editing source code
- **Terminal** — running and testing the program
- **GDB** — debugging
- **GitHub** — version control and repository management

## Individual Contributions

- **K Anusri:** Process creation using `fork()` and overall project integration
- **K Anshu:** Implementation of normal child-process termination and exit status handling
- **K Dheeraj:** Implementation of abnormal termination and signal handling
- **B Mohith:** Testing, output analysis, documentation, and GitHub repository management

## Expected Outcome

A functional Linux-based application that creates multiple child processes and monitors their termination in real time — displaying each child's PID and termination status (normal or signal-based) while guaranteeing no terminated child remains a zombie. The final system demonstrates use of `fork()`, `wait()`, and `waitpid()`, giving a clear, practical picture of process lifecycle management and parent-child relationships in Linux.

## Repository

[github.com/kanusri-tmj/Jarvis-kernel_OSSP](https://github.com/kanusri-tmj/Jarvis-kernel_OSSP)
