#ifndef NETWORK_PROTOCOL_DEFINITIONS_H
#define NETWORK_PROTOCOL_DEFINITIONS_H

#define NET_PROTOCOL_VERSION_MAJOR          0
#define NET_PROTOCOL_VERSION_MINOR          5
#define NET_MAX_MESSAGE_SIZE                128
#define NET_LOBBYSERVER_PORT                12521
#define NET_GAMEHOST_PORT                   12522
#define NET_MAX_PLAYER_NAME_LENGTH          16
#define NET_MAX_ROOM_NAME_LENGTH            32
#define NET_MAX_ROOM_PASSWORD_LENGTH        16
#define NET_MAX_CHAT_MSG_LENGTH             100
#define NET_GAMESTATE_FRAMES_TO_SEND        3 // send gamestate every Nth frame only

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
#define NET_NOTICE_ROOM_CHANGE              34
#define NET_NOTICE_MAP_CHANGE               35 // (after everyone connected) Game Host -> Players: "Here's the map file"
#define NET_NOTICE_GAMEMODESETTINGS         36
#define NET_NOTICE_SKIN_CHANGE              37
#define NET_NOTICE_ROOM_CHAT_MSG            38

#define NET_REQUEST_CREATE_ROOM             40
#define NET_RESPONSE_CREATE_OK              41
#define NET_RESPONSE_CREATE_ERROR           42 // TODO: What kind of error?

// L: lobby server
// G: game host player
// P: regular players

#define NET_G2L_START_ROOM                  50 // Game Host -> Server: "OK, we're ready to go!"
#define NET_L2P_GAMEHOST_INFO               51 // Server -> Normal Players: "Connect to this guy to play"
#define NET_L2G_CLIENTS_INFO                52 // Server -> Game Host: "These players want to connect you"
#define NET_G2P_SYNC                        55 // Game Host -> Players: "Prepare gameplay with this data (eg. RNG seed)"
#define NET_P2G_SYNC_OK                     56 // Normal Player -> Game Host: "I'm ready!"
#define NET_G2E_GAME_START                  57 // Game Host -> Everyone: "Everybody connected successfully!"
#define NET_G2L_GAME_RESULTS                58 // GH -> Server

#define NET_P2G_LEAVE_GAME                  70 // Normal players -> Game host
#define NET_P2G_LOCAL_KEYS                  71 // Normal players -> Game host
#define NET_G2P_REMOTE_KEYS                 72 // Game host -> Normal players
#define NET_G2P_GAME_STATE                  73 // GH -> Players
#define NET_P2G_REQ_POWERUP                 74 // Player wants to use powerup
#define NET_G2P_START_POWERUP               75 // Another player wants to use powerup
#define NET_G2P_TRIGGER_POWERUP             76 // Trigger powerup effect
#define NET_G2P_TRIGGER_MAPCOLL             77 // Trigger map collision for clients
#define NET_G2P_TRIGGER_P2PCOLL             78 // Trigger player-to-player collision for clients

#endif // NETWORK_PROTOCOL_DEFINITIONS_H
