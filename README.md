# Welcome to the ResourceBinding wiki!

## # **Goal**

In this Project we simulate the operation scheduling and resource binding that usually used in a compiler.

## # **Example**

1. In the input line, a user inputs a calcualtion opeartion like "a+b-c*d/e" within operators range of "+-*/".
2. In the process, we use different scheduling algorithm to schedule each operator. for example, for operation like "a+b-c*d/e", "c*d" will be performed first and hense will be scheduled in the cycle 0. And so on, we will schedule each opeartor occured in the operation.
3. In the output, we show the scheduled cycle for each operator and a latency to show how long to finish the whole input operation.

## # **Program Process**

![Resource Binding Work flow](https://github.com/allen1031/ResrouceBinding/blob/master/ConsoleApplication3/resource_binding.png)


### # **ASAP(As Soon As Possible) Algorithm:**
1. Find a candidate node.
   1.1. Candidate is a node whose predecessors have been scheduled and completed or has no predecessors.
2. Schedule node one cycle later than max cycle of predecessor. 
3. Repeat until all nodes scheduled.

### # **ALAP(As Late As Possible) Algorithm:**
1. Run ASAP, get minimum latency L.
2. Find a candidate (here, candidate is  a node whose successors are scheduled or has no successors). 
3. Schedule node one cycle before min cycle of a predecessor.
   3.1. Nodes with no successors scheduled to cycle L.
4. Repeat until all nodes scheduled.

## **Limited Situation**
The program doesn't solve the operations with "()". The priority of operators that this program follows is "*" = "/" > "+" = "-". 

