char * mqtt_topic(char * num)
{
    static char mqttTopicBuffer[38];
    memset(mqttTopicBuffer, 0, sizeof(mqttTopicBuffer));
    strcat(mqttTopicBuffer, memory.getName());
    strcat(mqttTopicBuffer, "/relay");
    strcat(mqttTopicBuffer, num);
    return mqttTopicBuffer;
}

boolean mqtt_reconnect()
{
    char lwtTopicBuffer[36];
    strcat(lwtTopicBuffer, memory.getName());
    strcat(lwtTopicBuffer, "/LWT");

    if (mqtt.connect(memory.getName(), lwtTopicBuffer, 0, false, "offline")) {
        mqtt.subscribe(mqtt_topic("1"));
        mqtt.subscribe(mqtt_topic("2"));
        mqtt.subscribe(mqtt_topic("3"));
        mqtt.subscribe(mqtt_topic("4"));
        mqtt.publish(lwtTopicBuffer, "online");
        Serial.println(F("Connected to MQTT..."));
    }

    return mqtt.connected();
}

void mqtt_callback(char* topic, byte* payload, unsigned int payloadLength)
{
    uint8_t relay;
    uint8_t command;

    if (strcmp(topic, mqtt_topic("1")) == 0) {
        relay = RELAY1;
    } else if(strcmp(topic, mqtt_topic("2")) == 0) {
        relay = RELAY2;
    } else if(strcmp(topic, mqtt_topic("3")) == 0) {
        relay = RELAY3;
    } else if(strcmp(topic, mqtt_topic("4")) == 0) {
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
