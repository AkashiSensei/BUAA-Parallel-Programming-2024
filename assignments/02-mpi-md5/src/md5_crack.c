#include <stdio.h>
#include <mpi.h>
#include <md5.h>
#include <string.h>

char CHARSET[40];
// int CHARMAP[128];
char NEXTMAP[128];

void init_charset(char *charset) {
	int i = 0;
	char ch = '0';
	while (ch <= '9') {
		charset[i++] = ch++;
	}

	ch = 'a';
	while (ch <= 'z') {
		charset[i++] = ch++;
	}

	charset[i] = '|';

	// for (int i = 0; i < 36; i++) {
	// 	printf("%02d:%c\n", i, charset[i]);
	// }
}

// void init_charmap(int *charmap) {
// 	int idx_ch = (int) '0';
// 	int i = 0;
// 	while (idx_ch <= (int) '9') {
// 		charmap[idx_ch++] = i++;
// 	}

// 	idx_ch = (int) 'a';
// 	while (idx_ch <= (int) 'z')	{
// 		charmap[idx_ch++] = i++;
// 	}
// }

void init_nextmap(char *nextmap) {
	int idx_ch = '0';
	while (idx_ch <= '8') {
		nextmap[idx_ch] = idx_ch + 1;
		idx_ch++;
	}
	nextmap['9'] = 'a';

	idx_ch = 'a';
	while (idx_ch <= 'y') {
		nextmap[idx_ch] = idx_ch + 1;
		idx_ch++;
	}
	nextmap['z'] = '0';
	
	// for (int i = 0; i < 128; i++) {
	// 	if (nextmap[i] != '\0') {
	// 		printf("%c:%c\n", (char) i, nextmap[i]);
	// 	}
	// }
}

void cal_start(int my_rank, int comm_sz, char *start) {
	int bound = 36 * 36 * my_rank / comm_sz;
	// printf("%d-%d-%d\n", my_rank, comm_sz, bound);
	start[0] = CHARSET[bound / 36];
	start[1] = CHARSET[bound % 36];
	start[2] = '0';
    start[3] = '0';
    start[4] = '0';
	start[5] = '0';
	start[6] = '\0';
}

int code_next(char *code) {
	int carry = 1;
	for (int i = 5; i >= 0; i--) {
		if (carry == 1) {
			if (code[i] != 'z') {
				carry = 0;
			}
			code[i] = NEXTMAP[code[i]];
		}
	}
	return carry;
}

int main() {
	int mpi_my_rank;
	int mpi_comm_sz;
	unsigned char target[20] = {0};
	char start[10] = {0};
	char end[10] = {0};
	char result[10] = {0};
	int carry = 0;
	MD5_CTX mdContext;

	init_charset(CHARSET);
	// init_charmap(CHARMAP);
	init_nextmap(NEXTMAP);

	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &mpi_comm_sz);
	MPI_Comm_rank(MPI_COMM_WORLD, &mpi_my_rank);

	/* get target */
	if (mpi_my_rank == 0) {
		printf("please enter md5 digest:");
		for (int i = 0; i < 16; i++) {
			scanf("%2x", (unsigned int*)(target + i));
		}

		// for (int i = 0; i < 16; i++) {
		// 	printf("%02x", target[i]);
		// }
		// printf("\n");
	}
	MPI_Bcast(target, 20, MPI_UNSIGNED_CHAR, 0, MPI_COMM_WORLD);
	
	/* calculate start and end */	
	cal_start(mpi_my_rank, mpi_comm_sz, start);
	cal_start(mpi_my_rank + 1, mpi_comm_sz, end);
	printf("process%d: [%s, %s)\n", mpi_my_rank, start, end);

	for (strcpy(result, start); strcmp(result, end) < 0; carry = code_next(result)) {
		if (carry == 1) {
			break;
		}

		// puts(result);
		MD5Init (&mdContext);
		MD5Update (&mdContext, result, 6);
		MD5Final (&mdContext);

		if (memcmp(mdContext.digest, target, 16) == 0) {
			printf("process%d found a result: %s\n", mpi_my_rank, result);
		}
	}
	
	MPI_Finalize();
	
	return 0;
}
