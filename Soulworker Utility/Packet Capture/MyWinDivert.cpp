#include "pch.h"
#include ".\Packet Capture\MyWinDivert.h"
#include ".\Soulworker Packet\SWPacketMaker.h"

DWORD MyWinDivert::Init() {

	DWORD error = ERROR_SUCCESS;

	do {
		_handle = WinDivertOpen(WINDIVERT_FILTER_RULE, WINDIVERT_LAYER_NETWORK, 0, WINDIVERT_FLAG_SNIFF);

		if (_handle == INVALID_HANDLE_VALUE) {
			Log::WriteLog(const_cast<LPTSTR>(_T("Error in WinDivertOpen: %x")), GetLastError());
			break;
		}

		CreateThread(NULL, 0, ReceiveCallback, this, 0, NULL);

	} while (false);

	return error;
}

DWORD MyWinDivert::ReceiveCallback(LPVOID prc) {

	DWORD error = ERROR_SUCCESS;

	do {

		if (prc == nullptr) {
			error = ERROR_INVALID_PARAMETER;
			break;
		}

		MyWinDivert* _this = (MyWinDivert*)prc;

		WINDIVERT_ADDRESS addr;
		UINT addrlen = sizeof(WINDIVERT_ADDRESS);

		BYTE* pkt_data = new BYTE[WINDIVERT_MTU_MAX];
		UINT packetlen = _msize(pkt_data);

		UINT recvlen = 0;

		IPv4Packet packet;

		while (TRUE) {

			if (!WinDivertRecvEx(_this->_handle, pkt_data, packetlen, &recvlen, 0, &addr, &addrlen, NULL)) {
				Log::WriteLog(const_cast<LPTSTR>(_T("Error in WinDivertRecv : %x")), GetLastError());
				continue;
			}

			// IP 헤더 분리
			packet._ipHeader = (IPHEADER*)(pkt_data);
			packet._ipHeader->length = _byteswap_ushort(packet._ipHeader->length);

			switch (packet._ipHeader->version) {
			case 4: { // IPv4

				// TCP 헤더 분리
				packet._tcpHeader = (TCPHEADER*)(pkt_data + packet._ipHeader->len * 4);
				packet._datalength = packet._ipHeader->length - (packet._ipHeader->len * 4 + packet._tcpHeader->length * 4);

				// TCP 세그먼트
				packet._data = (pkt_data + packet._ipHeader->len * 4 + packet._tcpHeader->length * 4);

#if DEBUG_DIVERT_ALL == 1
				PrintIPHeader(&packet);
				PrintTCPHeader(&packet);

				printf("[Packet Data Start]\n");
				for (int i = 0; i < packet._datalength; i++)
					printf("%02x ", packet._data[i]);
				printf("\n");
				printf("[Packet Data End]\n");
#endif
#if DEBUG_DIVERT_IP == 1
				PrintIPHeader(&packet);
#endif

#if DEBUG_DIVERT_TCP == 1
				PrintTCPHeader(&packet);
#endif
#if DEBUG_DIVERT_DATA == 1
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

VOID MyWinDivert::PrintIPHeader(IPv4Packet* p_packet) {

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

VOID MyWinDivert::PrintTCPHeader(IPv4Packet* p_packet) {

	if (p_packet == nullptr)
		return;

	TCPHEADER* th = p_packet->_tcpHeader;

	printf("======== TCP Header ========\n");
	printf("src_port : %u\n", _byteswap_ushort(th->src_port));
	printf("dest_port : %u\n", _byteswap_ushort(th->dest_port));
	printf("length : %d\n", th->length * 4);
}