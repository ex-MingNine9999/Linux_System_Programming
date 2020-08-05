#define SECOND_TO_MICRO 1000000

struct timeval begin_t, cur_t, end_t;

void ssu_runtime(struct timeval *begin_t, struct timeval *end_t)
{
	end_t -> tv_sec -= begin_t -> tv_sec;

	if (end_t -> tv_usec < begin_t -> tv_usec) {
		end_t -> tv_sec--;
		end_t -> tv_usec += SECOND_TO_MICRO;
	}

	end_t -> tv_usec -= begin_t -> tv_usec;
	printf("\nRuntime: %ld:%06d(sec:usec)\n",end_t -> tv_sec, end_t -> tv_usec);
}

void check_runtime(struct timeval *begin_t, struct timeval *cur_t)
{
	if (cur_t -> tv_sec - begin_t -> tv_sec > 5) {
		printf("\nIt's timeover\n");
		exit(1);
	}
}
