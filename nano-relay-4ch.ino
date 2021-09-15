//
//

#include <Arduino.h>
#include <UIPEthernet.h>
#include <PubSubClient.h>
#include "src/Shell-1.2.0/Shell.h"
#include "src/memory.h"
#include "src/ip_tools.h"

//MAC адрес устройства
static uint8_t ethernetMac[6] = {0xda, 0x20, 0x3a, 0x22, 0xf8, 0x01};

Memory memory;
EthernetServer ethernetServerTcp(23);
EthernetClient ethernetClientTcp;
EthernetClient ethernetClientMqtt;
PubSubClient mqtt;
long lastReconnectAttempt = 0;
long buttonScan = 0;
EthernetLinkStatus ethernetLinkStatus;
bool mqttIpIsSet;
char ethernetMacStr[13];

#include "src/relay.h"
#include "src/shell_reader.h"
#include "src/mqtt.h"
#include "src/commands.h"
#include "src/buttons.h"

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
    pinMode(BUTTON1, INPUT_PULLUP);
    pinMode(BUTTON2, INPUT_PULLUP);
    pinMode(BUTTON3, INPUT_PULLUP);
    pinMode(BUTTON4, INPUT_PULLUP);

    //Генерация строки с mac адресом
    for (uint8_t i = 0; i < 6; i++) {
        if (ethernetMac[i] < 0x10) {
            strcat(ethernetMacStr, "0");
        }
        strcat(ethernetMacStr, String(ethernetMac[i], HEX).c_str());
    }

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

    //Регистрация консольных комманд
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

    //Переподключение в mqtt серверу, раз в десять секунд
    if (ethernetLinkStatus == LinkON && mqttIpIsSet) {
        if (!mqtt.connected()) {
            long now = millis();
            if (now - lastReconnectAttempt > 10000) {
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

    //Сканирование кнопок
    long nowButtonTime = millis();
    if (nowButtonTime - buttonScan > 50) {
        buttonScan = nowButtonTime;
        scan_buttons();
    }
}
