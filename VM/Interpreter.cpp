#include <string>
#include <iostream>
#include <sstream>
#include "Interpreter.h"


Interpreter::Interpreter(Ins* prog, int size)
{
    program = prog;
    program_size = size;
    pc = 0;
}

void Interpreter::print_stack(Ins ins)
{
    int size = mem.size();
    int32_t copy[size];
    std::string op;

    switch(ins.op)
    {
        case READ: {op ="READ"; break;} 
        case WRITE: {op = "WRITE"; break;}
        case DUP: {op = "DUP"; break;}
        case MUL: {op = "MUL"; break;}
        case ADD: {op = "ADD"; break;}
        case SUB: {op = "SUB"; break;}
        case GT: {op = "GT"; break;}
        case LT: {op = "LT"; break;}
        case EQ: {op = "EQ"; break;}
        case JMPZ: {op = "JMPZ"; break;}
        case PUSH: {op = "PUSH"; break;}
        case POP: {op = "POP"; break;}
        case ROT: {op = "ROT"; break;}
    }

    std::cout << op << " " << ins.arg << "\n|";
    for(int i = 0; i < size; ++i)
    {
        copy[i] = mem.top();
        mem.pop();
    }

    for(int i = size-1; i >= 0; --i)
    {
        std::cout << copy[i] << "|";
        mem.push(copy[i]);
    }
    std::cout << "\n" << std::endl;
}

void Interpreter::terminate(bool* running)
{
    *running = pc < (program_size);
}

void Interpreter::run()
{
    bool running = true;

    while(running)
    {
        Ins ins = program[pc++];

        switch (ins.op)
        {
            case READ:
            {
                std::string input;
                std::getline(std::cin, input);

                try {
                    int32_t int_read = stoi(input);
                    mem.push(int_read);
                }
                catch (std::invalid_argument)
                {
                    std::ostringstream exception;
                    exception << "Invalid argument for: READ" << ", instruction " << (pc - 1) << std::endl;

                    throw ExceptionGen(exception.str());
                }
            }
            break;

            case WRITE:
            {
                int32_t out = mem.top();
                mem.pop();

                std::cout << out << std::endl;
            }
            break;

            case DUP:
            {
                int32_t dup = mem.top();
                mem.push(dup);
            }
            break;

            case MUL:
            {
                int32_t arg_top = mem.top();
                mem.pop();

                int32_t arg_second = mem.top();
                mem.pop();

                int32_t res = arg_top*arg_second;

                mem.push(res);
            }
            break;

            case ADD:
            {
                int32_t arg_top = mem.top();
                mem.pop();

                int32_t arg_second = mem.top();
                mem.pop();

                int32_t res = arg_top + arg_second;
                mem.push(res);
            }
            break;

            case SUB:
            {
                int32_t arg_top = mem.top();
                mem.pop();

                int32_t arg_second = mem.top();
                mem.pop();

                int32_t res = arg_top - arg_second;
                mem.push(res);
            }
            break;

            case GT:
            {
                int32_t arg_top = mem.top();
                mem.pop();

                int32_t arg_second = mem.top();
                mem.pop();

                int32_t res = (arg_top > arg_second) ? 1 : 0;
                mem.push(res);
            }
            break;

            case LT:
            {
                int32_t arg_top = mem.top();
                mem.pop();

                int32_t arg_second = mem.top();
                mem.pop();

                int32_t res = (arg_top < arg_second) ? 1 : 0;
                mem.push(res);
            }
            break;

            case EQ:
            {
                int32_t arg_top = mem.top();
                mem.pop();

                int32_t arg_second = mem.top();
                mem.pop();

                int32_t res = (arg_top == arg_second) ? 1 : 0;
                mem.push(res);
            }
            break;

            case JMPZ:
            {
                int32_t arg_top = mem.top();
                mem.pop();

                int32_t arg_second = mem.top();
                mem.pop();

                if(arg_second == 0)
                {
                    if(arg_top <= program_size && arg_top >= 0)
                    {
                        pc = arg_top;
                    }
                    else
                    {
                        std::ostringstream exception;
                        exception << "Jump to Illegal region: JMP " << arg_top << ", instruction " << pc-1 << std::endl;

                        throw ExceptionGen(exception.str());
                    }
                    
                }
            }
            break;

            case PUSH:
            {
                mem.push(ins.arg);
            }
            break;

            case POP:
            {
                int32_t s = ins.arg;

                if (s <= mem.size())
                {
                    while(s-- > 0)
                    {
                        mem.pop();
                    }
                }
                else
                {
                    std::ostringstream exception;
                    exception << "Illegal access error: POP " << s << ", instruction " << (pc-1) << std::endl;

                    throw ExceptionGen(exception.str());
                }
            }
            break;

            case ROT:
            {
                int32_t rot_size = ins.arg;

                if (rot_size <= mem.size())
                {
                    int32_t temp_mem[rot_size];

                    for (int item = 0; item < rot_size; ++item )
                    {
                        int32_t val = mem.top();
                        mem.pop();

                        temp_mem[ (rot_size - item) % rot_size] = val;
                    }

                    for (int i = 0; i < rot_size; ++i)
                    {
                        mem.push(temp_mem[i]);
                    }
                }
                else
                {
                    std::ostringstream exception;
                    exception << "Illegal access error: ROT " << rot_size << ", instruction " << (pc-1) << std::endl;

                    throw ExceptionGen(exception.str());
                }
                
            }
            break;

            default:
            {
                std::ostringstream exception;
                exception << "Illegal instruction: " << (pc-1) << std::endl;

                throw ExceptionGen(exception.str());
            }
            break;
        }

        terminate(&running);

        //print_stack(ins);
    }

}
