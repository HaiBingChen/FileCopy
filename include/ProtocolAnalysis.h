//
// Created by user on 2021/10/7.
//

#ifndef FILECOPYSERVER_PROTOCOLANALYSIS_H
#define FILECOPYSERVER_PROTOCOLANALYSIS_H

/*
* 数据包格式
*     1Byte                1Byte         1Byte       1Byte       1Byte      0-65529 Bytes    1 Byte
* 包起始头MSB(0xFF) | 包起始头LSB(0x5A) | 包长度MSB | 包长度LSB | 头部检验和 | ... 负载数据 ...| 负载检验和
*/

#define kMaxPackectSize      65535
#define kPacketHeaderLen     5
#define kPacketChksumLen     1
#define kPacketPayloadChksum 1

#define kMaxPayLoadSize (kMaxPackectSize - kPacketHeaderLen - kPacketPayloadChksum)

#define kPacketStart1 0xFF
#define kPacketStart2 0x5A

typedef enum PacketParseState {
    PacketParseStateHead1 = 0,
    PacketParseStateHead2,
    PacketParseStateLEN1,
    PacketParseStateLEN2,
    PacketParseStateCHK,
    PacketParseStatePAYLOAD,
    PacketParseStatePAYLOADCHK,
    PacketParseStateFINISH,
} PacketParseState;

typedef struct PacketData {
    unsigned char head1;
    unsigned char head2;
    unsigned char len1;
    unsigned char len2;
    unsigned char chk;
    unsigned char data[1];
} PacketData;

typedef struct Packet {
    unsigned int bufferLen;
    unsigned int packetLen;
    unsigned char dataChecksum;
    unsigned char sendPacket;
    unsigned int dataCurLen;
    PacketParseState state: 8;
    PacketData *pckData;
} Packet;

unsigned int ProcessRecvData(int socket_fd, unsigned char *readBuffer, unsigned int bufferLen);
void CreatePacket(unsigned char *in_payload, unsigned int in_payload_len, unsigned char **out_packet, unsigned int *out_len);

#endif //FILECOPYSERVER_PROTOCOLANALYSIS_H
