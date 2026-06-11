#include "NetConfigManager.h"

#include "net.h"
#include "path.h"

#include <toml.hpp>

#include <cassert>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

constexpr const char* CONFIG_FILENAME = "servers.toml";

bool load_file(toml::value& config)
{
    try {
        config = toml::parse(GetHomeDirectory() + CONFIG_FILENAME);
        return true;
    }
    catch (const toml::file_io_error&) {
        printf("[net][warning] Could not open %s, using default values.\n", CONFIG_FILENAME);
    }
    catch (const std::exception& error) {
        printf("[net][warning] %s: %s\n", CONFIG_FILENAME, error.what());
    }

    return false;
}

void read_playername(const toml::value& config)
{
    try {
        if (!config.contains("player_name"))
            return;

        const toml::value& config_playername = config.at("player_name");
        if (!config_playername.is_string())
            throw std::runtime_error("player name must be a simple string");

        const std::string& net_player_name = config_playername.as_string();

        if (net_player_name.length() < 3)
            throw std::runtime_error("player name too short");

        if (net_player_name.length() >= NET_MAX_PLAYER_NAME_LENGTH) {
            std::string err("player name must be less than ");
            err += std::to_string(NET_MAX_PLAYER_NAME_LENGTH);
            err += " letters";
            throw std::runtime_error(err);
        }

        netplay.myPlayerName = net_player_name;
    }
    catch (const std::exception& error) {
        printf("[net][warning] %s: %s\n", CONFIG_FILENAME, error.what());
    }
}

void read_servers(const toml::value& config)
{
    try {
        if (!config.contains("servers"))
            return;

        const toml::value& config_servers = config.at("servers");
        if (!config_servers.is_array())
            throw std::runtime_error("`servers` is in wrong format");

        const toml::array& servers = config_servers.as_array();
        for (size_t i = 0; i < servers.size(); i++) {
            if (!servers[i].is_string()) {
                printf("[net][warning] %s: server #%lu is invalid\n", CONFIG_FILENAME, i + 1);
                continue;
            }

            const std::string& address_str = servers[i].as_string();
            if (address_str.length() < 8 || address_str.length() > 250) {
                printf("[net][warning] %s: server #%lu is invalid\n", CONFIG_FILENAME, i + 1);
                continue;
            }

            ServerAddress host;
            host.hostname = address_str;
            netplay.savedServers.push_back(host);
        }
    }
    catch (const std::exception& error) {
        printf("[net][warning] %s: %s\n", CONFIG_FILENAME, error.what());
    }
}

} // namespace

void NetConfigManager::save()
{
    assert(!netplay.myPlayerName.empty());

    std::ofstream config(GetHomeDirectory() + CONFIG_FILENAME);
    if (!config.is_open()) {
        printf("[net][error] Could not save network settings\n");
        return;
    }

    // Remove `(none)`
    if (netplay.savedServers.size() == 1) {
        if (netplay.savedServers[0].hostname.compare("(none)") == 0)
            netplay.savedServers.clear();
    }

    std::vector<std::string> servers;
    servers.reserve(netplay.savedServers.size());
    for (const ServerAddress& server : netplay.savedServers)
        servers.push_back(server.hostname);

    const toml::value content = toml::table {
        { "player_name", netplay.myPlayerName },
        { "servers", servers },
    };

    config << toml::format(content);
}

void NetConfigManager::load()
{
    toml::value config;
    if (!load_file(config))
        return;

    read_playername(config);
    read_servers(config);
}
