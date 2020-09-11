

SEC("hooker_sockops/0")
int hooker_monitor_apps(struct bpf_sock_ops *sk_ops) {
	
	int op;

	op = (int) sk_ops->op;

	// How to fill TCP option header here???
	switch(op) {
		case BPF_SOCK_OPS_TCP_CONNECT_CB:
			// Application (client) is trying active connexion -- connect()

			// TODO: Add VTL_COMPLIANT opt to TCP hdr
			bpf_printk("[HOOKER]: Connect() issued !\n");
			break;

		case BPF_SOCK_OPS_PASSIVE_ESTABLISHED_CB:
			bpf_printk("[HOOKER]: Accept() issued !\n");

		case BPF_SOCK_OPS_TCP_LISTEN_CB:
			bpf_printk("[HOOKER]: Listen() issued !\n");

		default:
			bpf_printk("[HOOKER]: Prog called at unkown TCP execution path()\n"); 
	}

	return 0;
}