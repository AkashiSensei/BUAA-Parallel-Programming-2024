#include <stdio.h>
#include <mpi.h>
#include <stdlib.h>
#include <time.h>

long timespec_diff_millis(struct timespec *start, struct timespec *stop);

int main() {
	int mpi_my_rank;
	int mpi_comm_sz;
	unsigned long long total_cnt;
	unsigned long long local_hit = 0;

	/* initailize mpi relative variables */
	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &mpi_comm_sz);
	MPI_Comm_rank(MPI_COMM_WORLD, &mpi_my_rank);

	/* do different tasks */
	if (mpi_my_rank == 0) {
		unsigned long long total_hit;
		double pi;
		
		
		/* get input of total count and sync to other processes */
		printf("Enter the total number of tosses:");
		scanf("%lld", &total_cnt);
		MPI_Bcast(&total_cnt, 1, MPI_UNSIGNED_LONG_LONG, 0, MPI_COMM_WORLD);

		/* get result from other process */
		MPI_Reduce(&local_hit, &total_hit, 1, MPI_UNSIGNED_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);

		pi = (double)total_hit / total_cnt * 4;
		printf("pi estimate = %lf\n", pi);
	} else {
		/* local variable needed */
		struct timespec start_ts;
		struct timespec end_ts;
		unsigned long long local_cnt;
		unsigned long long i, x, y;
		unsigned long long area = (unsigned long long)RAND_MAX * RAND_MAX;
		unsigned long long dis;

		/* get total count from process 0 */
		MPI_Bcast(&total_cnt, 1, MPI_LONG_LONG, 0, MPI_COMM_WORLD);
		
		/* calculate local conut */
		mpi_comm_sz--;
		local_cnt = total_cnt / mpi_comm_sz + (mpi_my_rank <= total_cnt % mpi_comm_sz);
		mpi_comm_sz++;

		/* get start time & initialize random seed */
		timespec_get(&start_ts, TIME_UTC);
		srand(start_ts.tv_nsec);

		printf("Process %d ready, local count %llu, start time %ld.%ld\n", mpi_my_rank, local_cnt, start_ts.tv_sec, start_ts.tv_nsec);

		/* count hits */
		for (i = 0llu; i < local_cnt; i++) {
			x = rand();
			y = rand();
			dis = x * x + y * y;
			if (dis < area) {
				local_hit++;
			}
		}

		/* record end time */
		timespec_get(&end_ts, TIME_UTC);

		printf("Process %d finish, hits %llu/%llu, cost %ld ms, estimate pi %lf\n",
				mpi_my_rank, local_hit, local_cnt, timespec_diff_millis(&start_ts, &end_ts), (double)local_hit / local_cnt * 4);

		/* send result back to process 0 */
		MPI_Reduce(&local_hit, NULL, 1, MPI_UNSIGNED_LONG_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
	}

	MPI_Finalize();
	return 0;
}

long timespec_diff_millis(struct timespec *start, struct timespec *stop) {
    long seconds = stop->tv_sec - start->tv_sec;
    long nanoseconds = stop->tv_nsec - start->tv_nsec;
    
    if (nanoseconds < 0) {
        seconds--;
        nanoseconds += 1000000000;
    }
    
    long millis = seconds * 1000 + nanoseconds / 1000000;
    return millis;
}
