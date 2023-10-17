#pragma once
#include "graph.hpp"
#include "pvector.h"
#include <type_traits>
#include <iostream>
#include <tuple>
#include "timer.h"
#include "util.h"
#include "reader.hpp"
#include "platform_atomics.h"

template <typename NodeID_ = uint64_t,
          typename DestID_ = NodeID_,
          typename WeightT_ = NodeID_,
          bool invert = true>
class Builder
{
  typedef EdgePair<NodeID_, DestID_> Edge;
  typedef pvector<Edge> EdgeList;

  const CL *cli_ = nullptr;
  std::string filename_;
  bool symmetrize_ = false;
  bool needs_weights_;
  NodeID_ num_nodes_ = 0;
  NodeID_ num_edges_ = 0;

  pvector<NodeID_> degrees;
  pvector<NodeID_> offsets;
  pvector<NodeID_> indegrees;
  pvector<NodeID_> inoffsets;

private:
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

public:
  explicit Builder(const CL *cli, bool needs_weights) : cli_(cli), needs_weights_(needs_weights)
  {
    filename_ = cli->filename();
    symmetrize_ = cli->symmetrize();
    if (needs_weights_ && (!std::is_same<DestID_, NodeWeight<NodeID_, WeightT_>>::value))
    {
      std::cerr << "The types of index and value do not match the type of weight node in the graph builder with weights." << std::endl;
      exit(-1);
    }

    if (!needs_weights_ && (!std::is_same<NodeID_, DestID_>::value))
    {
      std::cerr << "The NodeID_ and DestID_ data types do not match in the graph builder without weights." << std::endl;
      exit(-1);
    }
  }

  explicit Builder(const std::string filename) : filename_(filename)
  {
    std::string suffix = GetSuffix();
    if (suffix == ".sg")
    {
      needs_weights_ = false;
    }
    else if (suffix == ".wsg")
    {
      needs_weights_ = true;
    }
    else
    {
      std::cerr << "Unsupported file name suffix : " << suffix << std::endl;
      exit(-1);
    }
  }

  explicit Builder(const std::string filename, bool needs_weights) : filename_(filename), needs_weights_(needs_weights)
  {
    std::string suffix = GetSuffix();
    if(!(suffix == ".mtx"))
    {
      std::cerr << "Unsupported input file. It should be a MatrixMarket format file. " << suffix << std::endl;
      exit(-1);
    }

    if (needs_weights_ && (!std::is_same<DestID_, NodeWeight<NodeID_, WeightT_>>::value))
    {
      std::cerr << "The types of index and value do not match the type of weight node in the graph builder with weights." << std::endl;
      exit(-1);
    }

    if (!needs_weights_ && (!std::is_same<NodeID_, DestID_>::value))
    {
      std::cerr << "The NodeID_ and DestID_ data types do not match in the graph builder without weights." << std::endl;
      exit(-1);
    }
  }

  pvector<NodeID_> CountDegrees(const EdgeList &el, bool transpose)
  {
    pvector<NodeID_> degrees(num_nodes_, 0);
#pragma omp parallel for
    for (auto it = el.begin(); it < el.end(); it++)
    {
      Edge e = *it;
      if (symmetrize_ || (!symmetrize_ && !transpose))
        fetch_and_add(degrees[e.u], 1);
      if ((symmetrize_) || (!symmetrize_ && transpose))
        fetch_and_add(degrees[(NodeID_)e.v], 1);
    }
    return degrees;
  }

  static pvector<NodeID_> ParallelPrefixSum(const pvector<NodeID_> &degrees)
  {
    const size_t block_size = 1 << 20;
    const size_t num_blocks = (degrees.size() + block_size - 1) / block_size;
    pvector<NodeID_> local_sums(num_blocks);
#pragma omp parallel for
    for (size_t block = 0; block < num_blocks; block++)
    {
      NodeID_ lsum = 0;
      size_t block_end = std::min((block + 1) * block_size, degrees.size());
      for (size_t i = block * block_size; i < block_end; i++)
        lsum += degrees[i];
      local_sums[block] = lsum;
    }
    pvector<NodeID_> bulk_prefix(num_blocks + 1);
    NodeID_ total = 0;
    for (size_t block = 0; block < num_blocks; block++)
    {
      bulk_prefix[block] = total;
      total += local_sums[block];
    }
    bulk_prefix[num_blocks] = total;
    pvector<NodeID_> prefix(degrees.size() + 1);
#pragma omp parallel for
    for (size_t block = 0; block < num_blocks; block++)
    {
      NodeID_ local_total = bulk_prefix[block];
      size_t block_end = std::min((block + 1) * block_size, degrees.size());
      for (size_t i = block * block_size; i < block_end; i++)
      {
        prefix[i] = local_total;
        local_total += degrees[i];
      }
    }
    prefix[degrees.size()] = bulk_prefix[num_blocks];
    return prefix;
  }

  /*
  In-Place Graph Building Steps
    - sort edges and squish (remove self loops and redundant edges)
    - overwrite EdgeList's memory with outgoing neighbors
    - if graph not being symmetrized
      - finalize structures and make incoming structures if requested
    - if being symmetrized
      - search for needed inverses, make room for them, add them in place
  */
  void MakeCSRInPlace(EdgeList &el, DestID_ **neighs, DestID_ **inv_neighs)
  {
    // preprocess EdgeList - sort & squish in place
    std::sort(el.begin(), el.end());
    auto new_end = std::unique(el.begin(), el.end());
    el.resize(new_end - el.begin());
    auto self_loop = [](Edge e)
    { return e.u == e.v; };
    new_end = std::remove_if(el.begin(), el.end(), self_loop);
    num_edges_ = new_end - el.begin();
    el.resize(new_end - el.begin());
    // analyze EdgeList and repurpose it for outgoing edges
    degrees = CountDegrees(el, false);
    offsets = ParallelPrefixSum(degrees);
    indegrees = CountDegrees(el, true);
    *neighs = reinterpret_cast<DestID_ *>(el.data());
    for (Edge e : el)
      (*neighs)[offsets[e.u]++] = e.v;
    size_t num_edges = el.size();
    el.leak();
    // revert offsets by shifting them down
    for (NodeID_ n = num_nodes_ + 1; n > 0; n--)
    {
      offsets[n - 1] = n - 1 != 0 ? offsets[n - 2] : 0;
    }
    if (!symmetrize_)
    { // not going to symmetrize so no need to add edges
      size_t new_size = num_edges * sizeof(DestID_);
      *neighs = static_cast<DestID_ *>(std::realloc(*neighs, new_size));
      if (invert)
      { // create inv_neighs & inv_index for incoming edges
        inoffsets = ParallelPrefixSum(indegrees);
        *inv_neighs = new DestID_[inoffsets[num_nodes_]];
        NodeID_ * I = new NodeID_[num_nodes_];
        for (NodeID_ i = 0; i < num_nodes_ ;i++)
        {
          I[i] = 0;
        }
        for (NodeID_ u = 0; u < num_nodes_; u++)
        {
          for (DestID_ *it = (*neighs) + offsets[u]; it < (*neighs) + offsets[u + 1]; it++)
          {
            NodeID_ v = static_cast<NodeID_>(*it);
            (*inv_neighs)[inoffsets[v] + I[v]] = u;
            I[v]++;
          }
        }
        delete[] I;
      }
    }
    else
    { // symmetrize graph by adding missing inverse edges
      // Step 1 - count number of needed inverses
      pvector<NodeID_> invs_needed(num_nodes_, 0);
      for (NodeID_ u = 0; u < num_nodes_; u++)
      {
        for (NodeID_ i = offsets[u]; i < offsets[u + 1]; i++)
        {
          DestID_ v = (*neighs)[i];
          bool inv_found = std::binary_search(*neighs + offsets[v],
                                              *neighs + offsets[v + 1],
                                              static_cast<DestID_>(u));
          if (!inv_found)
            invs_needed[v]++;
        }
      }
      // increase offsets to account for missing inverses, realloc neighs
      NodeID_ total_missing_inv = 0;
      for (NodeID_ n = 0; n <= num_nodes_; n++)
      {
        offsets[n] += total_missing_inv;
        total_missing_inv += invs_needed[n];
      }
      size_t newsize = (offsets[num_nodes_] * sizeof(DestID_));
      *neighs = static_cast<DestID_ *>(std::realloc(*neighs, newsize));
      if (*neighs == nullptr)
      {
        std::cout << "Call to realloc() failed" << std::endl;
        exit(-33);
      }
      // Step 2 - spread out existing neighs to make room for inverses
      //   copies backwards (overwrites) and inserts free space at starts
      NodeID_ tail_index = offsets[num_nodes_] - 1;
      for (NodeID_ n = num_nodes_ - 1; n >= 0; n--)
      {
        NodeID_ new_start = offsets[n] + invs_needed[n];
        for (NodeID_ i = offsets[n + 1] - 1; i >= new_start; i--)
        {
          (*neighs)[tail_index] = (*neighs)[i - total_missing_inv];
          tail_index--;
        }
        total_missing_inv -= invs_needed[n];
        tail_index -= invs_needed[n];
      }
      // Step 3 - add missing inverse edges into free spaces from Step 2
      for (NodeID_ u = 0; u < num_nodes_; u++)
      {
        for (NodeID_ i = offsets[u] + invs_needed[u]; i < offsets[u + 1]; i++)
        {
          DestID_ v = (*neighs)[i];
          bool inv_found = std::binary_search(
              *neighs + offsets[v] + invs_needed[v],
              *neighs + offsets[v + 1],
              static_cast<DestID_>(u));
          if (!inv_found)
          {
            (*neighs)[offsets[v] + invs_needed[v] - 1] = static_cast<DestID_>(u);
            invs_needed[v]--;
          }
        }
      }
      for (NodeID_ n = 0; n < num_nodes_; n++)
        std::sort(*neighs + offsets[n], *neighs + offsets[n + 1]);
    }
  }

  NodeID_ FindMaxNodeID(const EdgeList &el)
  {
    NodeID_ max_seen = 0;
#pragma omp parallel for reduction(max \
                                   : max_seen)
    for (auto it = el.begin(); it < el.end(); it++)
    {
      Edge e = *it;
      max_seen = std::max(max_seen, e.u);
      max_seen = std::max(max_seen, (NodeID_)e.v);
    }
    return max_seen;
  }

  CSRGraph<NodeID_, DestID_, invert> MakeGraphFromEL(EdgeList &el)
  {
    // DestID_ **index = nullptr, **inv_index = nullptr;
    DestID_ *neighs = nullptr, *inv_neighs = nullptr;
    Timer t;
    t.Start();
    if (num_nodes_ < FindMaxNodeID(el) + 1)
    {
      std::cerr << "There is an error in the input file content for an out-of-range Node ID!" << std::endl;
      exit(-1);
    }

    MakeCSRInPlace(el, &neighs, &inv_neighs);

    t.Stop();
    PrintTime("Build Time", t.Seconds());
    if (symmetrize_)
      return CSRGraph<NodeID_, DestID_, invert>(num_nodes_, num_edges_, offsets.data(), neighs);
    else
    {
      return CSRGraph<NodeID_, DestID_, invert>(num_nodes_, num_edges_, offsets.data(), neighs, inoffsets.data(), inv_neighs);
    }
  }

  CSRGraph<NodeID_, DestID_, invert> MakeGraph()
  {
    if (filename_ != "")
    {
      Reader<NodeID_, DestID_, WeightT_, invert> r(filename_);
      auto suffix = r.GetSuffix();
      if ((suffix == ".sg") || (suffix == ".wsg"))
      {
        // TODO Read the .sg file.
        return r.ReadSerializedGraph();
      }
      else
      {
        uint64_t m, n, nonzeros;
        auto el = r.ReadFile(needs_weights_, m, n, nonzeros);
        num_nodes_ = m;
        num_edges_ = nonzeros;
        return MakeGraphFromEL(el);
      }
    }
    else
    {
      std::cerr << "The address of the input matrix does not exist and the corresponding sparse graph cannot be built!" << std::endl;
      exit(-1);
    }
  }
};