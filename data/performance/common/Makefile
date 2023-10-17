#Each graph link lives in its own directory
LINK_SUBDIRS =  belgium_osm delaunay_n24 coAuthorsCiteseer coAuthorsDBLP soc-LiveJournal1 road_central road_usa cit-Patents soc-orkut indochina-2004 hollywood-2009 roadNet-CA com-Orkut G43 ship_003 Journals rgg_n_2_24_s0

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
