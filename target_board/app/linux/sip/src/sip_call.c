#include "/mnt/SHARE/SITARA3552_Ab/target_board/app/linux/sip/inc/digcalc.h"
#include "/mnt/SHARE/SITARA3552_Ab/target_board/app/linux/sip/inc/sip_call.h"

typedef struct
{
    int mess_code;
    char mess_command[20];
    char mess_from[255];
    char mess_via[255];
    char mess_to[255];
    char mess_call_id[255];
    char mess_cseq[255];
    char mess_server[255];
    char mess_allow[255];
    char mess_supported[255];
    char mess_contact[255];
    char mess_auth[255];
    int mess_content_lenght;
} SIP_message;

char buf[BUFSIZE];
char payload_buf[BUFSIZE];
char filebuf[FILEBUFSIZE];
/*char wavbuf[WAV_HEADER];*/

char SIP_mess[BUFSIZE];
char SIP_field[255];
char Phone_num[4];
char Own_num[4];
char Own_pass[20];
char realm[255];
char nonce[255];
char Auth_String[255];
char CallID[255];
char OFilename[255];
char IFilename[255];

SIP_message SIP_ans_mess;

FILE *ofile;

char* CreateInvite(char* own_acc, char* acc, char* auth_str, char* ownip, char* ownsipport, char* ownrtpport, char* serverip);
char* CreateAuthString(char* own_acc, char* own_pass, char* acc, char* serverip);
char* CreateAck(char* acc, char* serverip);
char* Create200();
char* GetField(char* mess, char* FName);

void Vyzov_sip(char* ownnumber, char* ownpass, char* ownip, char* ownsipport, char* ownrtpport, char* serverip, char* serversipport, char* phonenum, char* ifname, struct sip_call *scall);

int main_sip_call(char* ownnumber, char* ownpass, char* ownip, char* ownsipport, char* ownrtpport, char* serverip, char* serversipport, char* phonenum, char* ofname, char* ifname, struct sip_call *scall);

SIP_message* SIP_parser(char* mess, int mess_len);

char rtppacket[BUFSIZE];
char rtpPacketHeader[12];

struct sockaddr_in serv_data_addr;

int bytes;
char *mess;
int data_len;
int CSeq =1;
int c, y;
int fsize;

char* CreateInvite(char* own_acc, char* acc, char* auth_str, char* ownip, char* ownsipport, char* ownrtpport, char* serverip)
{
  int mLen;
  int mLen1;

  char SDP_mess[BUFSIZE];

  bzero (&SDP_mess, BUFSIZE);
  bzero (&SIP_mess, BUFSIZE);

  mLen = 0;
  sprintf(&SDP_mess[mLen], "v=0\r\n");
  mLen = strlen(SDP_mess);
  sprintf(&SDP_mess[mLen], "o=LONIIS_test 1234567890 1234567890 IN IP4 %s\r\n", ownip);
  mLen = strlen(SDP_mess);
  sprintf(&SDP_mess[mLen], "s=LONIIS\r\n");
  mLen = strlen(SDP_mess);
  sprintf(&SDP_mess[mLen], "c=IN IP4 %s\r\n", ownip);
  mLen = strlen(SDP_mess);
  sprintf(&SDP_mess[mLen], "t=0 0\r\n");
  mLen = strlen(SDP_mess);
  sprintf(&SDP_mess[mLen], "m=audio %s RTP/AVP 0 101\r\n", ownrtpport);
  mLen = strlen(SDP_mess);
  sprintf(&SDP_mess[mLen], "a=rtpmap:0 PCMU/8000\r\n");
  mLen = strlen(SDP_mess);
  sprintf(&SDP_mess[mLen], "a=sendrecv\r\n");
  mLen = strlen(SDP_mess);
  sprintf(&SDP_mess[mLen], "a=rtpmap:101 telephone-event/8000\r\n");
  mLen = strlen(SDP_mess);
  sprintf(&SDP_mess[mLen], "a=fmtp:101 0-15\r\n\r\n");

  mLen1 = strlen(SDP_mess);
  mLen = 0;

  sprintf(&SIP_mess[mLen], "INVITE sip:%s@%s SIP/2.0\r\n", acc, serverip);
  mLen = strlen(SIP_mess);
  sprintf(&SIP_mess[mLen], "Via: SIP/2.0/UDP %s:%s;branch=z9hG4bK%x%i;rport\r\n", ownip, ownsipport, CallID, random());
  mLen = strlen(SIP_mess);
  sprintf(&SIP_mess[mLen], "From: \"%s\" <sip:%s@%s>;tag=123456789123456789\r\n", own_acc, own_acc, serverip);
  mLen = strlen(SIP_mess);
  sprintf(&SIP_mess[mLen], "Max-Forwards: 70\r\n");
  mLen = strlen(SIP_mess);
  sprintf(&SIP_mess[mLen], "To: sip:%s@%s\r\n", acc, serverip);
  mLen = strlen(SIP_mess);
  sprintf(&SIP_mess[mLen], "Contact: <sip:%s@%s>\r\n", own_acc, ownip);
  mLen = strlen(SIP_mess);
  sprintf(&SIP_mess[mLen], "Call-ID: %x\r\n", CallID);
  mLen = strlen(SIP_mess);
  sprintf(&SIP_mess[mLen], "CSeq: %d INVITE\r\n", CSeq);
  mLen = strlen(SIP_mess);
  sprintf(&SIP_mess[mLen], "Allow: PRACK, INVITE, ACK, BYE, CANCEL, UPDATE\r\n");
  mLen = strlen(SIP_mess);
  sprintf(&SIP_mess[mLen], "Supported: 100rel\r\n");
  mLen = strlen(SIP_mess);
  sprintf(&SIP_mess[mLen], "Content-Type: application/sdp\r\n");
  mLen = strlen(SIP_mess);
  sprintf(&SIP_mess[mLen], "Content-Length: %d\r\n", mLen1);
  mLen = strlen(SIP_mess);
  sprintf(&SIP_mess[mLen], "%s\r\n\r\n", auth_str);
  mLen = strlen(SIP_mess);

  memcpy(&SIP_mess[mLen], SDP_mess, strlen(SDP_mess));

  return SIP_mess;
}

char* CreateAuthString(char* own_acc, char* own_pass, char* acc, char* serverip)
{

  char uri[255];

  bzero(&Auth_String, sizeof(Auth_String));

  HASHHEX HA1;
  HASHHEX HA2 = "";
  HASHHEX Response;
  char szNonceCount[9] = "00000001";

  sprintf(uri,"sip:%s@%s", acc, serverip);

  DigestCalcHA1("MD5", own_acc, realm, own_pass, nonce, "", HA1);
  DigestCalcResponse(HA1, nonce, szNonceCount, "", "", "INVITE", uri, HA2, Response);

  sprintf(Auth_String, "Authorization: Digest username=\"%s\",realm=\"%s\",nonce=\"%s\",uri=\"%s\",response=\"%s\",algorithm=MD5",own_acc, realm, nonce,uri,Response);

  return Auth_String;
}


char* CreateAck(char* acc, char* serverip)
{
  int mLen;
  bzero ((char *)&SIP_mess, sizeof (SIP_mess));

  mLen = 0;
  sprintf(&SIP_mess[mLen], "ACK sip:%s@%s SIP/2.0\r\n", acc, serverip);
  mLen = strlen(SIP_mess);
  sprintf(&SIP_mess[mLen], "Via:%s\r\n", SIP_ans_mess.mess_via);
  mLen = strlen(SIP_mess);
  sprintf(&SIP_mess[mLen], "From:%s\r\n", SIP_ans_mess.mess_from);
  mLen = strlen(SIP_mess);
  sprintf(&SIP_mess[mLen], "Max-Forwards: 70\r\n");
  mLen = strlen(SIP_mess);
  sprintf(&SIP_mess[mLen], "To:%s\r\n", SIP_ans_mess.mess_to);
  mLen = strlen(SIP_mess);
  sprintf(&SIP_mess[mLen], "Contact:%s\r\n", SIP_ans_mess.mess_contact);
  mLen = strlen(SIP_mess);
  sprintf(&SIP_mess[mLen], "Call-ID:%s\r\n", SIP_ans_mess.mess_call_id);
  mLen = strlen(SIP_mess);
  sprintf(&SIP_mess[mLen], "CSeq:%s\r\n", SIP_ans_mess.mess_cseq);
  mLen = strlen(SIP_mess);
  sprintf(&SIP_mess[mLen], "Content-Length:   %d\r\n\r\n", SIP_ans_mess.mess_content_lenght);

  return SIP_mess;
}

char* Create200()
{
  int mLen;
  bzero (SIP_mess, sizeof (SIP_mess));

  mLen = 0;
  sprintf(&SIP_mess[mLen], "SIP/2.0 200 OK\r\n");
  mLen = strlen(SIP_mess);
  sprintf(&SIP_mess[mLen], "Via:%s\r\n", SIP_ans_mess.mess_via);
  mLen = strlen(SIP_mess);
  sprintf(&SIP_mess[mLen], "From:%s\r\n", SIP_ans_mess.mess_from);
  mLen = strlen(SIP_mess);
  sprintf(&SIP_mess[mLen], "To:%s\r\n", SIP_ans_mess.mess_to);
  mLen = strlen(SIP_mess);
  sprintf(&SIP_mess[mLen], "Contact:%s\r\n", SIP_ans_mess.mess_contact);
  mLen = strlen(SIP_mess);
  sprintf(&SIP_mess[mLen], "Call-ID:%s\r\n", SIP_ans_mess.mess_call_id);
  mLen = strlen(SIP_mess);
  sprintf(&SIP_mess[mLen], "CSeq:%s\r\n", SIP_ans_mess.mess_cseq);
  mLen = strlen(SIP_mess);
  sprintf(&SIP_mess[mLen], "Content-Length:   %d\r\n\r\n", SIP_ans_mess.mess_content_lenght);

  return SIP_mess;
}

char* GetField(char* mess, char* FName)
{
  char *m = mess;
  char *r1, *r2, *r3;
  int n1, n2, n3;

  bzero (&SIP_field, sizeof (SIP_field));

  r1 = strstr(m,FName);
  n1 = r1 - m + 1;

  if (r1)
  {
    r2 = strstr(&m[n1], ":");
    n2 = r2 - &m[n1] + 1;
    r3 = strstr(&m[n1+n2], "\r");
    n3 = r3 - &m[n1+n2] + 1;
    strncpy(SIP_field, &m[n1+n2], n3);
  }

  return SIP_field;
}

char* GetParam(char* mess, char* PName)
{
  char *m = mess;
  char *r1, *r2, *r3;
  int n1, n2, n3;

  bzero (&SIP_field, sizeof (SIP_field));

  r1 = strstr(m,PName);
  n1 = r1 - m + 1;

  if (r1)
  {
    r2 = strstr(&m[n1], "\"");
    n2 = r2 - &m[n1] + 1;
    r3 = strstr(&m[n1+n2], "\"");
    n3 = r3 - &m[n1+n2];
    strncpy(SIP_field, &m[n1+n2], n3);
  }

  return SIP_field;
}

SIP_message* SIP_parser(char* mess, int mess_len) {
  char *SIPField;
  char *m = mess;
  char *r = strstr(m,"SIP");
  char mcode[3];
  int n;

  bzero (&SIP_ans_mess, sizeof (SIP_ans_mess));

  if (r)
  {
    r = strstr(m, " ");
    n = r - m + 1;

    bzero (mcode, 3);
    strncpy(mcode, &m[n], MESS_CODE_LEN);
    SIP_ans_mess.mess_code = atoi(mcode);

    if (SIP_ans_mess.mess_code == 0)
      strncpy(SIP_ans_mess.mess_command, &m[0], n -1);


    SIPField = GetField(mess, "Via");
    strcpy(SIP_ans_mess.mess_via, SIPField);

    SIPField = GetField(mess, "From");
    strcpy(SIP_ans_mess.mess_from, SIPField);

    SIPField = GetField(mess, "To");
    strcpy(SIP_ans_mess.mess_to, SIPField);

    SIPField = GetField(mess, "Call-ID");
    strcpy(SIP_ans_mess.mess_call_id, SIPField);

    SIPField = GetField(mess, "Server");
    strcpy(SIP_ans_mess.mess_server, SIPField);

    SIPField = GetField(mess, "CSeq");
    strcpy(SIP_ans_mess.mess_cseq, SIPField);

    SIPField = GetField(mess, "Allow");
    strcpy(SIP_ans_mess.mess_allow, SIPField);

    SIPField = GetField(mess, "Supported");
    strcpy(SIP_ans_mess.mess_supported, SIPField);

    SIPField = GetField(mess, "Contact");
    strcpy(SIP_ans_mess.mess_contact, SIPField);

    SIPField = GetField(mess, "WWW-Authenticate");
    strcpy(SIP_ans_mess.mess_auth, SIPField);

    SIPField = GetField(mess, "Content-Lenght");
    SIP_ans_mess.mess_content_lenght = atoi(SIPField);

  }

  return &SIP_ans_mess;
}

void Vyzov_sip(char* ownnumber, char* ownpass, char* ownip, char* ownsipport, char* ownrtpport, char* serverip, char* serversipport, char* phonenum, char* ifname, struct sip_call *scall)
{
	switch((*scall).sip_state)
	{
		case SIP_SESSION_STATE_START:

		strcpy(buf, CreateInvite(ownnumber, phonenum, Auth_String, ownip, ownsipport, ownrtpport, serverip));
		bytes = send ((*scall).sudp, buf, strlen(buf), 0);
		
		if (bytes > 0)
		{
			printf("Invite sended\n");
			
			(*scall).sip_state = SIP_SESSION_STATE_INVITE_SENDED;
		}
    	break;	
			
    	case SIP_SESSION_STATE_INVITE_SENDED:

		
   		if ((bytes = recv((*scall).sudp, (char *)buf, BUFSIZE, 0)) < 0)
   		{
   			//printf("No SIP message, %d\n", fdError());
   		} else
   		{
   			printf("SIP Packet received\n");
   			mess = buf;
   			SIP_parser(mess, bytes);
   			printf("Code = %d\n",SIP_ans_mess.mess_code );
   			if (SIP_ans_mess.mess_code == 100)
   			{
   				///
   			}
   			else if (SIP_ans_mess.mess_code == 180)
   			{
   				///
   			}
   			else if (SIP_ans_mess.mess_code == 401)
   			{
				CSeq ++;
				strcpy(realm, GetParam(SIP_ans_mess.mess_auth, "realm"));
				strcpy(nonce, GetParam(SIP_ans_mess.mess_auth, "nonce"));
				strcpy(buf, CreateInvite(ownnumber, phonenum, CreateAuthString(ownnumber, ownpass, phonenum, serverip), ownip, ownsipport, ownrtpport, serverip));
				bytes = send ((*scall).sudp, buf, strlen(buf), 0);
   			}

   			else if (SIP_ans_mess.mess_code == 486)
   			{
   				printf("Busy here\n");
   				bzero (&buf, BUFSIZE);
   				strcpy(buf, CreateAck(phonenum, serverip));
   				bytes = send ((*scall).sudp, (char *)&buf, strlen(buf), 0);
   				(*scall).sip_state = SIP_SESSION_STATE_BYE_COMPLETED;
				(*scall).return_code = 2;
			}

   			else if (SIP_ans_mess.mess_code == 200)
   			{
   				printf("Call\n");
				strcpy(IFilename, ifname);
				(*scall).ifile = fopen(IFilename,"w");
				
				y = 0;
				
				/*while (y < WAV_HEADER) 
				{
					fputc(wavbuf[y++], (*scall).ifile);
				}*/

			
   				bzero (&buf, BUFSIZE);
   				strcpy(buf, CreateAck(phonenum, serverip));
   				bytes = send ((*scall).sudp, (char *)&buf, strlen(buf), 0);
   				(*scall).sip_state = SIP_SESSION_STATE_INVITE_COMPLETED;
   			}
			else 
			{
				printf("Error, code = %d\n", SIP_ans_mess.mess_code);
   				bzero (&buf, BUFSIZE);
   				strcpy(buf, CreateAck(phonenum, serverip));
   				bytes = send ((*scall).sudp, (char *)&buf, strlen(buf), 0);
   				(*scall).sip_state = SIP_SESSION_STATE_BYE_COMPLETED;
				(*scall).return_code = 2;
			}
   		}
   	break;

   	case SIP_SESSION_STATE_INVITE_COMPLETED:

	bzero (&payload_buf, BUFSIZE);
   	data_len = sizeof(serv_data_addr);
   	bytes = recvfrom((*scall).RTP_data_sock, (char *)rtppacket, BUFSIZE, 0, &serv_data_addr, &data_len);
   	if (bytes > 0 && (*scall).mess_trans == 0)
	{
		memcpy(rtpPacketHeader, rtppacket, 12);
		bzero (&rtppacket, BUFSIZE);
		y = 0;
		memcpy(rtppacket, rtpPacketHeader, 12);
		while ((*scall).buf_point < fsize && y < 160) 
		{
			(*scall).buf_point=(*scall).buf_point+1;
			rtppacket[12+y++] = filebuf[(*scall).buf_point];
		}
		if ((*scall).buf_point == fsize) 
		{
			(*scall).mess_trans = 1;
		}

		sendto((*scall).RTP_data_sock, (char *)&rtppacket, 172 /*bytes*/, 0, &serv_data_addr, sizeof(serv_data_addr));
		} else 
   		if (bytes > 0 && (*scall).mess_trans == 1)
		{
			y = 0;
			while (y < 160) 
			{
				(*scall).buf_point = rtppacket[12+y++];
				fputc((*scall).buf_point, (*scall).ifile);
			}
		}

	  	if ((bytes = recv((*scall).sudp, (char *)buf, BUFSIZE, 0)) < 0)
	  	{
	  		//printf("No SIP message, %d\n", fdError());
	 	} else
	  	{	
			mess = buf;
	  		SIP_parser(mess, bytes);
	  		if (!strcmp(SIP_ans_mess.mess_command,"BYE"))
	  		(*scall).sip_state = SIP_SESSION_STATE_BYE_SENDED;
	  	}
	   	break;
	   		
	   	case SIP_SESSION_STATE_BYE_SENDED:

		fclose((*scall).ifile);	
	  	bzero (&buf, BUFSIZE);
	   	strcpy(buf, Create200());
	  	bytes = send ((*scall).sudp, (char *)&buf, strlen(buf), 0);
	   	if (bytes > 0)
	    	{
	    		printf("BYE\n");
	    		(*scall).sip_state = SIP_SESSION_STATE_BYE_COMPLETED;
	    	}
			
		break;

        	case SIP_SESSION_STATE_BYE_COMPLETED:

		close((*scall).sudp);
 		close((*scall).RTP_data_sock);
		(*scall).call_end = 1;

        	break;
	}
}

int main_sip_call(char* ownnumber, char* ownpass, char* ownip, char* ownsipport, char* ownrtpport, char* serverip, char* serversipport, char* phonenum, char* ofname, char* ifname, struct sip_call *scall)
{

/*"Usage : <Own Number> <Own pass> <Local IP> <Local SIP Port> <Local RTP Port> <Server IP> <Server SIP Port> <PhoneNumber> <OutputFilename> <InputFilename> <Vyzov>"*/

	int  fStop=0;

  switch((*scall).V)

  {
       	case VYZOV_STATE_INIT:

	y = 0;
	(*scall).return_code = 0;
	    
	strcpy(OFilename, ofname);
	ofile = fopen(OFilename,"r");

	while (ofile == 0 && y < 5)
	{
		y++;			
		printf("File opening... Try %d\n", y);
		sleep(5);
		ofile = fopen(OFilename,"r");
	}
	
	if (ofile == 0)
	{
		printf("Can not open file '%s'\n",OFilename);
		(*scall).return_code = 1;
		
	}

	fseek(ofile, 0, SEEK_END);
	fsize = ftell(ofile);
	fseek(ofile, 0, SEEK_SET);

	c = 0;
	y = 0;

	if (fsize > WAV_HEADER)
	{
		/*while (y < WAV_HEADER) 
		{
			c = fgetc(ofile);
			wavbuf[y++] = c;
		}*/
	
		y = 0;
	
		while (c != EOF) 
		{
			c = fgetc(ofile);
			filebuf[y++] = c;
		}
	}
	else 
	{
		printf("File '%s' corrupted\n",OFilename);
		(*scall).return_code = 1;
		
	}

	fclose(ofile);	
	(*scall).call_end = 0;
	(*scall).mess_trans = 0;  
	strcpy(CallID,"loniis");
	(*scall).V = VYZOV_STATE_START;

       	break;

	case VYZOV_STATE_START:

	if ((*scall).call_end!=1) Vyzov_sip(ownnumber, ownpass, ownip, ownsipport, ownrtpport, serverip, serversipport, phonenum, ifname, scall);
	else (*scall).V = VYZOV_STATE_CLOSE;

	break;

	case VYZOV_STATE_CLOSE:

	(*scall).V = VYZOV_STATE_END;
	
	break;

	case VYZOV_STATE_END:
		fStop=1;
	break;


  } 
	return fStop;

}

int main_sip_call_Helen(char* ownnumber, char* ownpass, char* ownip, char* ownsipport, char* ownrtpport, char* serverip, char* serversipport, char* phonenum, char* ofname, char* ifname)
{
	struct sip_call scall1; 

	scall1.return_code=-1;

	scall1.V = VYZOV_STATE_INIT;

	scall1.sip_state = SIP_SESSION_STATE_START;

	
scall1.buf_point = 0;

	scall1.call_end = 0;

	
strcpy(scall1.REMOTE_SIP_IPADDR, serverip);

	scall1.LOCAL_SIP_PORT = atoi(ownsipport);

	scall1.LOCAL_RTP_PORT = atoi(ownrtpport);

	scall1.REMOTE_SIP_PORT = atoi(serversipport);
	if ((scall1.sudp = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
        
		err("socket");
 
  

	/* Set Port = LOCAL_PORT, leaving IP address = Any */

	bzero(&scall1.sin1, sizeof(struct sockaddr_in));
    
	scall1.sin1.sin_family = AF_INET;
    
	scall1.sin1.sin_addr.s_addr = inet_addr(ownip); 
    
	scall1.sin1.sin_port = htons(scall1.LOCAL_SIP_PORT);

   
 
	/* Bind the socket */
    
	bind (scall1.sudp, (struct sockaddr *)&scall1.sin1, sizeof(scall1.sin1));
    
	fcntl(scall1.sudp, F_SETFL, O_NONBLOCK); 

    

	/* Set Port = REMOTE_PORT, IP address = REMOTE_IPADDR */
    
	bzero(&scall1.sin1, sizeof(struct sockaddr_in));
    
	scall1.sin1.sin_family = AF_INET;
    
	scall1.sin1.sin_addr.s_addr = inet_addr(scall1.REMOTE_SIP_IPADDR); 
    
	scall1.sin1.sin_port = htons(scall1.REMOTE_SIP_PORT);

    

	connect (scall1.sudp, &scall1.sin1, sizeof(scall1.sin1));   

    

	if ((scall1.RTP_data_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)

        	err("socket");


	
	// Set Port = LOCAL_RTP_PORT, leaving IP address = Any
	
	bzero(&scall1.clnt_data_addr, sizeof(struct sockaddr_in));
	
	scall1.clnt_data_addr.sin_family = AF_INET;

	scall1.clnt_data_addr.sin_addr.s_addr = inet_addr(ownip); 	
	scall1.clnt_data_addr.sin_port = htons(scall1.LOCAL_RTP_PORT);

  

	/* Bind the socket */
    
	bind(scall1.RTP_data_sock, (struct sockaddr *)&scall1.clnt_data_addr, sizeof(scall1.clnt_data_addr));
    
	fcntl(scall1.RTP_data_sock, F_SETFL, O_NONBLOCK); 

	int fStop=0;
	while (!fStop) 

	{
		fStop=
main_sip_call (ownnumber, ownpass, ownip, ownsipport, ownrtpport, serverip, serversipport, phonenum, ofname, ifname, &scall1);
	

} 

	return scall1.return_code; 

}