#include "download.h"
#include <curl/curl.h>
#include <cstring>
#include <cstdlib>
#include <stdio.h>

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;

  char *ptr = reinterpret_cast<char *>(realloc(mem->memory, mem->size + realsize + 1));
  if(!ptr) {
    /* out of memory! */
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }

  mem->memory = ptr;
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;

  return realsize;
}

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    size_t written = fwrite(ptr, size, nmemb, stream);
    return written;
}

// As rewriting the download commands for curl can get quite lengthy, here is a function.
CURLcode curl_download(const char *url, FILE *fp) {
    CURL *curl;
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    CURLcode res;

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            curl_easy_cleanup(curl);
            curl_global_cleanup();
            return res;
        }
    }

    curl_easy_cleanup(curl);
    curl_global_cleanup();

    return res;
}

CURLcode curl_download_to_memory(const char *url, MemoryStruct *chunk) {
  CURL *curl;
  curl_global_init(CURL_GLOBAL_DEFAULT);
  curl = curl_easy_init();
  CURLcode res;

  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)chunk);

    res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
      curl_easy_cleanup(curl);
      curl_global_cleanup();
      return res;
    }
  }

  curl_easy_cleanup(curl);
  curl_global_cleanup();

  return res;
}