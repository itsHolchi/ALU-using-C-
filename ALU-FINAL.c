#include <stdio.h>
#include <stdlib.h>
#include <math.h>  


//=======GLOBAL VARIABLES=========
unsigned char CF = 0;
unsigned char ZF = 0;
unsigned char OF = 0;
unsigned char SF = 0;
unsigned char ACC = 0;

//======FUNCTION PROTOTYPEs=====
int ALU(unsigned char operand1, unsigned char operand2, unsigned char control_signal);
unsigned char twosComp(unsigned data);
void printBin(int data, unsigned char data_width);

void main(void)
{       
    //=======TEST CASES=========
        ALU(0x03, 0x05, 0x02); // SUB
        //ALU(0x88, 0x85, 0x01); // ADD
        //ALU(0x46, 0x02, 0x03); // MUL
        //ALU(0x10, 0x10, 0x04); // AND
        //ALU(0x10, 0xC0, 0x05); // OR
        //ALU(0x10, 0xC0, 0x06); // NOT
        //ALU(0x10, 0xC0, 0x07); // XOR
        //ALU(0x10, 0x10, 0x08); // Shift left
        //ALU(0x10, 0x02, 0x09); // Shift right
}

//=======FLAGS=========
void setFlags(unsigned char ACC) 
{ 
        if (ACC == 0x0000) // check if value of ACC is equal to 0 
                ZF = 1; 
        
        if ((ACC & 0x0080) == 0x0080) // check if sign (8th bit) of ACC is 1 (negative) 
                ZF = 0;
                
        // Check if the sign bit (8th bit) of ACC is 1 (negative)
        if (ACC & 0x80) {  
                SF = 1; 
        } else {
                SF = 0; 
        }

        // Check if ACC is greater than or equal to 0xFF
        if (ACC >= 0xFF) {
                CF = 1;  
        } else {
                CF = 0; 
        }

        // Check if ACC is greater than or equal to 0x7F
        if (ACC >= 0x7F) {
                OF = 1; 
        } else {
                OF = 0; 
        }
}

//=======ALU FUNCTION=========
int ALU(unsigned char operand1, unsigned char operand2, unsigned char control_signal)
{
        static unsigned int ACC;
        unsigned char temp_OP1 = 0x00, temp_OP2 = 0x00;
        
        int count = 0;

        // Setting ACC and flags to initial values
        ACC = 0x000; 
        SF = 0; 
        CF = 0; 
        ZF = 0; 
        OF = 0;

        printf("\n*********************\n");
        printf("Fetching operands...\n");
        printf("OP1 = ");
        printBin(operand1, 8);
        printf("\nOP2 = ");
        printBin(operand2, 8);
        printf("\n");


        //======ADDITION AND SUBTRACTION=========
        if (control_signal == 0x01 || control_signal == 0x02) 
        {
                temp_OP1 = operand1;

                if (control_signal == 0x02) // check if operation is SUB
                {
                        printf("\nOperation: SUB\n");
                        temp_OP2 = twosComp(operand2); // 2’s complement operand2
                        printf("2's Complement OP2\n");
                        ACC = temp_OP2;
                }
                else
                {
                        temp_OP2 = operand2;
                        printf("\nOperation: ADD\n");
                        ACC = temp_OP1 + temp_OP2; 
                        printf("\n");
                }
        }

        //=======MULTIPLICATION=========
        else if (control_signal == 0x03)
        {
                printf("Operation: MUL\n");
                printf("\tA\t");
                printf("Q\t");
                printf("Q_n1\t");
                printf("M\t");
                printf("n\n");
                unsigned char temp_prod, temp_OP2, op1, Q_0, Q_n1; 
                unsigned char LSB_A, MSB_A;
                int cycle;

                temp_OP2 = operand2;
                temp_OP1 = operand1;
                temp_prod = 0x00;
                Q_n1 = 0;

                // 4 cycles because using 4 bits
                for (cycle = 0; cycle < 8; cycle++)
                { 
                        LSB_A = temp_prod & (0x01); // saving the LSB of A (temp product)
                        Q_0 = temp_OP2 & (0x01); // saving LSB of Q (multiplier)

                        // Printing the iterations
                        printf("  ");
                        printBin(temp_prod, 8);
                        printf("  ");
                        printBin(temp_OP2, 8);
                        printf(" \t");
                        printBin(Q_n1, 1);
                        printf("   ");
                        printBin(temp_OP1, 8);
                        printf("\t");
                        printf("%d", cycle);
                        printf("\n");

                        // Shifting if Q0 and Q_n1 == 0 shift temp_prod (A) and Q
                        if ((Q_0 == 0) && (Q_n1 == 0) || (Q_0 == 1) && (Q_n1 == 1))
                        {
                                MSB_A = temp_prod & (0x80); // saving the MSB of A (temp product)

                                // SHIFTING
                                temp_prod >>= 1;
                                temp_OP2 >>= 1;

                                temp_prod = temp_prod | (MSB_A << 7); // shift msb 7x
                                temp_OP2 = temp_OP2 | (LSB_A << 7);
                                if (MSB_A != 0) {
                                        temp_prod = temp_prod | 0x80;
                                }
                                Q_n1 = Q_0;
                        }
                        // A <- A - M
                        else if ((Q_0 == 1) && (Q_n1 == 0))
                        { 
                                temp_prod = temp_prod + twosComp(temp_OP1);

                                // A <- A - M
                                // Before we shift:
                                MSB_A = temp_prod & (0x80); // saving the MSB of A (temp product)

                                temp_prod >>= 1;
                                temp_OP2 >>= 1;

                                temp_prod = temp_prod | (MSB_A << 7); // shift msb 7x
                                temp_OP2 = temp_OP2 | (LSB_A << 7);
                                                
                                if (MSB_A != 0) {
                                        temp_prod = temp_prod | 0x80;
                                }
                                Q_n1 = Q_0;
                        }
                        else if ((Q_0 == 0) && (Q_n1 == 1))
                        { 
                                temp_prod = temp_prod + temp_OP1; 

                                MSB_A = temp_prod & (0x80); // saving the MSB of A (temp product)

                                temp_prod >>= 1;
                                temp_OP2 >>= 1;

                                temp_prod = temp_prod | (MSB_A << 7); // shift msb 7x
                                temp_OP2 = temp_OP2 | (LSB_A << 7);
                                if (MSB_A != 0) {
                                        temp_prod = temp_prod | 0x80;
                                }

                                Q_n1 = Q_0;
                        }
                }

                // Printing the iterations
                printf("  ");
                printBin(temp_prod, 8);
                printf("  ");
                printBin(temp_OP2, 8);
                printf(" \t");
                printBin(Q_n1, 1);
                printf("   ");
                printBin(temp_OP1, 8);
                printf("\t");
                printf("%d", 8);
                printf("\n"); 

                // Merging ACC = A(temp_prod) and Q(temp_op2), 8 bits each. So ACC will become 16 bits
                ACC = (temp_prod << 8) | temp_OP2;
        }

        // LOGICAL OPERATORS

        // AND
        else if (control_signal == 0x04)
        {
                printf("\nOperation: AND");
                ACC = operand1 & operand2;
        }

        // OR
        else if (control_signal == 0x05) 
        {
                printf("\nOperation: OR");
                ACC = operand1 | operand2;
        }

        // NOT
        else if (control_signal == 0x06)
        {
                printf("\nOperation: NOT");
                ACC = ~operand1;
        }

        // XOR
        else if (control_signal == 0x07)
        {
                printf("\nOperation: XOR");
                ACC = operand1 ^ operand2;
        }

        // Shift Left
        else if (control_signal == 0x08)
        { 
                printf("\nOperation: Shift Left");
                ACC = operand1 << 1;
        }

        // Shift Right
        else if (control_signal == 0x09)
        {
                printf("\nOperation: Shift Right");
                ACC = operand1 >> 1;
        }
        else
        {
                printf("\nInvalid Control Signal");
        }

        printf("\nACC: "); // Print the value of ACC in binary format
        printBin(ACC, 16); // ACC is 16-bit
        setFlags(ACC); // Set flags based on the value of temp_OP
        printf("\nZF = %d, CF = %d, SF = %d, OF = %d\n", ZF, CF, SF, OF);
}

//=======TWOS COMPLIMENT=========
unsigned char twosComp(unsigned data) // Function to calculate the two's complement of an unsigned integer
{
        unsigned char op2;
        op2 = ~data + 1; // Calculate the two's complement
        return op2; // Return the result
}


//=======PRINTING=========
void printBin(int data, unsigned char data_width) 
{
        unsigned int i; 

        // Start the loop from the most significant bit (MSB) to the least significant bit (LSB)
        for (i = 1 << (data_width - 1); i > 0; i = i >> 1) {
                // Check if the current bit is set or not and print accordingly
                if (data & i) {
                        printf("1"); 
                } else {
                        printf("0"); 
                }
        }
}