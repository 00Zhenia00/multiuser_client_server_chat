#pragma once

#include <string>

const int MAX_MESSAGE_SIZE = 1024;
const int MAX_NAME_SIZE = 128;

enum MessageType {
    NONE,
    JOIN,
    OK,
    CONNECT,
    ACCEPT,
    DENY,
    PING,
    TEXT,
};

struct Message {
    MessageType type = MessageType::NONE;
    char name[MAX_NAME_SIZE] = "";
    char text[MAX_MESSAGE_SIZE] = "";
};
