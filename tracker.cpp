#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/socket.h>
#include<bits/stdc++.h>
#include <unistd.h>
#include <iostream>
using namespace std;
string log_file,trFile;
string ip1,ip2,curIp;
int port1,port2,curPort,trackerId;
vector<thread> th1;
// threadForQuit.join();
void printv(vector<string> v){
    for(auto s: v){
        cout<<s<<endl;
    }
}
void detectIfQuitIsInput(){
    while(1){
        string st;
        getline(cin, st);
        if(st== "quit"){
            exit(0);
        }
    }
}
int sampFun(int i){
    cout<<"Connection number "<<i<<" succefful"<<endl;
    return 0;
}
int processTracker(int trId){
    if(trId==2){
        curIp=ip2;
        curPort=port2;
    }
    else if(trId==1){
        curIp=ip1;
        curPort=port1;
    }
    log_file="trackerlog"+to_string(trId)+".txt";
    ofstream clearingFile;
    clearingFile.open(log_file);
    clearingFile.clear();
    clearingFile.close();
    ofstream filep(log_file,ios_base::out | ios_base::app);
    string logs1="tracker1: ip1 "+ip1+" port1: "+to_string(port1);
    string logs2="tracker2: ip2 "+ip2+" port2: "+to_string(port2);
    filep<<logs1<<endl;
    filep<<logs2<<endl;
    filep<<"log file name "+log_file<<endl;
    return 0;
}
void connHandler(int sock){
    while(1){

    }
}
int main(int arg, char *argv[]){
    thread threadForQuit(detectIfQuitIsInput);
    if(arg<=2 || arg>3){
        cout<<"Arguments less than 2 (insuffcient) must be 3..."<<endl;
    }
    else{

        //getting port and ip from tracker.txt

        trFile=argv[1];
        trackerId=stoi(argv[2]);
        ifstream fin(trFile);
        fin>>ip1>>port1>>ip2>>port2;
        fin.close();
        cout<<ip1<<" "<<port1<<endl;
        cout<<ip2<<" "<<port2<<endl;
        processTracker(trackerId);

        // socket creation

        int socketTracker;
        socketTracker=socket(AF_INET, SOCK_STREAM, 0);
        if(socketTracker<0){
            perror("connection of Socket failed");
            exit(1);
        }
        cout<<"socket for Tacker creation successful"<<endl;


        
        sockaddr_in addr;
        int addressLen=sizeof(addr),op=1;
        addr.sin_family = AF_INET; 
        addr.sin_port = htons(curPort);
        int inet1= inet_pton(AF_INET, &curIp[0], &addr.sin_addr);
        if(inet1 < 0)  { 
        printf("\nInvalid address/ Address not supported \n"); 
        return -1; 
    } 

        //binding socket to currentPort

        int binding = bind(socketTracker, (sockaddr*)&addr,  addressLen);
        if(binding<0){
            perror("binding for tracker failed");
            exit(1);
        }
        cout<<"binding for socket successful"<<endl;

        //listening for client 

        int listening = listen(socketTracker, 10);
        if(listening<0){
            perror("Listening failed");
            exit(1);
        }
        cout<<"Listening . . . "<<endl;
    int new_sock;
    while(1){
        new_sock = accept(socketTracker, (sockaddr *)&addr,(socklen_t*)&addressLen);
        if(new_sock<0){
            perror("connaction unsuccessful");
        }
        th1.push_back(thread(connHandler,new_sock));
    }
    for(auto it=th1.begin();it!=th1.end();it++){
        it->join();
    }

    }
    return 0;
}