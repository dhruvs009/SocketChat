# Basic Chat Application using UNIX IPCs
This is a basic chat application I made during my Computer Architecture and Operating Systems course written in C using threads and UNIX domain sockets. This app lets you message different clients on the same system and talk to them individually at a time or everyone at once using username tags.

## How to run?
1. Clone the repository on your system using `git clone https://github.com/dhruvs009/SocketChat.git`. <br>
2. Navigate to the download directory. <br>
3. Run `make server` to create a server. <br>
4. Run `make SocketName="socket_address" Username"username user"` on a different terminal to create a user. Replace "socket_address" with that displayed by the server and "username with a username of your choice (without the quotes). <br>
5. On the user terminals write `@username message` to send a message to the username as mentioned. <br>

> Dhruv Sahnan <br>
> Computer Architecture and Operating Systems | Monsoon'19 <br>
> B.Tech CSAM | Batch of 2022 <br>
> IIIT Delhi
