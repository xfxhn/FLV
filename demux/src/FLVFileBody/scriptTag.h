
#ifndef FLV_SCRIPTTAG_H
#define FLV_SCRIPTTAG_H

#include <fstream>
#include <cstdint>


#include <map>

class ReadStream;

enum marker {
    number_marker,
    boolean_marker,
    string_marker,
    object_marker,
    movieclip_marker,
    null_marker,
    undefined_marker,
    reference_marker,
    ecma_array_marker,
    object_end_marker,
    strict_array_marker,
    date_marker,
    long_string_marker,
    unsupported_marker,
    recordset_marker,
    xml_document_marker,
    typed_object_marker
};

struct item {
    char *name{nullptr};
    union {
        bool b;
        double n;
        char *s;
    } u{};
    marker type{null_marker};
};


class ScriptTag {
public:
    int parseData(std::ifstream &fs, uint32_t size);

    void printData();

    ~ScriptTag();

private:
    int parseArray(ReadStream &rs);

    static int parseString(ReadStream &rs, char *&str);

    static int parseNumber(ReadStream &rs, double &number);

private:
    char *mark{nullptr};
    item *info{nullptr};
    uint32_t length{0};
};


#endif //FLV_SCRIPTTAG_H
