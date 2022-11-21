#include <cmath>
#include "scriptTag.h"
#include "readStream.h"


int ScriptTag::parseData(std::ifstream &fs, uint32_t size) {

    uint8_t *buf = new uint8_t[size]; // NOLINT(modernize-use-auto)
    fs.read(reinterpret_cast<char *>(buf), size);
    ReadStream rs(buf, size);


    while (true) {
        uint32_t endMarker = rs.getMultiBit(24);
        if (endMarker == 9) {
            break;
        }
        uint8_t type = rs.readMultiBit(8);

        switch (type) {
            case string_marker: {
                parseString(rs, mark);
                break;
            }
            case ecma_array_marker: {
                /*根据AMF0存储数组的长度是u32*/
                parseArray(rs);
                break;
            }
            default: {
                fprintf(stderr, "AMF0 parse %d error \n", type);
                return -1;
            }

        }
    }


    delete[] buf;
    return 0;
}


int ScriptTag::parseArray(ReadStream &rs) {
    /*根据AMF0存储数组的长度是u32*/
    length = rs.readMultiBit(32);
    info = new item[length];
    for (int i = 0; i < length; ++i) {
        parseString(rs, info[i].name);
        uint8_t itemType = rs.readMultiBit(8);
        if (itemType == marker::number_marker) {
            parseNumber(rs, info[i].u.n);
            info[i].type = marker::number_marker;
        } else if (itemType == marker::boolean_marker) {
            info[i].u.b = rs.readMultiBit(8);
            info[i].type = marker::boolean_marker;
        } else if (itemType == marker::string_marker) {
            parseString(rs, info[i].u.s);
            info[i].type = marker::string_marker;
        } else {
            fprintf(stderr, "arr 不支持解析maker = %d\n", itemType);
            return -1;
        }
    }
    return 0;
}


int ScriptTag::parseString(ReadStream &rs, char *&str) {
    uint16_t length = rs.readMultiBit(16);
    str = new char[length + 1]();
    rs.getString(str, length);
    return 0;
}

int ScriptTag::parseNumber(ReadStream &rs, double &number) {

    /*符号*/
    int sign = rs.readBit();
    /*指数*/
    uint16_t exponent = rs.readMultiBit(11) - 1023;
    double m = 0.0;
    for (int j = 1; j <= 52; ++j) {
        bool bit = rs.readBit();
        if (bit) {
            m += pow(2, -j);
        }
    }
    m = m + 1;
    if (sign) {
        sign = -1;
    } else {
        sign = 1;
    }
    number = sign * m * pow(2, exponent);

    return 0;
}

void ScriptTag::printData() {
    if (!info) {
        return;
    }
    for (int i = 0; i < length; ++i) {
        if (info[i].type == number_marker) {
            printf("%s=%f\n", info[i].name, info[i].u.n);
        } else if (info[i].type == string_marker) {
            printf("%s=%s\n", info[i].name, info[i].u.s);
        } else if (info[i].type == boolean_marker) {
            printf("%s=%d\n", info[i].name, info[i].u.b);
        }

    }
}

ScriptTag::~ScriptTag() {
    if (mark) {
        delete[]mark;
        mark = nullptr;
    }
    if (info) {
        for (int i = 0; i < length; ++i) {
            delete[] info[i].name;
            if (info[i].type == string_marker) {
                delete[] info[i].u.s;
            }
        }
        delete[] info;
    }

}




