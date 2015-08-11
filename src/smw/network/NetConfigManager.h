#ifndef NET_CONFIG_MANAGER_H
#define NET_CONFIG_MANAGER_H

#include "yaml-cpp/yaml.h"

/*

  Network preferences file reader/writer

*/

class NetConfigManager {
public:
    NetConfigManager() {}
    virtual ~NetConfigManager() {}

    void load();
    void save();

private:
    bool load_file(YAML::Node&);
    void read_playername(YAML::Node&);
    void read_servers(YAML::Node&);
};

#endif // NET_CONFIG_MANAGER_H
