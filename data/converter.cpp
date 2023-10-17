#include "../utilities/command_line.h"
#include "../utilities/graph.hpp"
#include "../utilities/builder.hpp"
#include "../utilities/writer.hpp"
#include "../include/OpenSparseBLAS.h"

int main(int argc, char *argv[])
{
    CLConvert cli(argc, argv, "converter : This tool can convert .mtx format to .sg format, which can be loaded at a much higher speed than .mtx");

    if (!cli.ParseArgs())
        return -1;

    if (cli.needs_weights())
    {
        typedef struct NodeWeight<SpB_Index, int32_t> DestID_T;

        Builder<SpB_Index, DestID_T, int32_t> builder(&cli, true);

        CSRGraph<SpB_Index, DestID_T> g = builder.MakeGraph();

        Writer<SpB_Index, DestID_T> w(g);

        w.WriteGraph(cli.out_filename());
    }
    else
    {
        Builder<SpB_Index, SpB_Index, SpB_Index> builder(&cli, false);

        CSRGraph<SpB_Index, SpB_Index> g = builder.MakeGraph();

        Writer<SpB_Index, SpB_Index> w(g);

        w.WriteGraph(cli.out_filename());
    }

    return 0;
}