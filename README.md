# XV6 Operating System Project

## Overview
This project focuses on enhancing the XV6 operating system by adding custom system calls and features. Additionally, a multithreaded file management system was implemented as part of the second phase.

## Part 1: Customized XV6 System Calls
In this part, I implemented several new system calls and synchronization mechanisms in XV6:

### 1. `killall()`
- **Description**: Kills all descendant processes of the current process.
- **Usage**: Ensures that child processes and their descendants are terminated.

### 2. `waitpid(pid)`
- **Description**: Waits until a process with a specific process ID terminates.
- **Usage**: Used for parent-child process synchronization.

### 3. Message Queue
- **Functions**:
  - `messageQueueInit()`: Initializes a message queue.
  - `sendMessage()`: Sends a message to the queue.
  - `getMessage()`: Retrieves a message from the queue.

### 4. `getppid()`
- **Description**: Retrieves the parent process ID (PID).
- **Usage**: Provides a way to obtain the PID of the parent process.

### 5. Semaphores
- **Functions**:
  - `sem_init()`: Initializes a semaphore.
  - `sem_wait()`: Decrements the semaphore value, blocking if the value is zero.
  - `sem_signal()`: Increments the semaphore value, potentially unblocking waiting processes.

## Part 2: Multithreaded File Management System
This part involves the development of a multithreaded file management system, designed as a client-server-based application to handle file operations efficiently in a concurrent environment.

### Features Implemented

1. **Concurrent File Reading**
   - Multiple threads can read from a file simultaneously without conflicts.

2. **Exclusive File Writing**
   - Only one thread can write to a file at a time, ensuring data consistency.

3. **File Deletion**
   - Users can securely delete files using a thread-safe mechanism.

4. **File Renaming**
   - Provides a safe method to rename files, avoiding conflicts during operations.

5. **File Copying**
   - Supports creating copies of files using dedicated threads.

6. **File Metadata Display**
   - Shows detailed metadata such as file size, creation date, and permissions.

7. **Error Handling**
   - Handles errors gracefully, including scenarios like file not found, permission denied, or disk space issues.

8. **Logging Operations**
   - Maintains a log of all file operations for transparency and debugging.

9. **Compression and Decompression**
   - Provides functionality for file compression and decompression to optimize storage.

### Non-Functional Requirements

1. **Performance**
   - Ensures fast and efficient handling of file operations, even with multiple concurrent users.

2. **Scalability**
   - Supports an increasing number of concurrent users and files without significant performance degradation.

3. **Security**
   - Protects file access with proper locking mechanisms to prevent unauthorized or conflicting operations.

4. **Usability**
   - Offers a user-friendly GUI with intuitive controls for all supported operations.

5. **Reliability**
   - Guarantees stable operation with minimal downtime, even under heavy load or unexpected failures.

6. **Portability**
   - Ensures compatibility across different platforms for the client-side GUI.

7. **Maintainability**
   - Uses a modular code structure to simplify future updates and enhancements.

### System Constraints

1. The server must run on a system capable of supporting multithreading in C, with sufficient resources to handle concurrent operations.
2. The client GUI requires Python and its dependencies, including socket programming libraries, to function correctly.
3. File operations are limited to the permissions and storage capacity of the server’s file system.
4. Network latency may impact communication speed between the client and server.
5. The system requires reliable network connectivity to ensure uninterrupted client-server interaction.
6. Compression and decompression operations may increase CPU usage during execution.

### Assumptions and Dependencies

1. Assumes that the system will be used on devices with sufficient hardware resources.
2. Relies on stable network connectivity for seamless client-server communication.
3. Requires proper configuration of file permissions on the server.

## Conclusion
This project enhances the functionality of the XV6 operating system by introducing new system calls and synchronization primitives. Additionally, the multithreaded file management system provides a robust and efficient solution for handling file operations in a concurrent environment. By leveraging multithreading and a client-server architecture, it ensures secure, reliable, and scalable file management.
