
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <pcap.h>

#include <linux/if_link.h> //for xdp_flags

#include "../launcher/launcher.h"

#define BPF_TC_FILENAME         "../XTFsPool/egress_test_tf.o"
#define BPF_XDP_FILENAME        "../XTFsPool/ingress_test_tf.o"
#define BPF_HOOKER_FILENAME     "../XTFsPool/hooker_test_progs.o"

#define INPUT_MODE              0x1
#define OUTPUT_MODE             0x2
#define STOP_HOOKER             0x4

int menu_start(void)
{
        int choice;
        printf("\n\tKTF Orchestrator menu\n\n");
        printf("Choose from the options given below\n");
        printf("->> 1- Deploy TF\n");
        printf("->> 2- Remove TF\n");
        printf("->> 3- Exit KTF Orchestrator\n\n");

        printf("Enter your choice: ");
        //scanf("%d", &choice);
        if(scanf("%d", &choice) != 1) {
            printf("ERROR: scanf()");
        }

        return choice;

}

int menu_mode(void)
{
        int choice;
        printf("\n\tKTF Orchestrator\n\n");
        printf("Choose mode:\n");
        printf("->> 1- Input mode (reception ~~ XDP)\n");
        printf("->> 2- Output mode (emission ~~ TC)\n");
        printf("->> 3- Activate Hooker\n");
        printf("->> 4- Deactivate Hooker\n\n");

        //4- return to menu start

        printf("Enter your choice: ");
        if(scanf("%d", &choice) != 1) {
            printf("ERROR: scanf()");
        }

        return choice;

}

void select_interface(char *interface)
{
        pcap_if_t *alldevsp, *device;
        char errbuf[100], devs[100][100];
        int count = 1, n;

        //First get the list of available devices
        printf("\nFinding available devices ... ");
        if (pcap_findalldevs(&alldevsp, errbuf))
        {
                printf("Error finding devices : %s", errbuf);
                exit(1);
        }
        printf("Done");

        //Print the available devices
        printf("\nAvailable Devices are :\n");
        for (device = alldevsp; device != NULL; device = device->next)
        {
                printf("->> %d. %s - %s\n", count, device->name, device->description);
                if (device->name != NULL)
                {
                        strcpy(devs[count], device->name);
                }
                count++;
        }
        //Ask user which device to use
        printf("\nEnter the number of the device you want use : ");
        if(scanf("%d", &n) != 1) {
            printf("ERROR: scanf()");
        }
        strcpy(interface, devs[n]);
}

int deploy_tf(int mode)
{
        int ret; //use it
        char interface[20];

        //int xdp_flags = 0;
        struct xdp_config xdp_cfg = {0};

        struct tc_config tc_cfg = {0};

        switch (mode)
        {
        case INPUT_MODE /* xdp */:

                select_interface(interface);

                printf("\n\nDeploying TF on %s interface in input mode...\n", interface);

                ret = launcher_load_graft_file(BPF_XDP_FILENAME, interface);
                if (ret != 0) {
                        fprintf(stderr, "%s", xdp_cfg.err_buf);
                        fprintf(stderr, "ERR: launcher_deploy_xdp_tf failed\n");
                        return -1;
                }
                printf("Success\n");

                break;

        case 2 /* Output mode = tc; TODO: aligned with INPUT_MODE */:
        {
                select_interface(interface);
                /* deploy tf on selected interface */
                printf("\n\nDeploying TF on %s interface in output mode...\n", interface);

                ret = launcher_load_graft_file(BPF_TC_FILENAME, interface);
                if (ret != 0) {
                        fprintf(stderr, "%s", tc_cfg.err_buf);
                        fprintf(stderr, "ERR: launcher_deploy_tc_tf failed\n");
                        return -1;
                }

                printf("Success\n");
                break;
        }
        case 3 /* TODO: aligned with INPUT_MODE */ :

            select_interface(interface);
            printf("[VTL_UI]: Enabling Hooker ...\n");
            if(system("mount -t bpf bpf /sys/fs/bpf/") == -1)
                printf("[VTL_UI]: WARN - unable to munt bpf map fs.\n");
            ret = launcher_load_graft_file(BPF_HOOKER_FILENAME, interface);

            if(ret != 0) {
                printf("[VTL_UI]: launcher_load_graft_file() -- failed to activate Hooker.\n");
                return -1;
            }
            break;

        default:
                fprintf(stderr, "ERR: unknown mode\n");
                return -1;
                break;
        }

        return 0;
}

int remove_tf(int mode)
{
        int ret; //use it
        char interface[20];

        int xdp_flags = 0;
        struct tc_config tc_cfg = {0};

        switch (mode)
        {
        case INPUT_MODE /* xdp */:
                /* code */

                /* select interface */
                select_interface(interface);

                /* Remove tf on selected interface */
                printf("\n\nRemoving TF on %s interface in input mode...", interface);

                xdp_flags &= ~XDP_FLAGS_MODES;   /* Clear flags */
                xdp_flags |= XDP_FLAGS_SKB_MODE; /* Set   flag */
                ret = launcher_unload_ingress_tf(interface, xdp_flags);
                if (ret < 0) {
                        fprintf(stderr, "ERR: launcher_remove_xdp_tf failed\n");
                        return -1;
                }

                printf("Success\n");
                break;

        case OUTPUT_MODE /* tc */:

                /* select interface */
                select_interface(interface);

                /* Remove tf on selected interface */
                printf("\n\nRemoving TF on %s interface in output mode...", interface);
                ret = launcher_unload_egress_tf(&tc_cfg, interface, TC_EGRESS_ATTACH);
                if (ret < 0) {
                        fprintf(stderr, "ERR: launcher_remove_tc_tf failed\n");
                        return -1;
                }

                printf("Success\n");
                break;

        case STOP_HOOKER:
            select_interface(interface);
            printf("\n\nDeactivating Hooker progs ...\n");
            launcher_unload_hooker_progs();
            ret = launcher_unload_listerner_tf(interface, 0);
            if(ret < 0) {
                fprintf(stderr, "ERR: launcher_unload_listerner_tf failed\n");
                return -1;
            }
            printf("Hooker progs deactivate\n");
            break;

        default:
                fprintf(stderr, "ERR: unknown mode\n");
                return -1;
                break;
        }

        return 0;

}

void clear_screen(void)
{
    if(system("clear") == -1) {
        printf("[UI]: clear_screen() -- Unable to clear console.\n");
    }
}


int main(int argc, char const *argv[])
{
        int ret;

        int menu_choice;
        int mode;
        //char enter = 0;
        char c;

        clear_screen();

        do
        {
                // start menu
                menu_choice = menu_start();
                clear_screen();

                switch (menu_choice)
                {
                case 1 /* Deploy TF*/:

                        //TODO: Display TF list
                        mode = menu_mode();

                        ret = deploy_tf(mode);
                        if (ret < 0) {
                                menu_choice = 3;
                                break;
                        }

                        printf("Do you return to main menu or exit ? (M or E)\n");
                        if(scanf("%c", &c) != 1) {
                            printf("ERROR: scanf()");
                            break;
                        }
                        if (c == 'E') {
                                menu_choice = 3;
                        }
    
                       // clear_screen(); // TODO: Fix.

                        break;
                case 2 /* Remove TF */:
                        mode = menu_mode();
                        ret = remove_tf(mode);
                        if (ret < 0) {
                                menu_choice = 3;
                                break;
                        }

                        printf("Do you return to main menu or exit ? (M or E)\n");
                        if(scanf("%c", &c) != 1) {
                            printf("ERROR: scanf()");
                            break;
                        }
                        if (c == 'E') {
                                menu_choice = 3;
                        }

                        clear_screen();

                        break;

                case 3 /* program exit */:
                        //Temporary: remove all tf at program exit.
                        printf("Exit program\n");

                        break;

                default:
                        break;
                }

        } while (menu_choice != 3 /* Exit */);


        return 0;
}
