#!/usr/bin/env python3
#Author: Zachary Reese 
#Dates: 10/23/23-10/31/23
#Due-Date: 10/31/23 @ 11pm
#Course: CSC328
#Professor: Dr. Schwesinger
#Assignment: Project 5
#Purpose: This program serves as the client for reading from an executable server on the localhost.
    #The program is to handle 1-10 packets from the server (though not hard coded to do so) with a 
    #maximum size of 65537 bytes. Packets are sent with the first 2 bytes being the big endian format 
    #string byte size, followed by the string of varying length.
#Citations: 
#Makefile (cp & chmod usage), .decode(), python binary (b"" type usage)
#Note: Further citation information is given at the bottom of the file as specified by KU CS/IT 
   #Department's Academic Integrity Policy

#My chosen port #: 32582

import socket
import sys

#Function name: error_exit
#Description: Handles errors by accepting either the Exception itself or a string corresponding to a 
    #proper error message, followed by exiting the program.
#Parameters: errormsg- Accepts either an Exception or an error message (string) to output to user.
def error_exit(errormsg):
    print(f"ERROR. {errormsg} \nError of type: {type(errormsg).__name__}")
    sys.exit(1)


#Function name: review_word_packet
#Description: Receives entire word packet one character at a time. If read in successfully, the word 
    #will be returned to main.
    #Note: One character at a time is inefficient compared to reading chunks at a time, but is still 
        #accepted by the professor when using Python.
#Parameters: client_socket- Socket object connected to user specified port for reading.
def receive_word_packet(client_socket):
    try:
        word_length_bytes = client_socket.recv(2) #standardized size of 2 bytes before each word to 
                                #represent the big endian integer of the size of the string in bytes.
    except Exception as err:
        error_exit(err)
    if not word_length_bytes: #if empty, then reached end of reading packets
        return None #Indicate the end of data in main

    word_length = int.from_bytes(word_length_bytes, 'big') #converts word length from binary big endian to int-16.
    word_data = b"" #empty binary variable

    while len(word_data) < word_length: #loop until the word read in's length equals the word to be read in's length
        try:
            chunk = client_socket.recv(word_length - len(word_data)) #read host message from port
        except Exception as err:
            error_exit(err)
        if not chunk: #if the end of file for port's data has been reached early
            error_exit("End of string reached. Incomplete packet received")
        word_data += chunk

    word_string = word_data.decode("utf-8") #extract the word string from binary word_data
    return word_string;

#Function name: connect_socket
#Description: Declares a socket to connect to user's supplied host and port #.
#Parameters: host- The user supplied host. (Our case, 'localhost')
    #port- The user supplied port number. (10000-65535)
def connect_socket(host, port):
    try: #attempt to connect to port
        client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        client_socket.connect((host, port))
        return client_socket
    except ConnectionRefusedError as err: #The port hasn't been opened by a host yet
        error_exit("Server connection not found ")
    except Exception as err:
        error_exit(err)

#Function name: main
#Description: Takes over "__main__" as the main function for this Python project.
def main(): #main function
    if len(sys.argv) != 3:
        print("ERROR. Usage: python project5 <host> <port>")
    host = sys.argv[1]
    port = int(sys.argv[2])

    if (port < 10000 or port > 65535): #validate port # input
        errorExit("Only enter five digit port numbers between 10000-65535.")

    client_socket = connect_socket(host, port)

    while True: #infinite loops that stops when end of file while reading port has been reached.
        word_string = receive_word_packet(client_socket)
        if not word_string: #if empty, then reached end of reading packets
            break

        print(word_string)

    client_socket.close()

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
#.decode()
   #Author: Pankaj @ DigitalOcean
   #Title: Pythin String encode() decode()
   #Retrieved: 10/28 at https://www.digitalocean.com/community/tutorials/python-string-encode-decode
#
#python binary (b"" type usage)
  #Author: Python (developers on python.org)
  #Title: struct — Interpret bytes as packed binary data
  #Retrieved: 10/28 at https://docs.python.org/3/library/struct.html
