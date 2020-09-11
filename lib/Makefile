LIBBPF_DIR = libbpf/src/



all: build-subdirs
	@echo "Build third-party libraries finished!"


build-subdirs: libbpf

libbpf: force
	@if [ ! -d $(LIBBPF_DIR) ]; then \
		echo "Error: Need libbpf submodule"; \
		echo "May need to run git submodule update --init"; \
		exit 1; \
	else \
		cd $(LIBBPF_DIR) && $(MAKE) all; \
		mkdir -p build; DESTDIR=build $(MAKE) install_headers; \
	fi


.PHONY: clean
clean:
	rm -rf $(LIBBPF_DIR)/build
	$(MAKE) -C $(LIBBPF_DIR) clean

force:;
