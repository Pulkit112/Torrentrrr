#include <bits/stdc++.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
//#include <openssl/sha.h>
#include <arpa/inet.h>
#include <pthread.h>
using namespace std;
#define MAX 15000
pthread_t clients[60];
int current_num=0;



class USER{
public:
  string user_id,password,ip,port;
  bool logged_in;
  vector<string>Downloaded_File_Names;
};

class FILE_INFO{
  public:
  string File_location,file_name;
  USER owner;
  int size;
  bool is_sharable;

};

class GROUP{
public:
  vector<USER>users;
  string group_id;
  string admin;
  vector<USER>request;
  vector<FILE_INFO>Files;
};



vector<GROUP>Group_Detail;
vector<USER>User_Detail;

string process_command(string s, string &current_username)
{  
    vector <string> tokens;
    stringstream check1(s);  
    string intermediate;
    while(getline(check1, intermediate, ' '))
    {
        tokens.push_back(intermediate);
    }

    if (tokens[0] == "create_user")
    {
        if (tokens.size() != 3)
        {
            return "Please give correct number of credentials\n";
        }
        for(int i=0;i<User_Detail.size();i++)
        { 
            if(User_Detail[i].user_id == tokens[1])
            return "There is a person with same username. Plzz choose another username\n";
        }
        USER U;
        U.user_id=tokens[1];
        U.password=tokens[2];
        U.logged_in=0;
        U.ip="NIL";
        U.port="NIL";
        User_Detail.push_back(U);
        return "success\n";
    }
    if(tokens[0]=="login")
    {
         if (tokens.size() != 5)
        {
            return "Please give correct number of credentials\n";
        }
        for(int i=0;i<User_Detail.size();i++)
        {
           if(User_Detail[i].user_id == current_username)
            {
                return "Please logout from this id\n";
            }
        }
        for(int i=0;i<User_Detail.size();i++)
        { 
            if(User_Detail[i].user_id == tokens[1])
            {
                
                if(User_Detail[i].logged_in==1)
                {
                    return "User Already Loogeed In\n";
                }
                else if(User_Detail[i].password == tokens[2])
                {
                    User_Detail[i].logged_in=1;
                    User_Detail[i].ip=tokens[3];
                    User_Detail[i].port=tokens[4];
                    current_username=tokens[1];
                    return "logged in successfully\n";
                }
                else
                {
                    return "Password_Incorrect\n";
                }
            }
        }
        return "No person with this username \n";
    }
    if(tokens[0]=="create_group")
    {
      if (tokens.size() != 2)
        {
            return "Please give correct number of credentials\n";
        }  
       else
        {
        for(int i=0;i<User_Detail.size();i++)
        {   
            if(User_Detail[i].user_id==current_username)
            {
                if(User_Detail[i].logged_in==0)
                {
                     string s = "Please login to create group\n";
                     return s;
                }
                else
                {
                  for(int j=0;j<Group_Detail.size();j++)
                  { 
                      if(Group_Detail[i].group_id == tokens[1])
                      {
                          return "Group_Id Already present.Please give another Id\n";
                      }
                  }
                  GROUP G;
                  G.group_id=tokens[1];
                  G.users.push_back(User_Detail[i]);
                  G.admin = User_Detail[i].user_id;
                  Group_Detail.push_back(G);
                  string s= "GROUP:"+G.group_id+" Created successfully\n";
                  return s;
                }
            }      
        }
     return "Please register yourself first\n";     
     }
    }
    if(tokens[0]=="logout")
    {
     if(tokens.size()!=1)
     {
          return "Please give correct number of credentials";
     }
     for(int i=0;i<User_Detail.size();i++)
        {    
            //cout<<"U1:"<<User_Detail[i].user_id.size()<<" U2:"<<current_username.size()<<endl;
            if(User_Detail[i].user_id == current_username)
            {  
                if(User_Detail[i].logged_in==0)
                return "Already Logged Out";
                else{
                User_Detail[i].logged_in=0;
                string s = User_Detail[i].user_id + " Logged Out Successfully \n";
                current_username= "";
                return s;
                 }
            } 
        }
     return "Please register yourself first\n";  
    }
    if(tokens[0]=="join_group")
    {
        if(tokens.size()!=2)
        {
         return "Please give correct number of credentials\n";
        }
        for(int i=0;i<Group_Detail.size();i++)
        { 
            if(Group_Detail[i].group_id==tokens[1])
            {
             for(int j=0;j<Group_Detail[i].users.size();j++)
             {
                  if(Group_Detail[i].users[j].user_id==current_username && Group_Detail[i].users[j].logged_in==1)
                  return "Already Added\n";
                  if(Group_Detail[i].users[j].user_id==current_username && Group_Detail[i].users[j].logged_in==0)
                  return "Please Login to join Group\n";
             }    
             for(int j=0;j<User_Detail.size();j++)
             { 
                //cout<<"INSIDE FOR"<<endl;
                if(User_Detail[j].user_id==current_username)
                 {    
                    Group_Detail[i].request.push_back(User_Detail[j]);
                    string s= current_username+" requested admin to for join \n";
                    return s;
                 }
             }
             return "Please Register Yourself\n";
            }
        }
        return "No Such group Exists\n";
    }
     if(tokens[0]=="leave_group")
    {
        if(tokens.size()!=2)
        {
         return "Please give correct number of credentials\n";
        }
        for(int i=0;i<Group_Detail.size();i++)
        { 
            if(Group_Detail[i].group_id==tokens[1])
            {
             if(Group_Detail[i].admin == current_username)
             {
                 Group_Detail.erase(Group_Detail.begin()+i);
                 return "You were the Group Admin and now have deleted the group upon leaving\n";
             }
             for(int j=0;j<Group_Detail[i].users.size();j++)
             {
                  if(Group_Detail[i].users[j].user_id==current_username && Group_Detail[i].users[j].logged_in==1)
                  {
                    Group_Detail[i].users.erase(Group_Detail[i].users.begin()+j);
                    return "User removed from the Group\n";
                  }
                  if(Group_Detail[i].users[j].user_id==current_username && Group_Detail[i].users[j].logged_in==0)
                  return "Please Login to leave Group\n";
             }
             return "User not present in this group\n";
            }
        }
        return "No Such group Exists\n";
    }
    if(tokens[0]=="requests")
    {   
        if(tokens.size()!=3)
        {
         return "Please give correct number of credentials\n";
        }
        if(current_username=="")
        return "ERROR: You are not registered or not logged in\n";
        for(int i=0;i<Group_Detail.size();i++)
        { 
            if(Group_Detail[i].group_id==tokens[2])
            {
             if(Group_Detail[i].request.size()==0)
             {
                  return "No Requests Found\n";
             }
             else
             {
             string s="";
             for(int j=0;j<Group_Detail[i].request.size();j++)
             { 
              s+=Group_Detail[i].request[j].user_id +"\n";    
             }
             return s;   
             }
            }
        }
        return "No Such group Exists\n";
    }
    if(tokens[0]=="accept_request")
    {   
        if(tokens.size()!=3)
        {
         return "Please give correct number of credentials\n";
        }
        if(current_username=="")
        return "ERROR: You are not registered or not logged in\n";
        
        for(int i=0;i<Group_Detail.size();i++)
        { 
            if(Group_Detail[i].group_id==tokens[1])
            {
              if(Group_Detail[i].admin!=current_username)
              {
                 return "You are not the admin\n";
              }
             for(int j=0;j<Group_Detail[i].request.size();j++)
             { 
              if(Group_Detail[i].request[j].user_id==tokens[2])
              {
                Group_Detail[i].users.push_back(Group_Detail[i].request[j]);
                Group_Detail[i].request.erase(Group_Detail[i].request.begin()+j);
                return tokens[2] + " added to the group\n";
              }    
             }
             return "No requests found from this userid\n";
            }
        }
        return "No Such group Exists\n";
    }
    if(tokens[0]=="list_groups")
    {   
        if(tokens.size()!=1)
        {
         return "Please give correct number of credentials\n";
        }
        if(current_username=="")
        return "ERROR: You are not registered or not logged in\n";
        if(Group_Detail.size()==0)
        return "No Groups Available\n";
        string s="";
        for(int i=0;i<Group_Detail.size();i++)
        { 
            s+=Group_Detail[i].group_id+"\n";
        }
        return s;
    }
    if(tokens[0]=="upload_file")
    {   
        if(tokens.size()!=4)
        {
         return "Please give correct number of credentials\n";
        }
        if(current_username=="")
        return "ERROR: You are not registered or not logged in\n";
        if(Group_Detail.size()==0)
        return "No Groups Available\n";
        string s="";
        for(int i=0;i<Group_Detail.size();i++)
        { 
           if(Group_Detail[i].group_id==tokens[2])
           {
              for(int j=0;j<Group_Detail[i].users.size();j++)
              {    //cout<<current_username<<" "<<Group_Detail[i].users[j].user_id<<endl;
                   //cout<<current_username.size()<<" "<<Group_Detail[i].users[j].user_id.size()<<endl;
                   if(Group_Detail[i].users[j].user_id == current_username)
                   {
                       FILE_INFO F1;
                       F1.File_location = tokens[1];  
                       F1.owner=Group_Detail[i].users[j];
                       F1.is_sharable=1;
                       F1.size = stoi(tokens[3]);
                       int found;
                       found = tokens[1].find_last_of('/');
                       string y =tokens[1].substr(found+1);
                       F1.file_name=y;
                       Group_Detail[i].Files.push_back(F1);
                       return "File Added to the Group\n";
                   }
              }
              return "You are not in this group\n";  
           }
        }
        return "No such group Exists\n";
    }
    if(tokens[0]=="list_files")
    {
         if(tokens.size()!=2)
        {
         return "Please give correct number of credentials\n";
        }
        if(current_username=="")
        return "ERROR: You are not registered or not logged in\n";
       
        if(Group_Detail.size()==0)
        return "No Groups Available\n";
        string s="";
        for(int i=0;i<Group_Detail.size();i++)
        {  cout<<"He"<<endl; 
           if(Group_Detail[i].group_id == tokens[1])
           { cout<<"Hp"<<endl;
               for(int j=0;j<Group_Detail[i].Files.size();j++)
               { cout<<"BB"<<endl;
                   if(Group_Detail[i].Files[j].is_sharable)
                   s+=Group_Detail[i].Files[j].file_name+"\n";
               }
           }
        }
        if(s=="")
        return "No Sharable file present\n";
        else
        return s;  
    }
    if(tokens[0]=="download_file")
    {   
        if(tokens.size()!=4)
        {
         return "Please give correct number of credentials\n";
        }
        if(current_username=="")
        return "ERROR: You are not registered or not logged in\n";
        if(Group_Detail.size()==0)
        return "No Groups Available\n";
        string s="";
        for(int i=0;i<Group_Detail.size();i++)
        { 
           if(Group_Detail[i].group_id==tokens[1])
           {
              for(int j=0;j<Group_Detail[i].users.size();j++)
              {    if(Group_Detail[i].users[j].user_id == current_username)
                   {
                      for(int k=0;k<Group_Detail[i].Files.size();k++)
                       {
                           if(Group_Detail[i].Files[k].file_name==tokens[2] && Group_Detail[i].Files[k].is_sharable==1)
                           {
                             return "OK "+Group_Detail[i].Files[k].owner.ip+" "+Group_Detail[i].Files[k].owner.port+" "+tokens[2]+" "+Group_Detail[i].Files[k].File_location+" "+to_string(Group_Detail[i].Files[k].size)+"\n";
                           }
                           if(Group_Detail[i].Files[k].file_name==tokens[2] && Group_Detail[i].Files[k].is_sharable!=1)
                           {
                            return "Sorry this file is not sharable at this moment\n";
                           }
                       }
                       return "No files found having such Name\n";
                   }
              }
              return "You are not in this group\n";  
           }
        }
        return "No such group Exists\n";
    }
    if(tokens[0]=="stop_share")
    {
        if(tokens.size()!=3)
        {
         return "Please give correct number of credentials\n";
        }
        if(current_username=="")
        return "ERROR: You are not registered or not logged in\n";
        if(Group_Detail.size()==0)
        return "No Groups Available\n";
        for(int i=0;i<Group_Detail.size();i++)
        {  
           if(Group_Detail[i].group_id == tokens[1])
           {   
               for(int j=0;j<Group_Detail[i].Files.size();j++)
               { 
                   if(Group_Detail[i].Files[j].file_name == tokens[2])
                   {
                       if(Group_Detail[i].Files[j].owner.user_id==current_username)
                       {
                           Group_Detail[i].Files[j].is_sharable=0;
                       string s= Group_Detail[i].Files[j].file_name + " is made unsharable\n";
                       return s;
                       }
                       else
                       return "You are not the owner of this file\n";
                   }
               }
               return "no such file exist in this group\n";
           }
        }
        return "No such group is present\n";  
    }

    return "HELLO";
}


void* fun(void * x)
{
    int sockfd = *(int *)x;
    string current_username,buff1;
    char buff[255];
    //map<string, User> user_details
    current_username="";
    while(1)
    {
        read(sockfd,buff,sizeof(buff));
         buff1=buff;
        cout << "Fromo client: " << buff1 << endl;
        string ss = process_command(buff1,current_username);
       // cout<<current_username<<endl;
        bzero(buff,sizeof(buff));
        strcpy(buff,ss.c_str());
        write(sockfd, buff, sizeof(buff));
    }
    return NULL;
}

void * Exitfun(void * arg)
{ 
    string s;
    cin >> s;
    if (s == "quit")
    {
        exit(1);
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    string server_addr,server_port;
    ifstream infile(argv[1]);
    getline(infile, server_addr);
    getline(infile, server_port);

    cout<<server_addr<<endl;
    cout<<server_port<<endl;
    int sockfd, connfd;
    socklen_t len;
    struct sockaddr_in servaddr, cli;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1)
    {
        printf("socket creation failed...\n");
        exit(0);
    }
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(atoi(server_port.c_str()));

    if ((bind(sockfd, (sockaddr *)&servaddr, sizeof(servaddr))) != 0)
    {
        printf("socket bind failed...\n");
        exit(0);
    }
    if ((listen(sockfd, 5)) != 0)
    {
        printf("Listen failed...\n");
        exit(0);
    }
    else
        printf("Server listening..\n");
     
    pthread_t ForExit;
    pthread_create(&ForExit, NULL, Exitfun, NULL);
   
    while (true)
    {
        len = sizeof(cli);
        
        connfd = accept(sockfd, (sockaddr *)&cli, &len);
        if (connfd < 0)
        {
            printf("server acccept failed...\n");
            exit(0);
        }
        else
            printf("server acccept the client...\n");
        
        pthread_create(&clients[current_num++], NULL, fun, &connfd);
    }
    close(sockfd);
   
}

