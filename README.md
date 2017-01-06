#  Producer - Consumer Simulation

## Author: [nefari0uss](https://www.github.com/nefari0uss)

[![Open Source Love](https://badges.frapsoft.com/os/mit/mit.svg?v=102)](https://github.com/ellerbrock/open-source-badge/)
 
## Summary
This simulation was to help learn about threading in C using producers and consumers. Producers create and place an int in a buffer. Consumers will attempt to remove ints from the buffer. 

## Compilation Instructions
* Compile: gcc producer-consumer.c -o pc.out -lpthread -lrt
* Run: ./lab3 x y z where x,y, and z is some integer specifying the sleep time, number of producers, number of consumers.

