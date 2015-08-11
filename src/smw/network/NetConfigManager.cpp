#include "NetConfigManager.h"

#include "net.h"
#include "path.h"

#include <cassert>
#include <fstream>
#include <string>

void NetConfigManager::save()
{
    assert(netplay.myPlayerName);

    std::ofstream config(GetHomeDirectory() + "servers.yml");
    if (!config.is_open()) {
        printf("[net][error] Could not save network settings\n");
        return;
    }

    YAML::Emitter content;
    content << YAML::BeginMap;
    content << YAML::Key << "player_name";
    content << YAML::Value << netplay.myPlayerName;

    assert(content.good());

    content << YAML::Key << "servers";
    content << YAML::Value << YAML::BeginSeq;

    // Do not save `(none)` at [0]
    for (unsigned i = 1; i < netplay.savedServers.size(); i++)
        content << netplay.savedServers[i].hostname;

    content << YAML::EndSeq;
    content << YAML::EndMap;

    assert(content.good());

    config << content.c_str();
    config.close();
}

void NetConfigManager::load()
{
    YAML::Node config;
    if (!load_file(config))
        return;

    read_playername(config);
    read_servers(config);
}

bool NetConfigManager::load_file(YAML::Node& config) {
    try {
        config = YAML::LoadFile(GetHomeDirectory() + "servers.yml");
        return true;
    }
    catch (YAML::BadFile& error) {
        printf("[net][warning] Could not open servers.yml, using default values.\n");
    }
    catch (...) {
        printf("[net][warning] Something's wrong with servers.yml ...");
    }

    return false;
}

void NetConfigManager::read_playername(YAML::Node& config)
{
    try {
        YAML::Node config_playername = config["player_name"];
        if (config_playername.IsNull())
            return;

        if (!config_playername.IsScalar())
            throw std::runtime_error("player name must be a simple string");

        std::string net_player_name = config_playername.as<std::string>();

        if (net_player_name.length() < 3)
            throw std::runtime_error("player name too short");

        if (net_player_name.length() >= NET_MAX_PLAYER_NAME_LENGTH) {
            std::string err("player name must be less than ");
            err += std::to_string(NET_MAX_PLAYER_NAME_LENGTH);
            err += " letters";
            throw std::runtime_error(err);
        }

        strcpy(netplay.myPlayerName, net_player_name.c_str());
    }
    catch (std::runtime_error& error) {
        printf("[net][warning] servers.yml: %s\n", error.what());
    }
    catch (...) {
        printf("[net][warning] Something's wrong with servers.yml ...");
    }
}

void NetConfigManager::read_servers(YAML::Node& config)
{
    try {
        YAML::Node config_servers = config["servers"];
        if (config_servers.IsNull())
            return;

        if (!config_servers.IsSequence())
            throw std::runtime_error("`servers` is in wrong format");

        for (unsigned i = 0; i < config_servers.size(); i++) {
            std::string address_str = config_servers[i].as<std::string>();

            if (address_str.length() < 8 || address_str.length() > 250) {
                printf("[net][warning] servers.yml: server #%u is invalid\n", i + 1);
                continue;
            }

            ServerAddress host;
            host.hostname = address_str;
            netplay.savedServers.push_back(host);
        }
    }
    catch (std::runtime_error& error) {
        printf("[net][warning] servers.yml: %s\n", error.what());
    }
    catch (...) {
        printf("[net][warning] Something's wrong with servers.yml ...");
    }
}
