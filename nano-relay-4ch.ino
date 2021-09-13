//
//

#include <Arduino.h>
#include <UIPEthernet.h>
#include <PubSubClient.h>
#include "src/Shell-1.2.0/Shell.h"
#include "memory.h"
#include "ip_tools.h"

//MAC адрес устройства
static uint8_t ethernetMac[6] = {0xda, 0x20, 0x3a, 0x22, 0xf8, 0x01};

Memory memory;
EthernetServer ethernetServerTcp(23);
EthernetClient ethernetClientTcp;
EthernetClient ethernetClientMqtt;
PubSubClient mqtt;
long lastReconnectAttempt = 0;
EthernetLinkStatus ethernetLinkStatus;
bool mqttIpIsSet;
char ethernetMacStr[13];

#include "relay.h"
#include "commands.h"
#include "shell_reader.h"
#include "mqtt.h"

void interface_reboot()
{
    EthernetLinkStatus currentEthernetLinkStatus = Ethernet.linkStatus();
    if (currentEthernetLinkStatus != ethernetLinkStatus) {
        asm("JMP 0");
    }
}

void setup()
{
    Serial.begin(9600);
    Serial.println(F("System start..."));

    pinMode(RELAY1, OUTPUT);
    pinMode(RELAY2, OUTPUT);
    pinMode(RELAY3, OUTPUT);
    pinMode(RELAY4, OUTPUT);

    //Генерация строки с mac адресом
    strcat(ethernetMacStr, ((ethernetMac[0] < 0x10 ? "0" : "") + String(ethernetMac[0], HEX)).c_str());
    strcat(ethernetMacStr, ((ethernetMac[1] < 0x10 ? "0" : "") + String(ethernetMac[1], HEX)).c_str());
    strcat(ethernetMacStr, ((ethernetMac[2] < 0x10 ? "0" : "") + String(ethernetMac[2], HEX)).c_str());
    strcat(ethernetMacStr, ((ethernetMac[3] < 0x10 ? "0" : "") + String(ethernetMac[3], HEX)).c_str());
    strcat(ethernetMacStr, ((ethernetMac[4] < 0x10 ? "0" : "") + String(ethernetMac[4], HEX)).c_str());
    strcat(ethernetMacStr, ((ethernetMac[5] < 0x10 ? "0" : "") + String(ethernetMac[5], HEX)).c_str());

    ethernetLinkStatus = Ethernet.linkStatus();
    mqttIpIsSet = isIpSet(memory.getMqttIPAdress());

    //Network init
    if (ethernetLinkStatus == LinkON) {
	Serial.print(F("Getting ip from "));
	//DHCP
	if (memory.getDhcpActive()) {
	    Serial.println(F("DHCP server..."));
	    while(Ethernet.begin(ethernetMac) == 0) {
		interface_reboot();
		Serial.println(F("Retry..."));
	    }
	//Static ip address
	} else {
	    Serial.println(F("memory..."));
	    Ethernet.begin(ethernetMac, memory.getIPAddress(), memory.getGatewayIp(), memory.getGatewayIp(), memory.getSubnetMaskIp());
	}

	delay(300);
    } else {
	Serial.println(F("No link..."));
    }

    //Start network services only if Link is active
    if (ethernetLinkStatus == LinkON) {
	// Start TCP Server on port 23
	ethernetServerTcp.begin();

	mqtt.setClient(ethernetClientMqtt);
	mqtt.setServer(memory.getMqttIPAdress(), memory.getMqttPort());
	mqtt.setCallback(mqtt_callback);
	lastReconnectAttempt = 0;

	Serial.print(F("Start telnet server on "));
	Serial.print(Ethernet.localIP());
	Serial.println();
	shell_init(shell_reader_tcp, shell_writer_tcp, 0);
    } else {
	Serial.println(F("Start uart server..."));
	shell_init(shell_reader_uart, shell_writer_uart, 0);
    }

    shell_register(command_help, PSTR("?"));
    shell_register(command_ip, PSTR("ip"));
    shell_register(command_mqtt, PSTR("mqtt"));
    shell_register(command_relay, PSTR("relay"));
    shell_register(command_reboot, PSTR("reboot"));
}

void loop()
{
    ethernetClientTcp = ethernetServerTcp.available();
    shell_task();
    Ethernet.maintain();
    interface_reboot();

    //Reconnect to mqtt server only if mqtt ip adress in set
    if (ethernetLinkStatus == LinkON && mqttIpIsSet) {
	if (!mqtt.connected()) {
	    long now = millis();
	    if (now - lastReconnectAttempt > 5000) {
		lastReconnectAttempt = now;
		// Attempt to reconnect
		if (mqtt_reconnect()) {
		    lastReconnectAttempt = 0;
		}
	    }
	} else {
	    mqtt.loop();
	}
    }
}
