#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Converts a float to an integer that has an equivalent binary representation.
 * ie. The bits don't change!
 *
 * @param   float f the float to convert
 * @returns int     the converted integer
 */
int float_to_int(float f) {
    // We abuse void * here, because it really shouldn't mess with the bits
    // during a cast
    void * ptr = (void *) &f;
    int * iptr = (int *) ptr;
    return *iptr;
}

/**
 * Converts an integer into a float that has an equivalent binary
 * representation. ie. The bits don't change!
 *
 * @param  int   i the integer to convert
 * @return float   the converted float
 */
float int_to_float(int i) {
    // We abuse void * here, because it really shouldn't mess with the bits
    // during a cast
    void * ptr = (void *) &i;
    float * fptr = (float *) ptr;
    return *fptr;
}

/**
 * Adds two floating point numbers, in software, step by step. This is based off
 * flowchart.png.
 *
 * @param   float a the first operand
 * @param   float b the second operand
 * @returns float   a + b
 */
float evaluate(float a, float b)
{

    // First we convert a and b to integers, this makes looking at specific bit
    // ranges easy using bit-wise operators.
    unsigned int X = float_to_int(a);
    unsigned int Y = float_to_int(b);

    // If X is +/-0 we can return Y
    if((X & ~0x80000000) == 0)
    {
        return b; // Y
    }

    // If Y is +/-0 we can return X
    if((Y & ~0x80000000) == 0)
    {
        return a; // X
    }

    // We can extract the exponents by ANDing the 8 bits before the sign bit and
    // shifting them all the way back to the right.
    unsigned int exp_X = (X & 0b01111111100000000000000000000000) >> 23;
    unsigned int exp_Y = (Y & 0b01111111100000000000000000000000) >> 23;

    // We can extract Mantissas by ANDing the first 23 bits. We convert these to
    // the significant (of the form 1.M) by ORing a set 24th bit.
    unsigned int sig_X = (X
        & 0b00000000011111111111111111111111)
        | 0b00000000100000000000000000000000;
    unsigned int sig_Y = (Y
        & 0b00000000011111111111111111111111)
        | 0b00000000100000000000000000000000;

    // We need to get the exponent of X to equal the exponent of Y. We will
    // repeat the same process until they do.
    while(exp_X != exp_Y)
    {
        // If the exponent of X is less than the exponent of Y increment it and
        // shift the significand.
        if(exp_X < exp_Y)
        {
            exp_X++;
            sig_X = sig_X >> 1;

            // If the signficand of X reaches 0 then X is insignifanct to Y
            if(sig_X == 0)
            {
                return b; // Y
            }
        }
        // Otherwise the exponent of Y must be less than the exponent of X.
        // Increment the exponent of Y and shift the signfiicand.
        else
        {
            exp_Y++;
            sig_Y = sig_Y >> 1;

            // If the signficand of Y reaches 0 then Y is insignifanct to X
            if(sig_Y == 0)
            {
                return a; // X
            }
        }
    }

    // Once the exponents are equal, we have found our (semi) final exponent.
    unsigned int exp_Z = exp_X = exp_Y;

    // We use long long's to do two's complement addition to X and Y's
    // signficiands, this ensures we have sufficient bit resolution
    // 2^25-2 < 2^63-1
    signed long long sigl_X = (signed long long) sig_X;
    signed long long sigl_Y = (signed long long) sig_Y;

    // If X is negative, swap the signfiicand's sign
    if(X & 0x80000000)
    {
        sigl_X *= -1;
    }
    // If Y is negative, swap the significand's sign
    if(Y & 0x80000000)
    {
        sigl_Y *= -1;
    }

    // The long long form of Z's signficand can now be calculated via addition
    signed long long sigl_Z = sigl_X + sigl_Y;

    // We can now find our final sign
    int sign_Z = sigl_Z < 0 ? 1 : 0;

    // If the sign was negative we need to convert the significand back to a
    // positive number
    if(sigl_Z < 0)
    {
        sigl_Z *= -1;
    }

    // We can now convert the number back to an unisnged value
    unsigned int sig_Z = (unsigned int) sigl_Z;

    // If the significand ended up being 0, then Z is 0
    if(sig_Z == 0)
    {
        return 0;
    }

    // If the 25th bit of the significand is set, then addition overflowed. In
    // this case we need to shift the significand to the left, incremenet the
    // exponent and make sure the exponent is still in range.
    if(sig_Z & 0b1000000000000000000000000)
    {
        sig_Z = sig_Z >> 1;
        exp_Z += 1;

        // If the exponent overflows then Z has overflowed (+/-Infinity)
        if(exp_Z & 0b100000000)
        {
            if(sign_Z > 0)
            {
                return 1.0 / 0.0;
            }
            else
            {
                return -1.0 / 0.0;
            }
        }
    }

    // Now we normalize Z by shifting right and decrementing the previously
    // found exponent until the 24th bit is set, the 1 in 1.M
    while((sig_Z & 0b100000000000000000000000) == 0)
    {
        sig_Z = sig_Z << 1;
        exp_Z -= 1;

        // If the exponent 0's out we have underflow
        if(exp_Z == 0)
        {
            return 0;
        }
    }

    // We OR together all our components, the sign, exponent, and Mantissa, and
    // finall return the value once converted back to a float.
    unsigned int Z = (sign_Z << 31) | (exp_Z << 23)| (sig_Z & 0b11111111111111111111111);
    return int_to_float(Z);
}

/**
 * Prints the binary representation of a IEEE-754 Floating Point number.
 *
 * @param flaot f the float to print
 */
void print_float(float f)
{
    unsigned int i = float_to_int(f);

    printf("| Sign | Exponent | Mantissa                |"
         "\n|------|----------|-------------------------|"
         "\n| %c    | %c%c%c%c%c%c%c%c | %c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c |\n\n",
        // Sign
        (i & 0x80000000) ? '1' : '0',
        // Exponent
        (i & 0x40000000) ? '1' : '0',
        (i & 0x20000000) ? '1' : '0',
        (i & 0x10000000) ? '1' : '0',
        (i & 0x08000000) ? '1' : '0',
        (i & 0x04000000) ? '1' : '0',
        (i & 0x02000000) ? '1' : '0',
        (i & 0x01000000) ? '1' : '0',
        (i & 0x00800000) ? '1' : '0',
        // Mantissa
        (i & 0x00400000) ? '1' : '0',
        (i & 0x00200000) ? '1' : '0',
        (i & 0x00100000) ? '1' : '0',
        (i & 0x00080000) ? '1' : '0',
        (i & 0x00040000) ? '1' : '0',
        (i & 0x00020000) ? '1' : '0',
        (i & 0x00010000) ? '1' : '0',
        (i & 0x00008000) ? '1' : '0',
        (i & 0x00004000) ? '1' : '0',
        (i & 0x00002000) ? '1' : '0',
        (i & 0x00001000) ? '1' : '0',
        (i & 0x00000800) ? '1' : '0',
        (i & 0x00000400) ? '1' : '0',
        (i & 0x00000200) ? '1' : '0',
        (i & 0x00000100) ? '1' : '0',
        (i & 0x00000080) ? '1' : '0',
        (i & 0x00000040) ? '1' : '0',
        (i & 0x00000020) ? '1' : '0',
        (i & 0x00000010) ? '1' : '0',
        (i & 0x00000008) ? '1' : '0',
        (i & 0x00000004) ? '1' : '0',
        (i & 0x00000002) ? '1' : '0',
        (i & 0x00000001) ? '1' : '0'
    );
}

/**
 * Prints a single value.
 */
void parse_value()
{
    float value;
    scanf("%f", &value);
    print_float(value);
    return;
}

/**
 * Calculates and prints an addition expression.
 */
void parse_expression()
{
    float a;
    float b;
    char buffer[4];

    scanf("%f%3s%f", &a, buffer, &b);

    if(
        strcmp(buffer, " + ") != 0 &&
        strcmp(buffer, " +" ) != 0 &&
        strcmp(buffer, "+ " ) != 0 &&
        strcmp(buffer, "+"  ) != 0)
    {
        fprintf(stderr, "Unknown operator %s", buffer);
        return;
    }

    printf("\nNatural evaluation %f + %f = %f\n\n", a, b, a + b);
    print_float(a + b);

    float c = evaluate(a, b);

    printf("Artificial evalution %f + %f = %f\n\n", a, b, c);
    print_float(c);

    return;
}

int main(int argc, char ** argv)
{
    for(;;)
    {
        // Prompt
        printf("IEEE-754 Floating Point tester. Type:"
            "\n    q: to quit"
            "\n    v: <#> to see the FP representaiton of that value"
            "\n    e: <#> + <#> to evaluate an addition expression"
            "\n> "
        );

        // Get input
        char buffer[5];
        memset(buffer, 0, 5);
        scanf("%4s", buffer);

        // Dispatch
        if(strcmp(buffer, "q:") == 0)
        {
            puts("Bye! ^_^"); // User quit nicely
            return 0;
        }
        if(strcmp(buffer, "v:") == 0)
        {
            parse_value();
            continue;
        }
        if(strcmp(buffer, "e:") == 0)
        {
            parse_expression();
            continue;
        }
        fprintf(stderr, "Unknown command %s\n", buffer);
    }
    return -1;
}