#include "pch.h"
#include ".\Packet Capture\MyNpcap.h"
#include ".\Soulworker Packet\SWPacketMaker.h"

DWORD MyNpcap::LoadNpcapDlls() {

	DWORD error = ERROR_SUCCESS;

	do {
		TCHAR npcap_dir[MAX_PATH];

		UINT len;
		len = GetSystemDirectory(npcap_dir, MAX_PATH);

		if (!len) {
			Log::WriteLog(const_cast<LPTSTR>(_T("Error in GetSystemDirectory: %x")), GetLastError());
			error = ERROR_FUNCTION_FAILED;
			break;
		}

		lstrcatW(npcap_dir, L"\\Npcap");

		if (SetDllDirectory(npcap_dir) == 0) {
			Log::WriteLog(const_cast<LPTSTR>(_T("Error in SetDllDirectory: %x")), GetLastError());
			error = ERROR_FUNCTION_FAILED;
			break;
		}

	} while (false);

	return error;
}

DWORD MyNpcap::Filter(pcap_t* device) {

	DWORD error = ERROR_SUCCESS;

	do {

		if (device == nullptr) {
			error = ERROR_INVALID_PARAMETER;
			break;
		}

		bpf_program fcode;

		if (pcap_compile(device, &fcode, NPCAP_FILTER_RULE, 1, NULL) < 0) {
			error = ERROR_API_UNAVAILABLE;
			Log::WriteLog(const_cast<LPTSTR>(_T("Error in pcap_compile")));
			break;
		}

		if (pcap_setfilter(device, &fcode) < 0) {
			error = ERROR_API_UNAVAILABLE;
			Log::WriteLog(const_cast<LPTSTR>(_T("Error in pcap_setfilter")));
			break;
		}

	} while (false);

	return error;
}

DWORD MyNpcap::Init() {

	DWORD error = ERROR_SUCCESS;

	pcap_if_t* alldevs = nullptr;

	char temp[MAX_BUFFER_LENGTH] = { 0 };

	do {
		if (LoadNpcapDlls()) {
			error = ERROR_DLL_NOT_FOUND;
			break;
		}
		
		if (pcap_findalldevs_ex(const_cast<char*>(PCAP_SRC_IF_STRING), NULL, &alldevs, temp) == -1) {
			Log::WriteLogA(temp);
			error = ERROR_API_UNAVAILABLE;
			break;
		}

		for (pcap_if_t* d = alldevs; d != nullptr; d = d->next) {

			pcap_t* device = nullptr;

			if ((device = pcap_open(d->name, 65536, PCAP_OPENFLAG_PROMISCUOUS, 1000, NULL, temp)) == NULL)
				continue;

			if (Filter(device))
				continue;

			CreateThread(NULL, 0, ReceiveCallback, device, 0, NULL);
		}

		pcap_freealldevs(alldevs);

	} while (false);

	return error;
}

DWORD MyNpcap::ReceiveCallback(LPVOID prc) {

	DWORD error = ERROR_SUCCESS;

	do {

		if (prc == nullptr) {
			error = ERROR_INVALID_PARAMETER;
			break;
		}

		pcap_t* device = (pcap_t*)prc;

		int res;
		pcap_pkthdr* header = nullptr;
		const unsigned char* pkt_data = nullptr;

		IPv4Packet packet;

		while ((res = pcap_next_ex(device, &header, &pkt_data)) >= 0) {

			if (res == 0)
				continue;

			if (pkt_data == nullptr || header == nullptr)
				continue;

			if (header->caplen < 64)
				continue;

			// 이더넷 헤더 분리
			packet._ethernetHeader = (ETHERNETHEADER*)pkt_data;

			switch (packet._ethernetHeader->Type) {
			case 0x0008: { // IPv4

				// IP 헤더 분리
				packet._ipHeader = (IPHEADER*)(pkt_data + 14);
				packet._ipHeader->length = _byteswap_ushort(packet._ipHeader->length);

				// TCP 헤더 분리
				packet._tcpHeader = (TCPHEADER*)(pkt_data + 14 + packet._ipHeader->len * 4);
				packet._datalength = header->caplen - (14 + packet._ipHeader->len * 4 + packet._tcpHeader->length * 4);

				// TCP 세그먼트
				packet._data = (pkt_data + 14 + packet._ipHeader->len * 4 + packet._tcpHeader->length * 4);

#if DEBUG_NPCAP_ALL == 1
				PrintEthernetHeader(&packet);
				PrintIPHeader(&packet);
				PrintTCPHeader(&packet);

				printf("[Packet Data Start]\n");
				for (int i = 0; i < packet._datalength; i++)
					printf("%02x ", packet._data[i]);
				printf("\n");
				printf("[Packet Data End]\n");
#endif
#if DEBUG_NPCAP_ETHERNET == 1
				PrintEthernetHeader(&packet);
#endif
#if DEBUG_NPCAP_IP == 1
				PrintIPHeader(&packet);
#endif
#if DEBUG_NPCAP_TCP == 1
				PrintTCPHeader(&packet);
#endif
#if DEBUG_NPCAP_DATA == 1
				printf("[Packet Data Start]\n");
				for (int i = 0; i < packet._datalength; i++)
					printf("%02x ", packet._data[i]);
				printf("\n");
				printf("[Packet Data End]\n");
#endif
				// Todo
				SWPACKETMAKER.Parse(&packet);
			}
				break;
			default:
				break;
			}
		}

	} while (false);

	return error;
}

VOID MyNpcap::PrintEthernetHeader(IPv4Packet* p_packet) {

	if (p_packet == nullptr)
		return;

	ETHERNETHEADER* eh = p_packet->_ethernetHeader;

	printf("\n======== Ethernet Header ========\n");
	printf("Dst Mac %02x:%02x:%02x:%02x:%02x:%02x \n", eh->DesMac[0], eh->DesMac[1], eh->DesMac[2], eh->DesMac[3], eh->DesMac[4], eh->DesMac[5]);
	printf("Src Mac %02x:%02x:%02x:%02x:%02x:%02x \n", eh->SrcMac[0], eh->SrcMac[1], eh->SrcMac[2], eh->SrcMac[3], eh->SrcMac[4], eh->SrcMac[5]);
	printf("Type = %04x\n", eh->Type);
	return;
}

VOID MyNpcap::PrintIPHeader(IPv4Packet* p_packet) {

	if (p_packet == nullptr)
		return;

	IPHEADER* ih = p_packet->_ipHeader;

	printf("======== IP Header ========\n");
	printf("Version : %d\n", ih->version);
	printf("len : %d\n", ih->len * 4);
	printf("length : %d\n", ih->length);
	printf("TTL : %d\n", ih->ttl);
	printf("protocol : %d\n", ih->protocol);
	printf("Src IP  : %d.%d.%d.%d\n", ih->src_ip & 0xFF, (ih->src_ip >> 8) & 0xFF, (ih->src_ip >> 16) & 0xF, (ih->src_ip >> 24) & 0xFF);
	printf("Dst IP  : %d.%d.%d.%d\n", ih->dest_ip & 0xFF, (ih->dest_ip >> 8) & 0xFF, (ih->dest_ip >> 16) & 0xF, (ih->dest_ip >> 24) & 0xFF);

	return;
}

VOID MyNpcap::PrintTCPHeader(IPv4Packet* p_packet) {

	if (p_packet == nullptr)
		return;

	TCPHEADER* th = p_packet->_tcpHeader;

	printf("======== TCP Header ========\n");
	printf("src_port : %u\n", _byteswap_ushort(th->src_port));
	printf("dest_port : %u\n", _byteswap_ushort(th->dest_port));
	printf("length : %d\n", th->length * 4);
}