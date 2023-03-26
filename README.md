# Local "client-server" interactions through UNIX shell variables
This project on C is a local "client-server" (daemon) interactions through UNIX shell variables concept.

## Prerequisites
* UNIX operating system
* GCC compiler

## Installation

> All the commands must be entered in the terminal
1. Clone this repository to your local machine.
2. Compile the server program using the following command:
```
gcc server.c -o server
```
> or just use the already existing server-program in the same directory
3. Compile the client program using the following command:
```
gcc client.c -o client
```
> or just use the already existing client-program in the same directory

## Usage

1. Firstly, we need to open system journal with actievly following logs in the separate terminal:

``` 
$ journalctl -f 
```

2. Now open a server-app

```
$client-server-c/bin/server ./server
```

In logs-terminal we will see:
> user-name server[*number*]: Server opened

3. Now open a client-app

``` 
$client-server-c/bin/client ./client 
``` 
In the terminal we see:
> Client *number* started <br />
> ----------------Menu-------------------     <br />
> -n *file*      - N last file lines         <br />
> -s *file*      - is file a symbolic link   <br />
> -m *file*      - file meta-data            <br />
> <br />
> stopServer      - stop server               <br />
> ---------------------------------------     <br />
> <br />
> Incoming request to server:

4. For example let's check file's metadata

> Incoming request to server: ``` -m *filepath* ``` <br />
> <br />
> Last status change: *date of status change*  <br />
> Last file access: *date of last file access*    <br />
> Last file change: *date of last file change*    <br />
> <br />
> Client *number* finished working!

In logs-terminal we will see   <br />
> user-name server[*number*]: Got request from *client_number*                   <br />
> user-name server[*number*]: Client *client_number*: file exist                 <br />
> user-name server[*number*]: Client request *client_number* has been completed  <br />

## Channels (streams)

Also the program uses channels (streams), performance profit of which we can test 

### In the client-program directory we have 2 shell-scripts: </br>

> scriptmanual.sh
``` 
# Setup
echo Hello, World! > test

# Start
../server/./server

# Test
echo "-s test" && ./client
echo "-s test" && ./client
echo "-s test" && ./client
echo "-s test" && ./client
echo "-s test" && ./client
echo "-s test" && ./client
echo "-s test" && ./client

# Stop
killall server 
``` 

> scriptparallel.sh

``` 
# Setup
echo Hello, World! > test

# Start
../server/./server

# Test
echo "-s test" || ./client
echo "-s test" || ./client
echo "-s test" || ./client
echo "-s test" || ./client
echo "-s test" || ./client
echo "-s test" || ./client
echo "-s test" || ./client

# Stop
killall server
``` 
Obviously, ***scriptparallel.sh*** uses channels to each program it operates at the same time, unlike ***scriptmanual.sh*** which goes one by one. </br>
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
