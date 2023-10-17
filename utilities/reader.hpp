#pragma once
#include "graph.hpp"
#include "pvector.h"
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "timer.h"
#include <tuple>
#include "../include/OpenSparseBLAS.h"

template <typename NodeID_, typename DestID_ = NodeID_,
          typename WeightT_ = NodeID_, bool invert = true>
class Reader
{
    typedef EdgePair<NodeID_, DestID_> Edge;
    typedef pvector<Edge> EdgeList;
    std::string filename_;

    bool read_weights = false;
    bool undirected;
    uint64_t m_, n_, nonzeros_;

public:
    explicit Reader(std::string filename) : filename_(filename) {}
    std::string GetSuffix()
    {
        std::size_t suff_pos = filename_.rfind('.');
        if (suff_pos == std::string::npos)
        {
            std::cout << "Could't find suffix of " << filename_ << std::endl;
            std::exit(-1);
        }
        return filename_.substr(suff_pos);
    }

    void ReadInBasicFromMTX(std::ifstream &in, const bool needs_weights)
    {
        std::string start, object, format, field, symmetry;
        in >> start >> object >> format >> field >> symmetry >> std::ws;
        if (start != "%%MatrixMarket" && start != "%MatrixMarket")
        {
            std::cout << ".mtx file did not start with %%MatrixMarket or %MatrixMarket" << std::endl;
            std::exit(-21);
        }
        if ((object != "matrix") || (format != "coordinate"))
        {
            std::cout << "only allow matrix coordinate format for .mtx" << std::endl;
            std::exit(-22);
        }
        if (field == "complex")
        {
            std::cout << "do not support complex weights for .mtx" << std::endl;
            std::exit(-23);
        }
        // bool read_weights;
        if (field == "pattern")
        {
            read_weights = false;
            if (needs_weights)
            {
                std::cerr << "The input matrix is not an wighted graph!" << std::endl;
                exit(-1);
            }
        }
        else if ((field == "real") || (field == "double") ||
                 (field == "integer"))
        {
            if (needs_weights)
            {
                read_weights = true;
            }
            else
            {
                read_weights = false;
            }
        }
        else
        {
            std::cout << "unrecognized field type for .mtx" << std::endl;
            std::exit(-24);
        }
        if (symmetry == "symmetric")
        {
            undirected = true;
        }
        else if ((symmetry == "general") || (symmetry == "skew-symmetric"))
        {
            undirected = false;
        }
        else
        {
            std::cout << "unsupported symmetry type for .mtx" << std::endl;
            std::exit(-25);
        }
        while (true)
        {
            char c = in.peek();
            if (c == '%')
            {
                in.ignore(200, '\n');
            }
            else
            {
                break;
            }
        }
        
        in >> m_ >> n_ >> nonzeros_ >> std::ws;
        if(undirected)
        {
            nonzeros_ = nonzeros_ * 2;
        }
        if (m_ != n_)
        {
            std::cout << m_ << " " << n_ << " " << nonzeros_ << std::endl;
            std::cout << "matrix must be square for .mtx" << std::endl;
            std::exit(-26);
        }
    }

    // Note: converts vertex numbering from 1..N to 0..N-1
    // Note: weights casted to type WeightT_
    EdgeList ReadInELfromMTX(std::ifstream &in)
    {
        EdgeList el;
        std::string line;
        while (std::getline(in, line))
        {
            if (line.empty())
                continue;
            std::istringstream edge_stream(line);
            NodeID_ u;
            edge_stream >> u;
            if (read_weights)
            {
                NodeWeight<NodeID_, WeightT_> v;
                edge_stream >> v;
                v.v -= 1;
                el.push_back(Edge(u - 1, v));
                if (undirected)
                    el.push_back(Edge(v.v, NodeWeight<NodeID_, WeightT_>(u - 1, v.w)));
            }
            else
            {
                NodeID_ v;
                edge_stream >> v;
                el.push_back(Edge(u - 1, v - 1));
                if (undirected)
                    el.push_back(Edge(v - 1, u - 1));
            }
        }
        return el;
    }

    EdgeList ReadFile(bool &needs_weights, uint64_t &m, uint64_t &n, uint64_t &nonzeros)
    {
        Timer t;
        t.Start();
        EdgeList el;
        std::string suffix = GetSuffix();
        std::ifstream file(filename_);
        if (!file.is_open())
        {
            std::cout << "Couldn't open file " << filename_ << std::endl;
            std::exit(-2);
        }

        if (suffix == ".mtx")
        {
            ReadInBasicFromMTX(file, needs_weights);
            el = ReadInELfromMTX(file);
        }
        else
        {
            std::cout << "Unrecognized suffix: " << suffix << std::endl;
            std::exit(-3);
        }
        file.close();
        t.Stop();
        PrintTime("Read Time", t.Seconds());
        
        m = m_;
        n = n_;
        nonzeros = nonzeros_;
        return el;
    }

    CSRGraph<NodeID_, DestID_, invert> ReadSerializedGraph() {
    bool weighted = GetSuffix() == ".wsg";
    if (!std::is_same<NodeID_, SpB_Index>::value) {
      std::cout << "The NodeID_ is only allowed to be SpB_Index in serialized graphs!" << std::endl;
      std::exit(-5);
    }
    if (!weighted && !std::is_same<NodeID_, DestID_>::value) {
      std::cout << ".sg not allowed for weighted graphs" << std::endl;
      std::exit(-5);
    }
    if (weighted && std::is_same<NodeID_, DestID_>::value) {
      std::cout << ".wsg only allowed for weighted graphs" << std::endl;
      std::exit(-5);
    }
    if (weighted && !std::is_same<WeightT_, int32_t>::value) {
      std::cout << ".wsg only allowed for int32_t weights" << std::endl;
      std::exit(-5);
    }
    std::ifstream file(filename_);
    if (!file.is_open()) {
      std::cout << "Couldn't open file " << filename_ << std::endl;
      std::exit(-6);
    }
    Timer t;
    t.Start();
    bool directed;
    uint64_t num_nodes, num_edges;
    NodeID_ * out_offsets = nullptr, * in_offsets= nullptr;
    DestID_ *out_destValues = nullptr, *in_destValues = nullptr;
    file.read(reinterpret_cast<char*>(&directed), sizeof(bool));
    file.read(reinterpret_cast<char*>(&num_edges), sizeof(uint64_t));
    file.read(reinterpret_cast<char*>(&num_nodes), sizeof(uint64_t));
    out_offsets = new SpB_Index[num_nodes+1];
    out_destValues = new DestID_[num_edges];
    std::streamsize num_index_bytes = (num_nodes+1) * sizeof(SpB_Index);
    std::streamsize num_neigh_bytes = num_edges * sizeof(DestID_);
    file.read(reinterpret_cast<char*>(out_offsets), num_index_bytes);
    file.read(reinterpret_cast<char*>(out_destValues), num_neigh_bytes);
    if (directed && invert) {
            in_offsets = new SpB_Index[num_nodes+1];
    in_destValues = new DestID_[num_edges];
      file.read(reinterpret_cast<char*>(in_offsets), num_index_bytes);
      file.read(reinterpret_cast<char*>(in_destValues), num_neigh_bytes);
    }
    file.close();
    t.Stop();
    PrintTime("Read Time", t.Seconds());
    if (directed)
      return CSRGraph<NodeID_, DestID_, invert>(num_nodes, num_edges, out_offsets, out_destValues, in_offsets, in_destValues);
    else
      return CSRGraph<NodeID_, DestID_, invert>(num_nodes, num_edges, out_offsets, out_destValues);
  }
};
