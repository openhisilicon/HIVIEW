REGISTER sip:34020000002000000001@192.168.0.166:5060 SIP/2.0
Via: SIP/2.0/UDP 192.168.0.167:6060;rport;branch=z9hG4bK1653974836
To: <sip:34020000001110000001@3402000000>
From: <sip:34020000001110000001@3402000000>;tag=1372943013
Call-ID: 1037112723
CSeq: 1 REGISTER
Contact: <sip:34020000001110000001@192.168.0.167:6060;line=b457c8445ab8966>
Max-Forwards: 70
User-Agent: SipWrapper
Expires: 3600
Content-Length: 0

SIP/2.0 401 Unauthorized
Via: SIP/2.0/UDP 192.168.0.167:6060;rport;branch=z9hG4bK1653974836
From: <sip:34020000001110000001@3402000000>;tag=1372943013
To: <sip:34020000001110000001@3402000000>;tag=1724123124
Call-ID: 1037112723
CSeq: 1 REGISTER
WWW-Authenticate: Digest realm="34020000", nonce="awer23sdfj123123", opaque="c3a02f1ecb122d255c4ae2266129d044", algorithm=MD5
User-Agent: General
Content-Length: 0

REGISTER sip:34020000002000000001@192.168.0.166:5060 SIP/2.0
Via: SIP/2.0/UDP 192.168.0.167:6060;rport;branch=z9hG4bK2849973368
To: <sip:34020000001110000001@3402000000>
From: <sip:34020000001110000001@3402000000>;tag=1372943013
Call-ID: 1037112723
CSeq: 2 REGISTER
Contact: <sip:34020000001110000001@192.168.0.167:6060;line=b457c8445ab8966>
Authorization: Digest username="34020000001110000001", realm="34020000", nonce="awer23sdfj123123", uri="sip:34020000002000000001@192.168.0.166:5060", response="047c0ec67f7849c371f507393425e1d4", algorithm=MD5, opaque="c3a02f1ecb122d255c4ae2266129d044"
Max-Forwards: 70
User-Agent: SipWrapper
Expires: 3600
Content-Length: 0

SIP/2.0 200 OK
Via: SIP/2.0/UDP 192.168.0.167:6060;rport;branch=z9hG4bK2849973368
From: <sip:34020000001110000001@3402000000>;tag=1372943013
To: <sip:34020000001110000001@3402000000>;tag=31243r3412
Call-ID: 1037112723
CSeq: 2 REGISTER
User-Agent: General
Date: 2021-07-15T09:47:41
Expires: 300
Content-Length: 0

MESSAGE sip:34020000002000000001@3402000000 SIP/2.0
Via: SIP/2.0/UDP 192.168.0.167:6060;rport;branch=z9hG4bK2549303051
To: <sip:34020000002000000001@3402000000>
From: <sip:34020000001110000001@3402000000>;tag=1604160428
Call-ID: 1527599460
CSeq: 20 MESSAGE
Content-Type: Application/MANSCDP+xml
Max-Forwards: 70
User-Agent: SipWrapper
Content-Length:   168

<?xml version="1.0" encoding="GB2312"?>
<Notify>
<CmdType>Keepalive</CmdType>
<SN>1</SN>
<DeviceID>34020000001110000001</DeviceID>
<Status>OK</Status>
</Notify>
SIP/2.0 200 OK
Via: SIP/2.0/UDP 192.168.0.167:6060;rport;branch=z9hG4bK2549303051
To: <sip:34020000002000000001@3402000000>;tag=12341241234
From: <sip:34020000001110000001@3402000000>;tag=1604160428
Call-ID: 1527599460
CSeq: 20 MESSAGE
User-Agent: General
Content-Length: 0

MESSAGE sip:34020000001110000001@3402000000 SIP/2.0
From: <sip:34020000002000000001@3402000000>;tag=bccedfd000001
To: <sip:34020000001110000001@3402000000>
Content-Length: 127
CSeq: 5 MESSAGE
Route: <sip:34020000001110000001>
Call-ID: 12345678900001
Via: SIP/2.0/UDP 192.168.0.166:5060;wlsscid=377aa9afcf1b36f;branch=123133532300001
Content-Type: Application/MANSCDP+xml
Max-Forwards: 70

<?xml version="1.0"?>
<Query>
<CmdType>Catalog</CmdType>
<SN>2</SN>
<DeviceID>34020000001110000001</DeviceID>
</Query>

SIP/2.0 200 OK
Via: SIP/2.0/UDP 192.168.0.166:5060;wlsscid=377aa9afcf1b36f;branch=123133532300001
To: <sip:34020000001110000001@3402000000>;tag=1024165120
From: <sip:34020000002000000001@3402000000>;tag=bccedfd000001
Call-ID: 12345678900001
CSeq: 5 MESSAGE
User-Agent: SipWrapper
Content-Length: 0

MESSAGE sip:34020000002000000001@3402000000 SIP/2.0
Via: SIP/2.0/UDP 192.168.0.167:6060;rport;branch=z9hG4bK2408153614
To: <sip:34020000002000000001@3402000000>
From: <sip:34020000001110000001@3402000000>;tag=3171389754
Call-ID: 4242601779
CSeq: 20 MESSAGE
Content-Type: Application/MANSCDP+xml
Max-Forwards: 70
User-Agent: SipWrapper
Content-Length:   638

<?xml version="1.0" encoding="GB2312"?>
<Response>
<CmdType>Catalog</CmdType>
<SN>2</SN>
<DeviceID>34020000001110000001</DeviceID>
<SumNum>2</SumNum>
<DeviceList Num="1">
<Item>
<DeviceID>34020000001310000001</DeviceID>
<Name>IPC</Name>
<Manufacturer>Manufacturer</Manufacturer>
<Model>Model</Model>
<Owner>Owner</Owner>
<CivilCode>34020000</CivilCode>
<Block>Block</Block>
<Address>Address</Address>
<Parental>0</Parental>
<ParentID>34020000001110000001</ParentID>
<RegisterWay>1</RegisterWay>
<EndTime>2021-07-15T09:47:40.782</EndTime>
<Secrecy>0</Secrecy>
<Status>ON</Status>
</Item>
</DeviceList>
</Response>
MESSAGE sip:34020000002000000001@3402000000 SIP/2.0
Via: SIP/2.0/UDP 192.168.0.167:6060;rport;branch=z9hG4bK122901808
To: <sip:34020000002000000001@3402000000>
From: <sip:34020000001110000001@3402000000>;tag=3150436301
Call-ID: 3024482865
CSeq: 20 MESSAGE
Content-Type: Application/MANSCDP+xml
Max-Forwards: 70
User-Agent: SipWrapper
Content-Length:   640

<?xml version="1.0" encoding="GB2312"?>
<Response>
<CmdType>Catalog</CmdType>
<SN>2</SN>
<DeviceID>34020000001110000001</DeviceID>
<SumNum>2</SumNum>
<DeviceList Num="1">
<Item>
<DeviceID>34020000001340000001</DeviceID>
<Name>Alarm</Name>
<Manufacturer>Manufacturer</Manufacturer>
<Model>Model</Model>
<Owner>Owner</Owner>
<CivilCode>34020000</CivilCode>
<Block>Block</Block>
<Address>Address</Address>
<Parental>0</Parental>
<ParentID>34020000001110000001</ParentID>
<RegisterWay>1</RegisterWay>
<EndTime>2021-07-15T09:47:40.782</EndTime>
<Secrecy>0</Secrecy>
<Status>ON</Status>
</Item>
</DeviceList>
</Response>
SIP/2.0 200 OK
Via: SIP/2.0/UDP 192.168.0.167:6060;rport;branch=z9hG4bK2408153614
To: <sip:34020000002000000001@3402000000>;tag=12341241234
From: <sip:34020000001110000001@3402000000>;tag=3171389754
Call-ID: 4242601779
CSeq: 20 MESSAGE
User-Agent: General
Content-Length: 0

SIP/2.0 200 OK
Via: SIP/2.0/UDP 192.168.0.167:6060;rport;branch=z9hG4bK122901808
To: <sip:34020000002000000001@3402000000>;tag=12341241234
From: <sip:34020000001110000001@3402000000>;tag=3150436301
Call-ID: 3024482865
CSeq: 20 MESSAGE
User-Agent: General
Content-Length: 0

INVITE sip:34020000001310000001@3402000000 SIP/2.0
Call-ID: helloVideo
CSeq: 1 INVITE
From: <sip:34020000002000000001@3402000000>;tag=bccedfd0111
To: <sip:34020000001110000001@3402000000>
Max-Forwards: 70
Contact: <sip:34020000001310000001@3402000000>
Via: SIP/2.0/UDP 192.168.0.167:5060;branch=z9hG4bKee5c5d98-bff9-4f3000002
Content-Type: application/sdp
Content-Length: 227

v=0
o=34020000001310000001 0 0 IN IP4 192.168.0.166
s=Play
c=IN IP4 192.168.0.166
t=0 0
m=video 6000 RTP/AVP 96 98 97
a=recvonly
a=rtpmap:96 PS/90000
a=rtpmap:98 H264/90000
a=rtpmap:97 MPEG4/90000
y=0100000001
f=

SIP/2.0 200 OK
Via: SIP/2.0/UDP 192.168.0.167:5060;branch=z9hG4bKee5c5d98-bff9-4f3000002;received=192.168.0.166
To: <sip:34020000001110000001@3402000000>;tag=217693279
From: <sip:34020000002000000001@3402000000>;tag=bccedfd0111
Call-ID: helloVideo
CSeq: 1 INVITE
Contact: <sip:34020000001110000001@192.168.0.167:6060>
Content-Type: application/sdp
User-Agent: SipWrapper
Content-Length:   172

v=0
o=34020000001110000001 0 0 IN IP4 192.168.0.167
s=SIP Client
c=IN IP4 192.168.0.167
t=0 0
m=video 7000 RTP/AVP 96
a=rtpmap:96 PS/90000
a=sendonly
y=0100000001
ACK sip:34020000001310000001@3402000000 SIP/2.0
Call-ID: helloVideo
CSeq: 1 ACK
From: <sip:34020000002000000001@3402000000>;tag=bccedfd0111
To: <sip:34020000001110000001@3402000000>
Max-Forwards: 70
Via: SIP/2.0/UDP 192.168.0.167:5060;branch=z9hG4bKee5c5d98-00003
Content-Length: 0

BYE sip:34020000001310000001@3402000000 SIP/2.0
From: <sip:34020000002000000001@3402000000>;tag=bccedfd0111
To: sip:34020000001110000001@3402000000;tag=217693279
CSeq: 2 BYE
Call-ID: helloVideo
Via: SIP/2.0/UDP 192.168.0.167:5060;branch=z9hG4bKee5c5d98-00004
Max-Forwards: 70
Content-Length: 0

SIP/2.0 200 OK
Via: SIP/2.0/UDP 192.168.0.167:5060;branch=z9hG4bKee5c5d98-00004;received=192.168.0.166
To: <sip:34020000001110000001@3402000000>;tag=217693279
From: <sip:34020000002000000001@3402000000>;tag=bccedfd0111
Call-ID: helloVideo
CSeq: 2 BYE
User-Agent: SipWrapper
Content-Length: 0

MESSAGE sip:34020000002000000001@3402000000 SIP/2.0
Via: SIP/2.0/UDP 192.168.0.167:6060;rport;branch=z9hG4bK2272904383
To: <sip:34020000002000000001@3402000000>
From: <sip:34020000001110000001@3402000000>;tag=3886510725
Call-ID: 3377836990
CSeq: 20 MESSAGE
Content-Type: Application/MANSCDP+xml
Max-Forwards: 70
User-Agent: SipWrapper
Content-Length:   168

<?xml version="1.0" encoding="GB2312"?>
<Notify>
<CmdType>Keepalive</CmdType>
<SN>2</SN>
<DeviceID>34020000001110000001</DeviceID>
<Status>OK</Status>
</Notify>
SIP/2.0 200 OK
Via: SIP/2.0/UDP 192.168.0.167:6060;rport;branch=z9hG4bK2272904383
To: <sip:34020000002000000001@3402000000>;tag=12341241234
From: <sip:34020000001110000001@3402000000>;tag=3886510725
Call-ID: 3377836990
CSeq: 20 MESSAGE
User-Agent: General
Content-Length: 0

INVITE sip:34020000001310000001@3402000000 SIP/2.0
Call-ID: helloVideo
CSeq: 1 INVITE
From: <sip:34020000002000000001@3402000000>;tag=bccedfd0111
To: <sip:34020000001110000001@3402000000>
Max-Forwards: 70
Contact: <sip:34020000001310000001@3402000000>
Via: SIP/2.0/UDP 192.168.0.167:5060;branch=z9hG4bKee5c5d98-bff9-4f3000005
Content-Type: application/sdp
Content-Length: 266

v=0
o=34020000001310000001 0 0 IN IP4 192.168.0.166
s=Play
c=IN IP4 192.168.0.166
t=0 0
m=video 6004 TCP/RTP/AVP 96 98 97
a=recvonly
a=rtpmap:96 PS/90000
a=rtpmap:98 H264/90000
a=rtpmap:97 MPEG4/90000
a=setup:passive
a=connection:new
y=0100000001
f=

SIP/2.0 200 OK
Via: SIP/2.0/UDP 192.168.0.167:5060;branch=z9hG4bKee5c5d98-bff9-4f3000005;received=192.168.0.166
To: <sip:34020000001110000001@3402000000>;tag=2657485927
From: <sip:34020000002000000001@3402000000>;tag=bccedfd0111
Call-ID: helloVideo
CSeq: 1 INVITE
Contact: <sip:34020000001110000001@192.168.0.167:6060>
Content-Type: application/sdp
User-Agent: SipWrapper
Content-Length:   172

v=0
o=34020000001110000001 0 0 IN IP4 192.168.0.167
s=SIP Client
c=IN IP4 192.168.0.167
t=0 0
m=video 7000 RTP/AVP 96
a=rtpmap:96 PS/90000
a=sendonly
y=0100000001
ACK sip:34020000001310000001@3402000000 SIP/2.0
Call-ID: helloVideo
CSeq: 1 ACK
From: <sip:34020000002000000001@3402000000>;tag=bccedfd0111
To: <sip:34020000001110000001@3402000000>
Max-Forwards: 70
Via: SIP/2.0/UDP 192.168.0.167:5060;branch=z9hG4bKee5c5d98-00006
Content-Length: 0

BYE sip:34020000001310000001@3402000000 SIP/2.0
From: <sip:34020000002000000001@3402000000>;tag=bccedfd0111
To: sip:34020000001110000001@3402000000;tag=2657485927
CSeq: 2 BYE
Call-ID: helloVideo
Via: SIP/2.0/UDP 192.168.0.167:5060;branch=z9hG4bKee5c5d98-00007
Max-Forwards: 70
Content-Length: 0

SIP/2.0 200 OK
Via: SIP/2.0/UDP 192.168.0.167:5060;branch=z9hG4bKee5c5d98-00007;received=192.168.0.166
To: <sip:34020000001110000001@3402000000>;tag=2657485927
From: <sip:34020000002000000001@3402000000>;tag=bccedfd0111
Call-ID: helloVideo
CSeq: 2 BYE
User-Agent: SipWrapper
Content-Length: 0

MESSAGE sip:34020000002000000001@3402000000 SIP/2.0
Via: SIP/2.0/UDP 192.168.0.167:6060;rport;branch=z9hG4bK3780031360
To: <sip:34020000002000000001@3402000000>
From: <sip:34020000001110000001@3402000000>;tag=2232949116
Call-ID: 2685822621
CSeq: 20 MESSAGE
Content-Type: Application/MANSCDP+xml
Max-Forwards: 70
User-Agent: SipWrapper
Content-Length:   168

<?xml version="1.0" encoding="GB2312"?>
<Notify>
<CmdType>Keepalive</CmdType>
<SN>3</SN>
<DeviceID>34020000001110000001</DeviceID>
<Status>OK</Status>
</Notify>
SIP/2.0 200 OK
Via: SIP/2.0/UDP 192.168.0.167:6060;rport;branch=z9hG4bK3780031360
To: <sip:34020000002000000001@3402000000>;tag=12341241234
From: <sip:34020000001110000001@3402000000>;tag=2232949116
Call-ID: 2685822621
CSeq: 20 MESSAGE
User-Agent: General
Content-Length: 0



//////////////////////////////////////////////////////////////////////


REGISTER sip:34020000002000000001@192.168.0.166:5060 SIP/2.0
Via: SIP/2.0/UDP 192.168.0.167:6060;rport;branch=z9hG4bK3728106328
To: <sip:34020000001110000001@3402000000>
From: <sip:34020000001110000001@3402000000>;tag=1300599107
Call-ID: 595071153
CSeq: 1 REGISTER
Contact: <sip:34020000001110000001@192.168.0.167:6060;line=8b8d9be776c83ad>
Max-Forwards: 70
User-Agent: SipWrapper
Expires: 3600
Content-Length: 0

SIP/2.0 401 Unauthorized
Via: SIP/2.0/UDP 192.168.0.167:6060;rport;branch=z9hG4bK3728106328
From: <sip:34020000001110000001@3402000000>;tag=1300599107
To: <sip:34020000001110000001@3402000000>;tag=1724123124
Call-ID: 595071153
CSeq: 1 REGISTER
WWW-Authenticate: Digest realm="34020000", nonce="awer23sdfj123123", opaque="c3a02f1ecb122d255c4ae2266129d044", algorithm=MD5
User-Agent: General
Content-Length: 0

REGISTER sip:34020000002000000001@192.168.0.166:5060 SIP/2.0
Via: SIP/2.0/UDP 192.168.0.167:6060;rport;branch=z9hG4bK3960438213
To: <sip:34020000001110000001@3402000000>
From: <sip:34020000001110000001@3402000000>;tag=1300599107
Call-ID: 595071153
CSeq: 2 REGISTER
Contact: <sip:34020000001110000001@192.168.0.167:6060;line=8b8d9be776c83ad>
Authorization: Digest username="34020000001110000001", realm="34020000", nonce="awer23sdfj123123", uri="sip:34020000002000000001@192.168.0.166:5060", response="047c0ec67f7849c371f507393425e1d4", algorithm=MD5, opaque="c3a02f1ecb122d255c4ae2266129d044"
Max-Forwards: 70
User-Agent: SipWrapper
Expires: 3600
Content-Length: 0

SIP/2.0 200 OK
Via: SIP/2.0/UDP 192.168.0.167:6060;rport;branch=z9hG4bK3960438213
From: <sip:34020000001110000001@3402000000>;tag=1300599107
To: <sip:34020000001110000001@3402000000>;tag=31243r3412
Call-ID: 595071153
CSeq: 2 REGISTER
User-Agent: General
Date: 2021-07-19T09:59:36
Expires: 300
Content-Length: 0

MESSAGE sip:34020000002000000001@3402000000 SIP/2.0
Via: SIP/2.0/UDP 192.168.0.167:6060;rport;branch=z9hG4bK2146363272
To: <sip:34020000002000000001@3402000000>
From: <sip:34020000001110000001@3402000000>;tag=3004910448
Call-ID: 2369486520
CSeq: 20 MESSAGE
Content-Type: Application/MANSCDP+xml
Max-Forwards: 70
User-Agent: SipWrapper
Content-Length:   168

<?xml version="1.0" encoding="GB2312"?>
<Notify>
<CmdType>Keepalive</CmdType>
<SN>1</SN>
<DeviceID>34020000001110000001</DeviceID>
<Status>OK</Status>
</Notify>
SIP/2.0 200 OK
Via: SIP/2.0/UDP 192.168.0.167:6060;rport;branch=z9hG4bK2146363272
To: <sip:34020000002000000001@3402000000>;tag=12341241234
From: <sip:34020000001110000001@3402000000>;tag=3004910448
Call-ID: 2369486520
CSeq: 20 MESSAGE
User-Agent: General
Content-Length: 0

MESSAGE sip:34020000001110000001@3402000000 SIP/2.0
From: <sip:34020000002000000001@3402000000>;tag=bccedfd000001
To: <sip:34020000001110000001@3402000000>
Content-Length: 127
CSeq: 5 MESSAGE
Route: <sip:34020000001110000001>
Call-ID: 12345678900001
Via: SIP/2.0/UDP 192.168.0.166:5060;wlsscid=377aa9afcf1b36f;branch=123133532300001
Content-Type: Application/MANSCDP+xml
Max-Forwards: 70

<?xml version="1.0"?>
<Query>
<CmdType>Catalog</CmdType>
<SN>2</SN>
<DeviceID>34020000001110000001</DeviceID>
</Query>

SIP/2.0 200 OK
Via: SIP/2.0/UDP 192.168.0.166:5060;wlsscid=377aa9afcf1b36f;branch=123133532300001
To: <sip:34020000001110000001@3402000000>;tag=3502714241
From: <sip:34020000002000000001@3402000000>;tag=bccedfd000001
Call-ID: 12345678900001
CSeq: 5 MESSAGE
User-Agent: SipWrapper
Content-Length: 0

MESSAGE sip:34020000002000000001@3402000000 SIP/2.0
Via: SIP/2.0/UDP 192.168.0.167:6060;rport;branch=z9hG4bK616314673
To: <sip:34020000002000000001@3402000000>
From: <sip:34020000001110000001@3402000000>;tag=187835191
Call-ID: 4120558719
CSeq: 20 MESSAGE
Content-Type: Application/MANSCDP+xml
Max-Forwards: 70
User-Agent: SipWrapper
Content-Length:   638

<?xml version="1.0" encoding="GB2312"?>
<Response>
<CmdType>Catalog</CmdType>
<SN>2</SN>
<DeviceID>34020000001110000001</DeviceID>
<SumNum>2</SumNum>
<DeviceList Num="1">
<Item>
<DeviceID>34020000001310000001</DeviceID>
<Name>IPC</Name>
<Manufacturer>Manufacturer</Manufacturer>
<Model>Model</Model>
<Owner>Owner</Owner>
<CivilCode>34020000</CivilCode>
<Block>Block</Block>
<Address>Address</Address>
<Parental>0</Parental>
<ParentID>34020000001110000001</ParentID>
<RegisterWay>1</RegisterWay>
<EndTime>2021-07-19T09:59:36.666</EndTime>
<Secrecy>0</Secrecy>
<Status>ON</Status>
</Item>
</DeviceList>
</Response>
MESSAGE sip:34020000002000000001@3402000000 SIP/2.0
Via: SIP/2.0/UDP 192.168.0.167:6060;rport;branch=z9hG4bK1314356204
To: <sip:34020000002000000001@3402000000>
From: <sip:34020000001110000001@3402000000>;tag=2966413785
Call-ID: 4134157866
CSeq: 20 MESSAGE
Content-Type: Application/MANSCDP+xml
Max-Forwards: 70
User-Agent: SipWrapper
Content-Length:   640

<?xml version="1.0" encoding="GB2312"?>
<Response>
<CmdType>Catalog</CmdType>
<SN>2</SN>
<DeviceID>34020000001110000001</DeviceID>
<SumNum>2</SumNum>
<DeviceList Num="1">
<Item>
<DeviceID>34020000001340000001</DeviceID>
<Name>Alarm</Name>
<Manufacturer>Manufacturer</Manufacturer>
<Model>Model</Model>
<Owner>Owner</Owner>
<CivilCode>34020000</CivilCode>
<Block>Block</Block>
<Address>Address</Address>
<Parental>0</Parental>
<ParentID>34020000001110000001</ParentID>
<RegisterWay>1</RegisterWay>
<EndTime>2021-07-19T09:59:36.667</EndTime>
<Secrecy>0</Secrecy>
<Status>ON</Status>
</Item>
</DeviceList>
</Response>
SIP/2.0 200 OK
Via: SIP/2.0/UDP 192.168.0.167:6060;rport;branch=z9hG4bK616314673
To: <sip:34020000002000000001@3402000000>;tag=12341241234
From: <sip:34020000001110000001@3402000000>;tag=187835191
Call-ID: 4120558719
CSeq: 20 MESSAGE
User-Agent: General
Content-Length: 0

SIP/2.0 200 OK
Via: SIP/2.0/UDP 192.168.0.167:6060;rport;branch=z9hG4bK1314356204
To: <sip:34020000002000000001@3402000000>;tag=12341241234
From: <sip:34020000001110000001@3402000000>;tag=2966413785
Call-ID: 4134157866
CSeq: 20 MESSAGE
User-Agent: General
Content-Length: 0

INVITE sip:34020000001310000001@3402000000 SIP/2.0
Call-ID: helloVideo
CSeq: 1 INVITE
From: <sip:34020000002000000001@3402000000>;tag=bccedfd0111
To: <sip:34020000001110000001@3402000000>
Max-Forwards: 70
Contact: <sip:34020000001310000001@3402000000>
Via: SIP/2.0/UDP 192.168.0.167:5060;branch=z9hG4bKee5c5d98-bff9-4f3000002
Content-Type: application/sdp
Content-Length: 266

v=0
o=34020000001310000001 0 0 IN IP4 192.168.0.166
s=Play
c=IN IP4 192.168.0.166
t=0 0
m=video 6004 TCP/RTP/AVP 96 98 97
a=recvonly
a=rtpmap:96 PS/90000
a=rtpmap:98 H264/90000
a=rtpmap:97 MPEG4/90000
a=setup:passive
a=connection:new
y=0100000001
f=

SIP/2.0 200 OK
Via: SIP/2.0/UDP 192.168.0.167:5060;branch=z9hG4bKee5c5d98-bff9-4f3000002;received=192.168.0.166
To: <sip:34020000001110000001@3402000000>;tag=3724561948
From: <sip:34020000002000000001@3402000000>;tag=bccedfd0111
Call-ID: helloVideo
CSeq: 1 INVITE
Contact: <sip:34020000001110000001@192.168.0.167:6060>
Content-Type: application/sdp
User-Agent: SipWrapper
Content-Length:   172

v=0
o=34020000001110000001 0 0 IN IP4 192.168.0.167
s=SIP Client
c=IN IP4 192.168.0.167
t=0 0
m=video 7000 RTP/AVP 96
a=rtpmap:96 PS/90000
a=sendonly
y=0100000001
ACK sip:34020000001310000001@3402000000 SIP/2.0
Call-ID: helloVideo
CSeq: 1 ACK
From: <sip:34020000002000000001@3402000000>;tag=bccedfd0111
To: <sip:34020000001110000001@3402000000>
Max-Forwards: 70
Via: SIP/2.0/UDP 192.168.0.167:5060;branch=z9hG4bKee5c5d98-00003
Content-Length: 0

BYE sip:34020000001310000001@3402000000 SIP/2.0
From: <sip:34020000002000000001@3402000000>;tag=bccedfd0111
To: sip:34020000001110000001@3402000000;tag=3724561948
CSeq: 2 BYE
Call-ID: helloVideo
Via: SIP/2.0/UDP 192.168.0.167:5060;branch=z9hG4bKee5c5d98-00004
Max-Forwards: 70
Content-Length: 0

SIP/2.0 200 OK
Via: SIP/2.0/UDP 192.168.0.167:5060;branch=z9hG4bKee5c5d98-00004;received=192.168.0.166
To: <sip:34020000001110000001@3402000000>;tag=3724561948
From: <sip:34020000002000000001@3402000000>;tag=bccedfd0111
Call-ID: helloVideo
CSeq: 2 BYE
User-Agent: SipWrapper
Content-Length: 0