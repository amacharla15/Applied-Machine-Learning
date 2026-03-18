#include "tokenizer_assets.hpp"

int main()
{
    TokenizerAssets assets;

    load_vocab(assets, "data/vocab.json");
    load_merges(assets, "data/merges.txt");
    print_summary(assets);

    return 0;
}