#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <pcap.h>

#include <linux/if_link.h>

#include "../../launcher/launcher.h"

#define EGRESS_TF 			"../../XTFsPool/basic_egress_tf.o"
#define INGRESS_TF 			"../../XTFsPool/basic_ingress_tf.o"

int menu_start(void) {

	int choice;
	printf("\n\t KTF Launcher menu \n\n");
	printf("Choose from the options given below \n");
	printf("->> 1. Deploy KTF \n");
	printf("->> 2. Remove KTF \n");
	printf("->> 3. Exit KTF Launcher menu \n\n");

	printf("Enter your choice: \n");
	scanf("%d", &choice);

	return choice;
}

int menu_mode(void) {

	int choice;
	printf("\n\t KTF Launcher \n\n");
	printf("Choose mode: \n");
	printf("->> 1. Ingress mode (RX ~~ XDP) \n");
	printf("->> 2. Egress mode (TX ~~ TC) \n\n");

	printf("Enter your choice: \n");
	scanf("%d", &choice);

	return choice;
}

void select_interface(char *interface) {

	pcap_if_t *alldevsp, *device;
	char errbuf[100], devs[100][100];
	int count = 1, n;

	// Get the list of available devices
	printf("\n Finding available devices...\n");
	if(pcap_findalldevs(&alldevsp, errbuf)) {
		printf("ERR: Unable to find devices: %s\n", errbuf);
		exit(1);
	}
	printf("Done\n");

	// Print the available devices
	printf("\n Available devices are: \n");
	for(device = alldevsp; device != NULL; device = device->next) {
		printf("->> %d. %s - %s \n", count, device->name, device->description);
		if(device->name != NULL) {
			strcpy(devs[count], device->name);
		}
		count++;
	}

	// Ask user which device to use
	printf("\n Enter the number of the device you want to use: \n");
	scanf("%d", &n);
	strcpy(interface, devs[n]);
}

void deploy_tf(int mode) {

	int ret;
	char interface[20];

	switch(mode) {
		case 1: // Ingress mode = XDP
			// select interface
			select_interface(interface);

			// deploy tf on selected inteface
			printf("\n Deploying KTF on %s interface in ingress mode...\n", interface);
			struct xdp_config xdp_cfg = {};
			int xdp_flags;
			xdp_flags &= ~XDP_FLAGS_MODES; // Clear flags
			xdp_flags |= XDP_FLAGS_SKB_MODE; // Set flag
			ret = launcher_deploy_ingress_tf(&xdp_cfg, INGRESS_TF, interface, xdp_flags);
			printf("Success ! \n");

			break;

		case 2:
			// Select interface
			select_interface(interface);

			// Deploy KTF on selected interface
			printf("\n\n Deploying KTF on %s interface in egress mode...", interface);
			struct tc_config tc_cfg = {};
			ret = launcher_deploy_egress_tf(&tc_cfg, EGRESS_TF, interface, TC_EGRESS_ATTACH);
			printf("Success ! \n");

			break;

		default:
			break;
	}

	return;
}

void remove_tf(int mode) {

	int ret;
	char interface[20];

	switch(mode) {
		case 1:
			select_interface(interface);

			printf("\n\n Removing KTF on %s in ingress mode...", interface);
			struct xdp_config xdp_cfg = {};
			int xdp_flags;
			xdp_flags &= ~XDP_FLAGS_MODES;
			xdp_flags |= XDP_FLAGS_SKB_MODE;
			ret = launcher_remove_ingress_tf(&xdp_cfg, interface, xdp_flags);

			printf("Success ! \n");

			break;

		case 2:
			select_interface(interface);

			printf("\n\n Removing KTF on %s interface in egress mode...", interface);
			struct tc_config tc_cfg = {};
			ret = launcher_remove_egress_tf(&tc_cfg, interface, TC_EGRESS_ATTACH);

			printf("Success ! \n");

			break;

		default:
			break;
	}
}

void clear_screen() {
	system("clear");
}

int main() {

	int ret, c, menu_choice, mode;
	char enter = 0;

	clear_screen();

	do {
		// Start menu
		menu_choice = menu_start();
		clear_screen();

		// Clear input buffer
		while((c = getchar()) != '\n' && c != EOF) { }

		switch(menu_choice){
			case 1:
				mode = menu_mode();
				deploy_tf(mode);

				while((c = getchar()) != '\n' && c != EOF) { }

				printf("\n Press enter to return to start menu \n");
				while(enter != '\r' && enter != '\n') { enter = getchar(); }
				clear_screen();

				break;

			case 2:
				mode = menu_mode();
				remove_tf(mode);

				while((c = getchar()) != '\n' && c != EOF) { }

				printf("\n Press enter to return to start menu \n");
				while(enter != '\r' && enter != '\n') { enter = getchar(); }
				clear_screen();

				break;

			case 3: 
				printf(" Exit KTF Launcher menu \n");
				break;

			default:
				break;
		}
	} while(menu_choice != 3);

	return 0;
}