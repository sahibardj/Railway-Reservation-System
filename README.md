# Railway-Reservation-System
 

Problem Definition



The main aim is to make a system that not only has good locking so there is no rollback happening on database side but to also have a pure concurrent system of child processes, such that their requests are handled properly.
    • To study the reservation status, a reader must acquire a read lock, while a writer must acquire a write lock.
    • Processes must wait for their desired lock if they don't receive it immediately.
    • Checking for deadlock to not occur.


Project Specification




1. Write locks are exclusive -- a process can obtain a write lock only when no process has a read or write lock.
2. Read locks are shared -- one or more processes can obtain read locks when no process has a write lock.
3. Writers have higher priority -- new readers are not granted as long as there are waiting writers.
4. Locks are non-preemptive

Existing System 

Indian Railway Catering and Tourism Corporation(IRCTC) is an Indian public sector undertaking. that provides ticketing, catering, and tourism services for the Indian Railways. It was initially wholly owned by the Government of India and operated under the administrative control of the Ministry of Railways, but has been listed on the National Stock Exchange since 2019, with the Government continuing to hold majority ownership.
It pioneered internet-based rail ticket booking through its website, as well as from the mobile phones via WiFi, GPRS or SMS. It also provides SMS facility to check PNR status and Live Train Status as well. In addition to e-tickets, Indian Railways Catering and Tourism Corporation also offers I-tickets that are basically like regular tickets except that they are booked online and delivered by post. The tickets PNR status is also made available. Commuters on the suburban rail can also book season tickets through the website. It has also launched a loyalty program called Shubh Yatra for frequent travelers. Through this program, passengers can avail discounts on all tickets booked round the year by paying an upfront annual fee.
Seeking to make it easier to book e-tickets, It launched a scheme called Rolling Deposit Scheme (RDS). RDS is an e-ticket booking scheme allowing passengers to reserve seats against advance money kept with the corporation. It has also added flights and hotels booking facilities to their line of online reservation services.


System Design and Analysis

There is no doubt that IRCTC is a fairly complicated system with numerous drawbacks. It is considerably difficult for a layperson to understand the system as a whole. This project involves developing software to deal with critical sections and is easy-to-understand.
Main tasks involve system that can handle:
    • Developing server side to handle clients' requests.
    • Developing client side that will autonomously make bookings or cancellations.
    • Handling concurrency of client multiple processes through server.
Analysis of the system by checking how well its handling allocation and reading of multiple processes.

Implementation



RUNNING COMMAND & SYSTEM:
 compiler : g++ (g++ 20CS60R15_A6.cpp)
 run command : ./a.out in1.txt in2.txt in3.txt in4.txt
 operating system : linux (ubuntu)
  input : input file name as command line argument. (eg. ./a.out in1.txt in2.txt in3.txt in4.txt)

Conclusion


The program successfully reads input from multiple input text files in order to generate output while reserving or canceling reservation in a train represented by different processes.
Read and write locks are being successfully granted with out being stuck in deadlock.
Thus the system is successfully implemented. 
