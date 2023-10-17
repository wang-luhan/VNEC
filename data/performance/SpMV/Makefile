#Each graph link lives in its own directory
LINK_SUBDIRS = belgium_osm coAuthorsCiteseer coAuthorsDBLP roadNet-CA road_usa soc-sign-epinions

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
