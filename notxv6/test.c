#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

static pthread_cond_t threadDied = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t threadMutex = PTHREAD_MUTEX_INITIALIZER;;

static int totThreads = 0;
static int numLive = 0;

static int numUnjoined = 0;

enum tstate {
  TS_ALIVE,
  TS_TERMINATED,
  TS_JOINED,
};

static struct {
  pthread_t tid;
  enum tstate state;
  int sleepTime;
} *thread;

static void *
threadFunc(void *arg)
{
  int idx = *((int *) arg);
  int s;

  sleep(thread[idx].sleepTime);     /* Simulate doing some work */
  printf("Thread %d terminating\n", idx);

  s = pthread_mutex_lock(&threadMutex);
  if (s != 0)
    exit(EXIT_FAILURE);

  numUnjoined++;
  thread[idx].state = TS_TERMINATED;

  s = pthread_mutex_unlock(&threadMutex);
  if (s != 0)
    exit(EXIT_FAILURE);
  s = pthread_cond_signal(&threadDied);
  if (s != 0)
    exit(EXIT_FAILURE);

  return NULL;
}

int main(int argc, char **argv)
{
  int s, idx;

  if (argc < 2 || strcmp(argv[1], "--help") == 0) {
    fprintf(stderr, "%s nsecs...\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  thread = calloc(argc - 1, sizeof(*thread));
  if (thread == NULL)
    exit(EXIT_FAILURE);

  for (idx = 0; idx < argc - 1; idx++) {
    thread[idx].sleepTime = atoi(argv[idx + 1]);
    thread[idx].state = TS_ALIVE;
    s = pthread_create(&thread[idx].tid, NULL, threadFunc, &idx);
    if (s != 0)
      exit(EXIT_FAILURE);
  }

  totThreads = argc - 1;
  numLive = totThreads;


  while (numLive > 0) {
    s = pthread_mutex_lock(&threadMutex);
    if (s != 0)
      exit(EXIT_FAILURE);
    while (numUnjoined == 0) {
      s = pthread_cond_wait(&threadDied, &threadMutex);
      if (s != 0)
        exit(EXIT_FAILURE);
    }

    for (idx = 0; idx < totThreads; idx++) {
      if (thread[idx].state == TS_TERMINATED) {
        s = pthread_join(thread[idx].tid, NULL);
        if (s != 0)
          exit(EXIT_FAILURE);

        thread[idx].state = TS_JOINED;
        numLive--;
        numUnjoined--;

        printf("Reaped thread %d (numLive=%d)\n", idx, numLive);
      }
    }

    s = pthread_mutex_unlock(&threadMutex);
    if (s != 0)
      exit(EXIT_FAILURE);
  }

  exit(EXIT_SUCCESS);
}
