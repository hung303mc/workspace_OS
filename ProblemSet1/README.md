# Priority Readers and Writers #
Write a multi-threaded C program that gives readers priority over writers concerning a shared (global) variable. Essentially, if any readers are waiting, then they have priority over writer threads -- writers can only write when there are no readers. This program should adhere to the following constraints:

* [ ] Multiple readers/writers must be supported (5 of each is fine)
* [ ] Readers must read the shared variable X number of times
* [ ] Writers must write the shared variable X number of times
* [ ] Readers must print:
  * [ ] The value read
  * [ ] The number of readers present when value is read
* [ ] Writers must print:
  * [ ] The written value
  * [ ] The number of readers present were when value is written (should be 0)
* [ ] Before a reader/writer attempts to access the shared variable it should wait some random amount of time
  * [ ] Note: This will help ensure that reads and writes do not occur all at once
* [ ] Use pthreads, mutexes, and condition variables to synchronize access to the shared variable
# Simple Socket: Client #
Write a simple C program that creates, initializes, and connects a client socket to a server socket. You should provide a way to specify the connecting server address and port. This can be hardcoded or passed via the command line.

Here are some example servers you may use for testing client connections:

- [example client 1](https://s3.amazonaws.com/content.udacity-data.com/courses/ud923/resources/ud923-ps1-simple-socket-client-exe-linux.zip) (executable, should work on VM)
- [example client 2](https://s3.amazonaws.com/content.udacity-data.com/courses/ud923/resources/ud923-ps1-simple-socket-client-exe.zip) (executable, compiled with gcc on Mac)

# Simple Socket: Server #
Write a simple C program that creates and initializes a server socket. Once initialized, the server should accept a client connection, close the connection, and then exit.

You should provide a way to specify the server's listening port. This can be hardcoded or passed via the command line.

Here are some example clients you may use for testing the server:

- [example server 1](https://s3.amazonaws.com/content.udacity-data.com/courses/ud923/resources/ud923-ps1-simple-socket-server-exe-linux.zip)  (executable, should work on VM)
- [example server 2](https://s3.amazonaws.com/content.udacity-data.com/courses/ud923/resources/ud923-ps1-simple-socket-server-exe.zip) (executable, compiled with gcc on Mac)

# The Echo Protocol #
In C, write a server and client that implement the fictitious "echo protocol". To implement the protocol, the server should accept any string from the client, and then return that string with all letters capitalized (if letters exist).

### Echo Protocol Example ###
Client sends "Hello, wOrlD"
Server echoes "HELLO, WORLD"
As soon as the server responds to a client, it may close. And, as soon as the clients receives a response, it may close. Feel free to reuse portions of the Simple Socket: Client and Simple Socket: Server programs.

To assist with implementation, we provided two example echo clients and echo servers:

- [example echo client and server](https://s3.amazonaws.com/content.udacity-data.com/courses/ud923/resources/ud923-ps1-the-echo-protocol-exe-linux.zip) (executables, should work on VM)
- [example echo client and server](https://s3.amazonaws.com/content.udacity-data.com/courses/ud923/resources/ud923-ps1-the-echo-protocol-exe.zip) (executables, compiled with gcc on Mac)
