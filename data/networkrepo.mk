#common make file fragment for networkrepository.com
#just define GRAPH_NAME prior to including this fragment

GRAPH_ZIP  = $(GRAPH_NAME).zip

setup: $(GRAPH_NAME).mtx

$(GRAPH_NAME).mtx: $(GRAPH_ZIP)
	unzip -u $(GRAPH_ZIP)
	@if [ -e readme.txt ]; then rm -f readme.txt; fi

clean: sg_clean
	if [ -e $(GRAPH_NAME).mtx ]; then rm $(GRAPH_NAME).mtx; fi

real_clean: clean
	rm $(GRAPH_ZIP)

