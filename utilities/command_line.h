// Copyright (c) 2015, The Regents of the University of California (Regents)
// See LICENSE.txt for license details
#pragma once
#include <getopt.h>
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

/*
Handles command line argument parsing
*/

class CL
{
protected:
  int argc_;
  char **argv_;
  std::string brief_;
  std::string get_args_ = "f:hsw";
  std::vector<std::string> help_strings_;

  std::string filename_ = "";
  bool symmetrize_ = false;
  bool needs_weights_ = false;

  void AddHelpLine(char opt, std::string opt_arg, std::string text,
                   std::string def = "")
  {
    const int kBufLen = 100;
    char buf[kBufLen];
    if (opt_arg != "")
      opt_arg = "<" + opt_arg + ">";
    if (def != "")
      def = "[" + def + "]";
    snprintf(buf, kBufLen, " -%c %-9s: %-54s%10s", opt, opt_arg.c_str(),
             text.c_str(), def.c_str());
    help_strings_.push_back(buf);
  }

public:
  CL(int argc, char **argv, std::string brief = "") : argc_(argc), argv_(argv), brief_(brief)
  {
    AddHelpLine('h', "", "print this help message");
    AddHelpLine('f', "file", "load graph from file");
    AddHelpLine('s', "", "symmetrize input edge list", "false");
    AddHelpLine('w', "", "generate a weightd map from input", "false");
  }

  bool ParseArgs()
  {
    signed char c_opt;
    extern char *optarg; // from and for getopt
    while ((c_opt = getopt(argc_, argv_, get_args_.c_str())) != -1)
    {
      HandleArg(c_opt, optarg);
    }
    if (filename_ == "")
    {
      std::cout << "No graph input file specified. (Use -h for help)" << std::endl;
      return false;
    }

    return true;
  }

  void virtual HandleArg(signed char opt, char *opt_arg)
  {
    switch (opt)
    {
    case 'f':
      filename_ = std::string(opt_arg);
      break;
    case 'h':
      PrintUsage();
      break;
    case 's':
      symmetrize_ = true;
      break;
    case 'w':
      needs_weights_ = true;
      break;
    }
  }

  void PrintUsage()
  {
    std::cout << brief_ << std::endl;
    std::sort(help_strings_.begin(), help_strings_.end());
    for (std::string h : help_strings_)
      std::cout << h << std::endl;
    std::exit(0);
  }

  std::string filename() const { return filename_; }
  bool symmetrize() const { return symmetrize_; }
  bool needs_weights() const { return needs_weights_; }
};

class CLConvert : public CL
{
  std::string out_filename_ = "";

public:
  CLConvert(int argc, char **argv, std::string name)
      : CL(argc, argv, name)
  {
    get_args_ += "o:";
    AddHelpLine('o', "file", "output serialized graph to file (.sg/.wsg)");
  }

  std::string GetSuffix(const std::string name)
  {
    std::size_t suff_pos = name.rfind('.');
    if (suff_pos == std::string::npos)
    {
      std::cout << "Could't find suffix of " << name << std::endl;
      std::exit(-1);
    }
    std::string rt(name.substr(suff_pos));
    return rt;
  }

  void check_output()
  {
    std::string out_filename_suffix = GetSuffix(out_filename_);
    if (!(out_filename_suffix == ".sg" || out_filename_suffix == ".wsg"))
    {
      std::cerr << "Unsupported output file suffixes : " << out_filename_suffix << std::endl;
      exit(-1);
    }

    if (needs_weights_ && out_filename_suffix == ".sg")
    {
      std::cerr << "The output file with .sg suffix contradicts the generation of the weighted graph." << std::endl;
      exit(-1);
    }

    if (!needs_weights_ && out_filename_suffix == ".wsg")
    {
      std::cerr << "The output file with .wsg suffix contradicts the generation of the unweighted graph." << std::endl;
      exit(-1);
    }
  }

  void HandleArg(signed char opt, char *opt_arg) override
  {
    switch (opt)
    {
    case 'o':
      out_filename_ = std::string(opt_arg);
      check_output();
      break;
    default:
      CL::HandleArg(opt, opt_arg);
    }
  }

  std::string out_filename() const { return out_filename_; }
};
