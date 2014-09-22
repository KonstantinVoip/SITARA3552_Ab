//#include "/mnt/SHARE/SITARA3552_Ab/target_board/app/linux/sip/inc/Abarm_sip_test.h"
#include "/mnt/SHARE/SITARA3552_Ab/target_board/app/linux/sip/inc/sip_call.h"
     int main (void) {
  struct sip_call scall1;
  //////////////////////////////////////////////////////////////////////////////////////////////////////

  scall1.V = VYZOV_STATE_INIT;
  scall1.sip_state = SIP_SESSION_STATE_START;

  scall1.buf_point = 0;
  scall1.call_end = 0;

  strcpy(scall1.REMOTE_SIP_IPADDR, "172.24.3.1");
  scall1.LOCAL_SIP_PORT = 5060;
  scall1.LOCAL_RTP_PORT = 22222;
  scall1.REMOTE_SIP_PORT = 5060;


  if ((scall1.sudp = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1) err("socket");
 
  /* Set Port = LOCAL_PORT, leaving IP address = Any */
    bzero(&scall1.sin1, sizeof(struct sockaddr_in));
    scall1.sin1.sin_family = AF_INET;
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
	scall1.clnt_data_addr.sin_port = htons(scall1.LOCAL_RTP_PORT);

  /* Bind the socket */
    bind(scall1.RTP_data_sock, (struct sockaddr *)&scall1.clnt_data_addr, sizeof(scall1.clnt_data_addr));
    fcntl(scall1.RTP_data_sock, F_SETFL, O_NONBLOCK); 

///////////////////////////////////////////////////////////////////////////////////////////////////////

  while (1) {
    main_sip_call ("toto", "", "192.168.255.52", "5060", "22222", "172.24.3.1", "5060", "*60", "bu811.wav", "input1.wav", &scall1);
  }
}















