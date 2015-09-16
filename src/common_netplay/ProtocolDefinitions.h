#ifndef NETWORK_PROTOCOL_DEFINITIONS_H
#define NETWORK_PROTOCOL_DEFINITIONS_H

// L: lobby server
// G: game host player
// P: regular players
// E: every player (G+P)

#define NET_PROTOCOL_VERSION_MAJOR          0
#define NET_PROTOCOL_VERSION_MINOR          3
#define NET_MAX_MESSAGE_SIZE                128
#define NET_LOBBYSERVER_PORT                12521
#define NET_GAMEHOST_PORT                   12522
#define NET_MAX_PLAYER_NAME_LENGTH          16
#define NET_MAX_ROOM_NAME_LENGTH            32
#define NET_MAX_ROOM_PASSWORD_LENGTH        16
#define NET_MAX_CHAT_MSG_LENGTH             100

// Lobby server querying
#define NET_REQUEST_SERVERINFO              1
#define NET_RESPONSE_BADPROTOCOL            2
#define NET_RESPONSE_SERVERINFO             3
#define NET_RESPONSE_SERVER_MOTD            4 // Message of the Day
#define NET_NOTICE_SERVER_STOPPED           5

// Connection to lobby server
#define NET_REQUEST_CONNECT                 10
#define NET_REQUEST_LEAVE_SERVER            11
#define NET_RESPONSE_CONNECT_OK             12
#define NET_RESPONSE_CONNECT_DENIED         13
#define NET_RESPONSE_CONNECT_SERVERFULL     14
#define NET_RESPONSE_CONNECT_NAMETAKEN      15

// Interaction with room list on lobby server
#define NET_REQUEST_ROOM_LIST               20
#define NET_RESPONSE_NO_ROOMS               21 // no rooms on the server
#define NET_RESPONSE_ROOM_LIST_ENTRY        22 // basic info about a single room

// Room creation on lobby server
#define NET_G2L_CREATE_ROOM                 30
#define NET_L2G_CREATE_OK                   31
#define NET_L2G_CREATE_ERROR                32 // TODO: What kind of error?
#define NET_G2L_ROOM_CHANGED                33 // eg. player count, game mode, ...

#define NET_REQUEST_JOIN_ROOM               80
#define NET_L2P_GAMEHOST_INFO               81 // Server -> Normal Players: "Connect to this guy to play"
#define NET_L2G_CLIENT_INFO                 82 // Server -> Game Host: "This player wants to join"
#define NET_G2L_ROOM_STARTED                83 // Game Host -> Server: "Our room started!"
#define NET_G2L_GAME_RESULTS                84 // GH -> Server

// Communication between simple player and game host
#define NET_P2G_JOIN_ROOM                   40
#define NET_P2G_LEAVE_ROOM                  41
#define NET_G2P_JOIN_OK                     42
#define NET_G2P_ROOM_FULL                   43
#define NET_G2P_ROOM_CHANGED                44
#define NET_G2E_ROOM_CHAT_MSG               45

// Pre-game preparations
#define NET_G2P_MAP_CHANGED                 53 // Game Host -> Players: "Here's the map file"
#define NET_P2G_MAP_OK                      54 // Normal Player -> Game Host: "Download complete!"
#define NET_G2P_SYNC                        55 // Game Host -> Players: "Prepare gameplay with this data" (eg. RNG seed)
#define NET_P2G_SYNC_OK                     56 // Normal Player -> Game Host: "I'm ready!"
#define NET_G2E_GAME_START                  57 // Game Host -> Everyone: "Everybody connected successfully!"

// In-game communication
#define NET_P2G_LEAVE_GAME                  70 // Normal players -> Game host
#define NET_P2G_LOCAL_KEYS                  71 // Normal players -> Game host
#define NET_G2P_GAME_STATE                  72 // GH -> Players
#define NET_P2G_REQ_POWERUP                 73 // Player wants to use powerup
#define NET_G2P_START_POWERUP               74 // Another player wants to use powerup
#define NET_G2P_TRIGGER_POWERUP             75 // Trigger powerup effect
#define NET_G2P_TRIGGER_MAPCOLL             76 // Trigger map collision for clients
#define NET_G2P_TRIGGER_P2PCOLL             77 // Trigger player-to-player collision for clients

#endif // NETWORK_PROTOCOL_DEFINITIONS_H
