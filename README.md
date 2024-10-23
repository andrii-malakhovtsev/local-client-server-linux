# Local Client-Server Interactions Using UNIX Shell Variables
This C project demonstrates local client-server (daemon) interactions using UNIX shell variables.

## Prerequisites
- UNIX-based operating system
- GCC compiler

## Installation

> All commands should be executed in the terminal.

1. Clone this repository to your local machine:

    ```
    git clone https://github.com/malandrii/local-client-server-linux
    ```
2. Compile the server program:

   ```
    gcc server.c -o server
    ```
    > Alternatively, you can use the precompiled server program included in the directory.
4. Compile the client program:

   ```
    gcc client.c -o client
    ```
    > Alternatively, you can use the precompiled client program included in the directory.

## Usage

1. Open a terminal and follow the system journal logs:
    ``` 
    journalctl -f 
    ```

2. Launch the server application:
    ```
    ./bin/server ./server
    ```
    You should see the following output in the log terminal:
    ```
    [username] server[number]: Server opened
    ```

3. In another terminal, start the client application:
    ``` 
    ./bin/client ./client 
    ``` 
    The following menu will appear:
    ```
    Client [number] started
    ----------------Menu-------------------
    -n [file]  - Show the last N lines of the file
    -s [file]  - Check if the file is a symbolic link
    -m [file]  - Display file metadata
    stopServer - Stop the server
    ---------------------------------------
    Incoming request to server:
    ```

4. For example, to check file metadata, run:
    ```
    -m [filepath]
    ```
    The output will be similar to:
    ```
    Last status change: [date]
    Last file access: [date]
    Last file change: [date]
    Client [number] finished working!
    ```

    In the log terminal, you will see:
    ```
    [username] server[number]: Received request from client [number]
    [username] server[number]: Client [number]: file exists
    [username] server[number]: Request from client [number] completed
    ```

## Channels (Streams)

This program uses channels (streams), which can be tested for performance benefits.

### The client program directory contains two shell scripts:

- **scriptmanual.sh**
    ```
    # Setup
    echo "Hello, World!" > test

    # Start the server
    ../server/./server

    # Test
    echo "-s test" && ./client
    echo "-s test" && ./client
    echo "-s test" && ./client

    # Stop the server
    killall server
    ```

- **scriptparallel.sh**
    ```
    # Setup
    echo "Hello, World!" > test

    # Start the server
    ../server/./server

    # Test in parallel
    echo "-s test" || ./client &
    echo "-s test" || ./client &
    echo "-s test" || ./client &

    # Stop the server
    killall server
    ```

The `scriptparallel.sh` runs all client programs in parallel, whereas `scriptmanual.sh` runs them one by one.

### Test on ***scriptparallel.sh*** showed these results: </br>
``` 
$client-server-c/bin/client time sh scriptmanual.sh
``` 
> <pre>real   0m0.752s </br>
> user   0m0.014s </br>
> sys    0m0.016s </pre>
``` 
$client-server-c/bin/client time sh scriptparallel.sh 
``` 
> <pre>real   0m0.024s  </br>
> user   0m0.003s  </br>
> sys    0m0.014s  </pre>
