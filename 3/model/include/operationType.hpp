#pragma once

enum class OperationType {
// UnaryCommand
    INC,
    DEC,
    NEG, 
    NOT,

    MUL, //use GeneralRegisters
    DIV, //use GeneralRegisters
// BinaryCommand:
    MOV, 
    ADD,    
    SUB, 
    CMP,
    AND, 
    OR,
    XOR,
// JumpCommand:
    JMP,
    JE,
// DataDeclaration:
    DB, //declair 1 byte
    DW, //declair 2 byte
    DD, //declair 4 byte
    DQ, //declair 8 byte
// ThreadInit:
    ThreadInit,
// ThreadEnd:
    ThreadEnd,
//
//    +NOP пустая команда
};
