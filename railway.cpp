/**
 * Name : Sahiba Khan
  * Roll : 1901920100244
 * compiler : g++ (g++ 20CS60R15_A6.cpp)
 * run command : ./a.out in1.txt in2.txt in3.txt in4.txt
 * operating system : linux (ubuntu (wsl))
 * input : input file name as command line argument. (eg. ./a.out in1.txt in2.txt in3.txt in4.txt)
 **/

#include<iostream>
#include<fstream>
#include<unistd.h>
#include<sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h> // Semaphore
#include <sys/shm.h> // Share memory
#include<string.h>

#define MAX_ALLOWED_PROCESS 1
#define flag int
#define MAX 1000
#define NO_OF_BERTH 10
#define P(s) semop(s, &pop, MAX_ALLOWED_PROCESS)  /* pop is the structure we pass for doing the P(s) operation */
#define V(s) semop(s, &vop, MAX_ALLOWED_PROCESS)  /* vop is the structure we pass for doing the V(s) operation */
#define NO_OF_TRAIN 3
using namespace std;

// Global variable 
    // int *pnr, pnrSemaphore;
    int rSemaphore[NO_OF_TRAIN], rfSemaphore[NO_OF_TRAIN]; 
    int wSemaphore[NO_OF_TRAIN], gSemaphore[NO_OF_TRAIN];
    int shmid, shmid1;
    int *readerCount[NO_OF_TRAIN]; // ARRAY OF POINTER
	struct sembuf pop, vop;


struct Reservation{ // Reservation *ptr
    int pnr;
    char p_name[20], age[3], sex;
    //string name;
    char rClass[4];// AC2, AC3, or SC
    flag status; // 1 -- >waitlisted or 0 --> confirmed, 2 --> cancelled
    Reservation(){
        pnr = -1;
    }
    Reservation(int p, char* n, char* a, char s, char* c){
        pnr = p;
        strcpy(p_name, n);
        strcpy(age, a);
        strcpy(rClass, c);
        sex = s;
        getStatus();
    }
    void getStatus(){

    }
};
struct Train{
    int train_id;
    int AC2, AC3, SC;// No. of available berths
    int curr; // 4
    Reservation rlist[MAX]; // ac2_, ac3, _SC, _ //10 *1000
    void getEmptyReservationList(){
        for(int i = 0; i < MAX; i++){
            rlist[i] = *(new Reservation());
        }
    }
}; // Train t[3]

ifstream fin;
ofstream dout("output.txt");
ofstream bout("bug.txt");
/* Function Prototype */
void getReadLoack(int train_id);
void releaseReadLock(int train_id);
void getWriteLock(int train_id);
void releaseWriteLock(int train_id);
void to_char_arr(string str, char* res);
int split (string str, string delimiter, string res_str[]);

/* Driver Function */
int main(int argc, char* argv[]){

    // Create and initialize semaphores for all (3 / NO_OF_TRAIN) trains
    for(int i = 0; i < NO_OF_TRAIN; i++){
        // Create semaphores.
        rSemaphore[i] = semget(IPC_PRIVATE, 1, 0777|IPC_CREAT); // (OS) create semaphore (keep it to itself--> return an id (111) associated with the semaphore
        rfSemaphore[i] = semget(IPC_PRIVATE, 1, 0777|IPC_CREAT);
        wSemaphore[i] = semget(IPC_PRIVATE, 1, 0777|IPC_CREAT);
        gSemaphore[i] = semget(IPC_PRIVATE, 1, 0777|IPC_CREAT);

        // Initialize the semaphores.
        semctl(rSemaphore[i], 0, SETVAL, 1); // Set semaphore value as 0.
        semctl(rfSemaphore[i], 0, SETVAL, 1);
        semctl(wSemaphore[i], 0, SETVAL, 1);
        semctl(gSemaphore[i], 0, SETVAL, 1);
    }

    // Initialize pop and vop
    pop.sem_num = vop.sem_num = 0;
	pop.sem_flg = vop.sem_flg = SEM_UNDO;
	pop.sem_op = -1 ; vop.sem_op = 1 ;

    // Request for share memory for reader count
    shmid = shmget(IPC_PRIVATE, (NO_OF_TRAIN) * sizeof(int), 0777|IPC_CREAT); // memory(OS) --> return id.

    // Request for share memory for the database
    shmid1 = shmget(IPC_PRIVATE, NO_OF_TRAIN * sizeof(Train), 0777|IPC_CREAT);  // 3* Train size (void) mem (OS) --> return id. 

    // Get share memory for Train details and set up info
    Train *trainArr = (Train*) shmat(shmid1, 0, 0); // (void *) memory for 3 trains
    for(int i = 0; i < NO_OF_TRAIN; i++){ // trianArr[0], trianArr[1], trianArr[2]
        trainArr[i].train_id = i;
        trainArr[i].AC2 = NO_OF_BERTH;
        trainArr[i].AC3 = NO_OF_BERTH;
        trainArr[i].SC = NO_OF_BERTH;
        trainArr[i].curr = 0; // new reservation index
        trainArr[i].getEmptyReservationList(); 
    }

    /* Create 4 processes */
    int my_id;
    int pid = -1, pid1 = -1, pid2 = -1, pid3 = -1;
    pid = fork(); // 2 process
    if(pid == 0){ // 1st process
        try{
            fin.open(argv[1]);
            my_id = 1;
            dout << "hi i am 1st process " << getpid() << ", i open " << argv[1] << endl;
        }
        catch(const std::exception& e){
            std::cerr << e.what() << '\n';
        }
    }
    else{ // Main process
        pid1 = fork();
        if(pid1 == 0){ // 2nd child
            try{
                fin.open(argv[2]);
                my_id = 2;
                dout << "hi i am 2nd process " << getpid() << ", i open " << argv[2] << endl;
            }
            catch(const std::exception& e){
                std::cerr << e.what() << '\n';
            }
        }
        else{ // Main process again
            pid2 = fork();
            if(pid2 == 0){ // 3rd child
                try{
                    fin.open(argv[3]);
                    my_id = 3;
                    dout << "hi i am 3rd process " << getpid() << ", i open " << argv[3] << endl;
                }
                catch(const std::exception& e){
                    std::cerr << e.what() << '\n';
                }
            }
            else{ // Main process again :D
                pid3 = fork();
                if(pid3 == 0){ // 4th child
                    try{
                        fin.open(argv[4]);
                        my_id = 4;
                        dout << "hi i am 4st process " << getpid() <<  ", i open " << argv[4] << endl;
                    }
                    catch(const std::exception& e){
                        std::cerr << e.what() << '\n';
                    }
                }
                else{
                    dout << "I am parent process " << getpid() << endl;
                }
            }
        }
    }


    /* Common work */

        // Get the share memory for readerCount and initialize it to 0
        int *a = (int*) shmat(shmid, 0, 0); //FIRST BOLCK -->[123][456][789]
        for(int i = 0; i < NO_OF_TRAIN; i++){ //0 1 2
            readerCount[i] = a+i; // ADDRESS 
            // *readerCount[i] = 0; --> will be done autometically
        }

        // Get memory location of train again (for peace of mind)
        trainArr = (Train*) shmat(shmid1, 0, 0);


        sleep(0.1); // don't delete.

        // Read the input.
        string temp, temp_arr[10];
        string operation;
        int train_no, rnp;
        char fname[20], lname[20], age[3], sex;
        char rclass[4];
        string rClass;// AC2, AC3, or SC
        // Read input
        while(!fin.eof()){
            if(pid != 0 && pid1 != 0 && pid2 != 0 && pid3 != 0){
                break;
            }
            getline(fin, temp); // temp = "reserve Sontu Mistry 22 M 0 AC2"
                // if input file has empty line ignore that line
                    if(temp == "" || temp == "\r" )continue;
            split(temp, " " , temp_arr); // temp_arr[] = [reserve, Sontu, Mistry, 22 M 0 AC2]
            operation = temp_arr[0];
            if(operation == "reserve" || operation == "Reserve" || operation == "RESERVE"){
                // I have considered name will be not have any space
                to_char_arr(temp_arr[1], fname);
                to_char_arr(temp_arr[2], lname);
                strcat(fname, " ");
                strcat(fname, lname);
                to_char_arr(temp_arr[3], age);
                sex = temp_arr[4][0];
                train_no = stoi(temp_arr[5]);
                temp_arr[6].erase(temp_arr[6].find_last_not_of(" \n\r\t")+1); // ! Important, don't delete
                rClass = temp_arr[6];
                to_char_arr(temp_arr[6], rclass);
                // dout << fname << endl;
            }
            else{
                rnp = stoi(temp_arr[1]);
            }
            dout << operation << endl;
            // Process input
            if(operation == "reserve" || operation == "Reserve" || operation == "RESERVE"){
                getWriteLock(train_no); // Will lock the train database with train_id = "train_no"
                Reservation r; // Create a reservation object.
                strcpy(r.p_name, fname);
                strcpy(r.age, age);
                r.sex = sex;
                strcpy(r.rClass, rclass);

                if(rClass == "AC2" && trainArr[train_no].AC2 > 0){
                    int curr = trainArr[train_no].curr;
                    r.status = 0; // Confirm
                    r.pnr = (curr*10) + train_no; // set PNR
                    trainArr[train_no].rlist[curr] = r;
                    (trainArr[train_no].AC2)--;
                    (trainArr[train_no].curr)++; // Update curr value of train[i] in it's structure.
                    // dout << "Program is running....\nSimulating 1 sec sleep. Don't kill me..." << endl;
                    // sleep(1);
                    cout << "==> Reservation made successfully for passenger \"" << trainArr[train_no].rlist[curr].p_name;
                    cout << "\" in process " << my_id << " (" << getpid() << ") allocated PNR " << trainArr[train_no].rlist[curr].pnr <<".\n";
                }
                else if(rClass == "AC3" && trainArr[train_no].AC3 > 0){
                    int curr = trainArr[train_no].curr;
                    r.status = 0; // Confirm
                    r.pnr = (curr*10) + train_no; // set PNR
                    trainArr[train_no].rlist[curr] = r;
                    (trainArr[train_no].AC3)--;
                    (trainArr[train_no].curr)++; // Update curr value of train[i] in it's structure.
                    // dout << "Program is running....\nSimulating 1 sec sleep. Don't kill me..." << endl;
                    // sleep(1);
                    cout << "==> Reservation made successfully for passenger \"" << trainArr[train_no].rlist[curr].p_name;
                    cout << "\" in process " << my_id << " (" << getpid() << ") allocated PNR " << trainArr[train_no].rlist[curr].pnr <<".\n";
                }
                else if(rClass == "SC" && trainArr[train_no].SC > 0){
                    int curr = trainArr[train_no].curr;
                    r.status = 0; // Confirm
                    r.pnr = (curr*10) + train_no; // set PNR
                    trainArr[train_no].rlist[curr] = r;
                    (trainArr[train_no].SC)--;
                    (trainArr[train_no].curr)++; // Update curr value of train[i] in it's structure.
                    // dout << "Program is running....\nSimulating 1 sec sleep. Don't kill me..." << endl;
                    // sleep(1);
                    cout << "==> Reservation made successfully for passenger \"" << trainArr[train_no].rlist[curr].p_name;
                    cout << "\" in process " << my_id << " (" << getpid() << ") allocated PNR " << trainArr[train_no].rlist[curr].pnr <<".\n";
                }
                else{ // If full
                    int curr = trainArr[train_no].curr;
                    r.status = 1; // Wishlisted
                    r.pnr = (curr*10) + train_no; // set PNR
                    trainArr[train_no].rlist[curr] = r;
                    cout << "++ Added passenger \"" << trainArr[train_no].rlist[curr].p_name;
                    cout << "\" to waitlist by process " << my_id << " (" << getpid() << ").\n";
                    (trainArr[train_no].curr)++;

                }
                releaseWriteLock(train_no); // Release the lock when done.

            }
            else{ // cancelation
                int flag1 = 0;
                int tno, ind;
                
                tno = rnp % 10;
                ind = rnp / 10;
                getWriteLock(tno); // Get the write lock to update the reservation detail. 
                bout << "tno " << tno << endl;
                if(tno <= NO_OF_TRAIN){
                    // Check if given cancellation request is valid or not
                    if(trainArr[tno].rlist[ind].pnr == rnp && trainArr[tno].rlist[ind].status != -1){
                        flag1 = 1;
                    }
                }
                dout << "before delete\n";
                if(flag1 == 0){ // PNR is not found.
                    cout << "Cancellation request for PNR " << rnp << " is invalid. :(" << endl;
                }
                else if(flag1 == 1){ // PNR found and valid. Cancel the reservation.
                    // dout << "valid cancel\n";
                    int next_waiting = ind+1;
                    char* rClass = trainArr[tno].rlist[ind].rClass; // SC, AC2, AC3
                    if(trainArr[tno].rlist[ind].status == 0){ // If cancellation free a berth.
                        int abc = 1;
                        while(trainArr[tno].rlist[next_waiting].pnr != -1 && next_waiting < MAX){ // Find the waiting reservation
                            if(!strcmp(trainArr[tno].rlist[next_waiting].rClass, rClass) && trainArr[tno].rlist[next_waiting].status == 1){
                                trainArr[tno].rlist[next_waiting].status = 0; // Confirm the reservation of the first passenger waiting for the class.
                                cout << "Removed passenger \""<< trainArr[tno].rlist[next_waiting].p_name;
                                cout << "\" from waitlist and status changed to confirmed by process " << my_id << " (" << getpid() << ").\n";
                                break;
                            }
                            // dout << abc++ << " Hello from while loop\n" << endl;
                            next_waiting++;
                        }
                    }
                    trainArr[tno].rlist[ind].status = -1;
                    if(trainArr[tno].rlist[ind].status = -1){ // If everything goes well
                        cout << "Reservation cancelled for PNR " << rnp << " by process " << my_id << " (" << getpid() << ").\n";
                    }
                    flag1 = 0;
                }
                releaseWriteLock(tno);
            }
        }
    dout <<"\thi there " << my_id << endl;

    // Release all shared memory and semaphores
    for(int i = 0; i < NO_OF_TRAIN; i++){
        semctl(rSemaphore[i], 1, IPC_RMID);
        semctl(rfSemaphore[i], 1, IPC_RMID);
        semctl(gSemaphore[i], 1, IPC_RMID);
        semctl(wSemaphore[i], 1, IPC_RMID);
    }
    shmdt(readerCount);
    int ret_val = shmdt(trainArr);

    // Wait all child to terminate.
    int wpid;
    int status = 0;
    while ((wpid = wait(&status)) > 0);
    if(pid != 0 && pid1 != 0 && pid2 != 0 && pid3 != 0)
        dout << "\n***** Output is stored in the file named 'output.txt' *****.\n\n";
    
    if(ret_val == 0 ){ // on success return 0
        cout << "\t Shared memory released. :D\n";
    }
    return 0;
}

void getReadLoack(int train_id){ 
    int i = train_id; 
    /**
     * If reader wants to read then -
     * 1. It will either get blocked (if writer is already in critical section).
     * 2. It will be allowed to read. (if no writer is in critical section)
     * First reader will acquire the global Semaphore (gSemaphore)
     * And last reader will release the gSemaphore.
     * According to the problem statement, If any new writer comes (we can check it by checking the value of waitingSemaphore)
     * if(waitingSemaphore == 0), no new reader will be allowed then.
     **/

    dout << "before v\n";
    int v; 
    // v = semctl(wSemaphore[i], 0, GETVAL);
    // while(v < 0){
    //     v = semctl(wSemaphore[i], 0, GETVAL);
    //     dout << "here...\n";
    // }
    dout << "after v\n";
    P(rSemaphore[i]); // Don't allow multiple reader to enter for sometime. //  r2, r3, r4
    // Check if there is any writer or not if yes go into a inf loop util it can read again. 
    (*readerCount[i])++; 
    bout << "\t" << i <<"reader " << (*readerCount[i]) << endl;
    dout << "inside rsema\n";
    // If no writer is waiting.
    if(*readerCount[i] == 1 ){ // first reader tries to read the data
        dout << "before g down\n";
        P(gSemaphore[i]); // Acquire the global semaphore
        dout << "-->reader get gsem " << i << endl;

    }
    V(rSemaphore[i]); // Allow other reader to enter
    dout << "outside rsema" << endl;
}
void releaseReadLock(int train_id){
    /**
     * last reader will release the gSemaphore.
     * get rfSemaphore do operation and then release it
     **/
    int i = train_id;
    P(rSemaphore[i]);
    (*readerCount[i])--;
    bout << "\n" << i << "reader " << (*readerCount[i]) <<endl;
    if((*readerCount[i]) == 0){
        V(gSemaphore[i]);
        dout << "\treader relase gsem " << i << endl;
    }
    V(rSemaphore[i]);
}

void getWriteLock(int train_id){
    /**
     * Wait for reader to finish reading if they are already in critical section.
     **/
    int i = train_id;
    dout << "writer in " << i << endl;
    P(wSemaphore[i]); // will ensure that no two writer enter the critical section
    dout << "i'm waiting for g sema " << i << endl;
    P(gSemaphore[i]);
    dout << "writer out.\n";
}
void releaseWriteLock(int train_id){
    int i = train_id;
    dout << "inser release write\n";
    V(gSemaphore[i]);
    V(wSemaphore[i]);
    dout << "will relase gsema\n";
    dout << "release gsema " << i << endl ;
}

void to_char_arr(string str, char* res){
    const char *var;
    var = str.c_str();
    strcpy(res, var);
}

int split (string str, string delimiter, string res_str[]){
    /* Split the formatted input string. */    
    string token;
    size_t pos;
    int i = 0;
    while((pos = str.find(delimiter)) != string::npos){
        token = str.substr(0, pos);
        res_str[i++] = token;
        str.erase(0, pos + delimiter.length());
    }
    res_str[i++] = str;
    return i;
}
