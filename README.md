# tcp_chat

STEPS:

1. Compile 2 files individually;  
2. Run server with a specific port as a paramater( example : ./tcpserver 3000);
3. run client passing the IP address of the server and the port as parameters (example : ./tcpclient 127.0.0.1 3000)

ALTERNATIVE:
1. There is a raspberry client in the tags that you can run with the command make and then compile the tcp_chat_rasp.c file for the raspberry pi 4 b and run it
2. The green led in the board should blink every time it receives a message


FUNCTIONALITIES: 

1. each client can send message via client application;
2. server broadcasts messages to all clients excepts the client sending the message;
3. server prints status of each client connected every 10 sec;

MISSING:

1. Raspberry client is not tested, it should turn the led on everytime it receives;
2. Makefiles still not implemented
3. led device driver not attached to project
4. clients cant be removed

