#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h> 
#include <string.h>
#include <fcntl.h>

#define BUFSIZE 1024
#define FILEBUFSIZE 512000
#define MESS_CODE_LEN 3
#define WAV_HEADER 54

typedef enum 
{
	VYZOV_STATE_INIT,
	VYZOV_STATE_START,
	VYZOV_STATE_CLOSE,
	VYZOV_STATE_END,
} VYZOV_STATE; 

typedef enum 
{
	SIP_SESSION_STATE_START,
	SIP_SESSION_STATE_INVITE_SENDED,
	SIP_SESSION_STATE_INVITE_COMPLETED,
	SIP_SESSION_STATE_BYE_SENDED,
	SIP_SESSION_STATE_BYE_COMPLETED,
} SIP_SESSION_STATE; 


struct sip_call
{
	char REMOTE_SIP_IPADDR[20];
	int LOCAL_SIP_PORT;
	int LOCAL_RTP_PORT;
	int REMOTE_SIP_PORT;
	VYZOV_STATE V;
	SIP_SESSION_STATE sip_state;
	int call_end;
	int sudp;
	int RTP_data_sock;
	struct sockaddr_in serv_data_addr, clnt_data_addr, sin1;
	FILE *ifile;
	int buf_point;
	int mess_trans;
	int return_code;
};


char* CreateInvite
(
	char* own_acc, 
	char* acc, 
	char* auth_str,
	char* ownip,
	char* ownsipport,
	char* ownrtpport,
	char* serverip
);

char* CreateAuthString
(
	char* own_acc, 
	char* own_pass, 
	char* acc,
	char* serverip
);

char* CreateAck
(
	char* acc,
	char* serverip
);

char* Create200();

char* GetField
(
	char* mess, 
	char* FName
);

void Vyzov_sip
(
	char* ownnumber,
	char* ownpass,
	char* ownip,
	char* ownsipport,
	char* ownrtpport,
	char* serverip,
	char* serversipport,
	char* phonenum,
	char* ifname,
	struct sip_call *scall
);int main_sip_call
(
	char* ownnumber, 
	char* ownpass, 
	char* ownip, 
	char* ownsipport, 
	char* ownrtpport, 
	char* serverip, 
	char* serversipport, 
	char* phonenum,
	char* ofname,
	char* ifname,
	struct sip_call *scall
);

int main_sip_call_Helen(char* ownnumber, char* ownpass, char* ownip, char* ownsipport, char* ownrtpport, char* serverip, char* serversipport, char* phonenum, char* ofname, char* ifname);
