#include<arpa/inet.h>
#include<stdio.h>
#include<string.h>
#include<fcntl.h>
#include<sys/socket.h>
#include<bits/stdc++.h>
#include<unistd.h>
#include<iostream>
using namespace std;
#define PORT 5000
vector<thread> threads;
string ip1,ip2,peerIp,logFile;
int port1,port2,peerPort;
bool loggedIn=false;
unordered_map<string,string> mapUIDnPass;
void displayLogFile(string s){
    ofstream filep(logFile,ios_base::out | ios_base::app);
    filep<<s<<endl;
}
void commandFun(vector<string> commnd,int sk){
    char resp[10240];
    bzero(resp,10240);
    read(sk,resp,10240);
    cout<<resp<<endl;
    displayLogFile("primary server response: "+ string(resp));
    string command=commnd[0];
    if(command=="logout"){
        loggedIn=false;
    }
    else if(command=="login"){
        if(string(resp) == "logged in"){
            loggedIn = true;
            string peerAddress = peerIp + ":" + to_string(peerPort);
            write(sk, &peerAddress[0], peerAddress.size());
        }
    }
}
void display(vector<string> v){
    for(auto i: v){
        cout<<i<<endl;
    }
}

int connectToTracker(int trackerNum,sockaddr_in &serv_addr, int sock){
    string curTrackIP;
    int curTrackPort;
    if(trackerNum == 1){
        curTrackIP = ip1; 
        curTrackPort = port1;
    }
    else{
        curTrackIP = ip2; 
        curTrackPort = port2;
    }

    bool err = 0;

    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(curTrackPort); 
       
    if(inet_pton(AF_INET, curTrackIP.c_str(), &serv_addr.sin_addr)<=0)  { 
        err = 1;
    } 
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) { 
        err = 1;
    } 
    if(err){
        if(trackerNum == 1)
            return connectToTracker(2, serv_addr, sock);
        else
            return -1;
    }
    displayLogFile("connected to server " + to_string(curTrackPort));
    return 0;
}

pair<string,string> extractIPandPort(string ipPlusPort){
    string samp1="";
    string samp2="";
    int index=0;
    for(int i=0;i< ipPlusPort.length();i++){
        if(ipPlusPort[i]==':'){
            index=i;
            break;
        }
    }
    samp1=ipPlusPort.substr(0,index);
    samp2=ipPlusPort.substr(index+1);
    return {samp1,samp2};
}
void processArgs(int arg,char *argv[]){
    
    string peerInfo=argv[1],fName=argv[2];
    logFile=peerInfo+"_log.txt";
    ifstream fin(fName);
    ofstream out;
    out.open(logFile);
    out.clear();
    out.close();
    auto x=extractIPandPort(peerInfo);
    fin>>ip1>>port1>>ip2>>port2;
    fin.close();
    peerIp=x.first;
    peerPort=stoi(x.second);
    displayLogFile("Peer Address: "+peerIp+":"+to_string(peerPort));
    displayLogFile("Tracker1 address: "+ip1+":"+to_string(port1));
    displayLogFile("Tracker2 address: "+ip2+":"+to_string(port2));
    displayLogFile("Logfile name : "+logFile+"\n");
}
int main(int arg, char *argv[]){
    if(arg!=3){
        cout<<"Arguments insufficient"<<endl;
        exit(1);
    }
    else{
        processArgs(arg,argv);
        int sk=socket(AF_INET, SOCK_STREAM, 0);
        if(sk<0){
            cout<<"Connection not created"<<endl;
            return -1;
        }
        else{
            cout<<"Connection is created"<<endl;
        }
        //thread th;
        sockaddr_in serv_addr;
        if(connectToTracker(1,serv_addr, sk)<0){
            cout<<"Connection cannot be created"<<endl;
            exit(1);
        }
        while(1){
            cout<<"--->>";
            string input,st;
            getline(cin,input);
            stringstream s(input);
            vector<string> commnd;
            while(s>>st){
                commnd.push_back(st);
            }
            //display(commnd);
            if(commnd[0]=="login" && loggedIn==true){
                cout<<"already logged in"<<endl;
                continue;
            }
            if(send(sk , &input[0] , input.size() , MSG_NOSIGNAL ) == -1){
                printf("Error: %s\n",strerror(errno));
                return -1;
            }
            displayLogFile("sent to server"+input[0]);
            commandFun(commnd,sk);
        }
    }
    return 0;
}