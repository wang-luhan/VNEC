#pragma once
#include <string>
#include "graph.hpp"
#include "../include/OpenSparseBLAS.h"

template <typename NodeID_, typename DestID_ = NodeID_>
class Writer
{
public:
    explicit Writer(CSRGraph<NodeID_, DestID_> &g) : g_(g) {}

    void WriteSerializedGraph(std::fstream &out)
    {
        if (!std::is_same<NodeID_, SpB_Index>::value)
        {
            std::cout << "The NodeID_ is only allowed to be SpB_Index in serialized graphs!" << std::endl;
            std::exit(-4);
        }
        if (!std::is_same<DestID_, NodeID_>::value &&
            !std::is_same<DestID_, NodeWeight<NodeID_, int32_t>>::value)
        {
            std::cout << ".wsg only allowed for int32_t weights" << std::endl;
            std::exit(-8);
        }
        bool directed = g_.directed();
        uint64_t num_nodes = g_.num_nodes();
        uint64_t edges_to_write = g_.num_edges();
        // SGOffset edges_to_write = g_.num_edges_directed();
        std::streamsize index_bytes = (num_nodes + 1) * sizeof(SpB_Index);
        std::streamsize neigh_bytes;
        if (std::is_same<DestID_, NodeID_>::value)
            neigh_bytes = edges_to_write * sizeof(SpB_Index);
        else
            neigh_bytes = edges_to_write * sizeof(NodeWeight<SpB_Index, int32_t>);
        out.write(reinterpret_cast<char *>(&directed), sizeof(bool));
        out.write(reinterpret_cast<char *>(&edges_to_write), sizeof(uint64_t));
        out.write(reinterpret_cast<char *>(&num_nodes), sizeof(uint64_t));
        SpB_Index * offsets = g_.out_offsets();
        out.write(reinterpret_cast<char *>(offsets), index_bytes);
        DestID_ * destValues = g_.out_destValues();
        out.write(reinterpret_cast<char *>(destValues), neigh_bytes);
        if (directed)
        {
            offsets = g_.in_offsets();
            out.write(reinterpret_cast<char *>(offsets), index_bytes);
            destValues = g_.in_destValues();
            out.write(reinterpret_cast<char *>(destValues), neigh_bytes);
        }
    }

    void WriteGraph(std::string filename)
    {
        if (filename == "")
        {
            std::cout << "No output filename given (Use -h for help)" << std::endl;
            std::exit(-8);
        }
        std::fstream file(filename, std::ios::out | std::ios::binary);
        if (!file)
        {
            std::cout << "Couldn't write to file " << filename << std::endl;
            std::exit(-5);
        }

        WriteSerializedGraph(file);

        file.close();
    }

private:
    CSRGraph<NodeID_, DestID_> &g_;
    std::string filename_;
};