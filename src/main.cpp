#include "bpe.hpp"
#include "tokenizer_assets.hpp"
#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

static long long count_total_tokens(const vector<vector<int>>& batch_ids)
{
    long long total_tokens = 0;

    for (int i = 0; i < (int)batch_ids.size(); i++)
    {
        total_tokens += (long long)batch_ids[i].size();
    }

    return total_tokens;
}

int main()
{
    TokenizerAssets assets;
    load_vocab(assets, "data/vocab.json");
    load_merges(assets, "data/merges.txt");

    vector<string> texts = read_lines_from_file("data/wikitext_10k.txt");

    cout << "Loaded documents: " << texts.size() << endl;

    vector<string> modes = {"no_cache", "cache"};
    vector<int> thread_counts = {1, 2, 4, 8};

    ofstream fout("results/phase4_cache_benchmark.csv");
    fout << "mode,threads,trial,elapsed_sec,total_docs,total_tokens,docs_per_sec,tokens_per_sec,cache_hits,cache_misses,cache_hit_rate\n";

    for (int m = 0; m < (int)modes.size(); m++)
    {
        string mode = modes[m];

        cout << "\n==============================" << endl;
        cout << "Mode = " << mode << endl;
        cout << "==============================" << endl;

        for (int t = 0; t < (int)thread_counts.size(); t++)
        {
            int num_threads = thread_counts[t];
            vector<double> elapsed_trials;

            cout << "\nThreads = " << num_threads << endl;

            reset_cache_stats();

            vector<vector<int>> warmup_ids;
            if (mode == "no_cache")
            {
                warmup_ids = encode_batch_thread_pool_docs_no_cache(texts, assets, num_threads);
            }
            else
            {
                warmup_ids = encode_batch_thread_pool_docs_cache(texts, assets, num_threads);
            }

            long long warmup_tokens = count_total_tokens(warmup_ids);

            for (int trial = 1; trial <= 3; trial++)
            {
                reset_cache_stats();

                auto start = chrono::high_resolution_clock::now();

                vector<vector<int>> batch_ids;
                if (mode == "no_cache")
                {
                    batch_ids = encode_batch_thread_pool_docs_no_cache(texts, assets, num_threads);
                }
                else
                {
                    batch_ids = encode_batch_thread_pool_docs_cache(texts, assets, num_threads);
                }

                auto end = chrono::high_resolution_clock::now();

                chrono::duration<double> elapsed = end - start;
                double elapsed_sec = elapsed.count();

                long long total_docs = (long long)batch_ids.size();
                long long total_tokens = count_total_tokens(batch_ids);

                double docs_per_sec = (double)total_docs / elapsed_sec;
                double tokens_per_sec = (double)total_tokens / elapsed_sec;

                long long cache_hits = get_cache_hits();
                long long cache_misses = get_cache_misses();
                double cache_hit_rate = 0.0;

                if (cache_hits + cache_misses > 0)
                {
                    cache_hit_rate = (double)cache_hits / (double)(cache_hits + cache_misses);
                }

                elapsed_trials.push_back(elapsed_sec);

                cout << "Trial " << trial
                     << " | elapsed_sec = " << fixed << setprecision(6) << elapsed_sec
                     << " | total_docs = " << total_docs
                     << " | total_tokens = " << total_tokens
                     << " | docs_per_sec = " << docs_per_sec
                     << " | tokens_per_sec = " << tokens_per_sec
                     << " | cache_hits = " << cache_hits
                     << " | cache_misses = " << cache_misses
                     << " | cache_hit_rate = " << cache_hit_rate
                     << endl;

                fout << mode << ","
                     << num_threads << ","
                     << trial << ","
                     << fixed << setprecision(6) << elapsed_sec << ","
                     << total_docs << ","
                     << total_tokens << ","
                     << docs_per_sec << ","
                     << tokens_per_sec << ","
                     << cache_hits << ","
                     << cache_misses << ","
                     << cache_hit_rate << "\n";
            }

            sort(elapsed_trials.begin(), elapsed_trials.end());
            double median_elapsed = elapsed_trials[elapsed_trials.size() / 2];

            double median_docs_per_sec = (double)texts.size() / median_elapsed;
            double median_tokens_per_sec = (double)warmup_tokens / median_elapsed;

            cout << "Median"
                 << " | elapsed_sec = " << fixed << setprecision(6) << median_elapsed
                 << " | docs_per_sec = " << median_docs_per_sec
                 << " | tokens_per_sec = " << median_tokens_per_sec
                 << endl;
        }
    }

    fout.close();

    return 0;
}