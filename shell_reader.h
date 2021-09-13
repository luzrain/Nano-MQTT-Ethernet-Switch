int shell_reader_uart(char * data)
{
    if (Serial.available()) {
	*data = Serial.read();
	return 1;
    }
    return 0;
}

void shell_writer_uart(char data)
{
    Serial.write(data);
}

int shell_reader_tcp(char * data)
{
    int ret = 0;
    if (ethernetClientTcp == true && ethernetClientTcp.connected()) {
	if (ethernetClientTcp.available()) {
	    *data = ethernetClientTcp.read();
	    ret = 1;
	}
    } else {
	ethernetClientTcp.stop();
    }
    return ret;
}

void shell_writer_tcp(char data)
{
    // If we can write to a client
    if(ethernetClientTcp == true && ethernetClientTcp.connected()) {
	// Wrapper for Serial.write() method
	ethernetClientTcp.write(data);
    }
}