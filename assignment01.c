#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char* FILENAME = "measurements.csv";

#define STATION_NAME_LEN 48
#define MAX_LOCATIONS 50000
#define MAX_LINE_LEN 128

typedef struct __station_stat__ {
	char name[STATION_NAME_LEN];	
	float min_temp;
	float max_temp;
	float sum_temp;
	int count;
} station_stat;

station_stat stations[MAX_LOCATIONS];

int
compare_stations(const void *a, const void *b)
{
	station_stat* s1 = (station_stat*)a;
	station_stat* s2 = (station_stat*)b;
	return strcmp(s1->name, s2->name);
}

int
find_station_index(station_stat* stations,
		   int station_count,
		   const char *name)
{
	int i = 0;
	for (i = 0 ; i < station_count ; i++) {
		if (strcmp(stations[i].name, name) == 0) {
			return i;
		}
	}
	return -1;
}

int
main(int argc, char* argv[])
{
	FILE* fp = fopen(FILENAME, "r");
	char line[MAX_LINE_LEN];	
	int station_count = 0, i, checking = 0;


	if (!fp) {
		perror("File open error!\n");
		return 1;
	}

	while(fgets(line, MAX_LINE_LEN, fp)) {
		char *name = strtok(line, ";");
		char *temp_str = strtok(NULL, "\n");
		float temp;
		int idx;

		if (!name || !temp_str) continue;

		temp = atof(temp_str);
		idx = find_station_index(stations, station_count, name);

		if (idx == -1) {
			strncpy(stations[station_count].name, name, STATION_NAME_LEN);
			stations[station_count].min_temp = temp;
			stations[station_count].max_temp = temp;
			stations[station_count].sum_temp = temp;
			stations[station_count].count = 1;
			station_count++;
		} else {
			if (temp < stations[idx].min_temp) {
				stations[idx].min_temp = temp;
			}
			if (temp > stations[idx].max_temp) {
				stations[idx].max_temp = temp;
			}
			stations[idx].sum_temp += temp;
			stations[idx].count++;
		}
		checking++;

		if (!(checking % 1000000)) printf("%d\n", checking);
	}


	qsort(stations, station_count, sizeof(station_stat), compare_stations);

	for (i = 0 ; i < station_count ; i++) {
		float avg = stations[i].sum_temp / stations[i].count;
		printf("%s=%.1f/%.1f/%.1f\n", stations[i].name,
			stations[i].max_temp, avg, stations[i].min_temp);
	}


	fclose(fp);
	return 0;
}
