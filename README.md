# HM1-webshell-bruteForce
The algorithm works by generating all possible combinations of characters for the key and testing each combination against the encrypted payload. 

To find the key, a brute force approach can be used, which involves trying every possible combination until the correct key is found. I have written a C/C++ algorithm code to perform this task, using a multi-threaded approach to expedite the process.

## Methodology

The process is split into multiple threads, each handling a different range of characters for the key, in order to make use of the available processing power and speed up the search.

The code is designed to run on a powerful machine with multiple cores or processors, allowing it to efficiently handle the large number of calculations required to brute force the key. It is recommended to use a VM with enough resources 

However, it is important to note that this approach can still take a significant amount of time, depending on the length and complexity of the key.

Overall, the use of a multi-threaded brute force algorithm can be an effective way to decrypt webshell payloads and uncover the malicious activity being carried out by threat actors.


