#include <EEPROM.h>

#define DHCP_OFFSET 0 //1 byte
#define IP_OFFSET 1 //4 byte
#define SUBNET_OFFSET 5 //4 byte
#define GATEWAY_OFFSET 9 //4 byte
#define MQTT_IP_OFFSET 13 //4 byte
#define MQTT_PORT_OFFSET 17 //2 byte
#define NAME_OFFSET 19 //33 byte

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
    void setName(char * data);
    char * getName();
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

void Memory::setName(char * data)
{
    uint8_t size = strlen((char *) data);

    if (size > 32) {
        size = 32;
    }

    for (uint8_t i=0; i < size; i++) {
        EEPROM.write(NAME_OFFSET + i, data[i]);
    }

    EEPROM.write(NAME_OFFSET + size, '\0');
}

char * Memory::getName()
{
    static char data[32]; //Max 32 Bytes
    uint8_t len = 0;
    char k;

    do {
        k = EEPROM.read(NAME_OFFSET + len);
        if (len == 0 && (k == 0xFFFF || k == 0 || k == 20)) {
            break;
        }
        data[len++] = k;
    } while (k != '\0' && len < 32);

    //Генерация строки по умолчанию
    if (data[0] == 0) {
        strcat(data, "nano_relay");
    }

    return data;
}
