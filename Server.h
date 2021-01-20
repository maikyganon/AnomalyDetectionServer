#ifndef SERVER_H_
#define SERVER_H_

#include <iostream>
#include <sys/socket.h>
#include <pthread.h>
#include <thread>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sstream>
#include <unistd.h>

#include "CLI.h"


using namespace std;

// edit your ClientHandler interface here:
class ClientHandler{
    public:
    virtual void handle(int clientID)=0;
};


// you can add helper classes here and implement on the cpp file
class serverIO:public DefaultIO{
    int clientID;
public:
    serverIO(int id):clientID(id){

    }
    virtual string read(){
        char buffer[1024];
        int n= ::read(clientID,buffer, 1024);
        buffer[n] = '\0';
        string str(buffer);
        return str;
    }
    virtual void write(string text){
        //send(clientID,&text[0],strlen(&text[0]), 0);
        send(clientID,&text[0],text.size(), 0);
    }

    virtual void write(float f){
        std::stringstream ss;
        ss << f;
        std::string text(ss.str());
        send(clientID,&text[0],text.size(), 0);
    }

    virtual void read(float* f){
        char buffer[100];
        int n= ::read(clientID,buffer, sizeof(float));
        float* temp = (float*)buffer;
        *f=*temp;
    }

    void close(){

    }
    ~serverIO(){
        close();
    }
};

// edit your AnomalyDetectionHandler class here
class AnomalyDetectionHandler:public ClientHandler{

	public:
    AnomalyDetectionHandler(){

    }
    virtual void handle(int clientID){
        CLI cli(new serverIO(clientID));
        cli.start();
    }
};


// implement on Server.cpp
class Server {
	thread* t; // the thread to run the start() method in
    int fd;
    sockaddr_in server;
    sockaddr_in client;
	// you may add data members

public:
	Server(int port) throw (const char*);
	virtual ~Server();
	void start(ClientHandler& ch)throw(const char*);
	void stop();
};

#endif /* SERVER_H_ */
