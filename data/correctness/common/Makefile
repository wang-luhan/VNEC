#Each graph link lives in its own directory
SUBDIRS = ak2010 coAuthorsDBLP delaunay_n13
LINK_SUBDIRS = ak2010 coAuthorsDBLP delaunay_n13 delaunay_n21

.PHONY: $(GRAPHS)

#create soft links to every graph.
all: recurse

link: link_recurse

force_link: link_recurse

link_clean: link_recurse

clean: link_recurse

#recurse into each subdirectory and try to create soft links to each graph
link_recurse:
	for subdir in $(LINK_SUBDIRS); do $(MAKE) -C $$subdir $(MAKECMDGOALS); done

$(GRAPHS):
	$(MAKE) -C $@
