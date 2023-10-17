PATTERN=`head -n 1 $(GRAPH_NAME).mtx|awk '{print $$4}'`

$(GRAPH_NAME).sg: $(GRAPH_NAME).mtx
	../../converter -f $< -o $@

$(GRAPH_NAME).wsg: $(GRAPH_NAME).mtx
	@if [ ${PATTERN} != pattern ];then \
	echo "../../converter -wf $(GRAPH_NAME).mtx -o $(GRAPH_NAME).wsg";\
	../../converter -wf $< -o $@;\
	else echo "$< is not a weighted graph. There is no need to generate the $@ file";\
	fi;

convert:$(GRAPH_NAME).sg $(GRAPH_NAME).wsg

sg_clean:
	@if [ -e $(GRAPH_NAME).sg ];then rm $(GRAPH_NAME).sg;fi;
	@if [ -e $(GRAPH_NAME).wsg ];then rm $(GRAPH_NAME).wsg;fi;
