#Each graph lives in its own directory
# SUBDIRS =         ak2010             coPapersDBLP        gsm_106857         mouse_gene        soc-orkut \
# 3Dspectralwave    amazon-2008        com-Friendster      gupta3             nd12k             soc-sign-Slashdot081106 \
# G43               amazon0312         com-Orkut           higgs-twitter      p2p-Gnutella25    soc-sign-Slashdot090216 \
# GAP-kron          as-Skitter         com-Youtube         hollywood-2009     pdb1HYS           soc-sign-Slashdot090221 \
# GAP-twitter       bbmat              consph              hugebubbles-00010  pdf_test          soc-sign-epinions \
# GAP-web           belgium_osm        crankseg_2          hugetrace-00020    pwtk              twitter7 \
# Journals          cage12             delaunay_n13        human_gene1        rajat31           viscorocks \
# 2cubes_sphere     cage15             delaunay_n21        human_gene2        rgg_n_2_24_s0     water_tank \
# PFlow_742         cell1              delaunay_n24        indochina-2004     roadNet-CA        wb-edu \
# SiO2              cit-Patents        dielFilterV2clx     karate             road_central      web-Stanford \
# TSOPF_FS_b300_c2  cnr-2000           eu-2005             kron_g500-logn19   road_usa          webbase-1M \
# TSOPF_RS_b2383    coAuthorsCiteseer  fem_hifreq_circuit  kron_g500-logn21   ship_003          west0067 \
# adaptive          coAuthorsDBLP      germany_osm         lhr71              sk-2005           wiki-topcats \
# af_shell10        coPapersCiteseer   grb_ppt             ljournal-2008      soc-LiveJournal1
# SUBDIRS =         coPapersCiteseer             coPapersDBLP        road_usa         roadNet-CA        belgium_osm  soc-sign-epinions
SUBDIRS = roadNet-CA	amazon0312	com-Youtube	web-Stanford	as-Skitter	road_usa\
		  coAuthorsDBLP	web-Google	com-Orkut	higgs-twitter	wiki-topcats\
web-BerkStan	com-Amazon	com-DBLP	TSOPF_RS_b2383	coPapersCiteseer	eu-2005\
crankseg_2	nd12k	coPapersDBLP	cnr-2000	msdoor	bundle_adj\
F1	inline_1	audikw_1	ML_Laplace	dielFilterV3real	Flan_1565\
Long_Coup_dt0	mip1\
# # SMALL MATRIXS ( < 100M)
# SUBDIRS =         ship_003           consph              delaunay_n21       webbase-1M       gupta3 \
# water_tank        roadNet-CA         com-Youtube         cage12             bbmat            lhr71 \
# 2cubes_sphere     amazon0312         belgium_osm         viscorocks         web-Stanford     soc-sign-epinions \
# coAuthorsDBLP     coAuthorsCiteseer  ak2010              cell1              p2p-Gnutella25   soc-sign-Slashdot090221 \
# soc-sign-Slashdot090216              soc-sign-Slashdot081106                delaunay_n13     Journals \
# G43               west0067           pdf_test            karate             grb_ppt

# MIDDLE MATRIXS ( >= 100M, < 500M)
# SUBDIRS =         mouse_gene         hugebubbles-00010   road_usa           human_gene1      hugetrace-00020 \
# road_central      human_gene2        wiki-topcats        nd12k              germany_osm      crankseg_2 \
# adaptive          pwtk               SiO2                coPapersCiteseer   cit-Patents      eu-2005 \
# TSOPF_FS_b300_c2  as-Skitter         higgs-twitter       pdb1HYS

# LARGE MATRIXS ( >= 500M)
# SUBDIRS =         cage15             rgg_n_2_24_s0       indochina-2004      com-Orkut        soc-orkut af_shell10 \
# delaunay_n24      ljournal-2008      hollywood-2009      soc-LiveJournal1    wb-edu           rajat31 \
# TSOPF_RS_b2383

.PHONY: $(GRAPHS)

#fetches all graphs, extracts and sets up files for tests
all: recurse

#only download the graphs, but do not proceed further
fetch: recurse

convert: recurse

link: recurse

#clean everything except the downloaded graphs
clean: recurse

#clean all serialization graph
sg_clean: recurse

#clean everything including the downloaded graphs
real_clean: recurse

#recurse into each subdirectory and try to build the provided targets
recurse:
	for subdir in $(SUBDIRS); do $(MAKE) -C $$subdir $(MAKECMDGOALS); done

$(GRAPHS):
	$(MAKE) -C $@
