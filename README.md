Here is a GitHub-style README file based on the information you provided:

```markdown
# RocksDB and MariaDB Server

This project implements a simple HTTP server using `cpp-httplib`, with a key-value store powered by RocksDB and a relational database backend using MariaDB. The server accepts POST requests to store JSON data in both RocksDB and MariaDB.

**Author**: Najmul Islam  
**Email**: najmulislamru@gmail.com  
**Date**: 2025-03-25  
**Last Modified**: 2025-03-25  

## Files
- **`database.h`**: Header file defining the `Database` class for RocksDB operations.
- **`database.cpp`**: Implementation of the `Database` class, handling RocksDB CRUD operations (create, read, update, delete) for JSON and array data.
- **`server.cpp`**: Main application file, setting up an HTTP server with a POST endpoint (`/api/bill`) to store data in both RocksDB and MariaDB.

## Features
- Stores full JSON data in RocksDB using a unique key.
- Extracts specific fields (`billing_doc_no`, `billing_date`, `da_code`) from JSON and saves them to a MariaDB table (`rdl_test`).
- Supports multi-process access to RocksDB (one writer, multiple readers).

## Prerequisites
To run this project on a Linux system (e.g., Ubuntu), install the following dependencies:

### 1. GCC/G++ (C++17 support)
```bash
sudo apt update
sudo apt install g++-7
# Verify: g++ --version (must support C++17).
```

### 2. RocksDB
```bash
sudo apt install librocksdb-dev
```

### 3. MariaDB/MySQL Connector/C++
```bash
sudo apt install libmysqlcppconn-dev
```

### 4. cpp-httplib
Download `httplib.h` from cpp-httplib GitHub:
```bash
mkdir cpp-httplib
wget -O cpp-httplib/httplib.h https://github.com/yhirose/cpp-httplib/raw/master/httplib.h
```

### 5. nlohmann/json
Download `json.hpp` from nlohmann/json GitHub:
```bash
mkdir nlohmann
wget -O nlohmann/json.hpp https://github.com/nlohmann/json/raw/develop/single_include/nlohmann/json.hpp
```

### 6. MariaDB Server
Install and configure MariaDB:
```bash
sudo apt install mariadb-server
sudo systemctl start mariadb
sudo mysql_secure_installation
```

## Directory Structure
```text
rocksdb/
├── cpp-httplib/
│   └── httplib.h
├── nlohmann/
│   └── json.hpp
├── database.h
├── database.cpp
└── server.cpp
```

## Compilation
Compile the project from the project root directory:

```bash
g++ -o server server.cpp database.cpp -lrocksdb -lmysqlcppconn -std=c++17 -Icpp-httplib -Inlohmann
# -lrocksdb: Links the RocksDB library.
# -lmysqlcppconn: Links the MySQL Connector/C++ library for MariaDB.
# -std=c++17: Enables C++17 features.
# -Icpp-httplib: Includes the cpp-httplib directory for httplib.h.
# -Inlohmann: Includes the nlohmann directory for json.hpp.
```

## MariaDB Setup
Log in to MariaDB:
```bash
sudo mysql -u root -p
```

Create the database and table:
```sql
CREATE DATABASE odms_dev_db;
USE odms_dev_db;
CREATE TABLE rdl_test (
    billing_doc_no VARCHAR(255),
    billing_date DATE,
    da_code INT
);
```

Optionally, make `billing_doc_no` a primary key if uniqueness is required:
```sql
CREATE TABLE rdl_test (
    billing_doc_no VARCHAR(255) PRIMARY KEY,
    billing_date DATE,
    da_code INT
);
```

Update connection details in `server.cpp` (around line 30) if necessary:
- Host: `tcp://test:3306` (replace `test` with `localhost` or your MariaDB host).
- Username: `root`.
- Password: `&j}d` (replace with your actual MariaDB root password).

## Running the Server
Ensure the RocksDB database directory exists and is writable:
```bash
mkdir -p /mnt/test/Desktop/Impala/learning/rocksdb/learning
chmod -R u+rwx /mnt/test/Desktop/Impala/learning/rocksdb/learning
# Note: The default RocksDB path in server.cpp is /mnt/test/Desktop/Impala/learning/rocksdb/learning. For portability, consider changing it to ./db in the code.
```

Start the server:
```bash
./server
# The server runs on 0.0.0.0:8080.
```

## Testing
Send a POST request to the `/api/bill` endpoint:

```bash
curl -X POST -H "Content-Type: application/json" -d '{"key":"123456","billing_doc_no":3,"billing_date":"2025-03-25","da_code":5001}' http://localhost:8080/api/bill
# Expected Response: {"status": "success", "key": "123456"}.
```

## Verification
- **RocksDB**: The full JSON is stored in `/mnt/test/Desktop/Impala/learning/rocksdb/learning` under the key `123456`.
- **MariaDB**: Check the `rdl_test` table:
  ```sql
  SELECT * FROM rdl_test;
  # Expected output: '3', '2025-03-25', 5001.
  ```

## Troubleshooting
### RocksDB Lock Errors:
If you see "Resource temporarily unavailable," remove the stale LOCK file:
```bash
rm /mnt/test/Desktop/Impala/learning/rocksdb/learning/LOCK
```

### Compilation Errors:
- Ensure all libraries are installed (`librocksdb-dev`, `libmysqlcppconn-dev`).
- Verify `httplib.h` and `json.hpp` are in the correct directories.

### MariaDB Connection Issues:
- Confirm the host (`test:3306`), username (`root`), and password (`&j}d`) in `server.cpp` match your MariaDB setup.
- Test connectivity:
  ```bash
  mysql -u root -p -h test
  ```

## Git Workflow
To push the project to a remote repository:

### Initialize the repository (if not already done):
```bash
git init
git config --global --add safe.directory /mnt/test/Desktop/Impala/learning/rocksdb
```

### Set your Git identity:
```bash
git config --global user.name "Najmul Islam"
git config --global user.email "najmulislamru@gmail.com"
```

### Add and commit files:
```bash
git add database.h database.cpp server.cpp
git commit -m "Initial commit with RocksDB and MariaDB server"
```

### Push to a remote:
```bash
git remote add origin <your-repo-url>
git push -u origin main
# Replace <your-repo-url> with your repository URL (e.g., https://github.com/username/repo.git).
```

## Notes
- **Path Portability**: The RocksDB path is hardcoded. Update `server.cpp` to use a relative path (e.g., `./db`) for easier deployment.
- **Error Handling**: The server assumes JSON fields exist and match expected types. Add validation if needed.
- **Multi-Process**: The `Database` class supports read-only access from other processes (`Database db("./db", true)`).

For further assistance, contact **Najmul Islam** at [najmulislamru@gmail.com](mailto:najmulislamru@gmail.com).
```

You can copy and paste this to your `README.md` file in the project directory.
