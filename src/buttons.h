#define BUTTON1_FLAG 0b10000000
#define BUTTON2_FLAG 0b01000000
#define BUTTON3_FLAG 0b00100000
#define BUTTON4_FLAG 0b00010000

uint8_t buttonState1 = 0;
uint8_t buttonState2 = 0;

void scan_buttons()
{
    buttonState1 = 0;
    uint8_t bits = 0;

    if (digitalRead(BUTTON1) == LOW) {
        buttonState1 |= BUTTON1_FLAG;
        bits++;
    }

    if (digitalRead(BUTTON2) == LOW) {
        buttonState1 |= BUTTON2_FLAG;
        bits++;
    }

    if (digitalRead(BUTTON3) == LOW) {
        buttonState1 |= BUTTON3_FLAG;
        bits++;
    }

    if (digitalRead(BUTTON4) == LOW) {
        buttonState1 |= BUTTON4_FLAG;
        bits++;
    }

    //Не обрабатываем нажатие больше одной кнопки за раз
    if (bits > 1) {
        return;
    }

    //Если состояние кнопок не изменилось с прошлого сканирования
    if (buttonState1 == buttonState2) {
        return;
    }

    uint8_t relay;

    if (buttonState1 == BUTTON1_FLAG) {
        relay = RELAY1;
    }

    if (buttonState1 == BUTTON2_FLAG) {
        relay = RELAY2;
    }

    if (buttonState1 == BUTTON3_FLAG) {
        relay = RELAY3;
    }

    if (buttonState1 == BUTTON4_FLAG) {
        relay = RELAY4;
    }

    if (buttonState1 == BUTTON1_FLAG || buttonState1 == BUTTON2_FLAG || buttonState1 == BUTTON3_FLAG || buttonState1 == BUTTON4_FLAG) {
        relay_switch(relay, COMMAND_TOGGLE);
    }

    buttonState2 = buttonState1;
}