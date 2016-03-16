/*!
 * process-cpu.c
 *
 * Description: Calculates the CPU percent of a process, i.e. how much of 
 *              the CPU has the givn process been using.
 *
 * Usage:       process-cpu <process name>
 *
 * Caveats:     This programs expects to the find the pid file for a process
 *              in /var/run and doesn't make any effort to find the pid file
 *              elsewhere.
 */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

struct pstat {
  long unsigned int utime_ticks;
  long int          cutime_ticks;
  long unsigned int stime_ticks;
  long int          cstime_ticks;
  long unsigned int vsize;        // virtual memory size in bytes
  long unsigned int rss;          // Resident  Set  Size in bytes

  long unsigned int cpu_total_time;
};

/*!
 * read_pid
 *
 * \brief Reads the process' pid from the supplied pid file
 * \param   char* pidfile Pid file path
 * \return  int           The pid or 0
 */
int read_pid (char* pidfile) {
  FILE *f;
  int pid;

  if (!(f=fopen(pidfile, "r")))
    return 0;
  fscanf(f, "%d", &pid);
  fclose(f);
  return pid;
}

/*!
 * get_usage
 * \brief read /proc data into the passed struct pstat returns 0 on success, 
 *        -1 on error
 *
 * \param   pid_t   pid     Process pid
 * \param   pstat   result  Process usage result
 * \return  int             Non-zero on failure
*/
int get_usage (const pid_t pid, struct pstat* result) {
  //convert  pid to string
  char pid_s[20];
  snprintf(pid_s, sizeof(pid_s), "%d", pid);
  char stat_filepath[30] = "/proc/";
  strncat(stat_filepath, pid_s,
    sizeof(stat_filepath) - strlen(stat_filepath) -1);
  strncat(stat_filepath, "/stat", sizeof(stat_filepath) -
    strlen(stat_filepath) -1);

  FILE *fpstat = fopen(stat_filepath, "r");
  if (fpstat == NULL) {
    perror("FOPEN ERROR ");
    return -1;
  }

  FILE *fstat = fopen("/proc/stat", "r");
  if (fstat == NULL) {
    perror("FOPEN ERROR ");
    fclose(fstat);
    return -1;
  }

  //read values from /proc/pid/stat
  memset(result, 0, sizeof(struct pstat));
  long int rss;
  if (fscanf(fpstat, "%*d %*s %*c %*d %*d %*d %*d %*d %*u %*u %*u %*u %*u %lu"
      "%lu %ld %ld %*d %*d %*d %*d %*u %lu %ld",
      &result->utime_ticks, &result->stime_ticks,
      &result->cutime_ticks, &result->cstime_ticks, &result->vsize,
      &rss) == EOF) {
    fclose(fpstat);
    return -1;
  }
  fclose(fpstat);
  result->rss = rss * sysconf(_SC_PAGESIZE); 

  //read+calc cpu total time from /proc/stat
  long unsigned int cpu_time[10];
  memset(cpu_time, 0, sizeof(cpu_time));
  if (fscanf(fstat, "%*s %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu",
      &cpu_time[0], &cpu_time[1], &cpu_time[2], &cpu_time[3],
      &cpu_time[4], &cpu_time[5], &cpu_time[6], &cpu_time[7],
      &cpu_time[8], &cpu_time[9]) == EOF) {
    fclose(fstat);
    return -1;
  }

  fclose(fstat);

  for (int i=0; i < 10; i++)
    result->cpu_total_time += cpu_time[i];

  return 0;
}

/*!
 * calc_cpu_usage_pct
 * 
 * \brief Calculates the elapsed CPU usage between 2 measuring points. Returned
 *        as a percent in output params ucpu_usage and scpu_usage.
 *
 * \param   pstat   cur_usage   Current CPU usage
 * \param   pstat   last_usage  Last CPU usage
 * \param   double  ucpu_usage  User CPU usage
 * \param   double  scpu_usage  System CPU usage
 */
void calc_cpu_usage_pct(const struct pstat* cur_usage,
                        const struct pstat* last_usage,
                        double* ucpu_usage, double* scpu_usage) {
  const long unsigned int total_time_diff = cur_usage->cpu_total_time -
                                            last_usage->cpu_total_time;

  *ucpu_usage = 100 * (((cur_usage->utime_ticks + cur_usage->cutime_ticks)
                    - (last_usage->utime_ticks + last_usage->cutime_ticks))
                    / (double) total_time_diff);

  *scpu_usage = 100 * ((((cur_usage->stime_ticks + cur_usage->cstime_ticks)
                    - (last_usage->stime_ticks + last_usage->cstime_ticks))) /
                    (double) total_time_diff);
}

/*!
 * calc_cpu_usage_pct
 * 
 * \brief Calculates the elapsed CPU usage between 2 measuring points in ticks
 *        Returned in output params ucpu_usage and scpu_usage.
 *
 * \param   pstat             cur_usage   Current CPU usage
 * \param   pstat             last_usage  Last CPU usage
 * \param   long unsigned int ucpu_usage  User CPU usage
 * \param   long unsigned int scpu_usage  System CPU usage
 */
void calc_cpu_usage(const struct pstat* cur_usage,
                    const struct pstat* last_usage,
                    long unsigned int* ucpu_usage,
                    long unsigned int* scpu_usage) {

  *ucpu_usage = (cur_usage->utime_ticks + cur_usage->cutime_ticks) -
                (last_usage->utime_ticks + last_usage->cutime_ticks);

  *scpu_usage = (cur_usage->stime_ticks + cur_usage->cstime_ticks) -
                (last_usage->stime_ticks + last_usage->cstime_ticks);
}


int main (int argc, char **argv) {
  
  if (argc != 5) {
    printf("Usage: %s -p <process name / id> -i <interval>\n", argv[0]);
    return 1;
  }

  char* proc = NULL;

  int c, pid, interval;
  int got_proc_name = 0;

  double user, sys;
  struct pstat mark0, mark1;

  while ((c = getopt(argc, argv, "p:i:")) != -1) {
    switch (c) {
      case 'p':

        if (strspn(optarg, "0123456789") == strlen(optarg)) {
          pid = atoi(optarg);
          proc = NULL;
        } else {
          // not an integer, assume a process name
          got_proc_name = 1;
          proc = optarg;
        }
        break;
      case 'i':

        if (strspn(optarg, "0123456789") == strlen(optarg)) {
          interval = atoi(optarg);
        } else {
          fprintf(stderr, "invalid -i option %s - expecting a number\n",
            optarg ? optarg : "");
          exit(EXIT_FAILURE);
        }
        break;
      case '?':
        if (optopt == 'p') {
          fprintf(stderr, "Option -%c requires an argument.\n", optopt);
        } else if (optopt == 'i') {
          fprintf(stderr, "Option -%c requires an argument.\n", optopt);
        } else if (isprint(optopt)) {
          fprintf(stderr, "Unknown option `-%c'.\n", optopt);
        } else {
          fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
        }
        return 1;
      default:
        exit(EXIT_FAILURE);
    }
  }

  if (got_proc_name == 1) {
    char pidfile[75];
    strcpy(pidfile, "/var/run/");
    strcat(pidfile, proc);
    strcat(pidfile, ".pid");

    pid = read_pid(pidfile);

    if (pid == 0) 
      return 1;
  }

  get_usage(pid, &mark0);
  sleep(interval);
  get_usage(pid, &mark1);

  calc_cpu_usage_pct(&mark1, &mark0, &user, &sys);

  printf("%f\n", user + sys);

  return 0;
} 
