//
// Created by user on 2021/10/7.
//

#include "ProtocolAnalysis.h"
#include "Sockect.h"
#include "malloc.h"
#include "assert.h"
#include "memory.h"

static void ResetPacket(Packet *pck) {
    pck->sendPacket = 0;
    pck->dataCurLen = 0;
    pck->dataChecksum = 0;
    pck->bufferLen = 0;
    pck->packetLen = 0;
    pck->state = PacketParseStateHead1;
    memset(pck->pckData, 0, sizeof(PacketData));
}

static Packet *CreateEmptyRecvPacket() {
    Packet *pck = (Packet *) malloc(sizeof(Packet));
    assert(pck);
    memset(pck, 0, sizeof(Packet));

    pck->pckData = (PacketData *) malloc(kMaxPackectSize);
    assert(pck->pckData);
    memset(pck->pckData, 0, kMaxPackectSize);

    ResetPacket(pck);
    return pck;
}

static unsigned char PacketCalcChecksum(const unsigned char *buffer,unsigned int bufferLen) {
    unsigned char checksum = 0;

    if (buffer != NULL && bufferLen > 0) {
        const unsigned char *bufferEnd = buffer + bufferLen;

        while (buffer != bufferEnd) {
            checksum += *buffer; /* add to checksum */
            buffer++;
        }

        checksum = (unsigned char) (0x100 - checksum); /* 2's complement negative checksum */
    }

    return checksum;
}

static unsigned char *PacketGetPayload(const Packet *packet) {
    return packet->pckData->data;
}

static unsigned short PacketGetPayloadLen(const Packet *packet) {
    unsigned short payloadLen = 0;
    if (packet->packetLen > kPacketHeaderLen) {
        payloadLen = packet->packetLen - kPacketHeaderLen - kPacketChksumLen;
    }
    return payloadLen;
}

static unsigned int PacketParseBuffer(const unsigned char *buffer,
                               unsigned int bufferLen,
                               Packet *packet,
                               unsigned int maxPacketSize) {
    unsigned int totParsedLen = 0;

    if (buffer != NULL && packet != NULL) {
        const unsigned char *pt = buffer;
        unsigned int ptLen = bufferLen;
        const unsigned char *ptEnd = pt + ptLen;
        unsigned short calcChecksum;

        while (pt < ptEnd && packet->state < PacketParseStateFINISH) {
            switch (packet->state) {
                case PacketParseStateHead1: {
                    packet->dataCurLen = 0;
                    packet->dataChecksum = 0;
                    packet->packetLen = 0;
                    packet->bufferLen = 0;

                    if (kPacketStart1 == *pt) {
                        packet->state = PacketParseStateHead2;
                        packet->pckData->head1 = *pt;
                        packet->bufferLen += 1;
                    }
                    totParsedLen += 1;
                    pt += 1;
                    ptLen -= 1;
                }
                    break;
                case PacketParseStateHead2: {
                    if (kPacketStart2 == *pt) {
                        packet->state = PacketParseStateLEN1;
                        packet->pckData->head2 = *pt;
                        packet->bufferLen += 1;
                    } else if (kPacketStart1 != *pt) {
                        packet->state = PacketParseStateHead1;
                    } else {

                    }
                    totParsedLen += 1;
                    pt += 1;
                    ptLen -= 1;
                }
                    break;
                case PacketParseStateLEN1: {
                    packet->pckData->len1 = *pt;
                    packet->packetLen = (((unsigned short) *pt) << 8);
                    packet->state = PacketParseStateLEN2;
                    packet->bufferLen += 1;
                    totParsedLen += 1;
                    pt += 1;
                    ptLen -= 1;
                }
                    break;
                case PacketParseStateLEN2: {
                    packet->pckData->len2 = *pt;
                    packet->packetLen += *pt;
                    if (((maxPacketSize == 0 || packet->packetLen <= maxPacketSize) &&
                         packet->packetLen >= kPacketHeaderLen)) {
                        packet->state = PacketParseStateCHK;
                        packet->bufferLen += 1;
                        totParsedLen += 1;
                        pt += 1;
                        ptLen -= 1;
                    } else {
                        /*
                        ** Parse is done on received packets only which are
                        ** allocated with maximum receive payload size so it
                        ** can be reused.
                        */
                        printf("[ERROR]%s: packet len error\n", __FUNCTION__);
                        ResetPacket(packet);
                    }
                }
                    break;
                case PacketParseStateCHK: {
                    packet->pckData->chk = *pt;
                    packet->state = PacketParseStatePAYLOAD;
                    packet->bufferLen += 1;
                    totParsedLen += 1;
                    pt += 1;
                    ptLen -= 1;

                    calcChecksum = PacketCalcChecksum((unsigned char *) packet->pckData,
                                                      kPacketHeaderLen - kPacketChksumLen);

                    if (calcChecksum == packet->pckData->chk) {
                        packet->pckData->chk = calcChecksum;
                        packet->dataCurLen = 0;
                        //No payload data
                        if (packet->packetLen == kPacketHeaderLen) {
                            packet->state = PacketParseStateFINISH;
                        }
                    } else {
                        printf("[ERROR]%s: packet checksum error\n", __FUNCTION__);
                        ResetPacket(packet);
                    }
                }
                    break;
                case PacketParseStatePAYLOAD: {
                    unsigned int bytesToCopy = PacketGetPayloadLen(packet) - packet->dataCurLen;
                    if (bytesToCopy > 0) {
                        /* Get some payload bytes */
                        if (ptLen < bytesToCopy) {
                            bytesToCopy = ptLen;
                        }

                        memcpy(&PacketGetPayload(packet)[packet->dataCurLen], pt, bytesToCopy);

                        packet->dataCurLen += bytesToCopy;
                        packet->bufferLen += bytesToCopy;
                        totParsedLen += bytesToCopy;
                        pt += bytesToCopy;
                        ptLen -= bytesToCopy;
                    }
                    if (packet->dataCurLen == PacketGetPayloadLen(packet)) {
                        packet->state = PacketParseStatePAYLOADCHK;
                    }
                }
                    break;
                case PacketParseStatePAYLOADCHK: {
                    PacketGetPayload(packet)[packet->dataCurLen] = *pt;
                    /* Don't include Checksum Byte in payload data len (dataCurLen). */
                    /*packet->dataCurLen += 1;*/
                    packet->bufferLen += 1;
                    totParsedLen += 1;
                    pt += 1;
                    ptLen -= 1;
                    if (packet->packetLen == packet->bufferLen) {
                        /* Got all the payload bytes */

                        /* Need to check payload checksum */
                        unsigned char *payload = PacketGetPayload(packet);
                        unsigned short payloadLen = PacketGetPayloadLen(packet);
                        packet->dataChecksum = PacketCalcChecksum(payload, payloadLen);
                        if (packet->dataChecksum == payload[payloadLen]) {
                            packet->state = PacketParseStateFINISH;
                        } else {
                            printf("[ERROR]%s: payload checksum error\n", __FUNCTION__);
                            ResetPacket(packet);
                        }
                    }
                }
                    break;
                case PacketParseStateFINISH:
                    break;
            } /* switch (packet->state) */
        } /* while (pt < ptEnd) */

    } /* if (buffer != NULL && bufferLen >= kIAP2PacketHeaderLen &&  packet != NULL) */

    return totParsedLen;
}

static void PacketDelete (Packet *pck)
{
    if((pck != NULL) && (pck->pckData != NULL)) {
        free(pck->pckData);
        free(pck);
    }
}

unsigned int ProcessRecvData(int socket_fd, unsigned char *readBuffer, unsigned int bufferLen) {
    unsigned int parsedLen = 0;
    Packet *parse_packet = NULL;

    while (bufferLen > parsedLen &&
           bufferLen <= kMaxPackectSize &&
           parsedLen <= kMaxPackectSize) {
        parse_packet = CreateEmptyRecvPacket();

        parsedLen += PacketParseBuffer(readBuffer + parsedLen,
                                       bufferLen - parsedLen,
                                       parse_packet,
                                       bufferLen - parsedLen);

        //Vaild packet
        if (PacketParseStateFINISH <= parse_packet->state) {
            ProcessPayloadData(socket_fd, parse_packet->pckData->data, parse_packet->dataCurLen);
            PacketDelete(parse_packet);
            parse_packet = NULL;
        } else {
            printf("[ERROR] Invaild packet");
            PacketDelete(parse_packet);
            break;
        }
    }
    return parsedLen;
}

void CreatePacket(unsigned char *in_payload, unsigned int in_payload_len, unsigned char **out_packet, unsigned int *out_len)
{
    if(in_payload_len > kMaxPayLoadSize) {
        *out_packet = NULL;
        *out_len = 0;
        return;
    }

    unsigned int packet_len = kPacketHeaderLen + in_payload_len + kPacketPayloadChksum;
    *out_packet = malloc(packet_len);
    (*out_packet)[0] = kPacketStart1;
    (*out_packet)[1] = kPacketStart2;
    (*out_packet)[2] = (packet_len >> 8) & 0xFF;
    (*out_packet)[3] = packet_len & 0xFF;
    (*out_packet)[4] = PacketCalcChecksum(*out_packet, kPacketHeaderLen - kPacketChksumLen);
    memcpy(*out_packet+5, in_payload, in_payload_len);
    (*out_packet)[packet_len-1] = PacketCalcChecksum(in_payload, in_payload_len);
    *out_len = packet_len;
}