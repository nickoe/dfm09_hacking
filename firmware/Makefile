PROJECTS = dfm09-demo

.PHONY = libopencm3 clean_libopencm3

all:	libopencm3
	@for d in $(PROJECTS) ; do \
		echo "*** Building project $$d ***" ; \
		$(MAKE) -C $$d -s ; \
	done

clean:	clean_libopencm3
	@for d in $(PROJECTS) ; do \
		echo "*** Cleaning project $$d ***" ; \
		$(MAKE) -C $$d clean -s ; \
	done

clean_libopencm3:
	$(MAKE) -C libopencm3 clean -s

libopencm3: libopencm3/lib/libopencm3_stm32f1.a

libopencm3/lib/libopencm3_stm32f1.a:
	$(MAKE) -C libopencm3 lib/stm32/f1 -s
