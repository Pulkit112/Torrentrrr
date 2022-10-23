// C program for the Client Side
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include<bits/stdc++.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<netdb.h>
#include <arpa/inet.h>
#include <iostream>
#include <string.h>
#define SA struct sockaddr
#define BUF_SIZE 5120
// For threading, link with lpthread

using namespace std;
string client_addr,client_port;
int num_chuncks,last_chunk_size;
string loc;
void error(const char *s)
{
 perror(s);
 exit(1);
}

string process_command(string s)
{

    vector <string> tokens;
    stringstream check1(s);
    string intermediate;
    while(getline(check1, intermediate, ' '))
    {
        tokens.push_back(intermediate);
    }
   /* for(int i=0;i<tokens[0].size();i++)
    { 
      cout<<i<<":"<<tokens[0][i]<<endl;
    }*/
    if(tokens[0]=="create_user")
    { 
        return "1";
    }
    if(tokens[0]=="login")
    { 
        return "2";
    }
    if(tokens[0]=="create_group")
    { 
        return "3";
    }
    if(tokens[0]=="join_group")
    { 
        return "4";
    }
    if(tokens[0]=="leave_group")
    { 
        return "5";
    }

    if(tokens[0]=="requests")
    { 
        return "6";
    }
    if(tokens[0]=="accept_request")
    { 
        return "7";
    }
    if(tokens[0]=="list_groups")
    { 
        return "8";
    }  
    if(tokens[0]=="logout")
    { 
        return "12";
    }
    if(tokens[0]=="close")
    { 
        return "111";
    }
    if(tokens[0]=="upload_file")
    {
      char actualpath [255];
      string str;
      if(realpath(tokens[1].c_str(), actualpath)!=NULL)
      {str=realpath(tokens[1].c_str(), actualpath);
      ifstream file(str, ios::binary | ios::ate);
      int val = file.tellg();
      return("77"+tokens[0]+" "+str+" "+tokens[2]+" "+ to_string(val));}
      else
      return "100";
    }
    if(tokens[0]=="list_files")
    { 
        return "9";
    }
    if(tokens[0]=="download_file")
    {    
        if(tokens.size()!=4)
        return "100";
        else
        {
            char actualpath [255];
            string str;
            if(realpath(tokens[3].c_str(), actualpath)!=NULL)
            {
            str=realpath(tokens[3].c_str(), actualpath);
            loc=str+"/"+tokens[2];
            string ans = "78"+tokens[0]+" "+tokens[1]+" "+tokens[2]+" "+str;
            return(ans);
            }
            return "100";
        }
    }
    if(tokens[0]=="stop_share")
    { 
        return "11";
    }

    return "100";
}

bool Download_fun(vector<string>tokens)
{
   string c_ip = tokens[1];
   string c_pno = tokens[2];
   string file_name = tokens[3];
   string file_location = tokens[4];
   string file_size = tokens[5];
   num_chuncks= ceil(((float)stoi(file_size)/(float)BUF_SIZE));
   last_chunk_size = stoi(file_size)-(num_chuncks-1)*BUF_SIZE;

   cout<<"NUM OF CHUNKS ="<<num_chuncks<<endl;
   cout<<"LAST CHUNK SIZE ="<<last_chunk_size<<endl;
   
   struct sockaddr_in servaddr;
   int sockfd;
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   if (sockfd == -1)
   {
        printf("socket creation failed...\n");
        exit(0);
    }
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(c_pno.c_str()));
    if (inet_pton(AF_INET, c_ip.c_str(), &servaddr.sin_addr) <= 0)
    {
        cout << "Error in inet_pton for " <<client_addr<<endl;
        exit(-1);
    }
    if (connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) != 0)
    {
        printf("connection with the server failed...\n");
        exit(0);
    }
    int pos=0;
    char Buff[BUF_SIZE];
    ifstream sender_file(file_location, ifstream::binary );
    for(int i=1;i<num_chuncks;i++)
    { bzero(Buff, BUF_SIZE);
      sender_file.seekg(pos,ios::beg); 
      sender_file.read(Buff,BUF_SIZE);
      write(sockfd, Buff, BUF_SIZE);
      pos+=BUF_SIZE;
    }
    char Buff2[last_chunk_size];
    sender_file.seekg(pos,ios::beg); 
    sender_file.read(Buff2,last_chunk_size);
    write(sockfd, Buff2,last_chunk_size);
    bzero(Buff, BUF_SIZE);
    strcpy(Buff,"GG");
   // close(sockfd);
    return true;
}

void* func(void* x)
{
     int sockfd = *(int *)x;
    char buff1[255];
    string buff;
  //  map<string, file_details> files;
    while(1)
    {
        printf("Enter command : ");
        fgets(buff1, 255,stdin);
        buff1[strcspn(buff1, "\n")] = 0;
        buff=buff1;
        string val = process_command(buff);
        if (val == "1")
        {
             write(sockfd, buff1, sizeof(buff1));
             bzero(buff1, sizeof(buff1));
             read(sockfd, buff1, sizeof(buff1));
             buff=buff1;
             cout<<buff<<endl;
        }
        if (val == "2")
        {    buff=buff+" "+client_addr+" "+client_port;
             bzero(buff1,sizeof(buff1));
             strcpy(buff1,buff.c_str());
             write(sockfd, buff1, sizeof(buff1));
             bzero(buff1, sizeof(buff1));
             read(sockfd, buff1, sizeof(buff1));
             buff=buff1;
             cout<<buff<<endl;
        }
        if(val =="3")
        {
             write(sockfd, buff1, sizeof(buff1));
             bzero(buff1, sizeof(buff1));
             read(sockfd, buff1, sizeof(buff1));
             buff=buff1;
             cout<<buff<<endl; 
        }
        if(val =="4")
        {
             write(sockfd, buff1, sizeof(buff1));
             bzero(buff1, sizeof(buff1));
             read(sockfd, buff1, sizeof(buff1));
             buff=buff1;
             cout<<buff<<endl; 
        }
        if(val =="5")
        {
             write(sockfd, buff1, sizeof(buff1));
             bzero(buff1, sizeof(buff1));
             read(sockfd, buff1, sizeof(buff1));
             buff=buff1;
             cout<<buff<<endl; 
        }
        if(val =="6")
        {
             write(sockfd, buff1, sizeof(buff1));
             bzero(buff1, sizeof(buff1));
             read(sockfd, buff1, sizeof(buff1));
             buff=buff1;
             cout<<buff<<endl; 
        }
         if(val =="7")
        {
             write(sockfd, buff1, sizeof(buff1));
             bzero(buff1, sizeof(buff1));
             read(sockfd, buff1, sizeof(buff1));
             buff=buff1;
             cout<<buff<<endl; 
        }
        if(val == "8")
        {
             write(sockfd, buff1, sizeof(buff1));
             bzero(buff1, sizeof(buff1));
             read(sockfd, buff1, sizeof(buff1));
             buff=buff1;
             cout<<buff<<endl; 
        }
        if(val == "9")
        {
             write(sockfd, buff1, sizeof(buff1));
             bzero(buff1, sizeof(buff1));
             read(sockfd, buff1, sizeof(buff1));
             buff=buff1;
             cout<<buff<<endl; 
        }
         if(val[0] == '7' && val[1] == '8')
        {    val =val.substr(2);
             cout<<val<<endl;
             bzero(buff1, sizeof(buff1));
             strcpy(buff1, val.c_str());
             write(sockfd, buff1, sizeof(buff1));
             bzero(buff1, sizeof(buff1));
             read(sockfd, buff1, sizeof(buff1));
             string ret = buff1;
             vector <string> tokens;
            stringstream check1(ret);
            string intermediate;
            while(getline(check1, intermediate, ' '))
            {
            tokens.push_back(intermediate);
            } 
            if(tokens[0]=="OK")
            {
                 if(Download_fun(tokens))
                 cout<<"YES"<<endl;
                 else
                 cout<<"NO"<<endl;
            }
             bzero(buff1, sizeof(buff1));
        }
         if(val == "11")
        {
             write(sockfd, buff1, sizeof(buff1));
             bzero(buff1, sizeof(buff1));
             read(sockfd, buff1, sizeof(buff1));
             buff=buff1;
             cout<<buff<<endl; 
        }
        if(val == "12")
        {
             write(sockfd, buff1, sizeof(buff1));
             bzero(buff1, sizeof(buff1));
             read(sockfd, buff1, sizeof(buff1));
             buff=buff1;
             cout<<buff<<endl; 
        }
        
        if(val[0] == '7' && val[1]=='7' )
        {    val =val.substr(2);
             bzero(buff1, sizeof(buff1));
             strcpy(buff1, val.c_str());
             write(sockfd, buff1, sizeof(buff1));
             bzero(buff1, sizeof(buff1));
             read(sockfd, buff1, sizeof(buff1));
             buff=buff1;
             cout<<buff<<endl; 
        }
        if(val=="111")
        {
           break; 
        }
        if(val=="100")
        {
        cout<<"Please give correct command"<<endl; 
        }
    }
    return NULL;
}

void* client_as_server(void *pclient_s)
{
    int sockfd = *(int *)pclient_s;
    char buff[BUF_SIZE];
    int pos=0;
    cout<<loc<<endl;
    ofstream reciever_file(loc, ios_base::out | ios_base::binary | ios_base::trunc);
    while(1)
    {
     char buff[BUF_SIZE];
     bzero(buff, sizeof(buff));
     read(sockfd, buff, sizeof(buff));
     string bu =buff;
     int x = bu.size();
     if(bu=="GG")
     break;
     reciever_file.seekp(pos,ios::beg);
     reciever_file.write(buff,x);
     pos+=x ;
     cout<<pos<<" Bytes have been transffered"<<endl;
    }
    return NULL;
}

void clienthread(int portno)
{
  int sockfd,n;
  struct sockaddr_in serv_addr;
  char buff[255];
 
  sockfd = socket(AF_INET,SOCK_STREAM,0);
  if(sockfd <0)
    {
        error("ERROR OCCURED IN OPENING SOCKET");
    }
    
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno); 
    if(connect(sockfd, (struct sockaddr *)&serv_addr,sizeof(serv_addr))<0)
    error("Connection failed");
    else{
        printf("Connection Established\n");
    }
    int *pclient = (int *)malloc(sizeof(int));
    *pclient = sockfd;
     pthread_t t;
    pthread_create(&t, NULL, func, pclient);
//    pthread_join(t, NULL);

    int sockfd_s, connfd_s;
    socklen_t len;
    struct sockaddr_in servaddr, cli;
    sockfd_s = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd_s == -1)
    {
        printf("socket creation failed...\n");
        exit(0);
    }
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(atoi(client_port.c_str()));

    if ((bind(sockfd_s, (sockaddr *)&servaddr, sizeof(servaddr))) != 0)
    {
        printf("socket bind failed...\n");
        exit(0);
    }
    if ((listen(sockfd_s, 5)) != 0)
    {
        printf("Listen failed...\n");
        exit(0);
    }
    else
        printf("Server listening..\n");
     
    while (true)
    {   
        len = sizeof(cli);
        
        connfd_s = accept(sockfd_s, (sockaddr *)&cli, &len);
        if (connfd_s < 0)
        {
            printf("server acccept failed...\n");
            exit(0);
        }
        else
        printf("server acccept the client...\n");
        int *pclient_s = (int *)malloc(sizeof(int));
        *pclient_s = connfd_s;
        pthread_t t2;
        pthread_create(&t2, NULL, client_as_server, pclient_s);
        pthread_join(t2, NULL);

    }
pthread_join(t, NULL);
}


int main(int argc, char *argv[])
{
	
    string server_addr,server_port;
    ifstream infile(argv[3]);
    getline(infile, server_addr);
    getline(infile, server_port);
    int portno;
  	pthread_t tid;
     if(argc<4)
    {
       fprintf(stderr, "usage %s hostname port\n", argv[0]);
       exit(0); 
    }
    client_port=argv[2];
    client_addr=argv[1];
  portno = atoi(server_port.c_str());
  clienthread(portno);
  

   return 0;
}
