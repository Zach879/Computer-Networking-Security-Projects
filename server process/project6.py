#!/usr/bin/env python3
#Author: Zachary Reese
#Dates: 10/31/23-11/7/23
#Due-Date: 11/7/23 @ 11pm
#Course: CSC328
#Professor: PROFESSOR NAME REDACTED
#Assignment: Project 6
#Purpose: This program serves as the server for sending data to clients via ports. The program sends
    #1-10 packets to the client with a maximum size of 65537 bytes (per packet). Packets are sent with
    #the first 2 bytes being the big endian string byte size, followed by the string of varying length.
#Citations:
    #Makefile (cp & chmod usage) and .encode() (both used on project 5, too)
#Note: Further citation information is given at the bottom of the file as specified by KU CS/IT
   #Department's Academic Integrity Policy

#My chosen port #'s: 32582 and 32583

import socket
import sys
import random

    #word_packets = [len(x).to_bytes(2, 'big') + x.encode() for x in WORDS]

MAX_PACKETS = 10 # \/ list of 30 random strings
POSSIBLE_STRINGS = ["sector ", "distribute", "wage", "generate", "competence", "split", "radio", "welfare", "grace", "curve", "count", "lounge", "variant", "defendant", "empirical", "linger", "franchise", "lineage", "right", "strict", "declaration", "expenditure", "calculate", "appointment", "tournament", "bag", "from", "buy", "construct", "slide"]

#Function name: error_exit
#Description: Handles errors by accepting either the Exception itself or a string corresponding to a
    #proper error message, followed by exiting the program. Outputs exception type to further assist
    #in understanding the purpose of an error.
#Parameters: errormsg- Accepts either an Exception or an error message (string) to output to user.
def error_exit(errormsg):
    print(f"ERROR. {errormsg} \nError of type: {type(errormsg).__name__}")
    sys.exit(1)

#Function name: ctrl_exit
#Description: Handles controlled exit cases, specifically, when the user hits control + C to close
    #the server program. When called, handles closing the server's socket file descriptor.
#Parameters: server_socket- File descriptor that connects server to user specified port number to
                #listen for connections (to be closed).
    #client_socket- File descriptor that connects server to the client to transfer data to client for
        #reading (to be closed). client_socket will either be a file descriptor when used  at least
        #once to send data, or None when not used to send data. If None, then there's nothing to close.
def ctrl_exit(server_socket, client_socket):
    server_socket.close()
    if client_socket is not None:
        client_socket.close()
    print("\nClosing connection..")

#Function name: gen_word_packet_packet
#Description: Handles the generation of a new word packet with a random word from the list of possible strings.
#Return type: Tuple word packet containing 2 bytes of the string size, and the rest of the 1-65535
    #bytes are the binary utf-8 encoded word.
def gen_word_packet_packet():
    random_word = random.choice(POSSIBLE_STRINGS) #chooses a random element in a list of values
    word_size = len(random_word)
    binary_size = word_size.to_bytes(2, 'big') #convert int to binary big endian
    word_packet = binary_size + random_word.encode('utf-8') #convert str to binary
    return word_packet

#Function name: connect_socket
#Description: Declares a socket to connect to user's supplied port #.
#Parameters: port- The user supplied port number. (10000-65535, though not checked)
#Return type: A newly connected file descriptor to the server's port connection to be used until closed.
def connect_socket(port):
    try: #attempt to connect to port
        server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM) #create new socket object and set parameters
        server_socket.bind(('', port)) #connects socket object to network interface and port
        return server_socket
    except Exception as err:
        error_exit(err)

def main(): #main function
    if len(sys.argv) != 2:
        error_exit("Usage: ./server <port>")
    port = int(sys.argv[1])

    client_socket = None
    try:
        server_socket = connect_socket(port)
        server_socket.listen(1) #start listening for incoming connections
        print("Server accepting connections..")
        while True:
            client_socket, (clientIP, clientPort) = server_socket.accept() #waits for connection to server's port
            print(clientIP)
            print(clientPort)
            for _ in range(random.randint(1,MAX_PACKETS)):
                client_socket.sendall(gen_word_packet_packet())
            client_socket.close()
    except KeyboardInterrupt:
        ctrl_exit(server_socket, client_socket)
    except Exception as err:
        error_exit(err)

#Python default "main function" (is a conditional block serving as a construct for when project5.py
    #is the main project file). Calls custom main function
if __name__ == "__main__":
    main()

#Citations:
#Makefile (cp & chmod usage)
    #Author: Dr. Schwesinger
    #Title: Python/Makefile
    #Retrieved: 10/31 at /export/home/public/schwesin/csc328/examples/project4/Python/Makefile
#
#.encode()
   #Author: Pankaj @ DigitalOcean
   #Title: Pythin String encode() decode()
   #Retrieved: 10/28 at https://www.digitalocean.com/community/tutorials/python-string-encode-decode
#