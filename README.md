# Academia — Multi-User University Management System

> **C · POSIX Sockets · Pthreads · File-based Storage**  
> IIIT Bangalore · Systems Programming Mini Project · IMT2023567

---

## Overview

**Academia** is a multi-user, concurrent university management system built entirely in C using raw POSIX sockets and POSIX threads. It follows a classic client-server architecture where multiple clients connect simultaneously over TCP, and the server handles role-based operations for three distinct user types — **Admin**, **Faculty**, and **Student**.

All data is persisted to flat binary files, with file-level locking to ensure safe concurrent access across threads.

---

## Features

### Admin
- Create / activate / deactivate faculty and student accounts
- Add, remove, and manage courses
- Assign faculty to courses
- View all users and course enrollments

### Faculty
- View courses assigned to them
- View enrolled students per course
- Update their profile

### Student
- Browse available (active) courses
- Enroll in / unenroll from courses (seat-limited)
- View their enrolled courses
- Update their profile

---

## Architecture

```
academia/
├── server/
│   ├── include/
│   │   ├── constants.h          # Shared constants (PORT, limits, status codes)
│   │   ├── user.h               # User, Student, Faculty structs
│   │   ├── course.h             # Course, Enrollment structs + CRUD prototypes
│   │   ├── auth.h               # Authentication prototypes
│   │   ├── file_lock.h          # POSIX read-write lock wrappers
│   │   ├── admin_handler.h
│   │   ├── faculty_handler.h
│   │   └── student_handler.h
│   ├── src/
│   │   ├── server.c             # Main: socket setup, pthread dispatch, role routing
│   │   ├── auth.c               # Login / credential verification
│   │   ├── user.c               # User CRUD (binary file I/O)
│   │   ├── course.c             # Course & enrollment CRUD
│   │   ├── file_lock.c          # Thread-safe file locking
│   │   ├── admin_handler.c      # Admin menu handler
│   │   ├── faculty_handler.c    # Faculty menu handler
│   │   └── student_handler.c    # Student menu handler
│   └── Makefile
├── client/
│   ├── include/
│   │   └── client.h
│   ├── src/
│   │   └── client.c             # TCP client — connects and sends commands
│   └── Makefile
├── data/                        # Runtime-generated binary data files
│   ├── admin.dat
│   ├── faculty.dat
│   ├── student.dat
│   └── course.dat
├── Documentation.pdf            # Full project documentation
└── output_example.pdf           # Sample run output
```

---

## Build & Run

### Prerequisites
- GCC (any modern version)
- POSIX-compliant OS (Linux / macOS)
- `pthread` library (usually included)

### Build

```bash
# Build the server
cd server && make

# Build the client (in a separate terminal)
cd client && make
```

### Run

```bash
# Terminal 1 — start the server (listens on port 8080)
cd server && ./server

# Terminal 2+ — connect one or more clients
cd client && ./client
```

A default admin account is auto-created on first launch. See `Documentation.pdf` for credentials and full usage walkthrough.

---

## Technical Details

| Property | Value |
|---|---|
| Language | C (C11) |
| Transport | TCP over POSIX sockets (`AF_INET`, port 8080) |
| Concurrency | `pthread_create` per client, up to `MAX_CLIENTS = 10` |
| Data storage | Binary flat files (`.dat`) with custom structs |
| Locking | `fcntl`-based file locking (`file_lock.c`) |
| Max courses per student | 20 |
| Max seats per course | 50 |

---

## Documentation

- **`Documentation.pdf`** — full design document covering protocol, data structures, and all supported operations
- **`output_example.pdf`** — sample terminal output showing a complete session across Admin, Faculty, and Student roles
