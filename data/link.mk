$(GRAPH_NAME):
	$(MAKE) -C ../../../graph/$(GRAPH_NAME) convert
link:
	@if [ -e $(GRAPH_NAME).mtx ]; then rm $(GRAPH_NAME).mtx; fi
	@if [ -e ../../../graph/$(GRAPH_NAME)/$(GRAPH_NAME).mtx ]; then ln -s ../../../graph/$(GRAPH_NAME)/$(GRAPH_NAME).mtx $(GRAPH_NAME).mtx; fi
	@if [ -e $(GRAPH_NAME).sg ]; then rm $(GRAPH_NAME).sg; fi
	@if [ -e ../../../graph/$(GRAPH_NAME)/$(GRAPH_NAME).sg ]; then ln -s ../../../graph/$(GRAPH_NAME)/$(GRAPH_NAME).sg $(GRAPH_NAME).sg; fi
	@if [ -e $(GRAPH_NAME).wsg ]; then rm $(GRAPH_NAME).wsg; fi
	@if [ -e ../../../graph/$(GRAPH_NAME)/$(GRAPH_NAME).wsg ]; then ln -s ../../../graph/$(GRAPH_NAME)/$(GRAPH_NAME).wsg $(GRAPH_NAME).wsg; fi

force_link: $(GRAPH_NAME)
	@if [ -e $(GRAPH_NAME).mtx ]; then rm $(GRAPH_NAME).mtx; fi
	@if [ -e ../../../graph/$(GRAPH_NAME)/$(GRAPH_NAME).mtx ]; then ln -s ../../../graph/$(GRAPH_NAME)/$(GRAPH_NAME).mtx $(GRAPH_NAME).mtx; fi
	@if [ -e $(GRAPH_NAME).sg ]; then rm $(GRAPH_NAME).sg; fi
	@if [ -e ../../../graph/$(GRAPH_NAME)/$(GRAPH_NAME).sg ]; then ln -s ../../../graph/$(GRAPH_NAME)/$(GRAPH_NAME).sg $(GRAPH_NAME).sg; fi
	@if [ -e $(GRAPH_NAME).wsg ]; then rm $(GRAPH_NAME).wsg; fi
	@if [ -e ../../../graph/$(GRAPH_NAME)/$(GRAPH_NAME).wsg ]; then ln -s ../../../graph/$(GRAPH_NAME)/$(GRAPH_NAME).wsg $(GRAPH_NAME).wsg; fi

clean:
	@if [ -e $(GRAPH_NAME).mtx ]; then rm $(GRAPH_NAME).mtx; fi
	@if [ -e $(GRAPH_NAME).sg ]; then rm $(GRAPH_NAME).sg; fi
	@if [ -e $(GRAPH_NAME).wsg ]; then rm $(GRAPH_NAME).wsg; fi

link_clean: clean
