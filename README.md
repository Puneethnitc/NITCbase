# 🗄️ NITCBase — Mini Relational Database Management System

<div align="center">

![Language](https://img.shields.io/badge/Language-C%2B%2B-blue?style=flat-square&logo=cplusplus)
![Platform](https://img.shields.io/badge/Platform-Linux-orange?style=flat-square&logo=linux)
![Status](https://img.shields.io/badge/Status-Completed-brightgreen?style=flat-square)
![Stages](https://img.shields.io/badge/Stages-12%2F12-purple?style=flat-square)
![Course](https://img.shields.io/badge/Course-DB%20Systems%20%26%20Design%20Lab-grey?style=flat-square)

*A fully implemented mini RDBMS built from scratch in C++ — disk, buffer, cache, indexing, and an interactive command interface included.*

</div>

---

## Overview

NITCBase is a fully implemented mini Relational Database Management System (RDBMS) built from scratch in C++, developed as part of the **Database Systems and Design Lab** at **NIT Calicut**. It covers every internal layer of a real database engine — from disk simulation and buffer management to B+ Tree indexing and an interactive SQL-like command-line interface.

This project is designed for educational purposes, providing hands-on experience with how a database works under the hood, implemented across ~3000 lines of C++ code in 12 progressive stages.

---

## Highlights

- **Simulated 16MB disk** with block-level I/O
- **32-block LRU buffer pool** for fast in-memory access
- **Relation & attribute cache** for up to 12 open relations
- **B+ Tree indexing** for efficient attribute-based search
- **Relational algebra** — select, project, insert, and equi-join
- **Custom CLI** with SQL-like syntax and batch script support
- **Two interfaces** — Frontend Interface and XFS Interface

---

## Architecture

![NITCbase Overview](https://nitcbase.github.io/assets/images/HomepageFigure-3caab8b424c41a15c0958e84619dc2c0.svg)

NITCBase follows a strict **eight-layer design**, where each layer communicates only with the one directly below it:

```
User Command
     │
     ▼
Frontend Interface        ← Parses SQL-like commands
     │
     ▼
Schema Layer / Algebra Layer   ← DDL and DML logic
     │
     ▼
Block Access Layer        ← Core select, project, join
     │
     ├──────────────────┐
     ▼                  ▼
Cache Layer         B+ Tree Layer     ← Metadata caching & indexing
     │
     ▼
Buffer Layer              ← LRU 32-block disk cache
     │
     ▼
Physical Layer            ← Raw disk read / write
```

---

## Table of Contents

- [Getting Started](#getting-started)
  - [Using Docker (Recommended)](#using-docker-recommended)
  - [Manual Setup](#manual-setup)
- [Usage](#usage)
- [Project Structure](#project-structure)
- [Documentation](#documentation)
- [Contributing](#contributing)
- [License](#license)

---

## Getting Started

### Using Docker (Recommended)

Docker is the recommended way to run NITCBase. It ensures a consistent environment regardless of your host OS.

**Prerequisites:** [Install Docker](https://docs.docker.com/get-docker/) on your machine. Linux users should also follow the [post-installation steps](https://docs.docker.com/engine/install/linux-postinstall/) to run Docker without `sudo`.

#### 1. Set up the directory structure

```bash
git clone https://github.com/Puneethnitc/NITCbase.git
cd NITCbase
mkdir -p NITCbase   # local folder that maps into the container
```

Your directory should look like:

```
.
├── Dockerfile
└── NITCbase/       ← your working files live here
```

#### 2. Build the container image

```bash
docker build -t nitcbase:ubuntu20.04 .
```

#### 3. Start the container

```bash
docker run -v $PWD/NITCbase:/home/nitcbase/NITCbase -d --name nitcbase -i nitcbase:ubuntu20.04
```

This starts a container in the background and mounts your local `NITCbase/` folder into it, so your files persist outside the container.

#### 4. Connect to the container

```bash
docker start nitcbase                   # if not already running
docker exec -it nitcbase /bin/bash      # open a shell inside
```

#### 5. Run the setup script

Inside the container:

```bash
cd /home/nitcbase
./setup.sh
```

This bootstraps the NITCBase environment and creates the required directory structure.

#### 6. Initialise the disk

```bash
cd XFS_Interface
./xfs-interface
```

Inside the XFS prompt, format the disk:

```
# fdisk
Disk formatted
# exit
```

#### 7. Build and run NITCBase

```bash
cd /home/nitcbase/NITCbase/mynitcbase
make
./nitcbase
```

You are now inside the NITCBase interactive shell.

---

### Manual Setup

> Manual setup is not officially supported. Follow at your own discretion.

**Prerequisites:**

```bash
sudo apt-get install build-essential libreadline-dev git
```

**Steps:**

```bash
# 1. Clone the repository
git clone https://github.com/Puneethnitc/NITCbase.git
cd NITCbase/mynitcbase

# 2. Build
make

# 3. Initialise the disk via XFS Interface (first time only)
cd ../XFS_Interface
./xfs-interface
# inside prompt: run `fdisk`, then `exit`

# 4. Run NITCBase
cd ../mynitcbase
./nitcbase
```

---

## Usage

NITCBase uses its own command syntax. Commands resemble SQL but follow NITCBase conventions — for example, `SELECT` requires an `INTO` target relation.

```sql
-- Create and open a table
CREATE TABLE Students (RollNo NUM, Name STR, CGPA NUM, Dept STR);
OPEN TABLE Students;

-- Insert records
INSERT INTO Students VALUES (1, Alice, 9.5, CSE);
INSERT INTO Students VALUES (2, Bob, 8.2, ECE);

-- Select with a condition
SELECT * FROM Students INTO TopStudents WHERE CGPA >= 9.0;

-- Project specific attributes
SELECT RollNo, Name FROM Students INTO Names WHERE Dept = CSE;

-- Create a B+ Tree index
CREATE INDEX ON Students.CGPA;

-- Equi-join two relations
SELECT * FROM Students JOIN Courses INTO Enrolled WHERE Students.RollNo = Courses.RollNo;

-- Run a batch script
RUN myscript.txt;

-- Close the table
CLOSE TABLE Students;
```

For the complete command reference, see the [NITCBase User Interface Documentation](https://nitcbase.github.io/docs/User%20Interface%20Commands/).

---

## Project Structure

After setup, your working directory will look like this:

```
NITCbase/
├── Disk/                        # Disk binary file (XFS simulation)
├── XFS_Interface/               # XFS interface source + executable
├── Files/
│   ├── Batch_Execution_Files/   # Scripts for the RUN command
│   ├── Input_Files/             # Input data files (import, insert from file)
│   └── Output_Files/            # Output from dump and export commands
└── mynitcbase/                  # All implemented layers live here
    ├── define/                  # Global constants
    ├── Disk_Class/              # Physical layer (provided)
    ├── Buffer/                  # Buffer layer — StaticBuffer, BlockBuffer
    ├── Cache/                   # Cache layer — RelCacheTable, AttrCacheTable, OpenRelTable
    ├── BPlusTree/               # B+ Tree layer
    ├── BlockAccess/             # Block Access layer
    ├── Schema/                  # Schema layer
    ├── Algebra/                 # Algebra layer
    ├── Frontend/                # Frontend programming interface
    └── FrontendInterface/       # Frontend user interface (provided)
```

---

## Documentation

| Resource | Link |
|---|---|
| Official Documentation | [nitcbase.github.io](https://nitcbase.github.io/) |
| Installation Guide | [Installation Guidelines](https://nitcbase.github.io/docs/Misc/Installation%20Guidelines) |
| Architecture Overview | [Design/Architecture](https://nitcbase.github.io/docs/Design/Architecture) |
| Implementation Roadmap | [Roadmap](https://nitcbase.github.io/docs/Roadmap) |
| User Interface Commands | [Commands](https://nitcbase.github.io/docs/User%20Interface%20Commands/) |
| B+ Tree Tutorial | [Misc/B+ Trees](https://nitcbase.github.io/docs/Misc/B+%20Trees) |

---

## Contributing

Contributions are welcome. To contribute:

1. Fork the repository
2. Create a new branch — `git checkout -b feature/your-feature`
3. Commit your changes — `git commit -m 'Add some feature'`
4. Push to the branch — `git push origin feature/your-feature`
5. Open a Pull Request

Please follow clear naming conventions and include a short description of your changes.

If you encounter any issues, feel free to [open an issue](https://github.com/Puneethnitc/NITCbase/issues).

---

## License

NITCBase is licensed under the [MIT License](LICENSE).  
The NITCBase framework and documentation are developed by NIT Calicut and distributed under [CC BY-NC 4.0](http://creativecommons.org/licenses/by-nc/4.0/).

---

<div align="center">

**Course:** Database Systems and Design Lab &nbsp;|&nbsp; **Institution:** NIT Calicut &nbsp;|&nbsp; **Spec:** [nitcbase.github.io](https://nitcbase.github.io/)

</div>
