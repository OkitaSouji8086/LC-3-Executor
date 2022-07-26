#include <bitset>
#include <string>
#include <iostream>
using namespace std;

#define INIT 0x7777
#define getReg(bit2, bit1, bit0) reg[inst[bit2] * 4 + inst[bit1] * 2 + inst[bit0]]
typedef bitset<16> lc3int;
typedef enum
{
    ADD,
    AND,
    BR,
    JMP,
    JSR,
    LD,
    LDI,
    LDR,
    LEA,
    NOT,
    ST,
    STI,
    STR,
    HALT
} ISA;

lc3int PSR, PC;
lc3int reg[8] = {INIT, INIT, INIT, INIT, INIT, INIT, INIT, INIT};
lc3int mem[0xffff];

int str2int(string inst);
int decodeInst(lc3int opcode);
lc3int lc3Add(lc3int num1, lc3int num2);
lc3int SEXT(int bits, lc3int inst);
void setCC(lc3int result);

void instADD(lc3int inst)
{
    if (inst[5])
        getReg(11, 10, 9) = lc3Add(getReg(8, 7, 6), SEXT(5, inst));
    else
        getReg(11, 10, 9) = lc3Add(getReg(8, 7, 6), getReg(2, 1, 0));
    setCC(getReg(11, 10, 9));
}
void instAND(lc3int inst)
{
    if (inst[5])
        getReg(11, 10, 9) = getReg(8, 7, 6) & SEXT(5, inst);
    else
        getReg(11, 10, 9) = getReg(8, 7, 6) & getReg(2, 1, 0);
    setCC(getReg(11, 10, 9));
}
void instBR(lc3int inst)
{
    if ((inst[11] & PSR[2]) || (inst[10] & PSR[1]) || (inst[9] & PSR[0]))
        PC = lc3Add(PC, SEXT(9, inst));
}
void instJMP(lc3int inst)
{
    PC = getReg(8, 7, 6);
}
void instJSR(lc3int inst)
{
    reg[7] = PC;
    if (inst[11])
        PC = lc3Add(PC, SEXT(11, inst));
    else
        PC = getReg(8, 7, 6);
}
void instLD(lc3int inst)
{
    getReg(11, 10, 9) = mem[lc3Add(PC, SEXT(9, inst)).to_ulong()];
    setCC(getReg(11, 10, 9));
}
void instLDI(lc3int inst)
{
    getReg(11, 10, 9) = mem[mem[lc3Add(PC, SEXT(9, inst)).to_ulong()].to_ulong()];
    setCC(getReg(11, 10, 9));
}
void instLDR(lc3int inst)
{
    getReg(11, 10, 9) = mem[lc3Add(getReg(8, 7, 6), SEXT(6, inst)).to_ulong()];
    setCC(getReg(11, 10, 9));
}
void instLEA(lc3int inst)
{
    getReg(11, 10, 9) = lc3Add(PC, SEXT(9, inst));
}
void instNOT(lc3int inst)
{
    getReg(11, 10, 9) = ~getReg(8, 7, 6);
    setCC(getReg(11, 10, 9));
}
void instST(lc3int inst)
{
    mem[lc3Add(PC, SEXT(9, inst)).to_ulong()] = getReg(11, 10, 9);
}
void instSTI(lc3int inst)
{
    mem[mem[lc3Add(PC, SEXT(9, inst)).to_ulong()].to_ulong()] = getReg(11, 10, 9);
}
void instSTR(lc3int inst)
{
    mem[lc3Add(getReg(8, 7, 6), SEXT(6, inst)).to_ulong()] = getReg(11, 10, 9);
}

int main()
{
    string inst;
    lc3int instCount;
    lc3int opcode;
    lc3int oprand;
    lc3int getOpcode(0xf000);
    lc3int getOprand(0xfff);
    lc3int HALT(0xf000);
    int buffer;

    getline(cin, inst);
    instCount = PC = str2int(inst);
    while (getline(cin, inst))
    {
        mem[instCount.to_ulong()] = str2int(inst);
        instCount = instCount.to_ulong() + 1;
    }

    do
    {
        opcode = mem[PC.to_ulong()] & getOpcode;
        oprand = mem[PC.to_ulong()] & getOprand;
        PC = PC.to_ulong() + 1;
        switch (decodeInst(opcode))
        {
        case ADD:
            instADD(oprand);
            break;
        case AND:
            instAND(oprand);
            break;
        case BR:
            instBR(oprand);
            break;
        case JMP:
            instJMP(oprand);
            break;
        case JSR:
            instJSR(oprand);
            break;
        case LD:
            instLD(oprand);
            break;
        case LDI:
            instLDI(oprand);
            break;
        case LDR:
            instLDR(oprand);
            break;
        case LEA:
            instLEA(oprand);
            break;
        case NOT:
            instNOT(oprand);
            break;
        case ST:
            instST(oprand);
            break;
        case STI:
            instSTI(oprand);
            break;
        case STR:
            instSTR(oprand);
            break;
        default:
            break;
        }
    } while (opcode != HALT);

    for (int i = 0; i < 8; i++)
    {
        buffer = reg[i].to_ulong();
        printf("R%d = x%04hX\n", i, buffer);
    }

    return 0;
}

int str2int(string inst)
{
    int str2int(0);
    string::iterator it(inst.begin());
    while (it != inst.end())
    {
        str2int *= 2;
        str2int += *it - '0';
        it++;
    }
    return str2int;
}

int decodeInst(lc3int opcode)
{
    static lc3int ADD(0x1000), AND(0x5000), BR(0x0000), JMP(0xc000), JSR(0x4000), LD(0x2000), LDI(0xa000), LDR(0x6000), LEA(0xe000), NOT(0x9000), ST(0x3000), STI(0xb000), STR(0x7000), HALT(0xf000);
    if (opcode == ADD)
        return ISA::ADD;
    else if (opcode == AND)
        return ISA::AND;
    else if (opcode == BR)
        return ISA::BR;
    else if (opcode == JMP)
        return ISA::JMP;
    else if (opcode == JSR)
        return ISA::JSR;
    else if (opcode == LD)
        return ISA::LD;
    else if (opcode == LDI)
        return ISA::LDI;
    else if (opcode == LDR)
        return ISA::LDR;
    else if (opcode == LEA)
        return ISA::LEA;
    else if (opcode == NOT)
        return ISA::NOT;
    else if (opcode == ST)
        return ISA::ST;
    else if (opcode == STI)
        return ISA::STI;
    else if (opcode == STR)
        return ISA::STR;
    else if (opcode == HALT)
        return ISA::HALT;
}

lc3int lc3Add(lc3int num1, lc3int num2)
{
    lc3int lc3Add(0x0);
    bitset<2> sum(0x0);
    for (int i = 0; i < 16; i++)
    {
        sum = sum[1] + num1[i] + num2[i];
        lc3Add[i] = sum[0];
    }
    return lc3Add;
}

lc3int SEXT(int bits, lc3int inst)
{
    lc3int SEXT(0x0);
    if (inst[bits - 1])
        SEXT.set();
    for (int i = bits - 2; i > -1; i--)
        SEXT[i] = inst[i];

    return SEXT;
}

void setCC(lc3int result)
{
    PSR.reset();
    if (result.none())
        PSR.set(1);
    else if (result[15])
        PSR.set(2);
    else
        PSR.set(0);
}