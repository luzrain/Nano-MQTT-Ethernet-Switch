bool isIpSet(const IPAddress &address) {
    if (address[0] == 0 && address[1] == 0 && address[2] == 0 && address[3] == 0) {
        return false;
    } else if (address[0] == 255 && address[1] == 255 && address[2] == 255 && address[3] == 255) {
        return false;
    } else {
        return true;
    }
}
