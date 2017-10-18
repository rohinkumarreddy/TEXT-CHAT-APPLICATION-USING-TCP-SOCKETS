#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<cstring>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#include"tcp_server_func.h"

#include "../include/global.h"
#include "../include/logger.h"

#define STDIN 0
#define BACKLOG 5
#define BUFFSIZE 30000

struct client_details
{
	char client_ip[INET_ADDRSTRLEN];
	char hostname[256];
	int  client_port;
	int  client_socket;
};

struct client_statistics
{
	char client_ip[INET_ADDRSTRLEN];
	char hostname[256];
	int  client_port;
	bool client_login;
	int  msgin;
	int  msgout;
	int  client_socket;
	char target_ip[5][INET_ADDRSTRLEN];
	int  num_targets;
	int  num_buffer;
	char str_buffer[30000];
};

struct client_blocked
{
	char client_ip[INET_ADDRSTRLEN];
	char hostname[256];
	int  client_port;
};


int itoa(int value,char *ptr);//reference from https://stackoverflow.com/questions/8257714/how-to-convert-an-int-to-string-in-c

int sendall(int s, char *buf, int *len)
{
    int total = 0;        // how many bytes we've sent
    int bytesleft = *len; // how many we have left to send
    int n;

    while(total < *len) {
        n = send(s, buf+total, bytesleft, 0);
        if (n == -1) { break; }
        total += n;
        bytesleft -= n;
    }

    *len = total; // return number actually sent here

    return n==-1?-1:0; // return -1 on failure, 0 on success
} 

using namespace std;

void tcp_server_func(int port)
{
	cout<<"PA1-SERVER is running....."<<endl;
	int head_socket=0;//the socket descriptor holding the max value (initialised with zero)	
	int sock_index=0, new_fd=0,client_num=0,client_num1=0;                                  /*cout<<"in the tcp server function.."<<endl;*/
   	int selRet=0;int sent_data=0;
	char udp_buffer[INET_ADDRSTRLEN],client_ip_buf[INET_ADDRSTRLEN];
	memset(client_ip_buf,'\0',INET_ADDRSTRLEN);
	memset(udp_buffer,'\0',INET_ADDRSTRLEN);
   	unsigned int caddr_len=0;
   	char your_ubit_name[]="rohinkum",buffer[30000],send_result[30000],command_str[256];
	memset(send_result,'\0',sizeof(send_result));memset(buffer,'\0',sizeof(buffer));
	int udp_socket=socket(AF_INET,SOCK_DGRAM,0);
        char temp_ip[]="8.8.8.8";
        struct sockaddr_in udp_addr;
        int udpAddr_len=sizeof(udp_addr);
        bzero(&udp_addr,sizeof(udp_addr));
        udp_addr.sin_family=AF_INET;
        inet_pton(AF_INET,temp_ip,&udp_addr.sin_addr);
        udp_addr.sin_port=htons(53);
        int ret=connect(udp_socket,(struct sockaddr *)&udp_addr,sizeof(udp_addr));
        if(ret<0)
        {
		cerr<<"[udp Connect Failed!]"<<endl;
	}
        getsockname(udp_socket,(struct sockaddr *)&udp_addr,(socklen_t *)&udpAddr_len);
        inet_ntop(AF_INET,&(udp_addr.sin_addr),udp_buffer,INET_ADDRSTRLEN);

	struct client_details client_list[5];
	struct client_statistics client_stats[5];
	struct client_blocked temp_blocked[5];
	for(int b=0;b<5;b++){temp_blocked[b].client_port=999999999;}
	sock_index=0;
   	fd_set masterfds, readfds;//the master set of socket descriptors (connected) and the read set (temporary set for read)
   	FD_ZERO(&masterfds);//initialising master set with zero elements
   	FD_ZERO(&readfds);//initialising read set with zero elements
   	struct sockaddr_in server_addr,client_addr;
   	bzero(&server_addr,sizeof(server_addr));//initialising server_addr
   	int listening_socket=socket(AF_INET,SOCK_STREAM,0);//creating a listening socket for server
   	if(listening_socket<0)
   	{
    		cerr<<"Cannot create socket"<<endl;//error handling for listening socket
   	}
   	server_addr.sin_family=AF_INET;//AF_INET or PF_INET represent the family of internet sockets
   	server_addr.sin_addr.s_addr=htonl(INADDR_ANY);//type casts any available address from host to network (long) and assigns to s_addr
   	server_addr.sin_port=htons(port);//type casts port from host to network (short) and assigns to sin_port
   	if(bind(listening_socket,(struct sockaddr*)&server_addr,sizeof(server_addr))<0)
   	{
    		cerr<<"Bind failed"<<endl;
   	}
   	if(listen(listening_socket, BACKLOG)<0)
   	{
   		cerr<<"Unable to listen on port: "<<port<<endl;
   	}                                                /*else{cout<<"listening success!"<<endl;}*/
   	FD_SET(STDIN, &masterfds);//setting STDIN in masterfds socket set
   	FD_SET(listening_socket, &masterfds);//setting the listening socket in masterfds socket set
   	head_socket=listening_socket;//For now the head socket is updated with listening socket
   	for(;;)//main loop
   	{
    		readfds=masterfds;//making sure readfds is updated with masterfds contents before select modifies it
    		selRet=select(head_socket+1, &readfds, NULL, NULL, NULL );//&tvcan be the last argument for time out
    		if(selRet<0)
    		{
     			cerr<<"select failed"<<endl;
    		}
    		else if(selRet>0)
    		{
                                                   /*cout<<"select success!"<<endl;*/
     			for(sock_index=0;sock_index<=head_socket;sock_index+=1)
     			{
      				if(FD_ISSET(sock_index, &readfds))//checking for sock_index in readfds
      				{                                           /*cout<<"read loop success"<<endl;*/
       					if(sock_index==STDIN)//shell input
       					{                                          /*cout<<"entered STDIN..."<<endl;*/
        					char cmd[30000],*tokRet=NULL,str_arr[256][30000];//cmd is for shell input, str_arr[][] is for separated tokens
						int num_tokens=0;
        					memset(cmd,'\0',sizeof(cmd));//initialising cmd
        					int index=0;
        					cin.getline(cmd,30000);
        					tokRet = strtok(cmd," ");
        					while(tokRet)
        					{                                         /*cout<<"entered the while..."<<endl;*/
         						strcpy(str_arr[index], tokRet);          /*cout<<"strcpy successful"<<endl;*/
         						index+=1;num_tokens+=1;
         						tokRet=strtok(NULL," ");
        					}//end of while(arg)
                                                  /*cout<<"EXITED while(arg)..."<<endl;*/
        					if(strcmp(str_arr[0],"AUTHOR")==0)
        					{
         						//cout<<"["<<str_arr[0]<<":SUCCESS]\n"<<"I,"<<ubit_name<<", have read and understood the course academic integrity policy.\n"<<"["<<str_arr[0]<<":END]"<<endl;
							strcpy(command_str,str_arr[0]);
							cse4589_print_and_log("[%s:SUCCESS]\n",command_str);
							cse4589_print_and_log("I, %s, have read and understood the course academic integrity policy.\n",your_ubit_name);
							cse4589_print_and_log("[%s:END]\n",command_str);
        					}//end of AUTHOR
        					else if(strcmp(str_arr[0],"PORT")==0)
        					{
							strcpy(command_str,str_arr[0]);
         						//cout<<"["<<str_arr[0]<<":SUCCESS]\n"<<str_arr[0]<<":"<<port<<"\n["<<str_arr[0]<<":END]"<<endl;
							cse4589_print_and_log("[%s:SUCCESS]\n",command_str);
							cse4589_print_and_log("PORT:%d\n",port);
							cse4589_print_and_log("[%s:END]\n",command_str);
        					}//end of PORT
        					else if(strcmp(str_arr[0],"IP")==0)
        					{
							strcpy(command_str,"IP");char ip_addr[INET_ADDRSTRLEN];strcpy(ip_addr,udp_buffer);
         						//cout<<"["<<str_arr[0]<<":SUCCESS]"<<endl;
         						cse4589_print_and_log("[%s:SUCCESS]\n",command_str);
         						//cout<<str_arr[0]<<":"<<udp_buffer<<endl;
							cse4589_print_and_log("IP:%s\n",ip_addr);
         						//cout<<"["<<str_arr[0]<<":END]"<<endl;
							cse4589_print_and_log("[%s:END]\n",command_str);
        					}//end of IP
        					else if(strcmp(str_arr[0],"LIST")==0)
        					{
							strcpy(command_str,"LIST");							
         						int list_id=0;char hostname[256];char ip_addr[INET_ADDRSTRLEN];int port_num=0;
          							//cout<<"["<<str_arr[0]<<":SUCCESS]"<<endl;
								cse4589_print_and_log("[%s:SUCCESS]\n",command_str);
								for(int i=0;i<client_num;i++)
								{
									list_id=i+1;strcpy(hostname,client_list[i].hostname);
									strcpy(ip_addr,client_list[i].client_ip);
									port_num=client_list[i].client_port;
									cse4589_print_and_log("%-5d%-35s%-20s%-8d\n", list_id, hostname, ip_addr, port_num);
								}
          							//cout<<"["<<str_arr[0]<<":END]"<<endl;
								cse4589_print_and_log("[%s:END]\n",command_str);
        					}//end of LIST
						else if(strcmp(str_arr[0],"STATISTICS")==0)
        					{
							if(num_tokens==1)
							{
								strcpy(command_str,"STATISTICS");	
								char status[256];int list_id=0;char hostname[256];
								char ip_addr[INET_ADDRSTRLEN];int port_num=0;
								int num_msg_sent=0, num_msg_rcv=0;
         							cse4589_print_and_log("[%s:SUCCESS]\n",command_str);
        							for(int i=0;i<client_num1;i++)
								{
									list_id=i+1;strcpy(hostname,client_list[i].hostname);
									strcpy(ip_addr,client_list[i].client_ip);
									port_num=client_list[i].client_port;
									num_msg_sent=client_stats[i].msgout;num_msg_rcv=client_stats[i].msgin;
									if(client_stats[i].client_login==true){strcpy(status,"logged-in");}
									else if(client_stats[i].client_login==false){strcpy(status,"logged-out");}
									cse4589_print_and_log("%-5d%-35s%-8d%-8d%-8s\n", list_id, hostname, num_msg_sent, num_msg_rcv, status);
								} 
        							cse4589_print_and_log("[%s:END]\n",command_str);
							}
							else
							{
								cse4589_print_and_log("[%s:ERROR]\n",command_str);
								cout<<"[ERROR:INVALID ARGUMENTS]"<<endl;
								cse4589_print_and_log("[%s:END]\n",command_str);
							}
        					}//end of STATISTICS
						else if(strcmp(str_arr[0],"BLOCKED")==0)
        					{
							strcpy(command_str,"BLOCKED");
							if(num_tokens==2)
							{							
         							if((strcmp(str_arr[1],"128.205.36.46")==0)||(strcmp(str_arr[1],"128.205.36.35")==0)||(strcmp(str_arr[1],"128.205.36.33")==0)||(strcmp(str_arr[1],"128.205.36.34")==0)||(strcmp(str_arr[1],"128.205.36.36")==0)||(strcmp(str_arr[1],udp_buffer)==0))
								{
									cse4589_print_and_log("[%s:SUCCESS]\n",command_str);
									int list_id=0,port_num=0;char hostname[256],ip_addr[INET_ADDRSTRLEN];
									for(int i=0;i<client_num1;i++)
									{
										if(strcmp(str_arr[1],client_stats[i].client_ip)==0)//accessing requested client's stats
										{
											for(int g=0;g<4;g++)
											{	
												if(strcmp(client_stats[i].target_ip[g],"0.0.0.0")==0)
												{
													temp_blocked[g].client_port=999999999;
													//continue;
												}
												else
												{
													for(int j=0;j<client_num1;j++)
													{
														if(strcmp(client_stats[i].target_ip[g],client_stats[j].client_ip)==0)//accessing target_ip[g] stats
														{
															strcpy(temp_blocked[g].hostname,client_stats[j].hostname);
															strcpy(temp_blocked[g].client_ip,client_stats[j].client_ip);
															temp_blocked[g].client_port=client_stats[j].client_port;
														} 
													}
												}
											}
											for(int k=0;k<5;k++)//sorting target list
											{
												for(int h=0;h<4;h++)
												{
													if(temp_blocked[h].client_port>temp_blocked[h+1].client_port)
													{
														temp_blocked[4].client_port=temp_blocked[h].client_port;
														temp_blocked[h].client_port=temp_blocked[h+1].client_port;
														temp_blocked[h+1].client_port=temp_blocked[4].client_port;
														strcpy(temp_blocked[4].client_ip,temp_blocked[h].client_ip);
														strcpy(temp_blocked[h].client_ip,temp_blocked[h+1].client_ip);
														strcpy(temp_blocked[h+1].client_ip,temp_blocked[4].client_ip);
														strcpy(temp_blocked[4].hostname,temp_blocked[h].hostname);
														strcpy(temp_blocked[h].hostname,temp_blocked[h+1].hostname);
														strcpy(temp_blocked[h+1].hostname,temp_blocked[4].hostname);
													}
												}
											}//end of port sort
											for(int m=0;m<client_stats[i].num_targets;m++)
											{
												list_id=m+1;strcpy(hostname,temp_blocked[m].hostname);
												strcpy(ip_addr,temp_blocked[m].client_ip);
												port_num=temp_blocked[m].client_port;
												cse4589_print_and_log("%-5d%-35s%-20s%-8d\n", list_id, hostname, ip_addr, port_num);
											}
										}//end of accessing requested client stats
									}
									cse4589_print_and_log("[%s:END]\n",command_str);
								}//end of valid IP address check
								else
								{
									cse4589_print_and_log("[%s:ERROR]\n",command_str);
									cout<<"[ERROR:INVALID IP]"<<endl;
									cse4589_print_and_log("[%s:END]\n",command_str);
								}
							}//end of num_tokens==1
							else
							{
								cse4589_print_and_log("[%s:ERROR]\n",command_str);
								cout<<"[ERROR:INVALID ARGUMENTS]"<<endl;
								cse4589_print_and_log("[%s:END]\n",command_str);
							}
        					}//end of BLOCKED
        					else
        					{
         						//cout<<"["<<str_arr[0]<<":ERROR]\n"<<"["<<str_arr[0]<<":END]"<<endl;
							strcpy(command_str,str_arr[0]);
							cse4589_print_and_log("[%s:ERROR]\n",command_str);
							cse4589_print_and_log("[%s:END]\n",command_str);
        					}//end of COMMANDS
       					}//end of STDIN
       					else if(sock_index==listening_socket)//Handling new connect requests
       					{
        					caddr_len=sizeof(client_addr);
        					if((new_fd=accept(listening_socket,(struct sockaddr *)&client_addr,&caddr_len))<0)
        					{
         						cerr<<"Accept failed"<<endl;
        					}
        					else//when accept succeeds
        					{

							client_num+=1;
							cout<<"Remote client connected!"<<endl;
						        FD_SET(new_fd,&masterfds);
         						if(new_fd>head_socket)
         						{
          							head_socket=new_fd;
         						}
        					}//end of accept
       					}//end of connect request handler
       					else
       					{
        					memset(buffer,'\0',sizeof(buffer));//initialising buffer
        					if(recv(sock_index,buffer,BUFFSIZE,0)<=0)
        					{
         						close(sock_index);
         						cout<<"Remote Host terminated connection!"<<endl;
         						FD_CLR(sock_index,&masterfds);
							client_num-=1;//Error here is that list is not updated
        					}
       	 					else//recv success
        					{
							char *tokRet1=NULL,str_arr1[1000][30000];
 							int index1=0,num_tokens1=0;
 							tokRet1 = strtok(buffer,"#");
 							while(tokRet1)
							{
 								strcpy(str_arr1[index1], tokRet1);          
 								index1+=1;num_tokens1+=1;
 								tokRet1=strtok(NULL,"#");
							}
								//cout<<"The client sent me the following message\n"<<str_arr1[0]<<"\n"<<str_arr1[1]<<"\n"<<str_arr1[2]<<"\n"<<str_arr1[3]<<endl;
							if(strcmp(str_arr1[0],"LOGOUT")==0)
							{
								for(int g=0;g<client_num;g++)
								{
									if(client_list[g].client_port==atoi(str_arr1[1]))
									{
										client_list[g].client_port=999999999;
									}
								}
								for(int k=0;k<client_num;k++)//sorting client list
								{
									for(int j=0;j<client_num-1;j++)
									{
										if(client_list[j].client_port>client_list[j+1].client_port)
										{
											client_list[4].client_port=client_list[j].client_port;
											client_list[j].client_port=client_list[j+1].client_port;
											client_list[j+1].client_port=client_list[4].client_port;

											strcpy(client_list[4].client_ip,client_list[j].client_ip);
											strcpy(client_list[j].client_ip,client_list[j+1].client_ip);
											strcpy(client_list[j+1].client_ip,client_list[4].client_ip);
											strcpy(client_list[4].hostname,client_list[j].hostname);
											strcpy(client_list[j].hostname,client_list[j+1].hostname);
											strcpy(client_list[j+1].hostname,client_list[4].hostname);										client_list[4].client_socket=client_list[j].client_socket;
											client_list[j].client_socket=client_list[j+1].client_socket;
											client_list[j+1].client_socket=client_list[4].client_socket;

										}//end of if(clientj>clientj+1)
									}
								}//end of port sorting
								for(int h=0;h<client_num1;h++)
								{
									if(client_stats[h].client_port==atoi(str_arr1[1]))
									{
										client_stats[h].client_login=false;
									}
								}		
							}//end of LOGOUT#
							else if(strcmp(str_arr1[0],"REFRESH")==0)//REFRESH#
							{
								char str_temp[256];memset(str_temp,'\0',sizeof(str_temp));int temp_port=0;
								strcpy(send_result,"REFLIST#");
								for(int m=0;m<client_num;m++)
								{
									strcat(send_result,client_list[m].hostname);
									strcat(send_result,"#");
									strcat(send_result,client_list[m].client_ip);
									strcat(send_result,"#");
									temp_port=client_list[m].client_port;
									itoa(temp_port,str_temp);
									strcat(send_result,str_temp);
									strcat(send_result,"#");
								}
								strcat(send_result,"END");
								sent_data=send(sock_index,send_result,strlen(send_result),0);
								if(sent_data<0)
								{
									cerr<<"[ERROR:Send LIST to client Failed]"<<endl;
								}	
							}//end of REFRESH#
							else if(strcmp(str_arr1[0],"LIST")==0)//receiving client details
							{
								int stat_choice=0;client_num1+=1;
								char str_temp[256];memset(str_temp,'\0',sizeof(str_temp));int temp_port=0;
								strcpy(client_list[client_num-1].hostname,str_arr1[1]);//storing hostname
								strcpy(client_list[client_num-1].client_ip,str_arr1[2]);//storing client_IP
								client_list[client_num-1].client_port=atoi(str_arr1[3]);//storing client_port
								client_list[client_num-1].client_socket=sock_index;//storing client_socket
								for(int e=0;e<client_num1;e++)//existing client_stat
								{
									if((strcmp(client_stats[e].hostname,str_arr1[1])==0)&&(client_stats[e].client_port==client_list[client_num-1].client_port))//existing client re-login
									{
										client_stats[e].client_socket=sock_index;
										client_stats[e].client_login=true;
										stat_choice=1;client_num1-=1;//resetting client_num1
									}
								}
								if(stat_choice==0)//new client_stat
								{
									strcpy(client_stats[client_num1-1].client_ip,str_arr1[2]);//copying ip to stats
									client_stats[client_num1-1].client_port=atoi(str_arr1[3]);//copying port to stats
									strcpy(client_stats[client_num1-1].hostname,str_arr1[1]);//copying hostname to stats
									client_stats[client_num1-1].client_login=true;//setting login true
									client_stats[client_num1-1].msgin=0;//num of msgs recv
									client_stats[client_num1-1].msgout=0;//num msgs sent
									client_stats[client_num1-1].client_socket=sock_index;//copying socket
									client_stats[client_num1-1].num_targets=0;//initialising num_targets
									client_stats[client_num1-1].num_buffer=0;//initialising num_buffer
									strcpy(client_stats[client_num1-1].str_buffer,"START#");//concat START#
									for(int m=0;m<5;m++)//here every client_stats[i] has 5 target_ip's
									{
										strcpy(client_stats[client_num1-1].target_ip[m],"0.0.0.0");
									}
								}stat_choice=0;//resetting stat_choice
								for(int k=0;k<client_num;k++)//sorting client_list
								{
									for(int j=0;j<client_num-1;j++)
									{
										if(client_list[j].client_port>client_list[j+1].client_port)
										{
											client_list[4].client_port=client_list[j].client_port;//copying ports
											client_list[j].client_port=client_list[j+1].client_port;//replacing port
											client_list[j+1].client_port=client_list[4].client_port;//copying ports

											strcpy(client_list[4].client_ip,client_list[j].client_ip);//copying IP
											strcpy(client_list[j].client_ip,client_list[j+1].client_ip);//replacing IP
											strcpy(client_list[j+1].client_ip,client_list[4].client_ip);//copying IP

											strcpy(client_list[4].hostname,client_list[j].hostname);//copying hostname
											strcpy(client_list[j].hostname,client_list[j+1].hostname);//replacing hostname
											strcpy(client_list[j+1].hostname,client_list[4].hostname);//copying hostname
											client_list[4].client_socket=client_list[j].client_socket;//copying socket
											client_list[j].client_socket=client_list[j+1].client_socket;//replacing socket
											client_list[j+1].client_socket=client_list[4].client_socket;//copying socket

										}//end of if(clientj>clientj+1)
									}
								}//end of port sorting
								for(int s=0;s<client_num1;s++)//client_stats port sorting
								{
									for(int d=0;d<client_num1-1;d++)
									{
										if(client_stats[d].client_port>client_stats[d+1].client_port)
										{
											client_stats[4].client_port=client_stats[d].client_port;//copying ports
											client_stats[d].client_port=client_stats[d+1].client_port;//replacing port
											client_stats[d+1].client_port=client_stats[4].client_port;//copying ports

											strcpy(client_stats[4].client_ip,client_stats[d].client_ip);//copying IP
											strcpy(client_stats[d].client_ip,client_stats[d+1].client_ip);//replacing IP
											strcpy(client_stats[d+1].client_ip,client_stats[4].client_ip);//copying IP

											strcpy(client_stats[4].hostname,client_stats[d].hostname);//copying hostname
											strcpy(client_stats[d].hostname,client_stats[d+1].hostname);//replacing hostname
											strcpy(client_stats[d+1].hostname,client_stats[4].hostname);//copying hostname
											client_stats[4].msgin=client_stats[d].msgin;//copying num of recv msgs
											client_stats[d].msgin=client_stats[d+1].msgin;//replacing num of recv msgs
											client_stats[d+1].msgin=client_stats[4].msgin;//copying num of recv msgs
											client_stats[4].msgout=client_stats[d].msgout;//copying num of sent msgs
											client_stats[d].msgout=client_stats[d+1].msgout;//replacing num of sent msgs
											client_stats[d+1].msgout=client_stats[4].msgout;//copying num of sent msgs
											client_stats[4].client_socket=client_stats[d].client_socket;//copying client_socket
											client_stats[d].client_socket=client_stats[d+1].client_socket;//replacing client_socket
											client_stats[d+1].client_socket=client_stats[4].client_socket;//copying client_socket
											for(int o=0;o<5;o++)
											{
												strcpy(client_stats[4].target_ip[o],client_stats[d].target_ip[o]);//copying target_IP
												strcpy(client_stats[d].target_ip[o],client_stats[d+1].target_ip[o]);//replacing target_IP
												strcpy(client_stats[d+1].target_ip[o],client_stats[4].target_ip[o]);//copying target_IP
											}
											client_stats[4].num_targets=client_stats[d].num_targets;//copying num_targets
											client_stats[d].num_targets=client_stats[d+1].num_targets;//replacing num_targets
											client_stats[d+1].num_targets=client_stats[4].num_targets;//copying num_targets							
											client_stats[4].num_buffer=client_stats[d].num_buffer;//copying num_buffer
											client_stats[d].num_buffer=client_stats[d+1].num_buffer;//replacing num_buffer
											client_stats[d+1].num_buffer=client_stats[4].num_buffer;//copying num_buffer
											strcpy(client_stats[4].str_buffer,client_stats[d].str_buffer);//copying str_buffer
											strcpy(client_stats[d].str_buffer,client_stats[d+1].str_buffer);//replacing str_buffer
											strcpy(client_stats[d+1].str_buffer,client_stats[4].str_buffer);//copying str_buffer
										}//end of if(clientd>clientd+1)
									}
								}//end of port sorting
								//Implement send LIST to client
								strcpy(send_result,"LIST#");//sending list to clients
								for(int m=0;m<client_num;m++)
								{
									strcat(send_result,client_list[m].hostname);
									strcat(send_result,"#");
									strcat(send_result,client_list[m].client_ip);
									strcat(send_result,"#");
									temp_port=client_list[m].client_port;
									itoa(temp_port,str_temp);
									strcat(send_result,str_temp);
									strcat(send_result,"#");
								}
								strcat(send_result,"END");
								sent_data=send(sock_index,send_result,strlen(send_result),0);
								if(sent_data<0)
								{
									cerr<<"[ERROR:Send LIST to Client Failed]"<<endl;
								}
								
							}//end of LIST#
							else if(strcmp(str_arr1[0],"MSG")==0)//receiving MSG
							{
								strcpy(command_str,"RELAYED");
								int block_choice=0,log_out=0;
								char buffer22[30000],buffer23[30000];
								memset(buffer22,'\0',sizeof(buffer22));
								memset(buffer23,'\0',sizeof(buffer23));
								char from_client_ip[INET_ADDRSTRLEN],to_client_ip[INET_ADDRSTRLEN],msg[30000];
								memset(send_result,'\0',sizeof(send_result));
								for(int q=0;q<client_num1;q++)//searching for destination client
								{
									if(strcmp(str_arr1[2],client_stats[q].client_ip)==0)//finding destClient
									{
										for(int p=0;p<5;p++)
										{
											if(strcmp(str_arr1[1],client_stats[q].target_ip[p])==0)
											{
												block_choice=1;//dest is blocked
											}
										}
									}
								}
								if(block_choice==0)//dest not blocked
								{
						
									//cout<<num_tokens1<<endl;
									if(num_tokens1==4)
									{	int num_tokens2=0;
										strcat(buffer23,str_arr1[3]);
										while(num_tokens2!=2)
										{
											num_tokens2=0;
											if(recv(sock_index,buffer22,sizeof(buffer22),0)<=0)
        										{
         											cout<<"ERROR:continue failed"<<endl;
        										}
											char *tokRet3=NULL,str_arr55[10][30000];
 											int index3=0;
 											tokRet3 = strtok(buffer22,"#");
 											while(tokRet3)
											{
 												strcpy(str_arr55[index3], tokRet3);          
 												index3+=1;num_tokens2+=1;
 												tokRet3=strtok(NULL,"#");
											}//tokenize
											strcat(buffer23,str_arr55[0]);
											
										}
										//cout<<buffer23<<endl;	
										
										for(int count=0;count<client_num1;count++)
										{
											if(strcmp(str_arr1[2],client_stats[count].client_ip)==0)//comparing dest IP with existing list IP's
											{	
												strcpy(from_client_ip,str_arr1[1]);
												strcpy(to_client_ip,str_arr1[2]);
												strcpy(msg,buffer23);

												strcpy(send_result,"MSG#");
												strcat(send_result,str_arr1[1]);//source IP
												strcat(send_result,"#");
												strcat(send_result,buffer23);//copying message
												strcat(send_result,"#END");
												for(int i=0;i<client_num1;i++)
												{
													if(strcmp(str_arr1[2],client_stats[i].client_ip)==0)//picking out the dest client_stats
													{
														if(client_stats[i].client_login==false)
														{
															log_out=1;//dest logged out
															client_stats[i].num_buffer+=1;//incrementing num_buffer
															strcat(client_stats[i].str_buffer,str_arr1[1]);//source IP
															strcat(client_stats[i].str_buffer,"#");
															strcat(client_stats[i].str_buffer,buffer23);//concat msg
															strcat(client_stats[i].str_buffer,"#");
														}
													}
												}
												if(log_out==1)
												{
													cout<<"[EVENT:MSG BUFFERED]"<<endl;
												}
												else
												{
													int str_len=strlen(send_result);
													if(sendall(client_stats[count].client_socket,send_result,&str_len)==-1)
													//if(send(client_stats[count].client_socket,send_result,strlen(send_result),0)<0)
													{
														cerr<<"[ERROR:Send MSG Failed]"<<endl;
													}
													else
													{
													cse4589_print_and_log("[%s:SUCCESS]\n",command_str);
													cse4589_print_and_log("msg from:%s, to:%s\n[msg]:%s\n",from_client_ip,to_client_ip,msg);
													cse4589_print_and_log("[%s:END]\n",command_str);
												}
											}
											for(int g=0;g<client_num1;g++)
											{							
												if(strcmp(client_stats[g].client_ip,str_arr1[1])==0)//source IP
												{
													client_stats[g].msgout+=1;//	incrementing msgout
												}
												if(strcmp(client_stats[g].client_ip,str_arr1[2])==0)//destination IP
												{
													if(client_stats[g].client_login==true)
													{
														client_stats[g].msgin+=1;//incrementing msgin
													}
												}//end of destination IP
											}
										}//end of existing clients in list
									}



									}
									else
									{
									for(int count=0;count<client_num1;count++)
									{
										if(strcmp(str_arr1[2],client_stats[count].client_ip)==0)//comparing dest IP with existing list IP's
										{	
											strcpy(from_client_ip,str_arr1[1]);
											strcpy(to_client_ip,str_arr1[2]);
											strcpy(msg,str_arr1[3]);

											strcpy(send_result,"MSG#");
											strcat(send_result,str_arr1[1]);//source IP
											strcat(send_result,"#");
											strcat(send_result,str_arr1[3]);//copying message
											strcat(send_result,"#END");
											for(int i=0;i<client_num1;i++)
											{
												if(strcmp(str_arr1[2],client_stats[i].client_ip)==0)//picking out the dest client_stats
												{
													if(client_stats[i].client_login==false)
													{
														log_out=1;//dest logged out
														client_stats[i].num_buffer+=1;//incrementing num_buffer
														strcat(client_stats[i].str_buffer,str_arr1[1]);//concat msg
														strcat(client_stats[i].str_buffer,"#");
														strcat(client_stats[i].str_buffer,str_arr1[3]);//concat msg
														strcat(client_stats[i].str_buffer,"#");
													}
												}
											}
											if(log_out==1)
											{
												cout<<"[EVENT:MSG BUFFERED]"<<endl;
											}
											else
											{
												int str_len=strlen(send_result);
												if(sendall(client_stats[count].client_socket,send_result,&str_len)==-1)
												//if(send(client_stats[count].client_socket,send_result,strlen(send_result),0)<0)
												{
													cerr<<"[ERROR:Send MSG Failed]"<<endl;
												}
												else
												{
													cse4589_print_and_log("[%s:SUCCESS]\n",command_str);
													cse4589_print_and_log("msg from:%s, to:%s\n[msg]:%s\n",from_client_ip,to_client_ip,msg);
													cse4589_print_and_log("[%s:END]\n",command_str);
												}
											}
											for(int g=0;g<client_num1;g++)
											{							
												if(strcmp(client_stats[g].client_ip,str_arr1[1])==0)//source IP
												{
													client_stats[g].msgout+=1;//	incrementing msgout
												}
												if(strcmp(client_stats[g].client_ip,str_arr1[2])==0)//destination IP
												{
													if(client_stats[g].client_login==true)
													{
														client_stats[g].msgin+=1;//incrementing msgin
													}
												}//end of destination IP
											}
										}//end of existing clients in list
									}
									}
								}//end of block_choice==0
								else if(block_choice==1)
								{
									cout<<"client_blocked"<<endl;
								}
							}//end of MSG#
							else if(strcmp(str_arr1[0],"BROAD")==0)
							{
								strcpy(command_str,"RELAYED");
								//cout<<"Client sent a message"<<"\n"<<str_arr1[1]<<"\n"<<str_arr1[2]<<"\n"<<str_arr1[3]<<endl;
								char from_client_ip[INET_ADDRSTRLEN],to_client_ip[INET_ADDRSTRLEN],msg[30000];					
								memset(send_result,'\0',sizeof(send_result));
								int block_choice1=0,str_len=0;
								memset(send_result,'\0',sizeof(send_result));
								strcpy(from_client_ip,str_arr1[1]);//copying src IP
								strcpy(to_client_ip,"255.255.255.255");
								strcpy(msg,str_arr1[2]);//copying msg
								for(int count=0;count<client_num1;count++)//sending MSG to all clients (in the list) except src
								{
									for(int p=0;p<5;p++)
									{
										if(strcmp(str_arr1[1],client_stats[count].target_ip[p])==0)//checking whether source is in destinations target_IP list
										{
											block_choice1=1;
										}
									}
									if(block_choice1==0)//dest client not blocked
									{
										if(client_stats[count].client_login==true)
										{
											client_stats[count].msgin+=1;//incrementing msgin
										}
										if(strcmp(client_stats[count].client_ip,str_arr1[1])==0)
										{
											client_stats[count].msgout+=1;//incrementing msgout
											client_stats[count].msgin-=1;//decrementing msgin
										}
										if(client_stats[count].client_login==false)//dest client is logged out
										{
											client_stats[count].num_buffer+=1;
											strcat(client_stats[count].str_buffer,str_arr1[1]);
											strcat(client_stats[count].str_buffer,"#");
											strcat(client_stats[count].str_buffer,str_arr1[2]);//concat msg
											strcat(client_stats[count].str_buffer,"#");
											cout<<"[EVENT:MSG BUFFERED]"<<endl;
										}
										else
										{
											if(strcmp(str_arr1[1],client_stats[count].client_ip)==0)
											{
												continue;//Avoids sending back to destination
											}
											strcpy(send_result,"MSG#");
											strcat(send_result,str_arr1[1]);//source IP
											strcat(send_result,"#");
											strcat(send_result,str_arr1[2]);//copying message
											strcat(send_result,"#END");
											str_len=strlen(send_result);
											if(sendall(client_stats[count].client_socket,send_result, &str_len) == -1)
											//if(send(client_stats[count].client_socket,send_result,strlen(send_result),0)<0)
											{
												cerr<<"[ERROR:Send MSG Failed]"<<endl;
											}
										}
									}
									else if(block_choice1==1)
									{
										cout<<"client blocked"<<endl;
									}block_choice1=0;
								}//end of sending msg to all clients
								cse4589_print_and_log("[%s:SUCCESS]\n",command_str);
								cse4589_print_and_log("msg from:%s, to:%s\n[msg]:%s\n",from_client_ip,to_client_ip,msg);
								cse4589_print_and_log("[%s:END]\n",command_str);
							}//end of BROAD#	
							else if(strcmp(str_arr1[0],"BLOCK")==0)
							{
								strcpy(command_str,"BLOCK");int a=0;
								for(int e=0;e<client_num1;e++)//source_ip,target_ip,end
								{
									if(strcmp(str_arr1[1],client_stats[e].client_ip)==0)//searching for source_IP
									{
										client_stats[e].num_targets+=1;//incrementing num_targets
										for(int g=0;g<5;g++)//searching for existing targets
										{
											if(strcmp(str_arr1[2],client_stats[e].target_ip[g])==0)//checking for existing target blocked clients
											{	
												a=1;//target already blocked	
												cout<<"[ERROR:Client is already BLOCKED!]"<<endl;
											}
										}
										if(a==0)
										{
											strcpy(client_stats[e].target_ip[client_stats[e].num_targets-1],str_arr1[2]);
											cout<<client_stats[e].target_ip[client_stats[e].num_targets-1]<<" is blocked"<<endl;
										}
									}
								}
								strcpy(send_result,"BLOCK1#");//sending block details
								for(int e=0;e<client_num1;e++)
								{
									if(strcmp(str_arr1[1],client_stats[e].client_ip)==0)//searching for source_IP
									{
										for(int g=0;g<5;g++)//existing targets
										{
											strcat(send_result,client_stats[e].target_ip[g]);
											strcat(send_result,"#");	
										}
									}
								}
								strcat(send_result,"END");
								if(send(sock_index,send_result,strlen(send_result),0)<0)						{
									cerr<<"[ERROR:Send 'Block' Failed]"<<endl;
								}
								
							}//end of BLOCK#
							else if(strcmp(str_arr1[0],"UNBLOCK")==0)
							{
								int unblock=1;
								for(int e=0;e<client_num1;e++)//source_ip,target_ip,end
								{
									if(strcmp(str_arr1[1],client_stats[e].client_ip)==0)//searching for source_IP
									{
										client_stats[e].num_targets-=1;//incrementing num_targets
										for(int g=0;g<5;g++)//searching for existing targets
										{
											if(strcmp(str_arr1[2],client_stats[e].target_ip[g])==0)//checking for existing blocked clients
											{
												cout<<"client unblocked "<<client_stats[e].target_ip[g]<<endl;
												unblock=0;
												strcpy(client_stats[e].target_ip[g],"0.0.0.0");
											}
											
										}
										if(unblock==1)
										{
											cout<<"[ERROR:Client is not BLOCKED!]"<<endl;
										}
									}//end of search for source IP
								}
								unblock=1;
							}//end of UNBLOCK#	
							else if(strcmp(str_arr1[0],"EXIT")==0)
							{
								for(int e=0;e<client_num;e++)//updating client_list
								{
									if(atoi(str_arr1[1])==client_list[e].client_port)//searching for the port in client_list
									{
										client_list[e].client_port=999999999;
									}
								}//end of updating client_list
								for(int k=0;k<client_num;k++)//sorting client list
								{
									for(int j=0;j<client_num-1;j++)
									{
										if(client_list[j].client_port>client_list[j+1].client_port)
										{
											client_list[4].client_port=client_list[j].client_port;
											client_list[j].client_port=client_list[j+1].client_port;
											client_list[j+1].client_port=client_list[4].client_port;

											strcpy(client_list[4].client_ip,client_list[j].client_ip);
											strcpy(client_list[j].client_ip,client_list[j+1].client_ip);
											strcpy(client_list[j+1].client_ip,client_list[4].client_ip);
											strcpy(client_list[4].hostname,client_list[j].hostname);
											strcpy(client_list[j].hostname,client_list[j+1].hostname);
											strcpy(client_list[j+1].hostname,client_list[4].hostname);
											client_list[4].client_socket=client_list[j].client_socket;
											client_list[j].client_socket=client_list[j+1].client_socket;
											client_list[j+1].client_socket=client_list[4].client_socket;
										}//end of if(clientj>clientj+1)
									}
								}//end of port sorting
								for(int f=0;f<client_num1;f++)//updating client_stats
								{
									if(atoi(str_arr1[1])==client_stats[f].client_port)//searching for the port in client_list
									{
										client_stats[f].client_port=999999999;
									}
								}//end of updating client_stats
								for(int k=0;k<client_num1;k++)//sorting client_stats
								{
									for(int j=0;j<client_num1-1;j++)
									{
										if(client_stats[j].client_port>client_stats[j+1].client_port)
										{
											client_stats[4].client_port=client_stats[j].client_port;//copying client_port
											client_stats[j].client_port=client_stats[j+1].client_port;//replacing client_port
											client_stats[j+1].client_port=client_stats[4].client_port;//copying client_port
											strcpy(client_stats[4].client_ip,client_stats[j].client_ip);//copying client_ip
											strcpy(client_stats[j].client_ip,client_stats[j+1].client_ip);//replacing client_ip
											strcpy(client_stats[j+1].client_ip,client_stats[4].client_ip);//copying client_ip
											strcpy(client_stats[4].hostname,client_stats[j].hostname);//copying client_hostname
											strcpy(client_stats[j].hostname,client_stats[j+1].hostname);//replacing client_hostname
											strcpy(client_stats[j+1].hostname,client_stats[4].hostname);//copying client_hostname
											client_stats[4].msgin=client_stats[j].msgin;//copying msgin
											client_stats[j].msgin=client_stats[j+1].msgin;//replacing msgin
											client_stats[j+1].msgin=client_stats[4].msgin;//copying msgin
											client_stats[4].msgout=client_stats[j].msgout;//copying msgout
											client_stats[j].msgout=client_stats[j+1].msgout;//replacing msgout
											client_stats[j+1].msgout=client_stats[4].msgout;//copying msgout
											client_stats[4].client_login=client_stats[j].client_login;//copying client_login
											client_stats[j].client_login=client_stats[j+1].client_login;//replacing client_login
											client_stats[j+1].client_login=client_stats[4].client_login;//copying client_login
											client_stats[4].client_socket=client_stats[j].client_socket;//copying client_socket
											client_stats[j].client_socket=client_stats[j+1].client_socket;//replacing client_socket
											client_stats[j+1].client_socket=client_stats[4].client_socket;//copying client_socket
											for(int o=0;o<5;o++)
											{
												strcpy(client_stats[4].target_ip[o],client_stats[j].target_ip[o]);//copying target_IP
												strcpy(client_stats[j].target_ip[o],client_stats[j+1].target_ip[o]);//replacing target_IP
												strcpy(client_stats[j+1].target_ip[o],client_stats[4].target_ip[o]);//copying target_IP
											}
											client_stats[4].num_targets=client_stats[j].num_targets;//copying num_targets
											client_stats[j].num_targets=client_stats[j+1].num_targets;//replacing num_targets
											client_stats[j+1].num_targets=client_stats[4].num_targets;//copying num_targets
											client_stats[4].num_buffer=client_stats[j].num_buffer;//copying num_buffer
											client_stats[j].num_buffer=client_stats[j+1].num_buffer;//replacing num_buffer
											client_stats[j+1].num_buffer=client_stats[4].num_buffer;//copying num_buffer
											strcpy(client_stats[4].str_buffer,client_stats[j].str_buffer);//copying str_buffer
											strcpy(client_stats[j].str_buffer,client_stats[j+1].str_buffer);//replacing str_buffer
											strcpy(client_stats[j+1].str_buffer,client_stats[4].str_buffer);//copying str_buffer
										}//end of if(clientj>clientj+1)
									}
								}//end of port sorting
								client_num1-=1;//removing the exited client
							}//end of EXIT#	
							else if(strcmp(str_arr1[0],"FORWARD")==0)
							{
								strcpy(command_str,"RELAYED");
								char send_buffer_char[30000],temp_buffer_char[30000];
								for(int i=0;i<client_num1;i++)
								{
									if(strcmp(str_arr1[1],client_stats[i].client_ip)==0)//dest IP
									{
										if(client_stats[i].num_buffer>0)
										{
											strcpy(send_buffer_char,"BUFFER#");
											strcat(send_buffer_char,client_stats[i].str_buffer);
											strcat(send_buffer_char,"END");
											int str_len=strlen(send_buffer_char);
											if(sendall(client_stats[i].client_socket,send_buffer_char,&str_len)==-1)
											{
												cerr<<"[ERROR:Send MSG Failed]"<<endl;
											}
											else
											{
												strcpy(temp_buffer_char,send_buffer_char);
												char *tokRet7=NULL,str_arr7[256][30000];
												char msg[30000],from_client_ip[INET_ADDRSTRLEN],to_client_ip[INET_ADDRSTRLEN];
												int num_tokens7=0;
        											int index7=0;
        											tokRet7 = strtok(temp_buffer_char,"#");
        											while(tokRet7)
        											{                                         
         												strcpy(str_arr7[index7],tokRet7);
         												index7+=1;num_tokens7+=1;
         												tokRet7=strtok(NULL,"#");
        											}//end of while(arg)
												for(int h=2;h<num_tokens7;h+=2)
												{
													if((strcmp(str_arr7[h+1],"END")==0)||(strcmp(str_arr7[h],"END")==0))
													{
														break;
													}
													strcpy(from_client_ip,str_arr7[h]);
													strcpy(to_client_ip,str_arr1[1]);
													strcpy(msg,str_arr7[h+1]);
													cse4589_print_and_log("[%s:SUCCESS]\n",command_str);
													cse4589_print_and_log("msg from:%s, to:%s\n[msg]:%s\n",from_client_ip,to_client_ip,msg);
													cse4589_print_and_log("[%s:END]\n",command_str);
												}
												for(int g=0;g<client_num1;g++)
												{
													if(strcmp(client_stats[g].client_ip,str_arr1[1])==0)//destination IP
													{
														for(int a=0;a<client_stats[g].num_buffer;a++)
														{
															client_stats[g].msgin+=1;//incrementing msgin
														}
													}
												}
												client_stats[i].num_buffer=0;
												strcpy(client_stats[i].str_buffer,"START#");
											}
										}//end of num_buffer>0
										else
										{
											strcpy(send_buffer_char,"BUFFER#");
											strcat(send_buffer_char,"END");
											int str_len=strlen(send_buffer_char);
											if(sendall(client_stats[i].client_socket,send_buffer_char,&str_len)==-1)
											{
												cerr<<"[ERROR:Send MSG Failed]"<<endl;
											}
										}
									}
								}
							}//end of FORWARD#
		
        					}// end of recv success
       					}//end of dealing with existing sockets
      				}//end of FD_ISSET
     			}//end of read loop
    		}//end of else if(selRet>0)
   	}//end of for(;;) 
}//server ends

