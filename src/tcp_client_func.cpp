#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<cstring>
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#include"tcp_client_func.h"

#include "../include/global.h"
#include "../include/logger.h"

#define STDIN 0
#define BACKLOG 5
#define BUFFSIZE 30000

using namespace std;

struct client_details1//structure for storing client list
{
	char client_ip[INET_ADDRSTRLEN];
	char hostname[256];
	int  client_port;
};

int itoa(int value,char *ptr)//reference from https://stackoverflow.com/questions/8257714/how-to-convert-an-int-to-string-in-c
{
        int count=0,temp;
        if(ptr==NULL)
            return 0;   
        if(value==0)
        {   
            *ptr='0';
            return 1;
        }

        if(value<0)
        {
            value*=(-1);    
            *ptr++='-';
            count++;
        }
        for(temp=value;temp>0;temp/=10,ptr++);
        *ptr='\0';
        for(temp=value;temp>0;temp/=10)
        {
            *--ptr=temp%10+'0';
            count++;
        }
        return count;
}

int sendall(int s, char *buf, int *len);
	
int tcp_client_func(int port)
{
	cout<<"PA1-CLIENT is running....."<<endl;
   	int head_socket=0;//the socket descriptor holding the max value (initialised with zero)
   	int sock_index=0, server_socket=0;                                  /*cout<<"in the tcp server function.."<<endl;*/
   	int selRet=0;
	char char_list[]="LIST#",char_msg[]="MSG#",char_parse[]="#",send_result[30000],str_port[256],command_str[256];
	itoa(port,str_port);
	char udp_buffer[INET_ADDRSTRLEN],temp_ip1[INET_ADDRSTRLEN],blocked_ip[5][INET_ADDRSTRLEN];
	strcpy(blocked_ip[0],"0.0.0.0");
	strcpy(blocked_ip[1],"0.0.0.0");
	strcpy(blocked_ip[2],"0.0.0.0");
	strcpy(blocked_ip[3],"0.0.0.0");
	strcpy(blocked_ip[4],"0.0.0.0");
   	int saddr_len;int num_clients=0;
	struct client_details1 client_list[4];for(saddr_len=0;saddr_len<4;saddr_len++){strcpy(client_list[saddr_len].hostname,"END");}
	saddr_len=0;
   	char your_ubit_name[]="rohinkum",buffer[30000];//server_ip[INET_ADDRSTRLEN];
   	fd_set masterfds, readfds;//the master set of socket descriptors (connected) and the read set (temporary set for read)
   	FD_ZERO(&masterfds);//initialising master set with zero elements
   	FD_ZERO(&readfds);//initialising read set with zero elements

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
		cerr<<"[Connect Failed!]"<<endl;
	}
	getsockname(udp_socket,(struct sockaddr *)&udp_addr,(socklen_t *)&udpAddr_len);
       	inet_ntop(AF_INET,&(udp_addr.sin_addr),udp_buffer,INET_ADDRSTRLEN);

	char hostname[256];
	if(gethostname(hostname, sizeof(hostname))<0)
	{
  		cerr<<"gethostname failed!"<<endl;
 	}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   	struct sockaddr_in server_addr;
   	bzero(&server_addr,sizeof(server_addr));//initialising server_addr
   	FD_SET(STDIN, &masterfds);//setting STDIN in masterfds socket set
   	for(;;)
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
      				if(FD_ISSET(sock_index, &readfds))
      				{                                           /*cout<<"read loop success"<<endl;*/
       					if(sock_index==STDIN)
       					{                                          /*cout<<"entered STDIN..."<<endl;*/
        					char cmd[30000],snd[30000],*tokRet=NULL,str_arr[100][30000];
        					memset(cmd,'\0',sizeof(cmd));//initialising cmd
        					memset(snd,'\0',sizeof(snd));//initialising snd
        					int index=0;int num_tokens=0;
        					cin.getline(cmd,30000);
						strcpy(snd,cmd);
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
        					else if(strcmp(str_arr[0],"LOGIN")==0)
        					{
							strcpy(command_str,"LOGIN");
							if(num_tokens==3)
							{
         							char server_ip[INET_ADDRSTRLEN];
								int server_port=atoi(str_arr[2]);
         							strcpy(server_ip,str_arr[1]); //server_ip=str_arr[1];
         							if((inet_pton(AF_INET,server_ip,&server_addr.sin_addr))<=0)
         							{
          								//cerr<<"[LOGIN:ERROR]\n"<<"[ERROR:INVALID IP ADDRESS]\n"<<"[LOGIN:END]"<<endl;
									cse4589_print_and_log("[%s:ERROR]\n",command_str);
									cout<<"[ERROR:INVALID IP ADDRESS]"<<endl;
									cse4589_print_and_log("[%s:END]\n",command_str);
         							}
								else if(atoi(str_arr[2])==0)
								{
									cse4589_print_and_log("[%s:ERROR]\n",command_str);
									cout<<"[ERROR:INVALID PORT]"<<endl;
									cse4589_print_and_log("[%s:END]\n",command_str);
								}
								else if((strcmp(str_arr[1],"128.205.36.46")==0)||(strcmp(str_arr[1],"128.205.36.35")==0)||(strcmp(str_arr[1],"128.205.36.33")==0)||(strcmp(str_arr[1],"128.205.36.34")==0)||(strcmp(str_arr[1],"128.205.36.36")==0)||(strcmp(str_arr[1],udp_buffer)==0))
								{
	          							if((server_socket=socket(AF_INET,SOCK_STREAM,0))<0)
	          							{
										cse4589_print_and_log("[%s:ERROR]\n",command_str);
										cout<<"[ERROR:Failed to create socket]"<<endl;
										cse4589_print_and_log("[%s:END]\n",command_str);
	          							}
	          							else
	          							{                    //cout<<"second argument is: "<<str_arr[1]<<endl;
	           								bzero(&server_addr,sizeof(server_addr));
	           								server_addr.sin_family=AF_INET;
	           								inet_pton(AF_INET,server_ip,&server_addr.sin_addr);
	           								server_addr.sin_port=htons(server_port);
							 	        	if(connect(server_socket,(struct sockaddr *)&server_addr,sizeof(server_addr))<0)
     		      								{
											cse4589_print_and_log("[%s:ERROR]\n",command_str);
     		       									cout<<"[ERROR:Connect Failed]"<<endl;
											cse4589_print_and_log("[%s:END]\n",command_str);
     		      								}
     		      								else
     		      								{
											strcpy(temp_ip1,udp_buffer);
											strcpy(send_result,char_list);
											strcat(send_result,hostname);
											strcat(send_result,char_parse);
											strcat(send_result,temp_ip1);
											strcat(send_result,char_parse);
											strcat(send_result,str_port);
											//cout<<"send_result value is "<<send_result<<endl;
											if(send(server_socket,send_result,strlen(send_result),0)<0)
											{
												cse4589_print_and_log("[%s:ERROR]\n",command_str);
												cerr<<"[ERROR:Send IP to Server Failed]"<<endl;
												cse4589_print_and_log("[%s:END]\n",command_str);
											}
											/*else
											{
												cse4589_print_and_log("[%s:SUCCESS]\n",command_str);
          											cse4589_print_and_log("[%s:END]\n",command_str);
											}*/
            										FD_SET(server_socket, &masterfds);
            										if(server_socket>head_socket)
            										{
             											head_socket=server_socket;
            										}
            										cout<<"Remote Server Connected!"<<endl;
										
           									}//end of connect
          								}//end of server_socket success
         							}//end of valid (KNOWN) IP address
								else
								{
									cse4589_print_and_log("[%s:ERROR]\n",command_str);
									cout<<"[ERROR:UNKNOWN IP]"<<endl;
									cse4589_print_and_log("[%s:END]\n",command_str);
								}
							}//end of num_tokens
							else
							{
								cse4589_print_and_log("[%s:ERROR]\n",command_str);
								cout<<"[ERROR:INVALID ARGUMENTS]"<<endl;
								cse4589_print_and_log("[%s:END]\n",command_str);
							}
        					}//end of LOGIN
						else if(strcmp(str_arr[0],"LIST")==0)
						{
							strcpy(command_str,"LIST");
							if(server_socket>0)
         						{
								int list_id=0;char hostname[256];
								char ip_addr[INET_ADDRSTRLEN];int port_num=0;
          							//cout<<"["<<str_arr[0]<<":SUCCESS]"<<endl;
								cse4589_print_and_log("[%s:SUCCESS]\n",command_str);
								for(int i=0;i<num_clients;i++)
								{
									list_id=i+1;
									strcpy(hostname,client_list[i].hostname);
									strcpy(ip_addr,client_list[i].client_ip);
									port_num=client_list[i].client_port;
									cse4589_print_and_log("%-5d%-35s%-20s%-8d\n", list_id, hostname, ip_addr, port_num);
								}
          							//cout<<"["<<str_arr[0]<<":END]"<<endl;
								cse4589_print_and_log("[%s:END]\n",command_str);
         						}
							else
         						{
          							//cout<<"["<<str_arr[0]<<":ERROR]\n"<<"ERROR:Not Logged in!\n"<<"["<<str_arr[0]<<":END]"<<endl;
								cse4589_print_and_log("[%s:ERROR]\n",command_str);
								cout<<"ERROR:Not Logged in!"<<endl;
								cse4589_print_and_log("[%s:END]\n",command_str);
         						}
						}
        					else if(strcmp(str_arr[0],"LOGOUT")==0)
        					{
							strcpy(command_str,"LOGOUT");
         						if(server_socket>0)
         						{	
								strcpy(send_result,"LOGOUT#");strcat(send_result,str_port);//sending LOGOUT#port
								if(send(server_socket,send_result,strlen(send_result),0)<0)
								{
									cse4589_print_and_log("[%s:ERROR]\n",command_str);
          								cerr<<"[ERROR:Send LOGOUT Failed]"<<endl;
									cse4589_print_and_log("[%s:END]\n",command_str);
								}
								else
								{
									cse4589_print_and_log("[%s:SUCCESS]\n",command_str);
          								cse4589_print_and_log("[%s:END]\n",command_str);
								}
          							close(server_socket);
								FD_CLR(server_socket,&masterfds);
								server_socket=-1;
         						}
         						else
         						{
								cse4589_print_and_log("[%s:ERROR]\n",command_str);
          							cout<<"ERROR:Not Logged in!"<<endl;
								cse4589_print_and_log("[%s:END]\n",command_str);
         						}
        					}//end of LOGOUT 
						else if(strcmp(str_arr[0],"EXIT")==0)
        					{
							strcpy(command_str,"EXIT");
         						if(server_socket>0)
         						{	strcpy(send_result,"EXIT#");strcat(send_result,str_port);//sending LOGOUT#port
								if(send(server_socket,send_result,strlen(send_result),0)<0)
								{
									cse4589_print_and_log("[%s:ERROR]\n",command_str);
									cerr<<"[ERROR:Send EXIT Failed]"<<endl;
									cse4589_print_and_log("[%s:END]\n",command_str);
								}
								else
								{
									cse4589_print_and_log("[%s:SUCCESS]\n",command_str);
									cse4589_print_and_log("[%s:END]\n",command_str);
									return 0;
								}
          							close(server_socket);
								FD_CLR(server_socket,&masterfds);server_socket=-1;
         						}
         						else
         						{
          							close(server_socket);
								FD_CLR(server_socket,&masterfds);server_socket=-1;
								cse4589_print_and_log("[%s:SUCCESS]\n",command_str);
								cse4589_print_and_log("[%s:END]\n",command_str);
								return 0;
         						}
        					}//end of EXIT 
						else if(strcmp(str_arr[0],"REFRESH")==0)
        					{
							strcpy(command_str,"REFRESH");
							if(server_socket>0)
							{
								strcpy(send_result,"REFRESH");
         							//cout<<"["<<str_arr[0]<<":SUCCESS]"<<endl;
								cse4589_print_and_log("[%s:SUCCESS]\n",command_str);
         							if(send(server_socket,send_result,strlen(send_result),0)<0)
								{
									cse4589_print_and_log("[%s:ERROR]\n",command_str);
									cerr<<"[ERROR:Send REFRESH to Server Failed]"<<endl;
									cse4589_print_and_log("[%s:END]\n",command_str);
								}
         							//cout<<"["<<str_arr[0]<<":END]"<<endl;
								cse4589_print_and_log("[%s:END]\n",command_str);
							}
							else
							{
								cse4589_print_and_log("[%s:ERROR]\n",command_str);
								cout<<"ERROR:Not Logged in!"<<endl;
								cse4589_print_and_log("[%s:END]\n",command_str);
							}
        					}//end of REFRESH         
						else if(strcmp(str_arr[0],"SEND")==0)
        					{
							strcpy(command_str,"SEND");
							if(num_tokens>=3)
							{
         							if((strcmp(str_arr[1],"128.205.36.46")==0)||(strcmp(str_arr[1],"128.205.36.35")==0)||(strcmp(str_arr[1],"128.205.36.33")==0)||(strcmp(str_arr[1],"128.205.36.34")==0)||(strcmp(str_arr[1],"128.205.36.36")==0)||(strcmp(str_arr[1],udp_buffer)==0))
         							{
									if(server_socket>0)
									{
										char *tokRet2=NULL,str_arr2[1000][30000];int abcd=0;
	        								tokRet2 = strtok(snd," ");//tokRet2 has SEND
										tokRet2 = strtok(NULL," ");//tokRet2 has destination IP 
										strcpy(str_arr2[0],tokRet2);//Storing destination IP
										tokRet2 = strtok(NULL,"\n");//tokRet2 has msg;
										strcpy(str_arr2[1],tokRet2);//Storing msg
										strcpy(str_arr2[2],"MSG#");
										//strcat(str_arr2[2],str_arr[1]);
										for(int count=0;count<num_clients;count++)
										{
											if(strcmp(str_arr2[0],client_list[count].client_ip)==0)//comapring dest IP in list
											{
												abcd+=1;
											}
										}
										if(abcd==0)
										{
											cse4589_print_and_log("[%s:ERROR]\n",command_str);
											cout<<"[ERROR:UNKNOWN IP]"<<endl;
											cse4589_print_and_log("[%s:END]\n",command_str);
										}
										else//abcd!=0(KNOWN IP in list)
										{
											int str_len =0;
											for(int count=0;count<num_clients;count++)
											{
												if(strcmp(str_arr2[0],client_list[count].client_ip)==0)//checking destination IP
												{
													strcat(str_arr2[2],udp_buffer);//copying source IP
													strcat(str_arr2[2],"#");
													strcat(str_arr2[2],client_list[count].client_ip);//copying destination IP	
													strcat(str_arr2[2],"#");
													strcat(str_arr2[2],str_arr2[1]);//copying message
													strcat(str_arr2[2],"#END");//End
													str_len=strlen(str_arr2[2]);
													//WRITE CODE FOR BUFFER SEND!!!!!
													if(sendall(server_socket,str_arr2[2], &str_len) == -1)
													//if(send(server_socket,str_arr2[2],strlen(str_arr2[2]),0)<0)	
													{
														cse4589_print_and_log("[%s:ERROR]\n",command_str);
														cerr<<"[ERROR:Send MSG Failed]"<<endl;
														cse4589_print_and_log("[%s:END]\n",command_str);
													}
													else
													{
														cse4589_print_and_log("[%s:SUCCESS]\n",command_str);
														cse4589_print_and_log("[%s:END]\n",command_str);
													}
												}//end of dest IP check
											}//end for(num_clients)
	        	 							}//end of known IP in list
									}//end of server_socket >0 (logged in!)
									else// logged out
									{
										cse4589_print_and_log("[%s:ERROR]\n",command_str);
										cout<<"ERROR:Not Logged in!"<<endl;
										cse4589_print_and_log("[%s:END]\n",command_str);
									}
								}//end of comparing Known IP's (128.205...)
         							else// unknown IP with valid format
								{
									cse4589_print_and_log("[%s:ERROR]\n",command_str);
          								cerr<<"[ERROR:INVALID IP ADDRESS]"<<endl;
									cse4589_print_and_log("[%s:END]\n",command_str);
         							}
							}//end of num_tokens
							else//less tokens entered
							{
								cse4589_print_and_log("[%s:ERROR]\n",command_str);
	          						cerr<<"[ERROR:INVALID ARGUMENTS]"<<endl;
								cse4589_print_and_log("[%s:END]\n",command_str);
							}
        					}//end of SEND
						else if(strcmp(str_arr[0],"BROADCAST")==0)
        					{
							strcpy(command_str,"BROADCAST");
							if(num_tokens>=2)
							{
								if(server_socket>0)
								{
								
									char *tokRet3=NULL,str_arr3[1000][30000];
									int str_len=0;
	        							tokRet3 = strtok(snd," ");//tokRet3 has BROADCAST
									tokRet3 = strtok(NULL,"\n");//tokRet3 has msg
									strcpy(str_arr3[1],tokRet3);//Storing msg
									strcpy(str_arr3[2],"BROAD#");
									//strcat(str_arr2[2],str_arr[1]);
									strcat(str_arr3[2],udp_buffer);//copying source IP
									strcat(str_arr3[2],"#");
									strcat(str_arr3[2],str_arr3[1]);//copying message
									strcat(str_arr3[2],"#END");//End
									//cout<<str_arr2[2]<<endl;
									str_len=strlen(str_arr3[2]);
									if(sendall(server_socket,str_arr3[2], &str_len) == -1)
									//if(send(server_socket,str_arr3[2],strlen(str_arr3[2]),0)<0)
									{
										cse4589_print_and_log("[%s:ERROR]\n",command_str);
										cerr<<"[ERROR:Send MSG Failed]"<<endl;
										cse4589_print_and_log("[%s:END]\n",command_str);
									}
									else
									{
										cse4589_print_and_log("[%s:SUCCESS]\n",command_str);
	         								cse4589_print_and_log("[%s:END]\n",command_str);
									}
								}
								else
								{
									cse4589_print_and_log("[%s:ERROR]\n",command_str);
									cout<<"ERROR:Not Logged in!"<<endl;
									cse4589_print_and_log("[%s:END]\n",command_str);
								}
							}
							else
							{
								cse4589_print_and_log("[%s:ERROR]\n",command_str);
								cout<<"ERROR:INVALID ARGUMENTS"<<endl;
								cse4589_print_and_log("[%s:END]\n",command_str);
							}
        					}//end of BROADCAST
						else if(strcmp(str_arr[0],"BLOCK")==0)
        					{
							strcpy(command_str,"BLOCK");char tmpn[INET_ADDRSTRLEN];
							if(num_tokens==2)
							{
								if(server_socket>0)//logged in
								{
									if((strcmp(str_arr[1],"128.205.36.46")==0)||(strcmp(str_arr[1],"128.205.36.35")==0)||(strcmp(str_arr[1],"128.205.36.33")==0)||(strcmp(str_arr[1],"128.205.36.34")==0)||(strcmp(str_arr[1],"128.205.36.36")==0)||(strcmp(str_arr[1],udp_buffer)==0))
									{
										strcpy(tmpn,str_arr[1]);
										if((strcmp(tmpn,blocked_ip[0])==0)||(strcmp(tmpn,blocked_ip[1])==0)||(strcmp(tmpn,blocked_ip[2])==0)||(strcmp(tmpn,blocked_ip[3])==0)||(strcmp(tmpn,blocked_ip[4])==0))//checking blocked_ip to find existing targets
										{
											cse4589_print_and_log("[%s:ERROR]\n",command_str);
											cout<<"[ERROR:CLIENT IS ALREADY BLOCKED]"<<endl;
											cse4589_print_and_log("[%s:END]\n",command_str);
										}
										else
										{
											char str_arr4[100][1000];int abcd=0;
											strcpy(str_arr4[1],str_arr[1]);//Storing target IP to block
											strcpy(str_arr4[2],"BLOCK#");
											//strcat(str_arr2[2],str_arr[1]);
											for(int count=0;count<num_clients;count++)
											{
												if(strcmp(str_arr[1],client_list[count].client_ip)==0)//checking target IP in list	
												{
													abcd+=1;
												}
											}//end of KNOWN target IP check
											if(abcd==0)//target IP not in list
											{
												cse4589_print_and_log("[%s:ERROR]\n",command_str);
												cout<<"[ERROR:UNKNOWN IP]"<<endl;
												cse4589_print_and_log("[%s:END]\n",command_str);
											}
											else//target IP in list
											{
												strcat(str_arr4[2],udp_buffer);//copying source IP
												strcat(str_arr4[2],"#");
												strcat(str_arr4[2],str_arr4[1]);//copying target IP to block	
												strcat(str_arr4[2],"#END");//End
												//cout<<str_arr2[2]<<endl;
												if(send(server_socket,str_arr4[2],strlen(str_arr4[2]),0)<0)
												{
													cse4589_print_and_log("[%s:ERROR]\n",command_str);
													cerr<<"[ERROR:Send 'Block' Failed]"<<endl;
													cse4589_print_and_log("[%s:END]\n",command_str);	
												}
												else
												{
													cse4589_print_and_log("[%s:SUCCESS]\n",command_str);				     							cse4589_print_and_log("[%s:END]\n",command_str);
												}
											}//end of target IP in list
	         								}//end of existing targets
									}//end of valid IP (128.205...)
									else
									{
										cse4589_print_and_log("[%s:ERROR]\n",command_str);
										cout<<"ERROR:INVALID IP!"<<endl;
										cse4589_print_and_log("[%s:END]\n",command_str);
									}
								}//end of server_socket>0 login check
								else
								{
									cse4589_print_and_log("[%s:ERROR]\n",command_str);
									cout<<"ERROR:Not Logged in!"<<endl;
									cse4589_print_and_log("[%s:END]\n",command_str);
								}
							}//end of num_tokens==2
							else
							{
								cse4589_print_and_log("[%s:ERROR]\n",command_str);
								cout<<"ERROR:INVALID ARGUMENTS"<<endl;
								cse4589_print_and_log("[%s:END]\n",command_str);
							}
        					}//end of BLOCK
						else if(strcmp(str_arr[0],"UNBLOCK")==0)
        					{
							strcpy(command_str,"UNBLOCK");
							if(num_tokens==2)
							{
								if(server_socket>0)
								{
									if((strcmp(str_arr[1],"128.205.36.46")==0)||(strcmp(str_arr[1],"128.205.36.35")==0)||(strcmp(str_arr[1],"128.205.36.33")==0)||(strcmp(str_arr[1],"128.205.36.34")==0)||(strcmp(str_arr[1],"128.205.36.36")==0)||(strcmp(str_arr[1],udp_buffer)==0))
									{	
										if((strcmp(str_arr[1],blocked_ip[0])==0)||(strcmp(str_arr[1],blocked_ip[1])==0)||(strcmp(str_arr[1],blocked_ip[2])==0)||(strcmp(str_arr[1],blocked_ip[3])==0)||(strcmp(str_arr[1],blocked_ip[4])==0))//checking blocked_ip to find existing targets
										{
											char str_arr4[1000][30000];int abcd=0;
											strcpy(str_arr4[1],str_arr[1]);//Storing IP to block
											strcpy(str_arr4[2],"UNBLOCK#");
											//strcat(str_arr2[2],str_arr[1]);
											for(int count=0;count<num_clients;count++)
											{
												if(strcmp(str_arr[1],client_list[count].client_ip)==0)//checking target IP in list	
												{
													abcd+=1;
												}
											}//end of KNOWN target IP check
											if(abcd==0)//target IP not in list
											{
												cse4589_print_and_log("[%s:ERROR]\n",command_str);
												cout<<"[ERROR:UNKNOWN IP]"<<endl;
												cse4589_print_and_log("[%s:END]\n",command_str);
											}
											else//target IP in list
											{
												for(int count=0;count<num_clients;count++)
												{
													if(strcmp(str_arr4[1],client_list[count].client_ip)==0)
													{
														strcat(str_arr4[2],udp_buffer);//copying source IP
														strcat(str_arr4[2],"#");
														strcat(str_arr4[2],str_arr[1]);//copying target IP to block	
														strcat(str_arr4[2],"#END");//End
														//cout<<str_arr2[2]<<endl;
														if(send(server_socket,str_arr4[2],strlen(str_arr4[2]),0)<0)
														{
															cerr<<"[ERROR:Send 'Unblock' Failed]"<<endl;	
														}											         															else
														{
															cse4589_print_and_log("[%s:SUCCESS]\n",command_str);				     								cse4589_print_and_log("[%s:END]\n",command_str);												
															if(strcmp(str_arr[1],blocked_ip[0])==0)
															{
																strcpy(blocked_ip[0],"0.0.0.0");
															}
															else if(strcmp(str_arr[1],blocked_ip[1])==0)
															{
																strcpy(blocked_ip[1],"0.0.0.0");
															}
															else if(strcmp(str_arr[1],blocked_ip[2])==0)
															{
																strcpy(blocked_ip[2],"0.0.0.0");
															}
															else if(strcmp(str_arr[1],blocked_ip[3])==0)
															{
																strcpy(blocked_ip[3],"0.0.0.0");
															}
															else if(strcmp(str_arr[1],blocked_ip[4])==0)
															{
																strcpy(blocked_ip[4],"0.0.0.0");
															}
														}
													}
												}
											}//end of target ip in list
	         								}//end of existing target check
										else
										{
											cse4589_print_and_log("[%s:ERROR]\n",command_str);
											cout<<"[ERROR:CLIENT IS NOT BLOCKED]"<<endl;
											cse4589_print_and_log("[%s:END]\n",command_str);
										}
									}//end of invalid IP
									else
									{
										cse4589_print_and_log("[%s:ERROR]\n",command_str);
										cout<<"ERROR:INVALID IP!"<<endl;
										cse4589_print_and_log("[%s:END]\n",command_str);
									}
								}//end of login check
								else
								{
									cse4589_print_and_log("[%s:ERROR]\n",command_str);
									cout<<"ERROR:Not Logged in!"<<endl;
									cse4589_print_and_log("[%s:END]\n",command_str);
								}
							}//end of num_tokens==2
							else
							{
								cse4589_print_and_log("[%s:ERROR]\n",command_str);
								cout<<"ERROR:INVALID ARGUMENTS"<<endl;
								cse4589_print_and_log("[%s:END]\n",command_str);
							}
        					}//end of UNBLOCK
						else if(strcmp(str_arr[0],"BLOCK_LIST")==0)
        					{
							cout<<blocked_ip[0]<<endl;
							cout<<blocked_ip[1]<<endl;
							cout<<blocked_ip[2]<<endl;
							cout<<blocked_ip[3]<<endl;
							cout<<blocked_ip[4]<<endl;	
						}
        					else//wrong STDIN
        					{
         						//cout<<"["<<str_arr[0]<<":ERROR]\n"<<"["<<str_arr[0]<<":END]"<<endl;
							strcpy(command_str,str_arr[0]);
							cse4589_print_and_log("[%s:ERROR]\n",command_str);
							cse4589_print_and_log("[%s:END]\n",command_str);
        					}
       					}//end of STDIN
       					else if(sock_index==server_socket)//Handling data from socket
       					{
        					memset(buffer,'\0',sizeof(buffer));
        					if(recv(server_socket,buffer,BUFFSIZE,0)<=0)
        					{
         						close(server_socket);
         						cout<<"Remote Server terminated connection!"<<endl;
         						FD_CLR(server_socket,&masterfds);
							server_socket=-1;
        					}
        					else//receiving from server_port
        					{
							char *tokRet1=NULL,str_arr1[1000][30000];int temp_port=0;
 							int index1=0,num_tokens2=0;char buffer22[30000],buffer23[30000];
 							tokRet1 = strtok(buffer,"#");
				
							char client_ip[INET_ADDRSTRLEN],msg[30000];
								strcpy(command_str,"RECEIVED");
								
 							while(tokRet1)
							{
 								strcpy(str_arr1[index1], tokRet1);          
 								index1+=1;num_tokens2+=1;
 								tokRet1=strtok(NULL,"#");
							}
							if(strcmp(str_arr1[0],"MSG")==0)
							{
								//cout<<num_tokens2<<endl;

								if(num_tokens2==3)
								{	int num_tokens3=0;
									strcat(buffer23,str_arr1[2]);
									while(num_tokens3!=2)
									{
										num_tokens3=0;
										if(recv(server_socket,buffer22,sizeof(buffer22),0)<=0)
        									{
       											cout<<"ERROR:continue failed"<<endl;
       										}
										char *tokRet3=NULL,str_arr55[10][30000];
 										int index3=0;
 										tokRet3 = strtok(buffer22,"#");
 										while(tokRet3)
										{
 											strcpy(str_arr55[index3], tokRet3);          
 											index3+=1;num_tokens3+=1;
 											tokRet3=strtok(NULL,"#");
										}//tokenize
										strcat(buffer23,str_arr55[0]);
											
									}
									//cout<<buffer23<<endl;
									strcpy(msg,buffer23);
									strcpy(client_ip,str_arr1[1]);
									cse4589_print_and_log("[%s:SUCCESS]\n",command_str);
									cse4589_print_and_log("msg from:%s\n[msg]:%s\n",client_ip,msg);
									cse4589_print_and_log("[%s:END]\n",command_str);
				
								}
								else
								{
								
								strcpy(msg,str_arr1[2]);
								strcpy(client_ip,str_arr1[1]);
								cse4589_print_and_log("[%s:SUCCESS]\n",command_str);
								cse4589_print_and_log("msg from:%s\n[msg]:%s\n",client_ip,msg);
								cse4589_print_and_log("[%s:END]\n",command_str);
								/*cout<<"[RECEIVED:SUCCESS]"<<endl;
								cout<<"msg from:"<<str_arr1[1]<<"\n[msg]:"<<str_arr1[2]<<endl;
								cout<<"[RECEIVED:END]"<<endl;*/
								}
							}	
							else if(strcmp(str_arr1[0],"LIST")==0)
							{
								char temp_send[1000];
								strcpy(temp_send,"FORWARD#");
								strcpy(temp_ip,udp_buffer);//src addr
								strcat(temp_send,temp_ip);
								strcat(temp_send,"#END");
								num_clients=0;
								for(int i=0;i<50;i+=3)
								{
									if((strcmp(str_arr1[i],"END")==0)){break;}
									else if((strcmp(str_arr1[i+1],"END")==0)){break;}
									else if((strcmp(str_arr1[i+2],"END")==0)){break;}
									else if((strcmp(str_arr1[i+3],"END")==0)){break;}
									strcpy(client_list[num_clients].hostname,str_arr1[i+1]);//host
									strcpy(client_list[num_clients].client_ip,str_arr1[i+2]);//IP
									temp_port=atoi(str_arr1[i+3]);//PORT
									client_list[num_clients].client_port=temp_port;
									num_clients+=1;//obtaining number of clients
								}
								if(send(server_socket,temp_send,strlen(temp_send),0)<0)
								{
									cerr<<"[ERROR:Send temp_send Failed]"<<endl;
								}					
							}
							else if(strcmp(str_arr1[0],"REFLIST")==0)
							{
								num_clients=0;
								for(int i=0;i<50;i+=3)
								{
									if((strcmp(str_arr1[i],"END")==0)){break;}
									else if((strcmp(str_arr1[i+1],"END")==0)){break;}
									else if((strcmp(str_arr1[i+2],"END")==0)){break;}
									else if((strcmp(str_arr1[i+3],"END")==0)){break;}
									strcpy(client_list[num_clients].hostname,str_arr1[i+1]);//host
									strcpy(client_list[num_clients].client_ip,str_arr1[i+2]);//IP
									temp_port=atoi(str_arr1[i+3]);//PORT
									client_list[num_clients].client_port=temp_port;
									num_clients+=1;//obtaining number of clients
								}					
							}
							else if(strcmp(str_arr1[0],"BUFFER")==0)//receiving block array from server
							{
								strcpy(command_str,"RECEIVED");
								if(num_tokens2>2)
								{
									if(strcmp(str_arr1[num_tokens2-1],"END")==0)
									{
										for(int i=2;i<num_tokens2;i+=2)
										{
											if(strcmp(str_arr1[i+1],"END")==0){break;}
											strcpy(msg,str_arr1[i+1]);
											strcpy(client_ip,str_arr1[i]);
											cse4589_print_and_log("[%s:SUCCESS]\n",command_str);
											cse4589_print_and_log("msg from:%s\n[msg]:%s\n",client_ip,msg);
											cse4589_print_and_log("[%s:END]\n",command_str);
										
										}
											strcpy(command_str,"LOGIN");
											cse4589_print_and_log("[%s:SUCCESS]\n",command_str);
											cse4589_print_and_log("[%s:END]\n",command_str);
									}
									else
									{	/*char buffer24[30000];memset(buffer24,'\0',sizeof(buffer24));
										int num_tokens4=0;
										while(strcmp(str_arr1[num_tokens2-1],"END")!=0)
										{
											for(int p=0;p<num_tokens2;p++)
											{
												strcat(buffer24,str_arr1[2]);
											}
										}
										while(num_tokens3!=2)
										{
											num_tokens3=0;
											if(recv(server_socket,buffer22,sizeof(buffer22),0)<=0)
        										{
       												cout<<"ERROR:continue failed"<<endl;
       											}
											char *tokRet3=NULL,str_arr55[10][30000];
 											int index3=0;
 											tokRet3 = strtok(buffer22,"#");
 											while(tokRet3)
											{
 												strcpy(str_arr55[index3], tokRet3);          
 												index3+=1;num_tokens3+=1;
 												tokRet3=strtok(NULL,"#");
											}//tokenize
											strcat(buffer23,str_arr55[0]);
										}*/
									}
								}
								else
								{
									strcpy(command_str,"LOGIN");
									cse4589_print_and_log("[%s:SUCCESS]\n",command_str);
									cse4589_print_and_log("[%s:END]\n",command_str);
								}
							}

							else if(strcmp(str_arr1[0],"BLOCK1")==0)//receiving block array from server
							{
									strcpy(blocked_ip[0],str_arr1[1]);
									strcpy(blocked_ip[1],str_arr1[2]);
									strcpy(blocked_ip[2],str_arr1[3]);
									strcpy(blocked_ip[3],str_arr1[4]);
									strcpy(blocked_ip[4],str_arr1[5]);
							}	
         						//cout<<"The server sent me the following message \n"<<str_arr1[0]<<"\n"<<str_arr1[1]<<"\n"<<str_arr1[2]<<"\n"<<str_arr1[3]<<"\n"<<str_arr1[4]<<endl;
        					}
       					}//end of receive server_socket
      				}//end of FD_ISSET
     			}//end of read loop
    		}//end of else if(selRet>0)
   	}//end of for(;;) 
}//client ends


