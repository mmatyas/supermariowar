#ifndef NETWORK_PROTOCOL_CODES_H
#define NETWORK_PROTOCOL_CODES_H


#define NET_PROTOCOL_VERSION                1
#define NET_MAX_MESSAGE_SIZE                128
#define NET_SERVER_PORT                     12521
#define NET_MAX_PLAYER_NAME_LENGTH          16
#define NET_MAX_ROOM_NAME_LENGTH            32
#define NET_MAX_ROOM_PASSWORD_LENGTH        16
#define NET_MAX_CHAT_MSG_LENGTH             100

#define NET_REQUEST_SERVERINFO              1
#define NET_RESPONSE_BADPROTOCOL            2
#define NET_RESPONSE_SERVERINFO             3
#define NET_RESPONSE_SERVER_MOTD            4 // Message of the Day
#define NET_NOTICE_SERVER_STOPPED           5

#define NET_REQUEST_CONNECT                 10
#define NET_REQUEST_LEAVE_SERVER            11
#define NET_RESPONSE_CONNECT_OK             12
#define NET_RESPONSE_CONNECT_DENIED         13
#define NET_RESPONSE_CONNECT_SERVERFULL     14
#define NET_RESPONSE_CONNECT_NAMETAKEN      15

#define NET_REQUEST_ROOM_LIST               20
#define NET_RESPONSE_NO_ROOMS               21
#define NET_RESPONSE_ROOM_LIST_ENTRY        22

#define NET_REQUEST_JOIN_ROOM               30
#define NET_REQUEST_LEAVE_ROOM              31
#define NET_RESPONSE_JOIN_OK                32
#define NET_RESPONSE_ROOM_FULL              33
#define NET_NOTICE_ROOM_CHANGED             34 // + host quit?

#define NET_REQUEST_CREATE_ROOM             40
#define NET_RESPONSE_CREATE_OK              41
#define NET_RESPONSE_CREATE_ERROR           42 // TODO: What kind of error?

#define NET_REQUEST_START_GAME              50
#define NET_NOTICE_GAME_SYNC                51 // check if everyone is ready
#define NET_NOTICE_GAME_SYNC_OK             52
#define NET_NOTICE_GAME_STARTED             53

#define NET_NOTICE_LOCAL_KEYS               60
#define NET_NOTICE_REMOTE_KEYS              61
#define NET_NOTICE_HOST_STATE               62


#endif // NETWORK_PROTOCOL_CODES_H
