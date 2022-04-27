# CS4323-Spring2022 GroupD
[GitHub](https://github.com/Dalbr00n/CS4323FGP/tree/master)
### Members: 
- Lucas Stott       lstott@okstate.edu
- Nathan Bales      nathan.bales@okstate.edu
- Drew Nguyen       drew.nguyen@okstate.edu
- Daniel Albrecht   daniel.albrecht@okstate.edu

Project Purpose:
1. Learn Mutex/Semaphore Synchonization and Scheduling 
2. Build ThreadPooling Methods
3. Collorate within Team 
4. Understand Deadlock/Starvation Problems for (Consumer/Producer)

## Running the Program <br />
> gcc main.c -o main -lpthread && ./main 2 100 3 3 10 100
> Arguments (2, 100, 3, 3, 10 , 100)
> 1. Number Medical Proffesions (Nm)
> 2. Number of Patients (Np)
> 3. Waiting Room Capacity (Nw)
> 4. Number of Sofas (Ns)
> 5. Maximum Arrival Time Between Patients (ms)
> 6. Check-up Duration (ms)


### Thread Pools 
- [X] PatientQueue
- [X] MedicalProfessionalsQueue

### Functions Patient
- [X] enterWaitingRoom()
- [X] sitOnSofa()
- [X] getMedicalCheckup()
- [X] makePayment()
- [X] leaveClinic()

### Functions Doctor
- [X] waitForPatients()
- [X] performMedicalCheckup()
- [X] acceptPayment()
  
### Tasks Lucas 
- [X] Create Main Function for Args()
- [X] Create Independent Functions 
- [X] Start Performing Synchronization

### Tasks Nathan 
- [X] Concurrency Threading 
- [X] Performing Analysis 
- [X] Syncing Global Variables with Threads

### Tasks Daniel
- [X] Semaphore/Mutex Lock Implementing 
- [X] Sync Doctor/Patients 
- [X] Structuring Queue's 

### Tasks Drew 
- [X] Validating Threads Generation
- [X] Gracefully Exit Threads
- [X] Testing Shared Memory Exposure 

### Potential Errors
1. Passing Patient Information to Doctor
2. Possible Variable Overwriting on Globals

### Fixed
1. Patient and Doctor Threads Exited  
