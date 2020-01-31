#include <stack>
#include <vector>
#include <stdint.h>

struct ExceptionGen : public std::exception
{
   std::string s;
   ExceptionGen(std::string ss) : s(ss) {}
   ~ExceptionGen() throw () {} // Updated
   const char* what() const throw() { return s.c_str(); }
};

typedef enum Mnemonic{
    READ, WRITE, DUP, MUL, ADD, SUB, GT, LT, EQ, JMPZ, PUSH, POP, ROT
    }Mnemonic;

typedef struct Ins {
    Mnemonic op;
    int32_t arg = 0;
} Ins;

class Interpreter
{
private:
    std::stack<int32_t> mem;
    Ins* program;
    int program_size;
    int pc;

public:
    Interpreter(Ins* program, int size);
    void run();
    void terminate(bool*);
    void print_stack(Ins ins);
};
