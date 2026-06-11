#ifndef NET_CONFIG_MANAGER_H
#define NET_CONFIG_MANAGER_H

/*

  Network preferences file reader/writer

*/

class NetConfigManager {
public:
    virtual ~NetConfigManager() = default;

    void load();
    void save();
};

#endif // NET_CONFIG_MANAGER_H
