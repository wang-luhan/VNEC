std::string getFullPath_sg(const std::string path,
                           const std::string file);

std::string getFullPath_wsg(const std::string path,
                            const std::string file);

std::string getFullPath_mtx(const std::string path,
                            const std::string file);

void run_gtest(const std::string full_path,
               void (*func)(const std::string));

void run_gtest(const std::string full_path,
               void *args,
               void (*func)(const std::string, void *));

void run_gtest(const std::string full_path,
               const std::string result_path,
               void (*func)(const std::string, const std::string));

void run_gtest(const std::string full_path,
               const std::string result_path,
               void *args,
               void (*func)(const std::string, const std::string, void *));
