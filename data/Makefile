#Makefile to fetch and install graph data

#Matrix data is saved in the graph directory.
SUBDIRS =  graph

#Matrix data is divided into multiple groups according to test categories.
#Specific graph matrixs in each group are soft links to
# corresponding matrixs in the graph directory in order to save the space.
LINK_SUBDIRS =  performance/SpMV

#fetches all graphs, extracts and sets up files for tests
all: recurse

#only download the graphs, but do not proceed further
fetch: recurse

#build the converter
converter: converter.cpp
	$(CXX) -fopenmp $^ -o $@

#convert .mtx format to .sg and .wsg format.
convert: converter recurse

# Only create some link of existing .sg / .wsg / .mtx link in the graph dir.
link: link_recurse

#creates all links for all test categories
#after downloading and converting graph data.
force_link: converter link_recurse

#clean all links in the test categories.
link_clean: link_recurse

#clean everything except the downloaded graphs
clean: link_recurse recurse
	rm -f converter

#clean all converted serialization graphs.
sg_clean: recurse

#clean everything including the downloaded graphs.
real_clean: recurse

#recurse into each subdirectory and try to build the provided targets
recurse:
	for subdir in $(SUBDIRS); do $(MAKE) -C $$subdir $(MAKECMDGOALS); done

link_recurse:
	for subdir in $(LINK_SUBDIRS); do $(MAKE) -C $$subdir $(MAKECMDGOALS); done

