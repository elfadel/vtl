/*
 * @file :		net_monitor.h
 * @authors :		El-Fadel Bonfoh
 * @date :		12/2019
 * @version :		0.1
 * @brief :		
*/

#pragma once

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

struct vtl_rtt_t {
	double min;
	double avg;
	double max;
};

struct vtl_stat_t {
	struct vtl_rtt_t rtt;
	unsigned int loss_rate;
};

struct vtl_rtt_t nm_report_rtt(const char *target, unsigned int period);

unsigned int nm_report_loss(const char *target, unsigned int period);

struct vtl_stat_t nm_report_stats(const char *target, unsigned int period);