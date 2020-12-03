#include <stdlib.h>
#include <stdio.h>
#include<bits/stdc++.h>
#include <ctype.h>          
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <signal.h>
#include<math.h>
#include<cstring>
#include <unistd.h>
#include <thread>
#include <semaphore.h>
#include <errno.h>
#include <string.h>
#include<pthread.h>
#include<fstream>
#include <unistd.h>
#include<bits/stdc++.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <grp.h>
using namespace std;
sem_t m;
char buffer[1024]={0};
string TRIP,TRPort,peerIP,peerPort;
string req;
string curusr;
vector<string> parsedString;
bool isloggedin=false;
vector<string> Parser(string str){
	string str1="";
	vector<string> v1;
	for(int i=0;i<int(str.size());i++){
		if(str[i]==' '){
			v1.push_back(str1);
			str1="";		
		}
		else str1+=str[i];	
	}
	v1.push_back(str1);
	return v1;
} 
void* servercode(void* arg)
{
	int socket1,val,socket2;
	socklen_t size;
	if((socket1=socket(AF_INET,SOCK_STREAM,0))<0)
	{
      perror("failed to obtained the socket descriptor");
       exit(1);
	}   
	struct sockaddr_in myaddr;
	struct sockaddr_in otheraddr;
   
  myaddr.sin_port=htons(stoi(peerPort));
	myaddr.sin_family=AF_INET;
    
	inet_pton(AF_INET,peerIP.c_str() , &myaddr.sin_addr); 
  //myaddr.sin_addr.s_addr=TR1ip;

  bzero(&(myaddr.sin_zero),8);

  if(bind(socket1,(struct sockaddr *)&myaddr,sizeof(struct sockaddr))==-1)
  {
   perror("error in binding ");
   exit(1);
  }
  if(listen(socket1,1000)==-1)
  {
     perror("Waiting queue full");
     exit(1);

  }
  size=sizeof(struct sockaddr);
 while((socket2=accept(socket1,(struct sockaddr *)&otheraddr,&size))!=-1)
  {
   int port=(ntohs(otheraddr.sin_port));
   string ip=inet_ntoa(otheraddr.sin_addr);
	pthread_t newth;
  // pthread_create(serverequest,newsocketdes,ip,port);
   size=sizeof(struct sockaddr);
  }
  /*vector<thread>:: iterator it;
  for(it=threadVector.begin();it!=threadVector.end();it++)

   {
      if(it->joinable()) 
         it->join();
   }*/
  cout<<"Returning from server "<<endl;
}
//void* createuser(void* ar)
void clientdriver()
{
   //cout<<"creating user"<<endl;
   int socketdes;
   int newsocketdes;
   struct sockaddr_in trakeraddr;
   if((socketdes=socket(AF_INET,SOCK_STREAM,0))==-1)
   {
   	perror("Failed to obtain socket descriptor");
   	exit(1);
   }
   trakeraddr.sin_family=AF_INET;
   trakeraddr.sin_port=htons(stoi(TRPort));
   inet_pton(AF_INET,TRIP.c_str() , &trakeraddr.sin_addr); 
   trakeraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
   bzero(&(trakeraddr.sin_zero),8);
   if(connect(socketdes,(struct sockaddr *)&trakeraddr,sizeof(struct sockaddr))==-1)
   {
   	perror("Connect failed");
   	exit(1);
   }
   req+=" "+curusr;
   req+=" "+peerPort;
   send(socketdes,req.c_str(),strlen(req.c_str()),0);
   memset(buffer, 0, sizeof(buffer));

   int valread = read( socketdes, buffer, 1024);
   string chk=(string)buffer;
   vector<string> parsedchk=Parser(chk);
   if(parsedString[0]=="download_file"){
		string filepath=parsedchk[0];
		string port=parsedchk[1];
		string grpid=parsedString[1];
		string filename=parsedString[2];
		string despath=parsedString[3];
		string fileinfo=filepath+filename;
		ifstream in(filepath+filename,ios::ate|ios::binary);
		int size=in.tellg();
		in.close();
   		struct stat sl;
	cout<<filepath<<" "<<filename<<" "<<fileinfo<<endl;
	cout<<"Connected to "<<port<<endl;
	cout<<"Download in progress........ "<<endl;
    //check if the file already exist on the destination location
    
        int size1;		
	char buffer[524288];                
	int s=open((fileinfo).c_str(),O_RDONLY);
	int d=open(despath.c_str(),O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);		
	while((size1=read(s,buffer,524288))>0)
        {
            write(d,buffer,524288);
        }
        close(d);
        close(s);
        cout<<"File is downloaded successfully "<<endl;
       
    
		
   }
   else
   cout<<(string)buffer<<endl;
}
int main(int argc,char** argv){
	sem_init(&m,1,0);
	if(argc!=3){
		cout<<"Please give command line argument in the format <IP>:<PORT> tracker_info.txt"<<endl;
		return 0;	
	}
	string peerIPandPort=argv[1];
	string trackerpath=argv[2];
	
	int flg=0;	
	for(int i=0;i<int(peerIPandPort.size());i++){
		if(peerIPandPort[i]==':') {
			flg=1;
			continue;		
		}
		if(flg==0) peerIP+=peerIPandPort[i];
		else peerPort+=peerIPandPort[i];
	}
	vector<string>IPandPortTracker;
	string t;	
	fstream serverstream(trackerpath,ios::in);
	while(getline(serverstream,t)){
		IPandPortTracker.push_back(t);
	}
	flg=0;
	for(int i=0;i<int(IPandPortTracker[0].size());i++){
		if(IPandPortTracker[0][i]==':') {
			flg=1;
			continue;		
		}
		if(flg==0) TRIP+=IPandPortTracker[0][i];
		else TRPort+=IPandPortTracker[0][i];
	}	
	pthread_t serverpeer;
	pthread_create(&serverpeer,NULL,&servercode,NULL);
	int st=pthread_detach(serverpeer);	
	while(1){
	getline(cin,req);
	parsedString=Parser(req);
	if(parsedString[0]=="create_user"){	
		if(parsedString.size()!=3){
			cout<<"Invalid input\n";
			continue;
		}
		pthread_t crtnewuser; 
		const char* c=req.c_str();
		//cout<<"creating thread";
		//pthread_create(&crtnewuser,NULL,&createuser,NULL);
		clientdriver();
	}
	else if(parsedString[0]=="login"){
		if(parsedString.size()!=3){
			cout<<"Invalid input\n";
			continue;
		}
		else if(isloggedin) cout<<"Already logged in "<<endl;
		else {
			curusr=parsedString[1];
			clientdriver();
			isloggedin=true;
		}		
	}
	else if(parsedString[0]=="logout"){
		if(parsedString.size()!=1){
			cout<<"Invalid input\n";
			continue;
		}		
		if(!isloggedin) cout<<"Not logged in"<<endl;
		else{
			clientdriver();			
			curusr="";
			isloggedin=false;
		}
	}
	else if(parsedString[0]=="create_group"){
		if(parsedString.size()!=2){
			cout<<"Invalid input\n";
			continue;
		}	
		if(!isloggedin) cout<<"You need to login first"<<endl;
		else
		clientdriver();	
	}
	else if(parsedString[0]=="join_group"){
		if(parsedString.size()!=2){
			cout<<"Invalid input\n";
			continue;
		}	
		if(!isloggedin) cout<<"You need to login first"<<endl;
		else
		clientdriver();	
	}
	else if(parsedString[0]=="list_requests"){
		if(parsedString.size()!=2){
			cout<<"Invalid input\n";
			continue;
		}	
		if(!isloggedin) cout<<"You need to login first"<<endl;
		else
		clientdriver();	
	}
	else if(parsedString[0]=="list_groups"){
		if(parsedString.size()!=1){
			cout<<"Invalid input\n";
			continue;
		}	
		clientdriver();	
	}
	else if(parsedString[0]=="accept_request"){
		if(parsedString.size()!=3){
			cout<<"Invalid input\n";
			continue;
		}	
		if(!isloggedin) cout<<"You need to login first"<<endl;
		else
		clientdriver();	
	}
	else if(parsedString[0]=="leave_group"){
		if(parsedString.size()!=2){
			cout<<"Invalid input\n";
			continue;
		}	
		if(!isloggedin) cout<<"You need to login first"<<endl;
		else
		clientdriver();	
	}
	else if(parsedString[0]=="upload_file"){
		if(parsedString.size()!=3){
			cout<<"Invalid input\n";
			continue;
		}	
		if(!isloggedin) cout<<"You need to login first"<<endl;
		else
		clientdriver();	
	}
	else if(parsedString[0]=="list_files"){	
		if(parsedString.size()!=2){
			cout<<"Invalid input\n";
			continue;
		}
		if(!isloggedin) cout<<"You need to login first"<<endl;
		else
		clientdriver();	
	}
	else if(parsedString[0]=="download_file"){
		if(parsedString.size()!=4){
			cout<<"Invalid input\n";
			continue;
		}	
		if(!isloggedin) cout<<"You need to login first"<<endl;
		else
		clientdriver();	
	}
	else if(parsedString[0]=="show_downloads"){	
		cout<<"Not implemented"<<endl;	
	}
	else if(parsedString[0]=="stop_share"){	
		cout<<"Not implemented"<<endl;	
	}
	else{
		cout<<"Invalid input\n";
		continue;	
	}
	}
	return 0;
}
