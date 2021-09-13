#include <EEPROM.h>

#define DHCP_OFFSET 0 //1 byte
#define IP_OFFSET 1 //4 byte
#define SUBNET_OFFSET 5 //4 byte
#define GATEWAY_OFFSET 9 //4 byte
#define MQTT_IP_OFFSET 13 //4 byte
#define MQTT_PORT_OFFSET 17 //2 byte

class Memory {
    public:
    bool setDhcpActive(bool isActive);
    bool getDhcpActive();
    void setIPAddress(IPAddress ip);
    IPAddress getIPAddress();
    void setSubnetMaskIp(IPAddress ip);
    IPAddress getSubnetMaskIp();
    void setGatewayIp(IPAddress ip);
    IPAddress getGatewayIp();
    void setMqttIPAdress(IPAddress ip);
    IPAddress getMqttIPAdress();
    void setMqttPort(uint16_t port);
    uint16_t getMqttPort();
};

bool Memory::setDhcpActive(bool isActive)
{
    uint8_t val = isActive ? 1 : 0;
    EEPROM.write(DHCP_OFFSET, val);
    return isActive;
}

bool Memory::getDhcpActive()
{
    return EEPROM.read(DHCP_OFFSET) != 0;
}

void Memory::setIPAddress(IPAddress ip)
{
    EEPROM.write(IP_OFFSET, ip[0]);
    EEPROM.write(IP_OFFSET + 1, ip[1]);
    EEPROM.write(IP_OFFSET + 2, ip[2]);
    EEPROM.write(IP_OFFSET + 3, ip[3]);
}

IPAddress Memory::getIPAddress()
{
    IPAddress ip(EEPROM.read(IP_OFFSET), EEPROM.read(IP_OFFSET + 1), EEPROM.read(IP_OFFSET + 2), EEPROM.read(IP_OFFSET + 3));
    return ip;
}

void Memory::setSubnetMaskIp(IPAddress ip)
{
    EEPROM.write(SUBNET_OFFSET, ip[0]);
    EEPROM.write(SUBNET_OFFSET + 1, ip[1]);
    EEPROM.write(SUBNET_OFFSET + 2, ip[2]);
    EEPROM.write(SUBNET_OFFSET + 3, ip[3]);
}

IPAddress Memory::getSubnetMaskIp()
{
    IPAddress ip(EEPROM.read(SUBNET_OFFSET), EEPROM.read(SUBNET_OFFSET + 1), EEPROM.read(SUBNET_OFFSET + 2), EEPROM.read(SUBNET_OFFSET + 3));
    return ip;
}

void Memory::setGatewayIp(IPAddress ip)
{
    EEPROM.write(GATEWAY_OFFSET, ip[0]);
    EEPROM.write(GATEWAY_OFFSET + 1, ip[1]);
    EEPROM.write(GATEWAY_OFFSET + 2, ip[2]);
    EEPROM.write(GATEWAY_OFFSET + 3, ip[3]);
}

IPAddress Memory::getGatewayIp()
{
    IPAddress ip(EEPROM.read(GATEWAY_OFFSET), EEPROM.read(GATEWAY_OFFSET + 1), EEPROM.read(GATEWAY_OFFSET + 2), EEPROM.read(GATEWAY_OFFSET + 3));
    return ip;
}

void Memory::setMqttIPAdress(IPAddress ip)
{
    EEPROM.write(MQTT_IP_OFFSET, ip[0]);
    EEPROM.write(MQTT_IP_OFFSET + 1, ip[1]);
    EEPROM.write(MQTT_IP_OFFSET + 2, ip[2]);
    EEPROM.write(MQTT_IP_OFFSET + 3, ip[3]);
}

IPAddress Memory::getMqttIPAdress()
{
    IPAddress ip(EEPROM.read(MQTT_IP_OFFSET), EEPROM.read(MQTT_IP_OFFSET + 1), EEPROM.read(MQTT_IP_OFFSET + 2), EEPROM.read(MQTT_IP_OFFSET + 3));
    return ip;
}

void Memory::setMqttPort(uint16_t port)
{
    EEPROM.put(MQTT_PORT_OFFSET, port);
}

uint16_t Memory::getMqttPort()
{
    uint16_t port;
    EEPROM.get(MQTT_PORT_OFFSET, port);
    if (port == 0 || port == 65535) {
	port = 1883;
    }
    return port;
}