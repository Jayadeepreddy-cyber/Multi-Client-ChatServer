

//Example code: A simple server side code, which echos back the received message.
//Handle multiple socket connections with select and fd_set on Linux
#include <stdio.h>
#include <string.h> //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h> //close
#include <arpa/inet.h> //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
#include<ctype.h>
    
#define TRUE 1
#define FALSE 0
#define PORT 5000
void approveadminreqUtil(int ,int ,int );
void declineadminreqUtil(int ,int ,int );
struct groups
{
    int clientid[5];
    int admins[5];
    int broadcast;
    int clientrequests[5];
    int no_of_accepts[5];
    int no_of_rejects[5];
}g[100];
struct groupreq
{
    int clientid;
    int gid[100];
}g_req[100];
void makegroupreqto0()
{
    int i,j;
    for(i=0;i<100;i++)
    {
        g_req[i].clientid=0;
        for(j=0;j<100;j++)
        {
            g_req[i].gid[j]=-1;

        }
    }
}
void makegroup0()
{
    int i,j;
    for(i=0;i<100;i++)
    {
        for(j=0;j<5;j++)
        {
            g[i].clientid[j]=0;
            g[i].admins[j]=0;
            g[i].broadcast=0;
            g[i].clientrequests[i]=0;
            g[i].no_of_accepts[i]=0;
            g[i].no_of_rejects[i]=0;
        }

    }
}
int max_clients=10;
int Unique_id[10],client_socket[10],no_of_groups=0,no_of_greq=0;
char send_buffer[1025],receive_buffer[1025]; //data buffer of 1K
int checkifclientactive(int id)
{
    int i;
    for(i=0;i<max_clients;i++)
    {
        if(Unique_id[i]==id)
        {
            return i+1;

        }
    }
    return 0;
}
void invalidcommand(int sd)
{
    bzero(send_buffer,1024);
    sprintf(send_buffer,"InvalidCommand\n");
    send(sd,send_buffer,strlen(send_buffer),0);
}
int findclientid(int sd)
{
    int i;
    for(i=0;i<max_clients;i++)
    {
        if(client_socket[i]==sd)
        {
            return Unique_id[i];
        }
    }
}
void storerequests(int index,int gid)
{
    int flag=0,i,j;
    for(i=0;i<100;i++)
    {
        if(g_req[i].clientid==Unique_id[index])
        {
            flag=1;
            for(j=0;j<100;j++)
            {
                if(g_req[i].gid[j]==-1)
                {
                    
                    g_req[i].gid[j]=gid;
                    break;

                }
            }
            

        }
        if(flag==1)
        break;
    }
    if(flag==0)
    {
        for(i=0;i<100;i++)
        {
            if(g_req[i].clientid==0)
            {
                g_req[i].clientid=Unique_id[index];
                g_req[i].gid[0]=gid;
                break;
            }
        }
    }
    /*
    for(i=0;i<100;i++)
    {
        if(g_req[i].clientid!=0)
        {
            printf("%d",g_req[i].clientid);
            for(j=0;j<100;j++)
            {
                if(g_req[i].gid[j]!=-1)
                {
                    printf("%d",g_req[i].gid[j]);
                }
            }
        }
    }
    */
    
}
void makegroupreq(int sd)
{

    int no_of_spaces=0,i=0,adminflag=0,j=0,flag=0;
    char temp[10];
    while(i<strlen(receive_buffer)-1)
    {
        if(receive_buffer[i]==' ')
        {
            no_of_spaces++;
        }
        i++;
    }
    if(no_of_spaces<=4)
    {
        
        i=14;
        bzero(send_buffer,1024);
        while(i<strlen(receive_buffer)-1)
        {
            j=0;
            for(;receive_buffer[i]!=' ' && i<strlen(receive_buffer)-1;i++)
            {
                if(isdigit(receive_buffer[i]))
                {
                    temp[j++]=receive_buffer[i];

                }
                else
                {
                    
                invalidcommand(sd);
                flag=1;
                break;
                }
                
            }
            if(flag==1)
            break;
            temp[j]='\0';
            int id=atoi(temp);
            int index=checkifclientactive(id);
            int adminid=findclientid(sd);
            if(index)
            {
                for(i=0;i<max_clients;i++)
                {
                    if(client_socket[i]==sd)
                    {
                        g[no_of_groups].clientid[0]=Unique_id[i];
                        g[no_of_groups].admins[0]=1;
                        break;
                    }
                }
                char send_buffer1[1024];
                bzero(send_buffer1,1024);
                sprintf(send_buffer1,"Group:%d with admin Id:%d want you to be a group member\n",no_of_groups,adminid);
                storerequests(index-1,no_of_groups);
                send(client_socket[index-1],send_buffer1,sizeof(send_buffer1),0);

                
                
                
            }
            else{
                char message[100];
                sprintf(message,"Client %d is Inactive\n",id);
                strcat(send_buffer,message);
            }
            i++;

        }
        strcat(send_buffer,"Group Request Message sent to clients successfully\n");
        send(sd,send_buffer,sizeof(send_buffer),0);
        bzero(send_buffer,1024);
        no_of_groups++;
        
    }
    else
    {
        invalidcommand(sd);
        return;
    }

}
int findsocketid(int cid)
{
    int i;
    for(i=0;i<max_clients;i++)
    {
        if(Unique_id[i]==cid)
        {
            return client_socket[i];
        }
    }
}
void makegroup(int sd)
{
        
    
        int flag=0,i=0,j,no_of_spaces=0,index=1;
        char temp[10];
        i=0;
        while(i<strlen(receive_buffer))
        {
            if(receive_buffer[i]==' ')
            {
                no_of_spaces++;
            }
            i++;
        }
    
        if(no_of_spaces<=4)
        {
            for(i=0;i<max_clients;i++)
            {
                if(client_socket[i]==sd)
                {
                    g[no_of_groups].clientid[0]=Unique_id[i];
                    g[no_of_groups].admins[0]=1;
                    break;
                }
            }
            i=11;
            while(i<strlen(receive_buffer) && receive_buffer[i]!='\n')
            {
                j=0;
                for(;receive_buffer[i]!=' ' && receive_buffer[i]!='\n';i++)
                {
                    if(isdigit(receive_buffer[i]))
                    {
                        temp[j++]=receive_buffer[i];

                    }
                    else
                    {
                    invalidcommand(sd);
                    flag=1;
                    break;
                    }
                    
                }
                if(flag==1)
                break;
                temp[j]='\0';
                int id=atoi(temp);
                if(checkifclientactive(id))
                {
                    g[no_of_groups].clientid[index]=id;
                    index++;
                    
                }
                else{
                    bzero(send_buffer,1024);
                    char message[100];
                    sprintf(message,"Client %d is Inactive\n",id);
                    strcat(send_buffer,message);
                }
                i++;

            }
            if(flag==1)
            return;
            bzero(send_buffer,1024);
            sprintf(send_buffer,"Group is created with group ID:%d\n",no_of_groups);
            send(sd,send_buffer,sizeof(send_buffer),0);
            
            for(i=1;i<5;i++)
            {
                int c_id=g[no_of_groups].clientid[i];
                if(c_id!=0)
                {
                    bzero(send_buffer,1024);
                    sprintf(send_buffer,"you are added into the group with group id:%d and group admin id:%d\n",no_of_groups,g[no_of_groups].clientid[0]);
                    int s_id=findsocketid(c_id);
                    send(s_id,send_buffer,sizeof(send_buffer),0);
                    
                    
                }
            }
            no_of_groups++;
    }
    else{
        invalidcommand(sd);
        
    }
}
void joingroup(int sd)
{
    int i,j=0,flag=0;
    char temp[10];
    for(i=11;i<strlen(receive_buffer)-1;i++)
    {
        if(isdigit(receive_buffer[i]))
        temp[j++]=receive_buffer[i];
        else
        {
            invalidcommand(sd);
            return ;
        }

    }
    temp[j]='\0';
    int g_id=atoi(temp),count=0;
   
    int c_id=findclientid(sd);
    for(i=0;i<100;i++)
    {
        //printf("%d ",g_req[i].clientid);
        if(g_req[i].clientid==c_id)
        {
            for(j=0;j<100;j++)
            {
                if(g_req[i].gid[j]==g_id)
                {
                    flag=1;
                    g_req[i].gid[j]=-1;
                    break;
                    
                }
            }
            if(flag==0)
            {
                
                sprintf(send_buffer,"You cannot join a group  which you didn't get request\n");
                send(sd,send_buffer,sizeof(send_buffer),0);
                return;
            }
            else if(flag==1)
            break;
        }

    }
    if(flag==0)
    {
        
        sprintf(send_buffer,"You cannot join a group request which you didn't get request\n");
        send(sd,send_buffer,sizeof(send_buffer),0);
        return ;
    }
    
    for(i=0;i<5;i++)
        {
            if(g[g_id].clientid[i]==0)
            {
                g[g_id].clientid[i]=c_id;
                sprintf(send_buffer,"You are successfully added into the Group with Group Id:%d\n",g_id);
                send(sd,send_buffer,sizeof(send_buffer),0);
                break;
            }
        }
        
    
}
void makegroupbroadcast(int sd,int g_id)
{
    int i,uniqueid=findclientid(sd);
    for(i=0;i<5;i++)
    {
        if(g[g_id].clientid[i]==uniqueid)
        {
            
            if(g[g_id].broadcast==1)
            {
                sprintf(send_buffer,"Group already in broadcast mode\n");
                send(sd,send_buffer,sizeof(send_buffer),0);
                return ;

            }
            else{
                g[g_id].broadcast=1;
                sprintf(send_buffer,"Group mode is changed into broadcast mode\n");
                send(sd,send_buffer,sizeof(send_buffer),0);
                return ;

            }


        }
    }
    
}
void activegroups(int sd)
{
    
    int c_id=findclientid(sd),i,j,k,flag=0;
    char temp[100];
    bzero(send_buffer,1024);
    strcat(send_buffer,"The Groups with group ids that you are in:\n");
    for(i=0;i<no_of_groups;i++)
    {
        for(j=0;j<5;j++)
        {
            if(g[i].clientid[j]==c_id)
            {
                flag=1;
                sprintf(temp,"Group %d and Group Admins Id are:\n",i);
                strcat(send_buffer,temp);
                for(k=0;k<5;k++)
                {
                    if(g[i].admins[k]==1)
                    {
                        bzero(temp,100);
                        sprintf(temp,"%d ",g[i].clientid[k]);
                        strcat(send_buffer,temp);
                    }
                    if(k==4)
                    {
                        strcat(send_buffer,"\n");
                    }
                }
                break;
            }
        }
    }
    if(flag==0)
    strcat(send_buffer,"NILL\n");
    send(sd,send_buffer,sizeof(send_buffer),0);
    bzero(send_buffer,1024);
}
void makeadmin(int sd,int g_id,int c_id)
{
    int i;
    int sid=findsocketid(c_id),flag=0;
    for(i=0;i<5;i++)
    {
        if(g[g_id].clientid[i]==c_id)
        {
            flag=1;
            if(g[g_id].admins[i]==0)
            {
                g[g_id].admins[i]=1;
                sprintf(send_buffer,"You are added as Admin for Group:%d\n",g_id);
                send(sid,send_buffer,sizeof(send_buffer),0);
                break;
            }
            else
            {
                bzero(send_buffer,1024);
                sprintf(send_buffer,"Client is already an Admin for Group:%d\n",g_id);
                send(sd,send_buffer,sizeof(send_buffer),0);
                return;

            }
        }
    }
    if(flag==1)
    {
        bzero(send_buffer,1024);
        sprintf(send_buffer,"Client:%d is added as an Admin for Group:%d\n",c_id,g_id);
        send(sd,send_buffer,sizeof(send_buffer),0);
        return;
    }
    

}
int findIsClientExistsInGroup(int g_id,int c_id)
{
    int i;
    for(i=0;i<5;i++)
    {
        if(g[g_id].clientid[i]==c_id)
        return 1;
    }
    return 0;
}
void clearClientRequestsforGroup(int g_id,int c_id)
{
    int i,j;
    for(i=0;i<100;i++)
    {
        if(g_req[i].clientid==c_id)
        {
            for(j=0;j<100;j++)
            {
                if(g_req[i].gid[j]==g_id)
                {
                    g_req[i].gid[j]=-1;
                }
            }
        }
    }

}
void addtogroup(int sd,int g_id,int c_id)
{
    int i;
    if(checkifclientactive(c_id))
    {
        
        if(findIsClientExistsInGroup(g_id,c_id)==0)
        {
            clearClientRequestsforGroup(g_id,c_id);
            for(i=0;i<5;i++)
            {
                if(g[g_id].clientid[i]==0 )
                {
                    g[g_id].clientid[i]=c_id;
                    int sock_id=findsocketid(c_id);
                    char send_buffer1[1024];
                    sprintf(send_buffer1,"You are added into the group:%d\n",g_id);
                    send(sock_id,send_buffer1,sizeof(send_buffer1),0);
                    return;

                }
                    
            }
        }
        else
        {
            char message[100];
            sprintf(message,"Client %d is already present in the group\n",c_id);
            strcat(send_buffer,message);

        }
        
        
    }
    else{
        char message[100];
        sprintf(message,"Client %d is Inactive\n",c_id);
        strcat(send_buffer,message);
    }

}
int findIsAdmin(int sd,int g_id)
{
    int c_id=findclientid(sd),i;
    for(i=0;i<5;i++)
    {
        if(g[g_id].clientid[i]==c_id)
        {
            return g[g_id].admins[i];
        }
    }
    return 6;
}
void declinegroup(int sd)
{
    int i=14,j=0;
    char temp[5];
    for(i=14;receive_buffer[i]!='\n';i++)
    {
        temp[j++]=receive_buffer[i];
    }
    temp[j]='\0';
    int g_id=atoi(temp);
    int c_id=findclientid(sd);
    for(i=0;i<100;i++)
    {
        if(g_req[i].clientid==c_id)
        {
            for(j=0;j<100;j++)
            {
                if(g_req[i].gid[j]==g_id)
                {
                   
                    g_req[i].gid[j]=-1;
                    int admin_id=g[g_id].clientid[0];
                    int A_sd=findsocketid(admin_id);
                    sprintf(send_buffer,"Client %d has declined your Group %d request\n",c_id,g_id);
                    send(A_sd,send_buffer,sizeof(send_buffer),0);
                    return;
                    
                }
            }
            
            sprintf(send_buffer,"You cannot decline a group request which you didn't get request\n");
            send(sd,send_buffer,sizeof(send_buffer),0);
            return;
        
        }

    }

    sprintf(send_buffer,"You cannot decline a group request which you didn't get request\n");
    send(sd,send_buffer,sizeof(send_buffer),0);

}
int findgroupmembers(int g_id)
{
    int i,count=0;
    for(i=0;i<5;i++)
    {
        if(g[g_id].clientid!=0)
        {
            count++;
        }
    }
    return count;
}

void sendMessageToGroup(int sd,int g_id,int c_id,char message[])
{
    int i,s_id;
    for(i=0;i<5;i++)
    {
        if(g[g_id].clientid[i]!=0 && g[g_id].clientid[i]!=c_id)
        {
            strcpy(send_buffer,message);
            s_id=findsocketid(g[g_id].clientid[i]);
            send(s_id,send_buffer,sizeof(send_buffer),0);
            
        }
    }
    bzero(send_buffer,1024);
    sprintf(send_buffer,"Message send to members of Group:%d successfully\n",g_id);
    send(sd,send_buffer,sizeof(send_buffer),0);

}
int  findIsGroupBroadcast(int sd,int g_id)
{
    return g[g_id].broadcast;

}
void extractMessage(int i,char message[])
{
    int j=0;
    for(;receive_buffer[i]!='\n';i++)
    {
        message[j++]=receive_buffer[i];
    }
    message[j]='\n';
    return ;
}
void sendgroup(int sd)
{
    int i,j=0,g_id,c_id,val;
    char temp[5],message[1000];
    for(i=11;receive_buffer[i]!=' ';i++)
    {
        if(isdigit(receive_buffer[i]))
        temp[j++]=receive_buffer[i];
        else
        {
            invalidcommand(sd);
            return;
        }

    }
    i++;
    temp[j]='\0';
    g_id=atoi(temp);
    c_id=findclientid(sd);
    val=findIsAdmin(sd,g_id);
    if(val)
    {
        if(val==6)
        {
            sprintf(send_buffer,"Group %d does not exist or you are not part of the group\n",g_id);
            send(sd,send_buffer,sizeof(send_buffer),0);

        }
        else
        {
            
            extractMessage(i,message);
            sendMessageToGroup(sd,g_id,c_id,message);
        }

    }
    else if(findIsGroupBroadcast(sd,g_id)==0)
    {
        extractMessage(i,message);
        sendMessageToGroup(sd,g_id,c_id,message);
    }
    else
    {
        sprintf(send_buffer,"You cannot send message to the group as you are not admin to the group %d\n",g_id);
        send(sd,send_buffer,sizeof(send_buffer),0);

    }
    return;


}
void clearClientRequests(int sd,int c_id)
{
    int i,j;
    for(i=0;i<100;i++)
    {
        if(g_req[i].clientid==c_id)
        {
            g_req[i].clientid=0;
            for(j=0;j<100;j++)
            {
                g_req[i].gid[j]=-1;
            }
        }
    }


}

void clearClientFromGroup(int sd,int c_id)
{
    int i,j,k;
    for(i=0;i<100;i++)
    {
        for(j=0;j<5;j++)
        {
            if(g[i].clientid[j]==c_id)
            {
                
                g[i].clientid[j]=0;
                g[i].admins[j]=0;
                g[i].clientrequests[j]=0;
                g[i].no_of_accepts[j]=0;
                g[i].no_of_rejects[j]=0;
                
                bzero(send_buffer,1024);
                sprintf(send_buffer,"Client %d is disconnected from Group %d\n",c_id,i);
                for(k=0;k<5;k++)
                {
                    if(g[i].clientid[k]!=0)
                    {
                        int s_id=findsocketid(g[i].clientid[k]);
                        send(s_id,send_buffer,sizeof(send_buffer),0);
                    }

                }
                break;
            }
            

        }
        
    }
}
void clearGroupsIfNoAdmins(int sd)
{

    int i,j,k,count=0;
    for(i=0;i<100;i++)
    {
        count=0;
        for(j=0;j<5;j++)
        {
            if(g[i].admins[j]==1)
                {
                        count++;
                }
        }    
        if(count==0)
        {
            bzero(send_buffer,1024);
            sprintf(send_buffer,"Group:%d is Deactivated\n",i);
            for(k=0;k<5;k++)
            {
                if(g[i].clientid[k]!=0)
                {
                    int s_id=findsocketid(g[i].clientid[k]);
                    send(s_id,send_buffer,sizeof(send_buffer),0);
                    g[i].clientid[k]=0;
                    g[i].clientrequests[k]=0;
                    g[i].no_of_accepts[k]=0;
                    g[i].no_of_rejects[k]=0;

                }
            }

        }
    }
        
}



void removefromgroup(int sd,int g_id,int c_id)
{
    int i;
      for(i=0;i<5;i++)
        {
            if(g[g_id].clientid[i]==c_id)
            {
                
                g[g_id].clientid[i]=0;
                int sock_id=findsocketid(c_id);
                char send_buffer1[1024];
                sprintf(send_buffer1,"You are removed from the group:%d\n",g_id);
                send(sock_id,send_buffer1,sizeof(send_buffer1),0);
                return;

            }
        
        }
        clearGroupsIfNoAdmins(sd);
    
 
}
int  findIsGroupExists(int g_id)
{
    int i;
    for(i=0;i<5;i++)
    {
        if(g[g_id].clientid[i]!=0)
        return 1;
    }
    return 0;
}

void makeadminreqUtil(int g_id,int c_id)
{
    
    int j;
    for(j=0;j<5;j++)
    {
        if(g[g_id].clientid[j]==c_id)
        {
            g[g_id].clientrequests[j]=1;
            break;
        }
    }
    for(j=0;j<5;j++)
    {
        if(g[g_id].admins[j]==1)
        {
            int s_id=findsocketid(g[g_id].clientid[j]);
            sprintf(send_buffer,"Client %d wants to be Admin of the Group %d\n",c_id,g_id);
            send(s_id,send_buffer,sizeof(send_buffer),0);

        }
    }

}
void makeadminreq(int sd)
{
    int i,j=0,no_of_spaces=0;
    char temp[10];
    i=14;
    while(receive_buffer[i]!='\n')
    {
        if(receive_buffer[i]==' ')
        no_of_spaces++;
        i++;
    }
    if(no_of_spaces!=0)
    {
        invalidcommand(sd);
        return;

    }
    for(i=14;receive_buffer[i]!='\n';i++)
    {
        temp[j++]=receive_buffer[i];
    }
    temp[j]='\0';
    int g_id=atoi(temp);
    
    if(findIsGroupExists(g_id))
    {
        int c_id=findclientid(sd);
        
        if(findIsClientExistsInGroup(g_id,c_id))
        {
            
            if(findIsAdmin(sd,g_id))
            {
                sprintf(send_buffer,"You are already a Admin of the Group %d\n",g_id);
                send(sd,send_buffer,sizeof(send_buffer),0);
                return ;

            }
            else
            {
                
                makeadminreqUtil(g_id,c_id);
                
            }
            
            
        }
        else
        {
                sprintf(send_buffer,"You are not a memeber of the Group %d\n",g_id);
                send(sd,send_buffer,sizeof(send_buffer),0);
                return ;

        }
        

    }
    else
    {
        sprintf(send_buffer,"Group %d Does not Exists\n",g_id);
        send(sd,send_buffer,sizeof(send_buffer),0);
        return ;
    }
    
}
int findIsClientRequestedInGroup(int g_id,int c_id)
{
    int i;
    for(i=0;i<5;i++)
    {
        if(g[g_id].clientid[i]==c_id && g[g_id].clientrequests[i]==1)
        {
            return 1;

        }
    }
    return 0;
}
int findNoOfAdmins(int g_id)
{
    int i,count=0;
    for(i=0;i<5;i++)
    {
        if(g[g_id].admins[i]==1)
        count++;
    }
    return count;
}

void approveadminreqUtil(int sd,int g_id,int c_id)
{
    int i,index;
    for(i=0;i<5;i++)
    {
        if(g[g_id].clientid[i]==c_id)
        {
            index=i;
            g[g_id].no_of_accepts[i]+=1;
        }
    }
    int no_of_admins=findNoOfAdmins(g_id);
    
    int no_of_accepts=g[g_id].no_of_accepts[index];
    int no_of_rejects=g[g_id].no_of_rejects[index];
    if(no_of_admins==no_of_accepts+no_of_rejects && no_of_accepts>=no_of_rejects && no_of_admins!=0)
    {
        g[g_id].clientrequests[index]=0;
        g[g_id].no_of_accepts[index]=0;
        g[g_id].no_of_rejects[index]=0;
        g[g_id].admins[index]=1;
        sprintf(send_buffer,"You are now a admin for the Group %d\n",g_id);
        send(sd,send_buffer,sizeof(send_buffer),0);


    }
    else if(no_of_admins==no_of_accepts+no_of_rejects && no_of_accepts<no_of_rejects)
    {
        g[g_id].clientrequests[index]=0;
        g[g_id].no_of_accepts[index]=0;
        g[g_id].no_of_rejects[index]=0;
        g[g_id].admins[index]=0;
        sprintf(send_buffer,"Your request for  admin for the Group %d is rejected\n",g_id);
        send(sd,send_buffer,sizeof(send_buffer),0);
    }

}
void approveadminreq(int sd)
{
    int i,j=0,no_of_spaces=0;
    char temp[10];
    i=17;
    while(receive_buffer[i]!='\n')
    {
        if(receive_buffer[i]==' ')
        no_of_spaces++;
        i++;
    }
    if(no_of_spaces!=1)
    {
        invalidcommand(sd);
        return;

    }
    for(i=17;receive_buffer[i]!=' ';i++)
    {
        temp[j++]=receive_buffer[i];
    }
    i++;
    temp[j]='\0';
    int g_id=atoi(temp);
    j=0;
    while(receive_buffer[i]!='\n')
    {
        temp[j++]=receive_buffer[i];
        i++;
    }
    temp[j]='\0';
    int c_id=atoi(temp);
    if(findIsGroupExists(g_id))
    {
        if(findIsClientRequestedInGroup(g_id,c_id))
        {
            if(findIsAdmin(sd,g_id))
            {
                int s_id=findsocketid(c_id);
                
                approveadminreqUtil(s_id,g_id,c_id);
            }
            else
            {
                sprintf(send_buffer,"You cannot approve a admin request  of the Group %d as you are not Admin\n",g_id);
                send(sd,send_buffer,sizeof(send_buffer),0);
                return ;

            }
        }
        else
        {
            sprintf(send_buffer,"Client %d did not request for Admin/ client is not the member of the group %d\n",c_id,g_id);
            send(sd,send_buffer,sizeof(send_buffer),0);
            return ;
        }
    }
    else{
        sprintf(send_buffer,"Group %d Does not Exists\n",g_id);
        send(sd,send_buffer,sizeof(send_buffer),0);
        return ;
    }
}
void declineadminreqUtil(int sd,int g_id,int c_id)
{
    int i,index;
    for(i=0;i<5;i++)
    {
        if(g[g_id].clientid[i]==c_id)
        {
            index=i;
            g[g_id].no_of_rejects[i]+=1;
        }
    }
    int no_of_admins=findNoOfAdmins(g_id);
    int no_of_accepts=g[g_id].no_of_accepts[index];
    int no_of_rejects=g[g_id].no_of_rejects[index];
    if( no_of_admins==no_of_accepts+no_of_rejects && no_of_accepts<no_of_rejects && no_of_admins!=0)
    {
        g[g_id].clientrequests[index]=0;
        g[g_id].no_of_accepts[index]=0;
        g[g_id].no_of_rejects[index]=0;
        g[g_id].admins[index]=0;
        sprintf(send_buffer,"Your request for admin for the Group %d is rejected\n",g_id);
        send(sd,send_buffer,sizeof(send_buffer),0);


    }
    else if(no_of_admins==no_of_accepts+no_of_rejects && no_of_accepts>=no_of_rejects)
    {
        g[g_id].clientrequests[index]=0;
        g[g_id].no_of_accepts[index]=0;
        g[g_id].no_of_rejects[index]=0;
        g[g_id].admins[index]=1;
        sprintf(send_buffer,"Your request for admin for the Group %d is accepted\n",g_id);
        send(sd,send_buffer,sizeof(send_buffer),0);
    }
}
void declineadminreq(int sd)
{
    int i,j=0,no_of_spaces=0;
    char temp[10];
    i=17;
    while(receive_buffer[i]!='\n')
    {
        if(receive_buffer[i]==' ')
        no_of_spaces++;
        i++;
    }
    if(no_of_spaces!=1)
    {
        invalidcommand(sd);
        return;

    }
    for(i=17;receive_buffer[i]!=' ';i++)
    {
        temp[j++]=receive_buffer[i];
    }
    i++;
    temp[j]='\0';
    int g_id=atoi(temp);
    j=0;
    while(receive_buffer[i]!='\n')
    {
        temp[j++]=receive_buffer[i];
        i++;
    }
    temp[j]='\0';
    int c_id=atoi(temp);
    if(findIsGroupExists(g_id))
    {
        if(findIsClientRequestedInGroup(g_id,c_id))
        {
            if(findIsAdmin(sd,g_id))
            {
                int s_id=findsocketid(c_id);
                declineadminreqUtil(s_id,g_id,c_id);
            }
            else
            {
                sprintf(send_buffer,"You cannot decline a admin request  of the Group %d as you are not Admin\n",g_id);
                send(sd,send_buffer,sizeof(send_buffer),0);
                return ;

            }
        }
        else
        {
            sprintf(send_buffer,"Client %d did not request for Admin/ client is not the member of the group %d\n",c_id,g_id);
            send(sd,send_buffer,sizeof(send_buffer),0);
            return ;
        }
    }
    else{
        sprintf(send_buffer,"Group %d Does not Exists\n",g_id);
        send(sd,send_buffer,sizeof(send_buffer),0);
        return ;
    }

}

int main(int argc , char *argv[])
{
    
    int opt = TRUE;
    int master_socket , addrlen , new_socket, activity, i , valread , sd;
    int max_sd,count=1;
    struct sockaddr_in address;
    int n;
        
    
        
    //set of socket descriptors
    fd_set readfds;
        
    //a message
    
    
    //initialise all client_socket[] to 0 so not checked
    for (i = 0; i < max_clients; i++)
    {
        client_socket[i] = 0;
    }
        
    //create a master socket
    if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    
    //set master socket to allow multiple connections ,
    //this is just a good habit, it will work without this
    if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,
        sizeof(opt)) < 0 )
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    
    //type of socket created
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
        
    //bind the socket to localhost port 8888
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Listener on port %d \n", PORT);
        
    //try to specify maximum of 3 pending connections for the master socket
    if (listen(master_socket, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
        
    //accept the incoming connection
    addrlen = sizeof(address);
    puts("Waiting for connections ...");
    makegroupreqto0();
    makegroup0();   
    while(TRUE)
    {
        //clear the socket set
        FD_ZERO(&readfds);
    
        //add master socket to set
        FD_SET(master_socket, &readfds);
        max_sd = master_socket;
            
        //add child sockets to set
        for ( i = 0 ; i < max_clients ; i++)
        {
            //socket descriptor
            sd = client_socket[i];
                
            //if valid socket descriptor then add to read list
            if(sd > 0)
                FD_SET( sd , &readfds);
                
            //highest file descriptor number, need it for the select function
            if(sd > max_sd)
                max_sd = sd;
        }
    
        //wait for an activity on one of the sockets , timeout is NULL ,
        //so wait indefinitely
        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);
    
        if ((activity < 0) && (errno!=EINTR))
        {
            printf("select error");
        }
        
        //If something happened on the master socket ,
        //then its an incoming connection
        if (FD_ISSET(master_socket, &readfds))
        {
            if ((new_socket = accept(master_socket,
                    (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }
            if(count>max_clients)
            {
                bzero(send_buffer,1024);
                sprintf(send_buffer,"Connection Limit Exceeded\n");
                n=send(new_socket,send_buffer,strlen(send_buffer),0);

            }
            else
            {
            
            //inform user of socket number - used in send and receive commands
            printf("New connection , socket fd is %d , ip is : %s , port : %d\n" , new_socket , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
            bzero(send_buffer,1024);
            sprintf(send_buffer,"Welcome Client%d your socket ID is %d and Unique ID is %d\n",count,new_socket,ntohs(address.sin_port));
            count++;
        
            //send new connection greeting message
            if( send(new_socket, send_buffer, sizeof(send_buffer), 0) != strlen(send_buffer) )
            {
                perror("send");
            }
                
            puts("Welcome message sent successfully");
                
            //add new socket to array of sockets
            for (i = 0; i < max_clients; i++)
            {
                //if position is empty
                if( client_socket[i] == 0 )
                {
                    client_socket[i] = new_socket;
                    //printf("Adding to list of sockets as %d\n" , i);
                    Unique_id[i]=ntohs(address.sin_port);
                    break;
                }
            }
        }
        }
            
        //else its some IO operation on some other socket
        for (i = 0; i < max_clients; i++)
        {
            sd = client_socket[i];
                
            if (FD_ISSET( sd , &readfds))
            {
                //Check if it was for closing , and also read the
                //incoming message
                bzero(receive_buffer,1024);
                if ((valread = recv( sd , receive_buffer, sizeof(receive_buffer),0)) == 0)
                {
                    //Somebody disconnected , get his details and print
                    getpeername(sd , (struct sockaddr*)&address , \
                        (socklen_t*)&addrlen);
                    printf("Host disconnected , ip %s , port %d \n" ,
                        inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
                        
                    //Close the socket and mark as 0 in list for reuse
                    close( sd );
                    client_socket[i] = 0;
                    Unique_id[i]=0;
                    count--;
                }
                else if(strncmp(receive_buffer,"/quit",5)==0 && strlen(receive_buffer)==6)
                {
                    int c_id=findclientid(sd);
                    clearClientRequests(sd,c_id);
                    clearClientFromGroup(sd,c_id);
                    clearGroupsIfNoAdmins(sd);
                    bzero(send_buffer,1024);
                    strcpy(send_buffer,"exit");
                    n=send(sd,send_buffer,sizeof(send_buffer),0);
                    getpeername(sd , (struct sockaddr*)&address , \
                        (socklen_t*)&addrlen);
                    printf("Host disconnected , ip %s , port %d \n" ,
                        inet_ntoa(address.sin_addr) , ntohs(address.sin_port));
                        
                    //Close the socket and mark as 0 in list for reuse
                    
                    close( sd );
                    client_socket[i] = 0;
                    Unique_id[i]=0;
                    count--;
                    

                }
                else if(strncmp(receive_buffer,"/active",7)==0 && strlen(receive_buffer)==8)
                {
                    int i;
                    
                    
                    char temp[100];
                    bzero(send_buffer,1024);
                    strcpy(send_buffer,"Socketfd UniqueID\n");
                    for(i=0;i<max_clients;i++)
                    {
                        if(client_socket[i]!=0)
                        {
                        sprintf(temp,"%d\t%d\n",client_socket[i],Unique_id[i]);
                        strcat(send_buffer,temp);
                        memset(temp,0,strlen(temp));

                        }
                    }
                    n=send(sd,send_buffer,sizeof(send_buffer),0);

                    
                }
                else if(strncmp(receive_buffer,"/sendgroup",10)==0)
                    {
                        
                        sendgroup(sd);
                    }
                
                else if(strncmp(receive_buffer,"/send",5)==0)
                {
                    int i;
                    char temp[10];
                    int j=0,flag=0;
                    for(i=6;receive_buffer[i]!=' ' && receive_buffer[i]!='\0';i++)
                    {
                        if(isdigit(receive_buffer[i]))
                        {
                        temp[j++]=receive_buffer[i];
                        }
                        else{
                        invalidcommand(sd);
                        continue;
                        }
                    }
                    
                    
                    if(receive_buffer[i]=='\0')
                    {
                        invalidcommand(sd);
                        continue;
                    }
                    i++;
                    temp[j]='\0';
                    int senderid=atoi(temp);
                    bzero(send_buffer,1024);
                    j=0;
                    for(;receive_buffer[i]!='\0';i++)
                    {
                        send_buffer[j++]=receive_buffer[i];
                    }
                    send_buffer[j]='\0';
                    for(i=0;i<max_clients;i++)
                    {
                        if(senderid==Unique_id[i])
                        {
                            n=send(client_socket[i],send_buffer,sizeof(send_buffer),0);
                            flag=1;
                            break;
                        }
                        
                    }
                    if(flag==0)
                    {
                        bzero(send_buffer,1024);
                        sprintf(send_buffer,"Client with Unique ID:%d is disconnected\n",senderid);
                        send(sd,send_buffer,sizeof(send_buffer),0);
                    }
                    bzero(send_buffer,1024);
                    

                }
                else if(strncmp(receive_buffer,"/broadcast",10)==0)
                {
                    int i;
                    bzero(send_buffer,1024);
                    int j=0;
                    for(i=11;receive_buffer[i]!='\0';i++)
                    {
                        send_buffer[j++]=receive_buffer[i];

                    }
                    send_buffer[j]='\0';
                    
                    for(i=0;i<max_clients;i++)
                    {
                        if(client_socket[i]!=0 && client_socket[i]!=sd)
                        {
                            n=send(client_socket[i],send_buffer,sizeof(send_buffer),0);
                        }
                    }
                    bzero(send_buffer,1024);
                }
                else if(strncmp(receive_buffer,"/makegroupbroadcast",19)==0)
                {
                    
                    int i,g_id,flag=0,j=0,val;
                    char temp[10];
                    for(i=20;receive_buffer[i]!='\n';i++)
                    {
                        if(isdigit(receive_buffer[i]))
                        temp[j++]=receive_buffer[i];
                        else
                        {
                            invalidcommand(sd);
                            flag=1;
                            break;
                        }   

                    }
                    if(flag==0)
                    {
                        temp[j]='\0';
                        g_id=atoi(temp);
                        
                        val=findIsAdmin(sd,g_id);
                        if(val)
                        {
                            if(val!=6)
                                makegroupbroadcast(sd,g_id);
                            else
                            {
                                sprintf(send_buffer,"Invalid group Id or you are not member of this group id:%d\n",g_id);
                                send(sd,send_buffer,sizeof(send_buffer),0);

                            }

                        }
                            
                        else
                        {
                            sprintf(send_buffer,"You cannot change the type of Group:%d to broadcast as you are not admin\n",g_id);
                            send(sd,send_buffer,sizeof(send_buffer),0);
                        }
                    }
                }
                else if(strncmp(receive_buffer,"/makegroupreq",13)==0)
                {
                    makegroupreq(sd);
                }
                else if(strncmp(receive_buffer,"/makegroup",10)==0)
                {
                    
                    makegroup(sd);
                }
                else if(strncmp(receive_buffer,"/joingroup",10)==0)
                {
                    joingroup(sd);

                }
                
                else if(strncmp(receive_buffer,"/activegroups",13)==0)
                {
                    activegroups(sd);
                }
                else if(strncmp(receive_buffer,"/makeadminreq",13)==0)
                {
                    makeadminreq(sd);
                }
                else if(strncmp(receive_buffer,"/makeadmin",10)==0)
                {
                    
                    int i=11,g_id,flag=0,c_id,j=0;
                    char temp[10];
                    for(i=11;receive_buffer[i]!=' ';i++)
                    {
                        if(isdigit(receive_buffer[i]))
                        temp[j++]=receive_buffer[i];
                        else{
                            flag=1;
                            invalidcommand(sd);
                            break;
                            
                        }
                    }
                    if(flag==0)
                    {
                        temp[j]='\0';
                        g_id=atoi(temp);
                        j=0;
                        if(findIsAdmin(sd,g_id))
                        {
                            while(i<strlen(receive_buffer))
                            {
                                temp[j++]=receive_buffer[i];
                                i++;
                            }
                            temp[j]='\0';
                            c_id=atoi(temp);
                            makeadmin(sd,g_id,c_id);
                        }
                        else
                    {
                        sprintf(send_buffer,"You Cannot make others as Admin as you are not Admin\n");
                        send(sd,send_buffer,sizeof(send_buffer),0);
                    }                       
                    }
                    
                }
                else if(strncmp(receive_buffer,"/declinegroup",13)==0)
                {
                    declinegroup(sd);



                }
                else if(strncmp(receive_buffer,"/addtogroup",11)==0)
                {
                    int i,j=0,no_of_spaces1=0;
                    char temp[10];
                    i=12;
                    while(receive_buffer[i]!='\n')
                    {
                        if(receive_buffer[i]==' ')
                        no_of_spaces1++;
                        i++;
                    }
                    if(no_of_spaces1==0)
                    {
                        invalidcommand(sd);
                        continue;

                    }
                    for(i=12;receive_buffer[i]!=' ';i++)
                    {
                        temp[j++]=receive_buffer[i];
                    }
                    i++;
                    int index=i;
                    temp[j]='\0';
                    int g_id=atoi(temp);
                    
                    int no_of_spaces=0;
                    if(findIsAdmin(sd,g_id))
                    {
                        while(receive_buffer[i]!='\n')
                        {
                            if(receive_buffer[i]==' ')
                            no_of_spaces++;
                        i++;

                        }
                        int g_members=findgroupmembers(g_id);
                        if(g_members+no_of_spaces>5)
                        {
                            invalidcommand(sd);
                            continue;
                        }
                        else
                        {
                            i=index;
                            while(receive_buffer[i]!='\n')
                            {
                                j=0;
                                for(;receive_buffer[i]!=' ' && receive_buffer[i]!='\n';i++)
                                {
                                    temp[j++]=receive_buffer[i];
                                }
                                if(receive_buffer[i]==' ')
                                i++;
                                temp[j]='\0';
                                int c_id=atoi(temp);
                                addtogroup(sd,g_id,c_id);
                                

                            }
                            char message[100];
                            sprintf(message,"Group %d members added successfully\n",g_id);
                            strcpy(send_buffer,message);
                            send(sd,send_buffer,sizeof(send_buffer),0);
                            bzero(send_buffer,1024);
                            
                        }
                    }
                    else
                    {
                        sprintf(send_buffer,"You can't add members as you are not admin to group:%d\n",g_id);
                        send(sd,send_buffer,sizeof(send_buffer),0);
                        bzero(send_buffer,1024);

                    }
                    

                    
                }
                else if(strncmp(receive_buffer,"/removefromgroup",16)==0)
                {
                    int i,j=0,no_of_spaces1;
                    char temp[10];
                    i=17;
                    while(receive_buffer[i]!='\n')
                    {
                        if(receive_buffer[i]==' ')
                        no_of_spaces1++;
                        i++;
                    }
                    if(no_of_spaces1==0)
                    {
                        invalidcommand(sd);
                        continue;

                    }
                    for(i=17;receive_buffer[i]!=' ';i++)
                    {
                        temp[j++]=receive_buffer[i];
                    }
                    i++;
                    int index=i;
                    temp[j]='\0';
                    int g_id=atoi(temp);
                    int no_of_spaces=0;
                    if(findIsAdmin(sd,g_id))
                    {
                        while(receive_buffer[i]!='\n')
                        {
                            if(receive_buffer[i]==' ')
                            no_of_spaces++;
                        i++;

                        }
                        int g_members=findgroupmembers(g_id);
                        if((no_of_spaces+1)>g_members)
                        {
                            invalidcommand(sd);
                            continue;
                        }
                        else
                        {
                            i=index;
                            while(receive_buffer[i]!='\n')
                            {
                                j=0;
                                for(;receive_buffer[i]!=' ' && receive_buffer[i]!='\n';i++)
                                {
                                    temp[j++]=receive_buffer[i];
                                }
                                if(receive_buffer[i]==' ')
                                i++;
                                temp[j]='\0';
                                int c_id=atoi(temp);
                                removefromgroup(sd,g_id,c_id);
                                

                            }
                            char message[100];
                            sprintf(message,"Group %d members removed successfully\n",g_id);
                            bzero(send_buffer,1024);
                            strcpy(send_buffer,message);
                            send(sd,send_buffer,sizeof(send_buffer),0);
                            
                        }
                    }
                    else
                    {
                        
                        sprintf(send_buffer,"You can't remove  members as you are not admin to group:%d\n",g_id);
                        send(sd,send_buffer,sizeof(send_buffer),0);

                    }

                }
                
                else if(strncmp(receive_buffer,"/approveadminreq",16)==0)
                {
                    approveadminreq(sd);

                }
                else if(strncmp(receive_buffer,"/declineadminreq",16)==0)
                {
                    declineadminreq(sd);
                }
                
                //Echo back the message that came in
                else
                {
                    //set the string terminating NULL byte on the end
                    //of the data read
                    
                    invalidcommand(sd);
                }
            }
        }
    }
        
    return 0;
}






