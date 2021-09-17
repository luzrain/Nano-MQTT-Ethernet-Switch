#define COMMAND_ON 1
#define COMMAND_OFF 2
#define COMMAND_TOGGLE 3

char * mqtt_topic(const char * num, const char * type)
{
    static char mqttTopicBuffer[43];
    memset(mqttTopicBuffer, 0, sizeof(mqttTopicBuffer));
    strcat(mqttTopicBuffer, memory.getName());
    strcat(mqttTopicBuffer, "/relay");
    strcat(mqttTopicBuffer, num);
    strcat(mqttTopicBuffer, "/");
    strcat(mqttTopicBuffer, type);
    return mqttTopicBuffer;
}

relay_switch(uint8_t relay, uint8_t command)
{
    char *topic;

    if (relay == RELAY1) {
        topic = mqtt_topic("1", "stat");
    } else if (relay == RELAY2) {
        topic = mqtt_topic("2", "stat");
    } else if (relay == RELAY3) {
        topic = mqtt_topic("3", "stat");
    } else if (relay == RELAY4) {
        topic = mqtt_topic("4", "stat");
    }

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

bool relay_status(uint8_t relay)
{
    return digitalRead(relay) == HIGH;
}

boolean mqtt_reconnect()
{
    char lwtTopicBuffer[36];
    strcat(lwtTopicBuffer, memory.getName());
    strcat(lwtTopicBuffer, "/LWT");

    if (mqtt.connect(memory.getName(), lwtTopicBuffer, 0, true, "offline")) {
        mqtt.subscribe(mqtt_topic("1", "set"));
        mqtt.subscribe(mqtt_topic("2", "set"));
        mqtt.subscribe(mqtt_topic("3", "set"));
        mqtt.subscribe(mqtt_topic("4", "set"));
        mqtt.publish(lwtTopicBuffer, "online");
        Serial.println(F("Connected to MQTT..."));
    }

    return mqtt.connected();
}

void mqtt_callback(char* topic, byte* payload, unsigned int payloadLength)
{
    uint8_t command;

    if (payloadLength == 2 && (char)payload[0] == 'O' && (char)payload[1] == 'N') {
        command = COMMAND_ON;
    } else if (payloadLength == 3 && (char)payload[0] == 'O' && (char)payload[1] == 'F' && (char)payload[2] == 'F') {
        command = COMMAND_OFF;
    } else if (payloadLength == 6 && (char)payload[0] == 'T' && (char)payload[1] == 'O' && (char)payload[2] == 'G' && (char)payload[3] == 'G' && (char)payload[4] == 'L' && (char)payload[5] == 'E') {
        command = COMMAND_TOGGLE;
    } else {
        return;
    }

    if (strcmp(topic, mqtt_topic("1", "set")) == 0) {
        relay_switch(RELAY1, command);
    } else if(strcmp(topic, mqtt_topic("2", "set")) == 0) {
        relay_switch(RELAY2, command);
    } else if(strcmp(topic, mqtt_topic("3", "set")) == 0) {
        relay_switch(RELAY3, command);
    } else if(strcmp(topic, mqtt_topic("4", "set")) == 0) {
        relay_switch(RELAY4, command);
    } else {
        return;
    }
}
