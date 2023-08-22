#include "..\headerfiles\mail_box.h"

MailBox::MailBox() : head(0), tail(0) {}

void enqueue(MailBox& mailBox, std::vector<uint8_t>& data) {
    uint8_t size;
    size = data.size();
    uint8_t bufferSizeUsed;
    bufferSizeUsed = uint8_t((mailBox.head - mailBox.tail + 238) % 238);
    if ((bufferSizeUsed + size) < 238) {
        for (uint8_t index = 0; index < size; index++) {
            mailBox.buffer[mailBox.head++] = data[index];
        }
    }
}

std::vector<uint8_t> dequeue(MailBox& mailBox) {
    std::vector<uint8_t> data;
    uint8_t bufferSizeUsed;
    bufferSizeUsed = uint8_t((mailBox.head - mailBox.tail + 238) % 238);
    if (bufferSizeUsed != 0) {
        uint8_t size;
        size = mailBox.buffer[mailBox.tail];
        for (uint8_t index = 0; index < size; index++) {
            data.push_back(mailBox.buffer[mailBox.tail++]);
        }
    }
    return data;
}