/*
 * @file :		net_monitor.c
 * @authors :		El-Fadel Bonfoh
 * @date :		12/2019
 * @version :		0.1
 * @brief :		
*/

#include "net_monitor.h"

struct vtl_rtt_t nm_report_rtt(const char *target, unsigned int period) {

	char cmd[2048];
	FILE *temp = NULL;
	char c[256];
	double c_double = 0;
	struct vtl_rtt_t rtt_stat;

	/* Send pings */
	memset(&cmd, 0, 2048);

	snprintf(cmd, 2048, "ping -i %f -w %d %s > .temp.txt", 
			period/(double)10, period, target);

	if(system(cmd) == -1) {
		fprintf(stderr, "ERR: system(1) failed. err=%s\n", 
			strerror(errno));

		exit(0);
	}

	/* Get min/avg/max RTT */
	for(int i = 1; i <= 3; i++) {
		memset(&cmd, 0, 2048);

		snprintf(cmd, 2048, 
			"cat .temp.txt | grep rtt | cut -d \"=\" -f 2 | cut -d \"/\" -f %d > .rtt", 
			i);

		if(system(cmd) == -1) {
			fprintf(stderr, "ERR: system(%d) failed. err=%s\n", 
				i + 1, strerror(errno));

			exit(0);
		}

		temp = fopen(".rtt", "r");
		if(temp == NULL) {
			fprintf(stderr, "ERR: fopen(%d) failed. err=%s\n", 
				i, strerror(errno));
			exit(0);
		}

		fread(&c, 1, 256, temp);
		c_double = atof(c);

		switch(i) {
			case 1:
				rtt_stat.min = c_double;
				break;

			case 2:
				rtt_stat.avg = c_double;
				break;

			case 3:
				rtt_stat.max = c_double;
				break;

			default: // Should never happen
				break;
		}

		system("rm .rtt");
		fclose(temp);
	}
	
	system("rm .temp.txt");
	return rtt_stat;
}

unsigned int nm_report_loss(const char *target, unsigned int period) {

	char cmd[2048];
	FILE *temp = NULL;
	char c[256];
	unsigned int c_int = 0;

	/* Send pings */
	memset(&cmd, 0, 2048);

	snprintf(cmd, 2048, "ping -i %f -w %d %s > .temp.txt", 
			period/(double)10, period, target);

	if(system(cmd) == -1) {
		fprintf(stderr, "ERR: system(1) failed. err=%s\n", 
			strerror(errno));

		exit(0);
	}

	/* Get loss rate */
	memset(&cmd, 0, 2048);

	snprintf(cmd, 2048, 
		"cat .temp.txt | grep loss | cut -d \",\" -f3 | cut -d \"%%\" -f1 > .loss");

	if(system(cmd) == -1) {
		fprintf(stderr, "ERR: system(2) failed. err=%s\n", 
			strerror(errno));

		exit(0);
	}

	temp = fopen(".loss", "r");
	if(temp == NULL) {
		fprintf(stderr, "ERR: fopen(1) failed. err=%s\n", 
			strerror(errno));

		exit(0);
	}

	fread(&c, 1, 256, temp);
	c_int = (unsigned int) atoi(c);

	system("rm .loss");
	system("rm .temp.txt");
	return c_int;
}

struct vtl_stat_t nm_report_stats(const char *target, unsigned int period){

	char cmd[2048];
	FILE *temp = NULL;
	char c[256];
	double c_double = 0;
	unsigned int c_int = 0;
	struct vtl_stat_t stats;

	/* Send pings */
	memset(&cmd, 0, 2048);

	snprintf(cmd, 2048, "ping -i %f -w %d %s > .temp.txt", 
			period/(double)10, period, target);

	if(system(cmd) == -1) {
		fprintf(stderr, "ERR: system(1) failed. err=%s\n", 
			strerror(errno));

		exit(0);
	}

	/* Get min/avg/max RTT */
	for(int i = 1; i <= 3; i++) {
		memset(&cmd, 0, 2048);

		snprintf(cmd, 2048, 
			"cat .temp.txt | grep rtt | cut -d \"=\" -f 2 | cut -d \"/\" -f %d > .rtt", 
			i);

		if(system(cmd) == -1) {
			fprintf(stderr, "ERR: system(%d) failed. err=%s\n", 
				i + 1, strerror(errno));

			exit(0);
		}

		temp = fopen(".rtt", "r");
		if(temp == NULL) {
			fprintf(stderr, "ERR: fopen(%d) failed. err=%s\n", 
				i, strerror(errno));
			exit(0);
		}

		fread(&c, 1, 256, temp);
		c_double = atof(c);

		switch(i) {
			case 1:
				stats.rtt.min = c_double;
				break;

			case 2:
				stats.rtt.avg = c_double;
				break;

			case 3:
				stats.rtt.max = c_double;
				break;

			default: // Should never happen
				break;
		}

		system("rm .rtt");
		fclose(temp);
	}

	/* Get loss rate */
	memset(&cmd, 0, 2048);

	snprintf(cmd, 2048, 
		"cat .temp.txt | grep loss | cut -d \",\" -f3 | cut -d \"%%\" -f1 > .loss");

	if(system(cmd) == -1) {
		fprintf(stderr, "ERR: system(2) failed. err=%s\n", 
			strerror(errno));

		exit(0);
	}

	temp = fopen(".loss", "r");
	if(temp == NULL) {
		fprintf(stderr, "ERR: fopen(1) failed. err=%s\n", 
			strerror(errno));

		exit(0);
	}
	fread(&c, 1, 256, temp);
	c_int = (unsigned int) atoi(c);
	stats.loss_rate = c_int;
	system("rm .loss");

	system("rm .temp.txt");
	return stats;
}
