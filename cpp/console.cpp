//
// Created by os on 7/24/22.
//

#include "../h/console.hpp"

myBuffer* console::inputBuffer = nullptr;
myBuffer* console::outputBuffer = nullptr;
TCB* console::consoleThreadInput = nullptr;
TCB* console::consoleThreadOutput = nullptr;

void console::init() {
    inputBuffer = myBuffer::createBuffer(1024);
    outputBuffer = myBuffer::createBuffer(1024);
    uint64 * stack1 = (uint64*)MemoryAllocator::malloc(MemoryAllocator::bytesToBlocks(DEFAULT_STACK_SIZE));
    TCB::createThread(&consoleThreadInput, consoleHandlerInput, nullptr, stack1);
    uint64 * stack2 = (uint64*)MemoryAllocator::malloc(MemoryAllocator::bytesToBlocks(DEFAULT_STACK_SIZE));
    TCB::createThread(&consoleThreadOutput, consoleHandlerOutput, nullptr, stack2);
}

void console::consoleHandlerInput(void*) {
    privilagedMode();
    __asm__ volatile("csrs sstatus, 0x2");
    while (true) {
        uint8 *status = (uint8 *) CONSOLE_STATUS;
        while (*status & CONSOLE_RX_STATUS_BIT) {
            uint8 *data = (uint8 *) CONSOLE_RX_DATA;
            inputBuffer->put((char) (*data));
        }
        TCB::yield();
    }
}

void console::consoleHandlerOutput(void*) {
    privilagedMode();
    __asm__ volatile("csrs sstatus, 0x2");
    while(true) {
        uint8 *status = (uint8 *) CONSOLE_STATUS;
        while (*status & CONSOLE_TX_STATUS_BIT) {
            if(outputBuffer->getCnt()==0) break;
            uint8 *data = (uint8 *) CONSOLE_TX_DATA;
            *data = (char) (outputBuffer->get());
        }
        TCB::yield();
    }
}

void console::waitTillTheEnd() {
    while(outputBuffer->getCnt() != 0){
        TCB::yield();
    }
}

