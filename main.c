#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define REAL_FILE     "measurements.csv"
#define TEST_FILE     "weather_stations.csv"

#define HT_CAP        (65536)

struct result
{
  char city[100];
  int count;
  double sum, min, max;
};

static unsigned int hash(const unsigned char *data, int n) {
  unsigned int hash = 0;

  for (int i = 0; i < n; i++) {
    hash = (hash * 31) + data[i];
  }

  return hash;
}


static int cmp(const void *ptr_a, const void *ptr_b) {
  return strcmp(((struct result *)ptr_a)->city, ((struct result *)ptr_b)->city);
}

static const char *parse_double(double *dest, const char *s)
{
  double mod;
  if (*s == '-') {
    mod = -1.0;
    s++;
  }
  else  {
    mod = 1.0;
  }

  if (s[1] == '.') {
    *dest = (((double)s[0] + (double)s[2] / 10.0) - 1.1 * '0') * mod;
    return s + 4;
  }

  *dest = ((double)((s[0]) * 10 + s[1]) + (double)s[3] / 10.0 - 11.1 * '0') * mod;

  return s + 5;
}

int main(int argc, char **argv)
{
  // no need to flush
  setbuf(stdin, NULL);
  setbuf(stdout, NULL);

#ifdef TEST
  const char *file = TEST_FILE;
  printf("USE TEST_FILE\n");
  int rows = 50000;
#else
  const char *file = REAL_FILE;
  printf("USE REAL_FILE\n");
  int rows = 100000000;
#endif

  int cnt = 0;
  char buf[256];
  int map[HT_CAP]; memset(map, -1, sizeof(int) * HT_CAP);

  FILE *fp = fopen(file, "r");
  if (fp == NULL)
  {
    perror("[error] opening files");
    exit(EXIT_FAILURE);
  }

  struct result *results = malloc(sizeof(struct result) * rows);
  if (results == NULL)
  {
    perror("[error] malloc failed");
    exit(EXIT_FAILURE);
  }

  // read single line
  while(fgets(buf, 256, fp) != NULL)
  {
    char *pos = strchr(buf, ';');
    if (pos == NULL)
    {
      fprintf(stderr, "faile to find ; %s \n", buf);
      continue;
    }
    *pos = 0x00;
    double val;
    parse_double(&val, pos + 1);

    // city를 hash 처리한다.
    int h = hash((unsigned char *)buf, pos - buf) & (HT_CAP - 1);

    // hash collision 처리 (이미 채워져 있고, 지금 찾은 city와 buf가 같으면) h값을 증가시켜 저장함.
    while (map[h] != -1 && strcmp(results[map[h]].city, buf) != 0) {
      h = (h + 1) & (HT_CAP - 1);
    }

    int index = map[h];

    if (index < 0) // 최초 발견 된 경우
    {
      strcpy(results[cnt].city, buf);
      results[cnt].sum = val;
      results[cnt].min = val;
      results[cnt].max = val;
      results[cnt].count = 1;
      cnt++;
    }
    else  
  {
      results[index].sum += val;
      if (val > results[index].max )
      {
        results[index].max = val;
      }
      if (val < results[index].min)
      {
        results[index].min = val;
      }
      results[index].count += 1;
    }
  }

  qsort(results, (size_t)cnt, sizeof(struct result), cmp);

  for(int i = 0; i < cnt; i++)
  {
    printf("%s=%.1f/%.1f/%.1f/%d\n", 
           results[i].city, 
           results[i].min, 
           results[i].sum / results[i].count, 
           results[i].max,
           results[i].count);
  }

  free(results);
  fclose(fp);

  return 0;
}
