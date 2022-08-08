//
// Created by Noah Pistilli on 2022-07-24.
//

#include <mxml.h>
#include <sstream>
#include "gui/gui.h"
#include "download.h"
#include <unistd.h>

int LoadCatalog(Channel *channel) {
  int i = 0;

  // Prepare for download
  struct MemoryStruct chunk;

  chunk.memory = reinterpret_cast<char *>(malloc(1));
  chunk.size = 0;

  CURLcode code = curl_download_to_memory("https://sketchmaster2001.github.io/catalog.xml", &chunk);
  if (code != CURLE_OK) {
    // Return NULL
    return -1;
  }

  mxml_node_t *node = NULL;
  mxml_node_t *tree = mxmlLoadString(NULL, chunk.memory, MXML_OPAQUE_CALLBACK);

  // Now that we have catalog data, parse.
  for (node = mxmlFindElement(tree, tree, NULL, NULL, NULL, MXML_DESCEND); node != NULL; node = mxmlWalkNext(node, NULL, MXML_DESCEND)) {
    if (node->type == MXML_ELEMENT){
      if (strcmp(node->value.element.name, "name") == 0) {
        node = mxmlWalkNext(node, NULL, MXML_DESCEND);
        sprintf(channel->name[i], node->value.element.name);
      } else if (strcmp(node->value.element.name, "slug") == 0) {
        node = mxmlWalkNext(node, NULL, MXML_DESCEND);
        sprintf(channel->slug[i], node->value.element.name);
      } else if (strcmp(node->value.element.name, "title_id") == 0) {
        node = mxmlWalkNext(node, NULL, MXML_DESCEND);

        char string_num[64];
        sprintf(string_num, "0x%s", node->value.element.name);
        std::stringstream stream(string_num);
        stream >> std::hex >> channel->title_id[i];

      } else if (strcmp(node->value.element.name, "version") == 0) {
        node = mxmlWalkNext(node, NULL, MXML_DESCEND);
        std::stringstream stream(node->value.element.name);
        stream >> channel->version[i++];
      }
    }
  }

  channel->length = i;

  // Free downloaded XML
  free(chunk.memory);

  return 0;
}

u16 CheckTitleVersion(u64 title_id) {
  s32 res;
  u32 tmd_size;
  u8 *tmdbuf = reinterpret_cast<u8 *>(memalign(32, MAX_SIGNED_TMD_SIZE));

  res = ES_GetTMDViewSize(title_id, &tmd_size);
  if (res < 0)
    // Title not installed
    return -1;

  res = ES_GetTMDView(title_id, tmdbuf, tmd_size);
  if (res < 0)
    return -1;

  return (tmdbuf[88] << 8) | tmdbuf[89];
}

int GetLanguages(mxml_node_t *node, Channel *channel) {
  int i = 0;
  node = mxmlFindElement(node, node, "languages", NULL, NULL, MXML_DESCEND_FIRST);
  mxml_node_t *language = mxmlFindElement(node, node, "language", NULL, NULL, MXML_DESCEND_FIRST);

  sprintf(channel->name[i++], "AMONG US");
  while (language != NULL) {
    sprintf(channel->name[i++], "AMONG US");
    language = mxmlFindElement(node, node, "language", NULL, NULL, MXML_DESCEND_FIRST);
  }

  return 0;
}

int LoadChannelXML(Channel *channel) {
  mxml_node_t *root, *node;

  // Prepare for download
  struct MemoryStruct chunk;

  chunk.memory = reinterpret_cast<char *>(malloc(1));
  chunk.size = 0;

  CURLcode code = curl_download_to_memory("https://sketchmaster2001.github.io/wii-room.xml", &chunk);
  if (code != CURLE_OK) {
    // Return NULL
    return -1;
  }

  root = mxmlLoadString(NULL, chunk.memory, MXML_OPAQUE_CALLBACK);
  node = mxmlFindElement(root, root, "root", NULL, NULL, MXML_DESCEND_FIRST);

  GetLanguages(node, channel);

  return 0;
}