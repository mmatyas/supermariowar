#ifndef NET_CONFIG_MANAGER_H
#define NET_CONFIG_MANAGER_H

#include <string>
#include <list>

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
    void handle_playername(const std::string&);
    void handle_servers(const std::list<std::string>&);
};

#endif // NET_CONFIG_MANAGER_H
