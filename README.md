## local "client-server" interactions through UNIX shell variables concept

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
