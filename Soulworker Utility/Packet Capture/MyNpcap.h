#pragma once

#define NPCAP_FILTER_RULE "src port 10200"

#define DEBUG_NPCAP_ALL 0
#define DEBUG_NPCAP_ETHERNET 0
#define DEBUG_NPCAP_IP 0
#define DEBUG_NPCAP_TCP 0
#define DEBUG_NPCAP_DATA 0

#define NPCAP MyNpcap::getInstance()
 
class MyNpcap : public Singleton<MyNpcap> {
private:
	DWORD LoadNpcapDlls();
	DWORD Filter(pcap_t* device);

	static DWORD WINAPI ReceiveCallback(LPVOID prc);

	static VOID PrintEthernetHeader(IPv4Packet* p_packet);
	static VOID PrintIPHeader(IPv4Packet* p_packet);
	static VOID PrintTCPHeader(IPv4Packet* p_packet);

public:
	MyNpcap() {}
	~MyNpcap() {}

	DWORD Init();
};