#include "Map.hpp"

#include <iostream>
#include <string>
//#include <stdexcept>
//#include <utility>
//#include <random>
//#include <chrono>
#include <iterator>
#include <cassert>

void count_words() {
    cs540::Map<std::string, int> words_count;
    
    // just a big list of words
    auto words = {"this", "is", "a", "string", "with", "words", "some",
        "words", "in", "the", "string", "repeat", "the", "more", "they",
        "repeat", "the", "more", "they", "should", "count", "they", "more",
        "they", "count", "the", "more", "they", "will", "have", "their",
        "count", "increased"};

    for (auto& word : words) {
        // this works because int can be default constructed, and the 
        // default of int (by doing int{} ) is 0.
        words_count[word] += 1; // add 1 to the count
    }

    // print the frequency of each word
    std::cout << "word frequency:\n";
    for (auto& count : words_count) { // uses .begin() and .end()
        std::cout << count.first << ": " << count.second << '\n';
    }

    std::cout << "word frequency reversed order:\n";
    for (auto riter = words_count.rbegin();
            riter != words_count.rend();
            ++riter) {
        std::cout << (*riter).first << ": " << (*riter).second << '\n';
    }
    
}

int main () {
    count_words();

    return 0;
}
