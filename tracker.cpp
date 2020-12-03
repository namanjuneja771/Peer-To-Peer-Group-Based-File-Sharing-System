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
using namespace std;
string TRIP,TRPort;
char buffer[1024]={0};
unordered_map<string,string> usrnmpas;
unordered_map<string,vector<string>> grpmem;
unordered_map<string,vector<string>> grpreq;
unordered_map<string,unordered_map<string,set<pair<string,string>>>> grpfileusrport;
unordered_map<string,string> filepath;
map<pair<string,string>,bool> usrstatus;
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
void createuser(int newsocketdes){
	//cout<<"creating user";
	string str=(string)buffer;
	vector<string> v1=Parser(str);
	string usrnm=v1[1];
	string pass=v1[2];
	string peerport=v1[4];
	if(usrnmpas.find(usrnm)!=usrnmpas.end()){
		string ch="User already exists";
		send(newsocketdes,ch.c_str(),strlen(ch.c_str()),0);
	}
	else{
		usrnmpas.insert({usrnm,pass});
		usrstatus.insert({{usrnm,peerport},false});	
		string ch="User created successfully";
		send(newsocketdes,ch.c_str(),strlen(ch.c_str()),0);
	}	
}
void login(int newsocketdes){
	string str=(string)buffer;
	vector<string> v1=Parser(str);
	string usrnm=v1[1];
	string pass=v1[2];
	string peerport=v1[4];	
	if(usrnmpas.find(usrnm)==usrnmpas.end()){
		string ch="User doesnot exists";
		send(newsocketdes,ch.c_str(),strlen(ch.c_str()),0);
	}
	
	else if(pass==usrnmpas[usrnm]){
		usrstatus[{usrnm,peerport}]=true;
		string ch="Logged in as "+usrnm;
		send(newsocketdes,ch.c_str(),strlen(ch.c_str()),0);
	}
	else{
		string ch="Enter correct password ";
		send(newsocketdes,ch.c_str(),strlen(ch.c_str()),0);
		
	}
}
void creategrp(int newsocketdes,string curusr){ 
	string str=(string)buffer;
	vector<string> v1=Parser(str);
	string grpid=v1[1];	
	if(grpmem.find(grpid)!=grpmem.end()){
		string ch="Group already exists ";
		send(newsocketdes,ch.c_str(),strlen(ch.c_str()),0);
	}
	else{
		vector<string> mem;
		mem.push_back(v1[2]);
		grpmem.insert({grpid,mem});
		string ch="Group created with admin "+v1[2];
		send(newsocketdes,ch.c_str(),strlen(ch.c_str()),0);
	}	
}
void joingrp(int newsocketdes){
	string str=(string)buffer;
	vector<string> v1=Parser(str);
	string grpid=v1[1];
	string curusr=v1[2];
	if(grpmem.find(grpid)==grpmem.end()){
		string ch="Group doesnot exist ";
		send(newsocketdes,ch.c_str(),strlen(ch.c_str()),0);
	}
	else if(find(grpreq[grpid].begin(),grpreq[grpid].end(),curusr)!=grpreq[grpid].end()){
		string ch="Request already sent ";
		send(newsocketdes,ch.c_str(),strlen(ch.c_str()),0);
	}
	else{
		grpreq[grpid].push_back(curusr);
		string ch="Request sent successfully ";
		send(newsocketdes,ch.c_str(),strlen(ch.c_str()),0);
	}
}
void listreq(int newsocketdes){
	string str=(string)buffer;
	vector<string> v1=Parser(str);
	string grpid=v1[1];
	string curusr=v1[2];
	if(grpmem.find(grpid)==grpmem.end()){
		string ch="Group doesnot exist ";
		send(newsocketdes,ch.c_str(),strlen(ch.c_str()),0);
	}
	else if(grpmem[grpid].size()>0 and curusr!=grpmem[grpid][0]){
		string ch="You are not the admin ";
		send(newsocketdes,ch.c_str(),strlen(ch.c_str()),0);
	}
	else{
		vector<string> v1=grpreq[grpid];
		string ch=v1[0];
		for(int i=1;i<int(v1.size());i++) ch+=" "+v1[i];		
		send(newsocketdes,ch.c_str(),strlen(ch.c_str()),0);
	}
}
void listgrps(int newsocketdes){
	string ch="";
	for(auto i=grpmem.begin();i!=grpmem.end();i++) ch+=" "+i->first;			
	send(newsocketdes,ch.c_str(),strlen(ch.c_str()),0);
}
void acptrqst(int newsocketdes){
	string str=(string)buffer;
	vector<string> v1=Parser(str);
	string grpid=v1[1];
	string usrid=v1[2];
	string curusr=v1[3];
	if(grpmem.find(grpid)==grpmem.end()){
		string ch="Group doesnot exist ";
		send(newsocketdes,ch.c_str(),strlen(ch.c_str()),0);
	}
	else if(grpmem[grpid].size()>0 and curusr!=grpmem[grpid][0]){
		string ch="You are not the admin ";
		send(newsocketdes,ch.c_str(),strlen(ch.c_str()),0);
	}
	else{
		grpmem[grpid].push_back(usrid);
		for(auto i=grpreq[grpid].begin();i!=grpreq[grpid].end();i++){
			if(*i==usrid) {
				grpreq[grpid].erase(i);
				break;
			}		
		}
		string ch=usrid+" added successfully to group "+grpid;
		send(newsocketdes,ch.c_str(),strlen(ch.c_str()),0);
	}
}
void lvgrp(int newsocketdes){
	string str=(string)buffer;
	vector<string> v1=Parser(str);
	string grpid=v1[1];
	string curusr=v1[2];
	string peerport=v1[3];
	if(grpmem.find(grpid)==grpmem.end()){
		string ch="Group doesnot exist ";
		send(newsocketdes,ch.c_str(),strlen(ch.c_str()),0);
	}
	else if(find(grpmem[grpid].begin(),grpmem[grpid].end(),curusr)==grpmem[grpid].end()){
		string ch="You are not present in group "+grpid;
		send(newsocketdes,ch.c_str(),strlen(ch.c_str()),0);
	}
	else{
		for(auto i=grpmem[grpid].begin();i!=grpmem[grpid].end();i++){
			if(*i==curusr) {
				grpmem[grpid].erase(i);
				break;
			}		
		}
		int flg=1;
		auto i=grpfileusrport[grpid].begin();
		while(i!=grpfileusrport[grpid].end()){
			if((*i).second.size()==1 and (*i).second.find(make_pair(curusr,peerport))!=(*i).second.end()){
				i=grpfileusrport[grpid].erase(i);				
			}
			else{
				(*i).second.erase({curusr,peerport});
				i++;
			}
		}
	
		string ch="left group "+grpid;
		send(newsocketdes,ch.c_str(),strlen(ch.c_str()),0);
	}
}
vector<string> fileinfo(string s){
	string filepat="",filename="";
	int flg=0;
	for(int i=s.size()-1;i>=0;i--){
		if(flg==0 and s[i]=='/'){
			flg=1;
			filepat="/";		
		}
		else if(flg==1)			
			filepat=s[i]+filepat;
		else
			filename=s[i]+filename;
	}
	vector<string> v1;
	v1.push_back(filepat);
	v1.push_back(filename);
	filepath.insert({filename,filepat});
	return v1;
}
void upldfl(int newsocketdes){
	string str=(string)buffer;
	vector<string> v1=Parser(str);
	vector<string> fileid=fileinfo(v1[1]);
	string filename=fileid[1];
	string filepath=fileid[0];
	string grpid=v1[2];
	string curusr=v1[3];
	string peerport=v1[4];	
	if(grpmem.find(grpid)==grpmem.end()){
		string ch="Group doesnt exist ";
		send(newsocketdes,ch.c_str(),strlen(ch.c_str()),0);
		return;	
	}
	if(find(grpmem[grpid].begin(),grpmem[grpid].end(),curusr)==grpmem[grpid].end()){
		string ch="You are not a member of group "+grpid;
		send(newsocketdes,ch.c_str(),strlen(ch.c_str()),0);
		return;
	}
	if(grpfileusrport.find(grpid)==grpfileusrport.end()){
		set<pair<string,string>> s1;
		s1.insert(make_pair(curusr,peerport));
		unordered_map<string,set<pair<string,string>>> m1;
		m1.insert({filename,s1});		
		grpfileusrport.insert({grpid,m1});		
	}
	else if(grpfileusrport[grpid].find(filename)==grpfileusrport[grpid].end()){
		set<pair<string,string>> s1;	
		s1.insert(make_pair(curusr,peerport));
		grpfileusrport[grpid].insert({filename,s1});		
	}
	else{
		if(grpfileusrport[grpid][filename].find(make_pair(curusr,peerport))!=grpfileusrport[grpid][filename].end()){
			string ch="File already uploaded ";
			send(newsocketdes,ch.c_str(),strlen(ch.c_str()),0);
			return;	
		}
		grpfileusrport[grpid][filename].insert(make_pair(curusr,peerport));	
	}
	string ch="File uploaded successfully ";
	send(newsocketdes,ch.c_str(),strlen(ch.c_str()),0);
}
void lstfiles(int newsocketdes){
	string str=(string)buffer;
	vector<string> v1=Parser(str);
	string grpid=v1[1];
	string curusr=v1[2];
	string peerport=v1[3];
	string ch="";
	if(grpmem.find(grpid)==grpmem.end()){
		ch="Group doesnt exist ";
		send(newsocketdes,ch.c_str(),strlen(ch.c_str()),0);
		return;	
	}
	if(find(grpmem[grpid].begin(),grpmem[grpid].end(),curusr)==grpmem[grpid].end()){
		ch="You are not a member of group "+grpid;
		send(newsocketdes,ch.c_str(),strlen(ch.c_str()),0);
		return;
	}	
	if(grpfileusrport.find(grpid)==grpfileusrport.end()){
		ch="No files present in group "+grpid;
		send(newsocketdes,ch.c_str(),strlen(ch.c_str()),0);
		return;	
	}
	for(auto it=grpfileusrport[grpid].begin();it!=grpfileusrport[grpid].end();it++){
			bool st=false;
			for(auto it1=grpfileusrport[grpid][(*it).first].begin();it1!=grpfileusrport[grpid][(*it).first].end();it1++){
				if(usrstatus[*it1]){
					st=true;
					break;
				}			
			}
			if(st)
			ch+=(*it).first+" ";		
	}
	if(ch=="") ch="No files present in group "+grpid;
	send(newsocketdes,ch.c_str(),strlen(ch.c_str()),0);
}
void download(int newsocketdes){
	string str=(string)buffer;
	vector<string> v1=Parser(str);
	string grpid=v1[1];
	string filename=v1[2];
	string despath=v1[3];
	string curusr=v1[4];
	string ch="";
	if(grpmem.find(grpid)==grpmem.end()){
		ch="Group doesnt exist ";
		send(newsocketdes,ch.c_str(),strlen(ch.c_str()),0);
		return;	
	}
	if(find(grpmem[grpid].begin(),grpmem[grpid].end(),curusr)==grpmem[grpid].end()){
		ch="Group "+grpid+" doesnt include you";
		send(newsocketdes,ch.c_str(),strlen(ch.c_str()),0);
		return;
	}
	if(grpfileusrport[grpid].find(filename)==grpfileusrport[grpid].end()){
		ch="Group "+grpid+" doesnt contain the file";
		send(newsocketdes,ch.c_str(),strlen(ch.c_str()),0);
		return;
	}
	ch=filepath[filename]+" "+(*grpfileusrport[grpid][filename].begin()).second;
	send(newsocketdes,ch.c_str(),strlen(ch.c_str()),0);
}
void* driver(void* ar){
	int newsocketdes=* (int*)ar;
 	free(ar);	
	while(1){
	memset(buffer, 0, sizeof(buffer));
	int valread=read(newsocketdes,buffer,1024);		
	vector<string> v1=Parser((string)buffer);
	string curusr;	
	if(v1[0]=="create_user"){
		createuser(newsocketdes);	
	}
	if(v1[0]=="login"){
		//cout<<isloggedin<<" from driver"<<endl;
		login(newsocketdes);
		//cout<<isloggedin<<" from driver 1"<<endl;
	}
	if(v1[0]=="create_group"){
		creategrp(newsocketdes,curusr);	
	}	
	if(v1[0]=="join_group"){
		joingrp(newsocketdes);
	}
	if(v1[0]=="list_requests"){
		listreq(newsocketdes);	
	}
	if(v1[0]=="list_groups"){
		listgrps(newsocketdes);	
	}
	if(v1[0]=="accept_request"){
		acptrqst(newsocketdes);	
	}
	if(v1[0]=="leave_group"){
		lvgrp(newsocketdes);	
	}
	if(v1[0]=="upload_file"){
		upldfl(newsocketdes);
	}
	if(v1[0]=="list_files"){
		lstfiles(newsocketdes);
	}
	if(v1[0]=="download_file"){
		download(newsocketdes);	
	}
	if(v1[0]=="logout"){
		usrstatus[{v1[1],v1[2]}]=false;	
		string ch="Logged out successfully";
		send(newsocketdes,ch.c_str(),strlen(ch.c_str()),0);
	}
	}
}
int main(){
	TRIP="127.0.0.1";
	TRPort="3500";	
	int socketdes,newsocketdes,val;
 	socklen_t size;
 	struct sockaddr_in myaddr;
 	struct sockaddr_in otheraddr;
 	if((socketdes=socket(AF_INET,SOCK_STREAM,0))<0)
 	{
 		perror("failed to obtain the socket descriptor");
        return -1;
	}
  myaddr.sin_family=AF_INET;
  myaddr.sin_port=htons(stoi(TRPort));
  inet_pton(AF_INET,TRIP.c_str() , &myaddr.sin_addr); 
  myaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  bzero(&(myaddr.sin_zero),8);

  if(bind(socketdes,(struct sockaddr *)&myaddr,sizeof(struct sockaddr))==-1)
  {
  	perror("Error in binding ");
  	return -1;
  }
  if(listen(socketdes,1000)==-1)
  {
     perror("error backlog overflow");
     return -1;

  }
  size=sizeof(struct sockaddr);
  while((newsocketdes=accept(socketdes,(struct sockaddr *)&otheraddr,&size))!=-1)
  {
  	string ip=inet_ntoa(otheraddr.sin_addr);
  	int port=(ntohs(otheraddr.sin_port));
  	//cout<<"ip="<<ip<<"port"<<port<<endl;
	//cout<<"before reading in tracker";
	//cout<<"after reading in tracker";
	int *pc=(int*)malloc(sizeof(int));
 	*pc=newsocketdes;
	pthread_t newth;
  	pthread_create(&newth,NULL,&driver,pc);
  	size=sizeof(struct sockaddr);
  }
  return 0;
}
