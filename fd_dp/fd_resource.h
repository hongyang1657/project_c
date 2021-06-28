#ifndef FD_RESOURCE_H
#define FD_RESOURCE_H

#include <stdlib.h>
#include <stdio.h>
#include <map>
#include <vector>
#include <string>
#include <cstring>
#include <string.h>
#include "fd_config.h"


#define EMBEDDING_PATH          "/data/found/model/embedding.dat"
#define VOCAB_EMBEDDING_PATH    "/data/found/model/vocab_embedding.dat"
#define SLOT_EMBEDDING_PATH     "/data/found/model/slot_embedding.dat"
#define INTENT_EMBEDDING_PATH   "/data/found/model/intent_embedding.dat"
// #define CHAR_2_INDEX_PATH       "/data/found/model/char2index.txt"
#define CHAR_2_INDEX_PATH       "/data/found/model/char2index.json"
#define INDEX_2_CHAR_PATH       "/data/found/model/index2char.json"
#define INDEX_2_INTENT_PATH     "/data/found/model/index2intent.json"

#define INDEX_2_SLOT_PATH       "/data/found/model/index2slot.json"
#define SLOT_2_INDEX_PATH       "/data/found/model/slot2index.json"
#define INTENT_SCORE_PATH       "/data/found/model/intent_score.json"
#define INTENT_2_TYPE_PATH      "/data/found/model/intent2type.json"

#define TTS_URL_PATH            "/data/found/data/tts/ttsConfig.json"

#define MODEL_CONFIG_PATH      "/data/found/model/model_config.json"

#define EMBEDDING_ROW 12000
#define EMBEDDING_COL 300

#define VOCAB_EMBEDDING_ROW 12000
#define VOCABE_EMBEDDING_COL 300

#define SLOT_EMBEDDING_ROW 1000
#define SLOT_EMBEDDING_COL 50

#define INTENT_EMBEDDING_ROW 1000
#define INTENT_EMBEDDING_COL 50


class FdResource
{
private:
    FdResource();
    ~FdResource();
    FdConfig *config = FdConfig::_GetInstance();

    int set_embedding_for_file(std::string path);

    int set_vocab_embedding_for_file(std::string path);

    int set_slot_embedding_for_file(std::string path);

    int set_intent_embedding_for_file(std::string path);

    int set_index_intent_for_file(std::string path);

    int set_char_index_for_file(std::string path);

    int set_intent_score_dic_for_file(std::string path);
    
    int set_index_slot_dic_for_file(std::string path);
    
    int set_slot_index_dic_for_file(std::string path);

    int set_intent_type_dic_for_file(std::string path);

    int set_tts_url_for_file(std::string path);

    int set_model_config_for_file(std::string path);

public:
    static FdResource *_GetInstance()
    {
        static FdResource instance;
        return &instance;
    }
    
    float embeddings[EMBEDDING_ROW][EMBEDDING_COL];
    float *getEmbeddingWithIndex(int index);

    float vocab_embedding[VOCAB_EMBEDDING_ROW][VOCABE_EMBEDDING_COL];
    float *getVocabEmbeddingWithIndex(int index);

    float intent_embedding[INTENT_EMBEDDING_ROW][INTENT_EMBEDDING_COL];
    float *getIntentEmbeddingWithIndex(int index);

    float slot_embedding[SLOT_EMBEDDING_ROW][SLOT_EMBEDDING_COL];
    float *getSlotEmbeddingWithIndex(int index);

    //工具字典，通过set_dictionary设置，无初始化
    std::map<std::string, int> char_index_dic;
    int char2index(std::string char_str);

    std::map<int, std::string>index_char_dic;
    std::string index2char(int index);


    std::map<int, std::string> index_intent_dic;
    std::string getIntentWithIndex(int index);

    std::map<int, float> intent_score_dic;
    float getScoreWithIntent(int intent);

    std::map<int, float> intent_ambiguous_score_dic;
    float getAmbiguousScoreWithIntent(int intent);

    std::map<int, std::string> index_slot_dic;
    std::string getSlotWithIntent(int intent);

    std::map<std::string, int> slot_index_dic;
    int getIntentIndexWithSlot(std::string slot);

    std::map<std::string, std::string> intent_type_dic;
    std::string getTypeWithIntent(std::string intent);

    std::map<std::string, std::vector<std::vector<std::string> > > tts_url_dic;
    std::map<std::string, std::string> tts_mode_dic;
    std::vector<std::string> getTtsUrlsWithIntentIndex(std::string intent_index); 

};



#endif //FD_RESOURCE_H