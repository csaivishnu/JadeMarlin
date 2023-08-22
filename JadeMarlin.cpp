#include <iostream>
#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <chrono>
#include <bitset>
#include "headerfiles/mail_box.h"
using namespace std;

// Shared resources
mutex mtx1;
condition_variable cv1;
MailBox commandMailBox;
MailBox responseMailBox;
bitset<1024> sequenceIdBitmask;
int presentIndex;
bool shouldExit;

vector<uint8_t> processCommand(vector<uint8_t> command) {
    vector<uint8_t> response;
    for (auto x : command) {
        response.push_back(2 * x);
    }
    response[0] /= 2;
    return response;
}

// Server function
void server() {
    while (true) {
        vector<uint8_t> command;

        unique_lock<mutex> lock(mtx1);
        cv1.wait(lock, [] { return ((commandMailBox.head == commandMailBox.tail) && (shouldExit == false)) ? false : true; });

        if (shouldExit) {
            sequenceIdBitmask.reset();
            cv1.notify_one();
            break;
        }

        command = dequeue(commandMailBox);
        cout << "Server recieved the command: " << uint32_t(command[0]) << ' ' << uint32_t(command[1]) << ' ' << uint32_t(command[2]) << ' ' << uint32_t(command[3]) << " with payloads: ";
        uint8_t commandSize;
        commandSize = uint8_t(command.size());
        for (uint8_t i = 4; i < commandSize; i++) {
            cout << uint32_t(command[i]) << ' ';
        }
        cout << endl;
        lock.unlock();

        this_thread::sleep_for(chrono::milliseconds(500));

        lock.lock();
        vector<uint8_t> response = processCommand(command);
        enqueue(responseMailBox, response);
        cout << "Server sent the response: " << uint32_t(response[0]) << ' ' << uint32_t(response[1]) << ' ' << uint32_t(response[2]) << ' ' << uint32_t(response[3]) << " with payloads: ";
        uint8_t responseSize;
        responseSize = uint8_t(response.size());
        for (uint8_t i = 4; i < responseSize; i++) {
            cout << uint32_t(response[i]) << ' ';
        }
        cout << endl;
        sequenceIdBitmask.set(command[0]-4, false);
        lock.unlock();
        cv1.notify_one();
    }
}

// Callback function
void callbackFunction(vector<uint8_t> response) {
    cout << "Client recieved the response: " << uint32_t(response[0]) << ' ' << uint32_t(response[1]) << ' ' << uint32_t(response[2]) << ' ' << uint32_t(response[3]) << " with payloads: ";
    uint8_t responseSize;
    responseSize = uint8_t(response.size());
    for (uint8_t i = 4; i < responseSize; i++) {
        cout << uint32_t(response[i]) << ' ';
    }
    cout << endl;
}

// Synchronous function
void synchronousFunction(vector<uint8_t> command, function<void(vector<uint8_t>)> callback) {
    unique_lock<mutex> lock(mtx1);
    cout << "Client sent the command sync: " << uint32_t(command[0]) << ' ' << uint32_t(command[1]) << ' ' << uint32_t(command[2]) << ' ' << uint32_t(command[3]) << " with payloads: ";
    uint8_t commandSize;
    commandSize = uint8_t(command.size());
    for (uint8_t i = 4; i < commandSize; i++) {
        cout << uint32_t(command[i]) << ' ';
    }
    cout << endl;
    enqueue(commandMailBox, command);
    while (sequenceIdBitmask[presentIndex] == true) {
        presentIndex++;
    }
    int sequenceId = presentIndex++;
    sequenceIdBitmask.set(sequenceId, true);
    cv1.notify_one();

    cv1.wait(lock, [sequenceId] { return sequenceIdBitmask.test(sequenceId) ? false : true; });
    vector<uint8_t> response;
    response = dequeue(responseMailBox);
    callback(response);
}

int main() {
    presentIndex = 0;
    shouldExit = false;
    thread serverThread(server);

    vector<uint8_t> command;
    command = { 4,0,0,0 };
    synchronousFunction(command, callbackFunction);

    command = { 5,0,0,0,1 };
    synchronousFunction(command, callbackFunction);

    command = { 6,0,0,0,1,2 };
    synchronousFunction(command, callbackFunction);

    shouldExit = true;
    serverThread.join();
    return 0;
}