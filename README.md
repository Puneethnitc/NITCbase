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

| Layer | Responsibility | Code |
|---|---|---|
| Frontend Interface | Parses and routes SQL-like user commands | ~100 lines |
| Schema Layer | DDL: `CREATE`, `DROP`, `OPEN`, `CLOSE`, `RENAME` | ~200 lines |
| Algebra Layer | DML: `INSERT`, `SELECT`, `PROJECT`, `JOIN` | ~400 lines |
| Cache Layer | Relation & attribute metadata for open tables | ~500 lines |
| B+ Tree Layer | Index creation, insertion, and search | ~500 lines |
| Block Access Layer | Core select and project on disk blocks | ~500 lines |
| Buffer Layer | LRU-based 32-block in-memory disk cache | ~500 lines |
| Physical Layer | `readBlock()` / `writeBlock()` to XFS disk | *(provided)* |

---

## Table of Contents

- [Prerequisites](#prerequisites)
- [Getting Started](#getting-started)
- [Usage](#usage)
- [Project Structure](#project-structure)
- [Documentation](#documentation)
- [Contributing](#contributing)
- [License](#license)

---

## Prerequisites

- Linux-based OS (tested on Ubuntu 20.04+)
- C/C++ compiler (`gcc` / `g++`)
- `make`
- `libreadline-dev`
- `git`

Install all required packages:

```bash
sudo apt-get install build-essential libreadline-dev git
```

---

## Getting Started

### Installation

```bash
# 1. Clone the repository
git clone https://github.com/Puneethnitc/NITCbase.git

# 2. Navigate to the project directory
cd NITCbase/mynitcbase

# 3. Build
make
```

### Running NITCBase

**Frontend Interface** — interactive SQL-like shell:

```bash
./nitcbase
```

**XFS Interface** — disk-level management tool:

```bash
./xfs-interface
```

> **First-time setup:** Run `fdisk` inside the XFS Interface to format the simulated disk before first use.

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

```
NITCbase/
├── Disk_Class/         # Physical layer — disk simulation (provided)
├── Buffer/             # Buffer layer — StaticBuffer, BlockBuffer
├── Cache/              # Cache layer — RelCacheTable, AttrCacheTable, OpenRelTable
├── BPlusTree/          # B+ Tree layer — indexing
├── BlockAccess/        # Block Access layer — select, project
├── Schema/             # Schema layer — DDL operations
├── Algebra/            # Algebra layer — DML operations
├── Frontend/           # Frontend programming interface
├── FrontendInterface/  # Frontend user interface (provided)
└── XFS Interface/      # XFS disk management tool (provided)
```

---

## Documentation

| Resource | Link |
|---|---|
| Official Documentation | [nitcbase.github.io](https://nitcbase.github.io/) |
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
