#pragma once

#define SWMAGIC 5
#define SWCONSTVALUE 1

#pragma pack(push, 1)

typedef struct _SWHEADER { // 7 bytes
	USHORT _magic; // key udentifier
	USHORT _size;
	UCHAR _const_value01;
	USHORT _op;
} SWHEADER;

#pragma pack(pop)

class SWPacket : public MemoryPool<SWPacket, 10> {
protected:
	SWHEADER* _swheader;
	BYTE* _data;

	SWPacket() {}
	SWPacket(SWPacket& other) {}

public:
	SWPacket(SWHEADER* swheader, BYTE* data);
	virtual ~SWPacket() {}

	virtual VOID Debug();
	virtual VOID Log();
	virtual VOID Do();
};