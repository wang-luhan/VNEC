#ifdef __cplusplus
extern "C"{
#endif
void SpB_Error(char *errorMsg, char *filename, const int line);
#ifdef __cplusplus
}
#endif

#define CHECK_ERROR(condition, msg)           \
    if (condition)                           \
    {                                        \
        SpB_Error(msg, __FILE__, __LINE__); \
    }
