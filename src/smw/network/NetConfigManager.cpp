#include "NetConfigManager.h"

#include "net.h"
#include "path.h"

#include "cpptoml/cpptoml.h"

#include <cassert>
#include <cstring>
#include <fstream>
#include <stdexcept>
#include <string>

void NetConfigManager::save()
{
    assert(netplay.myPlayerName);

    FILE* config = fopen((GetHomeDirectory() + "netplay.toml").c_str(), "wt");
    if (!config) {
        printf("[net][error] Could not save network settings\n");
        return;
    }

    fprintf(config, "player_name = \"%s\"\n", netplay.myPlayerName);
    fprintf(config, "servers = [\n");
    for (auto server: netplay.savedServers)
        fprintf(config, "    \"%s\",\n", server.hostname.c_str());
    fprintf(config, "]\n");

    fprintf(config, "\n");
    fclose(config);
}

void NetConfigManager::load()
{
    std::shared_ptr<cpptoml::table> config;
    try {
        config = cpptoml::parse_file(GetHomeDirectory() + "netplay.toml");
    }
    catch (const cpptoml::parse_exception& e) {
        printf("[net][warning] netplay.toml: %s\n", e.what());
        return;
    }

    // read player name
    auto player_name = config->get_as<std::string>("player_name");
    if (player_name)
        handle_playername(*player_name);

    // read server list
    auto raw_array = config->get_array("server");
    if (raw_array) {
        auto str_array = raw_array->array_of<std::string>();

        std::list<std::string> servers;
        for (auto str_entry: str_array) {
            if (str_entry)
                servers.push_back(str_entry->get());
        }

        handle_servers(servers);
    }
}

void NetConfigManager::handle_playername(const std::string& net_player_name)
{
    try {
        if (net_player_name.length() < 3)
            throw std::runtime_error("player name too short");

        if (net_player_name.length() >= NET_MAX_PLAYER_NAME_LENGTH) {
#if (defined(_WIN32) && defined(__MINGW32__)) || defined(__ANDROID__)
            // There is a bug in MinGW and Android NDK's GCC <= 4.8.x
            std::string err("player name must be less than 16 letters");
#else
            std::string err("player name must be less than ");
            err += std::to_string(NET_MAX_PLAYER_NAME_LENGTH);
            err += " letters";
#endif
            throw std::runtime_error(err);
        }

        strncpy(netplay.myPlayerName, net_player_name.c_str(), NET_MAX_PLAYER_NAME_LENGTH);
        netplay.myPlayerName[NET_MAX_PLAYER_NAME_LENGTH - 1] = '\0';
    }
    catch (std::runtime_error& error) {
        printf("[net][warning] netplay.toml: %s\n", error.what());
    }
}

void NetConfigManager::handle_servers(const std::list<std::string>& net_servers)
{
    unsigned i = 1;
    for (auto server_address: net_servers) {
        if (server_address.length() < 8 || server_address.length() > 250) {
            printf("[net][warning] netplay.toml: server #%u is invalid\n", i);
            continue;
        }

        ServerAddress host;
        host.hostname = server_address;
        netplay.savedServers.push_back(host);

        i++;
    }
}
