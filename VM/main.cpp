#include <iostream>
#include "Interpreter.h"

Ins factorial_alt[29] = {
    {READ},
    {DUP},
    {PUSH, 0},
    {LT},
    {PUSH, 28},
    {JMPZ},
    {DUP},
    {PUSH, 1},
    {ROT, 2},
    {ROT, 3},
    {DUP},
    {ROT, 3},
    {ROT, 2},
    {DUP},
    {ROT, 5},
    {GT},
    {PUSH, 27},
    {JMPZ},
    {DUP},
    {ROT, 3},
    {MUL},
    {ROT, 2},
    {PUSH, 1},
    {ADD},
    {PUSH, 0},
    {PUSH, 8},
    {JMPZ},
    {ROT, 2},
    {WRITE},
};

/*
Ins factorial_implicit[43] = {
    {READ},
    {DUP},
    {PUSH, 0},
    {LT},
    {DUP},
    {PUSH, 42},
    {JMPZ},
    {POP, 1},
    {DUP},
    {PUSH, 0},
    {PUSH, 1},
    {ADD},
    {DUP},
    {ROT, 3},
    {ROT, 2},
    {DUP},
    {ROT, 4},
    {EQ},
    {PUSH, 10},
    {JMPZ},
    {ROT, 3},
    {POP, 2},
    {PUSH, 1},
    {PUSH, 1},
    {DUP},
    {ROT, 3},
    {MUL},
    {ROT, 2},
    {PUSH, 1},
    {ADD},
    {ROT, 2},
    {ROT, 3},
    {DUP},
    {ROT, 3},
    {ROT, 2},
    {DUP},
    {ROT, 5},
    {ROT, 2},
    {GT},
    {PUSH, 24},
    {JMPZ},
    {ROT, 2},
    {WRITE}
};
*/


int main(int argc, char** argv)
{
    Interpreter *interpreter = new Interpreter(factorial_alt, 29);
    //Interpreter *interpreter = new Interpreter(factorial_implicit, 43);
    

    try{
        interpreter->run();   
    }
    catch (ExceptionGen exception)
    {
        std::cout << exception.what() << std::endl;
    }

    return 0;
}