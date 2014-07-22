#ifndef NETWORK_PROTOCOL_DEFINITIONS_H
#define NETWORK_PROTOCOL_DEFINITIONS_H

#define NET_PROTOCOL_VERSION_MAJOR          1
#define NET_PROTOCOL_VERSION_MINOR          0
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
#define NET_NOTICE_ROOM_CHANGED             34
#define NET_NOTICE_ROOM_CHAT_MSG            35

#define NET_REQUEST_CREATE_ROOM             40
#define NET_RESPONSE_CREATE_OK              41
#define NET_RESPONSE_CREATE_ERROR           42 // TODO: What kind of error?

#define NET_G2L_START_ROOM                  50 // Game Host -> Server: "OK, we're ready to go!"
#define NET_NOTICE_GAMEHOST_INFO            51 // Server -> Normal Players: "Connect to this guy to play"
#define NET_P2G_SYNC_OK                     52 // Normal Player -> Game Host: "I'm ready!"
#define NET_G2P_GAME_START                  53 // Game Host -> Everyone: "Everybody connected successfully!"

#define NET_P2G_LOCAL_KEYS                  60 // Normal players -> Game host
#define NET_P2G_LEAVE_GAME                  61 // Normal players -> Game host
#define NET_G2P_GAME_STATE                  62 // GH -> Players
#define NET_G2L_GAME_RESULTS                63 // GH -> Server

#endif // NETWORK_PROTOCOL_DEFINITIONS_H
