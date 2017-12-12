# IEEE-754-Addition

A software implementation of IEEE 754 Addition. I created this file to breakdown
to myself the adding process for floating point numbers prior to my final for
the University of Victoria's CENG 355 class.

This implementation is based off the flowchart provided in class, cand makes
some assumptions about how to handle 0s, overflows, and underflows. The goal
of this code is not to provide an ideal implementation, but instead help give
me a better understanding of the generall process.

## Building & Running

On windows (assuming you have MinGW) you can run `build.bat` to compile and
run the program. On a unix platform you can use:

```bash
$ gcc -o fp ./fp
$ ./fp
```

## Using the Program

You can execute two commands, `v:`, which prints out the binary representaiton
of a following floating point value, and `e:` which calculates and prints out
the result of floatign point addition. Here are a couple of examples:

```
IEEE-754 Floating Point tester. Type:
    q: to quit
    v: <#> to see the FP representaiton of that value
    e: <#> + <#> to evaluate an addition expression
> v: 1
| Sign | Exponent | Mantissa                |
|------|----------|-------------------------|
| 0    | 01111111 | 00000000000000000000000 |

IEEE-754 Floating Point tester. Type:
    q: to quit
    v: <#> to see the FP representaiton of that value
    e: <#> + <#> to evaluate an addition expression
> v: 15244.99
| Sign | Exponent | Mantissa                |
|------|----------|-------------------------|
| 0    | 10001100 | 11011100011001111110110 |

IEEE-754 Floating Point tester. Type:
    q: to quit
    v: <#> to see the FP representaiton of that value
    e: <#> + <#> to evaluate an addition expression
> e: 0.00624 + -97942.12

Natural evaluation 0.006240 + -97942.117188 = -97942.110947

| Sign | Exponent | Mantissa                |
|------|----------|-------------------------|
| 1    | 10001111 | 01111110100101100001110 |

Artificial evalution 0.006240 + -97942.117188 = -97942.117188

| Sign | Exponent | Mantissa                |
|------|----------|-------------------------|
| 1    | 10001111 | 01111110100101100001111 |

IEEE-754 Floating Point tester. Type:
    q: to quit
    v: <#> to see the FP representaiton of that value
    e: <#> + <#> to evaluate an addition expression
```

## Contact

**Email:** root@ejrbuss.net

**Website:** ejrbuss.net

## License

MIT, don't use this code for anything real, it will undoubtely fail you.