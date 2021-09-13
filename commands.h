
#include <WString.h>


void printIpAdress(const IPAddress &address)
{
    char buf[4];
    utoa(address[0], buf, 10);
    shell_print(buf);
    shell_print(".");
    utoa(address[1], buf, 10);
    shell_print(buf);
    shell_print(".");
    utoa(address[2], buf, 10);
    shell_print(buf);
    shell_print(".");
    utoa(address[3], buf, 10);
    shell_print(buf);
}

int command_help(int argc, char** argv)
{
    shell_println_pm(PSTR("System commands"));
    shell_println_pm(PSTR("---------------"));
    shell_println_pm(PSTR("?\tip\tmqtt\trelay\treboot"));

    return SHELL_RET_SUCCESS;
}

int command_ip(int argc, char** argv)
{
    uint8_t i;

    // Parse command arguments
    for(i = 0; i < argc; i++) {
        if(i==0) {
            continue;
        }

        // DHCP
        else if(!strcmp(argv[i], "-D")) {
            bool dhcp = !strcmp(argv[i + 1], "enable");
            memory.setDhcpActive(dhcp);
        }

        // ipv4 address
        else if(!strcmp(argv[i], "-i")) {
            IPAddress ip;
            ip.fromString(argv[i + 1]);
            memory.setIPAddress(ip);
        }

        // subnet mask
        else if(!strcmp(argv[i], "-s")) {
            IPAddress ip;
            ip.fromString(argv[i + 1]);
            memory.setSubnetMaskIp(ip);
        }

        // gateway
        else if(!strcmp(argv[i], "-g")) {
            IPAddress ip;
            ip.fromString(argv[i + 1]);
            memory.setGatewayIp(ip);
        }
    }

    shell_print_pm(PSTR("Usage: "));
    shell_println_pm(PSTR("ip [-D <enable | disable>] [-i <ipv4 address>] [-s <subnet mask>] [-g <gateway ip>]"));
    shell_println_pm(PSTR("\nActive IPv4 settings"));
    shell_println_pm(PSTR("--------------------"));
    shell_print_pm(PSTR("  Active IPv4 address:\t\t"));
    printIpAdress(Ethernet.localIP());
    shell_println_pm(PSTR(""));
    shell_print_pm(PSTR("  Active IPv4 subnet mask:\t"));
    printIpAdress(Ethernet.subnetMask());
    shell_println_pm(PSTR(""));
    shell_print_pm(PSTR("  Active IPv4 gateway:\t\t"));
    printIpAdress(Ethernet.gatewayIP());
    shell_println_pm(PSTR(""));
    shell_println_pm(PSTR("\nManually configured IPv4 settings"));
    shell_println_pm(PSTR("---------------------------------"));
    shell_print_pm(PSTR("  DHCP:\t\t\t"));
    shell_println_pm(memory.getDhcpActive() ? PSTR("enable") : PSTR("disable"));
    shell_print_pm(PSTR("  IPv4 Address:\t\t"));
    printIpAdress(memory.getIPAddress());
    shell_println_pm(PSTR(""));
    shell_print_pm(PSTR("  Subnet Mask: \t\t"));
    printIpAdress(memory.getSubnetMaskIp());
    shell_println_pm(PSTR(""));
    shell_print_pm(PSTR("  Gateway: \t\t"));
    printIpAdress(memory.getGatewayIp());
    shell_println_pm(PSTR(""));

    return SHELL_RET_SUCCESS;
}

int command_mqtt(int argc, char** argv)
{
    uint8_t i;

    // Parse command arguments
    for(i = 0; i < argc; i++) {
        if(i==0) {
            continue;
        }

        // MQTT server
        else if(!strcmp(argv[i], "-s")) {
            IPAddress ip;
            ip.fromString(argv[i + 1]);
            memory.setMqttIPAdress(ip);
        }

        // MQTT port
        else if(!strcmp(argv[i], "-p")) {
            memory.setMqttPort(String(argv[i + 1]).toInt());
        }
    }

    char mqttPort[6];
    utoa(memory.getMqttPort(), mqttPort, 10);

    shell_print_pm(PSTR("Usage: "));
    shell_println_pm(PSTR("mqtt [-s <ipv4 address>] [-p <port>]"));
    shell_println_pm(PSTR("\nMQTT settings"));
    shell_println_pm(PSTR("--------------"));
    shell_print_pm(PSTR("  MQTT Server:\t\t"));
    printIpAdress(memory.getMqttIPAdress());
    shell_print_pm(PSTR(":"));
    shell_println(mqttPort);
    shell_print_pm(PSTR("  Relay 1 topic: \trelay/"));
    shell_print(ethernetMacStr);
    shell_println_pm(PSTR("/relay1"));
    shell_print_pm(PSTR("  Relay 2 topic: \trelay/"));
    shell_print(ethernetMacStr);
    shell_println_pm(PSTR("/relay2"));
    shell_print_pm(PSTR("  Relay 3 topic: \trelay/"));
    shell_print(ethernetMacStr);
    shell_println_pm(PSTR("/relay3"));
    shell_print_pm(PSTR("  Relay 4 topic: \trelay/"));
    shell_print(ethernetMacStr);
    shell_println_pm(PSTR("/relay4"));
    shell_println_pm(PSTR("  Payload: \t\t<ON|OFF|TOGGLE>"));

    return SHELL_RET_SUCCESS;
}

int command_relay(int argc, char** argv)
{
    uint8_t relay = 0;
    uint8_t command = 0;
    char topic[26];
    strcpy(topic, "relay/");
    strcat(topic, ethernetMacStr);
    strcat(topic, "/relay");

    if (argc == 3) {
        if (!strcmp(argv[1], "1")) {
            relay = RELAY1;
            strcat(topic, "1");
        } else if(!strcmp(argv[1], "2")) {
            relay = RELAY2;
            strcat(topic, "2");
        } else if(!strcmp(argv[1], "3")) {
            relay = RELAY3;
            strcat(topic, "3");
        } else if(!strcmp(argv[1], "4")) {
            relay = RELAY4;
            strcat(topic, "4");
        }

        if (!strcmp(argv[2], "ON")) {
            command = COMMAND_ON;
        } else if(!strcmp(argv[2], "OFF")) {
            command = COMMAND_OFF;
        } else if(!strcmp(argv[2], "TOGGLE")) {
            command = COMMAND_TOGGLE;
        }
    }

    if (relay != 0 && command != 0) {
        if (command == COMMAND_ON) {
            digitalWrite(relay, HIGH);
        } else if (command == COMMAND_OFF) {
            digitalWrite(relay, LOW);
        } else if (command == COMMAND_TOGGLE) {
            digitalWrite(relay, !digitalRead(relay));
        }

        if (mqtt.connected()) {
            mqtt.publish(topic, digitalRead(relay) == HIGH ? "ON" : "OFF");
        }
    }

    shell_print_pm(PSTR("Usage: "));
    shell_println_pm(PSTR("relay [<1|2|3|4> <ON|OFF|TOGGLE>]"));
    shell_println_pm(PSTR("\nRelay status"));
    shell_println_pm(PSTR("------------"));
    shell_print_pm(PSTR("  Relay 1:\t"));
    shell_println_pm(digitalRead(RELAY1) == HIGH ? PSTR("ON") : PSTR("OFF"));
    shell_print_pm(PSTR("  Relay 2:\t"));
    shell_println_pm(digitalRead(RELAY2) == HIGH ? PSTR("ON") : PSTR("OFF"));
    shell_print_pm(PSTR("  Relay 3:\t"));
    shell_println_pm(digitalRead(RELAY3) == HIGH ? PSTR("ON") : PSTR("OFF"));
    shell_print_pm(PSTR("  Relay 4:\t"));
    shell_println_pm(digitalRead(RELAY4) == HIGH ? PSTR("ON") : PSTR("OFF"));

    return SHELL_RET_SUCCESS;
}

int command_reboot(int argc, char** argv)
{
    asm("JMP 0");
    return SHELL_RET_SUCCESS;
}
