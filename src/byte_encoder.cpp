#include "byte_encoder.hpp"
#include <array>

using namespace std;

static string codepoint_to_utf8(int codepoint)
{
    string out;

    if (codepoint <= 127)
    {
        out.push_back((char)codepoint);
    }
    else if (codepoint <= 2047)
    {
        out.push_back((char)(192 | (codepoint >> 6)));
        out.push_back((char)(128 | (codepoint & 63)));
    }
    else if (codepoint <= 65535)
    {
        out.push_back((char)(224 | (codepoint >> 12)));
        out.push_back((char)(128 | ((codepoint >> 6) & 63)));
        out.push_back((char)(128 | (codepoint & 63)));
    }
    else
    {
        out.push_back((char)(240 | (codepoint >> 18)));
        out.push_back((char)(128 | ((codepoint >> 12) & 63)));
        out.push_back((char)(128 | ((codepoint >> 6) & 63)));
        out.push_back((char)(128 | (codepoint & 63)));
    }

    return out;
}

vector<int> text_to_bytes(const string& text)
{
    vector<int> bytes;

    for (int i = 0; i < (int)text.size(); i++)
    {
        unsigned char b = (unsigned char)text[i];
        bytes.push_back((int)b);
    }

    return bytes;
}

unordered_map<int, string> bytes_to_unicode_map()
{
    vector<int> original_bytes;
    vector<int> mapped_codepoints;
    array<bool, 256> present{};
    present.fill(false);

    for (int b = (int)'!'; b <= (int)'~'; b++)
    {
        original_bytes.push_back(b);
        mapped_codepoints.push_back(b);
        present[b] = true;
    }

    for (int b = 161; b <= 172; b++)
    {
        original_bytes.push_back(b);
        mapped_codepoints.push_back(b);
        present[b] = true;
    }

    for (int b = 174; b <= 255; b++)
    {
        original_bytes.push_back(b);
        mapped_codepoints.push_back(b);
        present[b] = true;
    }

    int extra_index = 0;

    for (int b = 0; b < 256; b++)
    {
        if (!present[b])
        {
            original_bytes.push_back(b);
            mapped_codepoints.push_back(256 + extra_index);
            present[b] = true;
            extra_index++;
        }
    }

    unordered_map<int, string> byte_encoder;

    for (int i = 0; i < (int)original_bytes.size(); i++)
    {
        byte_encoder[original_bytes[i]] = codepoint_to_utf8(mapped_codepoints[i]);
    }

    return byte_encoder;
}

vector<string> bytes_to_symbols(const vector<int>& bytes)
{
    unordered_map<int, string> byte_encoder = bytes_to_unicode_map();
    vector<string> symbols;

    for (int i = 0; i < (int)bytes.size(); i++)
    {
        symbols.push_back(byte_encoder.at(bytes[i]));
    }

    return symbols;
}