#include "process_manager.h"

/*
 * Function 1: Basic Producer-Consumer Demo
 * Creates one producer child (sends 1,2,3,4,5) and one consumer child (adds them up)
 */
int run_basic_demo(void) {
    int pipe_fd[2];
    pid_t producer_pid, consumer_pid;
    int status;
    
    printf("\nParent process (PID: %d) creating children...\n", getpid());
    
    // TODO 1: Create a pipe for communication
    // HINT: Use pipe(pipe_fd), check return value
    // if (pipe(pipe_fd) == -1) {
    //     perror("pipe");
    //     return -1;
    // }
    if(pipe(pipe_fd) == -1){
        perror("pipe");
        return -1;
	}

    // TODO 2: Fork the producer process
    // HINT: producer_pid = fork();
    // Child calls: producer_process(pipe_fd[1], 1);  // Start with number 1
    // Child must close pipe_fd[0] (read end)
    // Parent prints: "Created producer child (PID: %d)"
    producer_pid = fork();
    if(producer_pid == 0){
     producer_process(pipe_fd[1], 1);
     close(pipe_fd[0]);
     exit(0);
     }else if(producer_pid > 0){
       printf("Created producer child (PID: %d)", producer_pid);
        }else {
         perror("fork failed");
       }

    // TODO 3: Fork the consumer process
    // HINT: consumer_pid = fork();
    // Child calls: consumer_process(pipe_fd[0], 0);  // Pair ID 0 for basic demo
    // Child must close pipe_fd[1] (write end)
    // Parent prints: "Created consumer child (PID: %d)"
    consumer_pid = fork();
    if(consumer_pid == 0){
      consumer_process(pipe_fd[0], 0);
      close(pipe_fd[0]);
      exit(0);
     }else if(consumer_pid > 0){
      printf("Created producer child (PID: %d)", consumer_pid);
     }else {
      perror("fork failed");
       }

    // TODO 4: Parent cleanup - close pipe ends and wait for children
    // HINT: close(pipe_fd[0]); close(pipe_fd[1]);
    // Use waitpid() twice to wait for both specific children
    // Print exit status for each child
     pid_t consp  = waitpid(consumer_pid, &status, 0);
     pid_t prodp  = waitpid(producer_pid, &status, 0);
     printf("Child %d exited with status %d\n", prodp, status);
     printf("Child %d exited with status %d\n", consp, status);
         
    return 0;
}

/*
 * Function 2: Multiple Producer-Consumer Pairs
 * Creates multiple pairs: pair 1 uses numbers 1-5, pair 2 uses 6-10, etc.
 */
int run_multiple_pairs(int num_pairs) {
    pid_t pids[10]; // Store all child PIDs
    int pid_count = 0;

    printf("\nParent creating %d producer-consumer pairs...\n", num_pairs);

    // TODO 5: Create multiple producer-consumer pairs
    // HINT: Use a for loop from i=0 to i<num_pairs
    // For each iteration:
    //   - Create a new pipe
    //   - Fork producer: calls producer_process(write_fd, i*5 + 1)
    //     So pair 1 starts with 1, pair 2 starts with 6, pair 3 starts with 11
    //   - Fork consumer: calls consumer_process(read_fd, i+1)
    //   - Store both PIDs in pids array, increment pid_count
    //   - Parent closes both pipe ends
    //   - Print "=== Pair %d ===" for each pair
    int pipe_fd[2];
    pid_t producer_pid, consumer_pid;
    int status;
    for(int i = 0; i<num_pairs; i++){
      if(pipe(pipe_fd) == -1){
         perror("pipe failed");
         return -1;
            }
     producer_pid = fork();
     if(producer_pid == 0){
       producer_process(pipe_fd[1], i*5 + 1);
        }else if(producer_pid > 0){
         close(pipe_fd[0]);
         }

     consumer_pid = fork();
     if(consumer_pid == 0){
       consumer_process(pipe_fd[0], i+1);
       }else if(consumer_pid > 0){
         close(pipe_fd[1]);
            }
     
     pids[pid_count++] = producer_pid;
     pids[pid_count++] = consumer_pid;

    printf("=== Pair %d ===", i);

  }

    // TODO 6: Wait for all children
    // HINT: Use a for loop to wait for all PIDs in the pids array
    // Print exit status for each child
    printf("\nAll pairs completed successfully!\n");
    for(int i = 0; i<pid_count; i++){
    pids[i] = waitpid(pids[i], &status, 0);
    printf("Child %d exited with status %d\n", pids[i], status);
    }
    
    return 0;
}

/*
 * Producer Process - Sends 5 sequential numbers starting from start_num
 */
void producer_process(int write_fd, int start_num) {
    printf("Producer (PID: %d) starting...\n", getpid());
    
    // Send 5 numbers: start_num, start_num+1, start_num+2, start_num+3, start_num+4
    for (int i = 0; i < NUM_VALUES; i++) {
        int number = start_num + i;
        
        if (write(write_fd, &number, sizeof(number)) != sizeof(number)) {
            perror("write");
            exit(1);
        }
        
        printf("Producer: Sent number %d\n", number);
        usleep(100000); // Small delay to see output clearly
    }
    
    printf("Producer: Finished sending %d numbers\n", NUM_VALUES);
    close(write_fd);
    exit(0);
}

/*
 * Consumer Process - Receives numbers and calculates sum
 */
void consumer_process(int read_fd, int pair_id) {
    int number;
    int count = 0;
    int sum = 0;
    
    printf("Consumer (PID: %d) starting...\n", getpid());
    
    // Read numbers until pipe is closed
    while (read(read_fd, &number, sizeof(number)) > 0) {
        count++;
        sum += number;
        printf("Consumer: Received %d, running sum: %d\n", number, sum);
    }
    
    printf("Consumer: Final sum: %d\n", sum);
    close(read_fd);
    exit(0);
}
