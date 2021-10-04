# Netplay

SMW has an *experimental* online multiplayer mode. It consists of two parts:

- a lobby server
- the game itself

The **lobby server** is where people can gather, create or join game rooms and chat there. When everyone's ready, the server shares the skins and the map between all players, and the actual game starts.

You can join to a lobby server from the **game**, by entering its address (ie. URL or IP). You can add more than one of your favorite servers, and the game will remember it. The actual gameplay happens between the players, the lobby server is only used to connect you together.

## Running the server

The lobby server program is called `smw-server`. All players will try to join to the computer running it. As such, if the server runs behind a router, you have to allow incoming connections, and tell the router which device on your network runs the server (this is called *port forwarding*).

By default, the server listens on **UDP port 12521**. If necessary, you can change this, see the next section.

If you're playing with just your friends, only one of you has to run the lobby server. You still have to set up port forwarding though, as you're expecting incoming connections.

## Configuring the server

The lobby server has some parameters you can change:

Name | Description | Default
-----|-------------|--------
`port` | The server will listen on this UDP network port | 12521
`name` | The name of the server, to show for the players | `SMW Server`
`maxplayers` | The number of players allowed to be on the server at the same time | 20
`password` | A password players have to enter to join the server | (nothing)

You can change these values by creating a file called `serverconfig.txt`, and placing it next to `smw-server`. Here is an example:

```
# SMW Server configuration

port 12521
name My Cool Server
maxplayers 10
password supersekret
```

Empty lines and lines starting with `#` are ignored. The rest are in `name value` format, one entry per line.
