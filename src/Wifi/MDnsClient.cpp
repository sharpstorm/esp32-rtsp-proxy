#include "MDnsClient.h"

#include "../Config/ConfigManager.h"
#include "mdns.h"

void MDnsClient::start() {
  String dnsName = configManager.getDnsName();
  mdns_init();
  mdns_hostname_set(dnsName.c_str());
  mdns_instance_name_set(dnsName.c_str());
  mdns_service_add(dnsName.c_str(), "_http", "_tcp", 80, NULL, 0);
}

void MDnsClient::stop() { mdns_free(); }
