//#include "/mnt/SHARE/SITARA3552_Ab/target_board/app/linux/sip/inc/Abarm_sip_test.h"
#include "/mnt/SHARE/SITARA3552_Ab/target_board/app/linux/sip/inc/sip_call.h"
     

/*                                                           
int start_sip_test()
{


printf("START SIP TESTING  ROUTER\n\r");

}
*/





//#if 0

//int start_sip_test()
int main (void)
{


struct sip_call scall1;
struct sip_call scall2;

//////////////////////////////////////////////////////////////////////////////////////////////////////
scall1.V = VYZOV_STATE_INIT;
scall1.sip_state = SIP_SESSION_STATE_START;

scall1.buf_point = 0;
scall1.call_end = 0;

strcpy(scall1.REMOTE_SIP_IPADDR, "192.168.255.134");
scall1.LOCAL_SIP_PORT = 5060;
scall1.LOCAL_RTP_PORT = 22222;
scall1.REMOTE_SIP_PORT = 5060;


    if ((scall1.sudp = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
        err("socket");
 
    // Set Port = LOCAL_PORT, leaving IP address = Any */
    bzero(&scall1.sin1, sizeof(struct sockaddr_in));
    scall1.sin1.sin_family = AF_INET;
    scall1.sin1.sin_port = htons(scall1.LOCAL_SIP_PORT);

    // Bind the socket 
    bind (scall1.sudp, (struct sockaddr *)&scall1.sin1, sizeof(scall1.sin1));
    fcntl(scall1.sudp, F_SETFL, O_NONBLOCK); 

    // Set Port = REMOTE_PORT, IP address = REMOTE_IPADDR */
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
	scall1.clnt_data_addr.sin_port = htons(scall1.LOCAL_RTP_PORT);

    // Bind the socket */
    bind(scall1.RTP_data_sock, (struct sockaddr *)&scall1.clnt_data_addr, sizeof(scall1.clnt_data_addr));
    fcntl(scall1.RTP_data_sock, F_SETFL, O_NONBLOCK); 

///////////////////////////////////////////////////////////////////////////////////////////////////////

scall2.V = VYZOV_STATE_INIT;
scall2.sip_state = SIP_SESSION_STATE_START;

scall2.call_end = 0;
scall2.buf_point = 0;

strcpy(scall2.REMOTE_SIP_IPADDR, "192.168.255.134");
scall2.LOCAL_SIP_PORT = 5060;
scall2.LOCAL_RTP_PORT = 22223;
scall2.REMOTE_SIP_PORT = 5060;


    if ((scall2.sudp = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
        err("socket");
 
    // Set Port = LOCAL_PORT, leaving IP address = Any
    bzero(&scall2.sin1, sizeof(struct sockaddr_in));
    scall2.sin1.sin_family = AF_INET;
    scall2.sin1.sin_port = htons(scall2.LOCAL_SIP_PORT);

    // Bind the socket 
    bind (scall2.sudp, (struct sockaddr *)&scall2.sin1, sizeof(scall2.sin1));
    fcntl(scall2.sudp, F_SETFL, O_NONBLOCK); 

    // Set Port = REMOTE_PORT, IP address = REMOTE_IPADDR
    bzero(&scall2.sin1, sizeof(struct sockaddr_in));
    scall2.sin1.sin_family = AF_INET;
    scall2.sin1.sin_addr.s_addr = inet_addr(scall2.REMOTE_SIP_IPADDR); 
    scall2.sin1.sin_port = htons(scall2.REMOTE_SIP_PORT);

    connect (scall2.sudp, &scall2.sin1, sizeof(scall2.sin1));   

    if ((scall2.RTP_data_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
        err("socket");

	// Set Port = LOCAL_RTP_PORT, leaving IP address = Any
	bzero(&scall2.clnt_data_addr, sizeof(struct sockaddr_in));
	scall2.clnt_data_addr.sin_family = AF_INET;
	scall2.clnt_data_addr.sin_port = htons(scall2.LOCAL_RTP_PORT);

    // Bind the socket
    bind(scall2.RTP_data_sock, (struct sockaddr *)&scall2.clnt_data_addr, sizeof(scall2.clnt_data_addr));
    fcntl(scall2.RTP_data_sock, F_SETFL, O_NONBLOCK); 
    


///////////////////////////////////////////////////////////////////////////////////////////////////////
    
    //while (1) 
    //{
    
    main_sip_call_Helen ("215", "215test", "192.168.255.107", "5060", "22222", "192.168.255.134", "5060", "212", "bu811.wav", "input1.wav");
    //sleep(2);
    /*
    main_sip_call ("213", "213test", "192.168.255.107", "5060", "22222", "192.168.255.134", "5060", "214", "bu811.wav", "input2.wav", &scall2);
    //}

*/
}//END   FUNCTION MAIN

//#endif




