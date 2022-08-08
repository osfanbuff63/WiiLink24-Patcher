#include <stdio.h>
#include <curl/curl.h>

struct MemoryStruct {
  char *memory;
  size_t size;
};

CURLcode curl_download(const char *url, FILE *fp);
CURLcode curl_download_to_memory(const char *url, MemoryStruct *chunk);
