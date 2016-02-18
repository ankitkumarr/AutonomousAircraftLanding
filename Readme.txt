Ankit Kumar	akumar3@wpi.edu


Project assumptions:

1) Planes can became declare emergency regardless of their fuel left
2) Planes with emergency will be given highest priority and NO OTHER flights will initiate landing until it has finished
3) If multiple planes have requested emergency, the one that has requested the latest will be landed the first.
4) If a plane enters a danger zone, it still has to wait if other plane has emergency activity requested.

Runway number starts from 0

Used:

4 binary semaphores for solving using semaphores
4 mutexes for solving using mutexes
