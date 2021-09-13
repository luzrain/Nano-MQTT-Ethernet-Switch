
boolean mqtt_reconnect()
{
    char buf[20];
    strcpy(buf, "relay/");
    strcat(buf, ethernetMacStr);
    strcat(buf, "/#");
    if (mqtt.connect("relay")) {
        mqtt.subscribe(buf);
        Serial.println(F("Connected to MQTT..."));
    }
    return mqtt.connected();
}

void mqtt_callback(char* topic, byte* payload, unsigned int payloadLength)
{
    uint8_t relay;
    uint8_t command;
    uint8_t topicLength = strlen(topic);

    if (topicLength == 25 && strstr(topic, "relay1")) {
        relay = RELAY1;
    } else if(topicLength == 25 && strstr(topic, "relay2")) {
        relay = RELAY2;
    } else if(topicLength == 25 && strstr(topic, "relay3")) {
        relay = RELAY3;
    } else if(topicLength == 25 && strstr(topic, "relay4")) {
        relay = RELAY4;
    } else {
        return;
    }

    if (payloadLength == 2 && (char)payload[0] == 'O' && (char)payload[1] == 'N') {
        command = COMMAND_ON;
    } else if (payloadLength == 3 && (char)payload[0] == 'O' && (char)payload[1] == 'F' && (char)payload[2] == 'F') {
        command = COMMAND_OFF;
    } else if (payloadLength == 6 && (char)payload[0] == 'T' && (char)payload[1] == 'O' && (char)payload[2] == 'G' && (char)payload[3] == 'G' && (char)payload[4] == 'L' && (char)payload[5] == 'E') {
        command = COMMAND_TOGGLE;
    } else {
        return;
    }

    if (command == COMMAND_ON) {
        digitalWrite(relay, HIGH);
    } else if (command == COMMAND_OFF) {
        digitalWrite(relay, LOW);
    } else if (command == COMMAND_TOGGLE) {
        digitalWrite(relay, !digitalRead(relay));
    }
}
