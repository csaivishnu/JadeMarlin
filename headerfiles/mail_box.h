#ifndef MAIL_BOX_H
#define MAIL_BOX_H

#include <iostream>
#include <vector>

struct MailBox {
    uint32_t IRQ_IN_SET;							// 0-3 bytes
    uint32_t IRQ_OUT_SET;							// 4-7 bytes
    uint32_t IRQ_IN_CLR;							// 8-11 bytes
    uint32_t IRQ_OUT_CLR;							// 12-15 bytes
    uint8_t head;
    uint8_t tail;
    uint8_t buffer[238];

    MailBox();
};

void enqueue(MailBox& mailBox, std::vector<uint8_t>& commandData);
std::vector<uint8_t> dequeue(MailBox& mailBox);

#endif
