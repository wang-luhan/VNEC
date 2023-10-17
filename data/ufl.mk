#common make file fragment for ufl graph datasets
#just define GRAPH_NAME prior to including this fragment

GRAPH_TAR  = $(GRAPH_NAME).tar.gz

setup: $(GRAPH_NAME).mtx

$(GRAPH_NAME).mtx: $(GRAPH_TAR)
	tar xvfz $(GRAPH_TAR)
	cp $(GRAPH_NAME)/$(GRAPH_NAME).mtx $(GRAPH_NAME).mtx
	rm -rf $(GRAPH_NAME)

clean: sg_clean
	@if [ -e  $(GRAPH_NAME).mtx ]; then rm $(GRAPH_NAME).mtx; fi

real_clean: clean
	rm $(GRAPH_TAR)

