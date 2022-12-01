## local "client-server(daemon)" interactions through UNIX shell variables concept

Firstly, we need to open system journal with actievly <br />
following logs in separate terminal
> $ journalctl -f

Now open a server-app
> $client-server-c/bin/server ./server

In logs-terminal we will see
> user-name server[*number*]: Server opened

Now open a client-app
> $client-server-c/bin/client ./client <br />
> <pre>
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

For example let's check file's metadata

> Incoming request to server: -m *filepath* <br />
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
<br />
<b> Also the program uses channels, profit of which we can test </b> </br> 
</br>
In client directory we have 2 shell-scripts </br> </br>
<pre>
<b>scriptmanual.sh </b> <br />

\# Setup
echo Hello, World! > test

\# Start
../server/./server

\# Test
echo "-s test" && ./client
echo "-s test" && ./client
echo "-s test" && ./client
echo "-s test" && ./client
echo "-s test" && ./client
echo "-s test" && ./client
echo "-s test" && ./client

\# Stop
killall server
</pre>
</br>
<pre>
<b>scriptparallel.sh </b> <br />

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
</pre> </br>
Obviously, ***scriptparallel.sh*** uses channels to each program it has at the same time, unlike ***scriptmanual.sh*** which goes one by one. </br>
> $client-server-c/bin/client time sh scriptmanual.sh </br> </br>
> <pre>real   0m0.752s </br>
> user   0m0.014s </br>
> sys    0m0.016s </pre>
</br>

> $client-server-c/bin/client time sh scriptparallel.sh </br> </br>
> <pre>real   0m0.024s  </br>
> user   0m0.003s  </br>
> sys    0m0.014s  </pre>
