

#include "scriptTag.h"

#include <cstdio>
#include <cstring>

#include "mux.h"
#include "writeStream.h"

constexpr int LIST_LENGTH = 17;

/*
videocodecid
10(mp4a) AAC
7(avc1) H.264
4 On2 VP6
2 Sorenson H.263
 */

/*
audiocodecid
10(mp4a) AAC
11 speex
2 MP3
 ...
 */
static Item list[LIST_LENGTH] = {
        {"duration",          {false, 0,               ""},                              number_marker},
        {"width",             {false, 0,               ""},                              number_marker},
        {"height",            {false, 0,               ""},                              number_marker},
        {"videodatarate",     {false, 0,               ""},                              number_marker},
        {"framerate",         {false, 0,               ""},                              number_marker},
        {"videocodecid",      {false, 7.0,             ""},                              number_marker},
        {"audiodatarate",     {false, 0,               ""},                              number_marker},
        {"audiosamplerate",   {false, 0,               ""},                              number_marker},
        {"audiosamplesize",   {false, 0,               ""},                              number_marker},
        {"stereo",            {true,  0.0,             ""},                              boolean_marker},
        {"audiocodecid",      {false, 10.0,            ""},                              number_marker},
        {"major_brand",       {false, 0.0,             "isom"},                          string_marker},
        {"minor_version",     {false, 0.0,             "512"},                           string_marker},
        {"compatible_brands", {false, 0.0,             "isomiso2avc1mp41"},              string_marker},
        {"comment",           {false, 0.0,             "Content Adaptive Encoding 3.0"}, string_marker},
        {"encoder",           {false, 0.0,             "Lavf58.45.100"},                 string_marker},
        {"filesize",          {false, 21355370.000000, ""},                              number_marker},
};

int ScriptTag::write(std::ofstream &fs) {
    uint8_t buffer[413];

    //uint8_t *buffer = new uint8_t[413]; // NOLINT(modernize-use-auto)
    WriteStream ws(buffer, 413);


    ws.writeMultiBit(8, string_marker);
    const char *msg = "onMetaData";
    ws.writeMultiBit(16, strlen(msg));
    ws.setString(msg, strlen(msg));


    ws.writeMultiBit(8, ecma_array_marker);
    /*数组的长度*/
    ws.writeMultiBit(32, LIST_LENGTH);

    for (auto &i: list) {
        /*写入字符串大小*/
        ws.writeMultiBit(16, strlen(i.name));
        /*写入字符串*/
        ws.setString(i.name, strlen(i.name));
        ws.writeMultiBit(8, i.type);
        if (i.type == number_marker) {
            ws.writeMultiBit(64, *(uint64_t *) (&i.u.n));
        } else if (i.type == string_marker) {
            ws.writeMultiBit(16, strlen(i.u.s));
            ws.setString(i.u.s, strlen(i.u.s));
        } else if (i.type == boolean_marker) {
            ws.writeMultiBit(8, i.u.b);
        } else {
            fprintf(stderr, "AMF 数组写入不对\n");
            return -1;
        }
    }
    /*输入结束符*/
    ws.writeMultiBit(24, 9);
    fs.write(reinterpret_cast<char * >(buffer), 413);
    return 0;
}

uint64_t ScriptTag::reverse(uint8_t *arr, int n) {
    int temp = 0;
    for (int i = 0; i < n / 2; ++i) {
        temp = arr[n - i - 1];
        arr[n - i - 1] = arr[i];
        arr[i] = temp;
    }


    return *((uint64_t *) arr);
}

int ScriptTag::setScriptInfo(ScriptInfo &info) {
    list[0].u.n = std::max(info.videoDuration, info.audioDuration);
    list[1].u.n = info.width;
    list[2].u.n = info.height;
    list[3].u.n = info.videodatarate;
    list[4].u.n = info.framerate;
    list[6].u.n = info.audiodatarate;
    list[7].u.n = info.audiosamplerate;
    list[8].u.n = info.audiosamplesize;
    list[9].u.b = info.stereo;
    list[16].u.n = info.filesize;

    return 0;
}
